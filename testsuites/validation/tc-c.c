/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CValC
 */

/*
 * Copyright (C) 2021, 2024 embedded brains GmbH & Co. KG
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

#include <assert.h>
#include <errno.h>
#include <rtems.h>
#include <string.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CValC spec:/c/val/c
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests C library functions.
 *
 * This test case performs the following actions:
 *
 * - Call memcpy() for a sample set of buffers.
 *
 * - Call memset() for a sample set of buffers.
 *
 * - Call strlen() for a sample set of strings.
 *
 * - Call assert() with an expression which evaluates to true.  We expect that
 *   nothing will happen.
 *
 * - Call assert() with an expression which evaluates to false. Check that the
 *   expected fatal error happened.
 *
 * - Check that errno has a signed integer type.
 *
 * - Check that errno has thread storage duration.
 *
 * @{
 */

static void Clear( volatile uint8_t *b, const volatile uint8_t *e )
{
  while ( b != e ) {
    *b = 0;
    ++b;
  }
}

static bool Compare(
  volatile uint8_t       *b,
  const volatile uint8_t *e,
  uint8_t                 expected
)
{
  bool result;

  result = true;

  while ( b != e ) {
    result = result && *b == expected;
    ++b;
  }

  return result;
}

static int expected_line;

static void CheckAssert(
  rtems_fatal_source source,
  rtems_fatal_code   code,
  void              *arg
)
{
  T_eq_int( source, RTEMS_FATAL_SOURCE_ASSERT );

  if ( source == RTEMS_FATAL_SOURCE_ASSERT ) {
    const rtems_assert_context *ctx;

    ctx = (const rtems_assert_context *) code;
    T_true( ContainsSubstring( ctx->file, "tc-c.c" ) );
    T_eq_int( ctx->line, expected_line );
    T_true( ContainsSubstring( ctx->failed_expression, "arg != NULL" ) );
  }

  T_null( arg );
}

static void AssertFalse( void *arg )
{
  expected_line = __LINE__ + 1;
  assert( arg != NULL );
}

/**
 * @brief Call memcpy() for a sample set of buffers.
 */
static void CValC_Action_0( void )
{
  uint8_t  src[sizeof( long ) * 10];
  uint8_t  dst[sizeof( long ) * 10];
  uint8_t *begin;
  uint8_t *end;
  uint8_t *aligned_src;
  uint8_t *aligned_dst;
  size_t   offset_src;

  memset( src, 0x85, sizeof( src ) );
  begin = dst;
  end = begin + sizeof( dst );
  aligned_src = (uint8_t *) RTEMS_ALIGN_UP( (uintptr_t) src, sizeof( long ) );
  aligned_dst = (uint8_t *) RTEMS_ALIGN_UP( (uintptr_t) dst, sizeof( long ) );

  for ( offset_src = 0; offset_src < sizeof( long ); ++offset_src  ) {
    size_t offset_dst;

    for ( offset_dst = 0; offset_dst < sizeof( long ); ++offset_dst  ) {
      size_t size;

      for ( size = 0; size < sizeof( long ) * 8; ++size ) {
        uint8_t *s;
        uint8_t *p;
        uint8_t *q;

        s = aligned_src + offset_src;
        p = aligned_dst + offset_dst;
        q = p + size;

        Clear( begin, end );
        memcpy( p, s, size );
        T_true( Compare( begin, p, 0 ) );
        T_true( Compare( p, q, 0x85 ) );
        T_true( Compare( q, end, 0 ) );
      }
    }
  }
}

/**
 * @brief Call memset() for a sample set of buffers.
 */
static void CValC_Action_1( void )
{
  uint8_t  dst[sizeof( long ) * 10];
  uint8_t *begin;
  uint8_t *end;
  uint8_t *aligned;
  size_t   offset;

  begin = dst;
  end = begin + sizeof( dst );
  aligned = (uint8_t *) RTEMS_ALIGN_UP( (uintptr_t) dst, sizeof( long ) );

  for ( offset = 0; offset < sizeof( long ); ++offset  ) {
    size_t size;

    for ( size = 0; size < sizeof( long ) * 8; ++size ) {
      uint8_t *p;
      uint8_t *q;

      p = aligned + offset;
      q = p + size;

      Clear( begin, end );
      memset( p, 0x85, size );
      T_true( Compare( begin, p, 0 ) );
      T_true( Compare( p, q, 0x85 ) );
      T_true( Compare( q, end, 0 ) );
    }
  }
}

/**
 * @brief Call strlen() for a sample set of strings.
 */
static void CValC_Action_2( void )
{
  char    str[sizeof( long ) * 10];
  char   *aligned_str;
  size_t  offset;

  aligned_str = (char *) RTEMS_ALIGN_UP( (uintptr_t) str, sizeof( long ) );

  for ( offset = 0; offset < sizeof( long ); ++offset  ) {
    size_t size;

    for ( size = 0; size < sizeof( long ) * 8; ++size ) {
      char *s;

      s = aligned_str + offset;

      memset( s, 0x85, size );
      s[size] = '\0';
      RTEMS_OBFUSCATE_VARIABLE( s );
      T_eq_sz( strlen( s ), size );
    }
  }
}

/**
 * @brief Call assert() with an expression which evaluates to true.  We expect
 *   that nothing will happen.
 */
static void CValC_Action_3( void )
{
  assert( 1 );
}

/**
 * @brief Call assert() with an expression which evaluates to false. Check that
 *   the expected fatal error happened.
 */
static void CValC_Action_4( void )
{
  ProduceAndCheckFatalError( AssertFalse, CheckAssert, NULL );
}

/**
 * @brief Check that errno has a signed integer type.
 */
static void CValC_Action_5( void )
{
  errno = -1;
  T_eq_int( errno, -1 );
  T_lt_int( errno, 0 );
}

/**
 * @brief Check that errno has thread storage duration.
 */
static void CValC_Action_6( void )
{
  T_true( IsTLSObjectOfThread( RTEMS_SELF, &errno ) );
}

/**
 * @fn void T_case_body_CValC( void )
 */
T_TEST_CASE( CValC )
{
  CValC_Action_0();
  CValC_Action_1();
  CValC_Action_2();
  CValC_Action_3();
  CValC_Action_4();
  CValC_Action_5();
  CValC_Action_6();
}

/** @} */
