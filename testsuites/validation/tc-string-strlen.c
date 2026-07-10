/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringReqStrlen
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
 * @defgroup CStringReqStrlen spec:/c/string/req/strlen
 *
 * @{
 */

typedef enum {
  CStringReqStrlen_Pre_Alignment_Aligned,
  CStringReqStrlen_Pre_Alignment_Unaligned,
  CStringReqStrlen_Pre_Alignment_NA
} CStringReqStrlen_Pre_Alignment;

typedef enum {
  CStringReqStrlen_Pre_MatchPosition_FirstChunk,
  CStringReqStrlen_Pre_MatchPosition_SecondChunk,
  CStringReqStrlen_Pre_MatchPosition_NA
} CStringReqStrlen_Pre_MatchPosition;

typedef enum {
  CStringReqStrlen_Post_Result_Length,
  CStringReqStrlen_Post_Result_NA
} CStringReqStrlen_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Alignment_NA : 1;
  uint8_t Pre_MatchPosition_NA : 1;
  uint8_t Post_Result : 1;
} CStringReqStrlen_Entry;

/**
 * @brief Test context for spec:/c/string/req/strlen test case.
 */
typedef struct {
  /**
   * @brief This member specifies the ``s`` parameter value.
   */
  const char *s;

  /**
   * @brief This member contains the return value.
   */
  size_t retval;

  /**
   * @brief This member contains the expected return value.
   */
  size_t expected_len;

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
    CStringReqStrlen_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringReqStrlen_Context;

static CStringReqStrlen_Context
  CStringReqStrlen_Instance;

static const char * const CStringReqStrlen_PreDesc_Alignment[] = {
  "Aligned",
  "Unaligned",
  "NA"
};

static const char * const CStringReqStrlen_PreDesc_MatchPosition[] = {
  "FirstChunk",
  "SecondChunk",
  "NA"
};

static const char * const * const CStringReqStrlen_PreDesc[] = {
  CStringReqStrlen_PreDesc_Alignment,
  CStringReqStrlen_PreDesc_MatchPosition,
  NULL
};

static void CStringReqStrlen_Pre_Alignment_Prepare(
  CStringReqStrlen_Context      *ctx,
  CStringReqStrlen_Pre_Alignment state
)
{
  switch ( state ) {
    case CStringReqStrlen_Pre_Alignment_Aligned: {
      /*
       * While the ``s`` parameter is aligned on a `long` integer boundary of
       * the target architecture.
       */
      ctx->offset = sizeof( long );
      ctx->s = (const char *) ( ctx->s_buf + ctx->offset );
      break;
    }

    case CStringReqStrlen_Pre_Alignment_Unaligned: {
      /*
       * While the ``s`` parameter is not aligned on a `long` integer boundary
       * of the target architecture.
       */
      ctx->offset = sizeof( long ) + 1;
      ctx->s = (const char *) ( ctx->s_buf + ctx->offset );
      break;
    }

    case CStringReqStrlen_Pre_Alignment_NA:
      break;
  }
}

static void CStringReqStrlen_Pre_MatchPosition_Prepare(
  CStringReqStrlen_Context          *ctx,
  CStringReqStrlen_Pre_MatchPosition state
)
{
  switch ( state ) {
    case CStringReqStrlen_Pre_MatchPosition_FirstChunk: {
      /*
       * While the terminating null character occurs within the first aligned
       * `long` integer sized chunk of the string referenced by ``s``.
       */
      ctx->s_buf[ ctx->offset ] = 0;
      ctx->expected_len = 0;
      break;
    }

    case CStringReqStrlen_Pre_MatchPosition_SecondChunk: {
      /*
       * While the terminating null character occurs within the second aligned
       * `long` integer sized chunk of the string referenced by ``s``.
       */
      ctx->s_buf[ ctx->offset + sizeof( long ) ] = 0;
      ctx->expected_len = sizeof( long );
      break;
    }

    case CStringReqStrlen_Pre_MatchPosition_NA:
      break;
  }
}

static void CStringReqStrlen_Post_Result_Check(
  CStringReqStrlen_Context    *ctx,
  CStringReqStrlen_Post_Result state
)
{
  switch ( state ) {
    case CStringReqStrlen_Post_Result_Length: {
      /*
       * The return value of strlen() shall be equal to the length of the
       * string referenced by ``s``.
       */
      T_eq_uint( ctx->retval, ctx->expected_len );
      break;
    }

    case CStringReqStrlen_Post_Result_NA:
      break;
  }
}

static void CStringReqStrlen_Prepare( CStringReqStrlen_Context *ctx )
{
  memset( ctx->s_buf, 0x41, sizeof( ctx->s_buf ) );
  ctx->offset = sizeof( long );
  ctx->s = (const char *) ( ctx->s_buf + ctx->offset );
  ctx->retval = 0;
  ctx->expected_len = 0;
}

static void CStringReqStrlen_Action( CStringReqStrlen_Context *ctx )
{
  ctx->retval = strlen( ctx->s );
}

static const CStringReqStrlen_Entry
CStringReqStrlen_Entries[] = {
  { 0, 0, 0, CStringReqStrlen_Post_Result_Length }
};

static const uint8_t
CStringReqStrlen_Map[] = {
  0, 0, 0, 0
};

static size_t CStringReqStrlen_Scope( void *arg, char *buf, size_t n )
{
  CStringReqStrlen_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStringReqStrlen_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStringReqStrlen_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringReqStrlen_Scope,
  .initial_context = &CStringReqStrlen_Instance
};

static inline CStringReqStrlen_Entry CStringReqStrlen_PopEntry(
  CStringReqStrlen_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringReqStrlen_Entries[
    CStringReqStrlen_Map[ index ]
  ];
}

static void CStringReqStrlen_TestVariant( CStringReqStrlen_Context *ctx )
{
  CStringReqStrlen_Pre_Alignment_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringReqStrlen_Pre_MatchPosition_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CStringReqStrlen_Action( ctx );
  CStringReqStrlen_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CStringReqStrlen( void )
 */
T_TEST_CASE_FIXTURE( CStringReqStrlen, &CStringReqStrlen_Fixture )
{
  CStringReqStrlen_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CStringReqStrlen_Pre_Alignment_Aligned;
    ctx->Map.pcs[ 0 ] < CStringReqStrlen_Pre_Alignment_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CStringReqStrlen_Pre_MatchPosition_FirstChunk;
      ctx->Map.pcs[ 1 ] < CStringReqStrlen_Pre_MatchPosition_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CStringReqStrlen_PopEntry( ctx );
      CStringReqStrlen_Prepare( ctx );
      CStringReqStrlen_TestVariant( ctx );
    }
  }
}

/** @} */
