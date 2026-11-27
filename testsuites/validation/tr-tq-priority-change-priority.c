/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqPriorityChangePriority
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

#include "tr-tq-priority-change-priority.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreTqReqPriorityChangePriority \
 *   spec:/score/tq/req/priority-change-priority
 *
 * @ingroup TestsuitesValidationNoClock2
 *
 * @{
 */

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_TargetPosition_NA : 1;
  uint8_t Pre_NewPriority_NA : 1;
  uint8_t Pre_OtherQueue_NA : 1;
  uint8_t Post_Position : 2;
  uint8_t Post_QueueOrder : 1;
} ScoreTqReqPriorityChangePriority_Entry;

typedef enum { OTHER_NONE, OTHER_BEFORE, OTHER_AFTER } Other;

/**
 * @brief Test context for spec:/score/tq/req/priority-change-priority test
 *   case.
 */
typedef struct {
  /**
   * @brief This member specifies the priority of the thread before the change.
   */
  rtems_task_priority initial_priority;

  /**
   * @brief This member specifies the priority of the thread after the change.
   */
  rtems_task_priority new_priority;

  /**
   * @brief This member is true, if the priority queue contains a last thread
   *   with a priority lower than each new priority.
   */
  bool lower;

  /**
   * @brief This member specifies the position of the priority queue which does
   *   not contain the thread.
   */
  Other other;

  /**
   * @brief This member references the thread queue.
   */
  Thread_queue_Queue *queue;

  /**
   * @brief This member contains the priority queues before the change.
   */
  TQSnapshot before;

  /**
   * @brief This member contains the priority queues after the change.
   */
  TQSnapshot after;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTqReqPriorityChangePriority_Run() parameter.
   */
  TQContext *tq_ctx;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTqReqPriorityChangePriority_Run() parameter.
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
    ScoreTqReqPriorityChangePriority_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreTqReqPriorityChangePriority_Context;

static ScoreTqReqPriorityChangePriority_Context
  ScoreTqReqPriorityChangePriority_Instance;

static const char *const
  ScoreTqReqPriorityChangePriority_PreDesc_TargetPosition[] = {
    "FirstOneOther",
    "FirstTwoOthers",
    "SecondOneOther",
    "SecondTwoOthers",
    "NA"
};

static const char *const
  ScoreTqReqPriorityChangePriority_PreDesc_NewPriority[] =
    { "High", "Equal", "Low", "NA" };

static const char *const
  ScoreTqReqPriorityChangePriority_PreDesc_OtherQueue[] =
    { "None", "Before", "After", "NA" };

static const char *const *const ScoreTqReqPriorityChangePriority_PreDesc[] = {
  ScoreTqReqPriorityChangePriority_PreDesc_TargetPosition,
  ScoreTqReqPriorityChangePriority_PreDesc_NewPriority,
  ScoreTqReqPriorityChangePriority_PreDesc_OtherQueue,
  NULL
};

typedef enum {
  ScoreTqReqPriorityChangePriority_Pre_TargetPosition_First_OneOther,
  ScoreTqReqPriorityChangePriority_Pre_TargetPosition_First_TwoOthers
} ScoreTqReqPriorityChangePriority_Pre_TargetPosition_First_Rep;

typedef enum {
  ScoreTqReqPriorityChangePriority_Pre_TargetPosition_Second_OneOther,
  ScoreTqReqPriorityChangePriority_Pre_TargetPosition_Second_TwoOthers
} ScoreTqReqPriorityChangePriority_Pre_TargetPosition_Second_Rep;

static const uint8_t
  ScoreTqReqPriorityChangePriority_PreRepOffset_TargetPosition[] =
    { 0, 2, 4, 5 };

static const uint8_t
  ScoreTqReqPriorityChangePriority_PreRepOffset_NewPriority[] =
    { 0, 1, 2, 3, 4 };

static const uint8_t
  ScoreTqReqPriorityChangePriority_PreRepOffset_OtherQueue[] =
    { 0, 1, 2, 3, 4 };

static const uint8_t *const ScoreTqReqPriorityChangePriority_PreRepOffset[] = {
  ScoreTqReqPriorityChangePriority_PreRepOffset_TargetPosition,
  ScoreTqReqPriorityChangePriority_PreRepOffset_NewPriority,
  ScoreTqReqPriorityChangePriority_PreRepOffset_OtherQueue
};

static size_t ScoreTqReqPriorityChangePriority_GetRepresentativeCountOf(
  const ScoreTqReqPriorityChangePriority_Context *ctx,
  size_t                                          index
)
{
  const uint8_t *offsets;
  size_t         state;

  offsets = ScoreTqReqPriorityChangePriority_PreRepOffset[ index ];
  state = ctx->Map.pcs[ index ];

  return offsets[ state + 1 ] - offsets[ state ];
}

static size_t ScoreTqReqPriorityChangePriority_GetRepresentative(
  const ScoreTqReqPriorityChangePriority_Context *ctx,
  size_t                                          index
)
{
  size_t count;

  count = ScoreTqReqPriorityChangePriority_GetRepresentativeCountOf(
    ctx,
    index
  );

  if ( ctx->Map.rep < count ) {
    return ctx->Map.rep;
  }

  return count - 1;
}

static size_t ScoreTqReqPriorityChangePriority_GetRepresentativeCount(
  const ScoreTqReqPriorityChangePriority_Context *ctx
)
{
  size_t count;
  size_t i;

  count = 1;

  for ( i = 0; i < 3; ++i ) {
    size_t count_of_condition;

    count_of_condition =
      ScoreTqReqPriorityChangePriority_GetRepresentativeCountOf( ctx, i );

    if ( count_of_condition > count ) {
      count = count_of_condition;
    }
  }

  return count;
}

static ScoreTqReqPriorityChangePriority_Pre_TargetPosition_First_Rep
ScoreTqReqPriorityChangePriority_Pre_TargetPosition_First_GetRepresentative(
  const ScoreTqReqPriorityChangePriority_Context *ctx
)
{
  return (
    ScoreTqReqPriorityChangePriority_Pre_TargetPosition_First_Rep
  ) ScoreTqReqPriorityChangePriority_GetRepresentative( ctx, 0 );
}

static ScoreTqReqPriorityChangePriority_Pre_TargetPosition_Second_Rep
ScoreTqReqPriorityChangePriority_Pre_TargetPosition_Second_GetRepresentative(
  const ScoreTqReqPriorityChangePriority_Context *ctx
)
{
  return (
    ScoreTqReqPriorityChangePriority_Pre_TargetPosition_Second_Rep
  ) ScoreTqReqPriorityChangePriority_GetRepresentative( ctx, 0 );
}

static const uint8_t ScoreTqReqPriorityChangePriority_PreRepBase[] =
  { 0, 5, 9 };

static const uint8_t ScoreTqReqPriorityChangePriority_PreRepStates[] =
  { 2, 3, 3 };

static void ScoreTqReqPriorityChangePriority_ResetRepresentatives(
  ScoreTqReqPriorityChangePriority_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 13; ++i ) {
    ctx->Map.rep_ran[ i ] = false;
  }
}

static void ScoreTqReqPriorityChangePriority_MarkRepresentatives(
  ScoreTqReqPriorityChangePriority_Context *ctx
)
{
  size_t i;

  if ( ctx->Map.skip_rep ) {
    return;
  }

  for ( i = 0; i < 3; ++i ) {
    size_t rep;

    rep = ScoreTqReqPriorityChangePriority_PreRepBase[ i ] +
          ScoreTqReqPriorityChangePriority_PreRepOffset[ i ]
                                                       [ ctx->Map.pcs[ i ] ] +
          ScoreTqReqPriorityChangePriority_GetRepresentative( ctx, i );
    ctx->Map.rep_ran[ rep ] = true;
  }
}

static void ScoreTqReqPriorityChangePriority_CheckRepresentatives(
  const ScoreTqReqPriorityChangePriority_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 3; ++i ) {
    const uint8_t *offsets;
    const bool    *ran;
    size_t         state;

    offsets = ScoreTqReqPriorityChangePriority_PreRepOffset[ i ];
    ran = &ctx->Map
             .rep_ran[ ScoreTqReqPriorityChangePriority_PreRepBase[ i ] ];

    for (
      state = 0; state < ScoreTqReqPriorityChangePriority_PreRepStates[ i ];
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
            ScoreTqReqPriorityChangePriority_PreDesc[ i ][ k ]
          );
        }
      }
    }
  }
}

typedef ScoreTqReqPriorityChangePriority_Context Context;

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
    TQSetScheduler( ctx->tq_ctx, TQ_HELPER_C, SCHEDULER_B_ID, PRIO_LOW );
  }
}
#endif

static void CheckPosition( const Context *ctx, bool first )
{
  const TQPriorityQueueSnapshot *queue;
  size_t                         index;

  index = TQSnapshotFind( &ctx->after, SCHEDULER_A_ID );
  T_ne_sz( index, SIZE_MAX );

  if ( index == SIZE_MAX ) {
    return;
  }

  queue = &ctx->after.queue[ index ];
  T_eq_sz( queue->count, ctx->lower ? 3 : 2 );

  if ( first ) {
    T_eq_ptr( queue->thread[ 0 ], GetTCB( ctx, TQ_BLOCKER_C ) );
    T_eq_ptr( queue->thread[ 1 ], GetTCB( ctx, TQ_BLOCKER_B ) );
  } else {
    T_eq_ptr( queue->thread[ 0 ], GetTCB( ctx, TQ_BLOCKER_B ) );
    T_eq_ptr( queue->thread[ 1 ], GetTCB( ctx, TQ_BLOCKER_C ) );
  }

  if ( ctx->lower ) {
    T_eq_ptr( queue->thread[ 2 ], GetTCB( ctx, TQ_BLOCKER_D ) );
  }
}

static void ScoreTqReqPriorityChangePriority_Pre_TargetPosition_Prepare(
  ScoreTqReqPriorityChangePriority_Context           *ctx,
  ScoreTqReqPriorityChangePriority_Pre_TargetPosition state
)
{
  switch ( state ) {
    case ScoreTqReqPriorityChangePriority_Pre_TargetPosition_First: {
      /*
       * While the thread is the first thread of the priority queue associated
       * with its home scheduler, while the priority queue contains at least
       * one other thread.
       */
      switch (
        ScoreTqReqPriorityChangePriority_Pre_TargetPosition_First_GetRepresentative(
          ctx
        )
      ) {
        case ScoreTqReqPriorityChangePriority_Pre_TargetPosition_First_OneOther: {
          ctx->lower = false;
          break;
        }

        case ScoreTqReqPriorityChangePriority_Pre_TargetPosition_First_TwoOthers: {
          ctx->lower = true;
          break;
        }
      }

      ctx->initial_priority = PRIO_HIGH;

      if ( IsSticky( ctx ) ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqPriorityChangePriority_Pre_TargetPosition_Second: {
      /*
       * While the thread is the second thread of the priority queue associated
       * with its home scheduler.
       */
      switch (
        ScoreTqReqPriorityChangePriority_Pre_TargetPosition_Second_GetRepresentative(
          ctx
        )
      ) {
        case ScoreTqReqPriorityChangePriority_Pre_TargetPosition_Second_OneOther: {
          ctx->lower = false;
          break;
        }

        case ScoreTqReqPriorityChangePriority_Pre_TargetPosition_Second_TwoOthers: {
          ctx->lower = true;
          break;
        }
      }

      ctx->initial_priority = PRIO_VERY_LOW;

      if ( IsSticky( ctx ) ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqPriorityChangePriority_Pre_TargetPosition_NA:
      break;
  }
}

static void ScoreTqReqPriorityChangePriority_Pre_NewPriority_Prepare(
  ScoreTqReqPriorityChangePriority_Context        *ctx,
  ScoreTqReqPriorityChangePriority_Pre_NewPriority state
)
{
  switch ( state ) {
    case ScoreTqReqPriorityChangePriority_Pre_NewPriority_High: {
      /*
       * While the new eligible priority of the thread with respect to its home
       * scheduler is higher than the highest priority of the other threads of
       * the priority queue.
       */
      ctx->new_priority = PRIO_VERY_HIGH;
      break;
    }

    case ScoreTqReqPriorityChangePriority_Pre_NewPriority_Equal: {
      /*
       * While the new eligible priority of the thread with respect to its home
       * scheduler is equal to the highest priority of the other threads of the
       * priority queue.
       */
      ctx->new_priority = PRIO_NORMAL;
      break;
    }

    case ScoreTqReqPriorityChangePriority_Pre_NewPriority_Low: {
      /*
       * While the new eligible priority of the thread with respect to its home
       * scheduler is lower than the highest priority of the other threads of
       * the priority queue.
       */
      ctx->new_priority = PRIO_LOW;
      break;
    }

    case ScoreTqReqPriorityChangePriority_Pre_NewPriority_NA:
      break;
  }
}

static void ScoreTqReqPriorityChangePriority_Pre_OtherQueue_Prepare(
  ScoreTqReqPriorityChangePriority_Context       *ctx,
  ScoreTqReqPriorityChangePriority_Pre_OtherQueue state
)
{
  switch ( state ) {
    case ScoreTqReqPriorityChangePriority_Pre_OtherQueue_None: {
      /*
       * While the thread queue has exactly one priority queue.
       */
      ctx->other = OTHER_NONE;
      break;
    }

    case ScoreTqReqPriorityChangePriority_Pre_OtherQueue_Before: {
      /*
       * While the thread queue has a second priority queue, while this
       * priority queue is positioned before the priority queue which contains
       * the thread.
       */
      ctx->other = OTHER_BEFORE;

      if ( !HasProcessors( 2 ) || ctx->home_only ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqPriorityChangePriority_Pre_OtherQueue_After: {
      /*
       * While the thread queue has a second priority queue, while this
       * priority queue is positioned after the priority queue which contains
       * the thread.
       */
      ctx->other = OTHER_AFTER;

      if ( !HasProcessors( 2 ) || ctx->home_only ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqPriorityChangePriority_Pre_OtherQueue_NA:
      break;
  }
}

static void ScoreTqReqPriorityChangePriority_Post_Position_Check(
  ScoreTqReqPriorityChangePriority_Context      *ctx,
  ScoreTqReqPriorityChangePriority_Post_Position state
)
{
  switch ( state ) {
    case ScoreTqReqPriorityChangePriority_Post_Position_First: {
      /*
       * The thread queue shall position the thread as the first thread of the
       * priority queue associated with its home scheduler.
       */
      CheckPosition( ctx, true );
      break;
    }

    case ScoreTqReqPriorityChangePriority_Post_Position_Second: {
      /*
       * The thread queue shall position the thread in the priority queue
       * associated with its home scheduler after each thread of the priority
       * queue with a priority higher than or equal to the new eligible
       * priority of the thread and before each thread of the priority queue
       * with a lower priority.
       */
      CheckPosition( ctx, false );
      break;
    }

    case ScoreTqReqPriorityChangePriority_Post_Position_NA:
      break;
  }
}

static void ScoreTqReqPriorityChangePriority_Post_QueueOrder_Check(
  ScoreTqReqPriorityChangePriority_Context        *ctx,
  ScoreTqReqPriorityChangePriority_Post_QueueOrder state
)
{
  switch ( state ) {
    case ScoreTqReqPriorityChangePriority_Post_QueueOrder_Kept: {
      /*
       * The thread queue shall not change the order of its priority queues.
       */
      TQCheckSameQueues( &ctx->before, &ctx->after );
      break;
    }

    case ScoreTqReqPriorityChangePriority_Post_QueueOrder_NA:
      break;
  }
}

static void ScoreTqReqPriorityChangePriority_Setup(
  ScoreTqReqPriorityChangePriority_Context *ctx
)
{
  TQReset( ctx->tq_ctx );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_A, PRIO_VERY_HIGH );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_B, PRIO_NORMAL );

  #if defined( RTEMS_SMP )
  Setup( ctx );
  #endif
}

static void ScoreTqReqPriorityChangePriority_Setup_Wrap( void *arg )
{
  ScoreTqReqPriorityChangePriority_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqPriorityChangePriority_Setup( ctx );
}

static void ScoreTqReqPriorityChangePriority_Teardown(
  ScoreTqReqPriorityChangePriority_Context *ctx
)
{
  TQReset( ctx->tq_ctx );
}

static void ScoreTqReqPriorityChangePriority_Teardown_Wrap( void *arg )
{
  ScoreTqReqPriorityChangePriority_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqPriorityChangePriority_Teardown( ctx );
}

static void ScoreTqReqPriorityChangePriority_Action(
  ScoreTqReqPriorityChangePriority_Context *ctx
)
{
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_C, ctx->initial_priority );
  SetSelfPriority( PRIO_ULTRA_LOW );
  ctx->tq_ctx->how_many = 1;
  TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE_PREPARE );

  if ( ctx->other == OTHER_BEFORE ) {
    TQClearDone( ctx->tq_ctx, TQ_HELPER_C );
    Enqueue( ctx, TQ_HELPER_C, TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER );
    ++ctx->tq_ctx->how_many;
  }

  TQClearDone( ctx->tq_ctx, TQ_BLOCKER_C );
  Enqueue( ctx, TQ_BLOCKER_C, TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER );
  TQClearDone( ctx->tq_ctx, TQ_BLOCKER_B );
  Enqueue( ctx, TQ_BLOCKER_B, TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER );
  ctx->tq_ctx->how_many += 2;

  if ( ctx->lower ) {
    TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_D, PRIO_VERY_LOW );
    TQClearDone( ctx->tq_ctx, TQ_BLOCKER_D );
    Enqueue( ctx, TQ_BLOCKER_D, TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER );
    ++ctx->tq_ctx->how_many;
  }

  if ( ctx->other == OTHER_AFTER ) {
    TQClearDone( ctx->tq_ctx, TQ_HELPER_C );
    Enqueue( ctx, TQ_HELPER_C, TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER );
    ++ctx->tq_ctx->how_many;
  }

  ctx->queue = GetTCB( ctx, TQ_BLOCKER_C )->Wait.queue;
  T_assert_not_null( ctx->queue );
  TQTakeSnapshot( &ctx->before, ctx->queue );
  (void)
    SetPriority( ctx->tq_ctx->worker_id[ TQ_BLOCKER_C ], ctx->new_priority );
  TQTakeSnapshot( &ctx->after, ctx->queue );

  TQClearDone( ctx->tq_ctx, TQ_BLOCKER_A );
  TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE_DONE );
  TQWaitForDone( ctx->tq_ctx, TQ_BLOCKER_A );
  TQWaitForDone( ctx->tq_ctx, TQ_BLOCKER_C );
  TQWaitForDone( ctx->tq_ctx, TQ_BLOCKER_B );
  TQWaitForExecutionStop( ctx->tq_ctx, TQ_BLOCKER_C );
  TQWaitForExecutionStop( ctx->tq_ctx, TQ_BLOCKER_B );

  if ( ctx->lower ) {
    TQWaitForDone( ctx->tq_ctx, TQ_BLOCKER_D );
    TQWaitForExecutionStop( ctx->tq_ctx, TQ_BLOCKER_D );
  }

  if ( ctx->other != OTHER_NONE ) {
    TQWaitForDone( ctx->tq_ctx, TQ_HELPER_C );
    TQWaitForExecutionStop( ctx->tq_ctx, TQ_HELPER_C );
  }

  SetSelfPriority( PRIO_NORMAL );
}

/* clang-format off */

static const ScoreTqReqPriorityChangePriority_Entry
ScoreTqReqPriorityChangePriority_Entries[] = {
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, ScoreTqReqPriorityChangePriority_Post_Position_Second,
    ScoreTqReqPriorityChangePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, ScoreTqReqPriorityChangePriority_Post_Position_NA,
    ScoreTqReqPriorityChangePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, ScoreTqReqPriorityChangePriority_Post_Position_First,
    ScoreTqReqPriorityChangePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, ScoreTqReqPriorityChangePriority_Post_Position_NA,
    ScoreTqReqPriorityChangePriority_Post_QueueOrder_NA },
#endif
  { 0, 0, 0, 0, ScoreTqReqPriorityChangePriority_Post_Position_Second,
    ScoreTqReqPriorityChangePriority_Post_QueueOrder_Kept },
  { 0, 0, 0, 0, ScoreTqReqPriorityChangePriority_Post_Position_First,
    ScoreTqReqPriorityChangePriority_Post_QueueOrder_Kept }
};

static const uint8_t
ScoreTqReqPriorityChangePriority_Map[] = {
  3, 1, 1, 2, 0, 0, 2, 0, 0, 3, 1, 1, 2, 0, 0, 2, 0, 0
};

/* clang-format on */

static size_t ScoreTqReqPriorityChangePriority_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  ScoreTqReqPriorityChangePriority_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    size_t pcd[ 3 ];
    size_t i;

    for ( i = 0; i < 3; ++i ) {
      pcd[ i ] = ScoreTqReqPriorityChangePriority_PreRepOffset
                   [ i ][ ctx->Map.pcs[ i ] ] +
                 ScoreTqReqPriorityChangePriority_GetRepresentative( ctx, i );
    }

    return T_get_scope(
      ScoreTqReqPriorityChangePriority_PreDesc,
      buf,
      n,
      pcd
    );
  }

  return 0;
}

static T_fixture ScoreTqReqPriorityChangePriority_Fixture = {
  .setup = ScoreTqReqPriorityChangePriority_Setup_Wrap,
  .stop = NULL,
  .teardown = ScoreTqReqPriorityChangePriority_Teardown_Wrap,
  .scope = ScoreTqReqPriorityChangePriority_Scope,
  .initial_context = &ScoreTqReqPriorityChangePriority_Instance
};

static const uint8_t ScoreTqReqPriorityChangePriority_Weights[] = { 9, 3, 1 };

static void ScoreTqReqPriorityChangePriority_Skip(
  ScoreTqReqPriorityChangePriority_Context *ctx,
  size_t                                    index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pcs[ 1 ] = ScoreTqReqPriorityChangePriority_Pre_NewPriority_NA -
                          1;
      /* Fall through */
    case 2:
      ctx->Map.pcs[ 2 ] = ScoreTqReqPriorityChangePriority_Pre_OtherQueue_NA -
                          1;
      break;
  }
}

static inline ScoreTqReqPriorityChangePriority_Entry
ScoreTqReqPriorityChangePriority_PopEntry(
  ScoreTqReqPriorityChangePriority_Context *ctx
)
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 3; ++i ) {
      index += ScoreTqReqPriorityChangePriority_Weights[ i ] *
               ctx->Map.pcs[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return ScoreTqReqPriorityChangePriority_Entries
    [ ScoreTqReqPriorityChangePriority_Map[ index ] ];
}

static void ScoreTqReqPriorityChangePriority_TestVariant(
  ScoreTqReqPriorityChangePriority_Context *ctx
)
{
  ScoreTqReqPriorityChangePriority_Pre_TargetPosition_Prepare(
    ctx,
    ctx->Map.pcs[ 0 ]
  );

  if ( ctx->Map.skip ) {
    ScoreTqReqPriorityChangePriority_Skip( ctx, 0 );
    return;
  }

  ScoreTqReqPriorityChangePriority_Pre_NewPriority_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  ScoreTqReqPriorityChangePriority_Pre_OtherQueue_Prepare(
    ctx,
    ctx->Map.pcs[ 2 ]
  );

  if ( ctx->Map.skip ) {
    ScoreTqReqPriorityChangePriority_Skip( ctx, 2 );
    return;
  }

  if ( ctx->Map.skip_rep ) {
    return;
  }

  ScoreTqReqPriorityChangePriority_Action( ctx );
  ScoreTqReqPriorityChangePriority_Post_Position_Check(
    ctx,
    ctx->Map.entry.Post_Position
  );
  ScoreTqReqPriorityChangePriority_Post_QueueOrder_Check(
    ctx,
    ctx->Map.entry.Post_QueueOrder
  );
}

static T_fixture_node ScoreTqReqPriorityChangePriority_Node;

static T_remark ScoreTqReqPriorityChangePriority_Remark = {
  .next = NULL,
  .remark = "ScoreTqReqPriorityChangePriority"
};

void ScoreTqReqPriorityChangePriority_Run( TQContext *tq_ctx, bool home_only )
{
  ScoreTqReqPriorityChangePriority_Context *ctx;

  ctx = &ScoreTqReqPriorityChangePriority_Instance;
  ctx->tq_ctx = tq_ctx;
  ctx->home_only = home_only;

  ctx = T_push_fixture(
    &ScoreTqReqPriorityChangePriority_Node,
    &ScoreTqReqPriorityChangePriority_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;
  ScoreTqReqPriorityChangePriority_ResetRepresentatives( ctx );

  for (
    ctx->Map.pcs[ 0 ] =
      ScoreTqReqPriorityChangePriority_Pre_TargetPosition_First;
    ctx->Map.pcs[ 0 ] < ScoreTqReqPriorityChangePriority_Pre_TargetPosition_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] =
        ScoreTqReqPriorityChangePriority_Pre_NewPriority_High;
      ctx->Map.pcs[ 1 ] < ScoreTqReqPriorityChangePriority_Pre_NewPriority_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] =
          ScoreTqReqPriorityChangePriority_Pre_OtherQueue_None;
        ctx->Map.pcs[ 2 ] < ScoreTqReqPriorityChangePriority_Pre_OtherQueue_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = ScoreTqReqPriorityChangePriority_PopEntry( ctx );

        for (
          ctx->Map.rep = 0;
          ctx->Map.rep <
          ScoreTqReqPriorityChangePriority_GetRepresentativeCount( ctx );
          ++ctx->Map.rep
        ) {
          ctx->Map.skip_rep = false;
          ScoreTqReqPriorityChangePriority_TestVariant( ctx );

          if ( ctx->Map.skip ) {
            break;
          }

          ScoreTqReqPriorityChangePriority_MarkRepresentatives( ctx );
        }
      }
    }
  }

  ScoreTqReqPriorityChangePriority_CheckRepresentatives( ctx );

  T_add_remark( &ScoreTqReqPriorityChangePriority_Remark );
  T_pop_fixture();
}

/** @} */
