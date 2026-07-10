/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringReqMemcmp
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
 * @defgroup CStringReqMemcmp spec:/c/string/req/memcmp
 *
 * @{
 */

typedef enum {
  CStringReqMemcmp_Pre_N_Zero,
  CStringReqMemcmp_Pre_N_Small,
  CStringReqMemcmp_Pre_N_Word,
  CStringReqMemcmp_Pre_N_MultiWord,
  CStringReqMemcmp_Pre_N_NA
} CStringReqMemcmp_Pre_N;

typedef enum {
  CStringReqMemcmp_Pre_Compare_Equal,
  CStringReqMemcmp_Pre_Compare_Less,
  CStringReqMemcmp_Pre_Compare_Greater,
  CStringReqMemcmp_Pre_Compare_NA
} CStringReqMemcmp_Pre_Compare;

typedef enum {
  CStringReqMemcmp_Pre_Alignment_Aligned,
  CStringReqMemcmp_Pre_Alignment_Unaligned,
  CStringReqMemcmp_Pre_Alignment_NA
} CStringReqMemcmp_Pre_Alignment;

typedef enum {
  CStringReqMemcmp_Pre_MatchPosition_FirstChunk,
  CStringReqMemcmp_Pre_MatchPosition_SecondChunk,
  CStringReqMemcmp_Pre_MatchPosition_Remainder,
  CStringReqMemcmp_Pre_MatchPosition_NA
} CStringReqMemcmp_Pre_MatchPosition;

typedef enum {
  CStringReqMemcmp_Post_Result_Equal,
  CStringReqMemcmp_Post_Result_Less,
  CStringReqMemcmp_Post_Result_Greater,
  CStringReqMemcmp_Post_Result_NA
} CStringReqMemcmp_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_N_NA : 1;
  uint8_t Pre_Compare_NA : 1;
  uint8_t Pre_Alignment_NA : 1;
  uint8_t Pre_MatchPosition_NA : 1;
  uint8_t Post_Result : 2;
} CStringReqMemcmp_Entry;

/**
 * @brief Test context for spec:/c/string/req/memcmp test case.
 */
typedef struct {
  /**
   * @brief This member specifies the ``s1`` parameter value.
   */
  const void *s1;

  /**
   * @brief This member specifies the ``s2`` parameter value.
   */
  const void *s2;

  /**
   * @brief This member specifies the ``n`` parameter value.
   */
  size_t n;

  /**
   * @brief This member contains the return value.
   */
  int retval;

  /**
   * @brief This member contains the index of the first differing byte within
   *   the ``s1`` and ``s2`` buffers.
   */
  size_t diff_pos;

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
    CStringReqMemcmp_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringReqMemcmp_Context;

static CStringReqMemcmp_Context
  CStringReqMemcmp_Instance;

static const char * const CStringReqMemcmp_PreDesc_N[] = {
  "Zero",
  "Small",
  "Word",
  "MultiWord",
  "NA"
};

static const char * const CStringReqMemcmp_PreDesc_Compare[] = {
  "Equal",
  "Less",
  "Greater",
  "NA"
};

static const char * const CStringReqMemcmp_PreDesc_Alignment[] = {
  "Aligned",
  "Unaligned",
  "NA"
};

static const char * const CStringReqMemcmp_PreDesc_MatchPosition[] = {
  "FirstChunk",
  "SecondChunk",
  "Remainder",
  "NA"
};

static const char * const * const CStringReqMemcmp_PreDesc[] = {
  CStringReqMemcmp_PreDesc_N,
  CStringReqMemcmp_PreDesc_Compare,
  CStringReqMemcmp_PreDesc_Alignment,
  CStringReqMemcmp_PreDesc_MatchPosition,
  NULL
};

static void CStringReqMemcmp_Pre_N_Prepare(
  CStringReqMemcmp_Context *ctx,
  CStringReqMemcmp_Pre_N    state
)
{
  switch ( state ) {
    case CStringReqMemcmp_Pre_N_Zero: {
      /*
       * While the size specified by ``n`` is equal to zero.
       */
      ctx->n = 0;
      break;
    }

    case CStringReqMemcmp_Pre_N_Small: {
      /*
       * While the size specified by ``n`` is greater than zero, while the size
       * specified by ``n`` is less than the size of a `long` integer on the
       * target architecture.
       */
      ctx->n = sizeof( long ) - 1;
      break;
    }

    case CStringReqMemcmp_Pre_N_Word: {
      /*
       * While the size specified by ``n`` is equal to the size of a `long`
       * integer on the target architecture.
       */
      ctx->n = sizeof( long );
      break;
    }

    case CStringReqMemcmp_Pre_N_MultiWord: {
      /*
       * While the size specified by ``n`` is greater than twice the size of a
       * `long` integer on the target architecture, while the size specified by
       * ``n`` is not evenly divisible by the size of a `long` integer on the
       * target architecture.
       */
      ctx->n = 2 * sizeof( long ) + 3;
      break;
    }

    case CStringReqMemcmp_Pre_N_NA:
      break;
  }
}

static void CStringReqMemcmp_Pre_Compare_Prepare(
  CStringReqMemcmp_Context    *ctx,
  CStringReqMemcmp_Pre_Compare state
)
{
  switch ( state ) {
    case CStringReqMemcmp_Pre_Compare_Equal: {
      /*
       * While the first bytes specified by ``n`` of the memory areas
       * referenced by ``s1`` and ``s2`` are equal.
       */
      /* The buffers are already equal from test-prepare */
      break;
    }

    case CStringReqMemcmp_Pre_Compare_Less: {
      /*
       * While the first bytes specified by ``n`` of the memory areas
       * referenced by ``s1`` and ``s2`` are not equal, while the byte value at
       * the first differing position of the memory area referenced by ``s1``
       * is less than the corresponding byte value of the memory area
       * referenced by ``s2``.
       */
      ctx->diff_delta = -1;
      ctx->diff_pos = ctx->offset + 2;
      ctx->s1_buf[ ctx->diff_pos ] = (unsigned char)
        ( ctx->s2_buf[ ctx->diff_pos ] + ctx->diff_delta );
      break;
    }

    case CStringReqMemcmp_Pre_Compare_Greater: {
      /*
       * While the first bytes specified by ``n`` of the memory areas
       * referenced by ``s1`` and ``s2`` are not equal, while the byte value at
       * the first differing position of the memory area referenced by ``s1``
       * is greater than the corresponding byte value of the memory area
       * referenced by ``s2``.
       */
      ctx->diff_delta = 1;
      ctx->diff_pos = ctx->offset + 2;
      ctx->s1_buf[ ctx->diff_pos ] = (unsigned char)
        ( ctx->s2_buf[ ctx->diff_pos ] + ctx->diff_delta );
      break;
    }

    case CStringReqMemcmp_Pre_Compare_NA:
      break;
  }
}

static void CStringReqMemcmp_Pre_Alignment_Prepare(
  CStringReqMemcmp_Context      *ctx,
  CStringReqMemcmp_Pre_Alignment state
)
{
  switch ( state ) {
    case CStringReqMemcmp_Pre_Alignment_Aligned: {
      /*
       * While the ``s1`` parameter and the ``s2`` parameter are aligned on a
       * `long` integer boundary of the target architecture.
       */
      ctx->offset = sizeof( long );
      ctx->s1 = ctx->s1_buf + ctx->offset;
      ctx->s2 = ctx->s2_buf + ctx->offset;
      break;
    }

    case CStringReqMemcmp_Pre_Alignment_Unaligned: {
      /*
       * While the ``s1`` parameter or the ``s2`` parameter are not aligned on
       * a `long` integer boundary of the target architecture.
       */
      ctx->offset = sizeof( long ) + 1;
      ctx->s1 = ctx->s1_buf + ctx->offset;
      ctx->s2 = ctx->s2_buf + ctx->offset;
      break;
    }

    case CStringReqMemcmp_Pre_Alignment_NA:
      break;
  }
}

static void CStringReqMemcmp_Pre_MatchPosition_Prepare(
  CStringReqMemcmp_Context          *ctx,
  CStringReqMemcmp_Pre_MatchPosition state
)
{
  switch ( state ) {
    case CStringReqMemcmp_Pre_MatchPosition_FirstChunk: {
      /*
       * While the first differing byte occurs within the first aligned `long`
       * integer sized chunk of the memory areas referenced by ``s1`` and
       * ``s2``.
       */
      ctx->s1_buf[ ctx->diff_pos ] = ctx->s2_buf[ ctx->diff_pos ];
      ctx->diff_pos = ctx->offset + 3;
      ctx->s1_buf[ ctx->diff_pos ] = (unsigned char)
        ( ctx->s2_buf[ ctx->diff_pos ] + ctx->diff_delta );
      break;
    }

    case CStringReqMemcmp_Pre_MatchPosition_SecondChunk: {
      /*
       * While the first differing byte occurs within the second aligned `long`
       * integer sized chunk of the memory areas referenced by ``s1`` and
       * ``s2``.
       */
      ctx->s1_buf[ ctx->diff_pos ] = ctx->s2_buf[ ctx->diff_pos ];
      ctx->diff_pos = ctx->offset + sizeof( long ) + 3;
      ctx->s1_buf[ ctx->diff_pos ] = (unsigned char)
        ( ctx->s2_buf[ ctx->diff_pos ] + ctx->diff_delta );
      break;
    }

    case CStringReqMemcmp_Pre_MatchPosition_Remainder: {
      /*
       * While the first differing byte occurs within the trailing bytes of the
       * memory areas referenced by ``s1`` and ``s2`` which remain after all
       * aligned `long` integer sized chunks have been processed.
       */
      ctx->s1_buf[ ctx->diff_pos ] = ctx->s2_buf[ ctx->diff_pos ];
      ctx->diff_pos = ctx->offset + 2 * sizeof( long ) + 1;
      ctx->s1_buf[ ctx->diff_pos ] = (unsigned char)
        ( ctx->s2_buf[ ctx->diff_pos ] + ctx->diff_delta );
      break;
    }

    case CStringReqMemcmp_Pre_MatchPosition_NA:
      break;
  }
}

static void CStringReqMemcmp_Post_Result_Check(
  CStringReqMemcmp_Context    *ctx,
  CStringReqMemcmp_Post_Result state
)
{
  switch ( state ) {
    case CStringReqMemcmp_Post_Result_Equal: {
      /*
       * The return value of memcmp() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CStringReqMemcmp_Post_Result_Less: {
      /*
       * The return value of memcmp() shall be less than zero.
       */
      T_lt_int( ctx->retval, 0 );
      break;
    }

    case CStringReqMemcmp_Post_Result_Greater: {
      /*
       * The return value of memcmp() shall be greater than zero.
       */
      T_gt_int( ctx->retval, 0 );
      break;
    }

    case CStringReqMemcmp_Post_Result_NA:
      break;
  }
}

static void CStringReqMemcmp_Prepare( CStringReqMemcmp_Context *ctx )
{
  memset( ctx->s1_buf, 0x11, sizeof( ctx->s1_buf ) );
  memset( ctx->s2_buf, 0x11, sizeof( ctx->s2_buf ) );
  ctx->offset = sizeof( long );
  ctx->s1 = ctx->s1_buf + ctx->offset;
  ctx->s2 = ctx->s2_buf + ctx->offset;
  ctx->retval = 0;
  ctx->diff_pos = 0;
  ctx->diff_delta = 0;
}

static void CStringReqMemcmp_Action( CStringReqMemcmp_Context *ctx )
{
  ctx->retval = memcmp( ctx->s1, ctx->s2, ctx->n );
}

static const CStringReqMemcmp_Entry
CStringReqMemcmp_Entries[] = {
  { 0, 0, 1, 1, 1, CStringReqMemcmp_Post_Result_Equal },
  { 0, 0, 0, 1, 1, CStringReqMemcmp_Post_Result_Equal },
  { 0, 0, 0, 1, 1, CStringReqMemcmp_Post_Result_Less },
  { 0, 0, 0, 1, 1, CStringReqMemcmp_Post_Result_Greater },
  { 0, 0, 0, 0, 1, CStringReqMemcmp_Post_Result_Equal },
  { 0, 0, 0, 0, 0, CStringReqMemcmp_Post_Result_Less },
  { 0, 0, 0, 0, 0, CStringReqMemcmp_Post_Result_Greater }
};

static const uint8_t
CStringReqMemcmp_Map[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2,
  2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3,
  3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6
};

static size_t CStringReqMemcmp_Scope( void *arg, char *buf, size_t n )
{
  CStringReqMemcmp_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStringReqMemcmp_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStringReqMemcmp_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringReqMemcmp_Scope,
  .initial_context = &CStringReqMemcmp_Instance
};

static inline CStringReqMemcmp_Entry CStringReqMemcmp_PopEntry(
  CStringReqMemcmp_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringReqMemcmp_Entries[
    CStringReqMemcmp_Map[ index ]
  ];
}

static void CStringReqMemcmp_SetPreConditionStates(
  CStringReqMemcmp_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Compare_NA ) {
    ctx->Map.pcs[ 1 ] = CStringReqMemcmp_Pre_Compare_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_Alignment_NA ) {
    ctx->Map.pcs[ 2 ] = CStringReqMemcmp_Pre_Alignment_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_MatchPosition_NA ) {
    ctx->Map.pcs[ 3 ] = CStringReqMemcmp_Pre_MatchPosition_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }
}

static void CStringReqMemcmp_TestVariant( CStringReqMemcmp_Context *ctx )
{
  CStringReqMemcmp_Pre_N_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringReqMemcmp_Pre_Compare_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CStringReqMemcmp_Pre_Alignment_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CStringReqMemcmp_Pre_MatchPosition_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CStringReqMemcmp_Action( ctx );
  CStringReqMemcmp_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CStringReqMemcmp( void )
 */
T_TEST_CASE_FIXTURE( CStringReqMemcmp, &CStringReqMemcmp_Fixture )
{
  CStringReqMemcmp_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CStringReqMemcmp_Pre_N_Zero;
    ctx->Map.pci[ 0 ] < CStringReqMemcmp_Pre_N_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CStringReqMemcmp_Pre_Compare_Equal;
      ctx->Map.pci[ 1 ] < CStringReqMemcmp_Pre_Compare_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CStringReqMemcmp_Pre_Alignment_Aligned;
        ctx->Map.pci[ 2 ] < CStringReqMemcmp_Pre_Alignment_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CStringReqMemcmp_Pre_MatchPosition_FirstChunk;
          ctx->Map.pci[ 3 ] < CStringReqMemcmp_Pre_MatchPosition_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = CStringReqMemcmp_PopEntry( ctx );
          CStringReqMemcmp_SetPreConditionStates( ctx );
          CStringReqMemcmp_Prepare( ctx );
          CStringReqMemcmp_TestVariant( ctx );
        }
      }
    }
  }
}

/** @} */
