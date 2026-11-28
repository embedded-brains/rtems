/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup NewlibReqSchedProcessorCount
 */

/*
 * Copyright (C) 2026 OHB System AG
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

#include <rtems.h>
#include <sys/lock.h>

#include <rtems/test.h>

/**
 * @defgroup NewlibReqSchedProcessorCount \
 *   spec:/newlib/req/sched-processor-count
 *
 * @ingroup TestsuitesValidationNoClock1
 *
 * @{
 */

typedef enum {
  NewlibReqSchedProcessorCount_Pre_Index_OneProcessor,
  NewlibReqSchedProcessorCount_Pre_Index_TwoProcessors,
  NewlibReqSchedProcessorCount_Pre_Index_Count,
  NewlibReqSchedProcessorCount_Pre_Index_Negative,
  NewlibReqSchedProcessorCount_Pre_Index_NA
} NewlibReqSchedProcessorCount_Pre_Index;

typedef enum {
  NewlibReqSchedProcessorCount_Post_Result_ProcessorCount,
  NewlibReqSchedProcessorCount_Post_Result_Zero,
  NewlibReqSchedProcessorCount_Post_Result_NA
} NewlibReqSchedProcessorCount_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Index_NA : 1;
  uint8_t Post_Result : 2;
} NewlibReqSchedProcessorCount_Entry;

/**
 * @brief Test context for spec:/newlib/req/sched-processor-count test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `index` parameter value.
   */
  int index;

  /**
   * @brief This member contains the return value of the
   *   _Sched_Processor_count() call.
   */
  int result;

  /**
   * @brief This member contains the expected count of processors owned by the
   *   scheduler instance specified by the `index` parameter.
   */
  int expected;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 1 ];

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
    NewlibReqSchedProcessorCount_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} NewlibReqSchedProcessorCount_Context;

static NewlibReqSchedProcessorCount_Context
  NewlibReqSchedProcessorCount_Instance;

static const char *const NewlibReqSchedProcessorCount_PreDesc_Index[] =
  { "OneProcessor", "TwoProcessors", "Count", "Negative", "NA" };

static const char *const *const NewlibReqSchedProcessorCount_PreDesc[] = {
  NewlibReqSchedProcessorCount_PreDesc_Index,
  NULL
};

#if defined( RTEMS_SMP )
#define SCHEDULER_COUNT 4
#else
#define SCHEDULER_COUNT 1
#endif

static void NewlibReqSchedProcessorCount_Pre_Index_Prepare(
  NewlibReqSchedProcessorCount_Context  *ctx,
  NewlibReqSchedProcessorCount_Pre_Index state
)
{
  switch ( state ) {
    case NewlibReqSchedProcessorCount_Pre_Index_OneProcessor: {
      /*
       * While the `index` parameter is the index of a scheduler instance which
       * owns exactly one processor.
       */
      /* Scheduler A owns the processor with index zero */
      ctx->index = 0;
      ctx->expected = 1;
      break;
    }

    case NewlibReqSchedProcessorCount_Pre_Index_TwoProcessors: {
      /*
       * While the `index` parameter is the index of a scheduler instance which
       * owns exactly two processors.
       */
      /* Scheduler C owns the processors with the indices two and three */
      if ( rtems_scheduler_get_processor_maximum() < 4 ) {
        ctx->Map.skip = true;
      }

      ctx->index = 2;
      ctx->expected = 2;
      break;
    }

    case NewlibReqSchedProcessorCount_Pre_Index_Count: {
      /*
       * While the `index` parameter is equal to the count of scheduler
       * instances.
       */
      ctx->index = SCHEDULER_COUNT;
      break;
    }

    case NewlibReqSchedProcessorCount_Pre_Index_Negative: {
      /*
       * While the `index` parameter is negative.
       */
      ctx->index = -1;
      break;
    }

    case NewlibReqSchedProcessorCount_Pre_Index_NA:
      break;
  }
}

static void NewlibReqSchedProcessorCount_Post_Result_Check(
  NewlibReqSchedProcessorCount_Context    *ctx,
  NewlibReqSchedProcessorCount_Post_Result state
)
{
  switch ( state ) {
    case NewlibReqSchedProcessorCount_Post_Result_ProcessorCount: {
      /*
       * The return value of _Sched_Processor_count() shall be the count of
       * processors owned by the scheduler instance specified by the `index`
       * parameter.
       */
      T_eq_int( ctx->result, ctx->expected );
      break;
    }

    case NewlibReqSchedProcessorCount_Post_Result_Zero: {
      /*
       * The return value of _Sched_Processor_count() shall be equal to zero.
       */
      T_eq_int( ctx->result, 0 );
      break;
    }

    case NewlibReqSchedProcessorCount_Post_Result_NA:
      break;
  }
}

static void NewlibReqSchedProcessorCount_Action(
  NewlibReqSchedProcessorCount_Context *ctx
)
{
  ctx->result = _Sched_Processor_count( ctx->index );
}

/* clang-format off */

static const NewlibReqSchedProcessorCount_Entry
NewlibReqSchedProcessorCount_Entries[] = {
  { 0, 0, NewlibReqSchedProcessorCount_Post_Result_ProcessorCount },
  { 0, 0, NewlibReqSchedProcessorCount_Post_Result_Zero }
};

static const uint8_t
NewlibReqSchedProcessorCount_Map[] = {
  0, 0, 1, 1
};

/* clang-format on */

static size_t NewlibReqSchedProcessorCount_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  NewlibReqSchedProcessorCount_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      NewlibReqSchedProcessorCount_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture NewlibReqSchedProcessorCount_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NewlibReqSchedProcessorCount_Scope,
  .initial_context = &NewlibReqSchedProcessorCount_Instance
};

static const uint8_t NewlibReqSchedProcessorCount_Weights[] = { 1 };

static void NewlibReqSchedProcessorCount_Skip(
  NewlibReqSchedProcessorCount_Context *ctx,
  size_t                                index
)
{
  (void) ctx;
  (void) index;
}

static inline NewlibReqSchedProcessorCount_Entry
NewlibReqSchedProcessorCount_PopEntry(
  NewlibReqSchedProcessorCount_Context *ctx
)
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 1; ++i ) {
      index += NewlibReqSchedProcessorCount_Weights[ i ] * ctx->Map.pcs[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return NewlibReqSchedProcessorCount_Entries
    [ NewlibReqSchedProcessorCount_Map[ index ] ];
}

static void NewlibReqSchedProcessorCount_TestVariant(
  NewlibReqSchedProcessorCount_Context *ctx
)
{
  NewlibReqSchedProcessorCount_Pre_Index_Prepare( ctx, ctx->Map.pcs[ 0 ] );

  if ( ctx->Map.skip ) {
    NewlibReqSchedProcessorCount_Skip( ctx, 0 );
    return;
  }

  NewlibReqSchedProcessorCount_Action( ctx );
  NewlibReqSchedProcessorCount_Post_Result_Check(
    ctx,
    ctx->Map.entry.Post_Result
  );
}

/**
 * @fn void T_case_body_NewlibReqSchedProcessorCount( void )
 */
T_TEST_CASE_FIXTURE(
  NewlibReqSchedProcessorCount,
  &NewlibReqSchedProcessorCount_Fixture
)
{
  NewlibReqSchedProcessorCount_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;

  for (
    ctx->Map.pcs[ 0 ] = NewlibReqSchedProcessorCount_Pre_Index_OneProcessor;
    ctx->Map.pcs[ 0 ] < NewlibReqSchedProcessorCount_Pre_Index_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = NewlibReqSchedProcessorCount_PopEntry( ctx );
    NewlibReqSchedProcessorCount_TestVariant( ctx );
  }
}

/** @} */
