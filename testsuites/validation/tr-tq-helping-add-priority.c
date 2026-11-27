/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqHelpingAddPriority
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

#include <string.h>
#include <rtems/score/priorityimpl.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/threadqimpl.h>

#include "tr-tq-helping-add-priority.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreTqReqHelpingAddPriority \
 *   spec:/score/tq/req/helping-add-priority
 *
 * @ingroup TestsuitesValidationNoClock2
 *
 * @{
 */

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_NewQueue_NA : 1;
  uint8_t Pre_OtherQueue_NA : 1;
  uint8_t Post_AddPosition : 2;
  uint8_t Post_AddOrder : 1;
} ScoreTqReqHelpingAddPriority_Entry;

typedef enum { PLACE_INITIAL, PLACE_FIRST, PLACE_SECOND } Place;

/**
 * @brief Test context for spec:/score/tq/req/helping-add-priority test case.
 */
typedef struct {
  /**
   * @brief This member specifies the priority of the thread in the priority
   *   queue of the added helping scheduler. PRIO_INVALID means that this
   *   priority queue does not exist before the addition.
   */
  rtems_task_priority waiter_priority;

  /**
   * @brief This member is true, if the priority queue of the added helping
   *   scheduler contains a thread of a lower priority before the addition.
   */
  bool lower;

  /**
   * @brief This member is true, if the thread queue has a priority queue of a
   *   third scheduler.
   */
  bool other;

  /**
   * @brief This member references the thread queue.
   */
  Thread_queue_Queue *queue;

  /**
   * @brief This member contains the priority queues before the addition.
   */
  TQSnapshot before;

  /**
   * @brief This member contains the priority queues after the addition.
   */
  TQSnapshot added;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTqReqHelpingAddPriority_Run() parameter.
   */
  TQContext *tq_ctx;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTqReqHelpingAddPriority_Run() parameter.
   */
  bool home_only;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 2 ];

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
    ScoreTqReqHelpingAddPriority_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreTqReqHelpingAddPriority_Context;

static ScoreTqReqHelpingAddPriority_Context
  ScoreTqReqHelpingAddPriority_Instance;

static const char *const ScoreTqReqHelpingAddPriority_PreDesc_NewQueue[] = {
  "Empty",
  "HighAlone",
  "HighLower",
  "EqualAlone",
  "EqualLower",
  "LowAlone",
  "LowLower",
  "NA"
};

static const char *const ScoreTqReqHelpingAddPriority_PreDesc_OtherQueue[] =
  { "None", "One", "NA" };

static const char *const *const ScoreTqReqHelpingAddPriority_PreDesc[] = {
  ScoreTqReqHelpingAddPriority_PreDesc_NewQueue,
  ScoreTqReqHelpingAddPriority_PreDesc_OtherQueue,
  NULL
};

typedef enum {
  ScoreTqReqHelpingAddPriority_Pre_NewQueue_High_Alone,
  ScoreTqReqHelpingAddPriority_Pre_NewQueue_High_Lower
} ScoreTqReqHelpingAddPriority_Pre_NewQueue_High_Rep;

typedef enum {
  ScoreTqReqHelpingAddPriority_Pre_NewQueue_Equal_Alone,
  ScoreTqReqHelpingAddPriority_Pre_NewQueue_Equal_Lower
} ScoreTqReqHelpingAddPriority_Pre_NewQueue_Equal_Rep;

typedef enum {
  ScoreTqReqHelpingAddPriority_Pre_NewQueue_Low_Alone,
  ScoreTqReqHelpingAddPriority_Pre_NewQueue_Low_Lower
} ScoreTqReqHelpingAddPriority_Pre_NewQueue_Low_Rep;

static const uint8_t ScoreTqReqHelpingAddPriority_PreRepOffset_NewQueue[] =
  { 0, 1, 3, 5, 7, 8 };

static const uint8_t ScoreTqReqHelpingAddPriority_PreRepOffset_OtherQueue[] =
  { 0, 1, 2, 3 };

static const uint8_t *const ScoreTqReqHelpingAddPriority_PreRepOffset[] = {
  ScoreTqReqHelpingAddPriority_PreRepOffset_NewQueue,
  ScoreTqReqHelpingAddPriority_PreRepOffset_OtherQueue
};

static size_t ScoreTqReqHelpingAddPriority_GetRepresentativeCountOf(
  const ScoreTqReqHelpingAddPriority_Context *ctx,
  size_t                                      index
)
{
  const uint8_t *offsets;
  size_t         state;

  offsets = ScoreTqReqHelpingAddPriority_PreRepOffset[ index ];
  state = ctx->Map.pcs[ index ];

  return offsets[ state + 1 ] - offsets[ state ];
}

static size_t ScoreTqReqHelpingAddPriority_GetRepresentative(
  const ScoreTqReqHelpingAddPriority_Context *ctx,
  size_t                                      index
)
{
  size_t count;

  count = ScoreTqReqHelpingAddPriority_GetRepresentativeCountOf( ctx, index );

  if ( ctx->Map.rep < count ) {
    return ctx->Map.rep;
  }

  return count - 1;
}

static size_t ScoreTqReqHelpingAddPriority_GetRepresentativeCount(
  const ScoreTqReqHelpingAddPriority_Context *ctx
)
{
  size_t count;
  size_t i;

  count = 1;

  for ( i = 0; i < 2; ++i ) {
    size_t count_of_condition;

    count_of_condition = ScoreTqReqHelpingAddPriority_GetRepresentativeCountOf(
      ctx,
      i
    );

    if ( count_of_condition > count ) {
      count = count_of_condition;
    }
  }

  return count;
}

static ScoreTqReqHelpingAddPriority_Pre_NewQueue_High_Rep
ScoreTqReqHelpingAddPriority_Pre_NewQueue_High_GetRepresentative(
  const ScoreTqReqHelpingAddPriority_Context *ctx
)
{
  return (
    ScoreTqReqHelpingAddPriority_Pre_NewQueue_High_Rep
  ) ScoreTqReqHelpingAddPriority_GetRepresentative( ctx, 0 );
}

static ScoreTqReqHelpingAddPriority_Pre_NewQueue_Equal_Rep
ScoreTqReqHelpingAddPriority_Pre_NewQueue_Equal_GetRepresentative(
  const ScoreTqReqHelpingAddPriority_Context *ctx
)
{
  return (
    ScoreTqReqHelpingAddPriority_Pre_NewQueue_Equal_Rep
  ) ScoreTqReqHelpingAddPriority_GetRepresentative( ctx, 0 );
}

static ScoreTqReqHelpingAddPriority_Pre_NewQueue_Low_Rep
ScoreTqReqHelpingAddPriority_Pre_NewQueue_Low_GetRepresentative(
  const ScoreTqReqHelpingAddPriority_Context *ctx
)
{
  return (
    ScoreTqReqHelpingAddPriority_Pre_NewQueue_Low_Rep
  ) ScoreTqReqHelpingAddPriority_GetRepresentative( ctx, 0 );
}

static const uint8_t ScoreTqReqHelpingAddPriority_PreRepBase[] = { 0, 8 };

static const uint8_t ScoreTqReqHelpingAddPriority_PreRepStates[] = { 4, 2 };

static void ScoreTqReqHelpingAddPriority_ResetRepresentatives(
  ScoreTqReqHelpingAddPriority_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 11; ++i ) {
    ctx->Map.rep_ran[ i ] = false;
  }
}

static void ScoreTqReqHelpingAddPriority_MarkRepresentatives(
  ScoreTqReqHelpingAddPriority_Context *ctx
)
{
  size_t i;

  if ( ctx->Map.skip_rep ) {
    return;
  }

  for ( i = 0; i < 2; ++i ) {
    size_t rep;

    rep = ScoreTqReqHelpingAddPriority_PreRepBase[ i ] +
          ScoreTqReqHelpingAddPriority_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
          ScoreTqReqHelpingAddPriority_GetRepresentative( ctx, i );
    ctx->Map.rep_ran[ rep ] = true;
  }
}

static void ScoreTqReqHelpingAddPriority_CheckRepresentatives(
  const ScoreTqReqHelpingAddPriority_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 2; ++i ) {
    const uint8_t *offsets;
    const bool    *ran;
    size_t         state;

    offsets = ScoreTqReqHelpingAddPriority_PreRepOffset[ i ];
    ran = &ctx->Map.rep_ran[ ScoreTqReqHelpingAddPriority_PreRepBase[ i ] ];

    for (
      state = 0; state < ScoreTqReqHelpingAddPriority_PreRepStates[ i ];
      ++state
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
            ScoreTqReqHelpingAddPriority_PreDesc[ i ][ k ]
          );
        }
      }
    }
  }
}

typedef ScoreTqReqHelpingAddPriority_Context Context;

static bool HasProcessors( uint32_t count )
{
  return rtems_scheduler_get_processor_maximum() >= count;
}

static bool IsSticky( const Context *ctx )
{
  return ctx->tq_ctx->enqueue_variant == TQ_ENQUEUE_STICKY;
}

/*
 * A thread which waits in a sticky enqueue does not stop its execution.
 */
static void Enqueue(
  Context        *ctx,
  TQWorkerKind    worker,
  rtems_event_set events
)
{
  if ( IsSticky( ctx ) ) {
    TQSendAndWaitForIntendToBlock( ctx->tq_ctx, worker, events );
  } else {
    TQSendAndWaitForExecutionStop( ctx->tq_ctx, worker, events );
  }
}

static const rtems_tcb *GetTCB( const Context *ctx, TQWorkerKind worker )
{
  return ctx->tq_ctx->worker_tcb[ worker ];
}

#if defined( RTEMS_SMP )
static void Setup( Context *ctx )
{
  if ( HasProcessors( 2 ) ) {
    TQSetScheduler( ctx->tq_ctx, TQ_WORKER_F, SCHEDULER_B_ID, PRIO_LOW );
    TQSetScheduler( ctx->tq_ctx, TQ_WORKER_G, SCHEDULER_B_ID, PRIO_LOW );
    TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_E, SCHEDULER_B_ID, PRIO_HIGH );
  }

  if ( HasProcessors( 3 ) ) {
    TQSetScheduler( ctx->tq_ctx, TQ_HELPER_C, SCHEDULER_C_ID, PRIO_LOW );
  }
}
#endif

static void CheckAddPosition( const Context *ctx, Place place )
{
  const TQPriorityQueueSnapshot *queue;
  size_t                         count;
  size_t                         index;

  index = TQSnapshotFind( &ctx->added, SCHEDULER_B_ID );
  T_ne_sz( index, SIZE_MAX );

  if ( index == SIZE_MAX ) {
    return;
  }

  queue = &ctx->added.queue[ index ];
  count = ctx->lower ? 3 : 2;

  switch ( place ) {
    case PLACE_INITIAL:
      T_eq_sz( queue->count, 1 );
      T_eq_ptr( queue->thread[ 0 ], GetTCB( ctx, TQ_BLOCKER_C ) );
      T_eq_sz( index, ctx->added.count - 1 );
      break;
    case PLACE_FIRST:
      T_eq_sz( queue->count, count );
      T_eq_ptr( queue->thread[ 0 ], GetTCB( ctx, TQ_BLOCKER_C ) );
      T_eq_ptr( queue->thread[ 1 ], GetTCB( ctx, TQ_WORKER_F ) );
      break;
    default:
      T_eq_int( place, PLACE_SECOND );
      T_eq_sz( queue->count, count );
      T_eq_ptr( queue->thread[ 0 ], GetTCB( ctx, TQ_WORKER_F ) );
      T_eq_ptr( queue->thread[ 1 ], GetTCB( ctx, TQ_BLOCKER_C ) );
      break;
  }

  if ( place != PLACE_INITIAL && ctx->lower ) {
    T_eq_ptr( queue->thread[ 2 ], GetTCB( ctx, TQ_WORKER_G ) );
  }
}

/*
 * The priority queues before the addition keep their order.  A new priority
 * queue of the added helping scheduler is not part of the comparison.
 */
static void CheckAddOrder( const Context *ctx )
{
  const Scheduler_Control *added;
  size_t                   i;
  size_t                   j;

  added = _Scheduler_Get_by_id( SCHEDULER_B_ID );
  j = 0;

  for ( i = 0; i < ctx->added.count; ++i ) {
    if (
      ctx->added.queue[ i ].scheduler == added &&
      TQSnapshotFind( &ctx->before, SCHEDULER_B_ID ) == SIZE_MAX
    ) {
      continue;
    }

    T_lt_sz( j, ctx->before.count );

    if ( j < ctx->before.count ) {
      T_eq_ptr(
        ctx->added.queue[ i ].scheduler,
        ctx->before.queue[ j ].scheduler
      );
    }

    ++j;
  }

  T_eq_sz( j, ctx->before.count );
}

static void ScoreTqReqHelpingAddPriority_Pre_NewQueue_Prepare(
  ScoreTqReqHelpingAddPriority_Context     *ctx,
  ScoreTqReqHelpingAddPriority_Pre_NewQueue state
)
{
  switch ( state ) {
    case ScoreTqReqHelpingAddPriority_Pre_NewQueue_Empty: {
      /*
       * While the thread queue has no priority queue associated with the
       * helping scheduler which is added to the thread.
       */
      ctx->waiter_priority = PRIO_INVALID;

      if ( !HasProcessors( 2 ) || IsSticky( ctx ) ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqHelpingAddPriority_Pre_NewQueue_High: {
      /*
       * While the thread queue has a priority queue associated with the
       * helping scheduler which is added to the thread, while the highest
       * priority of the priority queue is higher than the eligible priority of
       * the thread with respect to this helping scheduler.
       */
      switch (
        ScoreTqReqHelpingAddPriority_Pre_NewQueue_High_GetRepresentative( ctx )
      ) {
        case ScoreTqReqHelpingAddPriority_Pre_NewQueue_High_Alone: {
          ctx->lower = false;
          break;
        }

        case ScoreTqReqHelpingAddPriority_Pre_NewQueue_High_Lower: {
          ctx->lower = true;
          break;
        }
      }

      ctx->waiter_priority = PRIO_VERY_HIGH;

      if ( !HasProcessors( 2 ) || IsSticky( ctx ) || ctx->home_only ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqHelpingAddPriority_Pre_NewQueue_Equal: {
      /*
       * While the thread queue has a priority queue associated with the
       * helping scheduler which is added to the thread, while the highest
       * priority of the priority queue is equal to the eligible priority of
       * the thread with respect to this helping scheduler.
       */
      switch (
        ScoreTqReqHelpingAddPriority_Pre_NewQueue_Equal_GetRepresentative(
          ctx
        )
      ) {
        case ScoreTqReqHelpingAddPriority_Pre_NewQueue_Equal_Alone: {
          ctx->lower = false;
          break;
        }

        case ScoreTqReqHelpingAddPriority_Pre_NewQueue_Equal_Lower: {
          ctx->lower = true;
          break;
        }
      }

      ctx->waiter_priority = PRIO_HIGH;

      if ( !HasProcessors( 2 ) || IsSticky( ctx ) || ctx->home_only ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqHelpingAddPriority_Pre_NewQueue_Low: {
      /*
       * While the thread queue has a priority queue associated with the
       * helping scheduler which is added to the thread, while the highest
       * priority of the priority queue is lower than the eligible priority of
       * the thread with respect to this helping scheduler.
       */
      switch (
        ScoreTqReqHelpingAddPriority_Pre_NewQueue_Low_GetRepresentative( ctx )
      ) {
        case ScoreTqReqHelpingAddPriority_Pre_NewQueue_Low_Alone: {
          ctx->lower = false;
          break;
        }

        case ScoreTqReqHelpingAddPriority_Pre_NewQueue_Low_Lower: {
          ctx->lower = true;
          break;
        }
      }

      ctx->waiter_priority = PRIO_NORMAL;

      if ( !HasProcessors( 2 ) || IsSticky( ctx ) || ctx->home_only ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqHelpingAddPriority_Pre_NewQueue_NA:
      break;
  }
}

static void ScoreTqReqHelpingAddPriority_Pre_OtherQueue_Prepare(
  ScoreTqReqHelpingAddPriority_Context       *ctx,
  ScoreTqReqHelpingAddPriority_Pre_OtherQueue state
)
{
  switch ( state ) {
    case ScoreTqReqHelpingAddPriority_Pre_OtherQueue_None: {
      /*
       * While each priority queue of the thread queue is associated with the
       * home scheduler of the thread or with the helping scheduler which is
       * added to the thread.
       */
      ctx->other = false;
      break;
    }

    case ScoreTqReqHelpingAddPriority_Pre_OtherQueue_One: {
      /*
       * While the thread queue has a priority queue which is associated with a
       * scheduler other than the home scheduler of the thread and the helping
       * scheduler which is added to the thread.
       */
      ctx->other = true;

      if ( !HasProcessors( 3 ) || ctx->home_only ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqHelpingAddPriority_Pre_OtherQueue_NA:
      break;
  }
}

static void ScoreTqReqHelpingAddPriority_Post_AddPosition_Check(
  ScoreTqReqHelpingAddPriority_Context         *ctx,
  ScoreTqReqHelpingAddPriority_Post_AddPosition state
)
{
  switch ( state ) {
    case ScoreTqReqHelpingAddPriority_Post_AddPosition_Initial: {
      /*
       * The thread queue shall position the priority queue associated with the
       * helping scheduler which is added to the thread after each other
       * priority queue of the thread queue.
       */
      CheckAddPosition( ctx, PLACE_INITIAL );
      break;
    }

    case ScoreTqReqHelpingAddPriority_Post_AddPosition_First: {
      /*
       * The thread queue shall position the thread as the first thread of the
       * priority queue associated with the helping scheduler which is added to
       * the thread.
       */
      CheckAddPosition( ctx, PLACE_FIRST );
      break;
    }

    case ScoreTqReqHelpingAddPriority_Post_AddPosition_Second: {
      /*
       * The thread queue shall position the thread in the priority queue
       * associated with the helping scheduler which is added to the thread
       * after each thread of the priority queue with a priority higher than or
       * equal to the eligible priority of the thread with respect to this
       * helping scheduler and before each thread of the priority queue with a
       * lower priority.
       */
      CheckAddPosition( ctx, PLACE_SECOND );
      break;
    }

    case ScoreTqReqHelpingAddPriority_Post_AddPosition_NA:
      break;
  }
}

static void ScoreTqReqHelpingAddPriority_Post_AddOrder_Check(
  ScoreTqReqHelpingAddPriority_Context      *ctx,
  ScoreTqReqHelpingAddPriority_Post_AddOrder state
)
{
  switch ( state ) {
    case ScoreTqReqHelpingAddPriority_Post_AddOrder_Kept: {
      /*
       * The addition of the helping scheduler shall not change the order of
       * the priority queues which the thread queue contained before the
       * addition.
       */
      CheckAddOrder( ctx );
      break;
    }

    case ScoreTqReqHelpingAddPriority_Post_AddOrder_NA:
      break;
  }
}

static void ScoreTqReqHelpingAddPriority_Setup(
  ScoreTqReqHelpingAddPriority_Context *ctx
)
{
  TQReset( ctx->tq_ctx );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_A, PRIO_VERY_HIGH );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_C, PRIO_HIGH );

  #if defined( RTEMS_SMP )
  Setup( ctx );
  #endif
}

static void ScoreTqReqHelpingAddPriority_Setup_Wrap( void *arg )
{
  ScoreTqReqHelpingAddPriority_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqHelpingAddPriority_Setup( ctx );
}

static void ScoreTqReqHelpingAddPriority_Teardown(
  ScoreTqReqHelpingAddPriority_Context *ctx
)
{
  TQReset( ctx->tq_ctx );
  TQSetScheduler( ctx->tq_ctx, TQ_WORKER_F, SCHEDULER_A_ID, PRIO_LOW );
  TQSetScheduler( ctx->tq_ctx, TQ_WORKER_G, SCHEDULER_A_ID, PRIO_LOW );
}

static void ScoreTqReqHelpingAddPriority_Teardown_Wrap( void *arg )
{
  ScoreTqReqHelpingAddPriority_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqHelpingAddPriority_Teardown( ctx );
}

static void ScoreTqReqHelpingAddPriority_Prepare(
  ScoreTqReqHelpingAddPriority_Context *ctx
)
{
  ctx->lower = false;
}

static void ScoreTqReqHelpingAddPriority_Action(
  ScoreTqReqHelpingAddPriority_Context *ctx
)
{
  rtems_status_code sc;

  if ( ctx->waiter_priority != PRIO_INVALID ) {
    TQSetPriority( ctx->tq_ctx, TQ_WORKER_F, ctx->waiter_priority );
  }

  SetSelfPriority( PRIO_ULTRA_LOW );
  ctx->tq_ctx->how_many = 1;
  TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE_PREPARE );
  TQSendAndWaitForExecutionStop(
    ctx->tq_ctx,
    TQ_BLOCKER_C,
    TQ_EVENT_MUTEX_A_OBTAIN
  );
  TQClearDone( ctx->tq_ctx, TQ_BLOCKER_C );
  Enqueue(
    ctx,
    TQ_BLOCKER_C,
    TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER | TQ_EVENT_MUTEX_A_RELEASE
  );
  ++ctx->tq_ctx->how_many;

  if ( ctx->waiter_priority != PRIO_INVALID ) {
    TQClearDone( ctx->tq_ctx, TQ_WORKER_F );
    Enqueue( ctx, TQ_WORKER_F, TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER );
    ++ctx->tq_ctx->how_many;
  }

  if ( ctx->lower ) {
    TQClearDone( ctx->tq_ctx, TQ_WORKER_G );
    Enqueue( ctx, TQ_WORKER_G, TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER );
    ++ctx->tq_ctx->how_many;
  }

  if ( ctx->other ) {
    TQClearDone( ctx->tq_ctx, TQ_HELPER_C );
    Enqueue( ctx, TQ_HELPER_C, TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER );
    ++ctx->tq_ctx->how_many;
  }

  ctx->queue = GetTCB( ctx, TQ_BLOCKER_C )->Wait.queue;
  T_assert_not_null( ctx->queue );
  TQTakeSnapshot( &ctx->before, ctx->queue );
  TQSendAndWaitForExecutionStop(
    ctx->tq_ctx,
    TQ_BLOCKER_E,
    TQ_EVENT_MUTEX_A_OBTAIN
  );
  TQTakeSnapshot( &ctx->added, ctx->queue );

  /*
   * The restart extracts the thread from the thread queue of mutex A, so the
   * waiting thread loses the helping scheduler.
   */
  sc = rtems_task_restart(
    ctx->tq_ctx->worker_id[ TQ_BLOCKER_E ],
    (rtems_task_argument) ctx->tq_ctx
  );
  T_rsc_success( sc );
  TQWaitForExecutionStop( ctx->tq_ctx, TQ_BLOCKER_E );

  TQClearDone( ctx->tq_ctx, TQ_BLOCKER_A );
  TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE_DONE );
  TQWaitForDone( ctx->tq_ctx, TQ_BLOCKER_A );
  TQWaitForDone( ctx->tq_ctx, TQ_BLOCKER_C );
  TQWaitForExecutionStop( ctx->tq_ctx, TQ_BLOCKER_C );

  if ( ctx->waiter_priority != PRIO_INVALID ) {
    TQWaitForDone( ctx->tq_ctx, TQ_WORKER_F );
    TQWaitForExecutionStop( ctx->tq_ctx, TQ_WORKER_F );
  }

  if ( ctx->lower ) {
    TQWaitForDone( ctx->tq_ctx, TQ_WORKER_G );
    TQWaitForExecutionStop( ctx->tq_ctx, TQ_WORKER_G );
  }

  if ( ctx->other ) {
    TQWaitForDone( ctx->tq_ctx, TQ_HELPER_C );
    TQWaitForExecutionStop( ctx->tq_ctx, TQ_HELPER_C );
  }

  TQMutexObtain( ctx->tq_ctx, TQ_MUTEX_A );
  TQMutexRelease( ctx->tq_ctx, TQ_MUTEX_A );
  SetSelfPriority( PRIO_NORMAL );
}

/* clang-format off */

static const ScoreTqReqHelpingAddPriority_Entry
ScoreTqReqHelpingAddPriority_Entries[] = {
#if defined(RTEMS_SMP)
  { 0, 0, 0, ScoreTqReqHelpingAddPriority_Post_AddPosition_Second,
    ScoreTqReqHelpingAddPriority_Post_AddOrder_Kept },
#else
  { 1, 0, 0, ScoreTqReqHelpingAddPriority_Post_AddPosition_NA,
    ScoreTqReqHelpingAddPriority_Post_AddOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, ScoreTqReqHelpingAddPriority_Post_AddPosition_Initial,
    ScoreTqReqHelpingAddPriority_Post_AddOrder_Kept },
#else
  { 1, 0, 0, ScoreTqReqHelpingAddPriority_Post_AddPosition_NA,
    ScoreTqReqHelpingAddPriority_Post_AddOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, ScoreTqReqHelpingAddPriority_Post_AddPosition_First,
    ScoreTqReqHelpingAddPriority_Post_AddOrder_Kept }
#else
  { 1, 0, 0, ScoreTqReqHelpingAddPriority_Post_AddPosition_NA,
    ScoreTqReqHelpingAddPriority_Post_AddOrder_NA }
#endif
};

static const uint8_t
ScoreTqReqHelpingAddPriority_Map[] = {
  1, 1, 0, 0, 0, 0, 2, 2
};

/* clang-format on */

static size_t ScoreTqReqHelpingAddPriority_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  ScoreTqReqHelpingAddPriority_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    size_t pcd[ 2 ];
    size_t i;

    for ( i = 0; i < 2; ++i ) {
      pcd[ i ] =
        ScoreTqReqHelpingAddPriority_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
        ScoreTqReqHelpingAddPriority_GetRepresentative( ctx, i );
    }

    return T_get_scope( ScoreTqReqHelpingAddPriority_PreDesc, buf, n, pcd );
  }

  return 0;
}

static T_fixture ScoreTqReqHelpingAddPriority_Fixture = {
  .setup = ScoreTqReqHelpingAddPriority_Setup_Wrap,
  .stop = NULL,
  .teardown = ScoreTqReqHelpingAddPriority_Teardown_Wrap,
  .scope = ScoreTqReqHelpingAddPriority_Scope,
  .initial_context = &ScoreTqReqHelpingAddPriority_Instance
};

static const uint8_t ScoreTqReqHelpingAddPriority_Weights[] = { 2, 1 };

static void ScoreTqReqHelpingAddPriority_Skip(
  ScoreTqReqHelpingAddPriority_Context *ctx,
  size_t                                index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pcs[ 1 ] = ScoreTqReqHelpingAddPriority_Pre_OtherQueue_NA - 1;
      break;
  }
}

static inline ScoreTqReqHelpingAddPriority_Entry
ScoreTqReqHelpingAddPriority_PopEntry(
  ScoreTqReqHelpingAddPriority_Context *ctx
)
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 2; ++i ) {
      index += ScoreTqReqHelpingAddPriority_Weights[ i ] * ctx->Map.pcs[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return ScoreTqReqHelpingAddPriority_Entries
    [ ScoreTqReqHelpingAddPriority_Map[ index ] ];
}

static void ScoreTqReqHelpingAddPriority_TestVariant(
  ScoreTqReqHelpingAddPriority_Context *ctx
)
{
  ScoreTqReqHelpingAddPriority_Pre_NewQueue_Prepare( ctx, ctx->Map.pcs[ 0 ] );

  if ( ctx->Map.skip ) {
    ScoreTqReqHelpingAddPriority_Skip( ctx, 0 );
    return;
  }

  ScoreTqReqHelpingAddPriority_Pre_OtherQueue_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );

  if ( ctx->Map.skip ) {
    ScoreTqReqHelpingAddPriority_Skip( ctx, 1 );
    return;
  }

  if ( ctx->Map.skip_rep ) {
    return;
  }

  ScoreTqReqHelpingAddPriority_Action( ctx );
  ScoreTqReqHelpingAddPriority_Post_AddPosition_Check(
    ctx,
    ctx->Map.entry.Post_AddPosition
  );
  ScoreTqReqHelpingAddPriority_Post_AddOrder_Check(
    ctx,
    ctx->Map.entry.Post_AddOrder
  );
}

static T_fixture_node ScoreTqReqHelpingAddPriority_Node;

static T_remark ScoreTqReqHelpingAddPriority_Remark = {
  .next = NULL,
  .remark = "ScoreTqReqHelpingAddPriority"
};

void ScoreTqReqHelpingAddPriority_Run( TQContext *tq_ctx, bool home_only )
{
  ScoreTqReqHelpingAddPriority_Context *ctx;

  ctx = &ScoreTqReqHelpingAddPriority_Instance;
  ctx->tq_ctx = tq_ctx;
  ctx->home_only = home_only;

  ctx = T_push_fixture(
    &ScoreTqReqHelpingAddPriority_Node,
    &ScoreTqReqHelpingAddPriority_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;
  ScoreTqReqHelpingAddPriority_ResetRepresentatives( ctx );

  for (
    ctx->Map.pcs[ 0 ] = ScoreTqReqHelpingAddPriority_Pre_NewQueue_Empty;
    ctx->Map.pcs[ 0 ] < ScoreTqReqHelpingAddPriority_Pre_NewQueue_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = ScoreTqReqHelpingAddPriority_Pre_OtherQueue_None;
      ctx->Map.pcs[ 1 ] < ScoreTqReqHelpingAddPriority_Pre_OtherQueue_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = ScoreTqReqHelpingAddPriority_PopEntry( ctx );

      for (
        ctx->Map.rep = 0;
        ctx->Map.rep <
        ScoreTqReqHelpingAddPriority_GetRepresentativeCount( ctx );
        ++ctx->Map.rep
      ) {
        ctx->Map.skip_rep = false;
        ScoreTqReqHelpingAddPriority_Prepare( ctx );
        ScoreTqReqHelpingAddPriority_TestVariant( ctx );

        if ( ctx->Map.skip ) {
          break;
        }

        ScoreTqReqHelpingAddPriority_MarkRepresentatives( ctx );
      }
    }
  }

  ScoreTqReqHelpingAddPriority_CheckRepresentatives( ctx );

  T_add_remark( &ScoreTqReqHelpingAddPriority_Remark );
  T_pop_fixture();
}

/** @} */
