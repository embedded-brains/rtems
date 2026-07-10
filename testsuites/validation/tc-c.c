/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CValC
 */

/*
 * Copyright (C) 2021, 2025 embedded brains GmbH & Co. KG
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
 * - Call memchr() for a sample set of buffers and characters to locate.
 *
 * - Call memcmp() for a sample set of buffers.
 *
 * - Call memcpy() for a sample set of buffers.
 *
 * - Call memmove() for a sample set of buffers.
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

static void Fill( volatile uint8_t *b, const volatile uint8_t *e )
{
  uint8_t chr;

  chr = 1;

  while ( b != e ) {
    *b = chr;
    ++chr;
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
 * @brief Call memchr() for a sample set of buffers and characters to locate.
 */
static void CValC_Action_0( void )
{
  uint8_t  mem[sizeof( long ) * 10];
  uint8_t *aligned_mem;
  size_t   offset_mem;

  aligned_mem = (uint8_t *) RTEMS_ALIGN_UP( (uintptr_t) mem, sizeof( long ) );

  for ( offset_mem = 0; offset_mem < sizeof( long ); ++offset_mem  ) {
    size_t size;

    for ( size = 0; size < sizeof( long ) * 8; ++size ) {
      unsigned char chr;
      uint8_t      *m;
      uint8_t      *e;
      void         *p;

      m = aligned_mem + offset_mem;
      e = m + size;
      Fill( m, e );

      for ( chr = 0; chr < size; ++chr ) {
        p = memchr( m, chr + 1, size );
        T_eq_ptr( p, m + chr );
      }

      p = memchr( m, size + 1, size );
      T_null( p );
    }
  }
}

/**
 * @brief Call memcmp() for a sample set of buffers.
 */
static void CValC_Action_1( void )
{
  uint8_t  mem_a[sizeof( long ) * 10];
  uint8_t  mem_b[sizeof( long ) * 10];
  uint8_t *aligned_mem_a;
  uint8_t *aligned_mem_b;
  size_t   offset_mem_a;

  memset( mem_a, 0x85, sizeof( mem_a ) );
  memset( mem_b, 0x85, sizeof( mem_b ) );
  aligned_mem_a = (uint8_t *) RTEMS_ALIGN_UP( (uintptr_t) mem_a, sizeof( long ) );
  aligned_mem_b = (uint8_t *) RTEMS_ALIGN_UP( (uintptr_t) mem_b, sizeof( long ) );

  for ( offset_mem_a = 0; offset_mem_a < sizeof( long ); ++offset_mem_a  ) {
    size_t offset_mem_b;

    for ( offset_mem_b = 0; offset_mem_b < sizeof( long ); ++offset_mem_b  ) {
      size_t size;

      for ( size = 0; size < sizeof( long ) * 8; ++size ) {
        uint8_t *a;
        uint8_t *b;
        int      c;
        size_t   defect;

        a = aligned_mem_a + offset_mem_a;
        b = aligned_mem_b + offset_mem_b;
        Fill( a, a + size );
        Fill( b, b + size );

        c = memcmp( a, b, size );
        T_eq_int( c, 0 );

        for ( defect = 1; defect < size; ++defect ) {
          Fill( b, b + size );
          b[ defect ] += defect;

          c = memcmp( a, b, size );
          T_lt_int( c, 0 );
        }

        for ( defect = 1; defect < size; ++defect ) {
          Fill( b, b + size );
          b[ defect ] -= defect;

          c = memcmp( a, b, size );
          T_gt_int( c, 0 );
        }
      }
    }
  }
}

/**
 * @brief Call memcpy() for a sample set of buffers.
 */
static void CValC_Action_2( void )
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
        uint8_t *d;
        uint8_t *e;

        s = aligned_src + offset_src;
        d = aligned_dst + offset_dst;
        e = d + size;

        Clear( begin, end );
        memcpy( d, s, size );
        T_true( Compare( begin, d, 0 ) );
        T_true( Compare( d, e, 0x85 ) );
        T_true( Compare( e, end, 0 ) );
      }
    }
  }
}

/**
 * @brief Call memmove() for a sample set of buffers.
 */
static void CValC_Action_3( void )
{
  uint8_t  src[sizeof( long ) * 18];
  uint8_t  dst[sizeof( long ) * 10];
  uint8_t *begin;
  uint8_t *end;
  uint8_t *aligned_src;
  uint8_t *aligned_dst;
  size_t   offset_src;

  /* Non-overlapping memory areas */

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
        uint8_t *d;
        uint8_t *e;

        s = aligned_src + offset_src;
        d = aligned_dst + offset_dst;
        e = d + size;

        Clear( begin, end );
        memmove( d, s, size );
        T_true( Compare( begin, d, 0 ) );
        T_true( Compare( d, e, 0x85 ) );
        T_true( Compare( e, end, 0 ) );
      }
    }
  }

  /* Overlapping memory areas */

  begin = src;
  end = begin + sizeof( src );
  aligned_dst = aligned_src;

  for ( offset_src = 0; offset_src < sizeof( long ); ++offset_src  ) {
    size_t offset_dst;

    for ( offset_dst = 0; offset_dst < sizeof( long ); ++offset_dst  ) {
      size_t size;

      for ( size = 0; size < sizeof( long ) * 8; ++size ) {
        uint8_t *s;
        uint8_t *d;
        uint8_t *es;
        uint8_t *ed;

        s = aligned_src + offset_src;
        d = aligned_dst + offset_dst;
        es = s + size;
        ed = d + size;

        Clear( begin, end );
        memset( s, 0x85, size );
        memmove( d, s, size );

        T_true( Compare( d, ed, 0x85 ) );
        T_true( Compare( s, es, 0x85 ) );

        if ( offset_src <= offset_dst ) {
          T_true( Compare( begin, s, 0 ) );
          T_true( Compare( ed, end, 0 ) );

          if ( (uintptr_t) ed < (uintptr_t) s ) {
            T_true( Compare( ed, s, 0 ) );
          }
        } else {
          T_true( Compare( begin, d, 0 ) );
          T_true( Compare( es, end, 0 ) );

          if ( (uintptr_t) es < (uintptr_t) d ) {
            T_true( Compare( es, d, 0 ) );
          }
        }
      }
    }
  }
}

/**
 * @brief Call memset() for a sample set of buffers.
 */
static void CValC_Action_4( void )
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
      uint8_t *d;
      uint8_t *e;

      d = aligned + offset;
      e = d + size;

      Clear( begin, end );
      memset( d, 0x85, size );
      T_true( Compare( begin, d, 0 ) );
      T_true( Compare( d, e, 0x85 ) );
      T_true( Compare( e, end, 0 ) );
    }
  }
}

/**
 * @brief Call strlen() for a sample set of strings.
 */
static void CValC_Action_5( void )
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
static void CValC_Action_6( void )
{
  assert( 1 );
}

/**
 * @brief Call assert() with an expression which evaluates to false. Check that
 *   the expected fatal error happened.
 */
static void CValC_Action_7( void )
{
  ProduceAndCheckFatalError( AssertFalse, CheckAssert, NULL );
}

/**
 * @brief Check that errno has a signed integer type.
 */
static void CValC_Action_8( void )
{
  errno = -1;
  T_eq_int( errno, -1 );
  T_lt_int( errno, 0 );
}

/**
 * @brief Check that errno has thread storage duration.
 */
static void CValC_Action_9( void )
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
  CValC_Action_7();
  CValC_Action_8();
  CValC_Action_9();
}

/** @} */
