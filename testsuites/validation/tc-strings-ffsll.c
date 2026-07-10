/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringsReqFfsll
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
 * @defgroup CStringsReqFfsll spec:/c/strings/req/ffsll
 *
 * @{
 */

typedef enum {
  CStringsReqFfsll_Pre_I_Zero,
  CStringsReqFfsll_Pre_I_NonZero,
  CStringsReqFfsll_Pre_I_NA
} CStringsReqFfsll_Pre_I;

typedef enum {
  CStringsReqFfsll_Post_Result_Zero,
  CStringsReqFfsll_Post_Result_Index,
  CStringsReqFfsll_Post_Result_NA
} CStringsReqFfsll_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_I_NA : 1;
  uint8_t Post_Result : 2;
} CStringsReqFfsll_Entry;

/**
 * @brief Test context for spec:/c/strings/req/ffsll test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `i` parameter value.
   */
  long long i;

  /**
   * @brief This member stores the return value of the ffsll() call.
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
    CStringsReqFfsll_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringsReqFfsll_Context;

static CStringsReqFfsll_Context
  CStringsReqFfsll_Instance;

static const char * const CStringsReqFfsll_PreDesc_I[] = {
  "Zero",
  "NonZero",
  "NA"
};

static const char * const * const CStringsReqFfsll_PreDesc[] = {
  CStringsReqFfsll_PreDesc_I,
  NULL
};

static void CStringsReqFfsll_Pre_I_Prepare(
  CStringsReqFfsll_Context *ctx,
  CStringsReqFfsll_Pre_I    state
)
{
  switch ( state ) {
    case CStringsReqFfsll_Pre_I_Zero: {
      /*
       * While the value specified by `i` is equal to zero.
       */
      ctx->i = 0;
      break;
    }

    case CStringsReqFfsll_Pre_I_NonZero: {
      /*
       * While the value specified by `i` is not equal to zero.
       */
      ctx->i = 0x0008010000000000LL;
      break;
    }

    case CStringsReqFfsll_Pre_I_NA:
      break;
  }
}

static void CStringsReqFfsll_Post_Result_Check(
  CStringsReqFfsll_Context    *ctx,
  CStringsReqFfsll_Post_Result state
)
{
  switch ( state ) {
    case CStringsReqFfsll_Post_Result_Zero: {
      /*
       * The return value shall be zero.
       */
      T_eq_int( ctx->result, 0 );
      break;
    }

    case CStringsReqFfsll_Post_Result_Index: {
      /*
       * The return value shall be the index of the first bit set in the value
       * specified by `i`, where the least significant bit is bit one.
       */
      int expected;
      int j;

      expected = 0;

      for ( j = 0; j < (int) ( sizeof( long long ) * CHAR_BIT ); ++j ) {
        if ( ( ctx->i & ( 1LL << j ) ) != 0 ) {
          expected = j + 1;
          break;
        }
      }

      T_eq_int( ctx->result, expected );
      break;
    }

    case CStringsReqFfsll_Post_Result_NA:
      break;
  }
}

static void CStringsReqFfsll_Action( CStringsReqFfsll_Context *ctx )
{
  ctx->result = ffsll( ctx->i );
}

static const CStringsReqFfsll_Entry
CStringsReqFfsll_Entries[] = {
  { 0, 0, CStringsReqFfsll_Post_Result_Zero },
  { 0, 0, CStringsReqFfsll_Post_Result_Index }
};

static const uint8_t
CStringsReqFfsll_Map[] = {
  0, 1
};

static size_t CStringsReqFfsll_Scope( void *arg, char *buf, size_t n )
{
  CStringsReqFfsll_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStringsReqFfsll_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStringsReqFfsll_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringsReqFfsll_Scope,
  .initial_context = &CStringsReqFfsll_Instance
};

static inline CStringsReqFfsll_Entry CStringsReqFfsll_PopEntry(
  CStringsReqFfsll_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringsReqFfsll_Entries[
    CStringsReqFfsll_Map[ index ]
  ];
}

static void CStringsReqFfsll_TestVariant( CStringsReqFfsll_Context *ctx )
{
  CStringsReqFfsll_Pre_I_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringsReqFfsll_Action( ctx );
  CStringsReqFfsll_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CStringsReqFfsll( void )
 */
T_TEST_CASE_FIXTURE( CStringsReqFfsll, &CStringsReqFfsll_Fixture )
{
  CStringsReqFfsll_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CStringsReqFfsll_Pre_I_Zero;
    ctx->Map.pcs[ 0 ] < CStringsReqFfsll_Pre_I_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CStringsReqFfsll_PopEntry( ctx );
    CStringsReqFfsll_TestVariant( ctx );
  }
}

/** @} */
