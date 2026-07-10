/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringReqStrcspn
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
 * @defgroup CStringReqStrcspn spec:/c/string/req/strcspn
 *
 * @{
 */

typedef enum {
  CStringReqStrcspn_Pre_S1_Empty,
  CStringReqStrcspn_Pre_S1_NonEmpty,
  CStringReqStrcspn_Pre_S1_NA
} CStringReqStrcspn_Pre_S1;

typedef enum {
  CStringReqStrcspn_Pre_S2_Empty,
  CStringReqStrcspn_Pre_S2_NonEmpty,
  CStringReqStrcspn_Pre_S2_NA
} CStringReqStrcspn_Pre_S2;

typedef enum {
  CStringReqStrcspn_Pre_Match_Found,
  CStringReqStrcspn_Pre_Match_NotFound,
  CStringReqStrcspn_Pre_Match_NA
} CStringReqStrcspn_Pre_Match;

typedef enum {
  CStringReqStrcspn_Pre_MatchPosition_First,
  CStringReqStrcspn_Pre_MatchPosition_S1Later,
  CStringReqStrcspn_Pre_MatchPosition_S2Later,
  CStringReqStrcspn_Pre_MatchPosition_NA
} CStringReqStrcspn_Pre_MatchPosition;

typedef enum {
  CStringReqStrcspn_Post_Result_Zero,
  CStringReqStrcspn_Post_Result_Partial,
  CStringReqStrcspn_Post_Result_Full,
  CStringReqStrcspn_Post_Result_NA
} CStringReqStrcspn_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_S1_NA : 1;
  uint8_t Pre_S2_NA : 1;
  uint8_t Pre_Match_NA : 1;
  uint8_t Pre_MatchPosition_NA : 1;
  uint8_t Post_Result : 2;
} CStringReqStrcspn_Entry;

/**
 * @brief Test context for spec:/c/string/req/strcspn test case.
 */
typedef struct {
  /**
   * @brief This member specifies the ``s1`` parameter value.
   */
  const char *s1;

  /**
   * @brief This member specifies the ``s2`` parameter value.
   */
  const char *s2;

  /**
   * @brief This member contains the return value.
   */
  size_t retval;

  /**
   * @brief This member contains the expected return value for the Partial
   *   post-condition state.
   */
  size_t expected_len;

  /**
   * @brief This member provides the ``s1`` buffer.
   */
  char s1_buf[ 32 ];

  /**
   * @brief This member provides the ``s2`` buffer.
   */
  char s2_buf[ 16 ];

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 4 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 4 ];

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
    CStringReqStrcspn_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringReqStrcspn_Context;

static CStringReqStrcspn_Context
  CStringReqStrcspn_Instance;

static const char * const CStringReqStrcspn_PreDesc_S1[] = {
  "Empty",
  "NonEmpty",
  "NA"
};

static const char * const CStringReqStrcspn_PreDesc_S2[] = {
  "Empty",
  "NonEmpty",
  "NA"
};

static const char * const CStringReqStrcspn_PreDesc_Match[] = {
  "Found",
  "NotFound",
  "NA"
};

static const char * const CStringReqStrcspn_PreDesc_MatchPosition[] = {
  "First",
  "S1Later",
  "S2Later",
  "NA"
};

static const char * const * const CStringReqStrcspn_PreDesc[] = {
  CStringReqStrcspn_PreDesc_S1,
  CStringReqStrcspn_PreDesc_S2,
  CStringReqStrcspn_PreDesc_Match,
  CStringReqStrcspn_PreDesc_MatchPosition,
  NULL
};

static void CStringReqStrcspn_Pre_S1_Prepare(
  CStringReqStrcspn_Context *ctx,
  CStringReqStrcspn_Pre_S1   state
)
{
  switch ( state ) {
    case CStringReqStrcspn_Pre_S1_Empty: {
      /*
       * While the string referenced by ``s1`` is empty.
       */
      ctx->s1_buf[ 0 ] = '\0';
      break;
    }

    case CStringReqStrcspn_Pre_S1_NonEmpty: {
      /*
       * While the string referenced by ``s1`` is not empty.
       */
      strcpy( ctx->s1_buf, "XXXXX" );
      break;
    }

    case CStringReqStrcspn_Pre_S1_NA:
      break;
  }
}

static void CStringReqStrcspn_Pre_S2_Prepare(
  CStringReqStrcspn_Context *ctx,
  CStringReqStrcspn_Pre_S2   state
)
{
  switch ( state ) {
    case CStringReqStrcspn_Pre_S2_Empty: {
      /*
       * While the string referenced by ``s2`` is empty.
       */
      ctx->s2_buf[ 0 ] = '\0';
      break;
    }

    case CStringReqStrcspn_Pre_S2_NonEmpty: {
      /*
       * While the string referenced by ``s2`` is not empty.
       */
      strcpy( ctx->s2_buf, "ZZZ" );
      break;
    }

    case CStringReqStrcspn_Pre_S2_NA:
      break;
  }
}

static void CStringReqStrcspn_Pre_Match_Prepare(
  CStringReqStrcspn_Context  *ctx,
  CStringReqStrcspn_Pre_Match state
)
{
  switch ( state ) {
    case CStringReqStrcspn_Pre_Match_Found: {
      /*
       * While a character of the string referenced by ``s1`` occurs in the
       * string referenced by ``s2``.
       */
      ctx->s1_buf[ 0 ] = 'Y';
      ctx->s2_buf[ 0 ] = 'Y';
      break;
    }

    case CStringReqStrcspn_Pre_Match_NotFound: {
      /*
       * While no character of the string referenced by ``s1`` occurs in the
       * string referenced by ``s2``.
       */
      /* The default fill values of the buffers do not overlap */
      break;
    }

    case CStringReqStrcspn_Pre_Match_NA:
      break;
  }
}

static void CStringReqStrcspn_Pre_MatchPosition_Prepare(
  CStringReqStrcspn_Context          *ctx,
  CStringReqStrcspn_Pre_MatchPosition state
)
{
  switch ( state ) {
    case CStringReqStrcspn_Pre_MatchPosition_First: {
      /*
       * While the first character of the string referenced by ``s1`` matches
       * the first character of the string referenced by ``s2``.
       */
      /* Already set up generically by Match: Found */
      break;
    }

    case CStringReqStrcspn_Pre_MatchPosition_S1Later: {
      /*
       * While a character other than the first character of the string
       * referenced by ``s1`` is the first character of that string to match
       * the first character of the string referenced by ``s2``.
       */
      ctx->s1_buf[ 0 ] = 'X';
      ctx->s1_buf[ 2 ] = 'Y';
      ctx->expected_len = 2;
      break;
    }

    case CStringReqStrcspn_Pre_MatchPosition_S2Later: {
      /*
       * While the first character of the string referenced by ``s1`` matches a
       * character other than the first character of the string referenced by
       * ``s2``.
       */
      ctx->s2_buf[ 0 ] = 'Z';
      ctx->s2_buf[ 2 ] = 'Y';
      break;
    }

    case CStringReqStrcspn_Pre_MatchPosition_NA:
      break;
  }
}

static void CStringReqStrcspn_Post_Result_Check(
  CStringReqStrcspn_Context    *ctx,
  CStringReqStrcspn_Post_Result state
)
{
  switch ( state ) {
    case CStringReqStrcspn_Post_Result_Zero: {
      /*
       * The return value of strcspn() shall be equal to zero.
       */
      T_eq_uint( ctx->retval, 0 );
      break;
    }

    case CStringReqStrcspn_Post_Result_Partial: {
      /*
       * The return value of strcspn() shall be greater than zero, while the
       * return value of strcspn() is less than the length of the string
       * referenced by ``s1``.
       */
      T_eq_uint( ctx->retval, ctx->expected_len );
      T_gt_uint( ctx->retval, 0 );
      T_lt_uint( ctx->retval, strlen( ctx->s1 ) );
      break;
    }

    case CStringReqStrcspn_Post_Result_Full: {
      /*
       * The return value of strcspn() shall be equal to the length of the
       * string referenced by ``s1``.
       */
      T_eq_uint( ctx->retval, strlen( ctx->s1 ) );
      break;
    }

    case CStringReqStrcspn_Post_Result_NA:
      break;
  }
}

static void CStringReqStrcspn_Prepare( CStringReqStrcspn_Context *ctx )
{
  memset( ctx->s1_buf, 0, sizeof( ctx->s1_buf ) );
  memset( ctx->s2_buf, 0, sizeof( ctx->s2_buf ) );
  ctx->s1 = ctx->s1_buf;
  ctx->s2 = ctx->s2_buf;
  ctx->retval = 0;
  ctx->expected_len = 0;
}

static void CStringReqStrcspn_Action( CStringReqStrcspn_Context *ctx )
{
  ctx->retval = strcspn( ctx->s1, ctx->s2 );
}

static const CStringReqStrcspn_Entry
CStringReqStrcspn_Entries[] = {
  { 0, 0, 1, 1, 1, CStringReqStrcspn_Post_Result_Zero },
  { 0, 0, 0, 1, 1, CStringReqStrcspn_Post_Result_Full },
  { 0, 0, 0, 0, 1, CStringReqStrcspn_Post_Result_Full },
  { 0, 0, 0, 0, 0, CStringReqStrcspn_Post_Result_Zero },
  { 0, 0, 0, 0, 0, CStringReqStrcspn_Post_Result_Partial }
};

static const uint8_t
CStringReqStrcspn_Map[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 3, 4, 3, 2, 2, 2
};

static size_t CStringReqStrcspn_Scope( void *arg, char *buf, size_t n )
{
  CStringReqStrcspn_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStringReqStrcspn_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStringReqStrcspn_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringReqStrcspn_Scope,
  .initial_context = &CStringReqStrcspn_Instance
};

static inline CStringReqStrcspn_Entry CStringReqStrcspn_PopEntry(
  CStringReqStrcspn_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringReqStrcspn_Entries[
    CStringReqStrcspn_Map[ index ]
  ];
}

static void CStringReqStrcspn_SetPreConditionStates(
  CStringReqStrcspn_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_S2_NA ) {
    ctx->Map.pcs[ 1 ] = CStringReqStrcspn_Pre_S2_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_Match_NA ) {
    ctx->Map.pcs[ 2 ] = CStringReqStrcspn_Pre_Match_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_MatchPosition_NA ) {
    ctx->Map.pcs[ 3 ] = CStringReqStrcspn_Pre_MatchPosition_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }
}

static void CStringReqStrcspn_TestVariant( CStringReqStrcspn_Context *ctx )
{
  CStringReqStrcspn_Pre_S1_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringReqStrcspn_Pre_S2_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CStringReqStrcspn_Pre_Match_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CStringReqStrcspn_Pre_MatchPosition_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CStringReqStrcspn_Action( ctx );
  CStringReqStrcspn_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CStringReqStrcspn( void )
 */
T_TEST_CASE_FIXTURE( CStringReqStrcspn, &CStringReqStrcspn_Fixture )
{
  CStringReqStrcspn_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CStringReqStrcspn_Pre_S1_Empty;
    ctx->Map.pci[ 0 ] < CStringReqStrcspn_Pre_S1_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CStringReqStrcspn_Pre_S2_Empty;
      ctx->Map.pci[ 1 ] < CStringReqStrcspn_Pre_S2_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CStringReqStrcspn_Pre_Match_Found;
        ctx->Map.pci[ 2 ] < CStringReqStrcspn_Pre_Match_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CStringReqStrcspn_Pre_MatchPosition_First;
          ctx->Map.pci[ 3 ] < CStringReqStrcspn_Pre_MatchPosition_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = CStringReqStrcspn_PopEntry( ctx );
          CStringReqStrcspn_SetPreConditionStates( ctx );
          CStringReqStrcspn_Prepare( ctx );
          CStringReqStrcspn_TestVariant( ctx );
        }
      }
    }
  }
}

/** @} */
