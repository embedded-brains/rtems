/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringReqStrrchr
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
 * @defgroup CStringReqStrrchr spec:/c/string/req/strrchr
 *
 * @{
 */

typedef enum {
  CStringReqStrrchr_Pre_C_Zero,
  CStringReqStrrchr_Pre_C_NonZero,
  CStringReqStrrchr_Pre_C_NA
} CStringReqStrrchr_Pre_C;

typedef enum {
  CStringReqStrrchr_Pre_Match_NotFound,
  CStringReqStrrchr_Pre_Match_FoundOnce,
  CStringReqStrrchr_Pre_Match_FoundMultiple,
  CStringReqStrrchr_Pre_Match_NA
} CStringReqStrrchr_Pre_Match;

typedef enum {
  CStringReqStrrchr_Post_Result_Found,
  CStringReqStrrchr_Post_Result_Null,
  CStringReqStrrchr_Post_Result_NA
} CStringReqStrrchr_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_C_NA : 1;
  uint8_t Pre_Match_NA : 1;
  uint8_t Post_Result : 2;
} CStringReqStrrchr_Entry;

/**
 * @brief Test context for spec:/c/string/req/strrchr test case.
 */
typedef struct {
  /**
   * @brief This member specifies the ``s`` parameter value.
   */
  const char *s;

  /**
   * @brief This member specifies the ``c`` parameter value.
   */
  int c;

  /**
   * @brief This member contains the return value.
   */
  char *retval;

  /**
   * @brief This member contains the index of the last occurrence of the
   *   notable byte within the source buffer.
   */
  size_t match_pos;

  /**
   * @brief This member provides the source buffer.
   */
  char s_buf[ 16 ];

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
    CStringReqStrrchr_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringReqStrrchr_Context;

static CStringReqStrrchr_Context
  CStringReqStrrchr_Instance;

static const char * const CStringReqStrrchr_PreDesc_C[] = {
  "Zero",
  "NonZero",
  "NA"
};

static const char * const CStringReqStrrchr_PreDesc_Match[] = {
  "NotFound",
  "FoundOnce",
  "FoundMultiple",
  "NA"
};

static const char * const * const CStringReqStrrchr_PreDesc[] = {
  CStringReqStrrchr_PreDesc_C,
  CStringReqStrrchr_PreDesc_Match,
  NULL
};

static void CStringReqStrrchr_Pre_C_Prepare(
  CStringReqStrrchr_Context *ctx,
  CStringReqStrrchr_Pre_C    state
)
{
  switch ( state ) {
    case CStringReqStrrchr_Pre_C_Zero: {
      /*
       * While the byte specified by ``c`` is equal to the terminating null
       * character.
       */
      strcpy( ctx->s_buf, "ABC" );
      ctx->c = 0;
      ctx->match_pos = 3;
      break;
    }

    case CStringReqStrrchr_Pre_C_NonZero: {
      /*
       * While the byte specified by ``c`` is not equal to the terminating null
       * character.
       */
      ctx->c = 'X';
      break;
    }

    case CStringReqStrrchr_Pre_C_NA:
      break;
  }
}

static void CStringReqStrrchr_Pre_Match_Prepare(
  CStringReqStrrchr_Context  *ctx,
  CStringReqStrrchr_Pre_Match state
)
{
  switch ( state ) {
    case CStringReqStrrchr_Pre_Match_NotFound: {
      /*
       * While the byte specified by ``c`` does not occur in the string
       * referenced by ``s`` before its terminating null character.
       */
      strcpy( ctx->s_buf, "ABC" );
      break;
    }

    case CStringReqStrrchr_Pre_Match_FoundOnce: {
      /*
       * While the byte specified by ``c`` occurs exactly once in the string
       * referenced by ``s`` before its terminating null character.
       */
      strcpy( ctx->s_buf, "ABXCD" );
      ctx->match_pos = 2;
      break;
    }

    case CStringReqStrrchr_Pre_Match_FoundMultiple: {
      /*
       * While the byte specified by ``c`` occurs more than once in the string
       * referenced by ``s`` before its terminating null character.
       */
      strcpy( ctx->s_buf, "AXBXC" );
      ctx->match_pos = 3;
      break;
    }

    case CStringReqStrrchr_Pre_Match_NA:
      break;
  }
}

static void CStringReqStrrchr_Post_Result_Check(
  CStringReqStrrchr_Context    *ctx,
  CStringReqStrrchr_Post_Result state
)
{
  switch ( state ) {
    case CStringReqStrrchr_Post_Result_Found: {
      /*
       * The return value of strrchr() shall be a pointer to the last
       * occurrence of the notable byte in the string referenced by ``s``.
       */
      T_eq_ptr( ctx->retval, ctx->s_buf + ctx->match_pos );
      break;
    }

    case CStringReqStrrchr_Post_Result_Null: {
      /*
       * The return value of strrchr() shall be equal to NULL.
       */
      T_null( ctx->retval );
      break;
    }

    case CStringReqStrrchr_Post_Result_NA:
      break;
  }
}

static void CStringReqStrrchr_Prepare( CStringReqStrrchr_Context *ctx )
{
  memset( ctx->s_buf, 0, sizeof( ctx->s_buf ) );
  ctx->s = ctx->s_buf;
  ctx->c = 0;
  ctx->retval = NULL;
  ctx->match_pos = 0;
}

static void CStringReqStrrchr_Action( CStringReqStrrchr_Context *ctx )
{
  ctx->retval = strrchr( ctx->s, ctx->c );
}

static const CStringReqStrrchr_Entry
CStringReqStrrchr_Entries[] = {
  { 0, 0, 1, CStringReqStrrchr_Post_Result_Found },
  { 0, 0, 0, CStringReqStrrchr_Post_Result_Found },
  { 0, 0, 0, CStringReqStrrchr_Post_Result_Null }
};

static const uint8_t
CStringReqStrrchr_Map[] = {
  0, 0, 0, 2, 1, 1
};

static size_t CStringReqStrrchr_Scope( void *arg, char *buf, size_t n )
{
  CStringReqStrrchr_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStringReqStrrchr_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStringReqStrrchr_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringReqStrrchr_Scope,
  .initial_context = &CStringReqStrrchr_Instance
};

static inline CStringReqStrrchr_Entry CStringReqStrrchr_PopEntry(
  CStringReqStrrchr_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringReqStrrchr_Entries[
    CStringReqStrrchr_Map[ index ]
  ];
}

static void CStringReqStrrchr_SetPreConditionStates(
  CStringReqStrrchr_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Match_NA ) {
    ctx->Map.pcs[ 1 ] = CStringReqStrrchr_Pre_Match_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }
}

static void CStringReqStrrchr_TestVariant( CStringReqStrrchr_Context *ctx )
{
  CStringReqStrrchr_Pre_C_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringReqStrrchr_Pre_Match_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CStringReqStrrchr_Action( ctx );
  CStringReqStrrchr_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CStringReqStrrchr( void )
 */
T_TEST_CASE_FIXTURE( CStringReqStrrchr, &CStringReqStrrchr_Fixture )
{
  CStringReqStrrchr_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CStringReqStrrchr_Pre_C_Zero;
    ctx->Map.pci[ 0 ] < CStringReqStrrchr_Pre_C_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CStringReqStrrchr_Pre_Match_NotFound;
      ctx->Map.pci[ 1 ] < CStringReqStrrchr_Pre_Match_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      ctx->Map.entry = CStringReqStrrchr_PopEntry( ctx );
      CStringReqStrrchr_SetPreConditionStates( ctx );
      CStringReqStrrchr_Prepare( ctx );
      CStringReqStrrchr_TestVariant( ctx );
    }
  }
}

/** @} */
