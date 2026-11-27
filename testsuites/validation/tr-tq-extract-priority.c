/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqExtractPriority
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

#include "tr-tq-extract-priority.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreTqReqExtractPriority spec:/score/tq/req/extract-priority
 *
 * @ingroup TestsuitesValidationNoClock1
 * @ingroup TestsuitesValidationNoClock2
 *
 * @{
 */

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_TargetQueueThreads_NA : 1;
  uint8_t Pre_TargetQueues_NA : 1;
  uint8_t Pre_OtherQueue_NA : 1;
  uint8_t Post_Extract : 1;
  uint8_t Post_EmptyQueue : 1;
  uint8_t Post_QueueOrder : 1;
} ScoreTqReqExtractPriority_Entry;

typedef enum {
  ROLE_TARGET,
  ROLE_NEIGHBOR,
  ROLE_SHARER,
  ROLE_OTHER,
  ROLE_COUNT
} Role;

typedef enum { OTHER_NONE, OTHER_BEFORE, OTHER_BETWEEN, OTHER_AFTER } Other;

/**
 * @brief Test context for spec:/score/tq/req/extract-priority test case.
 */
typedef struct {
  /**
   * @brief This member is true, if the priority queue of the home scheduler of
   *   the extracted thread contains another thread.
   */
  bool neighbor;

  /**
   * @brief This member is true, if the extracted thread has a helping
   *   scheduler.
   */
  bool helping;

  /**
   * @brief This member is true, if the priority queue associated with the
   *   helping scheduler of the extracted thread contains no other thread.
   */
  bool alone;

  /**
   * @brief This member specifies the position of the priority queue which does
   *   not contain the extracted thread.
   */
  Other other;

  /**
   * @brief This member is true, if the extracted thread returned from the
   *   enqueue.
   */
  bool target_done;

  /**
   * @brief This member references the thread queue.
   */
  Thread_queue_Queue *queue;

  /**
   * @brief This member specifies for each role, if its thread waits on the
   *   thread queue after the extract.
   */
  bool waits[ ROLE_COUNT ];

  /**
   * @brief This member contains the priority queues before the extract.
   */
  TQSnapshot before;

  /**
   * @brief This member contains the priority queues after the extract.
   */
  TQSnapshot after;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTqReqExtractPriority_Run() parameter.
   */
  TQContext *tq_ctx;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTqReqExtractPriority_Run() parameter.
   */
  bool home_only;

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
    ScoreTqReqExtractPriority_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreTqReqExtractPriority_Context;

static ScoreTqReqExtractPriority_Context ScoreTqReqExtractPriority_Instance;

static const char *const
  ScoreTqReqExtractPriority_PreDesc_TargetQueueThreads[] =
    { "One", "More", "NA" };

static const char *const ScoreTqReqExtractPriority_PreDesc_TargetQueues[] =
  { "Only", "Shared", "Alone", "NA" };

static const char *const ScoreTqReqExtractPriority_PreDesc_OtherQueue[] =
  { "None", "PresentBefore", "PresentBetween", "PresentAfter", "NA" };

static const char *const *const ScoreTqReqExtractPriority_PreDesc[] = {
  ScoreTqReqExtractPriority_PreDesc_TargetQueueThreads,
  ScoreTqReqExtractPriority_PreDesc_TargetQueues,
  ScoreTqReqExtractPriority_PreDesc_OtherQueue,
  NULL
};

typedef enum {
  ScoreTqReqExtractPriority_Pre_OtherQueue_Present_Before,
  ScoreTqReqExtractPriority_Pre_OtherQueue_Present_Between,
  ScoreTqReqExtractPriority_Pre_OtherQueue_Present_After
} ScoreTqReqExtractPriority_Pre_OtherQueue_Present_Rep;

static const uint8_t
  ScoreTqReqExtractPriority_PreRepOffset_TargetQueueThreads[] = { 0, 1, 2, 3 };

static const uint8_t ScoreTqReqExtractPriority_PreRepOffset_TargetQueues[] =
  { 0, 1, 2, 3, 4 };

static const uint8_t ScoreTqReqExtractPriority_PreRepOffset_OtherQueue[] =
  { 0, 1, 4, 5 };

static const uint8_t *const ScoreTqReqExtractPriority_PreRepOffset[] = {
  ScoreTqReqExtractPriority_PreRepOffset_TargetQueueThreads,
  ScoreTqReqExtractPriority_PreRepOffset_TargetQueues,
  ScoreTqReqExtractPriority_PreRepOffset_OtherQueue
};

static size_t ScoreTqReqExtractPriority_GetRepresentativeCountOf(
  const ScoreTqReqExtractPriority_Context *ctx,
  size_t                                   index
)
{
  const uint8_t *offsets;
  size_t         state;

  offsets = ScoreTqReqExtractPriority_PreRepOffset[ index ];
  state = ctx->Map.pcs[ index ];

  return offsets[ state + 1 ] - offsets[ state ];
}

static size_t ScoreTqReqExtractPriority_GetRepresentative(
  const ScoreTqReqExtractPriority_Context *ctx,
  size_t                                   index
)
{
  size_t count;

  count = ScoreTqReqExtractPriority_GetRepresentativeCountOf( ctx, index );

  if ( ctx->Map.rep < count ) {
    return ctx->Map.rep;
  }

  return count - 1;
}

static size_t ScoreTqReqExtractPriority_GetRepresentativeCount(
  const ScoreTqReqExtractPriority_Context *ctx
)
{
  size_t count;
  size_t i;

  count = 1;

  for ( i = 0; i < 3; ++i ) {
    size_t count_of_condition;

    count_of_condition = ScoreTqReqExtractPriority_GetRepresentativeCountOf(
      ctx,
      i
    );

    if ( count_of_condition > count ) {
      count = count_of_condition;
    }
  }

  return count;
}

static ScoreTqReqExtractPriority_Pre_OtherQueue_Present_Rep
ScoreTqReqExtractPriority_Pre_OtherQueue_Present_GetRepresentative(
  const ScoreTqReqExtractPriority_Context *ctx
)
{
  return (
    ScoreTqReqExtractPriority_Pre_OtherQueue_Present_Rep
  ) ScoreTqReqExtractPriority_GetRepresentative( ctx, 2 );
}

static const uint8_t ScoreTqReqExtractPriority_PreRepBase[] = { 0, 3, 7 };

static const uint8_t ScoreTqReqExtractPriority_PreRepStates[] = { 2, 3, 2 };

static void ScoreTqReqExtractPriority_ResetRepresentatives(
  ScoreTqReqExtractPriority_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 12; ++i ) {
    ctx->Map.rep_ran[ i ] = false;
  }
}

static void ScoreTqReqExtractPriority_MarkRepresentatives(
  ScoreTqReqExtractPriority_Context *ctx
)
{
  size_t i;

  if ( ctx->Map.skip_rep ) {
    return;
  }

  for ( i = 0; i < 3; ++i ) {
    size_t rep;

    rep = ScoreTqReqExtractPriority_PreRepBase[ i ] +
          ScoreTqReqExtractPriority_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
          ScoreTqReqExtractPriority_GetRepresentative( ctx, i );
    ctx->Map.rep_ran[ rep ] = true;
  }
}

static void ScoreTqReqExtractPriority_CheckRepresentatives(
  const ScoreTqReqExtractPriority_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 3; ++i ) {
    const uint8_t *offsets;
    const bool    *ran;
    size_t         state;

    offsets = ScoreTqReqExtractPriority_PreRepOffset[ i ];
    ran = &ctx->Map.rep_ran[ ScoreTqReqExtractPriority_PreRepBase[ i ] ];

    for (
      state = 0; state < ScoreTqReqExtractPriority_PreRepStates[ i ]; ++state
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
            ScoreTqReqExtractPriority_PreDesc[ i ][ k ]
          );
        }
      }
    }
  }
}

typedef ScoreTqReqExtractPriority_Context Context;

static const TQWorkerKind role_worker[ ROLE_COUNT ] =
  { TQ_BLOCKER_C, TQ_BLOCKER_B, TQ_WORKER_F, TQ_HELPER_C };

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

static bool HasRole( const Context *ctx, Role role )
{
  switch ( role ) {
    case ROLE_TARGET:
      return true;
    case ROLE_NEIGHBOR:
      return ctx->neighbor;
    case ROLE_SHARER:
      return ctx->helping && !ctx->alone;
    default:
      return ctx->other != OTHER_NONE;
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

static const rtems_tcb *GetTarget( const Context *ctx )
{
  return ctx->tq_ctx->worker_tcb[ role_worker[ ROLE_TARGET ] ];
}

/*
 * The expected priority queues after the extract are the priority queues
 * before the extract which keep a thread after the removal of the target
 * thread.
 */
static void CheckOrder( const Context *ctx )
{
  const TQSnapshot *before;
  size_t            count;
  size_t            i;

  before = &ctx->before;
  count = 0;

  for ( i = 0; i < before->count; ++i ) {
    const TQPriorityQueueSnapshot *queue;

    queue = &before->queue[ i ];

    if (
      queue->count -
        ( TQSnapshotContains( queue, GetTarget( ctx ) ) ? 1 : 0 ) >
      0
    ) {
      T_lt_sz( count, ctx->after.count );

      if ( count < ctx->after.count ) {
        T_eq_ptr( ctx->after.queue[ count ].scheduler, queue->scheduler );
      }

      ++count;
    }
  }

  T_eq_sz( ctx->after.count, count );
}

static void CheckExtract( const Context *ctx )
{
  size_t i;

  T_true( ctx->target_done );

  for ( i = 0; i < ctx->after.count; ++i ) {
    T_false( TQSnapshotContains( &ctx->after.queue[ i ], GetTarget( ctx ) ) );
  }

  for ( i = ROLE_NEIGHBOR; i < ROLE_COUNT; ++i ) {
    if ( HasRole( ctx, (Role) i ) ) {
      T_true( ctx->waits[ i ] );
    }
  }
}

static void CheckEmptyQueues( const Context *ctx )
{
  size_t i;

  for ( i = 0; i < ctx->after.count; ++i ) {
    T_gt_sz( ctx->after.queue[ i ].count, 0 );
  }
}

static void ScoreTqReqExtractPriority_Pre_TargetQueueThreads_Prepare(
  ScoreTqReqExtractPriority_Context               *ctx,
  ScoreTqReqExtractPriority_Pre_TargetQueueThreads state
)
{
  switch ( state ) {
    case ScoreTqReqExtractPriority_Pre_TargetQueueThreads_One: {
      /*
       * While the priority queue associated with the home scheduler of the
       * extracted thread contains no other thread.
       */
      ctx->neighbor = false;

      if ( IsSticky( ctx ) && !HasProcessors( 4 ) ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqExtractPriority_Pre_TargetQueueThreads_More: {
      /*
       * While the priority queue associated with the home scheduler of the
       * extracted thread contains at least one other thread.
       */
      ctx->neighbor = true;

      if ( IsSticky( ctx ) && !HasProcessors( 4 ) ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqExtractPriority_Pre_TargetQueueThreads_NA:
      break;
  }
}

static void ScoreTqReqExtractPriority_Pre_TargetQueues_Prepare(
  ScoreTqReqExtractPriority_Context         *ctx,
  ScoreTqReqExtractPriority_Pre_TargetQueues state
)
{
  switch ( state ) {
    case ScoreTqReqExtractPriority_Pre_TargetQueues_Only: {
      /*
       * While the extracted thread is contained in no other priority queue of
       * the thread queue.
       */
      ctx->helping = false;
      ctx->alone = false;
      break;
    }

    case ScoreTqReqExtractPriority_Pre_TargetQueues_Shared: {
      /*
       * While the extracted thread is contained in another priority queue of
       * the thread queue, while this priority queue contains at least one
       * other thread.
       */
      ctx->helping = true;
      ctx->alone = false;

      if ( !HasProcessors( 2 ) || ctx->home_only ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqExtractPriority_Pre_TargetQueues_Alone: {
      /*
       * While the extracted thread is contained in another priority queue of
       * the thread queue, while this priority queue contains no other thread.
       */
      ctx->helping = true;
      ctx->alone = true;

      if ( !HasProcessors( 2 ) ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqExtractPriority_Pre_TargetQueues_NA:
      break;
  }
}

static void ScoreTqReqExtractPriority_Pre_OtherQueue_Prepare(
  ScoreTqReqExtractPriority_Context       *ctx,
  ScoreTqReqExtractPriority_Pre_OtherQueue state
)
{
  switch ( state ) {
    case ScoreTqReqExtractPriority_Pre_OtherQueue_None: {
      /*
       * While each priority queue of the thread queue contains the extracted
       * thread.
       */
      ctx->other = OTHER_NONE;
      break;
    }

    case ScoreTqReqExtractPriority_Pre_OtherQueue_Present: {
      /*
       * While the thread queue has a priority queue which does not contain the
       * extracted thread.
       */
      switch (
        ScoreTqReqExtractPriority_Pre_OtherQueue_Present_GetRepresentative(
          ctx
        )
      ) {
        case ScoreTqReqExtractPriority_Pre_OtherQueue_Present_Before: {
          ctx->other = OTHER_BEFORE;
          break;
        }

        case ScoreTqReqExtractPriority_Pre_OtherQueue_Present_Between: {
          if ( !ctx->helping || ( ctx->alone && !ctx->neighbor ) ) {
            ctx->Map.skip_rep = true;
          } else {
            ctx->other = OTHER_BETWEEN;
          }
          break;
        }

        case ScoreTqReqExtractPriority_Pre_OtherQueue_Present_After: {
          ctx->other = OTHER_AFTER;
          break;
        }
      }

      if ( !HasThirdScheduler( ctx ) || ctx->home_only ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqExtractPriority_Pre_OtherQueue_NA:
      break;
  }
}

static void ScoreTqReqExtractPriority_Post_Extract_Check(
  ScoreTqReqExtractPriority_Context     *ctx,
  ScoreTqReqExtractPriority_Post_Extract state
)
{
  switch ( state ) {
    case ScoreTqReqExtractPriority_Post_Extract_All: {
      /*
       * The thread queue extract operation shall remove the extracted thread
       * from each priority queue of the thread queue.
       */
      CheckExtract( ctx );
      break;
    }

    case ScoreTqReqExtractPriority_Post_Extract_NA:
      break;
  }
}

static void ScoreTqReqExtractPriority_Post_EmptyQueue_Check(
  ScoreTqReqExtractPriority_Context        *ctx,
  ScoreTqReqExtractPriority_Post_EmptyQueue state
)
{
  switch ( state ) {
    case ScoreTqReqExtractPriority_Post_EmptyQueue_Removed: {
      /*
       * The thread queue extract operation shall remove each priority queue
       * which contains no thread after the removal of the extracted thread
       * from the thread queue.
       */
      CheckEmptyQueues( ctx );
      break;
    }

    case ScoreTqReqExtractPriority_Post_EmptyQueue_NA:
      break;
  }
}

static void ScoreTqReqExtractPriority_Post_QueueOrder_Check(
  ScoreTqReqExtractPriority_Context        *ctx,
  ScoreTqReqExtractPriority_Post_QueueOrder state
)
{
  switch ( state ) {
    case ScoreTqReqExtractPriority_Post_QueueOrder_Kept: {
      /*
       * The thread queue extract operation shall not change the order of the
       * priority queues of the thread queue.
       */
      CheckOrder( ctx );
      break;
    }

    case ScoreTqReqExtractPriority_Post_QueueOrder_NA:
      break;
  }
}

static void ScoreTqReqExtractPriority_Setup(
  ScoreTqReqExtractPriority_Context *ctx
)
{
  TQReset( ctx->tq_ctx );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_A, PRIO_HIGH );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_C, PRIO_HIGH );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_B, PRIO_NORMAL );

  #if defined( RTEMS_SMP )
  Setup( ctx );
  #endif
}

static void ScoreTqReqExtractPriority_Setup_Wrap( void *arg )
{
  ScoreTqReqExtractPriority_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqExtractPriority_Setup( ctx );
}

static void ScoreTqReqExtractPriority_Teardown(
  ScoreTqReqExtractPriority_Context *ctx
)
{
  TQReset( ctx->tq_ctx );
  TQSetScheduler( ctx->tq_ctx, TQ_WORKER_F, SCHEDULER_A_ID, PRIO_LOW );
}

static void ScoreTqReqExtractPriority_Teardown_Wrap( void *arg )
{
  ScoreTqReqExtractPriority_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqExtractPriority_Teardown( ctx );
}

static void ScoreTqReqExtractPriority_Prepare(
  ScoreTqReqExtractPriority_Context *ctx
)
{
  ctx->target_done = false;
}

static void ScoreTqReqExtractPriority_Action(
  ScoreTqReqExtractPriority_Context *ctx
)
{
  static const Role order[ 4 ][ ROLE_COUNT ] = {
    { ROLE_OTHER, ROLE_TARGET, ROLE_NEIGHBOR, ROLE_SHARER },
    { ROLE_NEIGHBOR, ROLE_OTHER, ROLE_TARGET, ROLE_SHARER },
    { ROLE_SHARER, ROLE_OTHER, ROLE_TARGET, ROLE_NEIGHBOR },
    { ROLE_TARGET, ROLE_NEIGHBOR, ROLE_SHARER, ROLE_OTHER }
  };

  const Role      *roles;
  Per_CPU_Control *cpu_self;
  rtems_event_set  events;
  size_t           i;
  TQWait           wait;

  /*
   * The thread timeout of a thread which waits with a timeout would leave the
   * watchdog of the thread armed.
   */
  wait = ctx->tq_ctx->wait;
  ctx->tq_ctx->wait = TQ_WAIT_FOREVER;
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

  if ( ctx->other == OTHER_BEFORE ) {
    roles = order[ 0 ];
  } else if ( ctx->other == OTHER_BETWEEN && ctx->neighbor ) {
    roles = order[ 1 ];
  } else if ( ctx->other == OTHER_BETWEEN ) {
    roles = order[ 2 ];
  } else {
    roles = order[ 3 ];
  }

  for ( i = 0; i < ROLE_COUNT; ++i ) {
    Role role;

    role = roles[ i ];

    if ( HasRole( ctx, role ) ) {
      events = TQ_EVENT_ENQUEUE;

      if ( role != ROLE_TARGET ) {
        events |= TQ_EVENT_SURRENDER;
      }

      TQClearDone( ctx->tq_ctx, role_worker[ role ] );
      Enqueue( ctx, role_worker[ role ], events );
      ++ctx->tq_ctx->how_many;
    }
  }

  ctx->queue = GetTarget( ctx )->Wait.queue;
  T_assert_not_null( ctx->queue );
  TQTakeSnapshot( &ctx->before, ctx->queue );

  cpu_self = _Thread_Dispatch_disable();
  TQTimeout( ctx->tq_ctx->worker_tcb[ TQ_BLOCKER_C ] );
  _Thread_Dispatch_direct( cpu_self );

  TQWaitForDone( ctx->tq_ctx, TQ_BLOCKER_C );
  ctx->target_done = true;
  TQTakeSnapshot( &ctx->after, ctx->queue );

  for ( i = 0; i < ROLE_COUNT; ++i ) {
    ctx->waits[ i ] = ctx->tq_ctx->worker_tcb[ role_worker[ i ] ]
                        ->Wait.queue == ctx->queue;
  }

  if ( ctx->helping ) {
    TQClearDone( ctx->tq_ctx, TQ_BLOCKER_C );
    TQSend( ctx->tq_ctx, TQ_BLOCKER_C, TQ_EVENT_MUTEX_A_RELEASE );
    TQWaitForDone( ctx->tq_ctx, TQ_BLOCKER_C );
  }

  if ( IsSticky( ctx ) ) {
    TQEnqueueDone( ctx->tq_ctx );
  } else {
    TQClearDone( ctx->tq_ctx, TQ_BLOCKER_A );
    TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE_DONE );
    TQWaitForDone( ctx->tq_ctx, TQ_BLOCKER_A );
  }

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
  ctx->tq_ctx->wait = wait;
}

/* clang-format off */

static const ScoreTqReqExtractPriority_Entry
ScoreTqReqExtractPriority_Entries[] = {
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, ScoreTqReqExtractPriority_Post_Extract_All,
    ScoreTqReqExtractPriority_Post_EmptyQueue_Removed,
    ScoreTqReqExtractPriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, ScoreTqReqExtractPriority_Post_Extract_NA,
    ScoreTqReqExtractPriority_Post_EmptyQueue_NA,
    ScoreTqReqExtractPriority_Post_QueueOrder_NA },
#endif
  { 0, 0, 0, 0, ScoreTqReqExtractPriority_Post_Extract_All,
    ScoreTqReqExtractPriority_Post_EmptyQueue_Removed,
    ScoreTqReqExtractPriority_Post_QueueOrder_Kept }
};

static const uint8_t
ScoreTqReqExtractPriority_Map[] = {
  1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t ScoreTqReqExtractPriority_Scope( void *arg, char *buf, size_t n )
{
  ScoreTqReqExtractPriority_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    size_t pcd[ 3 ];
    size_t i;

    for ( i = 0; i < 3; ++i ) {
      pcd[ i ] = ScoreTqReqExtractPriority_PreRepOffset[ i ]
                                                       [ ctx->Map.pcs[ i ] ] +
                 ScoreTqReqExtractPriority_GetRepresentative( ctx, i );
    }

    return T_get_scope( ScoreTqReqExtractPriority_PreDesc, buf, n, pcd );
  }

  return 0;
}

static T_fixture ScoreTqReqExtractPriority_Fixture = {
  .setup = ScoreTqReqExtractPriority_Setup_Wrap,
  .stop = NULL,
  .teardown = ScoreTqReqExtractPriority_Teardown_Wrap,
  .scope = ScoreTqReqExtractPriority_Scope,
  .initial_context = &ScoreTqReqExtractPriority_Instance
};

static const uint8_t ScoreTqReqExtractPriority_Weights[] = { 6, 2, 1 };

static void ScoreTqReqExtractPriority_Skip(
  ScoreTqReqExtractPriority_Context *ctx,
  size_t                             index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pcs[ 1 ] = ScoreTqReqExtractPriority_Pre_TargetQueues_NA - 1;
      /* Fall through */
    case 2:
      ctx->Map.pcs[ 2 ] = ScoreTqReqExtractPriority_Pre_OtherQueue_NA - 1;
      break;
  }
}

static inline ScoreTqReqExtractPriority_Entry
ScoreTqReqExtractPriority_PopEntry( ScoreTqReqExtractPriority_Context *ctx )
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 3; ++i ) {
      index += ScoreTqReqExtractPriority_Weights[ i ] * ctx->Map.pcs[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return ScoreTqReqExtractPriority_Entries
    [ ScoreTqReqExtractPriority_Map[ index ] ];
}

static void ScoreTqReqExtractPriority_TestVariant(
  ScoreTqReqExtractPriority_Context *ctx
)
{
  ScoreTqReqExtractPriority_Pre_TargetQueueThreads_Prepare(
    ctx,
    ctx->Map.pcs[ 0 ]
  );

  if ( ctx->Map.skip ) {
    ScoreTqReqExtractPriority_Skip( ctx, 0 );
    return;
  }

  ScoreTqReqExtractPriority_Pre_TargetQueues_Prepare( ctx, ctx->Map.pcs[ 1 ] );

  if ( ctx->Map.skip ) {
    ScoreTqReqExtractPriority_Skip( ctx, 1 );
    return;
  }

  ScoreTqReqExtractPriority_Pre_OtherQueue_Prepare( ctx, ctx->Map.pcs[ 2 ] );

  if ( ctx->Map.skip ) {
    ScoreTqReqExtractPriority_Skip( ctx, 2 );
    return;
  }

  if ( ctx->Map.skip_rep ) {
    return;
  }

  ScoreTqReqExtractPriority_Action( ctx );
  ScoreTqReqExtractPriority_Post_Extract_Check(
    ctx,
    ctx->Map.entry.Post_Extract
  );
  ScoreTqReqExtractPriority_Post_EmptyQueue_Check(
    ctx,
    ctx->Map.entry.Post_EmptyQueue
  );
  ScoreTqReqExtractPriority_Post_QueueOrder_Check(
    ctx,
    ctx->Map.entry.Post_QueueOrder
  );
}

static T_fixture_node ScoreTqReqExtractPriority_Node;

static T_remark ScoreTqReqExtractPriority_Remark = {
  .next = NULL,
  .remark = "ScoreTqReqExtractPriority"
};

void ScoreTqReqExtractPriority_Run( TQContext *tq_ctx, bool home_only )
{
  ScoreTqReqExtractPriority_Context *ctx;

  ctx = &ScoreTqReqExtractPriority_Instance;
  ctx->tq_ctx = tq_ctx;
  ctx->home_only = home_only;

  ctx = T_push_fixture(
    &ScoreTqReqExtractPriority_Node,
    &ScoreTqReqExtractPriority_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;
  ScoreTqReqExtractPriority_ResetRepresentatives( ctx );

  for (
    ctx->Map.pcs[ 0 ] = ScoreTqReqExtractPriority_Pre_TargetQueueThreads_One;
    ctx->Map.pcs[ 0 ] < ScoreTqReqExtractPriority_Pre_TargetQueueThreads_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = ScoreTqReqExtractPriority_Pre_TargetQueues_Only;
      ctx->Map.pcs[ 1 ] < ScoreTqReqExtractPriority_Pre_TargetQueues_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = ScoreTqReqExtractPriority_Pre_OtherQueue_None;
        ctx->Map.pcs[ 2 ] < ScoreTqReqExtractPriority_Pre_OtherQueue_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = ScoreTqReqExtractPriority_PopEntry( ctx );

        for (
          ctx->Map.rep = 0;
          ctx->Map.rep <
          ScoreTqReqExtractPriority_GetRepresentativeCount( ctx );
          ++ctx->Map.rep
        ) {
          ctx->Map.skip_rep = false;
          ScoreTqReqExtractPriority_Prepare( ctx );
          ScoreTqReqExtractPriority_TestVariant( ctx );

          if ( ctx->Map.skip ) {
            break;
          }

          ScoreTqReqExtractPriority_MarkRepresentatives( ctx );
        }
      }
    }
  }

  ScoreTqReqExtractPriority_CheckRepresentatives( ctx );

  T_add_remark( &ScoreTqReqExtractPriority_Remark );
  T_pop_fixture();
}

/** @} */
