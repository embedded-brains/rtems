/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsCounterValCounter
 */

/*
 * Copyright (C) 2025 embedded brains GmbH & Co. KG
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/counter.h>

#include <rtems/test.h>

/**
 * @defgroup RtemsCounterValCounter spec:/rtems/counter/val/counter
 *
 * @ingroup TestsuitesValidation0
 *
 * @brief Tests the RTEMS counter.
 *
 * This test case performs the following actions:
 *
 * - Validate the RTEMS counter directives.
 *
 *   - Check that rtems_counter_difference() returns the expected value.
 *
 *   - Check that rtems_counter_frequency() returns a valid frequency.
 *
 *   - Check that rtems_counter_read() returns monotonically increasing values.
 *
 *   - Check that rtems_counter_ticks_to_nanoseconds() returns the expected
 *     value.
 *
 *   - Check that rtems_counter_nanoseconds_to_ticks() returns the expected
 *     value.
 *
 *   - Check that rtems_counter_ticks_to_sbintime() returns the expected value.
 *
 *   - Check that rtems_counter_sbintime_to_ticks() returns the expected value.
 *
 *   - Check that during a rtems_counter_delay_ticks() call the expected amount
 *     of time elapsed.
 *
 *   - Check that during a rtems_counter_delay_nanoseconds() call the expected
 *     amount of time elapsed.
 *
 * @{
 */

/**
 * @brief Validate the RTEMS counter directives.
 */
static void RtemsCounterValCounter_Action_0( void )
{
  rtems_counter_ticks ticks[ 10 ];
  rtems_counter_ticks max;
  rtems_counter_ticks interval_ticks;
  uint32_t            frequency;
  size_t              i;
  uint64_t            t0;
  uint64_t            t1;
  uint64_t            interval_ns;
  uint32_t            clock_resolution_ns;
  uint64_t            delta_ns;

  max = (rtems_counter_ticks) - 1;
  frequency = rtems_counter_frequency();
  interval_ticks = frequency / 123;
  interval_ns = UINT64_C( 1000000000 ) / 123;
  clock_resolution_ns = ( UINT32_C( 1000000000 ) + frequency - 1 ) / frequency;

  /*
   * Check that rtems_counter_difference() returns the expected value.
   */
  T_eq_ulong( rtems_counter_difference( 2, 1 ), 1 );

  /*
   * Check that rtems_counter_frequency() returns a valid frequency.
   */
  T_gt_u32( frequency, 0 );

  /*
   * Check that rtems_counter_read() returns monotonically increasing values.
   */
  ticks[ 0 ] = rtems_counter_read();

  for ( i = 1; i < RTEMS_ARRAY_SIZE( ticks ); ++i ) {
    rtems_counter_ticks delta;

    do {
      ticks[ i ] = rtems_counter_read();
    } while ( ticks[ i - 1 ] == ticks[ i ] );

    delta = ticks[ i ] - ticks[ i - 1 ];
    T_lt_ulong( delta, max / 2 );
  }

  /*
   * Check that rtems_counter_ticks_to_nanoseconds() returns the expected
   * value.
   */
  T_le_u32(
    rtems_counter_ticks_to_nanoseconds( frequency ) -
      UINT32_C( 1000000000 ),
    1
  );

  /*
   * Check that rtems_counter_nanoseconds_to_ticks() returns the expected
   * value.
   */
  T_le_ulong(
    rtems_counter_nanoseconds_to_ticks( UINT32_C( 1000000000 ) ) -
      frequency,
    1
  );

  /*
   * Check that rtems_counter_ticks_to_sbintime() returns the expected value.
   */
  T_le_i64(
    rtems_counter_ticks_to_sbintime( frequency ) -
      INT64_C( 0x100000000 ),
    1
  );

  /*
   * Check that rtems_counter_sbintime_to_ticks() returns the expected value.
   */
  T_le_ulong(
    rtems_counter_sbintime_to_ticks( INT64_C( 0x100000000 ) ) -
      frequency,
    1
  );

  /*
   * Check that during a rtems_counter_delay_ticks() call the expected amount
   * of time elapsed.
   */
  t0 = rtems_clock_get_uptime_nanoseconds();
  rtems_counter_delay_ticks( interval_ticks );
  t1 = rtems_clock_get_uptime_nanoseconds();
  delta_ns = t1 - t0;

  if ( delta_ns < interval_ns ) {
    T_lt_u64( interval_ns - delta_ns, clock_resolution_ns );
  }

  /*
   * Check that during a rtems_counter_delay_nanoseconds() call the expected
   * amount of time elapsed.
   */
  t0 = rtems_clock_get_uptime_nanoseconds();
  rtems_counter_delay_nanoseconds( interval_ns );
  t1 = rtems_clock_get_uptime_nanoseconds();
  delta_ns = t1 - t0;

  if ( delta_ns < interval_ns ) {
    T_lt_u64( interval_ns - delta_ns, clock_resolution_ns );
  }
}

/**
 * @fn void T_case_body_RtemsCounterValCounter( void )
 */
T_TEST_CASE( RtemsCounterValCounter )
{
  RtemsCounterValCounter_Action_0();
}

/** @} */
