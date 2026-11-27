/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsCpuuseValReport
 */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <inttypes.h>
#include <pthread.h>
#include <rtems.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <rtems/bspIo.h>
#include <rtems/cpuuse.h>
#include <rtems/cpuuseimpl.h>
#include <rtems/printer.h>
#include <sys/time.h>
#include <rtems/dev/io.h>
#include <rtems/score/thread.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsCpuuseValReport spec:/rtems/cpuuse/val/report
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests the CPU usage report.
 *
 * This test case performs the following actions:
 *
 * - Freeze the time. Reset the CPU usage. Advance the time by one tick of the
 *   software timecounter. Build the expected report from the identifiers, the
 *   names and the CPU-time clocks of all threads.
 *
 *   - Call rtems_cpu_usage_report_with_plugin() with a printer which captures
 *     the output. Check that the output is the expected report.
 *
 *   - Call rtems_cpu_usage_report(). Check that it prints the output of
 *     rtems_cpu_usage_report_with_plugin() through the printk printer.
 *
 *   - Restore the timecount handler. Reset the CPU usage.
 *
 * - Freeze the time and reset the CPU usage. Let each read of the timecounter
 *   advance the time by one millisecond.
 *
 *   - Call rtems_cpu_usage_report_with_plugin() with a printer which captures
 *     the output. Check that the PERCENT field of the calling thread is less
 *     than 100. The calling thread uses the processor since the reset point,
 *     so the PERCENT field is greater than 100 if the directive gets the time
 *     elapsed before the processor time of the thread.
 *
 *   - Restore the timecount handler. Reset the CPU usage.
 *
 * - Get the thread of the last thread line. Freeze the time and reset the CPU
 *   usage. Give the thread one second of processor time and move the CPU usage
 *   reset point two seconds back. Let each read of the timecounter advance the
 *   time by one millisecond.
 *
 *   - Call rtems_cpu_usage_report_with_plugin() with a printer which captures
 *     the output. Check that the PERCENT field of the last thread line is the
 *     SECONDS field of the line divided by the time of the total line. The
 *     times of the report have a resolution of one microsecond, so the check
 *     accepts a difference of one in the last decimal place.
 *
 *   - Restore the timecount handler. Reset the CPU usage.
 *
 * @{
 */

#define RULE_LINE \
  "-------------------------------------------------------------------------------\n"

#define COLUMN_RULE_LINE \
  "------------+----------------------------------------+---------------+---------\n"

#define SECONDS_FIELD_OFFSET ( 14 + 38 + 2 )

#define PERCENT_FIELD_OFFSET ( SECONDS_FIELD_OFFSET + 14 + 2 )

#define HEADER                                                           \
  RULE_LINE                                                              \
  "                              CPU USAGE BY THREAD\n" COLUMN_RULE_LINE \
  " ID         | NAME                                   | SECONDS       | PERCENT\n" COLUMN_RULE_LINE

#define MAX_THREADS 32

#define TOTAL_LINE_PREFIX " TIME SINCE LAST CPU USAGE RESET IN SECONDS:"

typedef struct {
  char   buffer[ 4096 ];
  size_t size;
} Output;

typedef struct {
  rtems_id ids[ MAX_THREADS ];
  size_t   count;
} Threads;

static Output output;

static Output expected;

static Output printk_output;

static BSP_output_char_function_type output_char;

static GetTimecountHandler previous_timecount_handler;

static uint32_t FreezeTime( void )
{
  return GetTimecountCounter() - 1;
}

static void ResetOutput( Output *out )
{
  memset( out->buffer, 0, sizeof( out->buffer ) );
  out->size = 0;
}

static void PutChar( Output *out, char c )
{
  if ( out->size < sizeof( out->buffer ) - 1 ) {
    out->buffer[ out->size ] = c;
    ++out->size;
  }
}

static void PutCharOfPrinter( int c, void *arg )
{
  PutChar( arg, (char) c );
}

static int Capture( void *arg, const char *format, va_list ap )
{
  return _IO_Vprintf( PutCharOfPrinter, arg, format, ap );
}

static int OutputChar( int c )
{
  /* The printk printer outputs a carriage return before each newline */
  if ( c != '\r' ) {
    PutChar( &printk_output, (char) c );
  }

  return 0;
}

static void Append( Output *out, const char *format, ... )
{
  va_list ap;

  va_start( ap, format );
  (void) _IO_Vprintf( PutCharOfPrinter, out, format, ap );
  va_end( ap );
  T_quiet_lt_sz( out->size, sizeof( out->buffer ) - 1 );
}

static void AppendTime( Output *out, const struct timespec *ts )
{
  Append(
    out,
    "%7" PRIu32 ".%06" PRIu32,
    (uint32_t) ts->tv_sec,
    (uint32_t) ( ts->tv_nsec / 1000 )
  );
}

static uint64_t ToNanoseconds( const struct timespec *ts )
{
  return (uint64_t) ts->tv_sec * 1000000000 + (uint64_t) ts->tv_nsec;
}

static bool CollectThread( rtems_tcb *tcb, void *arg )
{
  Threads *threads;

  threads = arg;
  T_quiet_lt_sz( threads->count, MAX_THREADS );

  if ( threads->count < MAX_THREADS ) {
    threads->ids[ threads->count ] = tcb->Object.id;
    ++threads->count;
  }

  return false;
}

static int CompareIds( const void *a, const void *b )
{
  rtems_id id_a;
  rtems_id id_b;

  id_a = *(const rtems_id *) a;
  id_b = *(const rtems_id *) b;

  if ( id_a < id_b ) {
    return -1;
  }

  if ( id_a > id_b ) {
    return 1;
  }

  return 0;
}

static void AppendThreadLine( rtems_id id, const struct timespec *elapsed )
{
  char            name[ 38 ];
  clockid_t       clock_id;
  struct timespec used;
  uint64_t        percent;
  uint64_t        elapsed_ns;
  int             status;

  (void) pthread_getname_np( (pthread_t) id, name, sizeof( name ) );
  status = pthread_getcpuclockid( (pthread_t) id, &clock_id );
  T_quiet_eq_int( status, 0 );
  status = clock_gettime( clock_id, &used );
  T_quiet_eq_int( status, 0 );

  elapsed_ns = ToNanoseconds( elapsed );

  if ( elapsed_ns == 0 ) {
    percent = 0;
  } else {
    percent = ( ToNanoseconds( &used ) * 100000 ) / elapsed_ns;
  }

  Append( &expected, " 0x%08" PRIx32 " | %-38s |", id, name );
  AppendTime( &expected, &used );
  Append(
    &expected,
    " |%4" PRIu32 ".%03" PRIu32 "\n",
    (uint32_t) ( percent / 1000 ),
    (uint32_t) ( percent % 1000 )
  );
}

static uint32_t AdvanceTime( void )
{
  uint32_t counter;

  counter = GetTimecountCounter() + SOFTWARE_TIMECOUNTER_FREQUENCY / 1000;
  (void) SetTimecountCounter( counter );

  return counter;
}

static uint64_t ParseDecimal( const char **s )
{
  uint64_t value;

  value = 0;

  while ( **s == ' ' ) {
    ++*s;
  }

  while ( **s >= '0' && **s <= '9' ) {
    value = value * 10 + (uint64_t) ( **s - '0' );
    ++*s;
  }

  return value;
}

static const char *FindThreadLine( rtems_id id )
{
  static Output prefix;
  const char   *line;

  ResetOutput( &prefix );
  Append( &prefix, " 0x%08" PRIx32 " | ", id );
  line = strstr( output.buffer, prefix.buffer );
  T_assert_not_null( line );

  return line;
}

static uint64_t ParseTime( const char **s )
{
  uint64_t ns;

  ns = ParseDecimal( s ) * 1000000000;
  T_assert_eq_char( **s, '.' );
  ++*s;
  ns += ParseDecimal( s ) * 1000;

  return ns;
}

static uint64_t ParsePercent( const char **s )
{
  uint64_t percent;

  percent = ParseDecimal( s ) * 1000;
  T_assert_eq_char( **s, '.' );
  ++*s;
  percent += ParseDecimal( s );

  return percent;
}

static void CheckPercentBelow100( rtems_id id )
{
  const char *line;

  line = FindThreadLine( id ) + PERCENT_FIELD_OFFSET;
  T_lt_u64( ParsePercent( &line ), 100000 );
}

static void CheckOneElapsedTime( rtems_id id )
{
  const char *line;
  const char *total;
  uint64_t    elapsed_ns;
  uint64_t    used_ns;
  uint64_t    expected_percent;
  uint64_t    actual;

  line = FindThreadLine( id ) + SECONDS_FIELD_OFFSET;
  used_ns = ParseTime( &line );
  T_assert_gt_u64( used_ns, 0 );
  line += 2;
  actual = ParsePercent( &line );

  total = strstr( output.buffer, TOTAL_LINE_PREFIX );
  T_assert_not_null( total );
  total += sizeof( TOTAL_LINE_PREFIX ) - 1;
  elapsed_ns = ParseTime( &total );
  T_assert_gt_u64( elapsed_ns, 0 );

  expected_percent = ( used_ns * 100000 ) / elapsed_ns;
  T_le_u64( actual, expected_percent + 1 );
  T_ge_u64( actual + 1, expected_percent );
}

static void BuildExpectedReport( void )
{
  Threads         threads;
  struct timespec elapsed;
  size_t          i;
  int             status;

  status = clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &elapsed );
  T_quiet_eq_int( status, 0 );
  threads.count = 0;
  rtems_task_iterate( CollectThread, &threads );
  qsort( threads.ids, threads.count, sizeof( threads.ids[ 0 ] ), CompareIds );

  ResetOutput( &expected );
  Append( &expected, "%s", HEADER );

  for ( i = 0; i < threads.count; ++i ) {
    AppendThreadLine( threads.ids[ i ], &elapsed );
  }

  Append(
    &expected,
    "%s TIME SINCE LAST CPU USAGE RESET IN SECONDS:                    ",
    COLUMN_RULE_LINE
  );
  AppendTime( &expected, &elapsed );
  Append( &expected, "\n%s", RULE_LINE );
}

/**
 * @brief Freeze the time. Reset the CPU usage. Advance the time by one tick of
 *   the software timecounter. Build the expected report from the identifiers,
 *   the names and the CPU-time clocks of all threads.
 */
static void RtemsCpuuseValReport_Action_0( void )
{
  rtems_printer printer;

  previous_timecount_handler = SetGetTimecountHandler( FreezeTime );
  rtems_cpu_usage_reset();
  TimecounterTick();
  BuildExpectedReport();

  /*
   * Call rtems_cpu_usage_report_with_plugin() with a printer which captures
   * the output. Check that the output is the expected report.
   */
  ResetOutput( &output );
  printer.context = &output;
  printer.printer = Capture;
  rtems_cpu_usage_report_with_plugin( &printer );
  T_eq_str( output.buffer, expected.buffer );

  /*
   * Call rtems_cpu_usage_report(). Check that it prints the output of
   * rtems_cpu_usage_report_with_plugin() through the printk printer.
   */
  ResetOutput( &printk_output );
  output_char = BSP_output_char;
  BSP_output_char = OutputChar;
  rtems_cpu_usage_report();
  BSP_output_char = output_char;
  T_eq_str( printk_output.buffer, expected.buffer );

  /*
   * Restore the timecount handler. Reset the CPU usage.
   */
  SetGetTimecountHandler( previous_timecount_handler );
  rtems_cpu_usage_reset();
}

/**
 * @brief Freeze the time and reset the CPU usage. Let each read of the
 *   timecounter advance the time by one millisecond.
 */
static void RtemsCpuuseValReport_Action_1( void )
{
  rtems_printer printer;

  previous_timecount_handler = SetGetTimecountHandler( FreezeTime );
  rtems_cpu_usage_reset();
  (void) SetGetTimecountHandler( AdvanceTime );

  /*
   * Call rtems_cpu_usage_report_with_plugin() with a printer which captures
   * the output. Check that the PERCENT field of the calling thread is less
   * than 100. The calling thread uses the processor since the reset point, so
   * the PERCENT field is greater than 100 if the directive gets the time
   * elapsed before the processor time of the thread.
   */
  ResetOutput( &output );
  printer.context = &output;
  printer.printer = Capture;
  rtems_cpu_usage_report_with_plugin( &printer );
  (void) SetGetTimecountHandler( FreezeTime );
  CheckPercentBelow100( rtems_task_self() );

  /*
   * Restore the timecount handler. Reset the CPU usage.
   */
  SetGetTimecountHandler( previous_timecount_handler );
  rtems_cpu_usage_reset();
}

/**
 * @brief Get the thread of the last thread line. Freeze the time and reset the
 *   CPU usage. Give the thread one second of processor time and move the CPU
 *   usage reset point two seconds back. Let each read of the timecounter
 *   advance the time by one millisecond.
 */
static void RtemsCpuuseValReport_Action_2( void )
{
  rtems_printer   printer;
  Thread_Control *thread;
  Threads         threads;
  rtems_id        last;

  threads.count = 0;
  rtems_task_iterate( CollectThread, &threads );
  T_assert_gt_sz( threads.count, 0 );
  last = threads.ids[ threads.count - 1 ];

  previous_timecount_handler = SetGetTimecountHandler( FreezeTime );
  rtems_cpu_usage_reset();
  thread = GetThread( last );
  thread->cpu_time_used_at_last_reset -= SBT_1S;
  CPU_usage_Uptime_at_last_reset -= 2 * SBT_1S;
  (void) SetGetTimecountHandler( AdvanceTime );

  /*
   * Call rtems_cpu_usage_report_with_plugin() with a printer which captures
   * the output. Check that the PERCENT field of the last thread line is the
   * SECONDS field of the line divided by the time of the total line. The times
   * of the report have a resolution of one microsecond, so the check accepts a
   * difference of one in the last decimal place.
   */
  ResetOutput( &output );
  printer.context = &output;
  printer.printer = Capture;
  rtems_cpu_usage_report_with_plugin( &printer );
  (void) SetGetTimecountHandler( FreezeTime );
  CheckOneElapsedTime( last );

  /*
   * Restore the timecount handler. Reset the CPU usage.
   */
  SetGetTimecountHandler( previous_timecount_handler );
  rtems_cpu_usage_reset();
}

/**
 * @fn void T_case_body_RtemsCpuuseValReport( void )
 */
T_TEST_CASE( RtemsCpuuseValReport )
{
  RtemsCpuuseValReport_Action_0();
  RtemsCpuuseValReport_Action_1();
  RtemsCpuuseValReport_Action_2();
}

/** @} */
