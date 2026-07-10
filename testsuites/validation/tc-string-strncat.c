/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringReqStrncat
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
 * @defgroup CStringReqStrncat spec:/c/string/req/strncat
 *
 * @{
 */

typedef enum {
  CStringReqStrncat_Pre_Alignment_Aligned,
  CStringReqStrncat_Pre_Alignment_Unaligned,
  CStringReqStrncat_Pre_Alignment_NA
} CStringReqStrncat_Pre_Alignment;

typedef enum {
  CStringReqStrncat_Pre_DstLength_Empty,
  CStringReqStrncat_Pre_DstLength_NonEmpty,
  CStringReqStrncat_Pre_DstLength_NA
} CStringReqStrncat_Pre_DstLength;

typedef enum {
  CStringReqStrncat_Pre_DstMatchPosition_FirstChunk,
  CStringReqStrncat_Pre_DstMatchPosition_SecondChunk,
  CStringReqStrncat_Pre_DstMatchPosition_NA
} CStringReqStrncat_Pre_DstMatchPosition;

typedef enum {
  CStringReqStrncat_Pre_N_Zero,
  CStringReqStrncat_Pre_N_Positive,
  CStringReqStrncat_Pre_N_NA
} CStringReqStrncat_Pre_N;

typedef enum {
  CStringReqStrncat_Pre_Fit_Fits,
  CStringReqStrncat_Pre_Fit_Truncate,
  CStringReqStrncat_Pre_Fit_NA
} CStringReqStrncat_Pre_Fit;

typedef enum {
  CStringReqStrncat_Post_Result_SameAsDst,
  CStringReqStrncat_Post_Result_NA
} CStringReqStrncat_Post_Result;

typedef enum {
  CStringReqStrncat_Post_DstArea_Unchanged,
  CStringReqStrncat_Post_DstArea_AppendedFull,
  CStringReqStrncat_Post_DstArea_AppendedTruncated,
  CStringReqStrncat_Post_DstArea_NA
} CStringReqStrncat_Post_DstArea;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Alignment_NA : 1;
  uint16_t Pre_DstLength_NA : 1;
  uint16_t Pre_DstMatchPosition_NA : 1;
  uint16_t Pre_N_NA : 1;
  uint16_t Pre_Fit_NA : 1;
  uint16_t Post_Result : 1;
  uint16_t Post_DstArea : 2;
} CStringReqStrncat_Entry;

/**
 * @brief Test context for spec:/c/string/req/strncat test case.
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
   * @brief This member contains the offset of the guarded ``dest`` region from
   *   the start of the destination buffer.
   */
  size_t offset;

  /**
   * @brief This member contains the position of the terminating null character
   *   of the ``dest`` string before the trigger action.
   */
  size_t term_pos;

  /**
   * @brief This member contains the position immediately following the last
   *   byte modified by the trigger action, used to verify the trailing guard.
   */
  size_t content_end;

  /**
   * @brief This member provides the destination buffer.
   */
  unsigned char dest_buf[ 8 * sizeof( long ) + 16 ] RTEMS_ALIGNED( sizeof( long ) );

  /**
   * @brief This member provides the source buffer.
   */
  char src_buf[ 16 ];

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 5 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 5 ];

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
    CStringReqStrncat_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringReqStrncat_Context;

static CStringReqStrncat_Context
  CStringReqStrncat_Instance;

static const char * const CStringReqStrncat_PreDesc_Alignment[] = {
  "Aligned",
  "Unaligned",
  "NA"
};

static const char * const CStringReqStrncat_PreDesc_DstLength[] = {
  "Empty",
  "NonEmpty",
  "NA"
};

static const char * const CStringReqStrncat_PreDesc_DstMatchPosition[] = {
  "FirstChunk",
  "SecondChunk",
  "NA"
};

static const char * const CStringReqStrncat_PreDesc_N[] = {
  "Zero",
  "Positive",
  "NA"
};

static const char * const CStringReqStrncat_PreDesc_Fit[] = {
  "Fits",
  "Truncate",
  "NA"
};

static const char * const * const CStringReqStrncat_PreDesc[] = {
  CStringReqStrncat_PreDesc_Alignment,
  CStringReqStrncat_PreDesc_DstLength,
  CStringReqStrncat_PreDesc_DstMatchPosition,
  CStringReqStrncat_PreDesc_N,
  CStringReqStrncat_PreDesc_Fit,
  NULL
};

static void CStringReqStrncat_Pre_Alignment_Prepare(
  CStringReqStrncat_Context      *ctx,
  CStringReqStrncat_Pre_Alignment state
)
{
  switch ( state ) {
    case CStringReqStrncat_Pre_Alignment_Aligned: {
      /*
       * While the ``dest`` parameter is aligned on a `long` integer boundary
       * of the target architecture.
       */
      ctx->offset = sizeof( long );
      ctx->dest = (char *) ( ctx->dest_buf + ctx->offset );
      break;
    }

    case CStringReqStrncat_Pre_Alignment_Unaligned: {
      /*
       * While the ``dest`` parameter is not aligned on a `long` integer
       * boundary of the target architecture.
       */
      ctx->offset = sizeof( long ) + 1;
      ctx->dest = (char *) ( ctx->dest_buf + ctx->offset );
      break;
    }

    case CStringReqStrncat_Pre_Alignment_NA:
      break;
  }
}

static void CStringReqStrncat_Pre_DstLength_Prepare(
  CStringReqStrncat_Context      *ctx,
  CStringReqStrncat_Pre_DstLength state
)
{
  switch ( state ) {
    case CStringReqStrncat_Pre_DstLength_Empty: {
      /*
       * While the string referenced by ``dest`` is empty.
       */
      ctx->dest_buf[ ctx->offset ] = 0;
      ctx->term_pos = ctx->offset;
      break;
    }

    case CStringReqStrncat_Pre_DstLength_NonEmpty: {
      /*
       * While the string referenced by ``dest`` is not empty.
       */
      ctx->dest_buf[ ctx->offset ] = 'P';
      ctx->dest_buf[ ctx->offset + 1 ] = 'P';
      ctx->dest_buf[ ctx->offset + 2 ] = 0;
      ctx->term_pos = ctx->offset + 2;
      break;
    }

    case CStringReqStrncat_Pre_DstLength_NA:
      break;
  }
}

static void CStringReqStrncat_Pre_DstMatchPosition_Prepare(
  CStringReqStrncat_Context             *ctx,
  CStringReqStrncat_Pre_DstMatchPosition state
)
{
  switch ( state ) {
    case CStringReqStrncat_Pre_DstMatchPosition_FirstChunk: {
      /*
       * While the terminating null character occurs within the first aligned
       * `long` integer sized chunk of the string referenced by ``dest``.
       */
      ctx->dest_buf[ ctx->term_pos ] = 'P';
      ctx->term_pos = ctx->offset + 3;
      ctx->dest_buf[ ctx->term_pos ] = 0;
      break;
    }

    case CStringReqStrncat_Pre_DstMatchPosition_SecondChunk: {
      /*
       * While the terminating null character occurs within the second aligned
       * `long` integer sized chunk of the string referenced by ``dest``.
       */
      for ( size_t i = 2; i < sizeof( long ) + 3; ++i ) {
        ctx->dest_buf[ ctx->offset + i ] = 'P';
      }
      ctx->term_pos = ctx->offset + sizeof( long ) + 3;
      ctx->dest_buf[ ctx->term_pos ] = 0;
      break;
    }

    case CStringReqStrncat_Pre_DstMatchPosition_NA:
      break;
  }
}

static void CStringReqStrncat_Pre_N_Prepare(
  CStringReqStrncat_Context *ctx,
  CStringReqStrncat_Pre_N    state
)
{
  switch ( state ) {
    case CStringReqStrncat_Pre_N_Zero: {
      /*
       * While the size specified by ``n`` is equal to zero.
       */
      ctx->n = 0;
      break;
    }

    case CStringReqStrncat_Pre_N_Positive: {
      /*
       * While the size specified by ``n`` is greater than zero.
       */
      /* The exact positive value is determined by the Fit pre-condition */
      break;
    }

    case CStringReqStrncat_Pre_N_NA:
      break;
  }
}

static void CStringReqStrncat_Pre_Fit_Prepare(
  CStringReqStrncat_Context *ctx,
  CStringReqStrncat_Pre_Fit  state
)
{
  switch ( state ) {
    case CStringReqStrncat_Pre_Fit_Fits: {
      /*
       * While the length of the string referenced by ``src`` is less than the
       * size specified by ``n``.
       */
      strcpy( ctx->src_buf, "AB" );
      ctx->n = 8;
      break;
    }

    case CStringReqStrncat_Pre_Fit_Truncate: {
      /*
       * While the length of the string referenced by ``src`` is greater than
       * or equal to the size specified by ``n``.
       */
      strcpy( ctx->src_buf, "ABCDE" );
      ctx->n = 3;
      break;
    }

    case CStringReqStrncat_Pre_Fit_NA:
      break;
  }
}

static void CStringReqStrncat_Post_Result_Check(
  CStringReqStrncat_Context    *ctx,
  CStringReqStrncat_Post_Result state
)
{
  switch ( state ) {
    case CStringReqStrncat_Post_Result_SameAsDst: {
      /*
       * The return value of strncat() shall be equal to the initial value of
       * ``dest``.
       */
      T_eq_ptr( ctx->retval, ctx->dest_buf + ctx->offset );
      break;
    }

    case CStringReqStrncat_Post_Result_NA:
      break;
  }
}

static void CStringReqStrncat_Post_DstArea_Check(
  CStringReqStrncat_Context     *ctx,
  CStringReqStrncat_Post_DstArea state
)
{
  switch ( state ) {
    case CStringReqStrncat_Post_DstArea_Unchanged: {
      /*
       * The string referenced by ``dest`` shall not be modified.
       */
      T_eq_uint( ctx->dest_buf[ ctx->term_pos ], 0 );
      ctx->content_end = ctx->term_pos + 1;
      break;
    }

    case CStringReqStrncat_Post_DstArea_AppendedFull: {
      /*
       * The string referenced by ``src``, including its terminating null
       * character, shall be appended to the string referenced by ``dest``,
       * overwriting its terminating null character.
       */
      size_t len = strlen( ctx->src );

      T_eq_nstr(
        (const char *) &ctx->dest_buf[ ctx->term_pos ],
        ctx->src,
        len + 1
      );
      ctx->content_end = ctx->term_pos + len + 1;
      break;
    }

    case CStringReqStrncat_Post_DstArea_AppendedTruncated: {
      /*
       * The first bytes specified by ``n`` of the string referenced by ``src``
       * shall be appended to the string referenced by ``dest``, overwriting
       * its terminating null character, followed by a terminating null
       * character.
       */
      T_eq_nstr(
        (const char *) &ctx->dest_buf[ ctx->term_pos ],
        ctx->src,
        ctx->n
      );
      T_eq_uint( ctx->dest_buf[ ctx->term_pos + ctx->n ], 0 );
      ctx->content_end = ctx->term_pos + ctx->n + 1;
      break;
    }

    case CStringReqStrncat_Post_DstArea_NA:
      break;
  }

  for ( size_t i = 0; i < ctx->offset; ++i ) {
    T_quiet_eq_uint( ctx->dest_buf[ i ], 0xaa );
  }

  for ( size_t i = ctx->content_end; i < sizeof( ctx->dest_buf ); ++i ) {
    T_quiet_eq_uint( ctx->dest_buf[ i ], 0xaa );
  }
}

static void CStringReqStrncat_Prepare( CStringReqStrncat_Context *ctx )
{
  memset( ctx->dest_buf, 0xaa, sizeof( ctx->dest_buf ) );
  memset( ctx->src_buf, 0, sizeof( ctx->src_buf ) );
  ctx->offset = sizeof( long );
  ctx->dest = (char *) ( ctx->dest_buf + ctx->offset );
  ctx->src = ctx->src_buf;
  ctx->retval = NULL;
  ctx->n = 0;
  ctx->term_pos = ctx->offset;
  ctx->content_end = ctx->offset;
}

static void CStringReqStrncat_Action( CStringReqStrncat_Context *ctx )
{
  ctx->retval = strncat( ctx->dest, ctx->src, ctx->n );
}

static const CStringReqStrncat_Entry
CStringReqStrncat_Entries[] = {
  { 0, 0, 0, 1, 0, 1, CStringReqStrncat_Post_Result_SameAsDst,
    CStringReqStrncat_Post_DstArea_Unchanged },
  { 0, 0, 0, 1, 0, 0, CStringReqStrncat_Post_Result_SameAsDst,
    CStringReqStrncat_Post_DstArea_AppendedFull },
  { 0, 0, 0, 1, 0, 0, CStringReqStrncat_Post_Result_SameAsDst,
    CStringReqStrncat_Post_DstArea_AppendedTruncated },
  { 0, 0, 0, 0, 0, 1, CStringReqStrncat_Post_Result_SameAsDst,
    CStringReqStrncat_Post_DstArea_Unchanged },
  { 0, 0, 0, 0, 0, 0, CStringReqStrncat_Post_Result_SameAsDst,
    CStringReqStrncat_Post_DstArea_AppendedFull },
  { 0, 0, 0, 0, 0, 0, CStringReqStrncat_Post_Result_SameAsDst,
    CStringReqStrncat_Post_DstArea_AppendedTruncated }
};

static const uint8_t
CStringReqStrncat_Map[] = {
  0, 0, 1, 2, 0, 0, 1, 2, 3, 3, 4, 5, 3, 3, 4, 5, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0,
  1, 2, 0, 0, 1, 2
};

static size_t CStringReqStrncat_Scope( void *arg, char *buf, size_t n )
{
  CStringReqStrncat_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStringReqStrncat_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStringReqStrncat_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringReqStrncat_Scope,
  .initial_context = &CStringReqStrncat_Instance
};

static inline CStringReqStrncat_Entry CStringReqStrncat_PopEntry(
  CStringReqStrncat_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringReqStrncat_Entries[
    CStringReqStrncat_Map[ index ]
  ];
}

static void CStringReqStrncat_SetPreConditionStates(
  CStringReqStrncat_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];

  if ( ctx->Map.entry.Pre_DstMatchPosition_NA ) {
    ctx->Map.pcs[ 2 ] = CStringReqStrncat_Pre_DstMatchPosition_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];

  if ( ctx->Map.entry.Pre_Fit_NA ) {
    ctx->Map.pcs[ 4 ] = CStringReqStrncat_Pre_Fit_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }
}

static void CStringReqStrncat_TestVariant( CStringReqStrncat_Context *ctx )
{
  CStringReqStrncat_Pre_Alignment_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringReqStrncat_Pre_DstLength_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CStringReqStrncat_Pre_DstMatchPosition_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CStringReqStrncat_Pre_N_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CStringReqStrncat_Pre_Fit_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CStringReqStrncat_Action( ctx );
  CStringReqStrncat_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
  CStringReqStrncat_Post_DstArea_Check( ctx, ctx->Map.entry.Post_DstArea );
}

/**
 * @fn void T_case_body_CStringReqStrncat( void )
 */
T_TEST_CASE_FIXTURE( CStringReqStrncat, &CStringReqStrncat_Fixture )
{
  CStringReqStrncat_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CStringReqStrncat_Pre_Alignment_Aligned;
    ctx->Map.pci[ 0 ] < CStringReqStrncat_Pre_Alignment_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CStringReqStrncat_Pre_DstLength_Empty;
      ctx->Map.pci[ 1 ] < CStringReqStrncat_Pre_DstLength_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CStringReqStrncat_Pre_DstMatchPosition_FirstChunk;
        ctx->Map.pci[ 2 ] < CStringReqStrncat_Pre_DstMatchPosition_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CStringReqStrncat_Pre_N_Zero;
          ctx->Map.pci[ 3 ] < CStringReqStrncat_Pre_N_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = CStringReqStrncat_Pre_Fit_Fits;
            ctx->Map.pci[ 4 ] < CStringReqStrncat_Pre_Fit_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            ctx->Map.entry = CStringReqStrncat_PopEntry( ctx );
            CStringReqStrncat_SetPreConditionStates( ctx );
            CStringReqStrncat_Prepare( ctx );
            CStringReqStrncat_TestVariant( ctx );
          }
        }
      }
    }
  }
}

/** @} */
