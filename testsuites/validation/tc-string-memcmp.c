/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringReqMemcmp
 */

/*
 * Copyright (C) 2025, 2026 embedded brains GmbH & Co. KG
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
#include <rtems.h>
#include <string.h>

#include <rtems/test.h>

/**
 * @defgroup CStringReqMemcmp spec:/c/string/req/memcmp
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * Both memory areas start inside buffers filled with the same byte. The first
 * differing byte is the first or the last byte. Its pair holds one byte below
 * 0x80 and one byte above 0x80, so a comparison as signed char inverts the
 * order. Equal memory areas differ in the byte which follows them.
 *
 * @{
 */

typedef enum {
  CStringReqMemcmp_Pre_N_Zero,
  CStringReqMemcmp_Pre_N_One,
  CStringReqMemcmp_Pre_N_More,
  CStringReqMemcmp_Pre_N_NA
} CStringReqMemcmp_Pre_N;

typedef enum {
  CStringReqMemcmp_Pre_S1Alignment_Aligned,
  CStringReqMemcmp_Pre_S1Alignment_Unaligned,
  CStringReqMemcmp_Pre_S1Alignment_NA
} CStringReqMemcmp_Pre_S1Alignment;

typedef enum {
  CStringReqMemcmp_Pre_S2Alignment_Aligned,
  CStringReqMemcmp_Pre_S2Alignment_Unaligned,
  CStringReqMemcmp_Pre_S2Alignment_NA
} CStringReqMemcmp_Pre_S2Alignment;

typedef enum {
  CStringReqMemcmp_Pre_Position_Before,
  CStringReqMemcmp_Pre_Position_Last,
  CStringReqMemcmp_Pre_Position_NA
} CStringReqMemcmp_Pre_Position;

typedef enum {
  CStringReqMemcmp_Pre_Diff_Equal,
  CStringReqMemcmp_Pre_Diff_Less,
  CStringReqMemcmp_Pre_Diff_Greater,
  CStringReqMemcmp_Pre_Diff_NA
} CStringReqMemcmp_Pre_Diff;

typedef enum {
  CStringReqMemcmp_Pre_Rest_Equal,
  CStringReqMemcmp_Pre_Rest_Differs,
  CStringReqMemcmp_Pre_Rest_NA
} CStringReqMemcmp_Pre_Rest;

typedef enum {
  CStringReqMemcmp_Post_Result_Zero,
  CStringReqMemcmp_Post_Result_Negative,
  CStringReqMemcmp_Post_Result_Positive,
  CStringReqMemcmp_Post_Result_NA
} CStringReqMemcmp_Post_Result;

typedef enum {
  CStringReqMemcmp_Post_S1_Nop,
  CStringReqMemcmp_Post_S1_NA
} CStringReqMemcmp_Post_S1;

typedef enum {
  CStringReqMemcmp_Post_S2_Nop,
  CStringReqMemcmp_Post_S2_NA
} CStringReqMemcmp_Post_S2;

typedef enum {
  CStringReqMemcmp_Post_Errno_Nop,
  CStringReqMemcmp_Post_Errno_NA
} CStringReqMemcmp_Post_Errno;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_N_NA : 1;
  uint16_t Pre_S1Alignment_NA : 1;
  uint16_t Pre_S2Alignment_NA : 1;
  uint16_t Pre_Position_NA : 1;
  uint16_t Pre_Diff_NA : 1;
  uint16_t Pre_Rest_NA : 1;
  uint16_t Post_Result : 2;
  uint16_t Post_S1 : 1;
  uint16_t Post_S2 : 1;
  uint16_t Post_Errno : 1;
} CStringReqMemcmp_Entry;

/**
 * @brief Test context for spec:/c/string/req/memcmp test case.
 */
typedef struct {
  /**
   * @brief This member provides the memory which holds the first memory area.
   */
  unsigned char buf1[ 16 * sizeof( long ) ] RTEMS_ALIGNED( sizeof( long ) );

  /**
   * @brief This member provides the memory which holds the second memory area.
   */
  unsigned char buf2[ 16 * sizeof( long ) ] RTEMS_ALIGNED( sizeof( long ) );

  /**
   * @brief This member contains the content of the memory which holds the
   *   memory area referenced by the `s1` parameter before the memcmp() call.
   */
  unsigned char before1[ 16 * sizeof( long ) ];

  /**
   * @brief This member contains the content of the memory which holds the
   *   memory area referenced by the `s2` parameter before the memcmp() call.
   */
  unsigned char before2[ 16 * sizeof( long ) ];

  /**
   * @brief This member specifies the `s1` parameter value.
   */
  unsigned char *s1;

  /**
   * @brief This member specifies the `s2` parameter value.
   */
  unsigned char *s2;

  /**
   * @brief This member specifies the index of the first differing byte.
   */
  size_t pos;

  /**
   * @brief This member specifies the offset of the memory area referenced by
   *   the `s1` parameter from an aligned address.
   */
  size_t s1_misalign;

  /**
   * @brief This member specifies the offset of the memory area referenced by
   *   the `s2` parameter from an aligned address.
   */
  size_t s2_misalign;

  /**
   * @brief This member specifies the `n` parameter value.
   */
  size_t n;

  /**
   * @brief This member contains the return value of the memcmp() call.
   */
  int retval;

  /**
   * @brief This member contains the value of errno after the memcmp() call.
   */
  int errno_value;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 6 ];

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
    bool rep_ran[ 36 ];

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

static CStringReqMemcmp_Context CStringReqMemcmp_Instance;

static const char *const CStringReqMemcmp_PreDesc_N[] = {
  "Zero",
  "One",
  "MoreTwo",
  "MoreWordMinusTwo",
  "MoreWordMinusOne",
  "MoreWord",
  "MoreWordPlusOne",
  "MoreWordPlusTwo",
  "MoreFiveWords",
  "MoreMax",
  "NA"
};

static const char *const CStringReqMemcmp_PreDesc_S1Alignment[] = {
  "Aligned",
  "UnalignedOne",
  "UnalignedTwo",
  "UnalignedWordMinusTwo",
  "UnalignedWordMinusOne",
  "NA"
};

static const char *const CStringReqMemcmp_PreDesc_S2Alignment[] = {
  "Aligned",
  "UnalignedOne",
  "UnalignedTwo",
  "UnalignedWordMinusTwo",
  "UnalignedWordMinusOne",
  "NA"
};

static const char *const CStringReqMemcmp_PreDesc_Position[] =
  { "BeforeFirst", "BeforeLate", "Last", "NA" };

static const char *const CStringReqMemcmp_PreDesc_Diff[] = {
  "Equal",
  "LessWide",
  "LessAdjacent",
  "GreaterWide",
  "GreaterAdjacent",
  "NA"
};

static const char *const CStringReqMemcmp_PreDesc_Rest[] =
  { "Equal", "Differs", "NA" };

static const char *const *const CStringReqMemcmp_PreDesc[] = {
  CStringReqMemcmp_PreDesc_N,
  CStringReqMemcmp_PreDesc_S1Alignment,
  CStringReqMemcmp_PreDesc_S2Alignment,
  CStringReqMemcmp_PreDesc_Position,
  CStringReqMemcmp_PreDesc_Diff,
  CStringReqMemcmp_PreDesc_Rest,
  NULL
};

typedef enum {
  CStringReqMemcmp_Pre_N_More_Two,
  CStringReqMemcmp_Pre_N_More_WordMinusTwo,
  CStringReqMemcmp_Pre_N_More_WordMinusOne,
  CStringReqMemcmp_Pre_N_More_Word,
  CStringReqMemcmp_Pre_N_More_WordPlusOne,
  CStringReqMemcmp_Pre_N_More_WordPlusTwo,
  CStringReqMemcmp_Pre_N_More_FiveWords,
  CStringReqMemcmp_Pre_N_More_Max
} CStringReqMemcmp_Pre_N_More_Rep;

typedef enum {
  CStringReqMemcmp_Pre_S1Alignment_Unaligned_One,
  CStringReqMemcmp_Pre_S1Alignment_Unaligned_Two,
  CStringReqMemcmp_Pre_S1Alignment_Unaligned_WordMinusTwo,
  CStringReqMemcmp_Pre_S1Alignment_Unaligned_WordMinusOne
} CStringReqMemcmp_Pre_S1Alignment_Unaligned_Rep;

typedef enum {
  CStringReqMemcmp_Pre_S2Alignment_Unaligned_One,
  CStringReqMemcmp_Pre_S2Alignment_Unaligned_Two,
  CStringReqMemcmp_Pre_S2Alignment_Unaligned_WordMinusTwo,
  CStringReqMemcmp_Pre_S2Alignment_Unaligned_WordMinusOne
} CStringReqMemcmp_Pre_S2Alignment_Unaligned_Rep;

typedef enum {
  CStringReqMemcmp_Pre_Position_Before_First,
  CStringReqMemcmp_Pre_Position_Before_Late
} CStringReqMemcmp_Pre_Position_Before_Rep;

typedef enum {
  CStringReqMemcmp_Pre_Diff_Less_Wide,
  CStringReqMemcmp_Pre_Diff_Less_Adjacent
} CStringReqMemcmp_Pre_Diff_Less_Rep;

typedef enum {
  CStringReqMemcmp_Pre_Diff_Greater_Wide,
  CStringReqMemcmp_Pre_Diff_Greater_Adjacent
} CStringReqMemcmp_Pre_Diff_Greater_Rep;

static const uint8_t CStringReqMemcmp_PreRepOffset_N[] = { 0, 1, 2, 10, 11 };

static const uint8_t CStringReqMemcmp_PreRepOffset_S1Alignment[] =
  { 0, 1, 5, 6 };

static const uint8_t CStringReqMemcmp_PreRepOffset_S2Alignment[] =
  { 0, 1, 5, 6 };

static const uint8_t CStringReqMemcmp_PreRepOffset_Position[] = { 0, 2, 3, 4 };

static const uint8_t CStringReqMemcmp_PreRepOffset_Diff[] = { 0, 1, 3, 5, 6 };

static const uint8_t CStringReqMemcmp_PreRepOffset_Rest[] = { 0, 1, 2, 3 };

static const uint8_t *const CStringReqMemcmp_PreRepOffset[] = {
  CStringReqMemcmp_PreRepOffset_N,
  CStringReqMemcmp_PreRepOffset_S1Alignment,
  CStringReqMemcmp_PreRepOffset_S2Alignment,
  CStringReqMemcmp_PreRepOffset_Position,
  CStringReqMemcmp_PreRepOffset_Diff,
  CStringReqMemcmp_PreRepOffset_Rest
};

static size_t CStringReqMemcmp_GetRepresentativeCountOf(
  const CStringReqMemcmp_Context *ctx,
  size_t                          index
)
{
  const uint8_t *offsets;
  size_t         state;

  offsets = CStringReqMemcmp_PreRepOffset[ index ];
  state = ctx->Map.pcs[ index ];

  return offsets[ state + 1 ] - offsets[ state ];
}

static size_t CStringReqMemcmp_GetRepresentative(
  const CStringReqMemcmp_Context *ctx,
  size_t                          index
)
{
  size_t count;

  count = CStringReqMemcmp_GetRepresentativeCountOf( ctx, index );

  if ( ctx->Map.rep < count ) {
    return ctx->Map.rep;
  }

  return count - 1;
}

static size_t CStringReqMemcmp_GetRepresentativeCount(
  const CStringReqMemcmp_Context *ctx
)
{
  size_t count;
  size_t i;

  count = 1;

  for ( i = 0; i < 6; ++i ) {
    size_t count_of_condition;

    count_of_condition = CStringReqMemcmp_GetRepresentativeCountOf( ctx, i );

    if ( count_of_condition > count ) {
      count = count_of_condition;
    }
  }

  return count;
}

static CStringReqMemcmp_Pre_N_More_Rep
CStringReqMemcmp_Pre_N_More_GetRepresentative(
  const CStringReqMemcmp_Context *ctx
)
{
  return (
    CStringReqMemcmp_Pre_N_More_Rep
  ) CStringReqMemcmp_GetRepresentative( ctx, 0 );
}

static CStringReqMemcmp_Pre_S1Alignment_Unaligned_Rep
CStringReqMemcmp_Pre_S1Alignment_Unaligned_GetRepresentative(
  const CStringReqMemcmp_Context *ctx
)
{
  return (
    CStringReqMemcmp_Pre_S1Alignment_Unaligned_Rep
  ) CStringReqMemcmp_GetRepresentative( ctx, 1 );
}

static CStringReqMemcmp_Pre_S2Alignment_Unaligned_Rep
CStringReqMemcmp_Pre_S2Alignment_Unaligned_GetRepresentative(
  const CStringReqMemcmp_Context *ctx
)
{
  return (
    CStringReqMemcmp_Pre_S2Alignment_Unaligned_Rep
  ) CStringReqMemcmp_GetRepresentative( ctx, 2 );
}

static CStringReqMemcmp_Pre_Position_Before_Rep
CStringReqMemcmp_Pre_Position_Before_GetRepresentative(
  const CStringReqMemcmp_Context *ctx
)
{
  return (
    CStringReqMemcmp_Pre_Position_Before_Rep
  ) CStringReqMemcmp_GetRepresentative( ctx, 3 );
}

static CStringReqMemcmp_Pre_Diff_Less_Rep
CStringReqMemcmp_Pre_Diff_Less_GetRepresentative(
  const CStringReqMemcmp_Context *ctx
)
{
  return (
    CStringReqMemcmp_Pre_Diff_Less_Rep
  ) CStringReqMemcmp_GetRepresentative( ctx, 4 );
}

static CStringReqMemcmp_Pre_Diff_Greater_Rep
CStringReqMemcmp_Pre_Diff_Greater_GetRepresentative(
  const CStringReqMemcmp_Context *ctx
)
{
  return (
    CStringReqMemcmp_Pre_Diff_Greater_Rep
  ) CStringReqMemcmp_GetRepresentative( ctx, 4 );
}

static const uint8_t CStringReqMemcmp_PreRepBase[] = { 0, 11, 17, 23, 27, 33 };

static const uint8_t CStringReqMemcmp_PreRepStates[] = { 3, 2, 2, 2, 3, 2 };

static void CStringReqMemcmp_ResetRepresentatives(
  CStringReqMemcmp_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 36; ++i ) {
    ctx->Map.rep_ran[ i ] = false;
  }
}

static void CStringReqMemcmp_MarkRepresentatives(
  CStringReqMemcmp_Context *ctx
)
{
  size_t i;

  if ( ctx->Map.skip_rep ) {
    return;
  }

  for ( i = 0; i < 6; ++i ) {
    size_t rep;

    rep = CStringReqMemcmp_PreRepBase[ i ] +
          CStringReqMemcmp_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
          CStringReqMemcmp_GetRepresentative( ctx, i );
    ctx->Map.rep_ran[ rep ] = true;
  }
}

static void CStringReqMemcmp_CheckRepresentatives(
  const CStringReqMemcmp_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 6; ++i ) {
    const uint8_t *offsets;
    const bool    *ran;
    size_t         state;

    offsets = CStringReqMemcmp_PreRepOffset[ i ];
    ran = &ctx->Map.rep_ran[ CStringReqMemcmp_PreRepBase[ i ] ];

    for ( state = 0; state < CStringReqMemcmp_PreRepStates[ i ]; ++state ) {
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
            CStringReqMemcmp_PreDesc[ i ][ k ]
          );
        }
      }
    }
  }
}

#define FILL 0x5a

#define LOW 0x11

#define HIGH 0xa5

static void CheckUnchanged(
  const unsigned char *actual,
  const unsigned char *expected,
  size_t               size
)
{
  size_t i;

  for ( i = 0; i < size; ++i ) {
    T_quiet_eq_u8( actual[ i ], expected[ i ] );
  }
}

static void CStringReqMemcmp_Pre_N_Prepare(
  CStringReqMemcmp_Context *ctx,
  CStringReqMemcmp_Pre_N    state
)
{
  switch ( state ) {
    case CStringReqMemcmp_Pre_N_Zero: {
      /*
       * While the size specified by `n` is equal to zero.
       */
      ctx->n = 0;
      break;
    }

    case CStringReqMemcmp_Pre_N_One: {
      /*
       * While the size specified by `n` is equal to one.
       */
      ctx->n = 1;
      break;
    }

    case CStringReqMemcmp_Pre_N_More: {
      /*
       * While the size specified by `n` is greater than one.
       */
      switch ( CStringReqMemcmp_Pre_N_More_GetRepresentative( ctx ) ) {
        case CStringReqMemcmp_Pre_N_More_Two: {
          ctx->n = 2;
          break;
        }

        case CStringReqMemcmp_Pre_N_More_WordMinusTwo: {
          ctx->n = sizeof( long ) - 2;
          break;
        }

        case CStringReqMemcmp_Pre_N_More_WordMinusOne: {
          ctx->n = sizeof( long ) - 1;
          break;
        }

        case CStringReqMemcmp_Pre_N_More_Word: {
          ctx->n = sizeof( long );
          break;
        }

        case CStringReqMemcmp_Pre_N_More_WordPlusOne: {
          ctx->n = sizeof( long ) + 1;
          break;
        }

        case CStringReqMemcmp_Pre_N_More_WordPlusTwo: {
          ctx->n = sizeof( long ) + 2;
          break;
        }

        case CStringReqMemcmp_Pre_N_More_FiveWords: {
          ctx->n = 5 * sizeof( long ) + 1;
          break;
        }

        case CStringReqMemcmp_Pre_N_More_Max: {
          ctx->n = 13 * sizeof( long );
          break;
        }
      }
      break;
    }

    case CStringReqMemcmp_Pre_N_NA:
      break;
  }
}

static void CStringReqMemcmp_Pre_S1Alignment_Prepare(
  CStringReqMemcmp_Context        *ctx,
  CStringReqMemcmp_Pre_S1Alignment state
)
{
  switch ( state ) {
    case CStringReqMemcmp_Pre_S1Alignment_Aligned: {
      /*
       * While the memory area referenced by the `s1` parameter starts at an
       * address which is aligned to the size of type long.
       */
      ctx->s1_misalign = 0;
      break;
    }

    case CStringReqMemcmp_Pre_S1Alignment_Unaligned: {
      /*
       * While the memory area referenced by the `s1` parameter starts at an
       * address which is not aligned to the size of type long.
       */
      switch (
        CStringReqMemcmp_Pre_S1Alignment_Unaligned_GetRepresentative( ctx )
      ) {
        case CStringReqMemcmp_Pre_S1Alignment_Unaligned_One: {
          ctx->s1_misalign = 1;
          break;
        }

        case CStringReqMemcmp_Pre_S1Alignment_Unaligned_Two: {
          ctx->s1_misalign = 2;
          break;
        }

        case CStringReqMemcmp_Pre_S1Alignment_Unaligned_WordMinusTwo: {
          ctx->s1_misalign = sizeof( long ) - 2;
          break;
        }

        case CStringReqMemcmp_Pre_S1Alignment_Unaligned_WordMinusOne: {
          ctx->s1_misalign = sizeof( long ) - 1;
          break;
        }
      }
      break;
    }

    case CStringReqMemcmp_Pre_S1Alignment_NA:
      break;
  }
}

static void CStringReqMemcmp_Pre_S2Alignment_Prepare(
  CStringReqMemcmp_Context        *ctx,
  CStringReqMemcmp_Pre_S2Alignment state
)
{
  switch ( state ) {
    case CStringReqMemcmp_Pre_S2Alignment_Aligned: {
      /*
       * While the memory area referenced by the `s2` parameter starts at an
       * address which is aligned to the size of type long.
       */
      ctx->s2_misalign = 0;
      break;
    }

    case CStringReqMemcmp_Pre_S2Alignment_Unaligned: {
      /*
       * While the memory area referenced by the `s2` parameter starts at an
       * address which is not aligned to the size of type long.
       */
      switch (
        CStringReqMemcmp_Pre_S2Alignment_Unaligned_GetRepresentative( ctx )
      ) {
        case CStringReqMemcmp_Pre_S2Alignment_Unaligned_One: {
          ctx->s2_misalign = 1;
          break;
        }

        case CStringReqMemcmp_Pre_S2Alignment_Unaligned_Two: {
          ctx->s2_misalign = 2;
          break;
        }

        case CStringReqMemcmp_Pre_S2Alignment_Unaligned_WordMinusTwo: {
          ctx->s2_misalign = sizeof( long ) - 2;
          break;
        }

        case CStringReqMemcmp_Pre_S2Alignment_Unaligned_WordMinusOne: {
          ctx->s2_misalign = sizeof( long ) - 1;
          break;
        }
      }
      break;
    }

    case CStringReqMemcmp_Pre_S2Alignment_NA:
      break;
  }

  ctx->s1 = &ctx->buf1[ sizeof( long ) + ctx->s1_misalign ];
  ctx->s2 = &ctx->buf2[ sizeof( long ) + ctx->s2_misalign ];
}

static void CStringReqMemcmp_Pre_Position_Prepare(
  CStringReqMemcmp_Context     *ctx,
  CStringReqMemcmp_Pre_Position state
)
{
  switch ( state ) {
    case CStringReqMemcmp_Pre_Position_Before: {
      /*
       * While a byte which precedes the last byte of the first `n` bytes of
       * the memory area referenced by the `s1` parameter differs from the byte
       * at the same position of the memory area referenced by the `s2`
       * parameter.
       */
      switch (
        CStringReqMemcmp_Pre_Position_Before_GetRepresentative( ctx )
      ) {
        case CStringReqMemcmp_Pre_Position_Before_First: {
          ctx->pos = 0;
          break;
        }

        case CStringReqMemcmp_Pre_Position_Before_Late: {
          ctx->pos = ctx->n - 2;
          break;
        }
      }
      break;
    }

    case CStringReqMemcmp_Pre_Position_Last: {
      /*
       * While each byte which precedes the last byte of the first `n` bytes of
       * the memory area referenced by the `s1` parameter is equal to the byte
       * at the same position of the memory area referenced by the `s2`
       * parameter.
       */
      ctx->pos = ctx->n - 1;
      break;
    }

    case CStringReqMemcmp_Pre_Position_NA:
      break;
  }
}

static void CStringReqMemcmp_Pre_Diff_Prepare(
  CStringReqMemcmp_Context *ctx,
  CStringReqMemcmp_Pre_Diff state
)
{
  switch ( state ) {
    case CStringReqMemcmp_Pre_Diff_Equal: {
      /*
       * While each byte of the first `n` bytes of the memory area referenced
       * by the `s1` parameter is equal to the byte at the same position of the
       * memory area referenced by the `s2` parameter.
       */
      ctx->s1[ ctx->n ] = LOW;
      ctx->s2[ ctx->n ] = HIGH;
      break;
    }

    case CStringReqMemcmp_Pre_Diff_Less: {
      /*
       * While the value as `unsigned char` of the first byte of the first `n`
       * bytes of the memory area referenced by the `s1` parameter which
       * differs from the byte at the same position of the memory area
       * referenced by the `s2` parameter is less than the value as `unsigned
       * char` of that byte.
       */
      switch ( CStringReqMemcmp_Pre_Diff_Less_GetRepresentative( ctx ) ) {
        case CStringReqMemcmp_Pre_Diff_Less_Wide: {
          ctx->s1[ ctx->pos ] = LOW;
          ctx->s2[ ctx->pos ] = HIGH;
          break;
        }

        case CStringReqMemcmp_Pre_Diff_Less_Adjacent: {
          ctx->s1[ ctx->pos ] = HIGH - 1;
          ctx->s2[ ctx->pos ] = HIGH;
          break;
        }
      }
      break;
    }

    case CStringReqMemcmp_Pre_Diff_Greater: {
      /*
       * While the value as `unsigned char` of the first byte of the first `n`
       * bytes of the memory area referenced by the `s1` parameter which
       * differs from the byte at the same position of the memory area
       * referenced by the `s2` parameter is greater than the value as
       * `unsigned char` of that byte.
       */
      switch ( CStringReqMemcmp_Pre_Diff_Greater_GetRepresentative( ctx ) ) {
        case CStringReqMemcmp_Pre_Diff_Greater_Wide: {
          ctx->s1[ ctx->pos ] = HIGH;
          ctx->s2[ ctx->pos ] = LOW;
          break;
        }

        case CStringReqMemcmp_Pre_Diff_Greater_Adjacent: {
          ctx->s1[ ctx->pos ] = HIGH;
          ctx->s2[ ctx->pos ] = HIGH - 1;
          break;
        }
      }
      break;
    }

    case CStringReqMemcmp_Pre_Diff_NA:
      break;
  }
}

static void CStringReqMemcmp_Pre_Rest_Prepare(
  CStringReqMemcmp_Context *ctx,
  CStringReqMemcmp_Pre_Rest state
)
{
  switch ( state ) {
    case CStringReqMemcmp_Pre_Rest_Equal: {
      /*
       * While each byte after the first differing byte of the first `n` bytes
       * of the memory area referenced by the `s1` parameter is equal to the
       * byte at the same position of the memory area referenced by the `s2`
       * parameter.
       */
      /* The prepare fills both memory areas with the same byte */
      break;
    }

    case CStringReqMemcmp_Pre_Rest_Differs: {
      /*
       * While a byte after the first differing byte of the first `n` bytes of
       * the memory area referenced by the `s1` parameter differs from the byte
       * at the same position of the memory area referenced by the `s2`
       * parameter.
       */
      ctx->s1[ ctx->n - 1 ] = ctx->s2[ ctx->pos ];
      ctx->s2[ ctx->n - 1 ] = ctx->s1[ ctx->pos ];
      break;
    }

    case CStringReqMemcmp_Pre_Rest_NA:
      break;
  }
}

static void CStringReqMemcmp_Post_Result_Check(
  CStringReqMemcmp_Context    *ctx,
  CStringReqMemcmp_Post_Result state
)
{
  switch ( state ) {
    case CStringReqMemcmp_Post_Result_Zero: {
      /*
       * The return value of memcmp() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CStringReqMemcmp_Post_Result_Negative: {
      /*
       * The return value of memcmp() shall be less than zero.
       */
      T_lt_int( ctx->retval, 0 );
      break;
    }

    case CStringReqMemcmp_Post_Result_Positive: {
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

static void CStringReqMemcmp_Post_S1_Check(
  CStringReqMemcmp_Context *ctx,
  CStringReqMemcmp_Post_S1  state
)
{
  switch ( state ) {
    case CStringReqMemcmp_Post_S1_Nop: {
      /*
       * The bytes of the memory area referenced by the `s1` parameter shall
       * not be modified.
       */
      CheckUnchanged( ctx->buf1, ctx->before1, sizeof( ctx->buf1 ) );
      break;
    }

    case CStringReqMemcmp_Post_S1_NA:
      break;
  }
}

static void CStringReqMemcmp_Post_S2_Check(
  CStringReqMemcmp_Context *ctx,
  CStringReqMemcmp_Post_S2  state
)
{
  switch ( state ) {
    case CStringReqMemcmp_Post_S2_Nop: {
      /*
       * The bytes of the memory area referenced by the `s2` parameter shall
       * not be modified.
       */
      CheckUnchanged( ctx->buf2, ctx->before2, sizeof( ctx->buf2 ) );
      break;
    }

    case CStringReqMemcmp_Post_S2_NA:
      break;
  }
}

static void CStringReqMemcmp_Post_Errno_Check(
  CStringReqMemcmp_Context   *ctx,
  CStringReqMemcmp_Post_Errno state
)
{
  switch ( state ) {
    case CStringReqMemcmp_Post_Errno_Nop: {
      /*
       * The value of errno shall not be modified.
       */
      T_eq_int( ctx->errno_value, -1 );
      break;
    }

    case CStringReqMemcmp_Post_Errno_NA:
      break;
  }
}

static void CStringReqMemcmp_Prepare( CStringReqMemcmp_Context *ctx )
{
  memset( ctx->buf1, FILL, sizeof( ctx->buf1 ) );
  memset( ctx->buf2, FILL, sizeof( ctx->buf2 ) );
  ctx->s1_misalign = 0;
  ctx->s2_misalign = 0;
  ctx->n = 0;
  ctx->retval = 0x7fffffff;
}

static void CStringReqMemcmp_Action( CStringReqMemcmp_Context *ctx )
{
  memcpy( ctx->before1, ctx->buf1, sizeof( ctx->buf1 ) );
  memcpy( ctx->before2, ctx->buf2, sizeof( ctx->buf2 ) );
  errno = -1;
  ctx->retval = memcmp( ctx->s1, ctx->s2, ctx->n );
  ctx->errno_value = errno;
}

/* clang-format off */

static const CStringReqMemcmp_Entry
CStringReqMemcmp_Entries[] = {
  { 1, 0, 0, 0, 0, 0, 0, CStringReqMemcmp_Post_Result_NA,
    CStringReqMemcmp_Post_S1_NA, CStringReqMemcmp_Post_S2_NA,
    CStringReqMemcmp_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 0, 0, CStringReqMemcmp_Post_Result_NA,
    CStringReqMemcmp_Post_S1_NA, CStringReqMemcmp_Post_S2_NA,
    CStringReqMemcmp_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 0, 0, CStringReqMemcmp_Post_Result_NA,
    CStringReqMemcmp_Post_S1_NA, CStringReqMemcmp_Post_S2_NA,
    CStringReqMemcmp_Post_Errno_NA },
  { 0, 0, 0, 0, 0, 0, 0, CStringReqMemcmp_Post_Result_Negative,
    CStringReqMemcmp_Post_S1_Nop, CStringReqMemcmp_Post_S2_Nop,
    CStringReqMemcmp_Post_Errno_Nop },
  { 1, 0, 0, 0, 0, 0, 0, CStringReqMemcmp_Post_Result_NA,
    CStringReqMemcmp_Post_S1_NA, CStringReqMemcmp_Post_S2_NA,
    CStringReqMemcmp_Post_Errno_NA },
  { 0, 0, 0, 0, 0, 0, 0, CStringReqMemcmp_Post_Result_Positive,
    CStringReqMemcmp_Post_S1_Nop, CStringReqMemcmp_Post_S2_Nop,
    CStringReqMemcmp_Post_Errno_Nop },
  { 0, 0, 0, 0, 0, 0, 0, CStringReqMemcmp_Post_Result_Zero,
    CStringReqMemcmp_Post_S1_Nop, CStringReqMemcmp_Post_S2_Nop,
    CStringReqMemcmp_Post_Errno_Nop }
};

static const uint8_t
CStringReqMemcmp_Map[] = {
  0, 0, 1, 1, 1, 1, 6, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 6, 0, 1, 1, 1, 1, 0, 0,
  1, 1, 1, 1, 6, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 6, 0, 1, 1, 1, 1, 0, 0, 2, 2,
  2, 2, 6, 0, 3, 4, 5, 4, 0, 0, 2, 2, 2, 2, 6, 0, 3, 4, 5, 4, 0, 0, 2, 2, 2, 2,
  6, 0, 3, 4, 5, 4, 0, 0, 2, 2, 2, 2, 6, 0, 3, 4, 5, 4, 0, 0, 3, 3, 5, 5, 6, 0,
  3, 4, 5, 4, 0, 0, 3, 3, 5, 5, 6, 0, 3, 4, 5, 4, 0, 0, 3, 3, 5, 5, 6, 0, 3, 4,
  5, 4, 0, 0, 3, 3, 5, 5, 6, 0, 3, 4, 5, 4
};

/* clang-format on */

static size_t CStringReqMemcmp_Scope( void *arg, char *buf, size_t n )
{
  CStringReqMemcmp_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    size_t pcd[ 6 ];
    size_t i;

    for ( i = 0; i < 6; ++i ) {
      pcd[ i ] = CStringReqMemcmp_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
                 CStringReqMemcmp_GetRepresentative( ctx, i );
    }

    return T_get_scope( CStringReqMemcmp_PreDesc, buf, n, pcd );
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
  return CStringReqMemcmp_Entries[ CStringReqMemcmp_Map[ index ] ];
}

static void CStringReqMemcmp_TestVariant( CStringReqMemcmp_Context *ctx )
{
  CStringReqMemcmp_Pre_N_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringReqMemcmp_Pre_S1Alignment_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CStringReqMemcmp_Pre_S2Alignment_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CStringReqMemcmp_Pre_Position_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CStringReqMemcmp_Pre_Diff_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CStringReqMemcmp_Pre_Rest_Prepare( ctx, ctx->Map.pcs[ 5 ] );

  if ( ctx->Map.skip_rep ) {
    return;
  }

  CStringReqMemcmp_Action( ctx );
  CStringReqMemcmp_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
  CStringReqMemcmp_Post_S1_Check( ctx, ctx->Map.entry.Post_S1 );
  CStringReqMemcmp_Post_S2_Check( ctx, ctx->Map.entry.Post_S2 );
  CStringReqMemcmp_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
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
  CStringReqMemcmp_ResetRepresentatives( ctx );

  for (
    ctx->Map.pcs[ 0 ] = CStringReqMemcmp_Pre_N_Zero;
    ctx->Map.pcs[ 0 ] < CStringReqMemcmp_Pre_N_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CStringReqMemcmp_Pre_S1Alignment_Aligned;
      ctx->Map.pcs[ 1 ] < CStringReqMemcmp_Pre_S1Alignment_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = CStringReqMemcmp_Pre_S2Alignment_Aligned;
        ctx->Map.pcs[ 2 ] < CStringReqMemcmp_Pre_S2Alignment_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = CStringReqMemcmp_Pre_Position_Before;
          ctx->Map.pcs[ 3 ] < CStringReqMemcmp_Pre_Position_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] = CStringReqMemcmp_Pre_Diff_Equal;
            ctx->Map.pcs[ 4 ] < CStringReqMemcmp_Pre_Diff_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            for (
              ctx->Map.pcs[ 5 ] = CStringReqMemcmp_Pre_Rest_Equal;
              ctx->Map.pcs[ 5 ] < CStringReqMemcmp_Pre_Rest_NA;
              ++ctx->Map.pcs[ 5 ]
            ) {
              ctx->Map.entry = CStringReqMemcmp_PopEntry( ctx );

              if ( ctx->Map.entry.Skip ) {
                continue;
              }

              for (
                ctx->Map.rep = 0;
                ctx->Map.rep < CStringReqMemcmp_GetRepresentativeCount( ctx );
                ++ctx->Map.rep
              ) {
                ctx->Map.skip_rep = false;
                CStringReqMemcmp_Prepare( ctx );
                CStringReqMemcmp_TestVariant( ctx );

                CStringReqMemcmp_MarkRepresentatives( ctx );
              }
            }
          }
        }
      }
    }
  }

  CStringReqMemcmp_CheckRepresentatives( ctx );
}

/** @} */
