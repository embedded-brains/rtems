/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringReqMemset
 */

/*
 * Copyright (C) 2021, 2026 embedded brains GmbH & Co. KG
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <limits.h>
#include <rtems.h>
#include <string.h>

#include <rtems/test.h>

/**
 * @defgroup CStringReqMemset spec:/c/string/req/memset
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * The memory area lies in a buffer filled with a byte which differs from the
 * value to set. The long size runs the loop of four blocks twice.
 *
 * @{
 */

typedef enum {
  CStringReqMemset_Pre_N_Zero,
  CStringReqMemset_Pre_N_Positive,
  CStringReqMemset_Pre_N_NA
} CStringReqMemset_Pre_N;

typedef enum {
  CStringReqMemset_Pre_Alignment_Aligned,
  CStringReqMemset_Pre_Alignment_Unaligned,
  CStringReqMemset_Pre_Alignment_NA
} CStringReqMemset_Pre_Alignment;

typedef enum {
  CStringReqMemset_Pre_C_InRange,
  CStringReqMemset_Pre_C_Negative,
  CStringReqMemset_Pre_C_Greater,
  CStringReqMemset_Pre_C_NA
} CStringReqMemset_Pre_C;

typedef enum {
  CStringReqMemset_Post_Result_S,
  CStringReqMemset_Post_Result_NA
} CStringReqMemset_Post_Result;

typedef enum {
  CStringReqMemset_Post_Area_Set,
  CStringReqMemset_Post_Area_NA
} CStringReqMemset_Post_Area;

typedef enum {
  CStringReqMemset_Post_Outside_Nop,
  CStringReqMemset_Post_Outside_NA
} CStringReqMemset_Post_Outside;

typedef enum {
  CStringReqMemset_Post_Errno_Nop,
  CStringReqMemset_Post_Errno_NA
} CStringReqMemset_Post_Errno;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_N_NA : 1;
  uint8_t Pre_Alignment_NA : 1;
  uint8_t Pre_C_NA : 1;
  uint8_t Post_Result : 1;
  uint8_t Post_Area : 1;
  uint8_t Post_Outside : 1;
  uint8_t Post_Errno : 1;
} CStringReqMemset_Entry;

/**
 * @brief Test context for spec:/c/string/req/memset test case.
 */
typedef struct {
  /**
   * @brief This member provides the memory which holds the memory area.
   */
  unsigned char buf[ 16 * sizeof( long ) ] RTEMS_ALIGNED( sizeof( long ) );

  /**
   * @brief This member specifies the `s` parameter value.
   */
  unsigned char *s;

  /**
   * @brief This member specifies the `c` parameter value.
   */
  int c;

  /**
   * @brief This member specifies the `n` parameter value.
   */
  size_t n;

  /**
   * @brief This member contains the return value of the memset() call.
   */
  void *retval;

  /**
   * @brief This member contains the value of errno after the memset() call.
   */
  int errno_value;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 3 ];

    /**
     * @brief This member defines the representative index for the next action.
     */
    size_t rep;

    /**
     * @brief If this member is true, then the current run of the
     *   representatives should be skipped.
     */
    bool skip_rep;

    /**
     * @brief This member records the representatives which ran.
     */
    bool rep_ran[ 28 ];

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
    CStringReqMemset_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringReqMemset_Context;

static CStringReqMemset_Context CStringReqMemset_Instance;

static const char *const CStringReqMemset_PreDesc_N[] = {
  "Zero",
  "PositiveOne",
  "PositiveTwo",
  "PositiveWordMinusTwo",
  "PositiveWordMinusOne",
  "PositiveWord",
  "PositiveWordPlusOne",
  "PositiveWordPlusTwo",
  "PositiveBlockMinusTwo",
  "PositiveBlockMinusOne",
  "PositiveBlock",
  "PositiveBlockPlusOne",
  "PositiveBlockPlusTwo",
  "PositiveTwoBlocks",
  "PositiveMax",
  "NA"
};

static const char *const CStringReqMemset_PreDesc_Alignment[] = {
  "Aligned",
  "UnalignedOne",
  "UnalignedTwo",
  "UnalignedWordMinusTwo",
  "UnalignedWordMinusOne",
  "NA"
};

static const char *const CStringReqMemset_PreDesc_C[] = {
  "InRange",
  "NegativeNear",
  "NegativeMin",
  "GreaterNear",
  "GreaterMax",
  "NA"
};

static const char *const *const CStringReqMemset_PreDesc[] = {
  CStringReqMemset_PreDesc_N,
  CStringReqMemset_PreDesc_Alignment,
  CStringReqMemset_PreDesc_C,
  NULL
};

typedef enum {
  CStringReqMemset_Pre_N_Positive_One,
  CStringReqMemset_Pre_N_Positive_Two,
  CStringReqMemset_Pre_N_Positive_WordMinusTwo,
  CStringReqMemset_Pre_N_Positive_WordMinusOne,
  CStringReqMemset_Pre_N_Positive_Word,
  CStringReqMemset_Pre_N_Positive_WordPlusOne,
  CStringReqMemset_Pre_N_Positive_WordPlusTwo,
  CStringReqMemset_Pre_N_Positive_BlockMinusTwo,
  CStringReqMemset_Pre_N_Positive_BlockMinusOne,
  CStringReqMemset_Pre_N_Positive_Block,
  CStringReqMemset_Pre_N_Positive_BlockPlusOne,
  CStringReqMemset_Pre_N_Positive_BlockPlusTwo,
  CStringReqMemset_Pre_N_Positive_TwoBlocks,
  CStringReqMemset_Pre_N_Positive_Max
} CStringReqMemset_Pre_N_Positive_Rep;

typedef enum {
  CStringReqMemset_Pre_Alignment_Unaligned_One,
  CStringReqMemset_Pre_Alignment_Unaligned_Two,
  CStringReqMemset_Pre_Alignment_Unaligned_WordMinusTwo,
  CStringReqMemset_Pre_Alignment_Unaligned_WordMinusOne
} CStringReqMemset_Pre_Alignment_Unaligned_Rep;

typedef enum {
  CStringReqMemset_Pre_C_Negative_Near,
  CStringReqMemset_Pre_C_Negative_Min
} CStringReqMemset_Pre_C_Negative_Rep;

typedef enum {
  CStringReqMemset_Pre_C_Greater_Near,
  CStringReqMemset_Pre_C_Greater_Max
} CStringReqMemset_Pre_C_Greater_Rep;

static const uint8_t CStringReqMemset_PreRepOffset_N[] = { 0, 1, 15, 16 };

static const uint8_t CStringReqMemset_PreRepOffset_Alignment[] =
  { 0, 1, 5, 6 };

static const uint8_t CStringReqMemset_PreRepOffset_C[] = { 0, 1, 3, 5, 6 };

static const uint8_t *const CStringReqMemset_PreRepOffset[] = {
  CStringReqMemset_PreRepOffset_N,
  CStringReqMemset_PreRepOffset_Alignment,
  CStringReqMemset_PreRepOffset_C
};

static size_t CStringReqMemset_GetRepresentativeCountOf(
  const CStringReqMemset_Context *ctx,
  size_t                          index
)
{
  const uint8_t *offsets;
  size_t         state;

  offsets = CStringReqMemset_PreRepOffset[ index ];
  state = ctx->Map.pcs[ index ];

  return offsets[ state + 1 ] - offsets[ state ];
}

static size_t CStringReqMemset_GetRepresentative(
  const CStringReqMemset_Context *ctx,
  size_t                          index
)
{
  size_t count;

  count = CStringReqMemset_GetRepresentativeCountOf( ctx, index );

  if ( ctx->Map.rep < count ) {
    return ctx->Map.rep;
  }

  return count - 1;
}

static size_t CStringReqMemset_GetRepresentativeCount(
  const CStringReqMemset_Context *ctx
)
{
  size_t count;
  size_t i;

  count = 1;

  for ( i = 0; i < 3; ++i ) {
    size_t count_of_condition;

    count_of_condition = CStringReqMemset_GetRepresentativeCountOf( ctx, i );

    if ( count_of_condition > count ) {
      count = count_of_condition;
    }
  }

  return count;
}

static CStringReqMemset_Pre_N_Positive_Rep
CStringReqMemset_Pre_N_Positive_GetRepresentative(
  const CStringReqMemset_Context *ctx
)
{
  return (
    CStringReqMemset_Pre_N_Positive_Rep
  ) CStringReqMemset_GetRepresentative( ctx, 0 );
}

static CStringReqMemset_Pre_Alignment_Unaligned_Rep
CStringReqMemset_Pre_Alignment_Unaligned_GetRepresentative(
  const CStringReqMemset_Context *ctx
)
{
  return (
    CStringReqMemset_Pre_Alignment_Unaligned_Rep
  ) CStringReqMemset_GetRepresentative( ctx, 1 );
}

static CStringReqMemset_Pre_C_Negative_Rep
CStringReqMemset_Pre_C_Negative_GetRepresentative(
  const CStringReqMemset_Context *ctx
)
{
  return (
    CStringReqMemset_Pre_C_Negative_Rep
  ) CStringReqMemset_GetRepresentative( ctx, 2 );
}

static CStringReqMemset_Pre_C_Greater_Rep
CStringReqMemset_Pre_C_Greater_GetRepresentative(
  const CStringReqMemset_Context *ctx
)
{
  return (
    CStringReqMemset_Pre_C_Greater_Rep
  ) CStringReqMemset_GetRepresentative( ctx, 2 );
}

static const uint8_t CStringReqMemset_PreRepBase[] = { 0, 16, 22 };

static const uint8_t CStringReqMemset_PreRepStates[] = { 2, 2, 3 };

static void CStringReqMemset_ResetRepresentatives(
  CStringReqMemset_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 28; ++i ) {
    ctx->Map.rep_ran[ i ] = false;
  }
}

static void CStringReqMemset_MarkRepresentatives(
  CStringReqMemset_Context *ctx
)
{
  size_t i;

  if ( ctx->Map.skip_rep ) {
    return;
  }

  for ( i = 0; i < 3; ++i ) {
    size_t rep;

    rep = CStringReqMemset_PreRepBase[ i ] +
          CStringReqMemset_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
          CStringReqMemset_GetRepresentative( ctx, i );
    ctx->Map.rep_ran[ rep ] = true;
  }
}

static void CStringReqMemset_CheckRepresentatives(
  const CStringReqMemset_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 3; ++i ) {
    const uint8_t *offsets;
    const bool    *ran;
    size_t         state;

    offsets = CStringReqMemset_PreRepOffset[ i ];
    ran = &ctx->Map.rep_ran[ CStringReqMemset_PreRepBase[ i ] ];

    for ( state = 0; state < CStringReqMemset_PreRepStates[ i ]; ++state ) {
      size_t k;
      bool   any;

      any = false;

      for ( k = offsets[ state ]; k < offsets[ state + 1 ]; ++k ) {
        any = any || ran[ k ];
      }

      if ( any ) {
        for ( k = offsets[ state ]; k < offsets[ state + 1 ]; ++k ) {
          T_quiet_true(
            ran[ k ],
            "representative %s did not run",
            CStringReqMemset_PreDesc[ i ][ k ]
          );
        }
      }
    }
  }
}

#define FILL 0x11

#define TARGET 0xa5

typedef CStringReqMemset_Context Context;

static void CheckArea( const Context *ctx )
{
  size_t i;

  for ( i = 0; i < ctx->n; ++i ) {
    T_quiet_eq_u8( ctx->s[ i ], TARGET );
  }
}

static void CheckOutside( const Context *ctx )
{
  size_t i;
  size_t begin;

  begin = (size_t) ( ctx->s - ctx->buf );

  for ( i = 0; i < sizeof( ctx->buf ); ++i ) {
    if ( i < begin || i >= begin + ctx->n ) {
      T_quiet_eq_u8( ctx->buf[ i ], FILL );
    }
  }
}

static void CStringReqMemset_Pre_N_Prepare(
  CStringReqMemset_Context *ctx,
  CStringReqMemset_Pre_N    state
)
{
  switch ( state ) {
    case CStringReqMemset_Pre_N_Zero: {
      /*
       * While the size specified by `n` is equal to zero.
       */
      ctx->n = 0;
      break;
    }

    case CStringReqMemset_Pre_N_Positive: {
      /*
       * While the size specified by `n` is greater than zero.
       */
      switch ( CStringReqMemset_Pre_N_Positive_GetRepresentative( ctx ) ) {
        case CStringReqMemset_Pre_N_Positive_One: {
          ctx->n = 1;
          break;
        }

        case CStringReqMemset_Pre_N_Positive_Two: {
          ctx->n = 2;
          break;
        }

        case CStringReqMemset_Pre_N_Positive_WordMinusTwo: {
          ctx->n = sizeof( long ) - 2;
          break;
        }

        case CStringReqMemset_Pre_N_Positive_WordMinusOne: {
          ctx->n = sizeof( long ) - 1;
          break;
        }

        case CStringReqMemset_Pre_N_Positive_Word: {
          ctx->n = sizeof( long );
          break;
        }

        case CStringReqMemset_Pre_N_Positive_WordPlusOne: {
          ctx->n = sizeof( long ) + 1;
          break;
        }

        case CStringReqMemset_Pre_N_Positive_WordPlusTwo: {
          ctx->n = sizeof( long ) + 2;
          break;
        }

        case CStringReqMemset_Pre_N_Positive_BlockMinusTwo: {
          ctx->n = 4 * sizeof( long ) - 2;
          break;
        }

        case CStringReqMemset_Pre_N_Positive_BlockMinusOne: {
          ctx->n = 4 * sizeof( long ) - 1;
          break;
        }

        case CStringReqMemset_Pre_N_Positive_Block: {
          ctx->n = 4 * sizeof( long );
          break;
        }

        case CStringReqMemset_Pre_N_Positive_BlockPlusOne: {
          ctx->n = 4 * sizeof( long ) + 1;
          break;
        }

        case CStringReqMemset_Pre_N_Positive_BlockPlusTwo: {
          ctx->n = 4 * sizeof( long ) + 2;
          break;
        }

        case CStringReqMemset_Pre_N_Positive_TwoBlocks: {
          ctx->n = 9 * sizeof( long ) + 1;
          break;
        }

        case CStringReqMemset_Pre_N_Positive_Max: {
          ctx->n = 12 * sizeof( long );
          break;
        }
      }
      break;
    }

    case CStringReqMemset_Pre_N_NA:
      break;
  }
}

static void CStringReqMemset_Pre_Alignment_Prepare(
  CStringReqMemset_Context      *ctx,
  CStringReqMemset_Pre_Alignment state
)
{
  switch ( state ) {
    case CStringReqMemset_Pre_Alignment_Aligned: {
      /*
       * While the memory area referenced by the `s` parameter starts at an
       * address which is aligned to the size of type long.
       */
      ctx->s = &ctx->buf[ 2 * sizeof( long ) ];
      break;
    }

    case CStringReqMemset_Pre_Alignment_Unaligned: {
      /*
       * While the memory area referenced by the `s` parameter starts at an
       * address which is not aligned to the size of type long.
       */
      switch (
        CStringReqMemset_Pre_Alignment_Unaligned_GetRepresentative( ctx )
      ) {
        case CStringReqMemset_Pre_Alignment_Unaligned_One: {
          ctx->s = &ctx->buf[ 2 * sizeof( long ) + 1 ];
          break;
        }

        case CStringReqMemset_Pre_Alignment_Unaligned_Two: {
          ctx->s = &ctx->buf[ 2 * sizeof( long ) + 2 ];
          break;
        }

        case CStringReqMemset_Pre_Alignment_Unaligned_WordMinusTwo: {
          ctx->s = &ctx->buf[ 2 * sizeof( long ) + sizeof( long ) - 2 ];
          break;
        }

        case CStringReqMemset_Pre_Alignment_Unaligned_WordMinusOne: {
          ctx->s = &ctx->buf[ 2 * sizeof( long ) + sizeof( long ) - 1 ];
          break;
        }
      }
      break;
    }

    case CStringReqMemset_Pre_Alignment_NA:
      break;
  }
}

static void CStringReqMemset_Pre_C_Prepare(
  CStringReqMemset_Context *ctx,
  CStringReqMemset_Pre_C    state
)
{
  switch ( state ) {
    case CStringReqMemset_Pre_C_InRange: {
      /*
       * While the value of the `c` parameter is in the range of the type
       * `unsigned char`.
       */
      ctx->c = TARGET;
      break;
    }

    case CStringReqMemset_Pre_C_Negative: {
      /*
       * While the value of the `c` parameter is less than zero.
       */
      switch ( CStringReqMemset_Pre_C_Negative_GetRepresentative( ctx ) ) {
        case CStringReqMemset_Pre_C_Negative_Near: {
          ctx->c = TARGET - ( UCHAR_MAX + 1 );
          break;
        }

        case CStringReqMemset_Pre_C_Negative_Min: {
          ctx->c = INT_MIN + TARGET;
          break;
        }
      }
      break;
    }

    case CStringReqMemset_Pre_C_Greater: {
      /*
       * While the value of the `c` parameter is greater than the maximum value
       * of the type `unsigned char`.
       */
      switch ( CStringReqMemset_Pre_C_Greater_GetRepresentative( ctx ) ) {
        case CStringReqMemset_Pre_C_Greater_Near: {
          ctx->c = TARGET + ( UCHAR_MAX + 1 );
          break;
        }

        case CStringReqMemset_Pre_C_Greater_Max: {
          ctx->c = INT_MAX - UCHAR_MAX + TARGET;
          break;
        }
      }
      break;
    }

    case CStringReqMemset_Pre_C_NA:
      break;
  }
}

static void CStringReqMemset_Post_Result_Check(
  CStringReqMemset_Context    *ctx,
  CStringReqMemset_Post_Result state
)
{
  switch ( state ) {
    case CStringReqMemset_Post_Result_S: {
      /*
       * The return value of memset() shall be the value of the `s` parameter.
       */
      T_eq_ptr( ctx->retval, ctx->s );
      break;
    }

    case CStringReqMemset_Post_Result_NA:
      break;
  }
}

static void CStringReqMemset_Post_Area_Check(
  CStringReqMemset_Context  *ctx,
  CStringReqMemset_Post_Area state
)
{
  switch ( state ) {
    case CStringReqMemset_Post_Area_Set: {
      /*
       * Each of the first `n` bytes of the memory area referenced by the `s`
       * parameter shall be set to the value of the `c` parameter converted to
       * `unsigned char`.
       */
      CheckArea( ctx );
      break;
    }

    case CStringReqMemset_Post_Area_NA:
      break;
  }
}

static void CStringReqMemset_Post_Outside_Check(
  CStringReqMemset_Context     *ctx,
  CStringReqMemset_Post_Outside state
)
{
  switch ( state ) {
    case CStringReqMemset_Post_Outside_Nop: {
      /*
       * The bytes outside the first `n` bytes of the memory area referenced by
       * the `s` parameter shall not be modified.
       */
      CheckOutside( ctx );
      break;
    }

    case CStringReqMemset_Post_Outside_NA:
      break;
  }
}

static void CStringReqMemset_Post_Errno_Check(
  CStringReqMemset_Context   *ctx,
  CStringReqMemset_Post_Errno state
)
{
  switch ( state ) {
    case CStringReqMemset_Post_Errno_Nop: {
      /*
       * The value of errno shall not be modified.
       */
      T_eq_int( ctx->errno_value, -1 );
      break;
    }

    case CStringReqMemset_Post_Errno_NA:
      break;
  }
}

static void CStringReqMemset_Prepare( CStringReqMemset_Context *ctx )
{
  memset( ctx->buf, FILL, sizeof( ctx->buf ) );
  ctx->s = NULL;
  ctx->c = 0;
  ctx->n = 0;
  ctx->retval = NULL;
}

static void CStringReqMemset_Action( CStringReqMemset_Context *ctx )
{
  errno = -1;
  ctx->retval = memset( ctx->s, ctx->c, ctx->n );
  ctx->errno_value = errno;
}

/* clang-format off */

static const CStringReqMemset_Entry
CStringReqMemset_Entries[] = {
  { 0, 0, 0, 0, CStringReqMemset_Post_Result_S, CStringReqMemset_Post_Area_NA,
    CStringReqMemset_Post_Outside_Nop, CStringReqMemset_Post_Errno_Nop },
  { 0, 0, 0, 0, CStringReqMemset_Post_Result_S, CStringReqMemset_Post_Area_Set,
    CStringReqMemset_Post_Outside_Nop, CStringReqMemset_Post_Errno_Nop }
};

static const uint8_t
CStringReqMemset_Map[] = {
  0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1
};

/* clang-format on */

static size_t CStringReqMemset_Scope( void *arg, char *buf, size_t n )
{
  CStringReqMemset_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    size_t pcd[ 3 ];
    size_t i;

    for ( i = 0; i < 3; ++i ) {
      pcd[ i ] = CStringReqMemset_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
                 CStringReqMemset_GetRepresentative( ctx, i );
    }

    return T_get_scope( CStringReqMemset_PreDesc, buf, n, pcd );
  }

  return 0;
}

static T_fixture CStringReqMemset_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringReqMemset_Scope,
  .initial_context = &CStringReqMemset_Instance
};

static inline CStringReqMemset_Entry CStringReqMemset_PopEntry(
  CStringReqMemset_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringReqMemset_Entries[ CStringReqMemset_Map[ index ] ];
}

static void CStringReqMemset_TestVariant( CStringReqMemset_Context *ctx )
{
  CStringReqMemset_Pre_N_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringReqMemset_Pre_Alignment_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CStringReqMemset_Pre_C_Prepare( ctx, ctx->Map.pcs[ 2 ] );

  if ( ctx->Map.skip_rep ) {
    return;
  }

  CStringReqMemset_Action( ctx );
  CStringReqMemset_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
  CStringReqMemset_Post_Area_Check( ctx, ctx->Map.entry.Post_Area );
  CStringReqMemset_Post_Outside_Check( ctx, ctx->Map.entry.Post_Outside );
  CStringReqMemset_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
}

/**
 * @fn void T_case_body_CStringReqMemset( void )
 */
T_TEST_CASE_FIXTURE( CStringReqMemset, &CStringReqMemset_Fixture )
{
  CStringReqMemset_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  CStringReqMemset_ResetRepresentatives( ctx );

  for (
    ctx->Map.pcs[ 0 ] = CStringReqMemset_Pre_N_Zero;
    ctx->Map.pcs[ 0 ] < CStringReqMemset_Pre_N_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CStringReqMemset_Pre_Alignment_Aligned;
      ctx->Map.pcs[ 1 ] < CStringReqMemset_Pre_Alignment_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = CStringReqMemset_Pre_C_InRange;
        ctx->Map.pcs[ 2 ] < CStringReqMemset_Pre_C_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = CStringReqMemset_PopEntry( ctx );

        for (
          ctx->Map.rep = 0;
          ctx->Map.rep < CStringReqMemset_GetRepresentativeCount( ctx );
          ++ctx->Map.rep
        ) {
          ctx->Map.skip_rep = false;
          CStringReqMemset_Prepare( ctx );
          CStringReqMemset_TestVariant( ctx );

          CStringReqMemset_MarkRepresentatives( ctx );
        }
      }
    }
  }

  CStringReqMemset_CheckRepresentatives( ctx );
}

/** @} */
