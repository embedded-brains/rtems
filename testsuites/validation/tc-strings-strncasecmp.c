/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringsReqStrncasecmp
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
 * @defgroup CStringsReqStrncasecmp spec:/c/strings/req/strncasecmp
 *
 * @{
 */

typedef enum {
  CStringsReqStrncasecmp_Pre_N_Zero,
  CStringsReqStrncasecmp_Pre_N_Positive,
  CStringsReqStrncasecmp_Pre_N_LongPositive,
  CStringsReqStrncasecmp_Pre_N_NA
} CStringsReqStrncasecmp_Pre_N;

typedef enum {
  CStringsReqStrncasecmp_Pre_Cmp_Less,
  CStringsReqStrncasecmp_Pre_Cmp_Equal,
  CStringsReqStrncasecmp_Pre_Cmp_Greater,
  CStringsReqStrncasecmp_Pre_Cmp_S1Longer,
  CStringsReqStrncasecmp_Pre_Cmp_LimitedByN,
  CStringsReqStrncasecmp_Pre_Cmp_NA
} CStringsReqStrncasecmp_Pre_Cmp;

typedef enum {
  CStringsReqStrncasecmp_Post_Result_Less,
  CStringsReqStrncasecmp_Post_Result_Equal,
  CStringsReqStrncasecmp_Post_Result_Greater,
  CStringsReqStrncasecmp_Post_Result_NA
} CStringsReqStrncasecmp_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_N_NA : 1;
  uint8_t Pre_Cmp_NA : 1;
  uint8_t Post_Result : 2;
} CStringsReqStrncasecmp_Entry;

/**
 * @brief Test context for spec:/c/strings/req/strncasecmp test case.
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
   * @brief This member specifies the `n` parameter value.
   */
  size_t n;

  /**
   * @brief This member contains the return value.
   */
  int retval;

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
    CStringsReqStrncasecmp_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringsReqStrncasecmp_Context;

static CStringsReqStrncasecmp_Context
  CStringsReqStrncasecmp_Instance;

static const char * const CStringsReqStrncasecmp_PreDesc_N[] = {
  "Zero",
  "Positive",
  "LongPositive",
  "NA"
};

static const char * const CStringsReqStrncasecmp_PreDesc_Cmp[] = {
  "Less",
  "Equal",
  "Greater",
  "S1Longer",
  "LimitedByN",
  "NA"
};

static const char * const * const CStringsReqStrncasecmp_PreDesc[] = {
  CStringsReqStrncasecmp_PreDesc_N,
  CStringsReqStrncasecmp_PreDesc_Cmp,
  NULL
};

static void CStringsReqStrncasecmp_Pre_N_Prepare(
  CStringsReqStrncasecmp_Context *ctx,
  CStringsReqStrncasecmp_Pre_N    state
)
{
  switch ( state ) {
    case CStringsReqStrncasecmp_Pre_N_Zero: {
      /*
       * While the value specified by `n` is equal to zero.
       */
      ctx->n = 0;
      break;
    }

    case CStringsReqStrncasecmp_Pre_N_Positive: {
      /*
       * While the value specified by `n` is greater than zero.
       */
      ctx->n = 5;
      break;
    }

    case CStringsReqStrncasecmp_Pre_N_LongPositive: {
      /*
       * While the value specified by `n` is greater than the length of the
       * string specified by `s2`.
       */
      ctx->n = 6;
      break;
    }

    case CStringsReqStrncasecmp_Pre_N_NA:
      break;
  }
}

static void CStringsReqStrncasecmp_Pre_Cmp_Prepare(
  CStringsReqStrncasecmp_Context *ctx,
  CStringsReqStrncasecmp_Pre_Cmp  state
)
{
  switch ( state ) {
    case CStringsReqStrncasecmp_Pre_Cmp_Less: {
      /*
       * While the first n characters of the string specified by `s1` are less
       * than the first n characters of the string specified by `s2` after
       * ignoring differences in case.
       */
      ctx->s1 = "abcde";
      ctx->s2 = "xyzab";
      break;
    }

    case CStringsReqStrncasecmp_Pre_Cmp_Equal: {
      /*
       * While the first n characters of the string specified by `s1` are equal
       * to the first n characters of the string specified by `s2` after
       * ignoring differences in case.
       */
      ctx->s1 = "Hello";
      ctx->s2 = "hello";
      break;
    }

    case CStringsReqStrncasecmp_Pre_Cmp_Greater: {
      /*
       * While the first n characters of the string specified by `s1` are
       * greater than the first n characters of the string specified by `s2`
       * after ignoring differences in case.
       */
      ctx->s1 = "xyzab";
      ctx->s2 = "abcde";
      break;
    }

    case CStringsReqStrncasecmp_Pre_Cmp_S1Longer: {
      /*
       * While the string specified by `s1` is longer than the string specified
       * by `s2`, while both strings share the same prefix for the first n
       * characters after ignoring differences in case.
       */
      ctx->s1 = "Hello!";
      ctx->s2 = "Hello";
      break;
    }

    case CStringsReqStrncasecmp_Pre_Cmp_LimitedByN: {
      /*
       * While the first n characters of the string specified by `s1` are equal
       * to the first n characters of the string specified by `s2` after
       * ignoring differences in case, while the strings differ after the first
       * n characters.
       */
      ctx->s1 = "HelloX";
      ctx->s2 = "HelloY";
      break;
    }

    case CStringsReqStrncasecmp_Pre_Cmp_NA:
      break;
  }
}

static void CStringsReqStrncasecmp_Post_Result_Check(
  CStringsReqStrncasecmp_Context    *ctx,
  CStringsReqStrncasecmp_Post_Result state
)
{
  switch ( state ) {
    case CStringsReqStrncasecmp_Post_Result_Less: {
      /*
       * The return value shall be less than zero.
       */
      T_lt_int( ctx->retval, 0 );
      break;
    }

    case CStringsReqStrncasecmp_Post_Result_Equal: {
      /*
       * The return value shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CStringsReqStrncasecmp_Post_Result_Greater: {
      /*
       * The return value shall be greater than zero.
       */
      T_gt_int( ctx->retval, 0 );
      break;
    }

    case CStringsReqStrncasecmp_Post_Result_NA:
      break;
  }
}

static void CStringsReqStrncasecmp_Prepare(
  CStringsReqStrncasecmp_Context *ctx
)
{
  ctx->s1 = "Hello";
  ctx->s2 = "hello";
}

static void CStringsReqStrncasecmp_Action(
  CStringsReqStrncasecmp_Context *ctx
)
{
  ctx->retval = strncasecmp( ctx->s1, ctx->s2, ctx->n );
}

static const CStringsReqStrncasecmp_Entry
CStringsReqStrncasecmp_Entries[] = {
  { 0, 0, 1, CStringsReqStrncasecmp_Post_Result_Equal },
  { 0, 0, 1, CStringsReqStrncasecmp_Post_Result_Greater },
  { 0, 0, 0, CStringsReqStrncasecmp_Post_Result_Greater },
  { 0, 0, 0, CStringsReqStrncasecmp_Post_Result_Equal },
  { 0, 0, 0, CStringsReqStrncasecmp_Post_Result_Less }
};

static const uint8_t
CStringsReqStrncasecmp_Map[] = {
  0, 0, 0, 0, 0, 4, 3, 2, 2, 3, 1, 1, 1, 2, 1
};

static size_t CStringsReqStrncasecmp_Scope( void *arg, char *buf, size_t n )
{
  CStringsReqStrncasecmp_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStringsReqStrncasecmp_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStringsReqStrncasecmp_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringsReqStrncasecmp_Scope,
  .initial_context = &CStringsReqStrncasecmp_Instance
};

static inline CStringsReqStrncasecmp_Entry CStringsReqStrncasecmp_PopEntry(
  CStringsReqStrncasecmp_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringsReqStrncasecmp_Entries[
    CStringsReqStrncasecmp_Map[ index ]
  ];
}

static void CStringsReqStrncasecmp_SetPreConditionStates(
  CStringsReqStrncasecmp_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Cmp_NA ) {
    ctx->Map.pcs[ 1 ] = CStringsReqStrncasecmp_Pre_Cmp_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }
}

static void CStringsReqStrncasecmp_TestVariant(
  CStringsReqStrncasecmp_Context *ctx
)
{
  CStringsReqStrncasecmp_Pre_N_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringsReqStrncasecmp_Pre_Cmp_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CStringsReqStrncasecmp_Action( ctx );
  CStringsReqStrncasecmp_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CStringsReqStrncasecmp( void )
 */
T_TEST_CASE_FIXTURE( CStringsReqStrncasecmp, &CStringsReqStrncasecmp_Fixture )
{
  CStringsReqStrncasecmp_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CStringsReqStrncasecmp_Pre_N_Zero;
    ctx->Map.pci[ 0 ] < CStringsReqStrncasecmp_Pre_N_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CStringsReqStrncasecmp_Pre_Cmp_Less;
      ctx->Map.pci[ 1 ] < CStringsReqStrncasecmp_Pre_Cmp_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      ctx->Map.entry = CStringsReqStrncasecmp_PopEntry( ctx );
      CStringsReqStrncasecmp_SetPreConditionStates( ctx );
      CStringsReqStrncasecmp_Prepare( ctx );
      CStringsReqStrncasecmp_TestVariant( ctx );
    }
  }
}

/** @} */
