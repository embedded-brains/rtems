/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringReqMemchr
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
 * @defgroup CStringReqMemchr spec:/c/string/req/memchr
 *
 * @{
 */

typedef enum {
  CStringReqMemchr_Pre_N_Zero,
  CStringReqMemchr_Pre_N_Small,
  CStringReqMemchr_Pre_N_Word,
  CStringReqMemchr_Pre_N_MultiWord,
  CStringReqMemchr_Pre_N_NA
} CStringReqMemchr_Pre_N;

typedef enum {
  CStringReqMemchr_Pre_Alignment_Aligned,
  CStringReqMemchr_Pre_Alignment_Unaligned,
  CStringReqMemchr_Pre_Alignment_NA
} CStringReqMemchr_Pre_Alignment;

typedef enum {
  CStringReqMemchr_Pre_Match_Found,
  CStringReqMemchr_Pre_Match_NotFound,
  CStringReqMemchr_Pre_Match_NA
} CStringReqMemchr_Pre_Match;

typedef enum {
  CStringReqMemchr_Pre_MatchPosition_FirstChunk,
  CStringReqMemchr_Pre_MatchPosition_SecondChunk,
  CStringReqMemchr_Pre_MatchPosition_Remainder,
  CStringReqMemchr_Pre_MatchPosition_NA
} CStringReqMemchr_Pre_MatchPosition;

typedef enum {
  CStringReqMemchr_Post_Result_Found,
  CStringReqMemchr_Post_Result_Null,
  CStringReqMemchr_Post_Result_NA
} CStringReqMemchr_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_N_NA : 1;
  uint8_t Pre_Alignment_NA : 1;
  uint8_t Pre_Match_NA : 1;
  uint8_t Pre_MatchPosition_NA : 1;
  uint8_t Post_Result : 2;
} CStringReqMemchr_Entry;

/**
 * @brief Test context for spec:/c/string/req/memchr test case.
 */
typedef struct {
  /**
   * @brief This member specifies the ``s`` parameter value.
   */
  const void *s;

  /**
   * @brief This member specifies the ``c`` parameter value.
   */
  int c;

  /**
   * @brief This member specifies the ``n`` parameter value.
   */
  size_t n;

  /**
   * @brief This member contains the return value.
   */
  void *retval;

  /**
   * @brief This member contains the index of the byte matching the ``c``
   *   parameter value within the source buffer.
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
    CStringReqMemchr_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringReqMemchr_Context;

static CStringReqMemchr_Context
  CStringReqMemchr_Instance;

static const char * const CStringReqMemchr_PreDesc_N[] = {
  "Zero",
  "Small",
  "Word",
  "MultiWord",
  "NA"
};

static const char * const CStringReqMemchr_PreDesc_Alignment[] = {
  "Aligned",
  "Unaligned",
  "NA"
};

static const char * const CStringReqMemchr_PreDesc_Match[] = {
  "Found",
  "NotFound",
  "NA"
};

static const char * const CStringReqMemchr_PreDesc_MatchPosition[] = {
  "FirstChunk",
  "SecondChunk",
  "Remainder",
  "NA"
};

static const char * const * const CStringReqMemchr_PreDesc[] = {
  CStringReqMemchr_PreDesc_N,
  CStringReqMemchr_PreDesc_Alignment,
  CStringReqMemchr_PreDesc_Match,
  CStringReqMemchr_PreDesc_MatchPosition,
  NULL
};

static void CStringReqMemchr_Pre_N_Prepare(
  CStringReqMemchr_Context *ctx,
  CStringReqMemchr_Pre_N    state
)
{
  switch ( state ) {
    case CStringReqMemchr_Pre_N_Zero: {
      /*
       * While the size specified by ``n`` is equal to zero.
       */
      ctx->n = 0;
      break;
    }

    case CStringReqMemchr_Pre_N_Small: {
      /*
       * While the size specified by ``n`` is greater than zero, while the size
       * specified by ``n`` is less than the size of a `long` integer on the
       * target architecture.
       */
      ctx->n = sizeof( long ) - 1;
      break;
    }

    case CStringReqMemchr_Pre_N_Word: {
      /*
       * While the size specified by ``n`` is equal to the size of a `long`
       * integer on the target architecture.
       */
      ctx->n = sizeof( long );
      break;
    }

    case CStringReqMemchr_Pre_N_MultiWord: {
      /*
       * While the size specified by ``n`` is greater than twice the size of a
       * `long` integer on the target architecture, while the size specified by
       * ``n`` is not evenly divisible by the size of a `long` integer on the
       * target architecture.
       */
      ctx->n = 2 * sizeof( long ) + 3;
      break;
    }

    case CStringReqMemchr_Pre_N_NA:
      break;
  }
}

static void CStringReqMemchr_Pre_Alignment_Prepare(
  CStringReqMemchr_Context      *ctx,
  CStringReqMemchr_Pre_Alignment state
)
{
  switch ( state ) {
    case CStringReqMemchr_Pre_Alignment_Aligned: {
      /*
       * While the ``s`` parameter is aligned on a `long` integer boundary of
       * the target architecture.
       */
      ctx->offset = sizeof( long );
      ctx->s = ctx->s_buf + ctx->offset;
      break;
    }

    case CStringReqMemchr_Pre_Alignment_Unaligned: {
      /*
       * While the ``s`` parameter is not aligned on a `long` integer boundary
       * of the target architecture.
       */
      ctx->offset = sizeof( long ) + 1;
      ctx->s = ctx->s_buf + ctx->offset;
      break;
    }

    case CStringReqMemchr_Pre_Alignment_NA:
      break;
  }
}

static void CStringReqMemchr_Pre_Match_Prepare(
  CStringReqMemchr_Context  *ctx,
  CStringReqMemchr_Pre_Match state
)
{
  switch ( state ) {
    case CStringReqMemchr_Pre_Match_Found: {
      /*
       * While the byte specified by ``c`` occurs within the first bytes
       * specified by ``n`` of the memory area referenced by ``s``.
       */
      ctx->match_pos = ctx->offset;
      ctx->s_buf[ ctx->match_pos ] = (unsigned char) ctx->c;
      break;
    }

    case CStringReqMemchr_Pre_Match_NotFound: {
      /*
       * While the byte specified by ``c`` does not occur within the first
       * bytes specified by ``n`` of the memory area referenced by ``s``.
       */
      ctx->s_buf[ ctx->offset + ctx->n ] = (unsigned char) ctx->c;
      break;
    }

    case CStringReqMemchr_Pre_Match_NA:
      break;
  }
}

static void CStringReqMemchr_Pre_MatchPosition_Prepare(
  CStringReqMemchr_Context          *ctx,
  CStringReqMemchr_Pre_MatchPosition state
)
{
  switch ( state ) {
    case CStringReqMemchr_Pre_MatchPosition_FirstChunk: {
      /*
       * While the byte specified by ``c`` occurs within the first aligned
       * `long` integer sized chunk of the memory area referenced by ``s``.
       */
      ctx->s_buf[ ctx->match_pos ] = 0x11;
      ctx->match_pos = ctx->offset + 3;
      ctx->s_buf[ ctx->match_pos ] = (unsigned char) ctx->c;
      break;
    }

    case CStringReqMemchr_Pre_MatchPosition_SecondChunk: {
      /*
       * While the byte specified by ``c`` occurs within the second aligned
       * `long` integer sized chunk of the memory area referenced by ``s``.
       */
      ctx->s_buf[ ctx->match_pos ] = 0x11;
      ctx->match_pos = ctx->offset + sizeof( long ) + 3;
      ctx->s_buf[ ctx->match_pos ] = (unsigned char) ctx->c;
      break;
    }

    case CStringReqMemchr_Pre_MatchPosition_Remainder: {
      /*
       * While the byte specified by ``c`` occurs within the trailing bytes of
       * the memory area referenced by ``s`` which remain after all aligned
       * `long` integer sized chunks have been processed.
       */
      ctx->s_buf[ ctx->match_pos ] = 0x11;
      ctx->match_pos = ctx->offset + 2 * sizeof( long ) + 1;
      ctx->s_buf[ ctx->match_pos ] = (unsigned char) ctx->c;
      break;
    }

    case CStringReqMemchr_Pre_MatchPosition_NA:
      break;
  }
}

static void CStringReqMemchr_Post_Result_Check(
  CStringReqMemchr_Context    *ctx,
  CStringReqMemchr_Post_Result state
)
{
  switch ( state ) {
    case CStringReqMemchr_Post_Result_Found: {
      /*
       * The return value of memchr() shall be a pointer to the byte matching
       * ``c`` in the memory area referenced by ``s``.
       */
      T_eq_ptr( ctx->retval, ctx->s_buf + ctx->match_pos );
      break;
    }

    case CStringReqMemchr_Post_Result_Null: {
      /*
       * The return value of memchr() shall be equal to NULL.
       */
      T_null( ctx->retval );
      break;
    }

    case CStringReqMemchr_Post_Result_NA:
      break;
  }
}

static void CStringReqMemchr_Prepare( CStringReqMemchr_Context *ctx )
{
  memset( ctx->s_buf, 0x11, sizeof( ctx->s_buf ) );
  ctx->c = 0x5a;
  ctx->offset = sizeof( long );
  ctx->s = ctx->s_buf + ctx->offset;
  ctx->retval = NULL;
  ctx->match_pos = 0;
}

static void CStringReqMemchr_Action( CStringReqMemchr_Context *ctx )
{
  ctx->retval = memchr( ctx->s, ctx->c, ctx->n );
}

static const CStringReqMemchr_Entry
CStringReqMemchr_Entries[] = {
  { 0, 0, 0, 0, 1, CStringReqMemchr_Post_Result_Null },
  { 0, 0, 0, 1, 1, CStringReqMemchr_Post_Result_Null },
  { 0, 0, 0, 0, 1, CStringReqMemchr_Post_Result_Found },
  { 0, 0, 0, 0, 0, CStringReqMemchr_Post_Result_Found }
};

static const uint8_t
CStringReqMemchr_Map[] = {
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 0, 0, 0, 2, 2, 2, 0, 0, 0, 2, 2,
  2, 0, 0, 0, 2, 2, 2, 0, 0, 0, 3, 3, 3, 0, 0, 0, 3, 3, 3, 0, 0, 0
};

static size_t CStringReqMemchr_Scope( void *arg, char *buf, size_t n )
{
  CStringReqMemchr_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStringReqMemchr_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStringReqMemchr_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringReqMemchr_Scope,
  .initial_context = &CStringReqMemchr_Instance
};

static inline CStringReqMemchr_Entry CStringReqMemchr_PopEntry(
  CStringReqMemchr_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringReqMemchr_Entries[
    CStringReqMemchr_Map[ index ]
  ];
}

static void CStringReqMemchr_SetPreConditionStates(
  CStringReqMemchr_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];

  if ( ctx->Map.entry.Pre_Match_NA ) {
    ctx->Map.pcs[ 2 ] = CStringReqMemchr_Pre_Match_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_MatchPosition_NA ) {
    ctx->Map.pcs[ 3 ] = CStringReqMemchr_Pre_MatchPosition_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }
}

static void CStringReqMemchr_TestVariant( CStringReqMemchr_Context *ctx )
{
  CStringReqMemchr_Pre_N_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringReqMemchr_Pre_Alignment_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CStringReqMemchr_Pre_Match_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CStringReqMemchr_Pre_MatchPosition_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CStringReqMemchr_Action( ctx );
  CStringReqMemchr_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CStringReqMemchr( void )
 */
T_TEST_CASE_FIXTURE( CStringReqMemchr, &CStringReqMemchr_Fixture )
{
  CStringReqMemchr_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CStringReqMemchr_Pre_N_Zero;
    ctx->Map.pci[ 0 ] < CStringReqMemchr_Pre_N_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CStringReqMemchr_Pre_Alignment_Aligned;
      ctx->Map.pci[ 1 ] < CStringReqMemchr_Pre_Alignment_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CStringReqMemchr_Pre_Match_Found;
        ctx->Map.pci[ 2 ] < CStringReqMemchr_Pre_Match_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CStringReqMemchr_Pre_MatchPosition_FirstChunk;
          ctx->Map.pci[ 3 ] < CStringReqMemchr_Pre_MatchPosition_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = CStringReqMemchr_PopEntry( ctx );
          CStringReqMemchr_SetPreConditionStates( ctx );
          CStringReqMemchr_Prepare( ctx );
          CStringReqMemchr_TestVariant( ctx );
        }
      }
    }
  }
}

/** @} */
