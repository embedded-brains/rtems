/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringsReqFfsl
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
 * @defgroup CStringsReqFfsl spec:/c/strings/req/ffsl
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CStringsReqFfsl_Pre_Value_MinusOne,
  CStringsReqFfsl_Pre_Value_Zero,
  CStringsReqFfsl_Pre_Value_One,
  CStringsReqFfsl_Pre_Value_Least,
  CStringsReqFfsl_Pre_Value_Middle,
  CStringsReqFfsl_Pre_Value_Most,
  CStringsReqFfsl_Pre_Value_NA
} CStringsReqFfsl_Pre_Value;

typedef enum {
  CStringsReqFfsl_Post_Result_Zero,
  CStringsReqFfsl_Post_Result_Index,
  CStringsReqFfsl_Post_Result_NA
} CStringsReqFfsl_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Value_NA : 1;
  uint8_t Post_Result : 2;
} CStringsReqFfsl_Entry;

/**
 * @brief Test context for spec:/c/strings/req/ffsl test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `i` parameter value.
   */
  long i;

  /**
   * @brief This member contains the return value of the ffsl() call.
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
    bool rep_ran[ 11 ];

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
    CStringsReqFfsl_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringsReqFfsl_Context;

static CStringsReqFfsl_Context CStringsReqFfsl_Instance;

static const char *const CStringsReqFfsl_PreDesc_Value[] = {
  "MinusOne",
  "Zero",
  "One",
  "LeastLeast",
  "LeastLeastNegative",
  "MiddleTwo",
  "MiddleInner",
  "MiddleBelowMost",
  "MiddleNegative",
  "Most",
  "NA"
};

static const char *const *const CStringsReqFfsl_PreDesc[] = {
  CStringsReqFfsl_PreDesc_Value,
  NULL
};

typedef enum {
  CStringsReqFfsl_Pre_Value_Least_Least,
  CStringsReqFfsl_Pre_Value_Least_LeastNegative
} CStringsReqFfsl_Pre_Value_Least_Rep;

typedef enum {
  CStringsReqFfsl_Pre_Value_Middle_Two,
  CStringsReqFfsl_Pre_Value_Middle_Inner,
  CStringsReqFfsl_Pre_Value_Middle_BelowMost,
  CStringsReqFfsl_Pre_Value_Middle_Negative
} CStringsReqFfsl_Pre_Value_Middle_Rep;

static const uint8_t CStringsReqFfsl_PreRepOffset_Value[] =
  { 0, 1, 2, 3, 5, 9, 10, 11 };

static const uint8_t *const CStringsReqFfsl_PreRepOffset[] = {
  CStringsReqFfsl_PreRepOffset_Value
};

static size_t CStringsReqFfsl_GetRepresentativeCountOf(
  const CStringsReqFfsl_Context *ctx,
  size_t                         index
)
{
  const uint8_t *offsets;
  size_t         state;

  offsets = CStringsReqFfsl_PreRepOffset[ index ];
  state = ctx->Map.pcs[ index ];

  return offsets[ state + 1 ] - offsets[ state ];
}

static size_t CStringsReqFfsl_GetRepresentative(
  const CStringsReqFfsl_Context *ctx,
  size_t                         index
)
{
  size_t count;

  count = CStringsReqFfsl_GetRepresentativeCountOf( ctx, index );

  if ( ctx->Map.rep < count ) {
    return ctx->Map.rep;
  }

  return count - 1;
}

static size_t CStringsReqFfsl_GetRepresentativeCount(
  const CStringsReqFfsl_Context *ctx
)
{
  size_t count;
  size_t i;

  count = 1;

  for ( i = 0; i < 1; ++i ) {
    size_t count_of_condition;

    count_of_condition = CStringsReqFfsl_GetRepresentativeCountOf( ctx, i );

    if ( count_of_condition > count ) {
      count = count_of_condition;
    }
  }

  return count;
}

static CStringsReqFfsl_Pre_Value_Least_Rep
CStringsReqFfsl_Pre_Value_Least_GetRepresentative(
  const CStringsReqFfsl_Context *ctx
)
{
  return (
    CStringsReqFfsl_Pre_Value_Least_Rep
  ) CStringsReqFfsl_GetRepresentative( ctx, 0 );
}

static CStringsReqFfsl_Pre_Value_Middle_Rep
CStringsReqFfsl_Pre_Value_Middle_GetRepresentative(
  const CStringsReqFfsl_Context *ctx
)
{
  return (
    CStringsReqFfsl_Pre_Value_Middle_Rep
  ) CStringsReqFfsl_GetRepresentative( ctx, 0 );
}

static const uint8_t CStringsReqFfsl_PreRepBase[] = { 0 };

static const uint8_t CStringsReqFfsl_PreRepStates[] = { 6 };

static void CStringsReqFfsl_ResetRepresentatives(
  CStringsReqFfsl_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 11; ++i ) {
    ctx->Map.rep_ran[ i ] = false;
  }
}

static void CStringsReqFfsl_MarkRepresentatives( CStringsReqFfsl_Context *ctx )
{
  size_t i;

  if ( ctx->Map.skip_rep ) {
    return;
  }

  for ( i = 0; i < 1; ++i ) {
    size_t rep;

    rep = CStringsReqFfsl_PreRepBase[ i ] +
          CStringsReqFfsl_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
          CStringsReqFfsl_GetRepresentative( ctx, i );
    ctx->Map.rep_ran[ rep ] = true;
  }
}

static void CStringsReqFfsl_CheckRepresentatives(
  const CStringsReqFfsl_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 1; ++i ) {
    const uint8_t *offsets;
    const bool    *ran;
    size_t         state;

    offsets = CStringsReqFfsl_PreRepOffset[ i ];
    ran = &ctx->Map.rep_ran[ CStringsReqFfsl_PreRepBase[ i ] ];

    for ( state = 0; state < CStringsReqFfsl_PreRepStates[ i ]; ++state ) {
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
            CStringsReqFfsl_PreDesc[ i ][ k ]
          );
        }
      }
    }
  }
}

static void CStringsReqFfsl_Pre_Value_Prepare(
  CStringsReqFfsl_Context  *ctx,
  CStringsReqFfsl_Pre_Value state
)
{
  switch ( state ) {
    case CStringsReqFfsl_Pre_Value_MinusOne: {
      /*
       * While the value of the `i` parameter is equal to minus one.
       */
      ctx->i = -1L;
      break;
    }

    case CStringsReqFfsl_Pre_Value_Zero: {
      /*
       * While the value of the `i` parameter is equal to zero.
       */
      ctx->i = 0;
      break;
    }

    case CStringsReqFfsl_Pre_Value_One: {
      /*
       * While the value of the `i` parameter is equal to one.
       */
      ctx->i = 1L;
      break;
    }

    case CStringsReqFfsl_Pre_Value_Least: {
      /*
       * While the least significant bit of the value of the `i` parameter is
       * set, while the value of the `i` parameter is neither one nor minus
       * one.
       */
      switch ( CStringsReqFfsl_Pre_Value_Least_GetRepresentative( ctx ) ) {
        case CStringsReqFfsl_Pre_Value_Least_Least: {
          ctx->i = 0x00080101L;
          break;
        }

        case CStringsReqFfsl_Pre_Value_Least_LeastNegative: {
          ctx->i = LONG_MIN + 1;
          break;
        }
      }
      break;
    }

    case CStringsReqFfsl_Pre_Value_Middle: {
      /*
       * While the least significant bit set in the value of the `i` parameter
       * is neither the least significant bit nor the most significant bit of
       * the value.
       */
      switch ( CStringsReqFfsl_Pre_Value_Middle_GetRepresentative( ctx ) ) {
        case CStringsReqFfsl_Pre_Value_Middle_Two: {
          ctx->i = 2L;
          break;
        }

        case CStringsReqFfsl_Pre_Value_Middle_Inner: {
          ctx->i = (long) ( 0x00080100UL
                            << ( sizeof( long ) * CHAR_BIT - 32 ) );
          break;
        }

        case CStringsReqFfsl_Pre_Value_Middle_BelowMost: {
          ctx->i = (long) ( 1UL << ( sizeof( long ) * CHAR_BIT - 2 ) );
          break;
        }

        case CStringsReqFfsl_Pre_Value_Middle_Negative: {
          ctx->i = LONG_MIN | 0x00000100L;
          break;
        }
      }
      break;
    }

    case CStringsReqFfsl_Pre_Value_Most: {
      /*
       * While the most significant bit is the only bit set in the value of the
       * `i` parameter.
       */
      ctx->i = LONG_MIN;
      break;
    }

    case CStringsReqFfsl_Pre_Value_NA:
      break;
  }
}

static void CStringsReqFfsl_Post_Result_Check(
  CStringsReqFfsl_Context    *ctx,
  CStringsReqFfsl_Post_Result state
)
{
  switch ( state ) {
    case CStringsReqFfsl_Post_Result_Zero: {
      /*
       * The return value of ffsl() shall be equal to zero.
       */
      T_eq_int( ctx->result, 0 );
      break;
    }

    case CStringsReqFfsl_Post_Result_Index: {
      /*
       * The return value of ffsl() shall be the number of the least
       * significant bit set in the value of the `i` parameter, with the bits
       * of the value numbered from one at the least significant bit.
       */
      unsigned long value;
      int           expected;

      value = (unsigned long) ctx->i;
      expected = 1;

      while ( ( value & 1U ) == 0 ) {
        value >>= 1;
        ++expected;
      }

      T_eq_int( ctx->result, expected );
      break;
    }

    case CStringsReqFfsl_Post_Result_NA:
      break;
  }
}

static void CStringsReqFfsl_Action( CStringsReqFfsl_Context *ctx )
{
  ctx->result = ffsl( ctx->i );
}

/* clang-format off */

static const CStringsReqFfsl_Entry
CStringsReqFfsl_Entries[] = {
  { 0, 0, CStringsReqFfsl_Post_Result_Index },
  { 0, 0, CStringsReqFfsl_Post_Result_Zero }
};

static const uint8_t
CStringsReqFfsl_Map[] = {
  0, 1, 0, 0, 0, 0
};

/* clang-format on */

static size_t CStringsReqFfsl_Scope( void *arg, char *buf, size_t n )
{
  CStringsReqFfsl_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    size_t pcd[ 1 ];
    size_t i;

    for ( i = 0; i < 1; ++i ) {
      pcd[ i ] = CStringsReqFfsl_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
                 CStringsReqFfsl_GetRepresentative( ctx, i );
    }

    return T_get_scope( CStringsReqFfsl_PreDesc, buf, n, pcd );
  }

  return 0;
}

static T_fixture CStringsReqFfsl_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringsReqFfsl_Scope,
  .initial_context = &CStringsReqFfsl_Instance
};

static inline CStringsReqFfsl_Entry CStringsReqFfsl_PopEntry(
  CStringsReqFfsl_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringsReqFfsl_Entries[ CStringsReqFfsl_Map[ index ] ];
}

static void CStringsReqFfsl_TestVariant( CStringsReqFfsl_Context *ctx )
{
  CStringsReqFfsl_Pre_Value_Prepare( ctx, ctx->Map.pcs[ 0 ] );

  if ( ctx->Map.skip_rep ) {
    return;
  }

  CStringsReqFfsl_Action( ctx );
  CStringsReqFfsl_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_CStringsReqFfsl( void )
 */
T_TEST_CASE_FIXTURE( CStringsReqFfsl, &CStringsReqFfsl_Fixture )
{
  CStringsReqFfsl_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  CStringsReqFfsl_ResetRepresentatives( ctx );

  for (
    ctx->Map.pcs[ 0 ] = CStringsReqFfsl_Pre_Value_MinusOne;
    ctx->Map.pcs[ 0 ] < CStringsReqFfsl_Pre_Value_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CStringsReqFfsl_PopEntry( ctx );

    for (
      ctx->Map.rep = 0;
      ctx->Map.rep < CStringsReqFfsl_GetRepresentativeCount( ctx );
      ++ctx->Map.rep
    ) {
      ctx->Map.skip_rep = false;
      CStringsReqFfsl_TestVariant( ctx );

      CStringsReqFfsl_MarkRepresentatives( ctx );
    }
  }

  CStringsReqFfsl_CheckRepresentatives( ctx );
}

/** @} */
