/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIoReqOutputCharIsTry
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
#include <rtems/bspIo.h>

#include <rtems/test.h>

/**
 * @defgroup RtemsIoReqOutputCharIsTry spec:/rtems/io/req/output-char-is-try
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsIoReqOutputCharIsTry_Pre_Sign_NonNegative,
  RtemsIoReqOutputCharIsTry_Pre_Sign_Negative,
  RtemsIoReqOutputCharIsTry_Pre_Sign_NA
} RtemsIoReqOutputCharIsTry_Pre_Sign;

typedef enum {
  RtemsIoReqOutputCharIsTry_Pre_Try_Set,
  RtemsIoReqOutputCharIsTry_Pre_Try_Cleared,
  RtemsIoReqOutputCharIsTry_Pre_Try_NA
} RtemsIoReqOutputCharIsTry_Pre_Try;

typedef enum {
  RtemsIoReqOutputCharIsTry_Pre_Drain_Set,
  RtemsIoReqOutputCharIsTry_Pre_Drain_Cleared,
  RtemsIoReqOutputCharIsTry_Pre_Drain_NA
} RtemsIoReqOutputCharIsTry_Pre_Drain;

typedef enum {
  RtemsIoReqOutputCharIsTry_Post_Result_True,
  RtemsIoReqOutputCharIsTry_Post_Result_False,
  RtemsIoReqOutputCharIsTry_Post_Result_NA
} RtemsIoReqOutputCharIsTry_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Sign_NA : 1;
  uint8_t Pre_Try_NA : 1;
  uint8_t Pre_Drain_NA : 1;
  uint8_t Post_Result : 2;
} RtemsIoReqOutputCharIsTry_Entry;

/**
 * @brief Test context for spec:/rtems/io/req/output-char-is-try test case.
 */
typedef struct {
  /**
   * @brief This member specifies the parameter of the action.
   */
  int c;

  /**
   * @brief This member contains the return value of the action.
   */
  bool result;

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
    bool rep_ran[ 12 ];

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
    RtemsIoReqOutputCharIsTry_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsIoReqOutputCharIsTry_Context;

static RtemsIoReqOutputCharIsTry_Context RtemsIoReqOutputCharIsTry_Instance;

static const char *const RtemsIoReqOutputCharIsTry_PreDesc_Sign[] = {
  "NonNegative:Zero",
  "NonNegative:Char",
  "NonNegative:Max",
  "Negative:Min",
  "Negative:MinusOne",
  "NA"
};

static const char *const RtemsIoReqOutputCharIsTry_PreDesc_Try[] =
  { "Set", "Cleared", "NA" };

static const char *const RtemsIoReqOutputCharIsTry_PreDesc_Drain[] =
  { "Set", "Cleared", "NA" };

static const char *const *const RtemsIoReqOutputCharIsTry_PreDesc[] = {
  RtemsIoReqOutputCharIsTry_PreDesc_Sign,
  RtemsIoReqOutputCharIsTry_PreDesc_Try,
  RtemsIoReqOutputCharIsTry_PreDesc_Drain,
  NULL
};

typedef enum {
  RtemsIoReqOutputCharIsTry_Pre_Sign_NonNegative_Zero,
  RtemsIoReqOutputCharIsTry_Pre_Sign_NonNegative_Char,
  RtemsIoReqOutputCharIsTry_Pre_Sign_NonNegative_Max
} RtemsIoReqOutputCharIsTry_Pre_Sign_NonNegative_Rep;

typedef enum {
  RtemsIoReqOutputCharIsTry_Pre_Sign_Negative_Min,
  RtemsIoReqOutputCharIsTry_Pre_Sign_Negative_MinusOne
} RtemsIoReqOutputCharIsTry_Pre_Sign_Negative_Rep;

static const uint8_t RtemsIoReqOutputCharIsTry_PreRepOffset_Sign[] =
  { 0, 3, 5, 6 };

static const uint8_t RtemsIoReqOutputCharIsTry_PreRepOffset_Try[] =
  { 0, 1, 2, 3 };

static const uint8_t RtemsIoReqOutputCharIsTry_PreRepOffset_Drain[] =
  { 0, 1, 2, 3 };

static const uint8_t *const RtemsIoReqOutputCharIsTry_PreRepOffset[] = {
  RtemsIoReqOutputCharIsTry_PreRepOffset_Sign,
  RtemsIoReqOutputCharIsTry_PreRepOffset_Try,
  RtemsIoReqOutputCharIsTry_PreRepOffset_Drain
};

static size_t RtemsIoReqOutputCharIsTry_GetRepresentativeCountOf(
  const RtemsIoReqOutputCharIsTry_Context *ctx,
  size_t                                   index
)
{
  const uint8_t *offsets;
  size_t         state;

  offsets = RtemsIoReqOutputCharIsTry_PreRepOffset[ index ];
  state = ctx->Map.pcs[ index ];

  return offsets[ state + 1 ] - offsets[ state ];
}

static size_t RtemsIoReqOutputCharIsTry_GetRepresentative(
  const RtemsIoReqOutputCharIsTry_Context *ctx,
  size_t                                   index
)
{
  size_t count;

  count = RtemsIoReqOutputCharIsTry_GetRepresentativeCountOf( ctx, index );

  if ( ctx->Map.rep < count ) {
    return ctx->Map.rep;
  }

  return count - 1;
}

static size_t RtemsIoReqOutputCharIsTry_GetRepresentativeCount(
  const RtemsIoReqOutputCharIsTry_Context *ctx
)
{
  size_t count;
  size_t i;

  count = 1;

  for ( i = 0; i < 3; ++i ) {
    size_t count_of_condition;

    count_of_condition = RtemsIoReqOutputCharIsTry_GetRepresentativeCountOf(
      ctx,
      i
    );

    if ( count_of_condition > count ) {
      count = count_of_condition;
    }
  }

  return count;
}

static RtemsIoReqOutputCharIsTry_Pre_Sign_NonNegative_Rep
RtemsIoReqOutputCharIsTry_Pre_Sign_NonNegative_GetRepresentative(
  const RtemsIoReqOutputCharIsTry_Context *ctx
)
{
  return (
    RtemsIoReqOutputCharIsTry_Pre_Sign_NonNegative_Rep
  ) RtemsIoReqOutputCharIsTry_GetRepresentative( ctx, 0 );
}

static RtemsIoReqOutputCharIsTry_Pre_Sign_Negative_Rep
RtemsIoReqOutputCharIsTry_Pre_Sign_Negative_GetRepresentative(
  const RtemsIoReqOutputCharIsTry_Context *ctx
)
{
  return (
    RtemsIoReqOutputCharIsTry_Pre_Sign_Negative_Rep
  ) RtemsIoReqOutputCharIsTry_GetRepresentative( ctx, 0 );
}

static const uint8_t RtemsIoReqOutputCharIsTry_PreRepBase[] = { 0, 6, 9 };

static const uint8_t RtemsIoReqOutputCharIsTry_PreRepStates[] = { 2, 2, 2 };

static void RtemsIoReqOutputCharIsTry_ResetRepresentatives(
  RtemsIoReqOutputCharIsTry_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 12; ++i ) {
    ctx->Map.rep_ran[ i ] = false;
  }
}

static void RtemsIoReqOutputCharIsTry_MarkRepresentatives(
  RtemsIoReqOutputCharIsTry_Context *ctx
)
{
  size_t i;

  if ( ctx->Map.skip_rep ) {
    return;
  }

  for ( i = 0; i < 3; ++i ) {
    size_t rep;

    rep = RtemsIoReqOutputCharIsTry_PreRepBase[ i ] +
          RtemsIoReqOutputCharIsTry_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
          RtemsIoReqOutputCharIsTry_GetRepresentative( ctx, i );
    ctx->Map.rep_ran[ rep ] = true;
  }
}

static void RtemsIoReqOutputCharIsTry_CheckRepresentatives(
  const RtemsIoReqOutputCharIsTry_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 3; ++i ) {
    const uint8_t *offsets;
    const bool    *ran;
    size_t         state;

    offsets = RtemsIoReqOutputCharIsTry_PreRepOffset[ i ];
    ran = &ctx->Map.rep_ran[ RtemsIoReqOutputCharIsTry_PreRepBase[ i ] ];

    for (
      state = 0; state < RtemsIoReqOutputCharIsTry_PreRepStates[ i ]; ++state
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
            RtemsIoReqOutputCharIsTry_PreDesc[ i ][ k ]
          );
        }
      }
    }
  }
}

static void RtemsIoReqOutputCharIsTry_Pre_Sign_Prepare(
  RtemsIoReqOutputCharIsTry_Context *ctx,
  RtemsIoReqOutputCharIsTry_Pre_Sign state
)
{
  switch ( state ) {
    case RtemsIoReqOutputCharIsTry_Pre_Sign_NonNegative: {
      /*
       * While the `c` parameter is non-negative.
       */
      switch (
        RtemsIoReqOutputCharIsTry_Pre_Sign_NonNegative_GetRepresentative( ctx )
      ) {
        case RtemsIoReqOutputCharIsTry_Pre_Sign_NonNegative_Zero: {
          ctx->c = 0;
          break;
        }

        case RtemsIoReqOutputCharIsTry_Pre_Sign_NonNegative_Char: {
          ctx->c = 'c';
          break;
        }

        case RtemsIoReqOutputCharIsTry_Pre_Sign_NonNegative_Max: {
          ctx->c = INT_MAX;
          break;
        }
      }
      break;
    }

    case RtemsIoReqOutputCharIsTry_Pre_Sign_Negative: {
      /*
       * While the `c` parameter is negative.
       */
      switch (
        RtemsIoReqOutputCharIsTry_Pre_Sign_Negative_GetRepresentative( ctx )
      ) {
        case RtemsIoReqOutputCharIsTry_Pre_Sign_Negative_Min: {
          ctx->c = INT_MIN;
          break;
        }

        case RtemsIoReqOutputCharIsTry_Pre_Sign_Negative_MinusOne: {
          ctx->c = -1;
          break;
        }
      }
      break;
    }

    case RtemsIoReqOutputCharIsTry_Pre_Sign_NA:
      break;
  }
}

static void RtemsIoReqOutputCharIsTry_Pre_Try_Prepare(
  RtemsIoReqOutputCharIsTry_Context *ctx,
  RtemsIoReqOutputCharIsTry_Pre_Try  state
)
{
  switch ( state ) {
    case RtemsIoReqOutputCharIsTry_Pre_Try_Set: {
      /*
       * While the `c` parameter sets RTEMS_OUTPUT_CHAR_TRY.
       */
      ctx->c |= RTEMS_OUTPUT_CHAR_TRY;
      break;
    }

    case RtemsIoReqOutputCharIsTry_Pre_Try_Cleared: {
      /*
       * While the `c` parameter clears RTEMS_OUTPUT_CHAR_TRY.
       */
      ctx->c &= ~RTEMS_OUTPUT_CHAR_TRY;
      break;
    }

    case RtemsIoReqOutputCharIsTry_Pre_Try_NA:
      break;
  }
}

static void RtemsIoReqOutputCharIsTry_Pre_Drain_Prepare(
  RtemsIoReqOutputCharIsTry_Context  *ctx,
  RtemsIoReqOutputCharIsTry_Pre_Drain state
)
{
  switch ( state ) {
    case RtemsIoReqOutputCharIsTry_Pre_Drain_Set: {
      /*
       * While the `c` parameter sets RTEMS_OUTPUT_CHAR_DRAIN.
       */
      ctx->c |= RTEMS_OUTPUT_CHAR_DRAIN;
      break;
    }

    case RtemsIoReqOutputCharIsTry_Pre_Drain_Cleared: {
      /*
       * While the `c` parameter clears RTEMS_OUTPUT_CHAR_DRAIN.
       */
      ctx->c &= ~RTEMS_OUTPUT_CHAR_DRAIN;
      break;
    }

    case RtemsIoReqOutputCharIsTry_Pre_Drain_NA:
      break;
  }
}

static void RtemsIoReqOutputCharIsTry_Post_Result_Check(
  RtemsIoReqOutputCharIsTry_Context    *ctx,
  RtemsIoReqOutputCharIsTry_Post_Result state
)
{
  switch ( state ) {
    case RtemsIoReqOutputCharIsTry_Post_Result_True: {
      /*
       * The return value of rtems_output_char_is_try() shall be true.
       */
      T_true( ctx->result );
      break;
    }

    case RtemsIoReqOutputCharIsTry_Post_Result_False: {
      /*
       * The return value of rtems_output_char_is_try() shall be false.
       */
      T_false( ctx->result );
      break;
    }

    case RtemsIoReqOutputCharIsTry_Post_Result_NA:
      break;
  }
}

static void RtemsIoReqOutputCharIsTry_Action(
  RtemsIoReqOutputCharIsTry_Context *ctx
)
{
  ctx->result = rtems_output_char_is_try( ctx->c );
}

/* clang-format off */

static const RtemsIoReqOutputCharIsTry_Entry
RtemsIoReqOutputCharIsTry_Entries[] = {
  { 0, 0, 0, 0, RtemsIoReqOutputCharIsTry_Post_Result_False },
  { 0, 0, 0, 0, RtemsIoReqOutputCharIsTry_Post_Result_True }
};

static const uint8_t
RtemsIoReqOutputCharIsTry_Map[] = {
  1, 1, 0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t RtemsIoReqOutputCharIsTry_Scope( void *arg, char *buf, size_t n )
{
  RtemsIoReqOutputCharIsTry_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    size_t pcd[ 3 ];
    size_t i;

    for ( i = 0; i < 3; ++i ) {
      pcd[ i ] = RtemsIoReqOutputCharIsTry_PreRepOffset[ i ]
                                                       [ ctx->Map.pcs[ i ] ] +
                 RtemsIoReqOutputCharIsTry_GetRepresentative( ctx, i );
    }

    return T_get_scope( RtemsIoReqOutputCharIsTry_PreDesc, buf, n, pcd );
  }

  return 0;
}

static T_fixture RtemsIoReqOutputCharIsTry_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsIoReqOutputCharIsTry_Scope,
  .initial_context = &RtemsIoReqOutputCharIsTry_Instance
};

static inline RtemsIoReqOutputCharIsTry_Entry
RtemsIoReqOutputCharIsTry_PopEntry( RtemsIoReqOutputCharIsTry_Context *ctx )
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsIoReqOutputCharIsTry_Entries
    [ RtemsIoReqOutputCharIsTry_Map[ index ] ];
}

static void RtemsIoReqOutputCharIsTry_TestVariant(
  RtemsIoReqOutputCharIsTry_Context *ctx
)
{
  RtemsIoReqOutputCharIsTry_Pre_Sign_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsIoReqOutputCharIsTry_Pre_Try_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsIoReqOutputCharIsTry_Pre_Drain_Prepare( ctx, ctx->Map.pcs[ 2 ] );

  if ( ctx->Map.skip_rep ) {
    return;
  }

  RtemsIoReqOutputCharIsTry_Action( ctx );
  RtemsIoReqOutputCharIsTry_Post_Result_Check(
    ctx,
    ctx->Map.entry.Post_Result
  );
}

/**
 * @fn void T_case_body_RtemsIoReqOutputCharIsTry( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsIoReqOutputCharIsTry,
  &RtemsIoReqOutputCharIsTry_Fixture
)
{
  RtemsIoReqOutputCharIsTry_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  RtemsIoReqOutputCharIsTry_ResetRepresentatives( ctx );

  for (
    ctx->Map.pcs[ 0 ] = RtemsIoReqOutputCharIsTry_Pre_Sign_NonNegative;
    ctx->Map.pcs[ 0 ] < RtemsIoReqOutputCharIsTry_Pre_Sign_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsIoReqOutputCharIsTry_Pre_Try_Set;
      ctx->Map.pcs[ 1 ] < RtemsIoReqOutputCharIsTry_Pre_Try_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsIoReqOutputCharIsTry_Pre_Drain_Set;
        ctx->Map.pcs[ 2 ] < RtemsIoReqOutputCharIsTry_Pre_Drain_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = RtemsIoReqOutputCharIsTry_PopEntry( ctx );

        for (
          ctx->Map.rep = 0;
          ctx->Map.rep <
          RtemsIoReqOutputCharIsTry_GetRepresentativeCount( ctx );
          ++ctx->Map.rep
        ) {
          ctx->Map.skip_rep = false;
          RtemsIoReqOutputCharIsTry_TestVariant( ctx );

          RtemsIoReqOutputCharIsTry_MarkRepresentatives( ctx );
        }
      }
    }
  }

  RtemsIoReqOutputCharIsTry_CheckRepresentatives( ctx );
}

/** @} */
