/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqSurrenderPriority
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

#include "tr-tq-surrender-priority.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreTqReqSurrenderPriority spec:/score/tq/req/surrender-priority
 *
 * @ingroup TestsuitesValidationNoClock1
 * @ingroup TestsuitesValidationNoClock2
 *
 * @{
 */

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_FirstQueueThreads_NA : 1;
  uint16_t Pre_FirstThreadQueues_NA : 1;
  uint16_t Pre_QueueCount_NA : 1;
  uint16_t Post_Dequeue : 1;
  uint16_t Post_Extract : 1;
  uint16_t Post_EmptyQueue : 1;
  uint16_t Post_QueueOrder : 2;
} ScoreTqReqSurrenderPriority_Entry;

typedef enum {
  ROLE_FIRST,
  ROLE_FIRST_NEXT,
  ROLE_SECOND,
  ROLE_THIRD,
  ROLE_COUNT
} Role;

/**
 * @brief Test context for spec:/score/tq/req/surrender-priority test case.
 */
typedef struct {
  /**
   * @brief This member is true, if the first priority queue contains at least
   *   two threads.
   */
  bool first_more;

  /**
   * @brief This member is true, if the first thread of the first priority
   *   queue has a helping scheduler.
   */
  bool helping;

  /**
   * @brief This member is true, if the priority queue associated with the
   *   helping scheduler of the first thread contains no other thread.
   */
  bool alone;

  /**
   * @brief This member specifies the count of priority queues.
   */
  size_t queue_count;

  /**
   * @brief This member is true, if the first thread of the first priority
   *   queue returned from the enqueue.
   */
  bool first_done;

  /**
   * @brief This member references the thread queue.
   */
  Thread_queue_Queue *queue;

  /**
   * @brief This member specifies for each role, if its thread waits on the
   *   thread queue after the surrender.
   */
  bool waits[ ROLE_COUNT ];

  /**
   * @brief This member contains the priority queues before the surrender.
   */
  TQSnapshot before;

  /**
   * @brief This member contains the priority queues after the surrender.
   */
  TQSnapshot after;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTqReqSurrenderPriority_Run() parameter.
   */
  TQContext *tq_ctx;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTqReqSurrenderPriority_Run() parameter.
   */
  bool home_only;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 3 ];

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
    ScoreTqReqSurrenderPriority_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreTqReqSurrenderPriority_Context;

static ScoreTqReqSurrenderPriority_Context
  ScoreTqReqSurrenderPriority_Instance;

static const char *const
  ScoreTqReqSurrenderPriority_PreDesc_FirstQueueThreads[] =
    { "One", "More", "NA" };

static const char *const
  ScoreTqReqSurrenderPriority_PreDesc_FirstThreadQueues[] =
    { "Only", "Shared", "Alone", "NA" };

static const char *const ScoreTqReqSurrenderPriority_PreDesc_QueueCount[] =
  { "One", "Two", "More", "NA" };

static const char *const *const ScoreTqReqSurrenderPriority_PreDesc[] = {
  ScoreTqReqSurrenderPriority_PreDesc_FirstQueueThreads,
  ScoreTqReqSurrenderPriority_PreDesc_FirstThreadQueues,
  ScoreTqReqSurrenderPriority_PreDesc_QueueCount,
  NULL
};

typedef ScoreTqReqSurrenderPriority_Context Context;

static const TQWorkerKind role_worker[ ROLE_COUNT ] =
  { TQ_BLOCKER_C, TQ_BLOCKER_B, TQ_WORKER_F, TQ_HELPER_C };

static bool HasProcessors( uint32_t count )
{
  return rtems_scheduler_get_processor_maximum() >= count;
}

static bool HasThirdScheduler( const Context *ctx );

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

static bool HasRole( const Context *ctx, Role role )
{
  switch ( role ) {
    case ROLE_FIRST:
      return true;
    case ROLE_FIRST_NEXT:
      return ctx->first_more;
    case ROLE_SECOND:
      return ctx->queue_count >= 2 && !ctx->alone;
    default:
      return ctx->queue_count >= 3;
  }
}

static rtems_id GetThirdScheduler( const Context *ctx )
{
  return IsSticky( ctx ) ? SCHEDULER_E_ID : SCHEDULER_C_ID;
}

/*
 * The test suite configuration may provide less schedulers than processors.
 */
static bool HasThirdScheduler( const Context *ctx )
{
  rtems_status_code sc;
  cpu_set_t         set;

  CPU_ZERO( &set );
  sc = rtems_scheduler_get_processor_set(
    GetThirdScheduler( ctx ),
    sizeof( set ),
    &set
  );

  return sc == RTEMS_SUCCESSFUL && CPU_COUNT( &set ) > 0;
}

#if defined( RTEMS_SMP )
/*
 * A thread which waits in a sticky enqueue occupies a processor of its home
 * scheduler.  The runner owns the thread queue and uses the only processor of
 * scheduler A.
 */
static void Setup( Context *ctx )
{
  rtems_id first;

  if ( IsSticky( ctx ) && !HasProcessors( 4 ) ) {
    return;
  }

  if ( IsSticky( ctx ) ) {
    first = SCHEDULER_C_ID;
  } else {
    first = SCHEDULER_A_ID;
  }

  TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_C, first, PRIO_HIGH );
  TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_B, first, PRIO_NORMAL );

  if ( HasProcessors( 2 ) ) {
    TQSetScheduler( ctx->tq_ctx, TQ_WORKER_F, SCHEDULER_B_ID, PRIO_LOW );
    TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_E, SCHEDULER_B_ID, PRIO_HIGH );
  }

  if ( HasThirdScheduler( ctx ) ) {
    TQSetScheduler(
      ctx->tq_ctx,
      TQ_HELPER_C,
      GetThirdScheduler( ctx ),
      PRIO_LOW
    );
  }
}
#endif

static const rtems_tcb *GetFirst( const Context *ctx )
{
  return ctx->tq_ctx->worker_tcb[ role_worker[ ROLE_FIRST ] ];
}

/*
 * The expected priority queues after the surrender are the priority queues
 * before the surrender which keep a thread after the removal of the dequeued
 * thread.  If rotate is true, the first priority queue goes to the end.
 */
static size_t GetExpected(
  const Context            *ctx,
  bool                      rotate,
  const Scheduler_Control **expected
)
{
  const TQSnapshot *before;
  size_t            count;
  size_t            i;

  before = &ctx->before;
  count = 0;

  for ( i = 0; i < before->count; ++i ) {
    size_t                         index;
    const TQPriorityQueueSnapshot *queue;

    if ( rotate ) {
      index = ( i + 1 ) % before->count;
    } else {
      index = i;
    }

    queue = &before->queue[ index ];

    if (
      queue->count - ( TQSnapshotContains( queue, GetFirst( ctx ) ) ? 1 : 0 ) >
      0
    ) {
      expected[ count ] = queue->scheduler;
      ++count;
    }
  }

  return count;
}

static void CheckOrder( const Context *ctx, bool rotate )
{
  const Scheduler_Control *expected[ RTEMS_ARRAY_SIZE( ctx->before.queue ) ];
  size_t                   count;
  size_t                   i;

  count = GetExpected( ctx, rotate, expected );
  T_eq_sz( ctx->after.count, count );

  for ( i = 0; i < count && i < ctx->after.count; ++i ) {
    T_eq_ptr( ctx->after.queue[ i ].scheduler, expected[ i ] );
  }
}

static void CheckDequeue( const Context *ctx )
{
  size_t i;

  T_true( ctx->first_done );

  for ( i = ROLE_FIRST_NEXT; i < ROLE_COUNT; ++i ) {
    if ( HasRole( ctx, (Role) i ) ) {
      T_true( ctx->waits[ i ] );
    }
  }
}

static void CheckExtract( const Context *ctx )
{
  size_t i;

  for ( i = 0; i < ctx->after.count; ++i ) {
    T_false( TQSnapshotContains( &ctx->after.queue[ i ], GetFirst( ctx ) ) );
  }
}

static void CheckEmptyQueues( const Context *ctx )
{
  size_t i;

  for ( i = 0; i < ctx->after.count; ++i ) {
    T_gt_sz( ctx->after.queue[ i ].count, 0 );
  }
}

static void ScoreTqReqSurrenderPriority_Pre_FirstQueueThreads_Prepare(
  ScoreTqReqSurrenderPriority_Context              *ctx,
  ScoreTqReqSurrenderPriority_Pre_FirstQueueThreads state
)
{
  switch ( state ) {
    case ScoreTqReqSurrenderPriority_Pre_FirstQueueThreads_One: {
      /*
       * While the first priority queue of the thread queue contains exactly
       * one thread.
       */
      ctx->first_more = false;

      if ( IsSticky( ctx ) && !HasProcessors( 4 ) ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqSurrenderPriority_Pre_FirstQueueThreads_More: {
      /*
       * While the first priority queue of the thread queue contains at least
       * two threads.
       */
      ctx->first_more = true;

      if ( IsSticky( ctx ) && !HasProcessors( 4 ) ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqSurrenderPriority_Pre_FirstQueueThreads_NA:
      break;
  }
}

static void ScoreTqReqSurrenderPriority_Pre_FirstThreadQueues_Prepare(
  ScoreTqReqSurrenderPriority_Context              *ctx,
  ScoreTqReqSurrenderPriority_Pre_FirstThreadQueues state
)
{
  switch ( state ) {
    case ScoreTqReqSurrenderPriority_Pre_FirstThreadQueues_Only: {
      /*
       * While the first thread of the first priority queue of the thread queue
       * is contained in no other priority queue of the thread queue.
       */
      ctx->helping = false;
      ctx->alone = false;
      break;
    }

    case ScoreTqReqSurrenderPriority_Pre_FirstThreadQueues_Shared: {
      /*
       * While the first thread of the first priority queue of the thread queue
       * is contained in another priority queue of the thread queue, while this
       * priority queue contains at least one other thread.
       */
      ctx->helping = true;
      ctx->alone = false;

      if ( !HasProcessors( 2 ) ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqSurrenderPriority_Pre_FirstThreadQueues_Alone: {
      /*
       * While the first thread of the first priority queue of the thread queue
       * is contained in another priority queue of the thread queue, while this
       * priority queue contains no other thread.
       */
      ctx->helping = true;
      ctx->alone = true;

      if ( !HasProcessors( 2 ) ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqSurrenderPriority_Pre_FirstThreadQueues_NA:
      break;
  }
}

static void ScoreTqReqSurrenderPriority_Pre_QueueCount_Prepare(
  ScoreTqReqSurrenderPriority_Context       *ctx,
  ScoreTqReqSurrenderPriority_Pre_QueueCount state
)
{
  switch ( state ) {
    case ScoreTqReqSurrenderPriority_Pre_QueueCount_One: {
      /*
       * While the thread queue has exactly one priority queue.
       */
      ctx->queue_count = 1;
      break;
    }

    case ScoreTqReqSurrenderPriority_Pre_QueueCount_Two: {
      /*
       * While the thread queue has exactly two priority queues.
       */
      ctx->queue_count = 2;

      if ( !HasProcessors( 2 ) || ( ctx->home_only && !ctx->alone ) ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqSurrenderPriority_Pre_QueueCount_More: {
      /*
       * While the thread queue has at least three priority queues.
       */
      ctx->queue_count = 3;

      if ( !HasThirdScheduler( ctx ) || ctx->home_only ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqSurrenderPriority_Pre_QueueCount_NA:
      break;
  }
}

static void ScoreTqReqSurrenderPriority_Post_Dequeue_Check(
  ScoreTqReqSurrenderPriority_Context     *ctx,
  ScoreTqReqSurrenderPriority_Post_Dequeue state
)
{
  switch ( state ) {
    case ScoreTqReqSurrenderPriority_Post_Dequeue_First: {
      /*
       * The surrender operation shall dequeue the first thread of the first
       * priority queue of the thread queue.
       */
      CheckDequeue( ctx );
      break;
    }

    case ScoreTqReqSurrenderPriority_Post_Dequeue_NA:
      break;
  }
}

static void ScoreTqReqSurrenderPriority_Post_Extract_Check(
  ScoreTqReqSurrenderPriority_Context     *ctx,
  ScoreTqReqSurrenderPriority_Post_Extract state
)
{
  switch ( state ) {
    case ScoreTqReqSurrenderPriority_Post_Extract_All: {
      /*
       * The surrender operation shall remove the dequeued thread from each
       * priority queue of the thread queue.
       */
      CheckExtract( ctx );
      break;
    }

    case ScoreTqReqSurrenderPriority_Post_Extract_NA:
      break;
  }
}

static void ScoreTqReqSurrenderPriority_Post_EmptyQueue_Check(
  ScoreTqReqSurrenderPriority_Context        *ctx,
  ScoreTqReqSurrenderPriority_Post_EmptyQueue state
)
{
  switch ( state ) {
    case ScoreTqReqSurrenderPriority_Post_EmptyQueue_Removed: {
      /*
       * The surrender operation shall remove each priority queue which
       * contains no thread after the removal of the dequeued thread from the
       * thread queue.
       */
      CheckEmptyQueues( ctx );
      break;
    }

    case ScoreTqReqSurrenderPriority_Post_EmptyQueue_NA:
      break;
  }
}

static void ScoreTqReqSurrenderPriority_Post_QueueOrder_Check(
  ScoreTqReqSurrenderPriority_Context        *ctx,
  ScoreTqReqSurrenderPriority_Post_QueueOrder state
)
{
  switch ( state ) {
    case ScoreTqReqSurrenderPriority_Post_QueueOrder_Rotated: {
      /*
       * The priority queue which was the first priority queue of the thread
       * queue before the surrender shall be positioned after each other
       * priority queue of the thread queue.
       */
      CheckOrder( ctx, true );
      break;
    }

    case ScoreTqReqSurrenderPriority_Post_QueueOrder_Kept: {
      /*
       * The surrender operation shall not change the order of the priority
       * queues of the thread queue.
       */
      CheckOrder( ctx, false );
      break;
    }

    case ScoreTqReqSurrenderPriority_Post_QueueOrder_NA:
      break;
  }
}

static void ScoreTqReqSurrenderPriority_Setup(
  ScoreTqReqSurrenderPriority_Context *ctx
)
{
  TQReset( ctx->tq_ctx );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_A, PRIO_VERY_HIGH );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_C, PRIO_HIGH );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_B, PRIO_NORMAL );

  #if defined( RTEMS_SMP )
  Setup( ctx );
  #endif
}

static void ScoreTqReqSurrenderPriority_Setup_Wrap( void *arg )
{
  ScoreTqReqSurrenderPriority_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqSurrenderPriority_Setup( ctx );
}

static void ScoreTqReqSurrenderPriority_Teardown(
  ScoreTqReqSurrenderPriority_Context *ctx
)
{
  TQReset( ctx->tq_ctx );
  TQSetScheduler( ctx->tq_ctx, TQ_WORKER_F, SCHEDULER_A_ID, PRIO_LOW );
}

static void ScoreTqReqSurrenderPriority_Teardown_Wrap( void *arg )
{
  ScoreTqReqSurrenderPriority_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqSurrenderPriority_Teardown( ctx );
}

static void ScoreTqReqSurrenderPriority_Prepare(
  ScoreTqReqSurrenderPriority_Context *ctx
)
{
  ctx->first_done = false;
}

static void ScoreTqReqSurrenderPriority_Action(
  ScoreTqReqSurrenderPriority_Context *ctx
)
{
  rtems_event_set events;
  size_t          i;

  SetSelfPriority( PRIO_VERY_LOW );
  ctx->tq_ctx->how_many = 1;

  if ( IsSticky( ctx ) ) {
    TQEnqueuePrepare( ctx->tq_ctx );
  } else {
    TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE_PREPARE );
  }

  if ( ctx->helping ) {
    TQSendAndWaitForExecutionStop(
      ctx->tq_ctx,
      TQ_BLOCKER_C,
      TQ_EVENT_MUTEX_A_OBTAIN
    );
    TQSendAndWaitForExecutionStop(
      ctx->tq_ctx,
      TQ_BLOCKER_E,
      TQ_EVENT_MUTEX_A_OBTAIN | TQ_EVENT_MUTEX_A_RELEASE
    );
  }

  for ( i = 0; i < ROLE_COUNT; ++i ) {
    if ( HasRole( ctx, (Role) i ) ) {
      events = TQ_EVENT_ENQUEUE;

      if ( i != ROLE_FIRST ) {
        events |= TQ_EVENT_SURRENDER;
      }

      TQClearDone( ctx->tq_ctx, role_worker[ i ] );
      Enqueue( ctx, role_worker[ i ], events );
      ++ctx->tq_ctx->how_many;
    }
  }

  ctx->queue = GetFirst( ctx )->Wait.queue;
  T_assert_not_null( ctx->queue );
  TQTakeSnapshot( &ctx->before, ctx->queue );

  if ( IsSticky( ctx ) ) {
    TQEnqueueDone( ctx->tq_ctx );
  } else {
    TQClearDone( ctx->tq_ctx, TQ_BLOCKER_A );
    TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE_DONE );
    TQWaitForDone( ctx->tq_ctx, TQ_BLOCKER_A );
  }

  TQWaitForDone( ctx->tq_ctx, TQ_BLOCKER_C );
  ctx->first_done = true;
  TQTakeSnapshot( &ctx->after, ctx->queue );

  for ( i = 0; i < ROLE_COUNT; ++i ) {
    ctx->waits[ i ] = ctx->tq_ctx->worker_tcb[ role_worker[ i ] ]
                        ->Wait.queue == ctx->queue;
  }

  events = TQ_EVENT_SURRENDER;

  if ( ctx->helping ) {
    events |= TQ_EVENT_MUTEX_A_RELEASE;
  }

  TQClearDone( ctx->tq_ctx, TQ_BLOCKER_C );
  TQSend( ctx->tq_ctx, TQ_BLOCKER_C, events );
  TQWaitForDone( ctx->tq_ctx, TQ_BLOCKER_C );

  for ( i = 0; i < ROLE_COUNT; ++i ) {
    if ( HasRole( ctx, (Role) i ) ) {
      TQWaitForDone( ctx->tq_ctx, role_worker[ i ] );
      TQWaitForExecutionStop( ctx->tq_ctx, role_worker[ i ] );
    }
  }

  if ( ctx->helping ) {
    TQMutexObtain( ctx->tq_ctx, TQ_MUTEX_A );
    TQMutexRelease( ctx->tq_ctx, TQ_MUTEX_A );
  }

  SetSelfPriority( PRIO_NORMAL );
}

/* clang-format off */

static const ScoreTqReqSurrenderPriority_Entry
ScoreTqReqSurrenderPriority_Entries[] = {
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, ScoreTqReqSurrenderPriority_Post_Dequeue_First,
    ScoreTqReqSurrenderPriority_Post_Extract_All,
    ScoreTqReqSurrenderPriority_Post_EmptyQueue_Removed,
    ScoreTqReqSurrenderPriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, ScoreTqReqSurrenderPriority_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriority_Post_Extract_NA,
    ScoreTqReqSurrenderPriority_Post_EmptyQueue_NA,
    ScoreTqReqSurrenderPriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, ScoreTqReqSurrenderPriority_Post_Dequeue_First,
    ScoreTqReqSurrenderPriority_Post_Extract_All,
    ScoreTqReqSurrenderPriority_Post_EmptyQueue_Removed,
    ScoreTqReqSurrenderPriority_Post_QueueOrder_Rotated },
#else
  { 1, 0, 0, 0, ScoreTqReqSurrenderPriority_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriority_Post_Extract_NA,
    ScoreTqReqSurrenderPriority_Post_EmptyQueue_NA,
    ScoreTqReqSurrenderPriority_Post_QueueOrder_NA },
#endif
  { 1, 0, 0, 0, ScoreTqReqSurrenderPriority_Post_Dequeue_NA,
    ScoreTqReqSurrenderPriority_Post_Extract_NA,
    ScoreTqReqSurrenderPriority_Post_EmptyQueue_NA,
    ScoreTqReqSurrenderPriority_Post_QueueOrder_NA },
  { 0, 0, 0, 0, ScoreTqReqSurrenderPriority_Post_Dequeue_First,
    ScoreTqReqSurrenderPriority_Post_Extract_All,
    ScoreTqReqSurrenderPriority_Post_EmptyQueue_Removed,
    ScoreTqReqSurrenderPriority_Post_QueueOrder_Kept },
  { 0, 0, 0, 0, ScoreTqReqSurrenderPriority_Post_Dequeue_First,
    ScoreTqReqSurrenderPriority_Post_Extract_All,
    ScoreTqReqSurrenderPriority_Post_EmptyQueue_Removed,
    ScoreTqReqSurrenderPriority_Post_QueueOrder_Rotated }
};

static const uint8_t
ScoreTqReqSurrenderPriority_Map[] = {
  3, 0, 0, 2, 0, 0, 2, 0, 0, 4, 1, 1, 2, 1, 1, 2, 1, 1
};

/* clang-format on */

static size_t ScoreTqReqSurrenderPriority_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  ScoreTqReqSurrenderPriority_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      ScoreTqReqSurrenderPriority_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture ScoreTqReqSurrenderPriority_Fixture = {
  .setup = ScoreTqReqSurrenderPriority_Setup_Wrap,
  .stop = NULL,
  .teardown = ScoreTqReqSurrenderPriority_Teardown_Wrap,
  .scope = ScoreTqReqSurrenderPriority_Scope,
  .initial_context = &ScoreTqReqSurrenderPriority_Instance
};

static const uint8_t ScoreTqReqSurrenderPriority_Weights[] = { 9, 3, 1 };

static void ScoreTqReqSurrenderPriority_Skip(
  ScoreTqReqSurrenderPriority_Context *ctx,
  size_t                               index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pcs[ 1 ] =
        ScoreTqReqSurrenderPriority_Pre_FirstThreadQueues_NA - 1;
      /* Fall through */
    case 2:
      ctx->Map.pcs[ 2 ] = ScoreTqReqSurrenderPriority_Pre_QueueCount_NA - 1;
      break;
  }
}

static inline ScoreTqReqSurrenderPriority_Entry
ScoreTqReqSurrenderPriority_PopEntry(
  ScoreTqReqSurrenderPriority_Context *ctx
)
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 3; ++i ) {
      index += ScoreTqReqSurrenderPriority_Weights[ i ] * ctx->Map.pcs[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return ScoreTqReqSurrenderPriority_Entries
    [ ScoreTqReqSurrenderPriority_Map[ index ] ];
}

static void ScoreTqReqSurrenderPriority_TestVariant(
  ScoreTqReqSurrenderPriority_Context *ctx
)
{
  ScoreTqReqSurrenderPriority_Pre_FirstQueueThreads_Prepare(
    ctx,
    ctx->Map.pcs[ 0 ]
  );

  if ( ctx->Map.skip ) {
    ScoreTqReqSurrenderPriority_Skip( ctx, 0 );
    return;
  }

  ScoreTqReqSurrenderPriority_Pre_FirstThreadQueues_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );

  if ( ctx->Map.skip ) {
    ScoreTqReqSurrenderPriority_Skip( ctx, 1 );
    return;
  }

  ScoreTqReqSurrenderPriority_Pre_QueueCount_Prepare( ctx, ctx->Map.pcs[ 2 ] );

  if ( ctx->Map.skip ) {
    ScoreTqReqSurrenderPriority_Skip( ctx, 2 );
    return;
  }

  ScoreTqReqSurrenderPriority_Action( ctx );
  ScoreTqReqSurrenderPriority_Post_Dequeue_Check(
    ctx,
    ctx->Map.entry.Post_Dequeue
  );
  ScoreTqReqSurrenderPriority_Post_Extract_Check(
    ctx,
    ctx->Map.entry.Post_Extract
  );
  ScoreTqReqSurrenderPriority_Post_EmptyQueue_Check(
    ctx,
    ctx->Map.entry.Post_EmptyQueue
  );
  ScoreTqReqSurrenderPriority_Post_QueueOrder_Check(
    ctx,
    ctx->Map.entry.Post_QueueOrder
  );
}

static T_fixture_node ScoreTqReqSurrenderPriority_Node;

static T_remark ScoreTqReqSurrenderPriority_Remark = {
  .next = NULL,
  .remark = "ScoreTqReqSurrenderPriority"
};

void ScoreTqReqSurrenderPriority_Run( TQContext *tq_ctx, bool home_only )
{
  ScoreTqReqSurrenderPriority_Context *ctx;

  ctx = &ScoreTqReqSurrenderPriority_Instance;
  ctx->tq_ctx = tq_ctx;
  ctx->home_only = home_only;

  ctx = T_push_fixture(
    &ScoreTqReqSurrenderPriority_Node,
    &ScoreTqReqSurrenderPriority_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;

  for (
    ctx->Map.pcs[ 0 ] = ScoreTqReqSurrenderPriority_Pre_FirstQueueThreads_One;
    ctx->Map.pcs[ 0 ] < ScoreTqReqSurrenderPriority_Pre_FirstQueueThreads_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] =
        ScoreTqReqSurrenderPriority_Pre_FirstThreadQueues_Only;
      ctx->Map.pcs[ 1 ] < ScoreTqReqSurrenderPriority_Pre_FirstThreadQueues_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = ScoreTqReqSurrenderPriority_Pre_QueueCount_One;
        ctx->Map.pcs[ 2 ] < ScoreTqReqSurrenderPriority_Pre_QueueCount_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = ScoreTqReqSurrenderPriority_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        ScoreTqReqSurrenderPriority_Prepare( ctx );
        ScoreTqReqSurrenderPriority_TestVariant( ctx );
      }
    }
  }

  T_add_remark( &ScoreTqReqSurrenderPriority_Remark );
  T_pop_fixture();
}

/** @} */
