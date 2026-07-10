/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringReqStrpbrk
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
 * @defgroup CStringReqStrpbrk spec:/c/string/req/strpbrk
 *
 * @{
 */

typedef enum {
  CStringReqStrpbrk_Pre_S1_Empty,
  CStringReqStrpbrk_Pre_S1_NonEmpty,
  CStringReqStrpbrk_Pre_S1_NA
} CStringReqStrpbrk_Pre_S1;

typedef enum {
  CStringReqStrpbrk_Pre_S2_Empty,
  CStringReqStrpbrk_Pre_S2_NonEmpty,
  CStringReqStrpbrk_Pre_S2_NA
} CStringReqStrpbrk_Pre_S2;

typedef enum {
  CStringReqStrpbrk_Pre_Match_Found,
  CStringReqStrpbrk_Pre_Match_NotFound,
  CStringReqStrpbrk_Pre_Match_NA
} CStringReqStrpbrk_Pre_Match;

typedef enum {
  CStringReqStrpbrk_Pre_MatchPosition_First,
  CStringReqStrpbrk_Pre_MatchPosition_S1Later,
  CStringReqStrpbrk_Pre_MatchPosition_S2Later,
  CStringReqStrpbrk_Pre_MatchPosition_NA
} CStringReqStrpbrk_Pre_MatchPosition;

typedef enum {
  CStringReqStrpbrk_Post_Result_Found,
  CStringReqStrpbrk_Post_Result_Null,
  CStringReqStrpbrk_Post_Result_NA
} CStringReqStrpbrk_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_S1_NA : 1;
  uint8_t Pre_S2_NA : 1;
  uint8_t Pre_Match_NA : 1;
  uint8_t Pre_MatchPosition_NA : 1;
  uint8_t Post_Result : 2;
} CStringReqStrpbrk_Entry;

/**
 * @brief Test context for spec:/c/string/req/strpbrk test case.
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
  char *retval;

  /**
   * @brief This member contains the index of the first matching character
   *   within the source buffer.
   */
  size_t match_pos;

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
    CStringReqStrpbrk_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringReqStrpbrk_Context;

static CStringReqStrpbrk_Context
  CStringReqStrpbrk_Instance;

static const char * const CStringReqStrpbrk_PreDesc_S1[] = {
  "Empty",
  "NonEmpty",
  "NA"
};

static const char * const CStringReqStrpbrk_PreDesc_S2[] = {
  "Empty",
  "NonEmpty",
  "NA"
};

static const char * const CStringReqStrpbrk_PreDesc_Match[] = {
  "Found",
  "NotFound",
  "NA"
};

static const char * const CStringReqStrpbrk_PreDesc_MatchPosition[] = {
  "First",
  "S1Later",
  "S2Later",
  "NA"
};

static const char * const * const CStringReqStrpbrk_PreDesc[] = {
  CStringReqStrpbrk_PreDesc_S1,
  CStringReqStrpbrk_PreDesc_S2,
  CStringReqStrpbrk_PreDesc_Match,
  CStringReqStrpbrk_PreDesc_MatchPosition,
  NULL
};

static void CStringReqStrpbrk_Pre_S1_Prepare(
  CStringReqStrpbrk_Context *ctx,
  CStringReqStrpbrk_Pre_S1   state
)
{
  switch ( state ) {
    case CStringReqStrpbrk_Pre_S1_Empty: {
      /*
       * While the string referenced by ``s1`` is empty.
       */
      ctx->s1_buf[ 0 ] = '\0';
      break;
    }

    case CStringReqStrpbrk_Pre_S1_NonEmpty: {
      /*
       * While the string referenced by ``s1`` is not empty.
       */
      strcpy( ctx->s1_buf, "XXXXX" );
      break;
    }

    case CStringReqStrpbrk_Pre_S1_NA:
      break;
  }
}

static void CStringReqStrpbrk_Pre_S2_Prepare(
  CStringReqStrpbrk_Context *ctx,
  CStringReqStrpbrk_Pre_S2   state
)
{
  switch ( state ) {
    case CStringReqStrpbrk_Pre_S2_Empty: {
      /*
       * While the string referenced by ``s2`` is empty.
       */
      ctx->s2_buf[ 0 ] = '\0';
      break;
    }

    case CStringReqStrpbrk_Pre_S2_NonEmpty: {
      /*
       * While the string referenced by ``s2`` is not empty.
       */
      strcpy( ctx->s2_buf, "ZZZ" );
      break;
    }

    case CStringReqStrpbrk_Pre_S2_NA:
      break;
  }
}

static void CStringReqStrpbrk_Pre_Match_Prepare(
  CStringReqStrpbrk_Context  *ctx,
  CStringReqStrpbrk_Pre_Match state
)
{
  switch ( state ) {
    case CStringReqStrpbrk_Pre_Match_Found: {
      /*
       * While a character of the string referenced by ``s1`` occurs in the
       * string referenced by ``s2``.
       */
      ctx->s1_buf[ 0 ] = 'Y';
      ctx->s2_buf[ 0 ] = 'Y';
      ctx->match_pos = 0;
      break;
    }

    case CStringReqStrpbrk_Pre_Match_NotFound: {
      /*
       * While no character of the string referenced by ``s1`` occurs in the
       * string referenced by ``s2``.
       */
      /* The default fill values of the buffers do not overlap */
      break;
    }

    case CStringReqStrpbrk_Pre_Match_NA:
      break;
  }
}

static void CStringReqStrpbrk_Pre_MatchPosition_Prepare(
  CStringReqStrpbrk_Context          *ctx,
  CStringReqStrpbrk_Pre_MatchPosition state
)
{
  switch ( state ) {
    case CStringReqStrpbrk_Pre_MatchPosition_First: {
      /*
       * While the first character of the string referenced by ``s1`` matches
       * the first character of the string referenced by ``s2``.
       */
      /* Already set up generically by Match: Found */
      break;
    }

    case CStringReqStrpbrk_Pre_MatchPosition_S1Later: {
      /*
       * While a character other than the first character of the string
       * referenced by ``s1`` is the first character of that string to match
       * the first character of the string referenced by ``s2``.
       */
      ctx->s1_buf[ 0 ] = 'X';
      ctx->s1_buf[ 2 ] = 'Y';
      ctx->match_pos = 2;
      break;
    }

    case CStringReqStrpbrk_Pre_MatchPosition_S2Later: {
      /*
       * While the first character of the string referenced by ``s1`` matches a
       * character other than the first character of the string referenced by
       * ``s2``.
       */
      ctx->s2_buf[ 0 ] = 'Z';
      ctx->s2_buf[ 2 ] = 'Y';
      break;
    }

    case CStringReqStrpbrk_Pre_MatchPosition_NA:
      break;
  }
}

static void CStringReqStrpbrk_Post_Result_Check(
  CStringReqStrpbrk_Context    *ctx,
  CStringReqStrpbrk_Post_Result state
)
{
  switch ( state ) {
    case CStringReqStrpbrk_Post_Result_Found: {
      /*
       * The return value of strpbrk() shall be a pointer to the first
       * character of the string referenced by ``s1`` which occurs in the
       * string referenced by ``s2``.
       */
      T_eq_ptr( ctx->retval, ctx->s1_buf + ctx->match_pos );
      break;
    }

    case CStringReqStrpbrk_Post_Result_Null: {
      /*
       * The return value of strpbrk() shall be equal to NULL.
       */
      T_null( ctx->retval );
      break;
    }

    case CStringReqStrpbrk_Post_Result_NA:
      break;
  }
}

static void CStringReqStrpbrk_Prepare( CStringReqStrpbrk_Context *ctx )
{
  memset( ctx->s1_buf, 0, sizeof( ctx->s1_buf ) );
  memset( ctx->s2_buf, 0, sizeof( ctx->s2_buf ) );
  ctx->s1 = ctx->s1_buf;
  ctx->s2 = ctx->s2_buf;
  ctx->retval = NULL;
  ctx->match_pos = 0;
}

static void CStringReqStrpbrk_Action( CStringReqStrpbrk_Context *ctx )
{
  ctx->retval = strpbrk( ctx->s1, ctx->s2 );
}

static const CStringReqStrpbrk_Entry
CStringReqStrpbrk_Entries[] = {
  { 0, 0, 1, 1, 1, CStringReqStrpbrk_Post_Result_Null },
  { 0, 0, 0, 1, 1, CStringReqStrpbrk_Post_Result_Null },
  { 0, 0, 0, 0, 0, CStringReqStrpbrk_Post_Result_Found },
  { 0, 0, 0, 0, 1, CStringReqStrpbrk_Post_Result_Null }
};

static const uint8_t
CStringReqStrpbrk_Map[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3
};

static size_t CStringReqStrpbrk_Scope( void *arg, char *buf, size_t n )
{
  CStringReqStrpbrk_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStringReqStrpbrk_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStringReqStrpbrk_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringReqStrpbrk_Scope,
  .initial_context = &CStringReqStrpbrk_Instance
};

static inline CStringReqStrpbrk_Entry CStringReqStrpbrk_PopEntry(
  CStringReqStrpbrk_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringReqStrpbrk_Entries[
    CStringReqStrpbrk_Map[ index ]
  ];
}

static void CStringReqStrpbrk_SetPreConditionStates(
  CStringReqStrpbrk_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_S2_NA ) {
    ctx->Map.pcs[ 1 ] = CStringReqStrpbrk_Pre_S2_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_Match_NA ) {
    ctx->Map.pcs[ 2 ] = CStringReqStrpbrk_Pre_Match_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_MatchPosition_NA ) {
    ctx->Map.pcs[ 3 ] = CStringReqStrpbrk_Pre_MatchPosition_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }
}

static void CStringReqStrpbrk_TestVariant( CStringReqStrpbrk_Context *ctx )
{
  CStringReqStrpbrk_Pre_S1_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringReqStrpbrk_Pre_S2_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CStringReqStrpbrk_Pre_Match_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CStringReqStrpbrk_Pre_MatchPosition_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CStringReqStrpbrk_Action( ctx );
  CStringReqStrpbrk_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CStringReqStrpbrk( void )
 */
T_TEST_CASE_FIXTURE( CStringReqStrpbrk, &CStringReqStrpbrk_Fixture )
{
  CStringReqStrpbrk_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CStringReqStrpbrk_Pre_S1_Empty;
    ctx->Map.pci[ 0 ] < CStringReqStrpbrk_Pre_S1_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CStringReqStrpbrk_Pre_S2_Empty;
      ctx->Map.pci[ 1 ] < CStringReqStrpbrk_Pre_S2_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CStringReqStrpbrk_Pre_Match_Found;
        ctx->Map.pci[ 2 ] < CStringReqStrpbrk_Pre_Match_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CStringReqStrpbrk_Pre_MatchPosition_First;
          ctx->Map.pci[ 3 ] < CStringReqStrpbrk_Pre_MatchPosition_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = CStringReqStrpbrk_PopEntry( ctx );
          CStringReqStrpbrk_SetPreConditionStates( ctx );
          CStringReqStrpbrk_Prepare( ctx );
          CStringReqStrpbrk_TestVariant( ctx );
        }
      }
    }
  }
}

/** @} */
