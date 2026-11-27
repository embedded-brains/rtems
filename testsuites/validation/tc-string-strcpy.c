/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringReqStrcpy
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

#include <errno.h>
#include <string.h>

#include <rtems/test.h>

/**
 * @defgroup CStringReqStrcpy spec:/c/string/req/strcpy
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * The source string holds the letters of the alphabet in turn. A character
 * which is not a null character follows its terminating null character. The
 * bytes of the destination buffer hold the value 0xaa before the call.
 *
 * @{
 */

typedef enum {
  CStringReqStrcpy_Pre_Alignment_Aligned,
  CStringReqStrcpy_Pre_Alignment_Unaligned,
  CStringReqStrcpy_Pre_Alignment_Mismatch,
  CStringReqStrcpy_Pre_Alignment_NA
} CStringReqStrcpy_Pre_Alignment;

typedef enum {
  CStringReqStrcpy_Pre_Length_Short,
  CStringReqStrcpy_Pre_Length_Medium,
  CStringReqStrcpy_Pre_Length_Long,
  CStringReqStrcpy_Pre_Length_NA
} CStringReqStrcpy_Pre_Length;

typedef enum {
  CStringReqStrcpy_Post_Result_S1,
  CStringReqStrcpy_Post_Result_NA
} CStringReqStrcpy_Post_Result;

typedef enum {
  CStringReqStrcpy_Post_Dst_Copy,
  CStringReqStrcpy_Post_Dst_NA
} CStringReqStrcpy_Post_Dst;

typedef enum {
  CStringReqStrcpy_Post_Src_Nop,
  CStringReqStrcpy_Post_Src_NA
} CStringReqStrcpy_Post_Src;

typedef enum {
  CStringReqStrcpy_Post_Errno_Nop,
  CStringReqStrcpy_Post_Errno_NA
} CStringReqStrcpy_Post_Errno;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Alignment_NA : 1;
  uint8_t Pre_Length_NA : 1;
  uint8_t Post_Result : 1;
  uint8_t Post_Dst : 1;
  uint8_t Post_Src : 1;
  uint8_t Post_Errno : 1;
} CStringReqStrcpy_Entry;

/**
 * @brief Test context for spec:/c/string/req/strcpy test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `s1` parameter value.
   */
  char *dst;

  /**
   * @brief This member specifies the `s2` parameter value.
   */
  const char *src;

  /**
   * @brief This member specifies the offset of the `s1` parameter value from
   *   the start of the destination buffer.
   */
  size_t dst_offset;

  /**
   * @brief This member specifies the offset of the `s2` parameter value from
   *   the start of the source buffer.
   */
  size_t src_offset;

  /**
   * @brief This member specifies the length of the string referenced by the
   *   `s2` parameter.
   */
  size_t length;

  /**
   * @brief This member provides the destination buffer.
   */
  RTEMS_ALIGNED( 8 ) unsigned char dst_buf[ 8 * sizeof( size_t ) ];

  /**
   * @brief This member provides the source buffer.
   */
  RTEMS_ALIGNED( 8 ) char src_buf[ 8 * sizeof( size_t ) ];

  /**
   * @brief This member contains the content of the source buffer before the
   *   strcpy() call.
   */
  char src_before[ 8 * sizeof( size_t ) ];

  /**
   * @brief This member contains the return value of the strcpy() call.
   */
  char *retval;

  /**
   * @brief This member contains the value of errno after the strcpy() call.
   */
  int errno_value;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 2 ];

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
    CStringReqStrcpy_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringReqStrcpy_Context;

static CStringReqStrcpy_Context CStringReqStrcpy_Instance;

static const char *const CStringReqStrcpy_PreDesc_Alignment[] =
  { "Aligned", "Unaligned", "Mismatch", "NA" };

static const char *const CStringReqStrcpy_PreDesc_Length[] =
  { "Short", "Medium", "Long", "NA" };

static const char *const *const CStringReqStrcpy_PreDesc[] = {
  CStringReqStrcpy_PreDesc_Alignment,
  CStringReqStrcpy_PreDesc_Length,
  NULL
};

typedef CStringReqStrcpy_Context Context;

static void Arrange( Context *ctx )
{
  char  *src;
  size_t i;

  src = ctx->src_buf + ctx->src_offset;

  for ( i = 0; i < ctx->length; ++i ) {
    src[ i ] = (char) ( 'A' + i % 26 );
  }

  src[ ctx->length ] = '\0';
  src[ ctx->length + 1 ] = 'Z';
  ctx->src = src;
  ctx->dst = (char *) ( ctx->dst_buf + ctx->dst_offset );
}

static void CStringReqStrcpy_Pre_Alignment_Prepare(
  CStringReqStrcpy_Context      *ctx,
  CStringReqStrcpy_Pre_Alignment state
)
{
  switch ( state ) {
    case CStringReqStrcpy_Pre_Alignment_Aligned: {
      /*
       * While the `s1` and `s2` parameters are aligned to the size of type
       * size_t.
       */
      ctx->src_offset = 0;
      ctx->dst_offset = 0;
      break;
    }

    case CStringReqStrcpy_Pre_Alignment_Unaligned: {
      /*
       * While the `s1` and `s2` parameters are not aligned to the size of type
       * size_t, while the `s1` and `s2` parameters have the same remainder of
       * the division by the size of type size_t.
       */
      ctx->src_offset = 1;
      ctx->dst_offset = 1;
      break;
    }

    case CStringReqStrcpy_Pre_Alignment_Mismatch: {
      /*
       * While the `s1` and `s2` parameters have different remainders of the
       * division by the size of type size_t.
       */
      ctx->src_offset = 1;
      ctx->dst_offset = 2;
      break;
    }

    case CStringReqStrcpy_Pre_Alignment_NA:
      break;
  }
}

static void CStringReqStrcpy_Pre_Length_Prepare(
  CStringReqStrcpy_Context   *ctx,
  CStringReqStrcpy_Pre_Length state
)
{
  switch ( state ) {
    case CStringReqStrcpy_Pre_Length_Short: {
      /*
       * While the length of the string referenced by the `s2` parameter is
       * less than the size of type size_t minus one.
       */
      ctx->length = 1;
      break;
    }

    case CStringReqStrcpy_Pre_Length_Medium: {
      /*
       * While the length of the string referenced by the `s2` parameter is
       * greater than or equal to the size of type size_t minus one, while the
       * length of the string referenced by the `s2` parameter is less than two
       * times the size of type size_t.
       */
      ctx->length = sizeof( size_t ) - 1;
      break;
    }

    case CStringReqStrcpy_Pre_Length_Long: {
      /*
       * While the length of the string referenced by the `s2` parameter is
       * greater than or equal to two times the size of type size_t.
       */
      ctx->length = 2 * sizeof( size_t ) + sizeof( size_t ) / 2;
      break;
    }

    case CStringReqStrcpy_Pre_Length_NA:
      break;
  }
}

static void CStringReqStrcpy_Post_Result_Check(
  CStringReqStrcpy_Context    *ctx,
  CStringReqStrcpy_Post_Result state
)
{
  switch ( state ) {
    case CStringReqStrcpy_Post_Result_S1: {
      /*
       * The return value of strcpy() shall be the value of the `s1` parameter.
       */
      T_eq_ptr( ctx->retval, ctx->dst );
      break;
    }

    case CStringReqStrcpy_Post_Result_NA:
      break;
  }
}

static void CStringReqStrcpy_Post_Dst_Check(
  CStringReqStrcpy_Context *ctx,
  CStringReqStrcpy_Post_Dst state
)
{
  switch ( state ) {
    case CStringReqStrcpy_Post_Dst_Copy: {
      /*
       * Only the string referenced by the `s2` parameter, including its
       * terminating null character, shall be copied to the array referenced by
       * the `s1` parameter.
       */
      size_t i;

      T_eq_nstr( ctx->dst, ctx->src, ctx->length + 1 );

      for ( i = 0; i < ctx->dst_offset; ++i ) {
        T_quiet_eq_u8( ctx->dst_buf[ i ], 0xaa );
      }

      for (
        i = ctx->dst_offset + ctx->length + 1; i < sizeof( ctx->dst_buf ); ++i
      ) {
        T_quiet_eq_u8( ctx->dst_buf[ i ], 0xaa );
      }
      break;
    }

    case CStringReqStrcpy_Post_Dst_NA:
      break;
  }
}

static void CStringReqStrcpy_Post_Src_Check(
  CStringReqStrcpy_Context *ctx,
  CStringReqStrcpy_Post_Src state
)
{
  switch ( state ) {
    case CStringReqStrcpy_Post_Src_Nop: {
      /*
       * The bytes of the string referenced by the `s2` parameter shall not be
       * modified.
       */
      size_t i;

      for ( i = 0; i < sizeof( ctx->src_buf ); ++i ) {
        T_quiet_eq_u8( ctx->src_buf[ i ], ctx->src_before[ i ] );
      }
      break;
    }

    case CStringReqStrcpy_Post_Src_NA:
      break;
  }
}

static void CStringReqStrcpy_Post_Errno_Check(
  CStringReqStrcpy_Context   *ctx,
  CStringReqStrcpy_Post_Errno state
)
{
  switch ( state ) {
    case CStringReqStrcpy_Post_Errno_Nop: {
      /*
       * The value of errno shall not be modified.
       */
      T_eq_int( ctx->errno_value, -1 );
      break;
    }

    case CStringReqStrcpy_Post_Errno_NA:
      break;
  }
}

static void CStringReqStrcpy_Prepare( CStringReqStrcpy_Context *ctx )
{
  memset( ctx->dst_buf, 0xaa, sizeof( ctx->dst_buf ) );
  memset( ctx->src_buf, 0, sizeof( ctx->src_buf ) );
  ctx->dst_offset = 0;
  ctx->src_offset = 0;
  ctx->length = 0;
  ctx->dst = (char *) ctx->dst_buf;
  ctx->src = ctx->src_buf;
  ctx->retval = NULL;
}

static void CStringReqStrcpy_Action( CStringReqStrcpy_Context *ctx )
{
  Arrange( ctx );
  memcpy( ctx->src_before, ctx->src_buf, sizeof( ctx->src_buf ) );
  errno = -1;
  ctx->retval = strcpy( ctx->dst, ctx->src );
  ctx->errno_value = errno;
}

/* clang-format off */

static const CStringReqStrcpy_Entry
CStringReqStrcpy_Entries[] = {
  { 0, 0, 0, CStringReqStrcpy_Post_Result_S1, CStringReqStrcpy_Post_Dst_Copy,
    CStringReqStrcpy_Post_Src_Nop, CStringReqStrcpy_Post_Errno_Nop }
};

static const uint8_t
CStringReqStrcpy_Map[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t CStringReqStrcpy_Scope( void *arg, char *buf, size_t n )
{
  CStringReqStrcpy_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStringReqStrcpy_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStringReqStrcpy_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringReqStrcpy_Scope,
  .initial_context = &CStringReqStrcpy_Instance
};

static inline CStringReqStrcpy_Entry CStringReqStrcpy_PopEntry(
  CStringReqStrcpy_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringReqStrcpy_Entries[ CStringReqStrcpy_Map[ index ] ];
}

static void CStringReqStrcpy_TestVariant( CStringReqStrcpy_Context *ctx )
{
  CStringReqStrcpy_Pre_Alignment_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringReqStrcpy_Pre_Length_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CStringReqStrcpy_Action( ctx );
  CStringReqStrcpy_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
  CStringReqStrcpy_Post_Dst_Check( ctx, ctx->Map.entry.Post_Dst );
  CStringReqStrcpy_Post_Src_Check( ctx, ctx->Map.entry.Post_Src );
  CStringReqStrcpy_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
}

/**
 * @fn void T_case_body_CStringReqStrcpy( void )
 */
T_TEST_CASE_FIXTURE( CStringReqStrcpy, &CStringReqStrcpy_Fixture )
{
  CStringReqStrcpy_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CStringReqStrcpy_Pre_Alignment_Aligned;
    ctx->Map.pcs[ 0 ] < CStringReqStrcpy_Pre_Alignment_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CStringReqStrcpy_Pre_Length_Short;
      ctx->Map.pcs[ 1 ] < CStringReqStrcpy_Pre_Length_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CStringReqStrcpy_PopEntry( ctx );
      CStringReqStrcpy_Prepare( ctx );
      CStringReqStrcpy_TestVariant( ctx );
    }
  }
}

/** @} */
