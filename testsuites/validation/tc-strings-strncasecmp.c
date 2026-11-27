/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringsReqStrncasecmp
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>

#include <rtems/test.h>

/**
 * @defgroup CStringsReqStrncasecmp spec:/c/strings/req/strncasecmp
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CStringsReqStrncasecmp_Pre_N_Zero,
  CStringsReqStrncasecmp_Pre_N_One,
  CStringsReqStrncasecmp_Pre_N_Between,
  CStringsReqStrncasecmp_Pre_N_Max,
  CStringsReqStrncasecmp_Pre_N_NA
} CStringsReqStrncasecmp_Pre_N;

typedef enum {
  CStringsReqStrncasecmp_Pre_Index_Before,
  CStringsReqStrncasecmp_Pre_Index_Last,
  CStringsReqStrncasecmp_Pre_Index_At,
  CStringsReqStrncasecmp_Pre_Index_Beyond,
  CStringsReqStrncasecmp_Pre_Index_NA
} CStringsReqStrncasecmp_Pre_Index;

typedef enum {
  CStringsReqStrncasecmp_Pre_Stop_BothEnd,
  CStringsReqStrncasecmp_Pre_Stop_S1Ends,
  CStringsReqStrncasecmp_Pre_Stop_S2Ends,
  CStringsReqStrncasecmp_Pre_Stop_Less,
  CStringsReqStrncasecmp_Pre_Stop_Greater,
  CStringsReqStrncasecmp_Pre_Stop_NA
} CStringsReqStrncasecmp_Pre_Stop;

typedef enum {
  CStringsReqStrncasecmp_Pre_Letters_None,
  CStringsReqStrncasecmp_Pre_Letters_One,
  CStringsReqStrncasecmp_Pre_Letters_Both,
  CStringsReqStrncasecmp_Pre_Letters_NA
} CStringsReqStrncasecmp_Pre_Letters;

typedef enum {
  CStringsReqStrncasecmp_Pre_CaseDiffers_Yes,
  CStringsReqStrncasecmp_Pre_CaseDiffers_No,
  CStringsReqStrncasecmp_Pre_CaseDiffers_NA
} CStringsReqStrncasecmp_Pre_CaseDiffers;

typedef enum {
  CStringsReqStrncasecmp_Post_Result_Less,
  CStringsReqStrncasecmp_Post_Result_Equal,
  CStringsReqStrncasecmp_Post_Result_Greater,
  CStringsReqStrncasecmp_Post_Result_NA
} CStringsReqStrncasecmp_Post_Result;

typedef enum {
  CStringsReqStrncasecmp_Post_S1_Nop,
  CStringsReqStrncasecmp_Post_S1_NA
} CStringsReqStrncasecmp_Post_S1;

typedef enum {
  CStringsReqStrncasecmp_Post_S2_Nop,
  CStringsReqStrncasecmp_Post_S2_NA
} CStringsReqStrncasecmp_Post_S2;

typedef enum {
  CStringsReqStrncasecmp_Post_Errno_Nop,
  CStringsReqStrncasecmp_Post_Errno_NA
} CStringsReqStrncasecmp_Post_Errno;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_N_NA : 1;
  uint16_t Pre_Index_NA : 1;
  uint16_t Pre_Stop_NA : 1;
  uint16_t Pre_Letters_NA : 1;
  uint16_t Pre_CaseDiffers_NA : 1;
  uint16_t Post_Result : 2;
  uint16_t Post_S1 : 1;
  uint16_t Post_S2 : 1;
  uint16_t Post_Errno : 1;
} CStringsReqStrncasecmp_Entry;

/**
 * @brief Test context for spec:/c/strings/req/strncasecmp test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `s1` parameter value.
   */
  const char *s1;

  /**
   * @brief This member specifies the `s2` parameter value.
   */
  const char *s2;

  /**
   * @brief This member specifies the `n` parameter value.
   */
  size_t n;

  /**
   * @brief This member contains the return value of the strncasecmp() call.
   */
  int retval;

  /**
   * @brief This member contains the value of errno after the strncasecmp()
   *   call.
   */
  int errno_value;

  /**
   * @brief This member specifies the stop index of the arrays.
   */
  size_t stop;

  /**
   * @brief This member references the pairs of bytes at the stop index, three
   *   pairs for each number of letters, one pair for each representative.
   */
  const unsigned char ( *pairs )[ 3 ][ 2 ];

  /**
   * @brief This member provides the array referenced by the `s1` parameter.
   */
  char s1_buf[ 32 ];

  /**
   * @brief This member provides the array referenced by the `s2` parameter.
   */
  char s2_buf[ 32 ];

  /**
   * @brief This member contains the content of the array referenced by the
   *   `s1` parameter before the strncasecmp() call.
   */
  char s1_before[ 32 ];

  /**
   * @brief This member contains the content of the array referenced by the
   *   `s2` parameter before the strncasecmp() call.
   */
  char s2_before[ 32 ];

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 5 ];

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
    bool rep_ran[ 31 ];

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
    CStringsReqStrncasecmp_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringsReqStrncasecmp_Context;

static CStringsReqStrncasecmp_Context CStringsReqStrncasecmp_Instance;

static const char *const CStringsReqStrncasecmp_PreDesc_N[] = {
  "Zero",
  "One",
  "BetweenThree",
  "BetweenFour",
  "BetweenLarge",
  "Max",
  "NA"
};

static const char *const CStringsReqStrncasecmp_PreDesc_Index[] =
  { "BeforeOne", "BeforeLate", "Last", "At", "BeyondNext", "BeyondFar", "NA" };

static const char *const CStringsReqStrncasecmp_PreDesc_Stop[] =
  { "BothEnd", "S1Ends", "S2Ends", "Less", "Greater", "NA" };

static const char *const CStringsReqStrncasecmp_PreDesc_Letters[] = {
  "NoneWide",
  "NoneBelow",
  "NoneAbove",
  "OneLow",
  "OneHigh",
  "BothLow",
  "BothHigh",
  "NA"
};

static const char *const CStringsReqStrncasecmp_PreDesc_CaseDiffers[] =
  { "Yes", "No", "NA" };

static const char *const *const CStringsReqStrncasecmp_PreDesc[] = {
  CStringsReqStrncasecmp_PreDesc_N,
  CStringsReqStrncasecmp_PreDesc_Index,
  CStringsReqStrncasecmp_PreDesc_Stop,
  CStringsReqStrncasecmp_PreDesc_Letters,
  CStringsReqStrncasecmp_PreDesc_CaseDiffers,
  NULL
};

typedef enum {
  CStringsReqStrncasecmp_Pre_N_Between_Three,
  CStringsReqStrncasecmp_Pre_N_Between_Four,
  CStringsReqStrncasecmp_Pre_N_Between_Large
} CStringsReqStrncasecmp_Pre_N_Between_Rep;

typedef enum {
  CStringsReqStrncasecmp_Pre_Index_Before_One,
  CStringsReqStrncasecmp_Pre_Index_Before_Late
} CStringsReqStrncasecmp_Pre_Index_Before_Rep;

typedef enum {
  CStringsReqStrncasecmp_Pre_Index_Beyond_Next,
  CStringsReqStrncasecmp_Pre_Index_Beyond_Far
} CStringsReqStrncasecmp_Pre_Index_Beyond_Rep;

typedef enum {
  CStringsReqStrncasecmp_Pre_Letters_None_Wide,
  CStringsReqStrncasecmp_Pre_Letters_None_Below,
  CStringsReqStrncasecmp_Pre_Letters_None_Above
} CStringsReqStrncasecmp_Pre_Letters_None_Rep;

typedef enum {
  CStringsReqStrncasecmp_Pre_Letters_One_Low,
  CStringsReqStrncasecmp_Pre_Letters_One_High
} CStringsReqStrncasecmp_Pre_Letters_One_Rep;

typedef enum {
  CStringsReqStrncasecmp_Pre_Letters_Both_Low,
  CStringsReqStrncasecmp_Pre_Letters_Both_High
} CStringsReqStrncasecmp_Pre_Letters_Both_Rep;

static const uint8_t CStringsReqStrncasecmp_PreRepOffset_N[] =
  { 0, 1, 2, 5, 6, 7 };

static const uint8_t CStringsReqStrncasecmp_PreRepOffset_Index[] =
  { 0, 2, 3, 4, 6, 7 };

static const uint8_t CStringsReqStrncasecmp_PreRepOffset_Stop[] =
  { 0, 1, 2, 3, 4, 5, 6 };

static const uint8_t CStringsReqStrncasecmp_PreRepOffset_Letters[] =
  { 0, 3, 5, 7, 8 };

static const uint8_t CStringsReqStrncasecmp_PreRepOffset_CaseDiffers[] =
  { 0, 1, 2, 3 };

static const uint8_t *const CStringsReqStrncasecmp_PreRepOffset[] = {
  CStringsReqStrncasecmp_PreRepOffset_N,
  CStringsReqStrncasecmp_PreRepOffset_Index,
  CStringsReqStrncasecmp_PreRepOffset_Stop,
  CStringsReqStrncasecmp_PreRepOffset_Letters,
  CStringsReqStrncasecmp_PreRepOffset_CaseDiffers
};

static size_t CStringsReqStrncasecmp_GetRepresentativeCountOf(
  const CStringsReqStrncasecmp_Context *ctx,
  size_t                                index
)
{
  const uint8_t *offsets;
  size_t         state;

  offsets = CStringsReqStrncasecmp_PreRepOffset[ index ];
  state = ctx->Map.pcs[ index ];

  return offsets[ state + 1 ] - offsets[ state ];
}

static size_t CStringsReqStrncasecmp_GetRepresentative(
  const CStringsReqStrncasecmp_Context *ctx,
  size_t                                index
)
{
  size_t count;

  count = CStringsReqStrncasecmp_GetRepresentativeCountOf( ctx, index );

  if ( ctx->Map.rep < count ) {
    return ctx->Map.rep;
  }

  return count - 1;
}

static size_t CStringsReqStrncasecmp_GetRepresentativeCount(
  const CStringsReqStrncasecmp_Context *ctx
)
{
  size_t count;
  size_t i;

  count = 1;

  for ( i = 0; i < 5; ++i ) {
    size_t count_of_condition;

    count_of_condition = CStringsReqStrncasecmp_GetRepresentativeCountOf(
      ctx,
      i
    );

    if ( count_of_condition > count ) {
      count = count_of_condition;
    }
  }

  return count;
}

static CStringsReqStrncasecmp_Pre_N_Between_Rep
CStringsReqStrncasecmp_Pre_N_Between_GetRepresentative(
  const CStringsReqStrncasecmp_Context *ctx
)
{
  return (
    CStringsReqStrncasecmp_Pre_N_Between_Rep
  ) CStringsReqStrncasecmp_GetRepresentative( ctx, 0 );
}

static CStringsReqStrncasecmp_Pre_Index_Before_Rep
CStringsReqStrncasecmp_Pre_Index_Before_GetRepresentative(
  const CStringsReqStrncasecmp_Context *ctx
)
{
  return (
    CStringsReqStrncasecmp_Pre_Index_Before_Rep
  ) CStringsReqStrncasecmp_GetRepresentative( ctx, 1 );
}

static CStringsReqStrncasecmp_Pre_Index_Beyond_Rep
CStringsReqStrncasecmp_Pre_Index_Beyond_GetRepresentative(
  const CStringsReqStrncasecmp_Context *ctx
)
{
  return (
    CStringsReqStrncasecmp_Pre_Index_Beyond_Rep
  ) CStringsReqStrncasecmp_GetRepresentative( ctx, 1 );
}

static CStringsReqStrncasecmp_Pre_Letters_None_Rep
CStringsReqStrncasecmp_Pre_Letters_None_GetRepresentative(
  const CStringsReqStrncasecmp_Context *ctx
)
{
  return (
    CStringsReqStrncasecmp_Pre_Letters_None_Rep
  ) CStringsReqStrncasecmp_GetRepresentative( ctx, 3 );
}

static CStringsReqStrncasecmp_Pre_Letters_One_Rep
CStringsReqStrncasecmp_Pre_Letters_One_GetRepresentative(
  const CStringsReqStrncasecmp_Context *ctx
)
{
  return (
    CStringsReqStrncasecmp_Pre_Letters_One_Rep
  ) CStringsReqStrncasecmp_GetRepresentative( ctx, 3 );
}

static CStringsReqStrncasecmp_Pre_Letters_Both_Rep
CStringsReqStrncasecmp_Pre_Letters_Both_GetRepresentative(
  const CStringsReqStrncasecmp_Context *ctx
)
{
  return (
    CStringsReqStrncasecmp_Pre_Letters_Both_Rep
  ) CStringsReqStrncasecmp_GetRepresentative( ctx, 3 );
}

static const uint8_t CStringsReqStrncasecmp_PreRepBase[] =
  { 0, 7, 14, 20, 28 };

static const uint8_t CStringsReqStrncasecmp_PreRepStates[] = { 4, 4, 5, 3, 2 };

static void CStringsReqStrncasecmp_ResetRepresentatives(
  CStringsReqStrncasecmp_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 31; ++i ) {
    ctx->Map.rep_ran[ i ] = false;
  }
}

static void CStringsReqStrncasecmp_MarkRepresentatives(
  CStringsReqStrncasecmp_Context *ctx
)
{
  size_t i;

  if ( ctx->Map.skip_rep ) {
    return;
  }

  for ( i = 0; i < 5; ++i ) {
    size_t rep;

    rep = CStringsReqStrncasecmp_PreRepBase[ i ] +
          CStringsReqStrncasecmp_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
          CStringsReqStrncasecmp_GetRepresentative( ctx, i );
    ctx->Map.rep_ran[ rep ] = true;
  }
}

static void CStringsReqStrncasecmp_CheckRepresentatives(
  const CStringsReqStrncasecmp_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 5; ++i ) {
    const uint8_t *offsets;
    const bool    *ran;
    size_t         state;

    offsets = CStringsReqStrncasecmp_PreRepOffset[ i ];
    ran = &ctx->Map.rep_ran[ CStringsReqStrncasecmp_PreRepBase[ i ] ];

    for (
      state = 0; state < CStringsReqStrncasecmp_PreRepStates[ i ]; ++state
    ) {
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
            CStringsReqStrncasecmp_PreDesc[ i ][ k ]
          );
        }
      }
    }
  }
}

typedef CStringsReqStrncasecmp_Context Context;

static const unsigned char both_end_pairs[ 3 ][ 3 ][ 2 ] = {
  { { '\0', '\0' }, { '\0', '\0' }, { '\0', '\0' } },
  { { '\0', '\0' }, { '\0', '\0' }, { '\0', '\0' } },
  { { '\0', '\0' }, { '\0', '\0' }, { '\0', '\0' } }
};

static const unsigned char s1_ends_pairs[ 3 ][ 3 ][ 2 ] = {
  { { '\0', 0xff }, { '\0', '@' }, { '\0', '[' } },
  { { '\0', 'a' }, { '\0', 'Z' }, { '\0', 'Z' } },
  { { '\0', '\0' }, { '\0', '\0' }, { '\0', '\0' } }
};

static const unsigned char s2_ends_pairs[ 3 ][ 3 ][ 2 ] = {
  { { 0xff, '\0' }, { '@', '\0' }, { '[', '\0' } },
  { { 'a', '\0' }, { 'Z', '\0' }, { 'Z', '\0' } },
  { { '\0', '\0' }, { '\0', '\0' }, { '\0', '\0' } }
};

static const unsigned char less_pairs[ 3 ][ 3 ][ 2 ] = {
  { { '!', 0x80 }, { '@', '[' }, { '[', '`' } },
  { { '_', 'A' }, { '_', 'Z' }, { '_', 'Z' } },
  { { 'a', 'B' }, { 'y', 'Z' }, { 'y', 'Z' } }
};

static const unsigned char greater_pairs[ 3 ][ 3 ][ 2 ] = {
  { { 0x80, '!' }, { '[', '@' }, { '`', '[' } },
  { { 'A', '_' }, { 'Z', '_' }, { 'Z', '_' } },
  { { 'B', 'a' }, { 'Z', 'y' }, { 'Z', 'y' } }
};

static void SetPair( Context *ctx, const unsigned char *pair )
{
  memcpy( &ctx->s1_buf[ ctx->stop ], &pair[ 0 ], 1 );
  memcpy( &ctx->s2_buf[ ctx->stop ], &pair[ 1 ], 1 );
  ctx->s1_buf[ ctx->stop + 1 ] = '\0';
  ctx->s2_buf[ ctx->stop + 1 ] = '\0';
}

static void CStringsReqStrncasecmp_Pre_N_Prepare(
  CStringsReqStrncasecmp_Context *ctx,
  CStringsReqStrncasecmp_Pre_N    state
)
{
  switch ( state ) {
    case CStringsReqStrncasecmp_Pre_N_Zero: {
      /*
       * While the `n` parameter is equal to zero.
       */
      ctx->n = 0;
      break;
    }

    case CStringsReqStrncasecmp_Pre_N_One: {
      /*
       * While the `n` parameter is equal to one.
       */
      ctx->n = 1;
      break;
    }

    case CStringsReqStrncasecmp_Pre_N_Between: {
      /*
       * While the `n` parameter is greater than one, while the `n` parameter
       * is less than SIZE_MAX.
       */
      switch (
        CStringsReqStrncasecmp_Pre_N_Between_GetRepresentative( ctx )
      ) {
        case CStringsReqStrncasecmp_Pre_N_Between_Three: {
          ctx->n = 3;
          break;
        }

        case CStringsReqStrncasecmp_Pre_N_Between_Four: {
          ctx->n = 4;
          break;
        }

        case CStringsReqStrncasecmp_Pre_N_Between_Large: {
          ctx->n = 24;
          break;
        }
      }
      break;
    }

    case CStringsReqStrncasecmp_Pre_N_Max: {
      /*
       * While the `n` parameter is equal to SIZE_MAX.
       */
      ctx->n = SIZE_MAX;
      break;
    }

    case CStringsReqStrncasecmp_Pre_N_NA:
      break;
  }
}

static void CStringsReqStrncasecmp_Pre_Index_Prepare(
  CStringsReqStrncasecmp_Context  *ctx,
  CStringsReqStrncasecmp_Pre_Index state
)
{
  switch ( state ) {
    case CStringsReqStrncasecmp_Pre_Index_Before: {
      /*
       * While the stop index of the arrays referenced by the `s1` parameter
       * and the `s2` parameter is less than the value of the `n` parameter
       * minus one.
       */
      switch (
        CStringsReqStrncasecmp_Pre_Index_Before_GetRepresentative( ctx )
      ) {
        case CStringsReqStrncasecmp_Pre_Index_Before_One: {
          ctx->stop = 1;
          break;
        }

        case CStringsReqStrncasecmp_Pre_Index_Before_Late: {
          if ( ctx->n == SIZE_MAX ) {
            ctx->stop = 3;
          } else {
            ctx->stop = ctx->n - 2;
          }
          break;
        }
      }
      break;
    }

    case CStringsReqStrncasecmp_Pre_Index_Last: {
      /*
       * While the stop index of the arrays referenced by the `s1` parameter
       * and the `s2` parameter is equal to the value of the `n` parameter
       * minus one.
       */
      ctx->stop = ctx->n - 1;
      break;
    }

    case CStringsReqStrncasecmp_Pre_Index_At: {
      /*
       * While the stop index of the arrays referenced by the `s1` parameter
       * and the `s2` parameter is equal to the value of the `n` parameter.
       */
      ctx->stop = ctx->n;
      break;
    }

    case CStringsReqStrncasecmp_Pre_Index_Beyond: {
      /*
       * While the stop index of the arrays referenced by the `s1` parameter
       * and the `s2` parameter is greater than the value of the `n` parameter.
       */
      switch (
        CStringsReqStrncasecmp_Pre_Index_Beyond_GetRepresentative( ctx )
      ) {
        case CStringsReqStrncasecmp_Pre_Index_Beyond_Next: {
          ctx->stop = ctx->n + 1;
          break;
        }

        case CStringsReqStrncasecmp_Pre_Index_Beyond_Far: {
          ctx->stop = ctx->n + 2;
          break;
        }
      }
      break;
    }

    case CStringsReqStrncasecmp_Pre_Index_NA:
      break;
  }
}

static void CStringsReqStrncasecmp_Pre_Stop_Prepare(
  CStringsReqStrncasecmp_Context *ctx,
  CStringsReqStrncasecmp_Pre_Stop state
)
{
  switch ( state ) {
    case CStringsReqStrncasecmp_Pre_Stop_BothEnd: {
      /*
       * While both arrays hold a null byte at the stop index.
       */
      ctx->pairs = both_end_pairs;
      break;
    }

    case CStringsReqStrncasecmp_Pre_Stop_S1Ends: {
      /*
       * While the array referenced by the `s1` parameter holds a null byte at
       * the stop index, while the array referenced by the `s2` parameter holds
       * no null byte at the stop index.
       */
      ctx->pairs = s1_ends_pairs;
      break;
    }

    case CStringsReqStrncasecmp_Pre_Stop_S2Ends: {
      /*
       * While the array referenced by the `s1` parameter holds no null byte at
       * the stop index, while the array referenced by the `s2` parameter holds
       * a null byte at the stop index.
       */
      ctx->pairs = s2_ends_pairs;
      break;
    }

    case CStringsReqStrncasecmp_Pre_Stop_Less: {
      /*
       * While no array holds a null byte at the stop index, while the
       * lowercase value of the byte of the array referenced by the `s1`
       * parameter at the stop index is less than the lowercase value of the
       * byte of the array referenced by the `s2` parameter at the stop index.
       */
      ctx->pairs = less_pairs;
      break;
    }

    case CStringsReqStrncasecmp_Pre_Stop_Greater: {
      /*
       * While no array holds a null byte at the stop index, while the
       * lowercase value of the byte of the array referenced by the `s1`
       * parameter at the stop index is greater than the lowercase value of the
       * byte of the array referenced by the `s2` parameter at the stop index.
       */
      ctx->pairs = greater_pairs;
      break;
    }

    case CStringsReqStrncasecmp_Pre_Stop_NA:
      break;
  }
}

static void CStringsReqStrncasecmp_Pre_Letters_Prepare(
  CStringsReqStrncasecmp_Context    *ctx,
  CStringsReqStrncasecmp_Pre_Letters state
)
{
  switch ( state ) {
    case CStringsReqStrncasecmp_Pre_Letters_None: {
      /*
       * While no byte of the arrays at the stop index is an uppercase letter
       * or a lowercase letter.
       */
      switch (
        CStringsReqStrncasecmp_Pre_Letters_None_GetRepresentative( ctx )
      ) {
        case CStringsReqStrncasecmp_Pre_Letters_None_Wide: {
          SetPair( ctx, ctx->pairs[ 0 ][ 0 ] );
          break;
        }

        case CStringsReqStrncasecmp_Pre_Letters_None_Below: {
          SetPair( ctx, ctx->pairs[ 0 ][ 1 ] );
          break;
        }

        case CStringsReqStrncasecmp_Pre_Letters_None_Above: {
          SetPair( ctx, ctx->pairs[ 0 ][ 2 ] );
          break;
        }
      }
      break;
    }

    case CStringsReqStrncasecmp_Pre_Letters_One: {
      /*
       * While exactly one byte of the arrays at the stop index is an uppercase
       * letter or a lowercase letter.
       */
      switch (
        CStringsReqStrncasecmp_Pre_Letters_One_GetRepresentative( ctx )
      ) {
        case CStringsReqStrncasecmp_Pre_Letters_One_Low: {
          SetPair( ctx, ctx->pairs[ 1 ][ 0 ] );
          break;
        }

        case CStringsReqStrncasecmp_Pre_Letters_One_High: {
          SetPair( ctx, ctx->pairs[ 1 ][ 1 ] );
          break;
        }
      }
      break;
    }

    case CStringsReqStrncasecmp_Pre_Letters_Both: {
      /*
       * While each byte of the arrays at the stop index is an uppercase letter
       * or a lowercase letter.
       */
      switch (
        CStringsReqStrncasecmp_Pre_Letters_Both_GetRepresentative( ctx )
      ) {
        case CStringsReqStrncasecmp_Pre_Letters_Both_Low: {
          SetPair( ctx, ctx->pairs[ 2 ][ 0 ] );
          break;
        }

        case CStringsReqStrncasecmp_Pre_Letters_Both_High: {
          SetPair( ctx, ctx->pairs[ 2 ][ 1 ] );
          break;
        }
      }
      break;
    }

    case CStringsReqStrncasecmp_Pre_Letters_NA:
      break;
  }
}

static void CStringsReqStrncasecmp_Pre_CaseDiffers_Prepare(
  CStringsReqStrncasecmp_Context        *ctx,
  CStringsReqStrncasecmp_Pre_CaseDiffers state
)
{
  switch ( state ) {
    case CStringsReqStrncasecmp_Pre_CaseDiffers_Yes: {
      /*
       * While the arrays hold different bytes at an index less than the stop
       * index.
       */
      ctx->s1_buf[ 0 ] = 'X';

      if ( ctx->stop > 1 ) {
        ctx->s2_buf[ 1 ] = 'X';
      }
      break;
    }

    case CStringsReqStrncasecmp_Pre_CaseDiffers_No: {
      /*
       * While the arrays hold equal bytes at each index less than the stop
       * index.
       */
      /* The prepared arrays hold equal bytes. */
      break;
    }

    case CStringsReqStrncasecmp_Pre_CaseDiffers_NA:
      break;
  }
}

static void CStringsReqStrncasecmp_Post_Result_Check(
  CStringsReqStrncasecmp_Context    *ctx,
  CStringsReqStrncasecmp_Post_Result state
)
{
  switch ( state ) {
    case CStringsReqStrncasecmp_Post_Result_Less: {
      /*
       * The return value of strncasecmp() shall be less than zero.
       */
      T_lt_int( ctx->retval, 0 );
      break;
    }

    case CStringsReqStrncasecmp_Post_Result_Equal: {
      /*
       * The return value of strncasecmp() shall be equal to zero.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CStringsReqStrncasecmp_Post_Result_Greater: {
      /*
       * The return value of strncasecmp() shall be greater than zero.
       */
      T_gt_int( ctx->retval, 0 );
      break;
    }

    case CStringsReqStrncasecmp_Post_Result_NA:
      break;
  }
}

static void CStringsReqStrncasecmp_Post_S1_Check(
  CStringsReqStrncasecmp_Context *ctx,
  CStringsReqStrncasecmp_Post_S1  state
)
{
  switch ( state ) {
    case CStringsReqStrncasecmp_Post_S1_Nop: {
      /*
       * The bytes of the array referenced by the `s1` parameter shall not be
       * modified.
       */
      T_eq_mem( ctx->s1_buf, ctx->s1_before, sizeof( ctx->s1_buf ) );
      break;
    }

    case CStringsReqStrncasecmp_Post_S1_NA:
      break;
  }
}

static void CStringsReqStrncasecmp_Post_S2_Check(
  CStringsReqStrncasecmp_Context *ctx,
  CStringsReqStrncasecmp_Post_S2  state
)
{
  switch ( state ) {
    case CStringsReqStrncasecmp_Post_S2_Nop: {
      /*
       * The bytes of the array referenced by the `s2` parameter shall not be
       * modified.
       */
      T_eq_mem( ctx->s2_buf, ctx->s2_before, sizeof( ctx->s2_buf ) );
      break;
    }

    case CStringsReqStrncasecmp_Post_S2_NA:
      break;
  }
}

static void CStringsReqStrncasecmp_Post_Errno_Check(
  CStringsReqStrncasecmp_Context   *ctx,
  CStringsReqStrncasecmp_Post_Errno state
)
{
  switch ( state ) {
    case CStringsReqStrncasecmp_Post_Errno_Nop: {
      /*
       * The value of errno shall not be modified.
       */
      T_eq_int( ctx->errno_value, -1 );
      break;
    }

    case CStringsReqStrncasecmp_Post_Errno_NA:
      break;
  }
}

static void CStringsReqStrncasecmp_Prepare(
  CStringsReqStrncasecmp_Context *ctx
)
{
  memset( ctx->s1_buf, 'x', sizeof( ctx->s1_buf ) );
  memset( ctx->s2_buf, 'x', sizeof( ctx->s2_buf ) );
  ctx->s1 = ctx->s1_buf;
  ctx->s2 = ctx->s2_buf;
}

static void CStringsReqStrncasecmp_Action(
  CStringsReqStrncasecmp_Context *ctx
)
{
  memcpy( ctx->s1_before, ctx->s1_buf, sizeof( ctx->s1_buf ) );
  memcpy( ctx->s2_before, ctx->s2_buf, sizeof( ctx->s2_buf ) );
  errno = -1;
  ctx->retval = strncasecmp( ctx->s1, ctx->s2, ctx->n );
  ctx->errno_value = errno;
}

/* clang-format off */

static const CStringsReqStrncasecmp_Entry
CStringsReqStrncasecmp_Entries[] = {
  { 1, 0, 0, 0, 0, 0, CStringsReqStrncasecmp_Post_Result_NA,
    CStringsReqStrncasecmp_Post_S1_NA, CStringsReqStrncasecmp_Post_S2_NA,
    CStringsReqStrncasecmp_Post_Errno_NA },
  { 0, 0, 0, 0, 0, 0, CStringsReqStrncasecmp_Post_Result_Equal,
    CStringsReqStrncasecmp_Post_S1_Nop, CStringsReqStrncasecmp_Post_S2_Nop,
    CStringsReqStrncasecmp_Post_Errno_Nop },
  { 1, 0, 0, 0, 0, 0, CStringsReqStrncasecmp_Post_Result_NA,
    CStringsReqStrncasecmp_Post_S1_NA, CStringsReqStrncasecmp_Post_S2_NA,
    CStringsReqStrncasecmp_Post_Errno_NA },
  { 1, 0, 0, 0, 0, 0, CStringsReqStrncasecmp_Post_Result_NA,
    CStringsReqStrncasecmp_Post_S1_NA, CStringsReqStrncasecmp_Post_S2_NA,
    CStringsReqStrncasecmp_Post_Errno_NA },
  { 0, 0, 0, 0, 0, 0, CStringsReqStrncasecmp_Post_Result_Less,
    CStringsReqStrncasecmp_Post_S1_Nop, CStringsReqStrncasecmp_Post_S2_Nop,
    CStringsReqStrncasecmp_Post_Errno_Nop },
  { 0, 0, 0, 0, 0, 0, CStringsReqStrncasecmp_Post_Result_Greater,
    CStringsReqStrncasecmp_Post_S1_Nop, CStringsReqStrncasecmp_Post_S2_Nop,
    CStringsReqStrncasecmp_Post_Errno_Nop },
  { 1, 0, 0, 0, 0, 0, CStringsReqStrncasecmp_Post_Result_NA,
    CStringsReqStrncasecmp_Post_S1_NA, CStringsReqStrncasecmp_Post_S2_NA,
    CStringsReqStrncasecmp_Post_Errno_NA }
};

static const uint8_t
CStringsReqStrncasecmp_Map[] = {
  2, 2, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 6, 1, 0, 0, 0, 0, 6, 1, 6, 1, 0, 0, 6, 1, 6, 1, 0, 0,
  6, 1, 6, 1, 6, 1, 6, 1, 6, 1, 6, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1,
  1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 0, 0, 0, 0, 2, 2, 2, 2,
  0, 0, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 1, 0, 0, 0, 0,
  6, 4, 6, 4, 0, 0, 6, 5, 6, 5, 0, 0, 6, 4, 6, 4, 6, 4, 6, 5, 6, 5, 6, 5, 1, 1,
  0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 5, 5, 5, 5, 0, 0, 4, 4,
  4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 1, 1, 0, 0, 0, 0, 4, 4, 4, 4, 0, 0, 5, 5, 5, 5,
  0, 0, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0,
  1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1,
  1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
  0, 0, 4, 4, 4, 4, 0, 0, 5, 5, 5, 5, 0, 0, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5,
  3, 3, 0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 0, 0,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3
};

/* clang-format on */

static size_t CStringsReqStrncasecmp_Scope( void *arg, char *buf, size_t n )
{
  CStringsReqStrncasecmp_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    size_t pcd[ 5 ];
    size_t i;

    for ( i = 0; i < 5; ++i ) {
      pcd[ i ] = CStringsReqStrncasecmp_PreRepOffset[ i ]
                                                    [ ctx->Map.pcs[ i ] ] +
                 CStringsReqStrncasecmp_GetRepresentative( ctx, i );
    }

    return T_get_scope( CStringsReqStrncasecmp_PreDesc, buf, n, pcd );
  }

  return 0;
}

static T_fixture CStringsReqStrncasecmp_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringsReqStrncasecmp_Scope,
  .initial_context = &CStringsReqStrncasecmp_Instance
};

static inline CStringsReqStrncasecmp_Entry CStringsReqStrncasecmp_PopEntry(
  CStringsReqStrncasecmp_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringsReqStrncasecmp_Entries[ CStringsReqStrncasecmp_Map[ index ] ];
}

static void CStringsReqStrncasecmp_TestVariant(
  CStringsReqStrncasecmp_Context *ctx
)
{
  CStringsReqStrncasecmp_Pre_N_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStringsReqStrncasecmp_Pre_Index_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CStringsReqStrncasecmp_Pre_Stop_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CStringsReqStrncasecmp_Pre_Letters_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CStringsReqStrncasecmp_Pre_CaseDiffers_Prepare( ctx, ctx->Map.pcs[ 4 ] );

  if ( ctx->Map.skip_rep ) {
    return;
  }

  CStringsReqStrncasecmp_Action( ctx );
  CStringsReqStrncasecmp_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
  CStringsReqStrncasecmp_Post_S1_Check( ctx, ctx->Map.entry.Post_S1 );
  CStringsReqStrncasecmp_Post_S2_Check( ctx, ctx->Map.entry.Post_S2 );
  CStringsReqStrncasecmp_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
}

/**
 * @fn void T_case_body_CStringsReqStrncasecmp( void )
 */
T_TEST_CASE_FIXTURE( CStringsReqStrncasecmp, &CStringsReqStrncasecmp_Fixture )
{
  CStringsReqStrncasecmp_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  CStringsReqStrncasecmp_ResetRepresentatives( ctx );

  for (
    ctx->Map.pcs[ 0 ] = CStringsReqStrncasecmp_Pre_N_Zero;
    ctx->Map.pcs[ 0 ] < CStringsReqStrncasecmp_Pre_N_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CStringsReqStrncasecmp_Pre_Index_Before;
      ctx->Map.pcs[ 1 ] < CStringsReqStrncasecmp_Pre_Index_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = CStringsReqStrncasecmp_Pre_Stop_BothEnd;
        ctx->Map.pcs[ 2 ] < CStringsReqStrncasecmp_Pre_Stop_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = CStringsReqStrncasecmp_Pre_Letters_None;
          ctx->Map.pcs[ 3 ] < CStringsReqStrncasecmp_Pre_Letters_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] = CStringsReqStrncasecmp_Pre_CaseDiffers_Yes;
            ctx->Map.pcs[ 4 ] < CStringsReqStrncasecmp_Pre_CaseDiffers_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            ctx->Map.entry = CStringsReqStrncasecmp_PopEntry( ctx );

            if ( ctx->Map.entry.Skip ) {
              continue;
            }

            for (
              ctx->Map.rep = 0;
              ctx->Map.rep <
              CStringsReqStrncasecmp_GetRepresentativeCount( ctx );
              ++ctx->Map.rep
            ) {
              ctx->Map.skip_rep = false;
              CStringsReqStrncasecmp_Prepare( ctx );
              CStringsReqStrncasecmp_TestVariant( ctx );

              CStringsReqStrncasecmp_MarkRepresentatives( ctx );
            }
          }
        }
      }
    }
  }

  CStringsReqStrncasecmp_CheckRepresentatives( ctx );
}

/** @} */
