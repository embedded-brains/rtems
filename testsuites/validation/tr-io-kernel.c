/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIoValKernel
 */

/*
 * Copyright (C) 2021, 2026 embedded brains GmbH & Co. KG
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

#include <rtems/bspIo.h>

#include "tr-io-kernel.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsIoValKernel spec:/rtems/io/val/kernel
 *
 * @ingroup TestsuitesValidationIoKernel
 *
 * @brief Tests the functions referenced by BSP_output_char and BSP_poll_char.
 *
 * This test case performs the following actions:
 *
 * - Call the function referenced by BSP_output_char with characters and no
 *   flag.
 *
 *   - Check that the function returned zero for each call.
 *
 * - Check the values of the flags and the mask of BSP_output_char.
 *
 *   - Check that RTEMS_OUTPUT_CHAR_MASK selects the bits of an unsigned
 *     character.
 *
 *   - Check that RTEMS_OUTPUT_CHAR_DRAIN and RTEMS_OUTPUT_CHAR_TRY are
 *     distinct bits outside of RTEMS_OUTPUT_CHAR_MASK.
 *
 * - Call the function referenced by BSP_output_char with
 *   RTEMS_OUTPUT_CHAR_DRAIN. Then call it with RTEMS_OUTPUT_CHAR_DRAIN and
 *   RTEMS_OUTPUT_CHAR_TRY.
 *
 *   - Check that the drain request returned zero or minus one.
 *
 *   - Check that the drain request with RTEMS_OUTPUT_CHAR_TRY returned zero if
 *     the drain request returned zero, otherwise minus one.
 *
 * - Call the function referenced by BSP_output_char with RTEMS_OUTPUT_CHAR_TRY
 *   and a character. Then call it with a new line character and no flag.
 *
 *   - Check that the request with RTEMS_OUTPUT_CHAR_TRY returned zero or minus
 *     one.
 *
 *   - Check that the call with the new line character returned zero.
 *
 * - If BSP_poll_char references a function, then call it.
 *
 *   - Check that the returned value is minus one or an unsigned character
 *     value.
 *
 * @{
 */

/**
 * @brief Call the function referenced by BSP_output_char with characters and
 *   no flag.
 */
static void RtemsIoValKernel_Action_0( void )
{
  int rv_x;
  int rv_nl;

  T_report_hash_sha256_update( 'X' );
  rv_x = ( *BSP_output_char )( 'X' );
  T_report_hash_sha256_update( '\n' );
  rv_nl = ( *BSP_output_char )( '\n' );

  /*
   * Check that the function returned zero for each call.
   */
  T_step_eq_int( 0, rv_x, 0 );
  T_step_eq_int( 1, rv_nl, 0 );
}

/**
 * @brief Check the values of the flags and the mask of BSP_output_char.
 */
static void RtemsIoValKernel_Action_1( void )
{
  /* No action */

  /*
   * Check that RTEMS_OUTPUT_CHAR_MASK selects the bits of an unsigned
   * character.
   */
  T_step_eq_int( 2, RTEMS_OUTPUT_CHAR_MASK, 0xff );

  /*
   * Check that RTEMS_OUTPUT_CHAR_DRAIN and RTEMS_OUTPUT_CHAR_TRY are distinct
   * bits outside of RTEMS_OUTPUT_CHAR_MASK.
   */
  T_step_eq_int( 3, RTEMS_OUTPUT_CHAR_DRAIN, 0x100 );
  T_step_eq_int( 4, RTEMS_OUTPUT_CHAR_TRY, 0x200 );
}

/**
 * @brief Call the function referenced by BSP_output_char with
 *   RTEMS_OUTPUT_CHAR_DRAIN. Then call it with RTEMS_OUTPUT_CHAR_DRAIN and
 *   RTEMS_OUTPUT_CHAR_TRY.
 */
static void RtemsIoValKernel_Action_2( void )
{
  int rv_drain;
  int rv_drain_try;

  rv_drain = ( *BSP_output_char )( RTEMS_OUTPUT_CHAR_DRAIN | 'D' );
  rv_drain_try = ( *BSP_output_char )(
    RTEMS_OUTPUT_CHAR_DRAIN | RTEMS_OUTPUT_CHAR_TRY | 'T'
  );

  /*
   * Check that the drain request returned zero or minus one.
   */
  T_step_true( 5, rv_drain == 0 || rv_drain == -1 );

  /*
   * Check that the drain request with RTEMS_OUTPUT_CHAR_TRY returned zero if
   * the drain request returned zero, otherwise minus one.
   */
  T_step_eq_int( 6, rv_drain_try, rv_drain );
}

/**
 * @brief Call the function referenced by BSP_output_char with
 *   RTEMS_OUTPUT_CHAR_TRY and a character. Then call it with a new line
 *   character and no flag.
 */
static void RtemsIoValKernel_Action_3( void )
{
  int rv_try;
  int rv_nl;

  rv_try = ( *BSP_output_char )( RTEMS_OUTPUT_CHAR_TRY | 'T' );

  if ( rv_try == 0 ) {
    T_report_hash_sha256_update( 'T' );
  }

  T_report_hash_sha256_update( '\n' );
  rv_nl = ( *BSP_output_char )( '\n' );

  /*
   * Check that the request with RTEMS_OUTPUT_CHAR_TRY returned zero or minus
   * one.
   */
  T_step_true( 7, rv_try == 0 || rv_try == -1 );

  /*
   * Check that the call with the new line character returned zero.
   */
  T_step_eq_int( 8, rv_nl, 0 );
}

/**
 * @brief If BSP_poll_char references a function, then call it.
 */
static void RtemsIoValKernel_Action_4( void )
{
  BSP_polling_getchar_function_type poll_char;
  int                               c;

  poll_char = BSP_poll_char;

  if ( poll_char != NULL ) {
    c = ( *poll_char )();
  } else {
    c = -1;
  }

  /*
   * Check that the returned value is minus one or an unsigned character value.
   */
  T_step_true( 9, c == -1 || ( c & ~0xff ) == 0 );
}

void RtemsIoValKernel_Run( void )
{
  (void) T_case_begin( "RtemsIoValKernel", &T_empty_fixture );

  T_plan( 10 );

  RtemsIoValKernel_Action_0();
  RtemsIoValKernel_Action_1();
  RtemsIoValKernel_Action_2();
  RtemsIoValKernel_Action_3();
  RtemsIoValKernel_Action_4();

  T_case_end();
}

/** @} */
