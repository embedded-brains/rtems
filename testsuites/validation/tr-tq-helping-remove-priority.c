/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqHelpingRemovePriority
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

#include "tr-tq-helping-remove-priority.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreTqReqHelpingRemovePriority \
 *   spec:/score/tq/req/helping-remove-priority
 *
 * @ingroup TestsuitesValidationNoClock2
 *
 * @{
 */

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_RemovedQueue_NA : 1;
  uint8_t Pre_OtherQueue_NA : 1;
  uint8_t Post_Thread : 1;
  uint8_t Post_RemovedQueue : 2;
  uint8_t Post_QueueOrder : 1;
} ScoreTqReqHelpingRemovePriority_Entry;

typedef enum { OTHER_NONE, OTHER_BEFORE, OTHER_AFTER } Other;

/**
 * @brief Test context for spec:/score/tq/req/helping-remove-priority test
 *   case.
 */
typedef struct {
  /**
   * @brief This member specifies the priority of the other thread in the
   *   priority queue of the removed helping scheduler. PRIO_INVALID means that
   *   this priority queue contains no other thread.
   */
  rtems_task_priority waiter_priority;

  /**
   * @brief This member specifies the position of the priority queue of a third
   *   scheduler.
   */
  Other other;

  /**
   * @brief This member references the thread queue.
   */
  Thread_queue_Queue *queue;

  /**
   * @brief This member contains the priority queues before the removal.
   */
  TQSnapshot before;

  /**
   * @brief This member contains the priority queues after the removal.
   */
  TQSnapshot after;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTqReqHelpingRemovePriority_Run() parameter.
   */
  TQContext *tq_ctx;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTqReqHelpingRemovePriority_Run() parameter.
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
    bool rep_ran[ 8 ];

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
    ScoreTqReqHelpingRemovePriority_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreTqReqHelpingRemovePriority_Context;

static ScoreTqReqHelpingRemovePriority_Context
  ScoreTqReqHelpingRemovePriority_Instance;

static const char *const
  ScoreTqReqHelpingRemovePriority_PreDesc_RemovedQueue[] =
    { "Alone", "SharedThreadFirst", "SharedThreadSecond", "NA" };

static const char *const ScoreTqReqHelpingRemovePriority_PreDesc_OtherQueue[] =
  { "None", "PresentBefore", "PresentAfter", "NA" };

static const char *const *const ScoreTqReqHelpingRemovePriority_PreDesc[] = {
  ScoreTqReqHelpingRemovePriority_PreDesc_RemovedQueue,
  ScoreTqReqHelpingRemovePriority_PreDesc_OtherQueue,
  NULL
};

typedef enum {
  ScoreTqReqHelpingRemovePriority_Pre_RemovedQueue_Shared_ThreadFirst,
  ScoreTqReqHelpingRemovePriority_Pre_RemovedQueue_Shared_ThreadSecond
} ScoreTqReqHelpingRemovePriority_Pre_RemovedQueue_Shared_Rep;

typedef enum {
  ScoreTqReqHelpingRemovePriority_Pre_OtherQueue_Present_Before,
  ScoreTqReqHelpingRemovePriority_Pre_OtherQueue_Present_After
} ScoreTqReqHelpingRemovePriority_Pre_OtherQueue_Present_Rep;

static const uint8_t
  ScoreTqReqHelpingRemovePriority_PreRepOffset_RemovedQueue[] = { 0, 1, 3, 4 };

static const uint8_t
  ScoreTqReqHelpingRemovePriority_PreRepOffset_OtherQueue[] = { 0, 1, 3, 4 };

static const uint8_t *const ScoreTqReqHelpingRemovePriority_PreRepOffset[] = {
  ScoreTqReqHelpingRemovePriority_PreRepOffset_RemovedQueue,
  ScoreTqReqHelpingRemovePriority_PreRepOffset_OtherQueue
};

static size_t ScoreTqReqHelpingRemovePriority_GetRepresentativeCountOf(
  const ScoreTqReqHelpingRemovePriority_Context *ctx,
  size_t                                         index
)
{
  const uint8_t *offsets;
  size_t         state;

  offsets = ScoreTqReqHelpingRemovePriority_PreRepOffset[ index ];
  state = ctx->Map.pcs[ index ];

  return offsets[ state + 1 ] - offsets[ state ];
}

static size_t ScoreTqReqHelpingRemovePriority_GetRepresentative(
  const ScoreTqReqHelpingRemovePriority_Context *ctx,
  size_t                                         index
)
{
  size_t count;

  count = ScoreTqReqHelpingRemovePriority_GetRepresentativeCountOf(
    ctx,
    index
  );

  if ( ctx->Map.rep < count ) {
    return ctx->Map.rep;
  }

  return count - 1;
}

static size_t ScoreTqReqHelpingRemovePriority_GetRepresentativeCount(
  const ScoreTqReqHelpingRemovePriority_Context *ctx
)
{
  size_t count;
  size_t i;

  count = 1;

  for ( i = 0; i < 2; ++i ) {
    size_t count_of_condition;

    count_of_condition =
      ScoreTqReqHelpingRemovePriority_GetRepresentativeCountOf( ctx, i );

    if ( count_of_condition > count ) {
      count = count_of_condition;
    }
  }

  return count;
}

static ScoreTqReqHelpingRemovePriority_Pre_RemovedQueue_Shared_Rep
ScoreTqReqHelpingRemovePriority_Pre_RemovedQueue_Shared_GetRepresentative(
  const ScoreTqReqHelpingRemovePriority_Context *ctx
)
{
  return (
    ScoreTqReqHelpingRemovePriority_Pre_RemovedQueue_Shared_Rep
  ) ScoreTqReqHelpingRemovePriority_GetRepresentative( ctx, 0 );
}

static ScoreTqReqHelpingRemovePriority_Pre_OtherQueue_Present_Rep
ScoreTqReqHelpingRemovePriority_Pre_OtherQueue_Present_GetRepresentative(
  const ScoreTqReqHelpingRemovePriority_Context *ctx
)
{
  return (
    ScoreTqReqHelpingRemovePriority_Pre_OtherQueue_Present_Rep
  ) ScoreTqReqHelpingRemovePriority_GetRepresentative( ctx, 1 );
}

static const uint8_t ScoreTqReqHelpingRemovePriority_PreRepBase[] = { 0, 4 };

static const uint8_t ScoreTqReqHelpingRemovePriority_PreRepStates[] = { 2, 2 };

static void ScoreTqReqHelpingRemovePriority_ResetRepresentatives(
  ScoreTqReqHelpingRemovePriority_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 8; ++i ) {
    ctx->Map.rep_ran[ i ] = false;
  }
}

static void ScoreTqReqHelpingRemovePriority_MarkRepresentatives(
  ScoreTqReqHelpingRemovePriority_Context *ctx
)
{
  size_t i;

  if ( ctx->Map.skip_rep ) {
    return;
  }

  for ( i = 0; i < 2; ++i ) {
    size_t rep;

    rep = ScoreTqReqHelpingRemovePriority_PreRepBase[ i ] +
          ScoreTqReqHelpingRemovePriority_PreRepOffset[ i ]
                                                      [ ctx->Map.pcs[ i ] ] +
          ScoreTqReqHelpingRemovePriority_GetRepresentative( ctx, i );
    ctx->Map.rep_ran[ rep ] = true;
  }
}

static void ScoreTqReqHelpingRemovePriority_CheckRepresentatives(
  const ScoreTqReqHelpingRemovePriority_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 2; ++i ) {
    const uint8_t *offsets;
    const bool    *ran;
    size_t         state;

    offsets = ScoreTqReqHelpingRemovePriority_PreRepOffset[ i ];
    ran = &ctx->Map.rep_ran[ ScoreTqReqHelpingRemovePriority_PreRepBase[ i ] ];

    for (
      state = 0; state < ScoreTqReqHelpingRemovePriority_PreRepStates[ i ];
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
            ScoreTqReqHelpingRemovePriority_PreDesc[ i ][ k ]
          );
        }
      }
    }
  }
}

typedef ScoreTqReqHelpingRemovePriority_Context Context;

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
    TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_E, SCHEDULER_B_ID, PRIO_HIGH );
  }

  if ( HasProcessors( 3 ) ) {
    TQSetScheduler( ctx->tq_ctx, TQ_HELPER_C, SCHEDULER_C_ID, PRIO_LOW );
  }
}
#endif

static void EnqueueOther( Context *ctx )
{
  TQClearDone( ctx->tq_ctx, TQ_HELPER_C );
  Enqueue( ctx, TQ_HELPER_C, TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER );
  ++ctx->tq_ctx->how_many;
}

static void CheckThread( const Context *ctx )
{
  size_t index;

  index = TQSnapshotFind( &ctx->after, SCHEDULER_B_ID );

  if ( index != SIZE_MAX ) {
    T_false( TQSnapshotContains(
      &ctx->after.queue[ index ],
      GetTCB( ctx, TQ_BLOCKER_C )
    ) );
  }

  index = TQSnapshotFind( &ctx->after, SCHEDULER_A_ID );
  T_ne_sz( index, SIZE_MAX );

  if ( index != SIZE_MAX ) {
    T_true( TQSnapshotContains(
      &ctx->after.queue[ index ],
      GetTCB( ctx, TQ_BLOCKER_C )
    ) );
  }
}

static void CheckRemovedQueue( const Context *ctx, bool kept )
{
  const TQPriorityQueueSnapshot *queue;
  size_t                         index;

  index = TQSnapshotFind( &ctx->after, SCHEDULER_B_ID );

  if ( !kept ) {
    T_eq_sz( index, SIZE_MAX );
    return;
  }

  T_ne_sz( index, SIZE_MAX );

  if ( index == SIZE_MAX ) {
    return;
  }

  queue = &ctx->after.queue[ index ];
  T_eq_sz( queue->count, 1 );
  T_eq_ptr( queue->thread[ 0 ], GetTCB( ctx, TQ_WORKER_F ) );
}

/*
 * The expected priority queues after the removal are the priority queues
 * before the removal without an empty priority queue of the removed helping
 * scheduler.
 */
static void CheckOrder( const Context *ctx )
{
  const Scheduler_Control *removed;
  size_t                   count;
  size_t                   i;

  removed = _Scheduler_Get_by_id( SCHEDULER_B_ID );
  count = 0;

  for ( i = 0; i < ctx->before.count; ++i ) {
    const TQPriorityQueueSnapshot *queue;

    queue = &ctx->before.queue[ i ];

    if ( queue->scheduler == removed && queue->count == 1 ) {
      continue;
    }

    T_lt_sz( count, ctx->after.count );

    if ( count < ctx->after.count ) {
      T_eq_ptr( ctx->after.queue[ count ].scheduler, queue->scheduler );
    }

    ++count;
  }

  T_eq_sz( ctx->after.count, count );
}

static void ScoreTqReqHelpingRemovePriority_Pre_RemovedQueue_Prepare(
  ScoreTqReqHelpingRemovePriority_Context         *ctx,
  ScoreTqReqHelpingRemovePriority_Pre_RemovedQueue state
)
{
  switch ( state ) {
    case ScoreTqReqHelpingRemovePriority_Pre_RemovedQueue_Alone: {
      /*
       * While the priority queue associated with the helping scheduler which
       * is removed from the thread contains no other thread.
       */
      ctx->waiter_priority = PRIO_INVALID;

      if ( !HasProcessors( 2 ) || IsSticky( ctx ) ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqHelpingRemovePriority_Pre_RemovedQueue_Shared: {
      /*
       * While the priority queue associated with the helping scheduler which
       * is removed from the thread contains at least one other thread.
       */
      switch (
        ScoreTqReqHelpingRemovePriority_Pre_RemovedQueue_Shared_GetRepresentative(
          ctx
        )
      ) {
        case ScoreTqReqHelpingRemovePriority_Pre_RemovedQueue_Shared_ThreadFirst: {
          ctx->waiter_priority = PRIO_NORMAL;
          break;
        }

        case ScoreTqReqHelpingRemovePriority_Pre_RemovedQueue_Shared_ThreadSecond: {
          ctx->waiter_priority = PRIO_VERY_HIGH;
          break;
        }
      }

      if ( !HasProcessors( 2 ) || IsSticky( ctx ) || ctx->home_only ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqHelpingRemovePriority_Pre_RemovedQueue_NA:
      break;
  }
}

static void ScoreTqReqHelpingRemovePriority_Pre_OtherQueue_Prepare(
  ScoreTqReqHelpingRemovePriority_Context       *ctx,
  ScoreTqReqHelpingRemovePriority_Pre_OtherQueue state
)
{
  switch ( state ) {
    case ScoreTqReqHelpingRemovePriority_Pre_OtherQueue_None: {
      /*
       * While each priority queue of the thread queue is associated with the
       * home scheduler of the thread or with the helping scheduler which is
       * removed from the thread.
       */
      ctx->other = OTHER_NONE;
      break;
    }

    case ScoreTqReqHelpingRemovePriority_Pre_OtherQueue_Present: {
      /*
       * While the thread queue has a priority queue which is associated with a
       * scheduler other than the home scheduler of the thread and the helping
       * scheduler which is removed from the thread.
       */
      switch (
        ScoreTqReqHelpingRemovePriority_Pre_OtherQueue_Present_GetRepresentative(
          ctx
        )
      ) {
        case ScoreTqReqHelpingRemovePriority_Pre_OtherQueue_Present_Before: {
          ctx->other = OTHER_BEFORE;
          break;
        }

        case ScoreTqReqHelpingRemovePriority_Pre_OtherQueue_Present_After: {
          ctx->other = OTHER_AFTER;
          break;
        }
      }

      if ( !HasProcessors( 3 ) || ctx->home_only ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqHelpingRemovePriority_Pre_OtherQueue_NA:
      break;
  }
}

static void ScoreTqReqHelpingRemovePriority_Post_Thread_Check(
  ScoreTqReqHelpingRemovePriority_Context    *ctx,
  ScoreTqReqHelpingRemovePriority_Post_Thread state
)
{
  switch ( state ) {
    case ScoreTqReqHelpingRemovePriority_Post_Thread_Removed: {
      /*
       * The thread queue shall remove the thread from the priority queue
       * associated with the helping scheduler which is removed from the
       * thread.
       */
      CheckThread( ctx );
      break;
    }

    case ScoreTqReqHelpingRemovePriority_Post_Thread_NA:
      break;
  }
}

static void ScoreTqReqHelpingRemovePriority_Post_RemovedQueue_Check(
  ScoreTqReqHelpingRemovePriority_Context          *ctx,
  ScoreTqReqHelpingRemovePriority_Post_RemovedQueue state
)
{
  switch ( state ) {
    case ScoreTqReqHelpingRemovePriority_Post_RemovedQueue_Removed: {
      /*
       * The thread queue shall remove the priority queue associated with the
       * helping scheduler which is removed from the thread.
       */
      CheckRemovedQueue( ctx, false );
      break;
    }

    case ScoreTqReqHelpingRemovePriority_Post_RemovedQueue_Kept: {
      /*
       * The priority queue associated with the helping scheduler which is
       * removed from the thread shall keep its other threads in their order.
       */
      CheckRemovedQueue( ctx, true );
      break;
    }

    case ScoreTqReqHelpingRemovePriority_Post_RemovedQueue_NA:
      break;
  }
}

static void ScoreTqReqHelpingRemovePriority_Post_QueueOrder_Check(
  ScoreTqReqHelpingRemovePriority_Context        *ctx,
  ScoreTqReqHelpingRemovePriority_Post_QueueOrder state
)
{
  switch ( state ) {
    case ScoreTqReqHelpingRemovePriority_Post_QueueOrder_Kept: {
      /*
       * The removal of the helping scheduler shall not change the order of the
       * other priority queues of the thread queue.
       */
      CheckOrder( ctx );
      break;
    }

    case ScoreTqReqHelpingRemovePriority_Post_QueueOrder_NA:
      break;
  }
}

static void ScoreTqReqHelpingRemovePriority_Setup(
  ScoreTqReqHelpingRemovePriority_Context *ctx
)
{
  TQReset( ctx->tq_ctx );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_A, PRIO_VERY_HIGH );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_C, PRIO_HIGH );

  #if defined( RTEMS_SMP )
  Setup( ctx );
  #endif
}

static void ScoreTqReqHelpingRemovePriority_Setup_Wrap( void *arg )
{
  ScoreTqReqHelpingRemovePriority_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqHelpingRemovePriority_Setup( ctx );
}

static void ScoreTqReqHelpingRemovePriority_Teardown(
  ScoreTqReqHelpingRemovePriority_Context *ctx
)
{
  TQReset( ctx->tq_ctx );
  TQSetScheduler( ctx->tq_ctx, TQ_WORKER_F, SCHEDULER_A_ID, PRIO_LOW );
}

static void ScoreTqReqHelpingRemovePriority_Teardown_Wrap( void *arg )
{
  ScoreTqReqHelpingRemovePriority_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqHelpingRemovePriority_Teardown( ctx );
}

static void ScoreTqReqHelpingRemovePriority_Action(
  ScoreTqReqHelpingRemovePriority_Context *ctx
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

  if ( ctx->other == OTHER_BEFORE ) {
    EnqueueOther( ctx );
  }

  if ( ctx->waiter_priority != PRIO_INVALID ) {
    TQClearDone( ctx->tq_ctx, TQ_WORKER_F );
    Enqueue( ctx, TQ_WORKER_F, TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER );
    ++ctx->tq_ctx->how_many;
  }

  ctx->queue = GetTCB( ctx, TQ_BLOCKER_C )->Wait.queue;
  T_assert_not_null( ctx->queue );
  TQSendAndWaitForExecutionStop(
    ctx->tq_ctx,
    TQ_BLOCKER_E,
    TQ_EVENT_MUTEX_A_OBTAIN
  );

  if ( ctx->other == OTHER_AFTER ) {
    EnqueueOther( ctx );
  }

  TQTakeSnapshot( &ctx->before, ctx->queue );

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
  TQTakeSnapshot( &ctx->after, ctx->queue );

  TQClearDone( ctx->tq_ctx, TQ_BLOCKER_A );
  TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE_DONE );
  TQWaitForDone( ctx->tq_ctx, TQ_BLOCKER_A );
  TQWaitForDone( ctx->tq_ctx, TQ_BLOCKER_C );
  TQWaitForExecutionStop( ctx->tq_ctx, TQ_BLOCKER_C );

  if ( ctx->waiter_priority != PRIO_INVALID ) {
    TQWaitForDone( ctx->tq_ctx, TQ_WORKER_F );
    TQWaitForExecutionStop( ctx->tq_ctx, TQ_WORKER_F );
  }

  if ( ctx->other != OTHER_NONE ) {
    TQWaitForDone( ctx->tq_ctx, TQ_HELPER_C );
    TQWaitForExecutionStop( ctx->tq_ctx, TQ_HELPER_C );
  }

  TQMutexObtain( ctx->tq_ctx, TQ_MUTEX_A );
  TQMutexRelease( ctx->tq_ctx, TQ_MUTEX_A );
  SetSelfPriority( PRIO_NORMAL );
}

/* clang-format off */

static const ScoreTqReqHelpingRemovePriority_Entry
ScoreTqReqHelpingRemovePriority_Entries[] = {
#if defined(RTEMS_SMP)
  { 0, 0, 0, ScoreTqReqHelpingRemovePriority_Post_Thread_Removed,
    ScoreTqReqHelpingRemovePriority_Post_RemovedQueue_Removed,
    ScoreTqReqHelpingRemovePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, ScoreTqReqHelpingRemovePriority_Post_Thread_NA,
    ScoreTqReqHelpingRemovePriority_Post_RemovedQueue_NA,
    ScoreTqReqHelpingRemovePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, ScoreTqReqHelpingRemovePriority_Post_Thread_Removed,
    ScoreTqReqHelpingRemovePriority_Post_RemovedQueue_Kept,
    ScoreTqReqHelpingRemovePriority_Post_QueueOrder_Kept }
#else
  { 1, 0, 0, ScoreTqReqHelpingRemovePriority_Post_Thread_NA,
    ScoreTqReqHelpingRemovePriority_Post_RemovedQueue_NA,
    ScoreTqReqHelpingRemovePriority_Post_QueueOrder_NA }
#endif
};

static const uint8_t
ScoreTqReqHelpingRemovePriority_Map[] = {
  0, 0, 1, 1
};

/* clang-format on */

static size_t ScoreTqReqHelpingRemovePriority_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  ScoreTqReqHelpingRemovePriority_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    size_t pcd[ 2 ];
    size_t i;

    for ( i = 0; i < 2; ++i ) {
      pcd[ i ] = ScoreTqReqHelpingRemovePriority_PreRepOffset
                   [ i ][ ctx->Map.pcs[ i ] ] +
                 ScoreTqReqHelpingRemovePriority_GetRepresentative( ctx, i );
    }

    return T_get_scope( ScoreTqReqHelpingRemovePriority_PreDesc, buf, n, pcd );
  }

  return 0;
}

static T_fixture ScoreTqReqHelpingRemovePriority_Fixture = {
  .setup = ScoreTqReqHelpingRemovePriority_Setup_Wrap,
  .stop = NULL,
  .teardown = ScoreTqReqHelpingRemovePriority_Teardown_Wrap,
  .scope = ScoreTqReqHelpingRemovePriority_Scope,
  .initial_context = &ScoreTqReqHelpingRemovePriority_Instance
};

static const uint8_t ScoreTqReqHelpingRemovePriority_Weights[] = { 2, 1 };

static void ScoreTqReqHelpingRemovePriority_Skip(
  ScoreTqReqHelpingRemovePriority_Context *ctx,
  size_t                                   index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pcs[ 1 ] = ScoreTqReqHelpingRemovePriority_Pre_OtherQueue_NA -
                          1;
      break;
  }
}

static inline ScoreTqReqHelpingRemovePriority_Entry
ScoreTqReqHelpingRemovePriority_PopEntry(
  ScoreTqReqHelpingRemovePriority_Context *ctx
)
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 2; ++i ) {
      index += ScoreTqReqHelpingRemovePriority_Weights[ i ] *
               ctx->Map.pcs[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return ScoreTqReqHelpingRemovePriority_Entries
    [ ScoreTqReqHelpingRemovePriority_Map[ index ] ];
}

static void ScoreTqReqHelpingRemovePriority_TestVariant(
  ScoreTqReqHelpingRemovePriority_Context *ctx
)
{
  ScoreTqReqHelpingRemovePriority_Pre_RemovedQueue_Prepare(
    ctx,
    ctx->Map.pcs[ 0 ]
  );

  if ( ctx->Map.skip ) {
    ScoreTqReqHelpingRemovePriority_Skip( ctx, 0 );
    return;
  }

  ScoreTqReqHelpingRemovePriority_Pre_OtherQueue_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );

  if ( ctx->Map.skip ) {
    ScoreTqReqHelpingRemovePriority_Skip( ctx, 1 );
    return;
  }

  if ( ctx->Map.skip_rep ) {
    return;
  }

  ScoreTqReqHelpingRemovePriority_Action( ctx );
  ScoreTqReqHelpingRemovePriority_Post_Thread_Check(
    ctx,
    ctx->Map.entry.Post_Thread
  );
  ScoreTqReqHelpingRemovePriority_Post_RemovedQueue_Check(
    ctx,
    ctx->Map.entry.Post_RemovedQueue
  );
  ScoreTqReqHelpingRemovePriority_Post_QueueOrder_Check(
    ctx,
    ctx->Map.entry.Post_QueueOrder
  );
}

static T_fixture_node ScoreTqReqHelpingRemovePriority_Node;

static T_remark ScoreTqReqHelpingRemovePriority_Remark = {
  .next = NULL,
  .remark = "ScoreTqReqHelpingRemovePriority"
};

void ScoreTqReqHelpingRemovePriority_Run( TQContext *tq_ctx, bool home_only )
{
  ScoreTqReqHelpingRemovePriority_Context *ctx;

  ctx = &ScoreTqReqHelpingRemovePriority_Instance;
  ctx->tq_ctx = tq_ctx;
  ctx->home_only = home_only;

  ctx = T_push_fixture(
    &ScoreTqReqHelpingRemovePriority_Node,
    &ScoreTqReqHelpingRemovePriority_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;
  ScoreTqReqHelpingRemovePriority_ResetRepresentatives( ctx );

  for (
    ctx->Map.pcs[ 0 ] = ScoreTqReqHelpingRemovePriority_Pre_RemovedQueue_Alone;
    ctx->Map.pcs[ 0 ] < ScoreTqReqHelpingRemovePriority_Pre_RemovedQueue_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = ScoreTqReqHelpingRemovePriority_Pre_OtherQueue_None;
      ctx->Map.pcs[ 1 ] < ScoreTqReqHelpingRemovePriority_Pre_OtherQueue_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = ScoreTqReqHelpingRemovePriority_PopEntry( ctx );

      for (
        ctx->Map.rep = 0;
        ctx->Map.rep <
        ScoreTqReqHelpingRemovePriority_GetRepresentativeCount( ctx );
        ++ctx->Map.rep
      ) {
        ctx->Map.skip_rep = false;
        ScoreTqReqHelpingRemovePriority_TestVariant( ctx );

        if ( ctx->Map.skip ) {
          break;
        }

        ScoreTqReqHelpingRemovePriority_MarkRepresentatives( ctx );
      }
    }
  }

  ScoreTqReqHelpingRemovePriority_CheckRepresentatives( ctx );

  T_add_remark( &ScoreTqReqHelpingRemovePriority_Remark );
  T_pop_fixture();
}

/** @} */
