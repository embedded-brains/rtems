/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringsReqFfsl
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

#include <limits.h>
#include <strings.h>

#include <rtems/test.h>

/**
 * @defgroup CStringsReqFfsl spec:/c/strings/req/ffsl
 *
 * @{
 */

typedef enum {
  CStringsReqFfsl_Pre_I_Zero,
  CStringsReqFfsl_Pre_I_NonZero,
  CStringsReqFfsl_Pre_I_NA
} CStringsReqFfsl_Pre_I;

typedef enum {
  CStringsReqFfsl_Post_Result_Zero,
  CStringsReqFfsl_Post_Result_Index,
  CStringsReqFfsl_Post_Result_NA
} CStringsReqFfsl_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_I_NA : 1;
  uint8_t Post_Result : 2;
} CStringsReqFfsl_Entry;

/**
 * @brief Test context for spec:/c/strings/req/ffsl test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `i` parameter value.
   */
  long i;

  /**
   * @brief This member stores the return value of the ffsl() call.
   */
  int result;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 1 ];

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
    CStringsReqFfsl_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringsReqFfsl_Context;

static CStringsReqFfsl_Context
  CStringsReqFfsl_Instance;

static const char * const CStringsReqFfsl_PreDesc_I[] = {
  "Zero",
  "NonZero",
  "NA"
};

static const char * const * const CStringsReqFfsl_PreDesc[] = {
  CStringsReqFfsl_PreDesc_I,
  NULL
};

static void CStringsReqFfsl_Pre_I_Prepare(
  CStringsReqFfsl_Context *ctx,
  CStringsReqFfsl_Pre_I    state
)
{
  switch ( state ) {
    case CStringsReqFfsl_Pre_I_Zero: {
      /*
       * While the value specified by `i` is equal to zero.
       */
      ctx->i = 0;
      break;
    }

    case CStringsReqFfsl_Pre_I_NonZero: {
      /*
       * While the value specified by `i` is not equal to zero.
       */
      ctx->i = 0x00080100L;
      break;
    }

    case CStringsReqFfsl_Pre_I_NA:
      break;
  }
}

static void CStringsReqFfsl_Post_Result_Check(
  CStringsReqFfsl_Context    *ctx,
  CStringsReqFfsl_Post_Result state
)
{
  switch ( state ) {
    case CStringsReqFfsl_Post_Result_Zero: {
      /*
       * The return value shall be zero.
       */
      T_eq_int( ctx->result, 0 );
      break;
    }

    case CStringsReqFfsl_Post_Result_Index: {
      /*
       * The return value shall be the index of the first bit set in the value
       * specified by `i`, where the least significant bit is bit one.
       */
      int expected;
      int j;

      expected = 0;

      for ( j = 0; j < (int) ( sizeof( long ) * CHAR_BIT ); ++j ) {
        if ( ( ctx->i & ( 1L << j ) ) != 0 ) {
          expected = j + 1;
          break;
        }
      }

      T_eq_int( ctx->result, expected );
      break;
    }

    case CStringsReqFfsl_Post_Result_NA:
      break;
  }
}

static void CStringsReqFfsl_Action( CStringsReqFfsl_Context *ctx )
{
  ctx->result = ffsl( ctx->i );
}

static const CStringsReqFfsl_Entry
CStringsReqFfsl_Entries[] = {
  { 0, 0, CStringsReqFfsl_Post_Result_Zero },
  { 0, 0, CStringsReqFfsl_Post_Result_Index }
};

static const uint8_t
CStringsReqFfsl_Map[] = {
  0, 1
};

static size_t CStringsReqFfsl_Scope( void *arg, char *buf, size_t n )
{
  CStringsReqFfsl_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStringsReqFfsl_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStringsReqFfsl_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringsReqFfsl_Scope,
  .initial_context = &CStringsReqFfsl_Instance
};

static inline CStringsReqFfsl_Entry CStringsReqFfsl_PopEntry(
  CStringsReqFfsl_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringsReqFfsl_Entries[
    CStringsReqFfsl_Map[ index ]
  ];
}

static void CStringsReqFfsl_TestVariant( CStringsReqFfsl_Context *ctx )
{
  CStringsReqFfsl_Pre_I_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringsReqFfsl_Action( ctx );
  CStringsReqFfsl_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CStringsReqFfsl( void )
 */
T_TEST_CASE_FIXTURE( CStringsReqFfsl, &CStringsReqFfsl_Fixture )
{
  CStringsReqFfsl_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CStringsReqFfsl_Pre_I_Zero;
    ctx->Map.pcs[ 0 ] < CStringsReqFfsl_Pre_I_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CStringsReqFfsl_PopEntry( ctx );
    CStringsReqFfsl_TestVariant( ctx );
  }
}

/** @} */
