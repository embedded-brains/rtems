/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup NewlibReqSchedIndex
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

#include "ts-config.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup NewlibReqSchedIndex spec:/newlib/req/sched-index
 *
 * @ingroup TestsuitesValidationNoClock1
 *
 * @{
 */

typedef enum {
  NewlibReqSchedIndex_Pre_Home_First,
  NewlibReqSchedIndex_Pre_Home_Second,
  NewlibReqSchedIndex_Pre_Home_Third,
  NewlibReqSchedIndex_Pre_Home_Fourth,
  NewlibReqSchedIndex_Pre_Home_NA
} NewlibReqSchedIndex_Pre_Home;

typedef enum {
  NewlibReqSchedIndex_Post_Result_Home,
  NewlibReqSchedIndex_Post_Result_NA
} NewlibReqSchedIndex_Post_Result;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Home_NA : 1;
  uint8_t Post_Result : 1;
} NewlibReqSchedIndex_Entry;

/**
 * @brief Test context for spec:/newlib/req/sched-index test case.
 */
typedef struct {
  /**
   * @brief This member contains the return value of the _Sched_Index() call.
   */
  int index;

  /**
   * @brief This member contains the index which the call shall return.
   */
  int expected_index;

  /**
   * @brief This member is true, if the home scheduler of the calling thread is
   *   not the first scheduler.
   */
  bool scheduler_other;

  /**
   * @brief This member contains the identifier of the scheduler which owns the
   *   processor with index one.
   */
  rtems_id processor_owner;

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
    NewlibReqSchedIndex_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} NewlibReqSchedIndex_Context;

static NewlibReqSchedIndex_Context NewlibReqSchedIndex_Instance;

static const char *const NewlibReqSchedIndex_PreDesc_Home[] =
  { "First", "Second", "Third", "Fourth", "NA" };

static const char *const *const NewlibReqSchedIndex_PreDesc[] = {
  NewlibReqSchedIndex_PreDesc_Home,
  NULL
};

typedef NewlibReqSchedIndex_Context Context;

#if defined( RTEMS_SMP )
static void MoveProcessorOne( Context *ctx, rtems_id scheduler_id )
{
  RemoveProcessor( SCHEDULER_B_ID, 1 );
  AddProcessor( scheduler_id, 1 );
  ctx->processor_owner = scheduler_id;
}
#endif

static void NewlibReqSchedIndex_Pre_Home_Prepare(
  NewlibReqSchedIndex_Context *ctx,
  NewlibReqSchedIndex_Pre_Home state
)
{
  switch ( state ) {
    case NewlibReqSchedIndex_Pre_Home_First: {
      /*
       * While the home scheduler of the calling thread is the first scheduler
       * of the table of configured schedulers.
       */
      ctx->expected_index = 0;
      break;
    }

    case NewlibReqSchedIndex_Pre_Home_Second: {
      /*
       * While the home scheduler of the calling thread is the second scheduler
       * of the table of configured schedulers.
       */
      #if defined( RTEMS_SMP )
      SetSelfScheduler( SCHEDULER_B_ID, PRIO_NORMAL );
      ctx->scheduler_other = true;
      ctx->expected_index = 1;
      #endif
      break;
    }

    case NewlibReqSchedIndex_Pre_Home_Third: {
      /*
       * While the home scheduler of the calling thread is the third scheduler
       * of the table of configured schedulers.
       */
      #if defined( RTEMS_SMP )
      MoveProcessorOne( ctx, SCHEDULER_C_ID );
      SetSelfScheduler( SCHEDULER_C_ID, PRIO_NORMAL );
      ctx->scheduler_other = true;
      ctx->expected_index = 2;
      #endif
      break;
    }

    case NewlibReqSchedIndex_Pre_Home_Fourth: {
      /*
       * While the home scheduler of the calling thread is the fourth scheduler
       * of the table of configured schedulers.
       */
      #if defined( RTEMS_SMP )
      MoveProcessorOne( ctx, SCHEDULER_D_ID );
      SetSelfScheduler( SCHEDULER_D_ID, PRIO_NORMAL );
      ctx->scheduler_other = true;
      ctx->expected_index = 3;
      #endif
      break;
    }

    case NewlibReqSchedIndex_Pre_Home_NA:
      break;
  }
}

static void NewlibReqSchedIndex_Post_Result_Check(
  NewlibReqSchedIndex_Context    *ctx,
  NewlibReqSchedIndex_Post_Result state
)
{
  switch ( state ) {
    case NewlibReqSchedIndex_Post_Result_Home: {
      /*
       * The return value of _Sched_Index() shall be the index of the home
       * scheduler of the calling thread, which is the position of that
       * scheduler in the table of configured schedulers counted from zero.
       */
      T_eq_int( ctx->index, ctx->expected_index );
      break;
    }

    case NewlibReqSchedIndex_Post_Result_NA:
      break;
  }
}

static void NewlibReqSchedIndex_Prepare( NewlibReqSchedIndex_Context *ctx )
{
  ctx->index = -2;
  ctx->expected_index = -3;
  ctx->scheduler_other = false;
  ctx->processor_owner = SCHEDULER_B_ID;
}

static void NewlibReqSchedIndex_Action( NewlibReqSchedIndex_Context *ctx )
{
  ctx->index = _Sched_Index();
}

static void NewlibReqSchedIndex_Cleanup( NewlibReqSchedIndex_Context *ctx )
{
  #if defined( RTEMS_SMP )
  if ( ctx->scheduler_other ) {
    RestoreRunnerScheduler();
  }

  if ( ctx->processor_owner != SCHEDULER_B_ID ) {
    RemoveProcessor( ctx->processor_owner, 1 );
    AddProcessor( SCHEDULER_B_ID, 1 );
  }
  #else
  (void) ctx;
  #endif
}

/* clang-format off */

static const NewlibReqSchedIndex_Entry
NewlibReqSchedIndex_Entries[] = {
#if !defined(RTEMS_SMP)
  { 1, 0, NewlibReqSchedIndex_Post_Result_NA },
#else
  { 0, 0, NewlibReqSchedIndex_Post_Result_Home },
#endif
  { 0, 0, NewlibReqSchedIndex_Post_Result_Home }
};

static const uint8_t
NewlibReqSchedIndex_Map[] = {
  1, 0, 0, 0
};

/* clang-format on */

static size_t NewlibReqSchedIndex_Scope( void *arg, char *buf, size_t n )
{
  NewlibReqSchedIndex_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( NewlibReqSchedIndex_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture NewlibReqSchedIndex_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NewlibReqSchedIndex_Scope,
  .initial_context = &NewlibReqSchedIndex_Instance
};

static inline NewlibReqSchedIndex_Entry NewlibReqSchedIndex_PopEntry(
  NewlibReqSchedIndex_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return NewlibReqSchedIndex_Entries[ NewlibReqSchedIndex_Map[ index ] ];
}

static void NewlibReqSchedIndex_TestVariant( NewlibReqSchedIndex_Context *ctx )
{
  NewlibReqSchedIndex_Pre_Home_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  NewlibReqSchedIndex_Action( ctx );
  NewlibReqSchedIndex_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
}

/**
 * @fn void T_case_body_NewlibReqSchedIndex( void )
 */
T_TEST_CASE_FIXTURE( NewlibReqSchedIndex, &NewlibReqSchedIndex_Fixture )
{
  NewlibReqSchedIndex_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = NewlibReqSchedIndex_Pre_Home_First;
    ctx->Map.pcs[ 0 ] < NewlibReqSchedIndex_Pre_Home_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = NewlibReqSchedIndex_PopEntry( ctx );

    if ( ctx->Map.entry.Skip ) {
      continue;
    }

    NewlibReqSchedIndex_Prepare( ctx );
    NewlibReqSchedIndex_TestVariant( ctx );
    NewlibReqSchedIndex_Cleanup( ctx );
  }
}

/** @} */
