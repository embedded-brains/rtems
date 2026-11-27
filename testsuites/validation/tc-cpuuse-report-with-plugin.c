/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsCpuuseReqReportWithPlugin
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
#include <string.h>
#include <time.h>
#include <rtems/cpuuse.h>
#include <rtems/cpuuseimpl.h>
#include <rtems/printer.h>
#include <rtems/dev/io.h>
#include <rtems/score/threadimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsCpuuseReqReportWithPlugin \
 *   spec:/rtems/cpuuse/req/report-with-plugin
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsCpuuseReqReportWithPlugin_Pre_Name_Fits,
  RtemsCpuuseReqReportWithPlugin_Pre_Name_Long,
  RtemsCpuuseReqReportWithPlugin_Pre_Name_NA
} RtemsCpuuseReqReportWithPlugin_Pre_Name;

typedef enum {
  RtemsCpuuseReqReportWithPlugin_Pre_Elapsed_Zero,
  RtemsCpuuseReqReportWithPlugin_Pre_Elapsed_NonZero,
  RtemsCpuuseReqReportWithPlugin_Pre_Elapsed_NA
} RtemsCpuuseReqReportWithPlugin_Pre_Elapsed;

typedef enum {
  RtemsCpuuseReqReportWithPlugin_Pre_Used_Zero,
  RtemsCpuuseReqReportWithPlugin_Pre_Used_Part,
  RtemsCpuuseReqReportWithPlugin_Pre_Used_All,
  RtemsCpuuseReqReportWithPlugin_Pre_Used_NA
} RtemsCpuuseReqReportWithPlugin_Pre_Used;

typedef enum {
  RtemsCpuuseReqReportWithPlugin_Post_Name_Complete,
  RtemsCpuuseReqReportWithPlugin_Post_Name_Truncated,
  RtemsCpuuseReqReportWithPlugin_Post_Name_NA
} RtemsCpuuseReqReportWithPlugin_Post_Name;

typedef enum {
  RtemsCpuuseReqReportWithPlugin_Post_Seconds_Used,
  RtemsCpuuseReqReportWithPlugin_Post_Seconds_NA
} RtemsCpuuseReqReportWithPlugin_Post_Seconds;

typedef enum {
  RtemsCpuuseReqReportWithPlugin_Post_Percent_Zero,
  RtemsCpuuseReqReportWithPlugin_Post_Percent_Ratio,
  RtemsCpuuseReqReportWithPlugin_Post_Percent_NA
} RtemsCpuuseReqReportWithPlugin_Post_Percent;

typedef enum {
  RtemsCpuuseReqReportWithPlugin_Post_Total_Elapsed,
  RtemsCpuuseReqReportWithPlugin_Post_Total_NA
} RtemsCpuuseReqReportWithPlugin_Post_Total;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Name_NA : 1;
  uint16_t Pre_Elapsed_NA : 1;
  uint16_t Pre_Used_NA : 1;
  uint16_t Post_Name : 2;
  uint16_t Post_Seconds : 1;
  uint16_t Post_Percent : 2;
  uint16_t Post_Total : 1;
} RtemsCpuuseReqReportWithPlugin_Entry;

/**
 * @brief Test context for spec:/rtems/cpuuse/req/report-with-plugin test case.
 */
typedef struct {
  /**
   * @brief This member contains the identifier of the thread of the thread
   *   line.
   */
  rtems_id id;

  /**
   * @brief This member references the TCB of the thread of the thread line.
   */
  Thread_Control *thread;

  /**
   * @brief This member contains the CPU-time clock of the thread.
   */
  clockid_t clock_id;

  /**
   * @brief This member contains the previous timecount handler.
   */
  GetTimecountHandler previous_timecount_handler;

  /**
   * @brief This member references the expected name of the thread.
   */
  const char *name;

  /**
   * @brief This member specifies the time elapsed since the last CPU usage
   *   reset point.
   */
  Timestamp_Control elapsed;

  /**
   * @brief This member specifies the processor time which the thread used
   *   since the last CPU usage reset point.
   */
  Timestamp_Control used;

  /**
   * @brief This member contains the output of the action.
   */
  char output[ 4096 ];

  /**
   * @brief This member contains the count of characters in the output.
   */
  size_t output_size;

  /**
   * @brief This member references the thread line of the thread in the output.
   */
  const char *line;

  /**
   * @brief This member references the total line in the output.
   */
  const char *total;

  /**
   * @brief This member contains the processor time of the thread which
   *   clock_gettime() returns after the action.
   */
  struct timespec used_time;

  /**
   * @brief This member contains the time of the CLOCK_PROCESS_CPUTIME_ID clock
   *   which clock_gettime() returns after the action.
   */
  struct timespec elapsed_time;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 3 ];

    /**
     * @brief This member defines the representative index for the next action.
     */
    size_t rep;

    /**
     * @brief If this member is true, then the current run of the
     *   representatives should be skipped.
     */
    bool skip_rep;

    /**
     * @brief This member records the representatives which ran.
     */
    bool rep_ran[ 13 ];

    /**
     * @brief If this member is true, then the test action loop is executed.
     */
    bool in_action_loop;

    /**
     * @brief This member contains the next transition map index.
     */
    size_t index;

    /**
     * @brief This member contains the current transition map entry.
     */
    RtemsCpuuseReqReportWithPlugin_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsCpuuseReqReportWithPlugin_Context;

static RtemsCpuuseReqReportWithPlugin_Context
  RtemsCpuuseReqReportWithPlugin_Instance;

static const char *const RtemsCpuuseReqReportWithPlugin_PreDesc_Name[] =
  { "FitsObject", "FitsMax", "LongMin", "LongMax", "NA" };

static const char *const RtemsCpuuseReqReportWithPlugin_PreDesc_Elapsed[] =
  { "Zero", "NonZeroSeconds", "NonZeroHours", "NA" };

static const char *const RtemsCpuuseReqReportWithPlugin_PreDesc_Used[] =
  { "Zero", "Part", "All", "NA" };

static const char *const *const RtemsCpuuseReqReportWithPlugin_PreDesc[] = {
  RtemsCpuuseReqReportWithPlugin_PreDesc_Name,
  RtemsCpuuseReqReportWithPlugin_PreDesc_Elapsed,
  RtemsCpuuseReqReportWithPlugin_PreDesc_Used,
  NULL
};

typedef enum {
  RtemsCpuuseReqReportWithPlugin_Pre_Name_Fits_Object,
  RtemsCpuuseReqReportWithPlugin_Pre_Name_Fits_Max
} RtemsCpuuseReqReportWithPlugin_Pre_Name_Fits_Rep;

typedef enum {
  RtemsCpuuseReqReportWithPlugin_Pre_Name_Long_Min,
  RtemsCpuuseReqReportWithPlugin_Pre_Name_Long_Max
} RtemsCpuuseReqReportWithPlugin_Pre_Name_Long_Rep;

typedef enum {
  RtemsCpuuseReqReportWithPlugin_Pre_Elapsed_NonZero_Seconds,
  RtemsCpuuseReqReportWithPlugin_Pre_Elapsed_NonZero_Hours
} RtemsCpuuseReqReportWithPlugin_Pre_Elapsed_NonZero_Rep;

static const uint8_t RtemsCpuuseReqReportWithPlugin_PreRepOffset_Name[] =
  { 0, 2, 4, 5 };

static const uint8_t RtemsCpuuseReqReportWithPlugin_PreRepOffset_Elapsed[] =
  { 0, 1, 3, 4 };

static const uint8_t RtemsCpuuseReqReportWithPlugin_PreRepOffset_Used[] =
  { 0, 1, 2, 3, 4 };

static const uint8_t *const RtemsCpuuseReqReportWithPlugin_PreRepOffset[] = {
  RtemsCpuuseReqReportWithPlugin_PreRepOffset_Name,
  RtemsCpuuseReqReportWithPlugin_PreRepOffset_Elapsed,
  RtemsCpuuseReqReportWithPlugin_PreRepOffset_Used
};

static size_t RtemsCpuuseReqReportWithPlugin_GetRepresentativeCountOf(
  const RtemsCpuuseReqReportWithPlugin_Context *ctx,
  size_t                                        index
)
{
  const uint8_t *offsets;
  size_t         state;

  offsets = RtemsCpuuseReqReportWithPlugin_PreRepOffset[ index ];
  state = ctx->Map.pcs[ index ];

  return offsets[ state + 1 ] - offsets[ state ];
}

static size_t RtemsCpuuseReqReportWithPlugin_GetRepresentative(
  const RtemsCpuuseReqReportWithPlugin_Context *ctx,
  size_t                                        index
)
{
  size_t count;

  count = RtemsCpuuseReqReportWithPlugin_GetRepresentativeCountOf(
    ctx,
    index
  );

  if ( ctx->Map.rep < count ) {
    return ctx->Map.rep;
  }

  return count - 1;
}

static size_t RtemsCpuuseReqReportWithPlugin_GetRepresentativeCount(
  const RtemsCpuuseReqReportWithPlugin_Context *ctx
)
{
  size_t count;
  size_t i;

  count = 1;

  for ( i = 0; i < 3; ++i ) {
    size_t count_of_condition;

    count_of_condition =
      RtemsCpuuseReqReportWithPlugin_GetRepresentativeCountOf( ctx, i );

    if ( count_of_condition > count ) {
      count = count_of_condition;
    }
  }

  return count;
}

static RtemsCpuuseReqReportWithPlugin_Pre_Name_Fits_Rep
RtemsCpuuseReqReportWithPlugin_Pre_Name_Fits_GetRepresentative(
  const RtemsCpuuseReqReportWithPlugin_Context *ctx
)
{
  return (
    RtemsCpuuseReqReportWithPlugin_Pre_Name_Fits_Rep
  ) RtemsCpuuseReqReportWithPlugin_GetRepresentative( ctx, 0 );
}

static RtemsCpuuseReqReportWithPlugin_Pre_Name_Long_Rep
RtemsCpuuseReqReportWithPlugin_Pre_Name_Long_GetRepresentative(
  const RtemsCpuuseReqReportWithPlugin_Context *ctx
)
{
  return (
    RtemsCpuuseReqReportWithPlugin_Pre_Name_Long_Rep
  ) RtemsCpuuseReqReportWithPlugin_GetRepresentative( ctx, 0 );
}

static RtemsCpuuseReqReportWithPlugin_Pre_Elapsed_NonZero_Rep
RtemsCpuuseReqReportWithPlugin_Pre_Elapsed_NonZero_GetRepresentative(
  const RtemsCpuuseReqReportWithPlugin_Context *ctx
)
{
  return (
    RtemsCpuuseReqReportWithPlugin_Pre_Elapsed_NonZero_Rep
  ) RtemsCpuuseReqReportWithPlugin_GetRepresentative( ctx, 1 );
}

static const uint8_t RtemsCpuuseReqReportWithPlugin_PreRepBase[] = { 0, 5, 9 };

static const uint8_t RtemsCpuuseReqReportWithPlugin_PreRepStates[] =
  { 2, 2, 3 };

static void RtemsCpuuseReqReportWithPlugin_ResetRepresentatives(
  RtemsCpuuseReqReportWithPlugin_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 13; ++i ) {
    ctx->Map.rep_ran[ i ] = false;
  }
}

static void RtemsCpuuseReqReportWithPlugin_MarkRepresentatives(
  RtemsCpuuseReqReportWithPlugin_Context *ctx
)
{
  size_t i;

  if ( ctx->Map.skip_rep ) {
    return;
  }

  for ( i = 0; i < 3; ++i ) {
    size_t rep;

    rep = RtemsCpuuseReqReportWithPlugin_PreRepBase[ i ] +
          RtemsCpuuseReqReportWithPlugin_PreRepOffset[ i ]
                                                     [ ctx->Map.pcs[ i ] ] +
          RtemsCpuuseReqReportWithPlugin_GetRepresentative( ctx, i );
    ctx->Map.rep_ran[ rep ] = true;
  }
}

static void RtemsCpuuseReqReportWithPlugin_CheckRepresentatives(
  const RtemsCpuuseReqReportWithPlugin_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 3; ++i ) {
    const uint8_t *offsets;
    const bool    *ran;
    size_t         state;

    offsets = RtemsCpuuseReqReportWithPlugin_PreRepOffset[ i ];
    ran = &ctx->Map.rep_ran[ RtemsCpuuseReqReportWithPlugin_PreRepBase[ i ] ];

    for (
      state = 0; state < RtemsCpuuseReqReportWithPlugin_PreRepStates[ i ];
      ++state
    ) {
      size_t k;
      bool   any;

      any = false;

      for ( k = offsets[ state ]; k < offsets[ state + 1 ]; ++k ) {
        any = any || ran[ k ];
      }

      if ( any ) {
        for ( k = offsets[ state ]; k < offsets[ state + 1 ]; ++k ) {
          T_quiet_true(
            ran[ k ],
            "representative %s did not run",
            RtemsCpuuseReqReportWithPlugin_PreDesc[ i ][ k ]
          );
        }
      }
    }
  }
}

typedef RtemsCpuuseReqReportWithPlugin_Context Context;

#define NAME_FIELD_OFFSET 14

#define NAME_FIELD_SIZE 38

#define SECONDS_FIELD_OFFSET ( NAME_FIELD_OFFSET + NAME_FIELD_SIZE + 2 )

#define PERCENT_FIELD_OFFSET ( SECONDS_FIELD_OFFSET + 14 + 2 )

#define TOTAL_LINE_PREFIX " TIME SINCE LAST CPU USAGE RESET IN SECONDS:"

#define TOTAL_FIELD_OFFSET 64

#define NAME_37 "abcdefghijklmnopqrstuvwxyz0123456789A"

#define NAME_38 NAME_37 "B"

#define NAME_47 NAME_38 "CDEFGHIJK"

typedef struct {
  char  *buffer;
  size_t size;
  size_t index;
} Sink;

static uint32_t FreezeTime( void )
{
  return GetTimecountCounter() - 1;
}

static void PutChar( int c, void *arg )
{
  Sink *sink;

  sink = arg;

  if ( sink->index + 1 < sink->size ) {
    sink->buffer[ sink->index ] = (char) c;
    ++sink->index;
    sink->buffer[ sink->index ] = '\0';
  }
}

static void Format( char *buffer, size_t size, const char *format, ... )
{
  Sink    sink;
  va_list ap;

  sink.buffer = buffer;
  sink.size = size;
  sink.index = 0;
  buffer[ 0 ] = '\0';
  va_start( ap, format );
  (void) _IO_Vprintf( PutChar, &sink, format, ap );
  va_end( ap );
}

static void ResetOutput( Context *ctx )
{
  memset( ctx->output, 0, sizeof( ctx->output ) );
  ctx->output_size = 0;
}

static int Capture( void *arg, const char *format, va_list ap )
{
  Context *ctx;
  Sink     sink;
  int      n;

  ctx = arg;
  sink.buffer = ctx->output;
  sink.size = sizeof( ctx->output );
  sink.index = ctx->output_size;
  n = _IO_Vprintf( PutChar, &sink, format, ap );
  ctx->output_size = sink.index;
  T_quiet_lt_sz( ctx->output_size, sizeof( ctx->output ) - 1 );

  return n;
}

static const char *FindThreadLine( const Context *ctx )
{
  char prefix[ NAME_FIELD_OFFSET + 1 ];

  Format( prefix, sizeof( prefix ), " 0x%08" PRIx32 " | ", ctx->id );

  return strstr( ctx->output, prefix );
}

static void SetName( Context *ctx, const char *name, const char *expected )
{
  int status;

  status = pthread_setname_np( (pthread_t) ctx->id, name );
  T_quiet_eq_int( status, 0 );
  ctx->name = expected;
}

static void CheckName( const Context *ctx, size_t count )
{
  char expected[ NAME_FIELD_SIZE + 1 ];

  memset( expected, ' ', NAME_FIELD_SIZE );
  expected[ NAME_FIELD_SIZE ] = '\0';
  memcpy(
    expected,
    ctx->name,
    strlen( ctx->name ) < count ? strlen( ctx->name ) : count
  );
  T_eq_nstr( ctx->line + NAME_FIELD_OFFSET, expected, NAME_FIELD_SIZE );
  T_eq_nstr( ctx->line + NAME_FIELD_OFFSET + NAME_FIELD_SIZE, " |", 2 );
}

static void CheckTime( const char *field, const struct timespec *ts )
{
  char expected[ 15 ];

  Format(
    expected,
    sizeof( expected ),
    "%7" PRIu32 ".%06" PRIu32,
    (uint32_t) ts->tv_sec,
    (uint32_t) ( ts->tv_nsec / 1000 )
  );
  T_eq_nstr( field, expected, 14 );
}

static void CheckPercent( const Context *ctx )
{
  const struct timespec *used;
  const struct timespec *elapsed;
  uint64_t               percent;
  char                   expected[ 10 ];

  used = &ctx->used_time;
  elapsed = &ctx->elapsed_time;

  /*
   * The times of the large representatives are whole seconds.  The quotient
   * of the seconds is then the quotient of the nanoseconds.
   */
  if ( used->tv_nsec == 0 && elapsed->tv_nsec == 0 ) {
    percent = ( (uint64_t) used->tv_sec * 100000 ) /
              (uint64_t) elapsed->tv_sec;
  } else {
    uint64_t used_ns;
    uint64_t elapsed_ns;

    used_ns = (uint64_t) used->tv_sec * 1000000000 + (uint64_t) used->tv_nsec;
    elapsed_ns = (uint64_t) elapsed->tv_sec * 1000000000 +
                 (uint64_t) elapsed->tv_nsec;
    T_quiet_lt_u64( used_ns, UINT64_MAX / 100000 );
    percent = ( used_ns * 100000 ) / elapsed_ns;
  }

  Format(
    expected,
    sizeof( expected ),
    "%4" PRIu32 ".%03" PRIu32 "\n",
    (uint32_t) ( percent / 1000 ),
    (uint32_t) ( percent % 1000 )
  );
  T_eq_nstr( ctx->line + PERCENT_FIELD_OFFSET, expected, 9 );
}

static void RtemsCpuuseReqReportWithPlugin_Pre_Name_Prepare(
  RtemsCpuuseReqReportWithPlugin_Context *ctx,
  RtemsCpuuseReqReportWithPlugin_Pre_Name state
)
{
  switch ( state ) {
    case RtemsCpuuseReqReportWithPlugin_Pre_Name_Fits: {
      /*
       * While the name of a thread has at most 37 characters.
       */
      switch (
        RtemsCpuuseReqReportWithPlugin_Pre_Name_Fits_GetRepresentative( ctx )
      ) {
        case RtemsCpuuseReqReportWithPlugin_Pre_Name_Fits_Object: {
          SetName( ctx, "", "WORK" );
          break;
        }

        case RtemsCpuuseReqReportWithPlugin_Pre_Name_Fits_Max: {
          SetName( ctx, NAME_37, NAME_37 );
          break;
        }
      }
      break;
    }

    case RtemsCpuuseReqReportWithPlugin_Pre_Name_Long: {
      /*
       * While the name of a thread has more than 37 characters.
       */
      switch (
        RtemsCpuuseReqReportWithPlugin_Pre_Name_Long_GetRepresentative( ctx )
      ) {
        case RtemsCpuuseReqReportWithPlugin_Pre_Name_Long_Min: {
          SetName( ctx, NAME_38, NAME_38 );
          break;
        }

        case RtemsCpuuseReqReportWithPlugin_Pre_Name_Long_Max: {
          SetName( ctx, NAME_47, NAME_47 );
          break;
        }
      }
      break;
    }

    case RtemsCpuuseReqReportWithPlugin_Pre_Name_NA:
      break;
  }
}

static void RtemsCpuuseReqReportWithPlugin_Pre_Elapsed_Prepare(
  RtemsCpuuseReqReportWithPlugin_Context    *ctx,
  RtemsCpuuseReqReportWithPlugin_Pre_Elapsed state
)
{
  switch ( state ) {
    case RtemsCpuuseReqReportWithPlugin_Pre_Elapsed_Zero: {
      /*
       * While the time elapsed since the last CPU usage reset point is zero.
       */
      ctx->elapsed = 0;
      break;
    }

    case RtemsCpuuseReqReportWithPlugin_Pre_Elapsed_NonZero: {
      /*
       * While the time elapsed since the last CPU usage reset point is greater
       * than zero.
       */
      switch (
        RtemsCpuuseReqReportWithPlugin_Pre_Elapsed_NonZero_GetRepresentative(
          ctx
        )
      ) {
        case RtemsCpuuseReqReportWithPlugin_Pre_Elapsed_NonZero_Seconds: {
          ctx->elapsed = 2 * SBT_1S;
          break;
        }

        case RtemsCpuuseReqReportWithPlugin_Pre_Elapsed_NonZero_Hours: {
          ctx->elapsed = 200 * 3600 * SBT_1S;
          break;
        }
      }
      break;
    }

    case RtemsCpuuseReqReportWithPlugin_Pre_Elapsed_NA:
      break;
  }

  CPU_usage_Uptime_at_last_reset -= ctx->elapsed;
}

static void RtemsCpuuseReqReportWithPlugin_Pre_Used_Prepare(
  RtemsCpuuseReqReportWithPlugin_Context *ctx,
  RtemsCpuuseReqReportWithPlugin_Pre_Used state
)
{
  switch ( state ) {
    case RtemsCpuuseReqReportWithPlugin_Pre_Used_Zero: {
      /*
       * While the processor time which the thread used since the last CPU
       * usage reset point is zero.
       */
      ctx->used = 0;
      break;
    }

    case RtemsCpuuseReqReportWithPlugin_Pre_Used_Part: {
      /*
       * While the processor time which the thread used since the last CPU
       * usage reset point is greater than zero and less than the time elapsed
       * since the last CPU usage reset point.
       */
      ctx->used = ctx->elapsed / 2;
      break;
    }

    case RtemsCpuuseReqReportWithPlugin_Pre_Used_All: {
      /*
       * While the processor time which the thread used since the last CPU
       * usage reset point is equal to the time elapsed since the last CPU
       * usage reset point.
       */
      ctx->used = ctx->elapsed;
      break;
    }

    case RtemsCpuuseReqReportWithPlugin_Pre_Used_NA:
      break;
  }

  ctx->thread->cpu_time_used_at_last_reset -= ctx->used;
}

static void RtemsCpuuseReqReportWithPlugin_Post_Name_Check(
  RtemsCpuuseReqReportWithPlugin_Context  *ctx,
  RtemsCpuuseReqReportWithPlugin_Post_Name state
)
{
  switch ( state ) {
    case RtemsCpuuseReqReportWithPlugin_Post_Name_Complete: {
      /*
       * The NAME field of the thread line of the thread shall be the name of
       * the thread, padded on the right with space characters to 38
       * characters.
       */
      CheckName( ctx, NAME_FIELD_SIZE );
      break;
    }

    case RtemsCpuuseReqReportWithPlugin_Post_Name_Truncated: {
      /*
       * The NAME field of the thread line of the thread shall be the first 37
       * characters of the name of the thread, followed by one space character.
       */
      CheckName( ctx, NAME_FIELD_SIZE - 1 );
      break;
    }

    case RtemsCpuuseReqReportWithPlugin_Post_Name_NA:
      break;
  }
}

static void RtemsCpuuseReqReportWithPlugin_Post_Seconds_Check(
  RtemsCpuuseReqReportWithPlugin_Context     *ctx,
  RtemsCpuuseReqReportWithPlugin_Post_Seconds state
)
{
  switch ( state ) {
    case RtemsCpuuseReqReportWithPlugin_Post_Seconds_Used: {
      /*
       * The SECONDS field of the thread line of the thread shall be the
       * processor time which the thread used since the last CPU usage reset
       * point at some time point during the directive call, in the time format
       * of the report.
       */
      CheckTime( ctx->line + SECONDS_FIELD_OFFSET, &ctx->used_time );
      break;
    }

    case RtemsCpuuseReqReportWithPlugin_Post_Seconds_NA:
      break;
  }
}

static void RtemsCpuuseReqReportWithPlugin_Post_Percent_Check(
  RtemsCpuuseReqReportWithPlugin_Context     *ctx,
  RtemsCpuuseReqReportWithPlugin_Post_Percent state
)
{
  switch ( state ) {
    case RtemsCpuuseReqReportWithPlugin_Post_Percent_Zero: {
      /*
       * The PERCENT field of the thread line of the thread shall be `
       * 0.000`.
       */
      T_eq_nstr( ctx->line + PERCENT_FIELD_OFFSET, "   0.000\n", 9 );
      break;
    }

    case RtemsCpuuseReqReportWithPlugin_Post_Percent_Ratio: {
      /*
       * The PERCENT field of the thread line of the thread shall be the
       * processor time which the thread used since the last CPU usage reset
       * point multiplied by 100 and divided by the time elapsed since the last
       * CPU usage reset point, truncated to three decimal places, in the form
       * of the integer part in decimal digits padded on the left with space
       * characters to four characters, followed by `.`, followed by the three
       * decimal places.
       */
      CheckPercent( ctx );
      break;
    }

    case RtemsCpuuseReqReportWithPlugin_Post_Percent_NA:
      break;
  }
}

static void RtemsCpuuseReqReportWithPlugin_Post_Total_Check(
  RtemsCpuuseReqReportWithPlugin_Context   *ctx,
  RtemsCpuuseReqReportWithPlugin_Post_Total state
)
{
  switch ( state ) {
    case RtemsCpuuseReqReportWithPlugin_Post_Total_Elapsed: {
      /*
       * The TIME field of the total line shall be the time elapsed since the
       * last CPU usage reset point at some time point during the directive
       * call, in the time format of the report.
       */
      CheckTime( ctx->total + TOTAL_FIELD_OFFSET, &ctx->elapsed_time );
      break;
    }

    case RtemsCpuuseReqReportWithPlugin_Post_Total_NA:
      break;
  }
}

static void RtemsCpuuseReqReportWithPlugin_Setup(
  RtemsCpuuseReqReportWithPlugin_Context *ctx
)
{
  int status;

  ctx->previous_timecount_handler = SetGetTimecountHandler( FreezeTime );
  ctx->id = CreateTask( "WORK", PRIO_LOW );
  ctx->thread = GetThread( ctx->id );
  status = pthread_getcpuclockid( (pthread_t) ctx->id, &ctx->clock_id );
  T_assert_eq_int( status, 0 );
}

static void RtemsCpuuseReqReportWithPlugin_Setup_Wrap( void *arg )
{
  RtemsCpuuseReqReportWithPlugin_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsCpuuseReqReportWithPlugin_Setup( ctx );
}

static void RtemsCpuuseReqReportWithPlugin_Teardown(
  RtemsCpuuseReqReportWithPlugin_Context *ctx
)
{
  rtems_cpu_usage_reset();
  DeleteTask( ctx->id );
  SetGetTimecountHandler( ctx->previous_timecount_handler );
}

static void RtemsCpuuseReqReportWithPlugin_Teardown_Wrap( void *arg )
{
  RtemsCpuuseReqReportWithPlugin_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsCpuuseReqReportWithPlugin_Teardown( ctx );
}

static void RtemsCpuuseReqReportWithPlugin_Prepare(
  RtemsCpuuseReqReportWithPlugin_Context *ctx
)
{
  rtems_cpu_usage_reset();
  ctx->name = NULL;
  ctx->elapsed = 0;
  ctx->used = 0;
}

static void RtemsCpuuseReqReportWithPlugin_Action(
  RtemsCpuuseReqReportWithPlugin_Context *ctx
)
{
  rtems_printer printer;
  int           status;

  ResetOutput( ctx );
  printer.context = ctx;
  printer.printer = Capture;
  rtems_cpu_usage_report_with_plugin( &printer );

  ctx->line = FindThreadLine( ctx );
  T_assert_not_null( ctx->line );
  ctx->total = strstr( ctx->output, TOTAL_LINE_PREFIX );
  T_assert_not_null( ctx->total );

  status = clock_gettime( ctx->clock_id, &ctx->used_time );
  T_eq_int( status, 0 );
  status = clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &ctx->elapsed_time );
  T_eq_int( status, 0 );
}

static void RtemsCpuuseReqReportWithPlugin_Cleanup( void )
{
  rtems_cpu_usage_reset();
}

/* clang-format off */

static const RtemsCpuuseReqReportWithPlugin_Entry
RtemsCpuuseReqReportWithPlugin_Entries[] = {
  { 1, 0, 0, 0, RtemsCpuuseReqReportWithPlugin_Post_Name_NA,
    RtemsCpuuseReqReportWithPlugin_Post_Seconds_NA,
    RtemsCpuuseReqReportWithPlugin_Post_Percent_NA,
    RtemsCpuuseReqReportWithPlugin_Post_Total_NA },
  { 0, 0, 0, 0, RtemsCpuuseReqReportWithPlugin_Post_Name_Complete,
    RtemsCpuuseReqReportWithPlugin_Post_Seconds_Used,
    RtemsCpuuseReqReportWithPlugin_Post_Percent_Ratio,
    RtemsCpuuseReqReportWithPlugin_Post_Total_Elapsed },
  { 0, 0, 0, 0, RtemsCpuuseReqReportWithPlugin_Post_Name_Truncated,
    RtemsCpuuseReqReportWithPlugin_Post_Seconds_Used,
    RtemsCpuuseReqReportWithPlugin_Post_Percent_Ratio,
    RtemsCpuuseReqReportWithPlugin_Post_Total_Elapsed },
  { 0, 0, 0, 0, RtemsCpuuseReqReportWithPlugin_Post_Name_Complete,
    RtemsCpuuseReqReportWithPlugin_Post_Seconds_Used,
    RtemsCpuuseReqReportWithPlugin_Post_Percent_Zero,
    RtemsCpuuseReqReportWithPlugin_Post_Total_Elapsed },
  { 0, 0, 0, 0, RtemsCpuuseReqReportWithPlugin_Post_Name_Truncated,
    RtemsCpuuseReqReportWithPlugin_Post_Seconds_Used,
    RtemsCpuuseReqReportWithPlugin_Post_Percent_Zero,
    RtemsCpuuseReqReportWithPlugin_Post_Total_Elapsed }
};

static const uint8_t
RtemsCpuuseReqReportWithPlugin_Map[] = {
  3, 0, 0, 1, 1, 1, 4, 0, 0, 2, 2, 2
};

/* clang-format on */

static size_t RtemsCpuuseReqReportWithPlugin_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  RtemsCpuuseReqReportWithPlugin_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    size_t pcd[ 3 ];
    size_t i;

    for ( i = 0; i < 3; ++i ) {
      pcd[ i ] =
        RtemsCpuuseReqReportWithPlugin_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
        RtemsCpuuseReqReportWithPlugin_GetRepresentative( ctx, i );
    }

    return T_get_scope( RtemsCpuuseReqReportWithPlugin_PreDesc, buf, n, pcd );
  }

  return 0;
}

static T_fixture RtemsCpuuseReqReportWithPlugin_Fixture = {
  .setup = RtemsCpuuseReqReportWithPlugin_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsCpuuseReqReportWithPlugin_Teardown_Wrap,
  .scope = RtemsCpuuseReqReportWithPlugin_Scope,
  .initial_context = &RtemsCpuuseReqReportWithPlugin_Instance
};

static inline RtemsCpuuseReqReportWithPlugin_Entry
RtemsCpuuseReqReportWithPlugin_PopEntry(
  RtemsCpuuseReqReportWithPlugin_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsCpuuseReqReportWithPlugin_Entries
    [ RtemsCpuuseReqReportWithPlugin_Map[ index ] ];
}

static void RtemsCpuuseReqReportWithPlugin_TestVariant(
  RtemsCpuuseReqReportWithPlugin_Context *ctx
)
{
  RtemsCpuuseReqReportWithPlugin_Pre_Name_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsCpuuseReqReportWithPlugin_Pre_Elapsed_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsCpuuseReqReportWithPlugin_Pre_Used_Prepare( ctx, ctx->Map.pcs[ 2 ] );

  if ( ctx->Map.skip_rep ) {
    return;
  }

  RtemsCpuuseReqReportWithPlugin_Action( ctx );
  RtemsCpuuseReqReportWithPlugin_Post_Name_Check(
    ctx,
    ctx->Map.entry.Post_Name
  );
  RtemsCpuuseReqReportWithPlugin_Post_Seconds_Check(
    ctx,
    ctx->Map.entry.Post_Seconds
  );
  RtemsCpuuseReqReportWithPlugin_Post_Percent_Check(
    ctx,
    ctx->Map.entry.Post_Percent
  );
  RtemsCpuuseReqReportWithPlugin_Post_Total_Check(
    ctx,
    ctx->Map.entry.Post_Total
  );
}

/**
 * @fn void T_case_body_RtemsCpuuseReqReportWithPlugin( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsCpuuseReqReportWithPlugin,
  &RtemsCpuuseReqReportWithPlugin_Fixture
)
{
  RtemsCpuuseReqReportWithPlugin_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  RtemsCpuuseReqReportWithPlugin_ResetRepresentatives( ctx );

  for (
    ctx->Map.pcs[ 0 ] = RtemsCpuuseReqReportWithPlugin_Pre_Name_Fits;
    ctx->Map.pcs[ 0 ] < RtemsCpuuseReqReportWithPlugin_Pre_Name_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsCpuuseReqReportWithPlugin_Pre_Elapsed_Zero;
      ctx->Map.pcs[ 1 ] < RtemsCpuuseReqReportWithPlugin_Pre_Elapsed_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsCpuuseReqReportWithPlugin_Pre_Used_Zero;
        ctx->Map.pcs[ 2 ] < RtemsCpuuseReqReportWithPlugin_Pre_Used_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = RtemsCpuuseReqReportWithPlugin_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        for (
          ctx->Map.rep = 0;
          ctx->Map.rep <
          RtemsCpuuseReqReportWithPlugin_GetRepresentativeCount( ctx );
          ++ctx->Map.rep
        ) {
          ctx->Map.skip_rep = false;
          RtemsCpuuseReqReportWithPlugin_Prepare( ctx );
          RtemsCpuuseReqReportWithPlugin_TestVariant( ctx );
          RtemsCpuuseReqReportWithPlugin_Cleanup();

          RtemsCpuuseReqReportWithPlugin_MarkRepresentatives( ctx );
        }
      }
    }
  }

  RtemsCpuuseReqReportWithPlugin_CheckRepresentatives( ctx );
}

/** @} */
