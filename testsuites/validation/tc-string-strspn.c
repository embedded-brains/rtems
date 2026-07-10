/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringReqStrspn
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
 * @defgroup CStringReqStrspn spec:/c/string/req/strspn
 *
 * @{
 */

typedef enum {
  CStringReqStrspn_Pre_S1_Empty,
  CStringReqStrspn_Pre_S1_NotEmpty,
  CStringReqStrspn_Pre_S1_NA
} CStringReqStrspn_Pre_S1;

typedef enum {
  CStringReqStrspn_Pre_S2_Empty,
  CStringReqStrspn_Pre_S2_NotEmpty,
  CStringReqStrspn_Pre_S2_NA
} CStringReqStrspn_Pre_S2;

typedef enum {
  CStringReqStrspn_Pre_Match_None,
  CStringReqStrspn_Pre_Match_Partial,
  CStringReqStrspn_Pre_Match_Full,
  CStringReqStrspn_Pre_Match_NA
} CStringReqStrspn_Pre_Match;

typedef enum {
  CStringReqStrspn_Post_Result_Zero,
  CStringReqStrspn_Post_Result_Partial,
  CStringReqStrspn_Post_Result_Length,
  CStringReqStrspn_Post_Result_NA
} CStringReqStrspn_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_S1_NA : 1;
  uint8_t Pre_S2_NA : 1;
  uint8_t Pre_Match_NA : 1;
  uint8_t Post_Result : 2;
} CStringReqStrspn_Entry;

/**
 * @brief Test context for spec:/c/string/req/strspn test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `s1` parameter value.
   */
  const char * s1;

  /**
   * @brief This member specifies the `s2` parameter value.
   */
  const char * s2;

  /**
   * @brief This member contains the return value.
   */
  size_t retval;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 3 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 3 ];

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
    CStringReqStrspn_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringReqStrspn_Context;

static CStringReqStrspn_Context
  CStringReqStrspn_Instance;

static const char * const CStringReqStrspn_PreDesc_S1[] = {
  "Empty",
  "NotEmpty",
  "NA"
};

static const char * const CStringReqStrspn_PreDesc_S2[] = {
  "Empty",
  "NotEmpty",
  "NA"
};

static const char * const CStringReqStrspn_PreDesc_Match[] = {
  "None",
  "Partial",
  "Full",
  "NA"
};

static const char * const * const CStringReqStrspn_PreDesc[] = {
  CStringReqStrspn_PreDesc_S1,
  CStringReqStrspn_PreDesc_S2,
  CStringReqStrspn_PreDesc_Match,
  NULL
};

static void CStringReqStrspn_Pre_S1_Prepare(
  CStringReqStrspn_Context *ctx,
  CStringReqStrspn_Pre_S1   state
)
{
  switch ( state ) {
    case CStringReqStrspn_Pre_S1_Empty: {
      /*
       * While the length of the string specified by `s1` is equal to zero.
       */
      ctx->s1 = "";
      break;
    }

    case CStringReqStrspn_Pre_S1_NotEmpty: {
      /*
       * While the length of the string specified by `s1` is greater than zero.
       */
      ctx->s1 = "bc";
      break;
    }

    case CStringReqStrspn_Pre_S1_NA:
      break;
  }
}

static void CStringReqStrspn_Pre_S2_Prepare(
  CStringReqStrspn_Context *ctx,
  CStringReqStrspn_Pre_S2   state
)
{
  switch ( state ) {
    case CStringReqStrspn_Pre_S2_Empty: {
      /*
       * While the length of the string specified by `s2` is equal to zero.
       */
      ctx->s2 = "";
      break;
    }

    case CStringReqStrspn_Pre_S2_NotEmpty: {
      /*
       * While the length of the string specified by `s2` is greater than zero.
       */
      ctx->s2 = "ab";
      break;
    }

    case CStringReqStrspn_Pre_S2_NA:
      break;
  }
}

static void CStringReqStrspn_Pre_Match_Prepare(
  CStringReqStrspn_Context  *ctx,
  CStringReqStrspn_Pre_Match state
)
{
  switch ( state ) {
    case CStringReqStrspn_Pre_Match_None: {
      /*
       * While the first character of the string specified by `s1` is not
       * present in the string specified by `s2`.
       */
      ctx->s1 = "c";
      break;
    }

    case CStringReqStrspn_Pre_Match_Partial: {
      /*
       * While the first character of the string specified by `s1` is present
       * in the string specified by `s2`, while the string specified by `s1`
       * contains at least one character not present in the string specified by
       * `s2`.
       */
      ctx->s1 = "bc";
      break;
    }

    case CStringReqStrspn_Pre_Match_Full: {
      /*
       * While all characters of the string specified by `s1` are present in
       * the string specified by `s2`.
       */
      ctx->s1 = "b";
      break;
    }

    case CStringReqStrspn_Pre_Match_NA:
      break;
  }
}

static void CStringReqStrspn_Post_Result_Check(
  CStringReqStrspn_Context    *ctx,
  CStringReqStrspn_Post_Result state
)
{
  switch ( state ) {
    case CStringReqStrspn_Post_Result_Zero: {
      /*
       * The return value shall be equal to zero.
       */
      T_eq_sz( ctx->retval, 0 );
      break;
    }

    case CStringReqStrspn_Post_Result_Partial: {
      /*
       * The return value shall be equal to the number of initial characters of
       * the string specified by `s1` which are present in the string specified
       * by `s2`.
       */
      T_eq_sz( ctx->retval, 1 );
      break;
    }

    case CStringReqStrspn_Post_Result_Length: {
      /*
       * The return value shall be equal to the length of the string specified
       * by `s1`.
       */
      T_eq_sz( ctx->retval, strlen( ctx->s1 ) );
      break;
    }

    case CStringReqStrspn_Post_Result_NA:
      break;
  }
}

static void CStringReqStrspn_Action( CStringReqStrspn_Context *ctx )
{
  ctx->retval = strspn( ctx->s1, ctx->s2 );
}

static const CStringReqStrspn_Entry
CStringReqStrspn_Entries[] = {
  { 0, 0, 0, 1, CStringReqStrspn_Post_Result_Zero },
  { 0, 0, 0, 0, CStringReqStrspn_Post_Result_Zero },
  { 0, 0, 0, 0, CStringReqStrspn_Post_Result_Partial },
  { 0, 0, 0, 0, CStringReqStrspn_Post_Result_Length }
};

static const uint8_t
CStringReqStrspn_Map[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3
};

static size_t CStringReqStrspn_Scope( void *arg, char *buf, size_t n )
{
  CStringReqStrspn_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStringReqStrspn_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStringReqStrspn_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringReqStrspn_Scope,
  .initial_context = &CStringReqStrspn_Instance
};

static inline CStringReqStrspn_Entry CStringReqStrspn_PopEntry(
  CStringReqStrspn_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringReqStrspn_Entries[
    CStringReqStrspn_Map[ index ]
  ];
}

static void CStringReqStrspn_SetPreConditionStates(
  CStringReqStrspn_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];

  if ( ctx->Map.entry.Pre_Match_NA ) {
    ctx->Map.pcs[ 2 ] = CStringReqStrspn_Pre_Match_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }
}

static void CStringReqStrspn_TestVariant( CStringReqStrspn_Context *ctx )
{
  CStringReqStrspn_Pre_S1_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringReqStrspn_Pre_S2_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CStringReqStrspn_Pre_Match_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CStringReqStrspn_Action( ctx );
  CStringReqStrspn_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CStringReqStrspn( void )
 */
T_TEST_CASE_FIXTURE( CStringReqStrspn, &CStringReqStrspn_Fixture )
{
  CStringReqStrspn_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CStringReqStrspn_Pre_S1_Empty;
    ctx->Map.pci[ 0 ] < CStringReqStrspn_Pre_S1_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CStringReqStrspn_Pre_S2_Empty;
      ctx->Map.pci[ 1 ] < CStringReqStrspn_Pre_S2_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CStringReqStrspn_Pre_Match_None;
        ctx->Map.pci[ 2 ] < CStringReqStrspn_Pre_Match_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = CStringReqStrspn_PopEntry( ctx );
        CStringReqStrspn_SetPreConditionStates( ctx );
        CStringReqStrspn_TestVariant( ctx );
      }
    }
  }
}

/** @} */
