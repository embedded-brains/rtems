/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringReqStrncpy
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
 * @defgroup CStringReqStrncpy spec:/c/string/req/strncpy
 *
 * @{
 */

typedef enum {
  CStringReqStrncpy_Pre_N_Zero,
  CStringReqStrncpy_Pre_N_Small,
  CStringReqStrncpy_Pre_N_Word,
  CStringReqStrncpy_Pre_N_MultiWord,
  CStringReqStrncpy_Pre_N_NA
} CStringReqStrncpy_Pre_N;

typedef enum {
  CStringReqStrncpy_Pre_Alignment_Aligned,
  CStringReqStrncpy_Pre_Alignment_Unaligned,
  CStringReqStrncpy_Pre_Alignment_NA
} CStringReqStrncpy_Pre_Alignment;

typedef enum {
  CStringReqStrncpy_Pre_Fit_Fits,
  CStringReqStrncpy_Pre_Fit_Truncate,
  CStringReqStrncpy_Pre_Fit_NA
} CStringReqStrncpy_Pre_Fit;

typedef enum {
  CStringReqStrncpy_Pre_MatchPosition_FirstChunk,
  CStringReqStrncpy_Pre_MatchPosition_SecondChunk,
  CStringReqStrncpy_Pre_MatchPosition_Remainder,
  CStringReqStrncpy_Pre_MatchPosition_NA
} CStringReqStrncpy_Pre_MatchPosition;

typedef enum {
  CStringReqStrncpy_Post_Result_SameAsDst,
  CStringReqStrncpy_Post_Result_NA
} CStringReqStrncpy_Post_Result;

typedef enum {
  CStringReqStrncpy_Post_DstArea_Unmodified,
  CStringReqStrncpy_Post_DstArea_CopiedWithPadding,
  CStringReqStrncpy_Post_DstArea_CopiedFull,
  CStringReqStrncpy_Post_DstArea_NA
} CStringReqStrncpy_Post_DstArea;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_N_NA : 1;
  uint8_t Pre_Alignment_NA : 1;
  uint8_t Pre_Fit_NA : 1;
  uint8_t Pre_MatchPosition_NA : 1;
  uint8_t Post_Result : 1;
  uint8_t Post_DstArea : 2;
} CStringReqStrncpy_Entry;

/**
 * @brief Test context for spec:/c/string/req/strncpy test case.
 */
typedef struct {
  /**
   * @brief This member specifies the ``dest`` parameter value.
   */
  char *dest;

  /**
   * @brief This member specifies the ``src`` parameter value.
   */
  const char *src;

  /**
   * @brief This member specifies the ``n`` parameter value.
   */
  size_t n;

  /**
   * @brief This member contains the return value.
   */
  char *retval;

  /**
   * @brief This member contains the index of the terminating null character
   *   within the source buffer.
   */
  size_t match_pos;

  /**
   * @brief This member contains the offset of the guarded ``dest`` and ``src``
   *   regions from the start of the destination and source buffers.
   */
  size_t offset;

  /**
   * @brief This member provides the destination buffer.
   */
  unsigned char dest_buf[ 8 * sizeof( long ) ] RTEMS_ALIGNED( sizeof( long ) );

  /**
   * @brief This member provides the source buffer.
   */
  unsigned char src_buf[ 8 * sizeof( long ) ] RTEMS_ALIGNED( sizeof( long ) );

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
    CStringReqStrncpy_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringReqStrncpy_Context;

static CStringReqStrncpy_Context
  CStringReqStrncpy_Instance;

static const char * const CStringReqStrncpy_PreDesc_N[] = {
  "Zero",
  "Small",
  "Word",
  "MultiWord",
  "NA"
};

static const char * const CStringReqStrncpy_PreDesc_Alignment[] = {
  "Aligned",
  "Unaligned",
  "NA"
};

static const char * const CStringReqStrncpy_PreDesc_Fit[] = {
  "Fits",
  "Truncate",
  "NA"
};

static const char * const CStringReqStrncpy_PreDesc_MatchPosition[] = {
  "FirstChunk",
  "SecondChunk",
  "Remainder",
  "NA"
};

static const char * const * const CStringReqStrncpy_PreDesc[] = {
  CStringReqStrncpy_PreDesc_N,
  CStringReqStrncpy_PreDesc_Alignment,
  CStringReqStrncpy_PreDesc_Fit,
  CStringReqStrncpy_PreDesc_MatchPosition,
  NULL
};

static void CStringReqStrncpy_Pre_N_Prepare(
  CStringReqStrncpy_Context *ctx,
  CStringReqStrncpy_Pre_N    state
)
{
  switch ( state ) {
    case CStringReqStrncpy_Pre_N_Zero: {
      /*
       * While the size specified by ``n`` is equal to zero.
       */
      ctx->n = 0;
      break;
    }

    case CStringReqStrncpy_Pre_N_Small: {
      /*
       * While the size specified by ``n`` is greater than zero, while the size
       * specified by ``n`` is less than the size of a `long` integer on the
       * target architecture.
       */
      ctx->n = sizeof( long ) - 1;
      break;
    }

    case CStringReqStrncpy_Pre_N_Word: {
      /*
       * While the size specified by ``n`` is equal to the size of a `long`
       * integer on the target architecture.
       */
      ctx->n = sizeof( long );
      break;
    }

    case CStringReqStrncpy_Pre_N_MultiWord: {
      /*
       * While the size specified by ``n`` is greater than twice the size of a
       * `long` integer on the target architecture, while the size specified by
       * ``n`` is not evenly divisible by the size of a `long` integer on the
       * target architecture.
       */
      ctx->n = 2 * sizeof( long ) + 3;
      break;
    }

    case CStringReqStrncpy_Pre_N_NA:
      break;
  }
}

static void CStringReqStrncpy_Pre_Alignment_Prepare(
  CStringReqStrncpy_Context      *ctx,
  CStringReqStrncpy_Pre_Alignment state
)
{
  switch ( state ) {
    case CStringReqStrncpy_Pre_Alignment_Aligned: {
      /*
       * While the ``dest`` parameter and the ``src`` parameter are aligned on
       * a `long` integer boundary of the target architecture.
       */
      ctx->offset = sizeof( long );
      ctx->dest = (char *) ( ctx->dest_buf + ctx->offset );
      ctx->src = (const char *) ( ctx->src_buf + ctx->offset );
      break;
    }

    case CStringReqStrncpy_Pre_Alignment_Unaligned: {
      /*
       * While the ``dest`` parameter or the ``src`` parameter are not aligned
       * on a `long` integer boundary of the target architecture.
       */
      ctx->offset = sizeof( long ) + 1;
      ctx->dest = (char *) ( ctx->dest_buf + ctx->offset );
      ctx->src = (const char *) ( ctx->src_buf + ctx->offset );
      break;
    }

    case CStringReqStrncpy_Pre_Alignment_NA:
      break;
  }
}

static void CStringReqStrncpy_Pre_Fit_Prepare(
  CStringReqStrncpy_Context *ctx,
  CStringReqStrncpy_Pre_Fit  state
)
{
  switch ( state ) {
    case CStringReqStrncpy_Pre_Fit_Fits: {
      /*
       * While the length of the string referenced by ``src`` is less than the
       * size specified by ``n``.
       */
      ctx->match_pos = ctx->offset + 2;
      ctx->src_buf[ ctx->match_pos ] = 0;
      break;
    }

    case CStringReqStrncpy_Pre_Fit_Truncate: {
      /*
       * While the length of the string referenced by ``src`` is greater than
       * or equal to the size specified by ``n``.
       */
      /* The default fill value of the source buffer does not contain a
         null character within the first n bytes */
      break;
    }

    case CStringReqStrncpy_Pre_Fit_NA:
      break;
  }
}

static void CStringReqStrncpy_Pre_MatchPosition_Prepare(
  CStringReqStrncpy_Context          *ctx,
  CStringReqStrncpy_Pre_MatchPosition state
)
{
  switch ( state ) {
    case CStringReqStrncpy_Pre_MatchPosition_FirstChunk: {
      /*
       * While the terminating null character occurs within the first aligned
       * `long` integer sized chunk of the string referenced by ``src``.
       */
      ctx->src_buf[ ctx->match_pos ] = 0x41;
      ctx->match_pos = ctx->offset + 3;
      ctx->src_buf[ ctx->match_pos ] = 0;
      break;
    }

    case CStringReqStrncpy_Pre_MatchPosition_SecondChunk: {
      /*
       * While the terminating null character occurs within the second aligned
       * `long` integer sized chunk of the string referenced by ``src``.
       */
      ctx->src_buf[ ctx->match_pos ] = 0x41;
      ctx->match_pos = ctx->offset + sizeof( long ) + 3;
      ctx->src_buf[ ctx->match_pos ] = 0;
      break;
    }

    case CStringReqStrncpy_Pre_MatchPosition_Remainder: {
      /*
       * While the terminating null character occurs within the trailing bytes
       * of the string referenced by ``src`` which remain after all aligned
       * `long` integer sized chunks have been processed.
       */
      ctx->src_buf[ ctx->match_pos ] = 0x41;
      ctx->match_pos = ctx->offset + 2 * sizeof( long ) + 1;
      ctx->src_buf[ ctx->match_pos ] = 0;
      break;
    }

    case CStringReqStrncpy_Pre_MatchPosition_NA:
      break;
  }
}

static void CStringReqStrncpy_Post_Result_Check(
  CStringReqStrncpy_Context    *ctx,
  CStringReqStrncpy_Post_Result state
)
{
  switch ( state ) {
    case CStringReqStrncpy_Post_Result_SameAsDst: {
      /*
       * The return value of strncpy() shall be equal to the initial value of
       * ``dest``.
       */
      T_eq_ptr( ctx->retval, ctx->dest_buf + ctx->offset );
      break;
    }

    case CStringReqStrncpy_Post_Result_NA:
      break;
  }
}

static void CStringReqStrncpy_Post_DstArea_Check(
  CStringReqStrncpy_Context     *ctx,
  CStringReqStrncpy_Post_DstArea state
)
{
  switch ( state ) {
    case CStringReqStrncpy_Post_DstArea_Unmodified: {
      /*
       * The memory area referenced by ``dest`` shall not be modified.
       */
      for ( size_t i = 0; i < sizeof( ctx->dest_buf ); ++i ) {
        T_quiet_eq_uint( ctx->dest_buf[ i ], 0xaa );
      }
      break;
    }

    case CStringReqStrncpy_Post_DstArea_CopiedWithPadding: {
      /*
       * The string referenced by ``src``, including its terminating null
       * character, shall be copied to the memory area referenced by ``dest``.
       * The remaining bytes of the memory area referenced by ``dest`` up to
       * the size specified by ``n`` shall be set to the terminating null
       * character.
       */
      size_t copy_len = ctx->match_pos - ctx->offset + 1;

      T_eq_nstr(
        (const char *) &ctx->dest_buf[ ctx->offset ],
        (const char *) &ctx->src_buf[ ctx->offset ],
        copy_len
      );

      for (
        size_t i = ctx->offset + copy_len;
        i < ctx->offset + ctx->n;
        ++i
      ) {
        T_quiet_eq_uint( ctx->dest_buf[ i ], 0 );
      }

      for ( size_t i = 0; i < ctx->offset; ++i ) {
        T_quiet_eq_uint( ctx->dest_buf[ i ], 0xaa );
      }

      for (
        size_t i = ctx->offset + ctx->n;
        i < sizeof( ctx->dest_buf );
        ++i
      ) {
        T_quiet_eq_uint( ctx->dest_buf[ i ], 0xaa );
      }
      break;
    }

    case CStringReqStrncpy_Post_DstArea_CopiedFull: {
      /*
       * The bytes specified by ``n`` of the string referenced by ``src`` shall
       * be copied to the memory area referenced by ``dest``.
       */
      T_eq_mem(
        ctx->dest_buf + ctx->offset,
        ctx->src_buf + ctx->offset,
        ctx->n
      );

      for ( size_t i = 0; i < ctx->offset; ++i ) {
        T_quiet_eq_uint( ctx->dest_buf[ i ], 0xaa );
      }

      for (
        size_t i = ctx->offset + ctx->n;
        i < sizeof( ctx->dest_buf );
        ++i
      ) {
        T_quiet_eq_uint( ctx->dest_buf[ i ], 0xaa );
      }
      break;
    }

    case CStringReqStrncpy_Post_DstArea_NA:
      break;
  }
}

static void CStringReqStrncpy_Prepare( CStringReqStrncpy_Context *ctx )
{
  memset( ctx->dest_buf, 0xaa, sizeof( ctx->dest_buf ) );
  memset( ctx->src_buf, 0x41, sizeof( ctx->src_buf ) );
  ctx->offset = sizeof( long );
  ctx->dest = (char *) ( ctx->dest_buf + ctx->offset );
  ctx->src = (const char *) ( ctx->src_buf + ctx->offset );
  ctx->retval = NULL;
  ctx->n = 0;
  ctx->match_pos = 0;
}

static void CStringReqStrncpy_Action( CStringReqStrncpy_Context *ctx )
{
  ctx->retval = strncpy( ctx->dest, ctx->src, ctx->n );
}

static const CStringReqStrncpy_Entry
CStringReqStrncpy_Entries[] = {
  { 0, 0, 1, 1, 1, CStringReqStrncpy_Post_Result_SameAsDst,
    CStringReqStrncpy_Post_DstArea_Unmodified },
  { 0, 0, 1, 0, 1, CStringReqStrncpy_Post_Result_SameAsDst,
    CStringReqStrncpy_Post_DstArea_CopiedWithPadding },
  { 0, 0, 1, 0, 1, CStringReqStrncpy_Post_Result_SameAsDst,
    CStringReqStrncpy_Post_DstArea_CopiedFull },
  { 0, 0, 0, 0, 0, CStringReqStrncpy_Post_Result_SameAsDst,
    CStringReqStrncpy_Post_DstArea_CopiedWithPadding },
  { 0, 0, 0, 0, 1, CStringReqStrncpy_Post_Result_SameAsDst,
    CStringReqStrncpy_Post_DstArea_CopiedFull }
};

static const uint8_t
CStringReqStrncpy_Map[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 1, 1, 1, 2, 2, 2, 1, 1,
  1, 2, 2, 2, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 3, 3, 3, 4, 4, 4
};

static size_t CStringReqStrncpy_Scope( void *arg, char *buf, size_t n )
{
  CStringReqStrncpy_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStringReqStrncpy_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStringReqStrncpy_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringReqStrncpy_Scope,
  .initial_context = &CStringReqStrncpy_Instance
};

static inline CStringReqStrncpy_Entry CStringReqStrncpy_PopEntry(
  CStringReqStrncpy_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringReqStrncpy_Entries[
    CStringReqStrncpy_Map[ index ]
  ];
}

static void CStringReqStrncpy_SetPreConditionStates(
  CStringReqStrncpy_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Alignment_NA ) {
    ctx->Map.pcs[ 1 ] = CStringReqStrncpy_Pre_Alignment_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_Fit_NA ) {
    ctx->Map.pcs[ 2 ] = CStringReqStrncpy_Pre_Fit_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_MatchPosition_NA ) {
    ctx->Map.pcs[ 3 ] = CStringReqStrncpy_Pre_MatchPosition_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }
}

static void CStringReqStrncpy_TestVariant( CStringReqStrncpy_Context *ctx )
{
  CStringReqStrncpy_Pre_N_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringReqStrncpy_Pre_Alignment_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CStringReqStrncpy_Pre_Fit_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CStringReqStrncpy_Pre_MatchPosition_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CStringReqStrncpy_Action( ctx );
  CStringReqStrncpy_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
  CStringReqStrncpy_Post_DstArea_Check( ctx, ctx->Map.entry.Post_DstArea );
}

/**
 * @fn void T_case_body_CStringReqStrncpy( void )
 */
T_TEST_CASE_FIXTURE( CStringReqStrncpy, &CStringReqStrncpy_Fixture )
{
  CStringReqStrncpy_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CStringReqStrncpy_Pre_N_Zero;
    ctx->Map.pci[ 0 ] < CStringReqStrncpy_Pre_N_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CStringReqStrncpy_Pre_Alignment_Aligned;
      ctx->Map.pci[ 1 ] < CStringReqStrncpy_Pre_Alignment_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CStringReqStrncpy_Pre_Fit_Fits;
        ctx->Map.pci[ 2 ] < CStringReqStrncpy_Pre_Fit_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CStringReqStrncpy_Pre_MatchPosition_FirstChunk;
          ctx->Map.pci[ 3 ] < CStringReqStrncpy_Pre_MatchPosition_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = CStringReqStrncpy_PopEntry( ctx );
          CStringReqStrncpy_SetPreConditionStates( ctx );
          CStringReqStrncpy_Prepare( ctx );
          CStringReqStrncpy_TestVariant( ctx );
        }
      }
    }
  }
}

/** @} */
