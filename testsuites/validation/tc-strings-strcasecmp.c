/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringsReqStrcasecmp
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
 * @defgroup CStringsReqStrcasecmp spec:/c/strings/req/strcasecmp
 *
 * @{
 */

typedef enum {
  CStringsReqStrcasecmp_Pre_Cmp_Less,
  CStringsReqStrcasecmp_Pre_Cmp_Equal,
  CStringsReqStrcasecmp_Pre_Cmp_Greater,
  CStringsReqStrcasecmp_Pre_Cmp_S1Longer,
  CStringsReqStrcasecmp_Pre_Cmp_BothEmpty,
  CStringsReqStrcasecmp_Pre_Cmp_NA
} CStringsReqStrcasecmp_Pre_Cmp;

typedef enum {
  CStringsReqStrcasecmp_Post_Result_Less,
  CStringsReqStrcasecmp_Post_Result_Equal,
  CStringsReqStrcasecmp_Post_Result_Greater,
  CStringsReqStrcasecmp_Post_Result_NA
} CStringsReqStrcasecmp_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Cmp_NA : 1;
  uint8_t Post_Result : 2;
} CStringsReqStrcasecmp_Entry;

/**
 * @brief Test context for spec:/c/strings/req/strcasecmp test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `s1` parameter value.
   */
  const char *s1;

  /**
   * @brief This member specifies the `s2` parameter value.
   */
  const char *s2;

  /**
   * @brief This member contains the return value.
   */
  int retval;

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
    CStringsReqStrcasecmp_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringsReqStrcasecmp_Context;

static CStringsReqStrcasecmp_Context
  CStringsReqStrcasecmp_Instance;

static const char * const CStringsReqStrcasecmp_PreDesc_Cmp[] = {
  "Less",
  "Equal",
  "Greater",
  "S1Longer",
  "BothEmpty",
  "NA"
};

static const char * const * const CStringsReqStrcasecmp_PreDesc[] = {
  CStringsReqStrcasecmp_PreDesc_Cmp,
  NULL
};

static void CStringsReqStrcasecmp_Pre_Cmp_Prepare(
  CStringsReqStrcasecmp_Context *ctx,
  CStringsReqStrcasecmp_Pre_Cmp  state
)
{
  switch ( state ) {
    case CStringsReqStrcasecmp_Pre_Cmp_Less: {
      /*
       * While the string specified by `s1` is less than the string specified
       * by `s2` after ignoring differences in case.
       */
      ctx->s1 = "abc";
      ctx->s2 = "xyz";
      break;
    }

    case CStringsReqStrcasecmp_Pre_Cmp_Equal: {
      /*
       * While the string specified by `s1` is equal to the string specified by
       * `s2` after ignoring differences in case.
       */
      ctx->s1 = "Hello";
      ctx->s2 = "hello";
      break;
    }

    case CStringsReqStrcasecmp_Pre_Cmp_Greater: {
      /*
       * While the string specified by `s1` is greater than the string
       * specified by `s2` after ignoring differences in case.
       */
      ctx->s1 = "xyz";
      ctx->s2 = "abc";
      break;
    }

    case CStringsReqStrcasecmp_Pre_Cmp_S1Longer: {
      /*
       * While the string specified by `s1` is longer than the string specified
       * by `s2`, while both strings share the same prefix after ignoring
       * differences in case.
       */
      ctx->s1 = "Hello!";
      ctx->s2 = "Hello";
      break;
    }

    case CStringsReqStrcasecmp_Pre_Cmp_BothEmpty: {
      /*
       * While both the string specified by `s1` and the string specified by
       * `s2` are empty.
       */
      ctx->s1 = "";
      ctx->s2 = "";
      break;
    }

    case CStringsReqStrcasecmp_Pre_Cmp_NA:
      break;
  }
}

static void CStringsReqStrcasecmp_Post_Result_Check(
  CStringsReqStrcasecmp_Context    *ctx,
  CStringsReqStrcasecmp_Post_Result state
)
{
  switch ( state ) {
    case CStringsReqStrcasecmp_Post_Result_Less: {
      /*
       * The return value shall be less than zero.
       */
      T_lt_int( ctx->retval, 0 );
      break;
    }

    case CStringsReqStrcasecmp_Post_Result_Equal: {
      /*
       * The return value shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CStringsReqStrcasecmp_Post_Result_Greater: {
      /*
       * The return value shall be greater than zero.
       */
      T_gt_int( ctx->retval, 0 );
      break;
    }

    case CStringsReqStrcasecmp_Post_Result_NA:
      break;
  }
}

static void CStringsReqStrcasecmp_Action( CStringsReqStrcasecmp_Context *ctx )
{
  ctx->retval = strcasecmp( ctx->s1, ctx->s2 );
}

static const CStringsReqStrcasecmp_Entry
CStringsReqStrcasecmp_Entries[] = {
  { 0, 0, CStringsReqStrcasecmp_Post_Result_Equal },
  { 0, 0, CStringsReqStrcasecmp_Post_Result_Greater },
  { 0, 0, CStringsReqStrcasecmp_Post_Result_Less }
};

static const uint8_t
CStringsReqStrcasecmp_Map[] = {
  2, 0, 1, 1, 0
};

static size_t CStringsReqStrcasecmp_Scope( void *arg, char *buf, size_t n )
{
  CStringsReqStrcasecmp_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStringsReqStrcasecmp_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStringsReqStrcasecmp_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringsReqStrcasecmp_Scope,
  .initial_context = &CStringsReqStrcasecmp_Instance
};

static inline CStringsReqStrcasecmp_Entry CStringsReqStrcasecmp_PopEntry(
  CStringsReqStrcasecmp_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringsReqStrcasecmp_Entries[
    CStringsReqStrcasecmp_Map[ index ]
  ];
}

static void CStringsReqStrcasecmp_TestVariant(
  CStringsReqStrcasecmp_Context *ctx
)
{
  CStringsReqStrcasecmp_Pre_Cmp_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringsReqStrcasecmp_Action( ctx );
  CStringsReqStrcasecmp_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CStringsReqStrcasecmp( void )
 */
T_TEST_CASE_FIXTURE( CStringsReqStrcasecmp, &CStringsReqStrcasecmp_Fixture )
{
  CStringsReqStrcasecmp_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CStringsReqStrcasecmp_Pre_Cmp_Less;
    ctx->Map.pcs[ 0 ] < CStringsReqStrcasecmp_Pre_Cmp_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CStringsReqStrcasecmp_PopEntry( ctx );
    CStringsReqStrcasecmp_TestVariant( ctx );
  }
}

/** @} */
