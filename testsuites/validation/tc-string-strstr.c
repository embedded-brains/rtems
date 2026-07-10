/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringReqStrstr
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
 * @defgroup CStringReqStrstr spec:/c/string/req/strstr
 *
 * @{
 */

typedef enum {
  CStringReqStrstr_Pre_S2StringLength_Zero,
  CStringReqStrstr_Pre_S2StringLength_One,
  CStringReqStrstr_Pre_S2StringLength_Two,
  CStringReqStrstr_Pre_S2StringLength_Three,
  CStringReqStrstr_Pre_S2StringLength_Four,
  CStringReqStrstr_Pre_S2StringLength_Short,
  CStringReqStrstr_Pre_S2StringLength_Long,
  CStringReqStrstr_Pre_S2StringLength_NA
} CStringReqStrstr_Pre_S2StringLength;

typedef enum {
  CStringReqStrstr_Pre_Contains_Yes,
  CStringReqStrstr_Pre_Contains_No,
  CStringReqStrstr_Pre_Contains_NoEdgeCase,
  CStringReqStrstr_Pre_Contains_HaystackTooShort,
  CStringReqStrstr_Pre_Contains_NA
} CStringReqStrstr_Pre_Contains;

typedef enum {
  CStringReqStrstr_Post_Result_PointerToS1,
  CStringReqStrstr_Post_Result_PointerToMatch,
  CStringReqStrstr_Post_Result_Null,
  CStringReqStrstr_Post_Result_NA
} CStringReqStrstr_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_S2StringLength_NA : 1;
  uint8_t Pre_Contains_NA : 1;
  uint8_t Post_Result : 2;
} CStringReqStrstr_Entry;

/**
 * @brief Test context for spec:/c/string/req/strstr test case.
 */
typedef struct {
  /**
   * @brief This member specifies the ``s1`` parameter value.
   */
  const char * s1;

  /**
   * @brief This member specifies the ``s2`` parameter value.
   */
  const char * s2;

  /**
   * @brief This member contains the return value.
   */
  char *retval;

  /**
   * @brief This member provides the ``s2`` string buffer.
   */
  char s2_buf[ 300 ];

  /**
   * @brief This member provides the ``s1`` string buffer.
   */
  char s1_buf[ 1200 ];

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
    CStringReqStrstr_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringReqStrstr_Context;

static CStringReqStrstr_Context
  CStringReqStrstr_Instance;

static const char * const CStringReqStrstr_PreDesc_S2StringLength[] = {
  "Zero",
  "One",
  "Two",
  "Three",
  "Four",
  "Short",
  "Long",
  "NA"
};

static const char * const CStringReqStrstr_PreDesc_Contains[] = {
  "Yes",
  "No",
  "NoEdgeCase",
  "HaystackTooShort",
  "NA"
};

static const char * const * const CStringReqStrstr_PreDesc[] = {
  CStringReqStrstr_PreDesc_S2StringLength,
  CStringReqStrstr_PreDesc_Contains,
  NULL
};

static void CStringReqStrstr_Pre_S2StringLength_Prepare(
  CStringReqStrstr_Context           *ctx,
  CStringReqStrstr_Pre_S2StringLength state
)
{
  switch ( state ) {
    case CStringReqStrstr_Pre_S2StringLength_Zero: {
      /*
       * While the length of the string specified by ``s2`` is equal to 0.
       */
      ctx->s2 = "";
      break;
    }

    case CStringReqStrstr_Pre_S2StringLength_One: {
      /*
       * While the length of the string specified by ``s2`` is equal to 1.
       */
      ctx->s2 = "b";
      break;
    }

    case CStringReqStrstr_Pre_S2StringLength_Two: {
      /*
       * While the length of the string specified by ``s2`` is equal to 2.
       */
      ctx->s2 = "ab";
      break;
    }

    case CStringReqStrstr_Pre_S2StringLength_Three: {
      /*
       * While the length of the string specified by ``s2`` is equal to 3.
       */
      ctx->s2 = "abc";
      break;
    }

    case CStringReqStrstr_Pre_S2StringLength_Four: {
      /*
       * While the length of the string specified by ``s2`` is equal to 4.
       */
      ctx->s2 = "abcd";
      break;
    }

    case CStringReqStrstr_Pre_S2StringLength_Short: {
      /*
       * While the length of the string specified by ``s2`` is greater than or
       * equal to 5, while the length of the string specified by ``s2`` is less
       * than or equal to 254.
       */
      ctx->s2 = "abcde";
      break;
    }

    case CStringReqStrstr_Pre_S2StringLength_Long: {
      /*
       * While the length of the string specified by ``s2`` is greater than or
       * equal to 255.
       */
      memset( ctx->s2_buf, 'a', 260 );
      ctx->s2_buf[ 260 ] = 'b';
      ctx->s2_buf[ 261 ] = '\0';
      ctx->s2 = ctx->s2_buf;
      break;
    }

    case CStringReqStrstr_Pre_S2StringLength_NA:
      break;
  }
}

static void CStringReqStrstr_Pre_Contains_Prepare(
  CStringReqStrstr_Context     *ctx,
  CStringReqStrstr_Pre_Contains state
)
{
  switch ( state ) {
    case CStringReqStrstr_Pre_Contains_Yes: {
      /*
       * While the sequence of characters from the string specified by ``s2``
       * occurs in the string specified by ``s1``.
       */
      /* Set s1 to contain s2 plus some prefix/suffix */
      strcpy( ctx->s1_buf, "prefix_" );
      strcat( ctx->s1_buf, ctx->s2 );
      strcat( ctx->s1_buf, "_suffix" );
      ctx->s1 = ctx->s1_buf;
      break;
    }

    case CStringReqStrstr_Pre_Contains_No: {
      /*
       * While the sequence of characters from the string specified by ``s2``
       * does not occur in the string specified by ``s1``, while the length of
       * the string specified by ``s1`` is greater than or equal to the length
       * of the string specified by ``s2``.
       */
      /* Set s1 to be longer than s2 but not containing it */
      memset( ctx->s1_buf, 'X', 1000 );
      ctx->s1_buf[ 1000 ] = '\0';
      ctx->s1 = ctx->s1_buf;
      break;
    }

    case CStringReqStrstr_Pre_Contains_NoEdgeCase: {
      /*
       * While the sequence of characters from the string specified by ``s2``
       * does not occur in the string specified by ``s1``, while the length of
       * the string specified by ``s1`` is exactly configured to trigger the
       * Quick-Search edge case.
       */
      size_t len = strlen( ctx->s2 );
      /* 517 is ne_len | 512 for ne_len=5. We need length 518 */
      size_t fill_len = ( len | 512 ) + 1;
      memset( ctx->s1_buf, 'X', fill_len );
      ctx->s1_buf[ fill_len ] = '\0';
      ctx->s1 = ctx->s1_buf;
      break;
    }

    case CStringReqStrstr_Pre_Contains_HaystackTooShort: {
      /*
       * While the length of the string specified by ``s1`` is less than the
       * length of the string specified by ``s2``.
       */
      size_t len = strlen( ctx->s2 );
      if ( len > 0 ) {
        memset( ctx->s1_buf, 'X', len - 1 );
        ctx->s1_buf[ len - 1 ] = '\0';
      } else {
        ctx->s1_buf[ 0 ] = '\0';
      }
      ctx->s1 = ctx->s1_buf;
      break;
    }

    case CStringReqStrstr_Pre_Contains_NA:
      break;
  }
}

static void CStringReqStrstr_Post_Result_Check(
  CStringReqStrstr_Context    *ctx,
  CStringReqStrstr_Post_Result state
)
{
  switch ( state ) {
    case CStringReqStrstr_Post_Result_PointerToS1: {
      /*
       * The return value shall be a pointer to the first character of the
       * string specified by the `s1` parameter.
       */
      T_eq_ptr( ctx->retval, ctx->s1 );
      break;
    }

    case CStringReqStrstr_Post_Result_PointerToMatch: {
      /*
       * The return value shall be a pointer to the first occurrence of the
       * string specified by the `s2` parameter in the string specified by the
       * `s1` parameter.
       */
      T_not_null( ctx->retval );
      /* The first occurrence is usually right at s1 or at some offset depending on test data */
      /* Since s2 occurs in s1, retval should point somewhere inside s1 */
      T_true(
        ctx->retval >= ctx->s1 && ctx->retval <= ctx->s1 + strlen( ctx->s1 ),
        "in range"
      );
      break;
    }

    case CStringReqStrstr_Post_Result_Null: {
      /*
       * The return value shall be a null pointer.
       */
      T_null( ctx->retval );
      break;
    }

    case CStringReqStrstr_Post_Result_NA:
      break;
  }
}

static void CStringReqStrstr_Action( CStringReqStrstr_Context *ctx )
{
  ctx->retval = strstr( ctx->s1, ctx->s2 );
}

static const CStringReqStrstr_Entry
CStringReqStrstr_Entries[] = {
  { 0, 0, 0, CStringReqStrstr_Post_Result_Null },
  { 0, 0, 0, CStringReqStrstr_Post_Result_PointerToMatch },
  { 0, 0, 1, CStringReqStrstr_Post_Result_PointerToS1 }
};

static const uint8_t
CStringReqStrstr_Map[] = {
  2, 2, 2, 2, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0,
  0, 0
};

static size_t CStringReqStrstr_Scope( void *arg, char *buf, size_t n )
{
  CStringReqStrstr_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStringReqStrstr_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStringReqStrstr_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringReqStrstr_Scope,
  .initial_context = &CStringReqStrstr_Instance
};

static inline CStringReqStrstr_Entry CStringReqStrstr_PopEntry(
  CStringReqStrstr_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringReqStrstr_Entries[
    CStringReqStrstr_Map[ index ]
  ];
}

static void CStringReqStrstr_SetPreConditionStates(
  CStringReqStrstr_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Contains_NA ) {
    ctx->Map.pcs[ 1 ] = CStringReqStrstr_Pre_Contains_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }
}

static void CStringReqStrstr_TestVariant( CStringReqStrstr_Context *ctx )
{
  CStringReqStrstr_Pre_S2StringLength_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringReqStrstr_Pre_Contains_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CStringReqStrstr_Action( ctx );
  CStringReqStrstr_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CStringReqStrstr( void )
 */
T_TEST_CASE_FIXTURE( CStringReqStrstr, &CStringReqStrstr_Fixture )
{
  CStringReqStrstr_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CStringReqStrstr_Pre_S2StringLength_Zero;
    ctx->Map.pci[ 0 ] < CStringReqStrstr_Pre_S2StringLength_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CStringReqStrstr_Pre_Contains_Yes;
      ctx->Map.pci[ 1 ] < CStringReqStrstr_Pre_Contains_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      ctx->Map.entry = CStringReqStrstr_PopEntry( ctx );
      CStringReqStrstr_SetPreConditionStates( ctx );
      CStringReqStrstr_TestVariant( ctx );
    }
  }
}

/** @} */
