/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIoReqOutputCharIsDrain
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
 * @defgroup RtemsIoReqOutputCharIsDrain \
 *   spec:/rtems/io/req/output-char-is-drain
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsIoReqOutputCharIsDrain_Pre_Sign_NonNegative,
  RtemsIoReqOutputCharIsDrain_Pre_Sign_Negative,
  RtemsIoReqOutputCharIsDrain_Pre_Sign_NA
} RtemsIoReqOutputCharIsDrain_Pre_Sign;

typedef enum {
  RtemsIoReqOutputCharIsDrain_Pre_Drain_Set,
  RtemsIoReqOutputCharIsDrain_Pre_Drain_Cleared,
  RtemsIoReqOutputCharIsDrain_Pre_Drain_NA
} RtemsIoReqOutputCharIsDrain_Pre_Drain;

typedef enum {
  RtemsIoReqOutputCharIsDrain_Pre_Try_Set,
  RtemsIoReqOutputCharIsDrain_Pre_Try_Cleared,
  RtemsIoReqOutputCharIsDrain_Pre_Try_NA
} RtemsIoReqOutputCharIsDrain_Pre_Try;

typedef enum {
  RtemsIoReqOutputCharIsDrain_Post_Result_True,
  RtemsIoReqOutputCharIsDrain_Post_Result_False,
  RtemsIoReqOutputCharIsDrain_Post_Result_NA
} RtemsIoReqOutputCharIsDrain_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Sign_NA : 1;
  uint8_t Pre_Drain_NA : 1;
  uint8_t Pre_Try_NA : 1;
  uint8_t Post_Result : 2;
} RtemsIoReqOutputCharIsDrain_Entry;

/**
 * @brief Test context for spec:/rtems/io/req/output-char-is-drain test case.
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
    RtemsIoReqOutputCharIsDrain_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsIoReqOutputCharIsDrain_Context;

static RtemsIoReqOutputCharIsDrain_Context
  RtemsIoReqOutputCharIsDrain_Instance;

static const char *const RtemsIoReqOutputCharIsDrain_PreDesc_Sign[] = {
  "NonNegative:Zero",
  "NonNegative:Char",
  "NonNegative:Max",
  "Negative:Min",
  "Negative:MinusOne",
  "NA"
};

static const char *const RtemsIoReqOutputCharIsDrain_PreDesc_Drain[] =
  { "Set", "Cleared", "NA" };

static const char *const RtemsIoReqOutputCharIsDrain_PreDesc_Try[] =
  { "Set", "Cleared", "NA" };

static const char *const *const RtemsIoReqOutputCharIsDrain_PreDesc[] = {
  RtemsIoReqOutputCharIsDrain_PreDesc_Sign,
  RtemsIoReqOutputCharIsDrain_PreDesc_Drain,
  RtemsIoReqOutputCharIsDrain_PreDesc_Try,
  NULL
};

typedef enum {
  RtemsIoReqOutputCharIsDrain_Pre_Sign_NonNegative_Zero,
  RtemsIoReqOutputCharIsDrain_Pre_Sign_NonNegative_Char,
  RtemsIoReqOutputCharIsDrain_Pre_Sign_NonNegative_Max
} RtemsIoReqOutputCharIsDrain_Pre_Sign_NonNegative_Rep;

typedef enum {
  RtemsIoReqOutputCharIsDrain_Pre_Sign_Negative_Min,
  RtemsIoReqOutputCharIsDrain_Pre_Sign_Negative_MinusOne
} RtemsIoReqOutputCharIsDrain_Pre_Sign_Negative_Rep;

static const uint8_t RtemsIoReqOutputCharIsDrain_PreRepOffset_Sign[] =
  { 0, 3, 5, 6 };

static const uint8_t RtemsIoReqOutputCharIsDrain_PreRepOffset_Drain[] =
  { 0, 1, 2, 3 };

static const uint8_t RtemsIoReqOutputCharIsDrain_PreRepOffset_Try[] =
  { 0, 1, 2, 3 };

static const uint8_t *const RtemsIoReqOutputCharIsDrain_PreRepOffset[] = {
  RtemsIoReqOutputCharIsDrain_PreRepOffset_Sign,
  RtemsIoReqOutputCharIsDrain_PreRepOffset_Drain,
  RtemsIoReqOutputCharIsDrain_PreRepOffset_Try
};

static size_t RtemsIoReqOutputCharIsDrain_GetRepresentativeCountOf(
  const RtemsIoReqOutputCharIsDrain_Context *ctx,
  size_t                                     index
)
{
  const uint8_t *offsets;
  size_t         state;

  offsets = RtemsIoReqOutputCharIsDrain_PreRepOffset[ index ];
  state = ctx->Map.pcs[ index ];

  return offsets[ state + 1 ] - offsets[ state ];
}

static size_t RtemsIoReqOutputCharIsDrain_GetRepresentative(
  const RtemsIoReqOutputCharIsDrain_Context *ctx,
  size_t                                     index
)
{
  size_t count;

  count = RtemsIoReqOutputCharIsDrain_GetRepresentativeCountOf( ctx, index );

  if ( ctx->Map.rep < count ) {
    return ctx->Map.rep;
  }

  return count - 1;
}

static size_t RtemsIoReqOutputCharIsDrain_GetRepresentativeCount(
  const RtemsIoReqOutputCharIsDrain_Context *ctx
)
{
  size_t count;
  size_t i;

  count = 1;

  for ( i = 0; i < 3; ++i ) {
    size_t count_of_condition;

    count_of_condition = RtemsIoReqOutputCharIsDrain_GetRepresentativeCountOf(
      ctx,
      i
    );

    if ( count_of_condition > count ) {
      count = count_of_condition;
    }
  }

  return count;
}

static RtemsIoReqOutputCharIsDrain_Pre_Sign_NonNegative_Rep
RtemsIoReqOutputCharIsDrain_Pre_Sign_NonNegative_GetRepresentative(
  const RtemsIoReqOutputCharIsDrain_Context *ctx
)
{
  return (
    RtemsIoReqOutputCharIsDrain_Pre_Sign_NonNegative_Rep
  ) RtemsIoReqOutputCharIsDrain_GetRepresentative( ctx, 0 );
}

static RtemsIoReqOutputCharIsDrain_Pre_Sign_Negative_Rep
RtemsIoReqOutputCharIsDrain_Pre_Sign_Negative_GetRepresentative(
  const RtemsIoReqOutputCharIsDrain_Context *ctx
)
{
  return (
    RtemsIoReqOutputCharIsDrain_Pre_Sign_Negative_Rep
  ) RtemsIoReqOutputCharIsDrain_GetRepresentative( ctx, 0 );
}

static const uint8_t RtemsIoReqOutputCharIsDrain_PreRepBase[] = { 0, 6, 9 };

static const uint8_t RtemsIoReqOutputCharIsDrain_PreRepStates[] = { 2, 2, 2 };

static void RtemsIoReqOutputCharIsDrain_ResetRepresentatives(
  RtemsIoReqOutputCharIsDrain_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 12; ++i ) {
    ctx->Map.rep_ran[ i ] = false;
  }
}

static void RtemsIoReqOutputCharIsDrain_MarkRepresentatives(
  RtemsIoReqOutputCharIsDrain_Context *ctx
)
{
  size_t i;

  if ( ctx->Map.skip_rep ) {
    return;
  }

  for ( i = 0; i < 3; ++i ) {
    size_t rep;

    rep = RtemsIoReqOutputCharIsDrain_PreRepBase[ i ] +
          RtemsIoReqOutputCharIsDrain_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
          RtemsIoReqOutputCharIsDrain_GetRepresentative( ctx, i );
    ctx->Map.rep_ran[ rep ] = true;
  }
}

static void RtemsIoReqOutputCharIsDrain_CheckRepresentatives(
  const RtemsIoReqOutputCharIsDrain_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 3; ++i ) {
    const uint8_t *offsets;
    const bool    *ran;
    size_t         state;

    offsets = RtemsIoReqOutputCharIsDrain_PreRepOffset[ i ];
    ran = &ctx->Map.rep_ran[ RtemsIoReqOutputCharIsDrain_PreRepBase[ i ] ];

    for (
      state = 0; state < RtemsIoReqOutputCharIsDrain_PreRepStates[ i ]; ++state
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
            RtemsIoReqOutputCharIsDrain_PreDesc[ i ][ k ]
          );
        }
      }
    }
  }
}

static void RtemsIoReqOutputCharIsDrain_Pre_Sign_Prepare(
  RtemsIoReqOutputCharIsDrain_Context *ctx,
  RtemsIoReqOutputCharIsDrain_Pre_Sign state
)
{
  switch ( state ) {
    case RtemsIoReqOutputCharIsDrain_Pre_Sign_NonNegative: {
      /*
       * While the `c` parameter is non-negative.
       */
      switch (
        RtemsIoReqOutputCharIsDrain_Pre_Sign_NonNegative_GetRepresentative(
          ctx
        )
      ) {
        case RtemsIoReqOutputCharIsDrain_Pre_Sign_NonNegative_Zero: {
          ctx->c = 0;
          break;
        }

        case RtemsIoReqOutputCharIsDrain_Pre_Sign_NonNegative_Char: {
          ctx->c = 'c';
          break;
        }

        case RtemsIoReqOutputCharIsDrain_Pre_Sign_NonNegative_Max: {
          ctx->c = INT_MAX;
          break;
        }
      }
      break;
    }

    case RtemsIoReqOutputCharIsDrain_Pre_Sign_Negative: {
      /*
       * While the `c` parameter is negative.
       */
      switch (
        RtemsIoReqOutputCharIsDrain_Pre_Sign_Negative_GetRepresentative( ctx )
      ) {
        case RtemsIoReqOutputCharIsDrain_Pre_Sign_Negative_Min: {
          ctx->c = INT_MIN;
          break;
        }

        case RtemsIoReqOutputCharIsDrain_Pre_Sign_Negative_MinusOne: {
          ctx->c = -1;
          break;
        }
      }
      break;
    }

    case RtemsIoReqOutputCharIsDrain_Pre_Sign_NA:
      break;
  }
}

static void RtemsIoReqOutputCharIsDrain_Pre_Drain_Prepare(
  RtemsIoReqOutputCharIsDrain_Context  *ctx,
  RtemsIoReqOutputCharIsDrain_Pre_Drain state
)
{
  switch ( state ) {
    case RtemsIoReqOutputCharIsDrain_Pre_Drain_Set: {
      /*
       * While the `c` parameter sets RTEMS_OUTPUT_CHAR_DRAIN.
       */
      ctx->c |= RTEMS_OUTPUT_CHAR_DRAIN;
      break;
    }

    case RtemsIoReqOutputCharIsDrain_Pre_Drain_Cleared: {
      /*
       * While the `c` parameter clears RTEMS_OUTPUT_CHAR_DRAIN.
       */
      ctx->c &= ~RTEMS_OUTPUT_CHAR_DRAIN;
      break;
    }

    case RtemsIoReqOutputCharIsDrain_Pre_Drain_NA:
      break;
  }
}

static void RtemsIoReqOutputCharIsDrain_Pre_Try_Prepare(
  RtemsIoReqOutputCharIsDrain_Context *ctx,
  RtemsIoReqOutputCharIsDrain_Pre_Try  state
)
{
  switch ( state ) {
    case RtemsIoReqOutputCharIsDrain_Pre_Try_Set: {
      /*
       * While the `c` parameter sets RTEMS_OUTPUT_CHAR_TRY.
       */
      ctx->c |= RTEMS_OUTPUT_CHAR_TRY;
      break;
    }

    case RtemsIoReqOutputCharIsDrain_Pre_Try_Cleared: {
      /*
       * While the `c` parameter clears RTEMS_OUTPUT_CHAR_TRY.
       */
      ctx->c &= ~RTEMS_OUTPUT_CHAR_TRY;
      break;
    }

    case RtemsIoReqOutputCharIsDrain_Pre_Try_NA:
      break;
  }
}

static void RtemsIoReqOutputCharIsDrain_Post_Result_Check(
  RtemsIoReqOutputCharIsDrain_Context    *ctx,
  RtemsIoReqOutputCharIsDrain_Post_Result state
)
{
  switch ( state ) {
    case RtemsIoReqOutputCharIsDrain_Post_Result_True: {
      /*
       * The return value of rtems_output_char_is_drain() shall be true.
       */
      T_true( ctx->result );
      break;
    }

    case RtemsIoReqOutputCharIsDrain_Post_Result_False: {
      /*
       * The return value of rtems_output_char_is_drain() shall be false.
       */
      T_false( ctx->result );
      break;
    }

    case RtemsIoReqOutputCharIsDrain_Post_Result_NA:
      break;
  }
}

static void RtemsIoReqOutputCharIsDrain_Action(
  RtemsIoReqOutputCharIsDrain_Context *ctx
)
{
  ctx->result = rtems_output_char_is_drain( ctx->c );
}

/* clang-format off */

static const RtemsIoReqOutputCharIsDrain_Entry
RtemsIoReqOutputCharIsDrain_Entries[] = {
  { 0, 0, 0, 0, RtemsIoReqOutputCharIsDrain_Post_Result_False },
  { 0, 0, 0, 0, RtemsIoReqOutputCharIsDrain_Post_Result_True }
};

static const uint8_t
RtemsIoReqOutputCharIsDrain_Map[] = {
  1, 1, 0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t RtemsIoReqOutputCharIsDrain_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  RtemsIoReqOutputCharIsDrain_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    size_t pcd[ 3 ];
    size_t i;

    for ( i = 0; i < 3; ++i ) {
      pcd[ i ] =
        RtemsIoReqOutputCharIsDrain_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
        RtemsIoReqOutputCharIsDrain_GetRepresentative( ctx, i );
    }

    return T_get_scope( RtemsIoReqOutputCharIsDrain_PreDesc, buf, n, pcd );
  }

  return 0;
}

static T_fixture RtemsIoReqOutputCharIsDrain_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsIoReqOutputCharIsDrain_Scope,
  .initial_context = &RtemsIoReqOutputCharIsDrain_Instance
};

static inline RtemsIoReqOutputCharIsDrain_Entry
RtemsIoReqOutputCharIsDrain_PopEntry(
  RtemsIoReqOutputCharIsDrain_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsIoReqOutputCharIsDrain_Entries
    [ RtemsIoReqOutputCharIsDrain_Map[ index ] ];
}

static void RtemsIoReqOutputCharIsDrain_TestVariant(
  RtemsIoReqOutputCharIsDrain_Context *ctx
)
{
  RtemsIoReqOutputCharIsDrain_Pre_Sign_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsIoReqOutputCharIsDrain_Pre_Drain_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsIoReqOutputCharIsDrain_Pre_Try_Prepare( ctx, ctx->Map.pcs[ 2 ] );

  if ( ctx->Map.skip_rep ) {
    return;
  }

  RtemsIoReqOutputCharIsDrain_Action( ctx );
  RtemsIoReqOutputCharIsDrain_Post_Result_Check(
    ctx,
    ctx->Map.entry.Post_Result
  );
}

/**
 * @fn void T_case_body_RtemsIoReqOutputCharIsDrain( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsIoReqOutputCharIsDrain,
  &RtemsIoReqOutputCharIsDrain_Fixture
)
{
  RtemsIoReqOutputCharIsDrain_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  RtemsIoReqOutputCharIsDrain_ResetRepresentatives( ctx );

  for (
    ctx->Map.pcs[ 0 ] = RtemsIoReqOutputCharIsDrain_Pre_Sign_NonNegative;
    ctx->Map.pcs[ 0 ] < RtemsIoReqOutputCharIsDrain_Pre_Sign_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsIoReqOutputCharIsDrain_Pre_Drain_Set;
      ctx->Map.pcs[ 1 ] < RtemsIoReqOutputCharIsDrain_Pre_Drain_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsIoReqOutputCharIsDrain_Pre_Try_Set;
        ctx->Map.pcs[ 2 ] < RtemsIoReqOutputCharIsDrain_Pre_Try_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = RtemsIoReqOutputCharIsDrain_PopEntry( ctx );

        for (
          ctx->Map.rep = 0;
          ctx->Map.rep <
          RtemsIoReqOutputCharIsDrain_GetRepresentativeCount( ctx );
          ++ctx->Map.rep
        ) {
          ctx->Map.skip_rep = false;
          RtemsIoReqOutputCharIsDrain_TestVariant( ctx );

          RtemsIoReqOutputCharIsDrain_MarkRepresentatives( ctx );
        }
      }
    }
  }

  RtemsIoReqOutputCharIsDrain_CheckRepresentatives( ctx );
}

/** @} */
