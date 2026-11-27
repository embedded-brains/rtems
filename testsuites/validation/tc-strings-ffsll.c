/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringsReqFfsll
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

#include <limits.h>
#include <strings.h>

#include <rtems/test.h>

/**
 * @defgroup CStringsReqFfsll spec:/c/strings/req/ffsll
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CStringsReqFfsll_Pre_Value_MinusOne,
  CStringsReqFfsll_Pre_Value_Zero,
  CStringsReqFfsll_Pre_Value_One,
  CStringsReqFfsll_Pre_Value_Least,
  CStringsReqFfsll_Pre_Value_Middle,
  CStringsReqFfsll_Pre_Value_Most,
  CStringsReqFfsll_Pre_Value_NA
} CStringsReqFfsll_Pre_Value;

typedef enum {
  CStringsReqFfsll_Post_Result_Zero,
  CStringsReqFfsll_Post_Result_Index,
  CStringsReqFfsll_Post_Result_NA
} CStringsReqFfsll_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Value_NA : 1;
  uint8_t Post_Result : 2;
} CStringsReqFfsll_Entry;

/**
 * @brief Test context for spec:/c/strings/req/ffsll test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `i` parameter value.
   */
  long long i;

  /**
   * @brief This member contains the return value of the ffsll() call.
   */
  int result;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 1 ];

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
    bool rep_ran[ 13 ];

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
    CStringsReqFfsll_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringsReqFfsll_Context;

static CStringsReqFfsll_Context CStringsReqFfsll_Instance;

static const char *const CStringsReqFfsll_PreDesc_Value[] = {
  "MinusOne",
  "Zero",
  "One",
  "LeastLeast",
  "LeastLeastNegative",
  "MiddleTwo",
  "MiddleBelowHalf",
  "MiddleAboveHalf",
  "MiddleInner",
  "MiddleBelowMost",
  "MiddleNegative",
  "Most",
  "NA"
};

static const char *const *const CStringsReqFfsll_PreDesc[] = {
  CStringsReqFfsll_PreDesc_Value,
  NULL
};

typedef enum {
  CStringsReqFfsll_Pre_Value_Least_Least,
  CStringsReqFfsll_Pre_Value_Least_LeastNegative
} CStringsReqFfsll_Pre_Value_Least_Rep;

typedef enum {
  CStringsReqFfsll_Pre_Value_Middle_Two,
  CStringsReqFfsll_Pre_Value_Middle_BelowHalf,
  CStringsReqFfsll_Pre_Value_Middle_AboveHalf,
  CStringsReqFfsll_Pre_Value_Middle_Inner,
  CStringsReqFfsll_Pre_Value_Middle_BelowMost,
  CStringsReqFfsll_Pre_Value_Middle_Negative
} CStringsReqFfsll_Pre_Value_Middle_Rep;

static const uint8_t CStringsReqFfsll_PreRepOffset_Value[] =
  { 0, 1, 2, 3, 5, 11, 12, 13 };

static const uint8_t *const CStringsReqFfsll_PreRepOffset[] = {
  CStringsReqFfsll_PreRepOffset_Value
};

static size_t CStringsReqFfsll_GetRepresentativeCountOf(
  const CStringsReqFfsll_Context *ctx,
  size_t                          index
)
{
  const uint8_t *offsets;
  size_t         state;

  offsets = CStringsReqFfsll_PreRepOffset[ index ];
  state = ctx->Map.pcs[ index ];

  return offsets[ state + 1 ] - offsets[ state ];
}

static size_t CStringsReqFfsll_GetRepresentative(
  const CStringsReqFfsll_Context *ctx,
  size_t                          index
)
{
  size_t count;

  count = CStringsReqFfsll_GetRepresentativeCountOf( ctx, index );

  if ( ctx->Map.rep < count ) {
    return ctx->Map.rep;
  }

  return count - 1;
}

static size_t CStringsReqFfsll_GetRepresentativeCount(
  const CStringsReqFfsll_Context *ctx
)
{
  size_t count;
  size_t i;

  count = 1;

  for ( i = 0; i < 1; ++i ) {
    size_t count_of_condition;

    count_of_condition = CStringsReqFfsll_GetRepresentativeCountOf( ctx, i );

    if ( count_of_condition > count ) {
      count = count_of_condition;
    }
  }

  return count;
}

static CStringsReqFfsll_Pre_Value_Least_Rep
CStringsReqFfsll_Pre_Value_Least_GetRepresentative(
  const CStringsReqFfsll_Context *ctx
)
{
  return (
    CStringsReqFfsll_Pre_Value_Least_Rep
  ) CStringsReqFfsll_GetRepresentative( ctx, 0 );
}

static CStringsReqFfsll_Pre_Value_Middle_Rep
CStringsReqFfsll_Pre_Value_Middle_GetRepresentative(
  const CStringsReqFfsll_Context *ctx
)
{
  return (
    CStringsReqFfsll_Pre_Value_Middle_Rep
  ) CStringsReqFfsll_GetRepresentative( ctx, 0 );
}

static const uint8_t CStringsReqFfsll_PreRepBase[] = { 0 };

static const uint8_t CStringsReqFfsll_PreRepStates[] = { 6 };

static void CStringsReqFfsll_ResetRepresentatives(
  CStringsReqFfsll_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 13; ++i ) {
    ctx->Map.rep_ran[ i ] = false;
  }
}

static void CStringsReqFfsll_MarkRepresentatives(
  CStringsReqFfsll_Context *ctx
)
{
  size_t i;

  if ( ctx->Map.skip_rep ) {
    return;
  }

  for ( i = 0; i < 1; ++i ) {
    size_t rep;

    rep = CStringsReqFfsll_PreRepBase[ i ] +
          CStringsReqFfsll_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
          CStringsReqFfsll_GetRepresentative( ctx, i );
    ctx->Map.rep_ran[ rep ] = true;
  }
}

static void CStringsReqFfsll_CheckRepresentatives(
  const CStringsReqFfsll_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 1; ++i ) {
    const uint8_t *offsets;
    const bool    *ran;
    size_t         state;

    offsets = CStringsReqFfsll_PreRepOffset[ i ];
    ran = &ctx->Map.rep_ran[ CStringsReqFfsll_PreRepBase[ i ] ];

    for ( state = 0; state < CStringsReqFfsll_PreRepStates[ i ]; ++state ) {
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
            CStringsReqFfsll_PreDesc[ i ][ k ]
          );
        }
      }
    }
  }
}

static void CStringsReqFfsll_Pre_Value_Prepare(
  CStringsReqFfsll_Context  *ctx,
  CStringsReqFfsll_Pre_Value state
)
{
  switch ( state ) {
    case CStringsReqFfsll_Pre_Value_MinusOne: {
      /*
       * While the value of the `i` parameter is equal to minus one.
       */
      ctx->i = -1LL;
      break;
    }

    case CStringsReqFfsll_Pre_Value_Zero: {
      /*
       * While the value of the `i` parameter is equal to zero.
       */
      ctx->i = 0;
      break;
    }

    case CStringsReqFfsll_Pre_Value_One: {
      /*
       * While the value of the `i` parameter is equal to one.
       */
      ctx->i = 1LL;
      break;
    }

    case CStringsReqFfsll_Pre_Value_Least: {
      /*
       * While the least significant bit of the value of the `i` parameter is
       * set, while the value of the `i` parameter is neither one nor minus
       * one.
       */
      switch ( CStringsReqFfsll_Pre_Value_Least_GetRepresentative( ctx ) ) {
        case CStringsReqFfsll_Pre_Value_Least_Least: {
          ctx->i = 0x0008010000000001LL;
          break;
        }

        case CStringsReqFfsll_Pre_Value_Least_LeastNegative: {
          ctx->i = LLONG_MIN + 1;
          break;
        }
      }
      break;
    }

    case CStringsReqFfsll_Pre_Value_Middle: {
      /*
       * While the least significant bit set in the value of the `i` parameter
       * is neither the least significant bit nor the most significant bit of
       * the value.
       */
      switch ( CStringsReqFfsll_Pre_Value_Middle_GetRepresentative( ctx ) ) {
        case CStringsReqFfsll_Pre_Value_Middle_Two: {
          ctx->i = 2LL;
          break;
        }

        case CStringsReqFfsll_Pre_Value_Middle_BelowHalf: {
          ctx->i = 0x0000000080000000LL;
          break;
        }

        case CStringsReqFfsll_Pre_Value_Middle_AboveHalf: {
          ctx->i = 0x0000000100000000LL;
          break;
        }

        case CStringsReqFfsll_Pre_Value_Middle_Inner: {
          ctx->i = 0x0008010000000000LL;
          break;
        }

        case CStringsReqFfsll_Pre_Value_Middle_BelowMost: {
          ctx->i = 0x4000000000000000LL;
          break;
        }

        case CStringsReqFfsll_Pre_Value_Middle_Negative: {
          ctx->i = LLONG_MIN | 0x0000000000000100LL;
          break;
        }
      }
      break;
    }

    case CStringsReqFfsll_Pre_Value_Most: {
      /*
       * While the most significant bit is the only bit set in the value of the
       * `i` parameter.
       */
      ctx->i = LLONG_MIN;
      break;
    }

    case CStringsReqFfsll_Pre_Value_NA:
      break;
  }
}

static void CStringsReqFfsll_Post_Result_Check(
  CStringsReqFfsll_Context    *ctx,
  CStringsReqFfsll_Post_Result state
)
{
  switch ( state ) {
    case CStringsReqFfsll_Post_Result_Zero: {
      /*
       * The return value of ffsll() shall be equal to zero.
       */
      T_eq_int( ctx->result, 0 );
      break;
    }

    case CStringsReqFfsll_Post_Result_Index: {
      /*
       * The return value of ffsll() shall be the number of the least
       * significant bit set in the value of the `i` parameter, with the bits
       * of the value numbered from one at the least significant bit.
       */
      unsigned long long value;
      int                expected;

      value = (unsigned long long) ctx->i;
      expected = 1;

      while ( ( value & 1U ) == 0 ) {
        value >>= 1;
        ++expected;
      }

      T_eq_int( ctx->result, expected );
      break;
    }

    case CStringsReqFfsll_Post_Result_NA:
      break;
  }
}

static void CStringsReqFfsll_Action( CStringsReqFfsll_Context *ctx )
{
  ctx->result = ffsll( ctx->i );
}

/* clang-format off */

static const CStringsReqFfsll_Entry
CStringsReqFfsll_Entries[] = {
  { 0, 0, CStringsReqFfsll_Post_Result_Index },
  { 0, 0, CStringsReqFfsll_Post_Result_Zero }
};

static const uint8_t
CStringsReqFfsll_Map[] = {
  0, 1, 0, 0, 0, 0
};

/* clang-format on */

static size_t CStringsReqFfsll_Scope( void *arg, char *buf, size_t n )
{
  CStringsReqFfsll_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    size_t pcd[ 1 ];
    size_t i;

    for ( i = 0; i < 1; ++i ) {
      pcd[ i ] = CStringsReqFfsll_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
                 CStringsReqFfsll_GetRepresentative( ctx, i );
    }

    return T_get_scope( CStringsReqFfsll_PreDesc, buf, n, pcd );
  }

  return 0;
}

static T_fixture CStringsReqFfsll_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringsReqFfsll_Scope,
  .initial_context = &CStringsReqFfsll_Instance
};

static inline CStringsReqFfsll_Entry CStringsReqFfsll_PopEntry(
  CStringsReqFfsll_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringsReqFfsll_Entries[ CStringsReqFfsll_Map[ index ] ];
}

static void CStringsReqFfsll_TestVariant( CStringsReqFfsll_Context *ctx )
{
  CStringsReqFfsll_Pre_Value_Prepare( ctx, ctx->Map.pcs[ 0 ] );

  if ( ctx->Map.skip_rep ) {
    return;
  }

  CStringsReqFfsll_Action( ctx );
  CStringsReqFfsll_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CStringsReqFfsll( void )
 */
T_TEST_CASE_FIXTURE( CStringsReqFfsll, &CStringsReqFfsll_Fixture )
{
  CStringsReqFfsll_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  CStringsReqFfsll_ResetRepresentatives( ctx );

  for (
    ctx->Map.pcs[ 0 ] = CStringsReqFfsll_Pre_Value_MinusOne;
    ctx->Map.pcs[ 0 ] < CStringsReqFfsll_Pre_Value_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CStringsReqFfsll_PopEntry( ctx );

    for (
      ctx->Map.rep = 0;
      ctx->Map.rep < CStringsReqFfsll_GetRepresentativeCount( ctx );
      ++ctx->Map.rep
    ) {
      ctx->Map.skip_rep = false;
      CStringsReqFfsll_TestVariant( ctx );

      CStringsReqFfsll_MarkRepresentatives( ctx );
    }
  }

  CStringsReqFfsll_CheckRepresentatives( ctx );
}

/** @} */
