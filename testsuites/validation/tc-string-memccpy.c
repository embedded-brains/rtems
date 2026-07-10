/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringReqMemccpy
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
 * @defgroup CStringReqMemccpy spec:/c/string/req/memccpy
 *
 * @{
 */

typedef enum {
  CStringReqMemccpy_Pre_N_Zero,
  CStringReqMemccpy_Pre_N_Small,
  CStringReqMemccpy_Pre_N_Word,
  CStringReqMemccpy_Pre_N_MultiWord,
  CStringReqMemccpy_Pre_N_NA
} CStringReqMemccpy_Pre_N;

typedef enum {
  CStringReqMemccpy_Pre_Match_Found,
  CStringReqMemccpy_Pre_Match_NotFound,
  CStringReqMemccpy_Pre_Match_NA
} CStringReqMemccpy_Pre_Match;

typedef enum {
  CStringReqMemccpy_Pre_MatchPosition_FirstChunk,
  CStringReqMemccpy_Pre_MatchPosition_SecondChunk,
  CStringReqMemccpy_Pre_MatchPosition_Remainder,
  CStringReqMemccpy_Pre_MatchPosition_NA
} CStringReqMemccpy_Pre_MatchPosition;

typedef enum {
  CStringReqMemccpy_Pre_Alignment_Aligned,
  CStringReqMemccpy_Pre_Alignment_Unaligned,
  CStringReqMemccpy_Pre_Alignment_NA
} CStringReqMemccpy_Pre_Alignment;

typedef enum {
  CStringReqMemccpy_Post_Result_AfterChar,
  CStringReqMemccpy_Post_Result_Null,
  CStringReqMemccpy_Post_Result_NA
} CStringReqMemccpy_Post_Result;

typedef enum {
  CStringReqMemccpy_Post_MemoryArea_CopiedToMatch,
  CStringReqMemccpy_Post_MemoryArea_CopiedAll,
  CStringReqMemccpy_Post_MemoryArea_Nop,
  CStringReqMemccpy_Post_MemoryArea_NA
} CStringReqMemccpy_Post_MemoryArea;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_N_NA : 1;
  uint16_t Pre_Match_NA : 1;
  uint16_t Pre_MatchPosition_NA : 1;
  uint16_t Pre_Alignment_NA : 1;
  uint16_t Post_Result : 2;
  uint16_t Post_MemoryArea : 2;
} CStringReqMemccpy_Entry;

/**
 * @brief Test context for spec:/c/string/req/memccpy test case.
 */
typedef struct {
  /**
   * @brief This member specifies the ``dest`` parameter value.
   */
  void * dest;

  /**
   * @brief This member specifies the ``src`` parameter value.
   */
  const void * src;

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
    CStringReqMemccpy_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringReqMemccpy_Context;

static CStringReqMemccpy_Context
  CStringReqMemccpy_Instance;

static const char * const CStringReqMemccpy_PreDesc_N[] = {
  "Zero",
  "Small",
  "Word",
  "MultiWord",
  "NA"
};

static const char * const CStringReqMemccpy_PreDesc_Match[] = {
  "Found",
  "NotFound",
  "NA"
};

static const char * const CStringReqMemccpy_PreDesc_MatchPosition[] = {
  "FirstChunk",
  "SecondChunk",
  "Remainder",
  "NA"
};

static const char * const CStringReqMemccpy_PreDesc_Alignment[] = {
  "Aligned",
  "Unaligned",
  "NA"
};

static const char * const * const CStringReqMemccpy_PreDesc[] = {
  CStringReqMemccpy_PreDesc_N,
  CStringReqMemccpy_PreDesc_Match,
  CStringReqMemccpy_PreDesc_MatchPosition,
  CStringReqMemccpy_PreDesc_Alignment,
  NULL
};

static void CStringReqMemccpy_Pre_N_Prepare(
  CStringReqMemccpy_Context *ctx,
  CStringReqMemccpy_Pre_N    state
)
{
  switch ( state ) {
    case CStringReqMemccpy_Pre_N_Zero: {
      /*
       * While the size specified by ``n`` is equal to zero.
       */
      ctx->n = 0;
      break;
    }

    case CStringReqMemccpy_Pre_N_Small: {
      /*
       * While the size specified by ``n`` is greater than zero, while the size
       * specified by ``n`` is less than the size of a `long` integer on the
       * target architecture.
       */
      ctx->n = sizeof( long ) - 1;
      break;
    }

    case CStringReqMemccpy_Pre_N_Word: {
      /*
       * While the size specified by ``n`` is equal to the size of a `long`
       * integer on the target architecture.
       */
      ctx->n = sizeof( long );
      break;
    }

    case CStringReqMemccpy_Pre_N_MultiWord: {
      /*
       * While the size specified by ``n`` is greater than twice the size of a
       * `long` integer on the target architecture, while the size specified by
       * ``n`` is not evenly divisible by the size of a `long` integer on the
       * target architecture.
       */
      ctx->n = 2 * sizeof( long ) + 3;
      break;
    }

    case CStringReqMemccpy_Pre_N_NA:
      break;
  }
}

static void CStringReqMemccpy_Pre_Match_Prepare(
  CStringReqMemccpy_Context  *ctx,
  CStringReqMemccpy_Pre_Match state
)
{
  switch ( state ) {
    case CStringReqMemccpy_Pre_Match_Found: {
      /*
       * While the byte specified by ``c`` occurs within the first bytes
       * specified by ``n`` of the memory area referenced by ``src``.
       */
      ctx->match_pos = ctx->offset + 2;
      ctx->src_buf[ ctx->match_pos ] = (unsigned char) ctx->c;
      break;
    }

    case CStringReqMemccpy_Pre_Match_NotFound: {
      /*
       * While the byte specified by ``c`` does not occur within the first
       * bytes specified by ``n`` of the memory area referenced by ``src``.
       */
      /* The default fill value of the source buffer does not match c */
      break;
    }

    case CStringReqMemccpy_Pre_Match_NA:
      break;
  }
}

static void CStringReqMemccpy_Pre_MatchPosition_Prepare(
  CStringReqMemccpy_Context          *ctx,
  CStringReqMemccpy_Pre_MatchPosition state
)
{
  switch ( state ) {
    case CStringReqMemccpy_Pre_MatchPosition_FirstChunk: {
      /*
       * While the byte specified by ``c`` occurs within the first aligned
       * `long` integer sized chunk of the memory area referenced by ``src``.
       */
      ctx->src_buf[ ctx->match_pos ] = 0x11;
      ctx->match_pos = ctx->offset + 3;
      ctx->src_buf[ ctx->match_pos ] = (unsigned char) ctx->c;
      break;
    }

    case CStringReqMemccpy_Pre_MatchPosition_SecondChunk: {
      /*
       * While the byte specified by ``c`` occurs within the second aligned
       * `long` integer sized chunk of the memory area referenced by ``src``.
       */
      ctx->src_buf[ ctx->match_pos ] = 0x11;
      ctx->match_pos = ctx->offset + sizeof( long ) + 3;
      ctx->src_buf[ ctx->match_pos ] = (unsigned char) ctx->c;
      break;
    }

    case CStringReqMemccpy_Pre_MatchPosition_Remainder: {
      /*
       * While the byte specified by ``c`` occurs within the trailing bytes of
       * the memory area referenced by ``src`` which remain after all aligned
       * `long` integer sized chunks have been processed.
       */
      ctx->src_buf[ ctx->match_pos ] = 0x11;
      ctx->match_pos = ctx->offset + 2 * sizeof( long ) + 1;
      ctx->src_buf[ ctx->match_pos ] = (unsigned char) ctx->c;
      break;
    }

    case CStringReqMemccpy_Pre_MatchPosition_NA:
      break;
  }
}

static void CStringReqMemccpy_Pre_Alignment_Prepare(
  CStringReqMemccpy_Context      *ctx,
  CStringReqMemccpy_Pre_Alignment state
)
{
  switch ( state ) {
    case CStringReqMemccpy_Pre_Alignment_Aligned: {
      /*
       * While the ``dest`` parameter and the ``src`` parameter are aligned on
       * a `long` integer boundary of the target architecture.
       */
      ctx->offset = sizeof( long );
      ctx->dest = ctx->dest_buf + ctx->offset;
      ctx->src = ctx->src_buf + ctx->offset;
      break;
    }

    case CStringReqMemccpy_Pre_Alignment_Unaligned: {
      /*
       * While the ``dest`` parameter or the ``src`` parameter are not aligned
       * on a `long` integer boundary of the target architecture.
       */
      ctx->offset = sizeof( long ) + 1;
      ctx->dest = ctx->dest_buf + ctx->offset;
      ctx->src = ctx->src_buf + ctx->offset;
      break;
    }

    case CStringReqMemccpy_Pre_Alignment_NA:
      break;
  }
}

static void CStringReqMemccpy_Post_Result_Check(
  CStringReqMemccpy_Context    *ctx,
  CStringReqMemccpy_Post_Result state
)
{
  switch ( state ) {
    case CStringReqMemccpy_Post_Result_AfterChar: {
      /*
       * The return value of memccpy() shall be a pointer to the byte
       * immediately following the copied byte specified by ``c`` in the memory
       * area referenced by ``dest``.
       */
      T_eq_ptr( ctx->retval, ctx->dest_buf + ctx->match_pos + 1 );
      break;
    }

    case CStringReqMemccpy_Post_Result_Null: {
      /*
       * The return value of memccpy() shall be equal to NULL.
       */
      T_null( ctx->retval );
      break;
    }

    case CStringReqMemccpy_Post_Result_NA:
      break;
  }
}

static void CStringReqMemccpy_Post_MemoryArea_Check(
  CStringReqMemccpy_Context        *ctx,
  CStringReqMemccpy_Post_MemoryArea state
)
{
  switch ( state ) {
    case CStringReqMemccpy_Post_MemoryArea_CopiedToMatch: {
      /*
       * The bytes of the memory area referenced by ``dest`` up to and
       * including the byte matching ``c`` shall be copied from the memory area
       * referenced by ``src``. The remaining bytes of the memory area
       * referenced by ``dest`` shall not be modified.
       */
      T_eq_mem(
        ctx->dest_buf + ctx->offset,
        ctx->src_buf + ctx->offset,
        ctx->match_pos + 1 - ctx->offset
      );

      for ( size_t i = 0; i < ctx->offset; ++i ) {
        T_quiet_eq_uint( ctx->dest_buf[ i ], 0xaa );
      }

      for ( size_t i = ctx->match_pos + 1; i < sizeof( ctx->dest_buf ); ++i ) {
        T_quiet_eq_uint( ctx->dest_buf[ i ], 0xaa );
      }
      break;
    }

    case CStringReqMemccpy_Post_MemoryArea_CopiedAll: {
      /*
       * The bytes specified by ``n`` of the memory area referenced by ``dest``
       * shall be copied from the memory area referenced by ``src``. The
       * remaining bytes of the memory area referenced by ``dest`` shall not be
       * modified.
       */
      T_eq_mem(
        ctx->dest_buf + ctx->offset,
        ctx->src_buf + ctx->offset,
        ctx->n
      );

      for ( size_t i = 0; i < ctx->offset; ++i ) {
        T_quiet_eq_uint( ctx->dest_buf[ i ], 0xaa );
      }

      for ( size_t i = ctx->offset + ctx->n; i < sizeof( ctx->dest_buf ); ++i ) {
        T_quiet_eq_uint( ctx->dest_buf[ i ], 0xaa );
      }
      break;
    }

    case CStringReqMemccpy_Post_MemoryArea_Nop: {
      /*
       * The memory area referenced by ``dest`` shall not be modified.
       */
      for ( size_t i = 0; i < sizeof( ctx->dest_buf ); ++i ) {
        T_quiet_eq_uint( ctx->dest_buf[ i ], 0xaa );
      }
      break;
    }

    case CStringReqMemccpy_Post_MemoryArea_NA:
      break;
  }
}

static void CStringReqMemccpy_Prepare( CStringReqMemccpy_Context *ctx )
{
  memset( ctx->dest_buf, 0xaa, sizeof( ctx->dest_buf ) );
  memset( ctx->src_buf, 0x11, sizeof( ctx->src_buf ) );
  ctx->offset = sizeof( long );
  ctx->dest = ctx->dest_buf + ctx->offset;
  ctx->src = ctx->src_buf + ctx->offset;
  ctx->c = 0x5a;
  ctx->retval = NULL;
  ctx->match_pos = 0;
}

static void CStringReqMemccpy_Action( CStringReqMemccpy_Context *ctx )
{
  ctx->retval = memccpy( ctx->dest, ctx->src, ctx->c, ctx->n );
}

static const CStringReqMemccpy_Entry
CStringReqMemccpy_Entries[] = {
  { 0, 0, 1, 1, 1, CStringReqMemccpy_Post_Result_Null,
    CStringReqMemccpy_Post_MemoryArea_Nop },
  { 0, 0, 0, 1, 1, CStringReqMemccpy_Post_Result_AfterChar,
    CStringReqMemccpy_Post_MemoryArea_CopiedToMatch },
  { 0, 0, 0, 1, 1, CStringReqMemccpy_Post_Result_Null,
    CStringReqMemccpy_Post_MemoryArea_CopiedAll },
  { 0, 0, 0, 1, 0, CStringReqMemccpy_Post_Result_Null,
    CStringReqMemccpy_Post_MemoryArea_CopiedAll },
  { 0, 0, 0, 0, 0, CStringReqMemccpy_Post_Result_AfterChar,
    CStringReqMemccpy_Post_MemoryArea_CopiedToMatch },
  { 0, 0, 0, 1, 0, CStringReqMemccpy_Post_Result_AfterChar,
    CStringReqMemccpy_Post_MemoryArea_CopiedToMatch }
};

static const uint8_t
CStringReqMemccpy_Map[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 1, 1,
  1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 4, 5, 4, 5, 4, 5, 3, 3, 3, 3, 3, 3
};

static size_t CStringReqMemccpy_Scope( void *arg, char *buf, size_t n )
{
  CStringReqMemccpy_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStringReqMemccpy_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStringReqMemccpy_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringReqMemccpy_Scope,
  .initial_context = &CStringReqMemccpy_Instance
};

static inline CStringReqMemccpy_Entry CStringReqMemccpy_PopEntry(
  CStringReqMemccpy_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringReqMemccpy_Entries[
    CStringReqMemccpy_Map[ index ]
  ];
}

static void CStringReqMemccpy_SetPreConditionStates(
  CStringReqMemccpy_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Match_NA ) {
    ctx->Map.pcs[ 1 ] = CStringReqMemccpy_Pre_Match_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_MatchPosition_NA ) {
    ctx->Map.pcs[ 2 ] = CStringReqMemccpy_Pre_MatchPosition_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_Alignment_NA ) {
    ctx->Map.pcs[ 3 ] = CStringReqMemccpy_Pre_Alignment_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }
}

static void CStringReqMemccpy_TestVariant( CStringReqMemccpy_Context *ctx )
{
  CStringReqMemccpy_Pre_N_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringReqMemccpy_Pre_Match_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CStringReqMemccpy_Pre_MatchPosition_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CStringReqMemccpy_Pre_Alignment_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CStringReqMemccpy_Action( ctx );
  CStringReqMemccpy_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
  CStringReqMemccpy_Post_MemoryArea_Check(
    ctx,
    ctx->Map.entry.Post_MemoryArea
  );
}

/**
 * @fn void T_case_body_CStringReqMemccpy( void )
 */
T_TEST_CASE_FIXTURE( CStringReqMemccpy, &CStringReqMemccpy_Fixture )
{
  CStringReqMemccpy_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CStringReqMemccpy_Pre_N_Zero;
    ctx->Map.pci[ 0 ] < CStringReqMemccpy_Pre_N_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CStringReqMemccpy_Pre_Match_Found;
      ctx->Map.pci[ 1 ] < CStringReqMemccpy_Pre_Match_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CStringReqMemccpy_Pre_MatchPosition_FirstChunk;
        ctx->Map.pci[ 2 ] < CStringReqMemccpy_Pre_MatchPosition_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CStringReqMemccpy_Pre_Alignment_Aligned;
          ctx->Map.pci[ 3 ] < CStringReqMemccpy_Pre_Alignment_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = CStringReqMemccpy_PopEntry( ctx );
          CStringReqMemccpy_SetPreConditionStates( ctx );
          CStringReqMemccpy_Prepare( ctx );
          CStringReqMemccpy_TestVariant( ctx );
        }
      }
    }
  }
}

/** @} */
