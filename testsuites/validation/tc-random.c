/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CValRandom
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

#include <stdlib.h>
#include <unistd.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CValRandom spec:/c/val/random
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests C library functions providing random numbers.
 *
 * This test case performs the following actions:
 *
 * - Call getentropy() for a sample set of buffers.
 *
 *   - Check that the buffer was filled with data.
 *
 * - Call arc4random() a couple of times.
 *
 *   - Check that the not always the same data was produced.
 *
 * - Allocate a buffer which has a length of twice the maximum reinitialization
 *   count.  Call arc4random_buf() with the buffer.  Perform a very simple
 *   randomness test.
 *
 *   - Check that the not always the same data was produced.
 *
 * - Let the getentropy() function fail while the arc4random() support uses it
 *   to reinitialize the pseudo-random number generator.  After this test, the
 *   internal mutex is still locked.
 *
 * @{
 */

static void CheckGetentropyFail(
  rtems_fatal_source source,
  rtems_fatal_code   code,
  void              *arg
)
{
  T_eq_int( source, INTERNAL_ERROR_CORE );
  T_eq_ulong( code, INTERNAL_ERROR_ARC4RANDOM_GETENTROPY_FAIL );
  T_null( arg );
}

static void ExhaustPseudoRandomNumbers( void *arg )
{
  T_null( arg );

  while ( true ) {
    (void) arc4random();
  }
}

static uint32_t getentropy_counter;

static int getentropy_status;

int __real_getentropy( void *ptr, size_t n );

int __wrap_getentropy( void *ptr, size_t n );

int __wrap_getentropy( void *ptr, size_t n )
{
  int status;

  ++getentropy_counter;
  status = getentropy_status;

  if ( status != 0 ) {
    getentropy_status = 0;
    return status;
  }

  return __real_getentropy( ptr, n );
}

/**
 * @brief Call getentropy() for a sample set of buffers.
 */
static void CValRandom_Action_0( void )
{
  uint8_t  buf[ 32 ];
  uint32_t same;
  size_t   i;

  same = 0;

  for ( i = 0; i < RTEMS_ARRAY_SIZE( buf ); ++i ) {
    size_t j;

    memset( buf, 0xff, sizeof( buf ) );
    T_eq_int( getentropy( buf, i ), 0 );

    for ( j = 0; j < i ; ++j ) {
      if ( buf[ j ] == 0xff ) {
        ++same;
      }
    }

    for ( j = i; j < RTEMS_ARRAY_SIZE( buf ) ; ++j ) {
      T_eq_u8( buf[ j ], 0xff );
    }
  }

  /*
   * Check that the buffer was filled with data.
   */
  /* Failure should be unlikely */
  T_lt_u32( same, 10 );
}

/**
 * @brief Call arc4random() a couple of times.
 */
static void CValRandom_Action_1( void )
{
  uint32_t expected_counter;
  uint32_t last;
  uint32_t same;

  expected_counter = getentropy_counter + 2;
  same = 0;
  last = 0;

  do {
    uint32_t now;

    now = arc4random();

    if ( now == last ) {
      ++same;
    }

    last = now;
  } while ( expected_counter != getentropy_counter );

  /*
   * Check that the not always the same data was produced.
   */
  /* Failure should be unlikely */
  T_lt_u32( same, 10 );
}

/**
 * @brief Allocate a buffer which has a length of twice the maximum
 *   reinitialization count.  Call arc4random_buf() with the buffer.  Perform a
 *   very simple randomness test.
 */
static void CValRandom_Action_2( void )
{
  size_t    len;
  uint32_t *buf;
  uint32_t  counter;
  uint32_t  same;
  uint32_t  last;
  size_t    i;

  len = 2 * 1024 * 1024;
  buf = T_malloc( len );
  T_assert_not_null( buf );
  counter = getentropy_counter;
  arc4random_buf( buf, len );
  T_ne_u32( getentropy_counter, counter );

  same = 0;
  last = 0;

  for ( i = 0; i < len / sizeof( *buf ); ++i ) {
    uint32_t now;

    now = buf[ i ];

    if ( now == last ) {
      ++same;
    }

    last = now;
  }

  /*
   * Check that the not always the same data was produced.
   */
  /* Failure should be unlikely */
  T_lt_u32( same, 10 );
}

/**
 * @brief Let the getentropy() function fail while the arc4random() support
 *   uses it to reinitialize the pseudo-random number generator.  After this
 *   test, the internal mutex is still locked.
 */
static void CValRandom_Action_3( void )
{
  getentropy_status = -1;
  ProduceAndCheckFatalError(
    ExhaustPseudoRandomNumbers,
    CheckGetentropyFail,
    NULL
  );
}

/**
 * @fn void T_case_body_CValRandom( void )
 */
T_TEST_CASE( CValRandom )
{
  CValRandom_Action_0();
  CValRandom_Action_1();
  CValRandom_Action_2();
  CValRandom_Action_3();
}

/** @} */
