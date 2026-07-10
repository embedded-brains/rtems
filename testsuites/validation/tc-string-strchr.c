/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringReqStrchr
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

#include <rtems.h>
#include <string.h>

#include <rtems/test.h>

/**
 * @defgroup CStringReqStrchr spec:/c/string/req/strchr
 *
 * @{
 */

typedef enum {
  CStringReqStrchr_Pre_C_Zero,
  CStringReqStrchr_Pre_C_NonZero,
  CStringReqStrchr_Pre_C_NA
} CStringReqStrchr_Pre_C;

typedef enum {
  CStringReqStrchr_Pre_Alignment_Aligned,
  CStringReqStrchr_Pre_Alignment_Unaligned,
  CStringReqStrchr_Pre_Alignment_NA
} CStringReqStrchr_Pre_Alignment;

typedef enum {
  CStringReqStrchr_Pre_Match_Found,
  CStringReqStrchr_Pre_Match_NotFound,
  CStringReqStrchr_Pre_Match_Empty,
  CStringReqStrchr_Pre_Match_NA
} CStringReqStrchr_Pre_Match;

typedef enum {
  CStringReqStrchr_Pre_MatchPosition_FirstChunk,
  CStringReqStrchr_Pre_MatchPosition_SecondChunk,
  CStringReqStrchr_Pre_MatchPosition_NA
} CStringReqStrchr_Pre_MatchPosition;

typedef enum {
  CStringReqStrchr_Post_Result_Found,
  CStringReqStrchr_Post_Result_Null,
  CStringReqStrchr_Post_Result_NA
} CStringReqStrchr_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_C_NA : 1;
  uint8_t Pre_Alignment_NA : 1;
  uint8_t Pre_Match_NA : 1;
  uint8_t Pre_MatchPosition_NA : 1;
  uint8_t Post_Result : 2;
} CStringReqStrchr_Entry;

/**
 * @brief Test context for spec:/c/string/req/strchr test case.
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
   * @brief This member contains the index of the notable byte within the
   *   source buffer.
   */
  size_t match_pos;

  /**
   * @brief This member contains the offset of the guarded ``s`` region from
   *   the start of the source buffer.
   */
  size_t offset;

  /**
   * @brief This member provides the source buffer.
   */
  unsigned char s_buf[ 8 * sizeof( long ) ] RTEMS_ALIGNED( sizeof( long ) );

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
    CStringReqStrchr_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringReqStrchr_Context;

static CStringReqStrchr_Context
  CStringReqStrchr_Instance;

static const char * const CStringReqStrchr_PreDesc_C[] = {
  "Zero",
  "NonZero",
  "NA"
};

static const char * const CStringReqStrchr_PreDesc_Alignment[] = {
  "Aligned",
  "Unaligned",
  "NA"
};

static const char * const CStringReqStrchr_PreDesc_Match[] = {
  "Found",
  "NotFound",
  "Empty",
  "NA"
};

static const char * const CStringReqStrchr_PreDesc_MatchPosition[] = {
  "FirstChunk",
  "SecondChunk",
  "NA"
};

static const char * const * const CStringReqStrchr_PreDesc[] = {
  CStringReqStrchr_PreDesc_C,
  CStringReqStrchr_PreDesc_Alignment,
  CStringReqStrchr_PreDesc_Match,
  CStringReqStrchr_PreDesc_MatchPosition,
  NULL
};

static void CStringReqStrchr_Pre_C_Prepare(
  CStringReqStrchr_Context *ctx,
  CStringReqStrchr_Pre_C    state
)
{
  switch ( state ) {
    case CStringReqStrchr_Pre_C_Zero: {
      /*
       * While the byte specified by ``c`` is equal to the terminating null
       * character.
       */
      ctx->c = 0;
      break;
    }

    case CStringReqStrchr_Pre_C_NonZero: {
      /*
       * While the byte specified by ``c`` is not equal to the terminating null
       * character.
       */
      ctx->c = 0x42;
      break;
    }

    case CStringReqStrchr_Pre_C_NA:
      break;
  }
}

static void CStringReqStrchr_Pre_Alignment_Prepare(
  CStringReqStrchr_Context      *ctx,
  CStringReqStrchr_Pre_Alignment state
)
{
  switch ( state ) {
    case CStringReqStrchr_Pre_Alignment_Aligned: {
      /*
       * While the ``s`` parameter is aligned on a `long` integer boundary of
       * the target architecture.
       */
      ctx->offset = sizeof( long );
      ctx->s = (const char *) ( ctx->s_buf + ctx->offset );
      break;
    }

    case CStringReqStrchr_Pre_Alignment_Unaligned: {
      /*
       * While the ``s`` parameter is not aligned on a `long` integer boundary
       * of the target architecture.
       */
      ctx->offset = sizeof( long ) + 1;
      ctx->s = (const char *) ( ctx->s_buf + ctx->offset );
      break;
    }

    case CStringReqStrchr_Pre_Alignment_NA:
      break;
  }
}

static void CStringReqStrchr_Pre_Match_Prepare(
  CStringReqStrchr_Context  *ctx,
  CStringReqStrchr_Pre_Match state
)
{
  switch ( state ) {
    case CStringReqStrchr_Pre_Match_Found: {
      /*
       * While the byte specified by ``c`` occurs in the string referenced by
       * ``s`` before its terminating null character.
       */
      ctx->match_pos = ctx->offset;
      ctx->s_buf[ ctx->match_pos ] = (unsigned char) ctx->c;
      ctx->s_buf[ ctx->offset + 2 * sizeof( long ) ] = 0;
      break;
    }

    case CStringReqStrchr_Pre_Match_NotFound: {
      /*
       * While the byte specified by ``c`` does not occur in the string
       * referenced by ``s`` before its terminating null character, while the
       * string referenced by ``s`` is not empty.
       */
      ctx->s_buf[ ctx->offset + 2 * sizeof( long ) ] = 0;
      ctx->s_buf[ ctx->offset + 2 * sizeof( long ) + 1 ] =
        (unsigned char) ctx->c;
      break;
    }

    case CStringReqStrchr_Pre_Match_Empty: {
      /*
       * While the string referenced by ``s`` is empty.
       */
      ctx->s_buf[ ctx->offset ] = 0;
      ctx->s_buf[ ctx->offset + 1 ] = (unsigned char) ctx->c;
      break;
    }

    case CStringReqStrchr_Pre_Match_NA:
      break;
  }
}

static void CStringReqStrchr_Pre_MatchPosition_Prepare(
  CStringReqStrchr_Context          *ctx,
  CStringReqStrchr_Pre_MatchPosition state
)
{
  switch ( state ) {
    case CStringReqStrchr_Pre_MatchPosition_FirstChunk: {
      /*
       * While the notable byte occurs within the first aligned `long` integer
       * sized chunk of the string referenced by ``s``.
       */
      ctx->s_buf[ ctx->match_pos ] = 0x41;
      ctx->match_pos = ctx->offset;
      ctx->s_buf[ ctx->match_pos ] = (unsigned char) ctx->c;
      break;
    }

    case CStringReqStrchr_Pre_MatchPosition_SecondChunk: {
      /*
       * While the notable byte occurs within the second aligned `long` integer
       * sized chunk of the string referenced by ``s``.
       */
      ctx->s_buf[ ctx->match_pos ] = 0x41;
      ctx->match_pos = ctx->offset + sizeof( long );
      ctx->s_buf[ ctx->match_pos ] = (unsigned char) ctx->c;
      break;
    }

    case CStringReqStrchr_Pre_MatchPosition_NA:
      break;
  }
}

static void CStringReqStrchr_Post_Result_Check(
  CStringReqStrchr_Context    *ctx,
  CStringReqStrchr_Post_Result state
)
{
  switch ( state ) {
    case CStringReqStrchr_Post_Result_Found: {
      /*
       * The return value of strchr() shall be a pointer to the notable byte in
       * the string referenced by ``s``.
       */
      T_eq_ptr( ctx->retval, (char *) ( ctx->s_buf + ctx->match_pos ) );
      break;
    }

    case CStringReqStrchr_Post_Result_Null: {
      /*
       * The return value of strchr() shall be equal to NULL.
       */
      T_null( ctx->retval );
      break;
    }

    case CStringReqStrchr_Post_Result_NA:
      break;
  }
}

static void CStringReqStrchr_Prepare( CStringReqStrchr_Context *ctx )
{
  memset( ctx->s_buf, 0x41, sizeof( ctx->s_buf ) );
  ctx->offset = sizeof( long );
  ctx->s = (const char *) ( ctx->s_buf + ctx->offset );
  ctx->c = 0;
  ctx->retval = NULL;
  ctx->match_pos = 0;
}

static void CStringReqStrchr_Action( CStringReqStrchr_Context *ctx )
{
  ctx->retval = strchr( ctx->s, ctx->c );
}

static const CStringReqStrchr_Entry
CStringReqStrchr_Entries[] = {
  { 0, 0, 0, 1, 0, CStringReqStrchr_Post_Result_Found },
  { 0, 0, 0, 0, 1, CStringReqStrchr_Post_Result_Null },
  { 0, 0, 0, 0, 0, CStringReqStrchr_Post_Result_Found }
};

static const uint8_t
CStringReqStrchr_Map[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1
};

static size_t CStringReqStrchr_Scope( void *arg, char *buf, size_t n )
{
  CStringReqStrchr_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStringReqStrchr_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStringReqStrchr_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringReqStrchr_Scope,
  .initial_context = &CStringReqStrchr_Instance
};

static inline CStringReqStrchr_Entry CStringReqStrchr_PopEntry(
  CStringReqStrchr_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringReqStrchr_Entries[
    CStringReqStrchr_Map[ index ]
  ];
}

static void CStringReqStrchr_SetPreConditionStates(
  CStringReqStrchr_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];

  if ( ctx->Map.entry.Pre_Match_NA ) {
    ctx->Map.pcs[ 2 ] = CStringReqStrchr_Pre_Match_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_MatchPosition_NA ) {
    ctx->Map.pcs[ 3 ] = CStringReqStrchr_Pre_MatchPosition_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }
}

static void CStringReqStrchr_TestVariant( CStringReqStrchr_Context *ctx )
{
  CStringReqStrchr_Pre_C_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringReqStrchr_Pre_Alignment_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CStringReqStrchr_Pre_Match_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CStringReqStrchr_Pre_MatchPosition_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CStringReqStrchr_Action( ctx );
  CStringReqStrchr_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CStringReqStrchr( void )
 */
T_TEST_CASE_FIXTURE( CStringReqStrchr, &CStringReqStrchr_Fixture )
{
  CStringReqStrchr_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CStringReqStrchr_Pre_C_Zero;
    ctx->Map.pci[ 0 ] < CStringReqStrchr_Pre_C_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CStringReqStrchr_Pre_Alignment_Aligned;
      ctx->Map.pci[ 1 ] < CStringReqStrchr_Pre_Alignment_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CStringReqStrchr_Pre_Match_Found;
        ctx->Map.pci[ 2 ] < CStringReqStrchr_Pre_Match_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CStringReqStrchr_Pre_MatchPosition_FirstChunk;
          ctx->Map.pci[ 3 ] < CStringReqStrchr_Pre_MatchPosition_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = CStringReqStrchr_PopEntry( ctx );
          CStringReqStrchr_SetPreConditionStates( ctx );
          CStringReqStrchr_Prepare( ctx );
          CStringReqStrchr_TestVariant( ctx );
        }
      }
    }
  }
}

/** @} */
