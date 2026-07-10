/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringsReqFfs
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

#include <strings.h>

#include <rtems/test.h>

/**
 * @defgroup CStringsReqFfs spec:/c/strings/req/ffs
 *
 * @{
 */

typedef enum {
  CStringsReqFfs_Pre_I_Zero,
  CStringsReqFfs_Pre_I_NonZero,
  CStringsReqFfs_Pre_I_NA
} CStringsReqFfs_Pre_I;

typedef enum {
  CStringsReqFfs_Post_Result_Zero,
  CStringsReqFfs_Post_Result_Index,
  CStringsReqFfs_Post_Result_NA
} CStringsReqFfs_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_I_NA : 1;
  uint8_t Post_Result : 2;
} CStringsReqFfs_Entry;

/**
 * @brief Test context for spec:/c/strings/req/ffs test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `i` parameter value.
   */
  int i;

  /**
   * @brief This member stores the return value of the ffs() call.
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
    CStringsReqFfs_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringsReqFfs_Context;

static CStringsReqFfs_Context
  CStringsReqFfs_Instance;

static const char * const CStringsReqFfs_PreDesc_I[] = {
  "Zero",
  "NonZero",
  "NA"
};

static const char * const * const CStringsReqFfs_PreDesc[] = {
  CStringsReqFfs_PreDesc_I,
  NULL
};

static void CStringsReqFfs_Pre_I_Prepare(
  CStringsReqFfs_Context *ctx,
  CStringsReqFfs_Pre_I    state
)
{
  switch ( state ) {
    case CStringsReqFfs_Pre_I_Zero: {
      /*
       * While the value specified by `i` is equal to zero.
       */
      ctx->i = 0;
      break;
    }

    case CStringsReqFfs_Pre_I_NonZero: {
      /*
       * While the value specified by `i` is not equal to zero.
       */
      ctx->i = 0x00080100;
      break;
    }

    case CStringsReqFfs_Pre_I_NA:
      break;
  }
}

static void CStringsReqFfs_Post_Result_Check(
  CStringsReqFfs_Context    *ctx,
  CStringsReqFfs_Post_Result state
)
{
  switch ( state ) {
    case CStringsReqFfs_Post_Result_Zero: {
      /*
       * The return value shall be zero.
       */
      T_eq_int( ctx->result, 0 );
      break;
    }

    case CStringsReqFfs_Post_Result_Index: {
      /*
       * The return value shall be the index of the first bit set in the value
       * specified by `i`, where the least significant bit is bit one.
       */
      int expected;
      int i;

      expected = 0;

      for ( i = 0; i < 32; ++i ) {
        if ( ( ctx->i & ( 1 << i ) ) != 0 ) {
          expected = i + 1;
          break;
        }
      }

      T_eq_int( ctx->result, expected );
      break;
    }

    case CStringsReqFfs_Post_Result_NA:
      break;
  }
}

static void CStringsReqFfs_Action( CStringsReqFfs_Context *ctx )
{
  ctx->result = ffs( ctx->i );
}

static const CStringsReqFfs_Entry
CStringsReqFfs_Entries[] = {
  { 0, 0, CStringsReqFfs_Post_Result_Zero },
  { 0, 0, CStringsReqFfs_Post_Result_Index }
};

static const uint8_t
CStringsReqFfs_Map[] = {
  0, 1
};

static size_t CStringsReqFfs_Scope( void *arg, char *buf, size_t n )
{
  CStringsReqFfs_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStringsReqFfs_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStringsReqFfs_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringsReqFfs_Scope,
  .initial_context = &CStringsReqFfs_Instance
};

static inline CStringsReqFfs_Entry CStringsReqFfs_PopEntry(
  CStringsReqFfs_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringsReqFfs_Entries[
    CStringsReqFfs_Map[ index ]
  ];
}

static void CStringsReqFfs_TestVariant( CStringsReqFfs_Context *ctx )
{
  CStringsReqFfs_Pre_I_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringsReqFfs_Action( ctx );
  CStringsReqFfs_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CStringsReqFfs( void )
 */
T_TEST_CASE_FIXTURE( CStringsReqFfs, &CStringsReqFfs_Fixture )
{
  CStringsReqFfs_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CStringsReqFfs_Pre_I_Zero;
    ctx->Map.pcs[ 0 ] < CStringsReqFfs_Pre_I_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CStringsReqFfs_PopEntry( ctx );
    CStringsReqFfs_TestVariant( ctx );
  }
}

/** @} */
