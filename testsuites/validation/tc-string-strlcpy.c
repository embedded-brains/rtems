/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringReqStrlcpy
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

#include <string.h>

#include <rtems/test.h>

/**
 * @defgroup CStringReqStrlcpy spec:/c/string/req/strlcpy
 *
 * @{
 */

typedef enum {
  CStringReqStrlcpy_Pre_N_Zero,
  CStringReqStrlcpy_Pre_N_Positive,
  CStringReqStrlcpy_Pre_N_NA
} CStringReqStrlcpy_Pre_N;

typedef enum {
  CStringReqStrlcpy_Pre_Fit_Fits,
  CStringReqStrlcpy_Pre_Fit_Truncate,
  CStringReqStrlcpy_Pre_Fit_NA
} CStringReqStrlcpy_Pre_Fit;

typedef enum {
  CStringReqStrlcpy_Post_Result_Truncated,
  CStringReqStrlcpy_Post_Result_NotTruncated,
  CStringReqStrlcpy_Post_Result_NA
} CStringReqStrlcpy_Post_Result;

typedef enum {
  CStringReqStrlcpy_Post_DstArea_Unmodified,
  CStringReqStrlcpy_Post_DstArea_CopiedFull,
  CStringReqStrlcpy_Post_DstArea_CopiedTruncated,
  CStringReqStrlcpy_Post_DstArea_NA
} CStringReqStrlcpy_Post_DstArea;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_N_NA : 1;
  uint8_t Pre_Fit_NA : 1;
  uint8_t Post_Result : 2;
  uint8_t Post_DstArea : 2;
} CStringReqStrlcpy_Entry;

/**
 * @brief Test context for spec:/c/string/req/strlcpy test case.
 */
typedef struct {
  /**
   * @brief This member specifies the ``dst`` parameter value.
   */
  char *dst;

  /**
   * @brief This member specifies the ``src`` parameter value.
   */
  const char *src;

  /**
   * @brief This member specifies the ``size`` parameter value.
   */
  size_t size;

  /**
   * @brief This member contains the return value.
   */
  size_t retval;

  /**
   * @brief This member contains the offset of the guarded ``dst`` region from
   *   the start of the destination buffer.
   */
  size_t offset;

  /**
   * @brief This member provides the destination buffer.
   */
  unsigned char dst_buf[ 32 ];

  /**
   * @brief This member provides the source buffer.
   */
  char src_buf[ 16 ];

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 2 ];

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
    CStringReqStrlcpy_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringReqStrlcpy_Context;

static CStringReqStrlcpy_Context
  CStringReqStrlcpy_Instance;

static const char * const CStringReqStrlcpy_PreDesc_N[] = {
  "Zero",
  "Positive",
  "NA"
};

static const char * const CStringReqStrlcpy_PreDesc_Fit[] = {
  "Fits",
  "Truncate",
  "NA"
};

static const char * const * const CStringReqStrlcpy_PreDesc[] = {
  CStringReqStrlcpy_PreDesc_N,
  CStringReqStrlcpy_PreDesc_Fit,
  NULL
};

static void CStringReqStrlcpy_Pre_N_Prepare(
  CStringReqStrlcpy_Context *ctx,
  CStringReqStrlcpy_Pre_N    state
)
{
  switch ( state ) {
    case CStringReqStrlcpy_Pre_N_Zero: {
      /*
       * While the size specified by ``size`` is equal to zero.
       */
      ctx->size = 0;
      break;
    }

    case CStringReqStrlcpy_Pre_N_Positive: {
      /*
       * While the size specified by ``size`` is greater than zero.
       */
      /* The exact positive value is determined by the Fit pre-condition */
      break;
    }

    case CStringReqStrlcpy_Pre_N_NA:
      break;
  }
}

static void CStringReqStrlcpy_Pre_Fit_Prepare(
  CStringReqStrlcpy_Context *ctx,
  CStringReqStrlcpy_Pre_Fit  state
)
{
  switch ( state ) {
    case CStringReqStrlcpy_Pre_Fit_Fits: {
      /*
       * While the length of the string referenced by ``src`` is less than the
       * size specified by ``size``.
       */
      strcpy( ctx->src_buf, "AB" );
      ctx->size = 8;
      break;
    }

    case CStringReqStrlcpy_Pre_Fit_Truncate: {
      /*
       * While the length of the string referenced by ``src`` is greater than
       * or equal to the size specified by ``size``.
       */
      strcpy( ctx->src_buf, "ABCDE" );
      ctx->size = 3;
      break;
    }

    case CStringReqStrlcpy_Pre_Fit_NA:
      break;
  }
}

static void CStringReqStrlcpy_Post_Result_Check(
  CStringReqStrlcpy_Context    *ctx,
  CStringReqStrlcpy_Post_Result state
)
{
  switch ( state ) {
    case CStringReqStrlcpy_Post_Result_Truncated: {
      /*
       * The return value of strlcpy() shall be greater than or equal to the
       * size specified by ``size``.
       */
      T_ge_uint( ctx->retval, ctx->size );
      T_eq_uint( ctx->retval, strlen( ctx->src ) );
      break;
    }

    case CStringReqStrlcpy_Post_Result_NotTruncated: {
      /*
       * The return value of strlcpy() shall be less than the size specified by
       * ``size``.
       */
      T_lt_uint( ctx->retval, ctx->size );
      T_eq_uint( ctx->retval, strlen( ctx->src ) );
      break;
    }

    case CStringReqStrlcpy_Post_Result_NA:
      break;
  }
}

static void CStringReqStrlcpy_Post_DstArea_Check(
  CStringReqStrlcpy_Context     *ctx,
  CStringReqStrlcpy_Post_DstArea state
)
{
  switch ( state ) {
    case CStringReqStrlcpy_Post_DstArea_Unmodified: {
      /*
       * The memory area referenced by ``dst`` shall not be modified.
       */
      for ( size_t i = 0; i < sizeof( ctx->dst_buf ); ++i ) {
        T_quiet_eq_uint( ctx->dst_buf[ i ], 0xaa );
      }
      break;
    }

    case CStringReqStrlcpy_Post_DstArea_CopiedFull: {
      /*
       * The string referenced by ``src``, including its terminating null
       * character, shall be copied to the memory area referenced by ``dst``.
       * The remaining bytes of the memory area referenced by ``dst`` up to the
       * size specified by ``size`` shall not be modified.
       */
      size_t len = strlen( ctx->src );

      T_eq_nstr( ctx->dst, ctx->src, len + 1 );

      for ( size_t i = 0; i < ctx->offset; ++i ) {
        T_quiet_eq_uint( ctx->dst_buf[ i ], 0xaa );
      }

      for (
        size_t i = ctx->offset + len + 1;
        i < sizeof( ctx->dst_buf );
        ++i
      ) {
        T_quiet_eq_uint( ctx->dst_buf[ i ], 0xaa );
      }
      break;
    }

    case CStringReqStrlcpy_Post_DstArea_CopiedTruncated: {
      /*
       * The first bytes of the string referenced by ``src``, up to one less
       * than the size specified by ``size``, shall be copied to the memory
       * area referenced by ``dst``, followed by a terminating null character.
       */
      T_eq_nstr( ctx->dst, ctx->src, ctx->size - 1 );
      T_eq_uint( ctx->dst[ ctx->size - 1 ], 0 );

      for ( size_t i = 0; i < ctx->offset; ++i ) {
        T_quiet_eq_uint( ctx->dst_buf[ i ], 0xaa );
      }

      for (
        size_t i = ctx->offset + ctx->size;
        i < sizeof( ctx->dst_buf );
        ++i
      ) {
        T_quiet_eq_uint( ctx->dst_buf[ i ], 0xaa );
      }
      break;
    }

    case CStringReqStrlcpy_Post_DstArea_NA:
      break;
  }
}

static void CStringReqStrlcpy_Prepare( CStringReqStrlcpy_Context *ctx )
{
  memset( ctx->dst_buf, 0xaa, sizeof( ctx->dst_buf ) );
  memset( ctx->src_buf, 0, sizeof( ctx->src_buf ) );
  ctx->offset = 4;
  ctx->dst = (char *) ( ctx->dst_buf + ctx->offset );
  ctx->src = ctx->src_buf;
  ctx->retval = 0;
  ctx->size = 0;
}

static void CStringReqStrlcpy_Action( CStringReqStrlcpy_Context *ctx )
{
  ctx->retval = strlcpy( ctx->dst, ctx->src, ctx->size );
}

static const CStringReqStrlcpy_Entry
CStringReqStrlcpy_Entries[] = {
  { 0, 0, 1, CStringReqStrlcpy_Post_Result_Truncated,
    CStringReqStrlcpy_Post_DstArea_Unmodified },
  { 0, 0, 0, CStringReqStrlcpy_Post_Result_NotTruncated,
    CStringReqStrlcpy_Post_DstArea_CopiedFull },
  { 0, 0, 0, CStringReqStrlcpy_Post_Result_Truncated,
    CStringReqStrlcpy_Post_DstArea_CopiedTruncated }
};

static const uint8_t
CStringReqStrlcpy_Map[] = {
  0, 0, 1, 2
};

static size_t CStringReqStrlcpy_Scope( void *arg, char *buf, size_t n )
{
  CStringReqStrlcpy_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStringReqStrlcpy_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStringReqStrlcpy_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringReqStrlcpy_Scope,
  .initial_context = &CStringReqStrlcpy_Instance
};

static inline CStringReqStrlcpy_Entry CStringReqStrlcpy_PopEntry(
  CStringReqStrlcpy_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringReqStrlcpy_Entries[
    CStringReqStrlcpy_Map[ index ]
  ];
}

static void CStringReqStrlcpy_SetPreConditionStates(
  CStringReqStrlcpy_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Fit_NA ) {
    ctx->Map.pcs[ 1 ] = CStringReqStrlcpy_Pre_Fit_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }
}

static void CStringReqStrlcpy_TestVariant( CStringReqStrlcpy_Context *ctx )
{
  CStringReqStrlcpy_Pre_N_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringReqStrlcpy_Pre_Fit_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CStringReqStrlcpy_Action( ctx );
  CStringReqStrlcpy_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
  CStringReqStrlcpy_Post_DstArea_Check( ctx, ctx->Map.entry.Post_DstArea );
}

/**
 * @fn void T_case_body_CStringReqStrlcpy( void )
 */
T_TEST_CASE_FIXTURE( CStringReqStrlcpy, &CStringReqStrlcpy_Fixture )
{
  CStringReqStrlcpy_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CStringReqStrlcpy_Pre_N_Zero;
    ctx->Map.pci[ 0 ] < CStringReqStrlcpy_Pre_N_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CStringReqStrlcpy_Pre_Fit_Fits;
      ctx->Map.pci[ 1 ] < CStringReqStrlcpy_Pre_Fit_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      ctx->Map.entry = CStringReqStrlcpy_PopEntry( ctx );
      CStringReqStrlcpy_SetPreConditionStates( ctx );
      CStringReqStrlcpy_Prepare( ctx );
      CStringReqStrlcpy_TestVariant( ctx );
    }
  }
}

/** @} */
