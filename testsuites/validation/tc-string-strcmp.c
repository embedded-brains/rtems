/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringReqStrcmp
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
 * @defgroup CStringReqStrcmp spec:/c/string/req/strcmp
 *
 * @{
 */

typedef enum {
  CStringReqStrcmp_Pre_Alignment_Aligned,
  CStringReqStrcmp_Pre_Alignment_Unaligned,
  CStringReqStrcmp_Pre_Alignment_NA
} CStringReqStrcmp_Pre_Alignment;

typedef enum {
  CStringReqStrcmp_Pre_Compare_Equal,
  CStringReqStrcmp_Pre_Compare_Less,
  CStringReqStrcmp_Pre_Compare_Greater,
  CStringReqStrcmp_Pre_Compare_NA
} CStringReqStrcmp_Pre_Compare;

typedef enum {
  CStringReqStrcmp_Pre_MatchPosition_FirstChunk,
  CStringReqStrcmp_Pre_MatchPosition_SecondChunk,
  CStringReqStrcmp_Pre_MatchPosition_NA
} CStringReqStrcmp_Pre_MatchPosition;

typedef enum {
  CStringReqStrcmp_Post_Result_Equal,
  CStringReqStrcmp_Post_Result_Less,
  CStringReqStrcmp_Post_Result_Greater,
  CStringReqStrcmp_Post_Result_NA
} CStringReqStrcmp_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Alignment_NA : 1;
  uint8_t Pre_Compare_NA : 1;
  uint8_t Pre_MatchPosition_NA : 1;
  uint8_t Post_Result : 2;
} CStringReqStrcmp_Entry;

/**
 * @brief Test context for spec:/c/string/req/strcmp test case.
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
  int retval;

  /**
   * @brief This member contains the signed difference applied at the differing
   *   byte position.
   */
  int diff_delta;

  /**
   * @brief This member contains the offset of the guarded ``s1`` and ``s2``
   *   regions from the start of the s1 and s2 buffers.
   */
  size_t offset;

  /**
   * @brief This member provides the ``s1`` buffer.
   */
  unsigned char s1_buf[ 8 * sizeof( long ) ] RTEMS_ALIGNED( sizeof( long ) );

  /**
   * @brief This member provides the ``s2`` buffer.
   */
  unsigned char s2_buf[ 8 * sizeof( long ) ] RTEMS_ALIGNED( sizeof( long ) );

  struct {
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
    CStringReqStrcmp_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringReqStrcmp_Context;

static CStringReqStrcmp_Context
  CStringReqStrcmp_Instance;

static const char * const CStringReqStrcmp_PreDesc_Alignment[] = {
  "Aligned",
  "Unaligned",
  "NA"
};

static const char * const CStringReqStrcmp_PreDesc_Compare[] = {
  "Equal",
  "Less",
  "Greater",
  "NA"
};

static const char * const CStringReqStrcmp_PreDesc_MatchPosition[] = {
  "FirstChunk",
  "SecondChunk",
  "NA"
};

static const char * const * const CStringReqStrcmp_PreDesc[] = {
  CStringReqStrcmp_PreDesc_Alignment,
  CStringReqStrcmp_PreDesc_Compare,
  CStringReqStrcmp_PreDesc_MatchPosition,
  NULL
};

static void CStringReqStrcmp_Pre_Alignment_Prepare(
  CStringReqStrcmp_Context      *ctx,
  CStringReqStrcmp_Pre_Alignment state
)
{
  switch ( state ) {
    case CStringReqStrcmp_Pre_Alignment_Aligned: {
      /*
       * While the ``s1`` parameter and the ``s2`` parameter are aligned on a
       * `long` integer boundary of the target architecture.
       */
      ctx->offset = sizeof( long );
      ctx->s1 = (const char *) ( ctx->s1_buf + ctx->offset );
      ctx->s2 = (const char *) ( ctx->s2_buf + ctx->offset );
      break;
    }

    case CStringReqStrcmp_Pre_Alignment_Unaligned: {
      /*
       * While the ``s1`` parameter or the ``s2`` parameter are not aligned on
       * a `long` integer boundary of the target architecture.
       */
      ctx->offset = sizeof( long ) + 1;
      ctx->s1 = (const char *) ( ctx->s1_buf + ctx->offset );
      ctx->s2 = (const char *) ( ctx->s2_buf + ctx->offset );
      break;
    }

    case CStringReqStrcmp_Pre_Alignment_NA:
      break;
  }
}

static void CStringReqStrcmp_Pre_Compare_Prepare(
  CStringReqStrcmp_Context    *ctx,
  CStringReqStrcmp_Pre_Compare state
)
{
  switch ( state ) {
    case CStringReqStrcmp_Pre_Compare_Equal: {
      /*
       * While the strings referenced by ``s1`` and ``s2`` are equal.
       */
      ctx->diff_delta = 0;
      break;
    }

    case CStringReqStrcmp_Pre_Compare_Less: {
      /*
       * While the strings referenced by ``s1`` and ``s2`` are not equal, while
       * the byte value at the first differing position of the string
       * referenced by ``s1`` is less than the corresponding byte value of the
       * string referenced by ``s2``.
       */
      ctx->diff_delta = -1;
      break;
    }

    case CStringReqStrcmp_Pre_Compare_Greater: {
      /*
       * While the strings referenced by ``s1`` and ``s2`` are not equal, while
       * the byte value at the first differing position of the string
       * referenced by ``s1`` is greater than the corresponding byte value of
       * the string referenced by ``s2``.
       */
      ctx->diff_delta = 1;
      break;
    }

    case CStringReqStrcmp_Pre_Compare_NA:
      break;
  }
}

static void CStringReqStrcmp_Pre_MatchPosition_Prepare(
  CStringReqStrcmp_Context          *ctx,
  CStringReqStrcmp_Pre_MatchPosition state
)
{
  switch ( state ) {
    case CStringReqStrcmp_Pre_MatchPosition_FirstChunk: {
      /*
       * While the notable position occurs within the first aligned `long`
       * integer sized chunk of the strings referenced by ``s1`` and ``s2``.
       */
      size_t pos = ctx->offset;

      if ( ctx->diff_delta == 0 ) {
        ctx->s1_buf[ pos ] = 0;
        ctx->s2_buf[ pos ] = 0;
      } else {
        ctx->s1_buf[ pos ] = (unsigned char)
          ( ctx->s2_buf[ pos ] + ctx->diff_delta );
        ctx->s1_buf[ ctx->offset + 2 * sizeof( long ) ] = 0;
        ctx->s2_buf[ ctx->offset + 2 * sizeof( long ) ] = 0;
      }
      break;
    }

    case CStringReqStrcmp_Pre_MatchPosition_SecondChunk: {
      /*
       * While the notable position occurs within the second aligned `long`
       * integer sized chunk of the strings referenced by ``s1`` and ``s2``.
       */
      size_t pos = ctx->offset + sizeof( long );

      if ( ctx->diff_delta == 0 ) {
        ctx->s1_buf[ pos ] = 0;
        ctx->s2_buf[ pos ] = 0;
      } else {
        ctx->s1_buf[ pos ] = (unsigned char)
          ( ctx->s2_buf[ pos ] + ctx->diff_delta );
        ctx->s1_buf[ ctx->offset + 2 * sizeof( long ) ] = 0;
        ctx->s2_buf[ ctx->offset + 2 * sizeof( long ) ] = 0;
      }
      break;
    }

    case CStringReqStrcmp_Pre_MatchPosition_NA:
      break;
  }
}

static void CStringReqStrcmp_Post_Result_Check(
  CStringReqStrcmp_Context    *ctx,
  CStringReqStrcmp_Post_Result state
)
{
  switch ( state ) {
    case CStringReqStrcmp_Post_Result_Equal: {
      /*
       * The return value of strcmp() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CStringReqStrcmp_Post_Result_Less: {
      /*
       * The return value of strcmp() shall be less than zero.
       */
      T_lt_int( ctx->retval, 0 );
      break;
    }

    case CStringReqStrcmp_Post_Result_Greater: {
      /*
       * The return value of strcmp() shall be greater than zero.
       */
      T_gt_int( ctx->retval, 0 );
      break;
    }

    case CStringReqStrcmp_Post_Result_NA:
      break;
  }
}

static void CStringReqStrcmp_Prepare( CStringReqStrcmp_Context *ctx )
{
  memset( ctx->s1_buf, 0x41, sizeof( ctx->s1_buf ) );
  memset( ctx->s2_buf, 0x41, sizeof( ctx->s2_buf ) );
  ctx->offset = sizeof( long );
  ctx->s1 = (const char *) ( ctx->s1_buf + ctx->offset );
  ctx->s2 = (const char *) ( ctx->s2_buf + ctx->offset );
  ctx->retval = 0;
  ctx->diff_delta = 0;
}

static void CStringReqStrcmp_Action( CStringReqStrcmp_Context *ctx )
{
  ctx->retval = strcmp( ctx->s1, ctx->s2 );
}

static const CStringReqStrcmp_Entry
CStringReqStrcmp_Entries[] = {
  { 0, 0, 0, 0, CStringReqStrcmp_Post_Result_Equal },
  { 0, 0, 0, 0, CStringReqStrcmp_Post_Result_Less },
  { 0, 0, 0, 0, CStringReqStrcmp_Post_Result_Greater }
};

static const uint8_t
CStringReqStrcmp_Map[] = {
  0, 0, 1, 1, 2, 2, 0, 0, 1, 1, 2, 2
};

static size_t CStringReqStrcmp_Scope( void *arg, char *buf, size_t n )
{
  CStringReqStrcmp_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStringReqStrcmp_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStringReqStrcmp_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringReqStrcmp_Scope,
  .initial_context = &CStringReqStrcmp_Instance
};

static inline CStringReqStrcmp_Entry CStringReqStrcmp_PopEntry(
  CStringReqStrcmp_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringReqStrcmp_Entries[
    CStringReqStrcmp_Map[ index ]
  ];
}

static void CStringReqStrcmp_TestVariant( CStringReqStrcmp_Context *ctx )
{
  CStringReqStrcmp_Pre_Alignment_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringReqStrcmp_Pre_Compare_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CStringReqStrcmp_Pre_MatchPosition_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CStringReqStrcmp_Action( ctx );
  CStringReqStrcmp_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CStringReqStrcmp( void )
 */
T_TEST_CASE_FIXTURE( CStringReqStrcmp, &CStringReqStrcmp_Fixture )
{
  CStringReqStrcmp_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CStringReqStrcmp_Pre_Alignment_Aligned;
    ctx->Map.pcs[ 0 ] < CStringReqStrcmp_Pre_Alignment_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CStringReqStrcmp_Pre_Compare_Equal;
      ctx->Map.pcs[ 1 ] < CStringReqStrcmp_Pre_Compare_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = CStringReqStrcmp_Pre_MatchPosition_FirstChunk;
        ctx->Map.pcs[ 2 ] < CStringReqStrcmp_Pre_MatchPosition_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = CStringReqStrcmp_PopEntry( ctx );
        CStringReqStrcmp_Prepare( ctx );
        CStringReqStrcmp_TestVariant( ctx );
      }
    }
  }
}

/** @} */
