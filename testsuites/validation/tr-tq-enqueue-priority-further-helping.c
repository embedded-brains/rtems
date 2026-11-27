/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqEnqueuePriorityFurtherHelping
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

#include "tr-tq-enqueue-priority-further-helping.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreTqReqEnqueuePriorityFurtherHelping \
 *   spec:/score/tq/req/enqueue-priority-further-helping
 *
 * @ingroup TestsuitesValidationNoClock2
 *
 * @{
 */

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_EligibleScheduler_NA : 1;
  uint8_t Pre_ThreadQueue_NA : 1;
  uint8_t Post_PositionEligible : 1;
  uint8_t Post_PositionFurtherHelping : 1;
  uint8_t Post_QueueOrder : 1;
} ScoreTqReqEnqueuePriorityFurtherHelping_Entry;

/**
 * @brief Test context for spec:/score/tq/req/enqueue-priority-further-helping
 *   test case.
 */
typedef struct {
  /**
   * @brief This member is true, if the thread queue contains a priority queue
   *   associated with an ineligible scheduler of the enqueueing thread before
   *   the enqueue.
   */
  bool other;

  /**
   * @brief This member contains the priority queues of the thread queue while
   *   the enqueueing thread waits.
   */
  TQSnapshot snapshot;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTqReqEnqueuePriorityFurtherHelping_Run() parameter.
   */
  TQContext *tq_ctx;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTqReqEnqueuePriorityFurtherHelping_Run() parameter.
   */
  bool home_only;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 2 ];

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
    ScoreTqReqEnqueuePriorityFurtherHelping_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreTqReqEnqueuePriorityFurtherHelping_Context;

static ScoreTqReqEnqueuePriorityFurtherHelping_Context
  ScoreTqReqEnqueuePriorityFurtherHelping_Instance;

static const char *const
  ScoreTqReqEnqueuePriorityFurtherHelping_PreDesc_EligibleScheduler[] = {
    "More",
    "NA"
};

static const char *const
  ScoreTqReqEnqueuePriorityFurtherHelping_PreDesc_ThreadQueue[] =
    { "Empty", "NotEmpty", "NA" };

static const char *const *const
  ScoreTqReqEnqueuePriorityFurtherHelping_PreDesc[] = {
    ScoreTqReqEnqueuePriorityFurtherHelping_PreDesc_EligibleScheduler,
    ScoreTqReqEnqueuePriorityFurtherHelping_PreDesc_ThreadQueue,
    NULL
};

typedef ScoreTqReqEnqueuePriorityFurtherHelping_Context Context;

/*
 * Each helper provider waits for mutex A, which the enqueueing thread owns.
 * The home scheduler of the helper provider becomes a helping scheduler of
 * the enqueueing thread.
 */
static const TQWorkerKind helper_provider[ 3 ] =
  { TQ_BLOCKER_E, TQ_HELPER_A, TQ_HELPER_B };

static bool HasProcessors( uint32_t count )
{
  return rtems_scheduler_get_processor_maximum() >= count;
}

static bool IsSticky( const Context *ctx )
{
  return ctx->tq_ctx->enqueue_variant == TQ_ENQUEUE_STICKY;
}

static bool UsesProcessors( const Context *ctx )
{
  return !ctx->home_only && HasProcessors( IsSticky( ctx ) ? 6 : 5 );
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

/*
 * A thread which waits in a sticky enqueue occupies a processor of its home
 * scheduler, so the enqueueing thread uses scheduler C as its home scheduler.
 * The runner owns the thread queue and uses the only processor of scheduler
 * A.  Index 0 is the home scheduler, the indices 1 to 3 are the helping
 * schedulers of the enqueueing thread.
 */
static rtems_id EligibleScheduler( const Context *ctx, size_t index )
{
  static const rtems_id blocks[ 4 ] =
    { SCHEDULER_A_ID, SCHEDULER_C_ID, SCHEDULER_B_ID, SCHEDULER_E_ID };
  static const rtems_id sticky[ 4 ] =
    { SCHEDULER_C_ID, SCHEDULER_B_ID, SCHEDULER_E_ID, SCHEDULER_F_ID };

  return IsSticky( ctx ) ? sticky[ index ] : blocks[ index ];
}

static void TakeSnapshot( Context *ctx )
{
  Thread_queue_Queue *queue;

  queue = ctx->tq_ctx->worker_tcb[ TQ_BLOCKER_C ]->Wait.queue;
  T_assert_not_null( queue );
  TQTakeSnapshot( &ctx->snapshot, queue );
}

/*
 * Returns the index of the priority queue associated with the eligible
 * scheduler, which contains exactly the enqueueing thread.
 */
static size_t CheckInitial( const Context *ctx, size_t eligible )
{
  const TQPriorityQueueSnapshot *queue;
  size_t                         index;

  index = TQSnapshotFind( &ctx->snapshot, EligibleScheduler( ctx, eligible ) );
  T_ne_sz( index, SIZE_MAX );

  if ( index == SIZE_MAX ) {
    return SIZE_MAX;
  }

  queue = &ctx->snapshot.queue[ index ];
  T_eq_sz( queue->count, 1 );
  T_eq_ptr( queue->thread[ 0 ], ctx->tq_ctx->worker_tcb[ TQ_BLOCKER_C ] );
  return index;
}

/*
 * The thread queue contained no priority queue of an eligible scheduler, so
 * the map of the enqueue states each such priority queue as a new priority
 * queue after the other priority queue and after the priority queues of the
 * preceding eligible schedulers.
 */
static void CheckEligible( const Context *ctx )
{
  size_t first;
  size_t i;
  size_t previous;

  first = ctx->other ? 1 : 0;
  previous = SIZE_MAX;

  for ( i = 0; i < 3; ++i ) {
    size_t index;

    index = CheckInitial( ctx, i );
    T_eq_sz( index, first + i );

    if ( previous != SIZE_MAX ) {
      T_gt_sz( index, previous );
    }

    previous = index;
  }
}

static void CheckFurther( const Context *ctx )
{
  size_t index;

  index = CheckInitial( ctx, 3 );
  T_eq_sz( index, ctx->snapshot.count - 1 );
}

static void CheckOrder( const Context *ctx )
{
  if ( ctx->other ) {
    T_eq_sz( ctx->snapshot.count, 5 );
    T_eq_sz( TQSnapshotFind( &ctx->snapshot, SCHEDULER_D_ID ), 0 );
  } else {
    T_eq_sz( ctx->snapshot.count, 4 );
  }
}

static void
ScoreTqReqEnqueuePriorityFurtherHelping_Pre_EligibleScheduler_Prepare(
  ScoreTqReqEnqueuePriorityFurtherHelping_Context              *ctx,
  ScoreTqReqEnqueuePriorityFurtherHelping_Pre_EligibleScheduler state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriorityFurtherHelping_Pre_EligibleScheduler_More: {
      /*
       * While the enqueueing thread has at least three helping schedulers,
       * while the thread queue has no priority queue associated with a helping
       * scheduler of the enqueueing thread other than the first and the second
       * helping scheduler.
       */
      if ( !UsesProcessors( ctx ) ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqEnqueuePriorityFurtherHelping_Pre_EligibleScheduler_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriorityFurtherHelping_Pre_ThreadQueue_Prepare(
  ScoreTqReqEnqueuePriorityFurtherHelping_Context        *ctx,
  ScoreTqReqEnqueuePriorityFurtherHelping_Pre_ThreadQueue state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriorityFurtherHelping_Pre_ThreadQueue_Empty: {
      /*
       * While the thread queue is empty.
       */
      ctx->other = false;
      break;
    }

    case ScoreTqReqEnqueuePriorityFurtherHelping_Pre_ThreadQueue_NotEmpty: {
      /*
       * While the thread queue is not empty.
       */
      ctx->other = true;
      break;
    }

    case ScoreTqReqEnqueuePriorityFurtherHelping_Pre_ThreadQueue_NA:
      break;
  }
}

static void
ScoreTqReqEnqueuePriorityFurtherHelping_Post_PositionEligible_Check(
  ScoreTqReqEnqueuePriorityFurtherHelping_Context              *ctx,
  ScoreTqReqEnqueuePriorityFurtherHelping_Post_PositionEligible state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriorityFurtherHelping_Post_PositionEligible_AsTwo: {
      /*
       * The enqueueing thread shall be enqueued in the priority queues
       * associated with the home scheduler, the first helping scheduler, and
       * the second helping scheduler of the enqueueing thread as specified by
       * spec:/score/tq/req/enqueue-priority for an enqueueing thread with
       * exactly two helping schedulers.
       */
      CheckEligible( ctx );
      break;
    }

    case ScoreTqReqEnqueuePriorityFurtherHelping_Post_PositionEligible_NA:
      break;
  }
}

static void
ScoreTqReqEnqueuePriorityFurtherHelping_Post_PositionFurtherHelping_Check(
  ScoreTqReqEnqueuePriorityFurtherHelping_Context                    *ctx,
  ScoreTqReqEnqueuePriorityFurtherHelping_Post_PositionFurtherHelping state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriorityFurtherHelping_Post_PositionFurtherHelping_Initial: {
      /*
       * For each helping scheduler of the enqueueing thread other than the
       * first and the second helping scheduler, a priority queue which
       * contains exactly the enqueueing thread and which is associated with
       * this helping scheduler shall be positioned after each priority queue
       * which the thread queue contained before the enqueue, after the
       * priority queues associated with the home scheduler, the first helping
       * scheduler, and the second helping scheduler of the enqueueing thread,
       * and after the priority queues associated with each helping scheduler
       * which became a helping scheduler of the enqueueing thread before this
       * helping scheduler.
       */
      CheckFurther( ctx );
      break;
    }

    case ScoreTqReqEnqueuePriorityFurtherHelping_Post_PositionFurtherHelping_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriorityFurtherHelping_Post_QueueOrder_Check(
  ScoreTqReqEnqueuePriorityFurtherHelping_Context        *ctx,
  ScoreTqReqEnqueuePriorityFurtherHelping_Post_QueueOrder state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriorityFurtherHelping_Post_QueueOrder_Kept: {
      /*
       * The order of the priority queues which the thread queue contained
       * before the enqueue shall not change.
       */
      CheckOrder( ctx );
      break;
    }

    case ScoreTqReqEnqueuePriorityFurtherHelping_Post_QueueOrder_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriorityFurtherHelping_Setup(
  ScoreTqReqEnqueuePriorityFurtherHelping_Context *ctx
)
{
  TQReset( ctx->tq_ctx );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_A, PRIO_VERY_HIGH );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_C, PRIO_HIGH );

  #if defined( RTEMS_SMP )
  if ( UsesProcessors( ctx ) ) {
    RemoveProcessor( SCHEDULER_C_ID, 3 );
    AddProcessor( SCHEDULER_D_ID, 3 );
    TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_D, SCHEDULER_D_ID, PRIO_LOW );

    if ( IsSticky( ctx ) ) {
      TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_C, SCHEDULER_C_ID, PRIO_HIGH );
      TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_E, SCHEDULER_B_ID, PRIO_HIGH );
      TQSetScheduler( ctx->tq_ctx, TQ_HELPER_A, SCHEDULER_E_ID, PRIO_HIGH );
      TQSetScheduler( ctx->tq_ctx, TQ_HELPER_B, SCHEDULER_F_ID, PRIO_HIGH );
    } else {
      TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_E, SCHEDULER_C_ID, PRIO_HIGH );
      TQSetScheduler( ctx->tq_ctx, TQ_HELPER_A, SCHEDULER_B_ID, PRIO_HIGH );
      TQSetScheduler( ctx->tq_ctx, TQ_HELPER_B, SCHEDULER_E_ID, PRIO_HIGH );
    }
  }
  #endif
}

static void ScoreTqReqEnqueuePriorityFurtherHelping_Setup_Wrap( void *arg )
{
  ScoreTqReqEnqueuePriorityFurtherHelping_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqEnqueuePriorityFurtherHelping_Setup( ctx );
}

static void ScoreTqReqEnqueuePriorityFurtherHelping_Teardown(
  ScoreTqReqEnqueuePriorityFurtherHelping_Context *ctx
)
{
  TQReset( ctx->tq_ctx );

  #if defined( RTEMS_SMP )
  if ( UsesProcessors( ctx ) ) {
    RemoveProcessor( SCHEDULER_D_ID, 3 );
    AddProcessor( SCHEDULER_C_ID, 3 );
  }
  #endif
}

static void ScoreTqReqEnqueuePriorityFurtherHelping_Teardown_Wrap( void *arg )
{
  ScoreTqReqEnqueuePriorityFurtherHelping_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqEnqueuePriorityFurtherHelping_Teardown( ctx );
}

static void ScoreTqReqEnqueuePriorityFurtherHelping_Action(
  ScoreTqReqEnqueuePriorityFurtherHelping_Context *ctx
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

  TQSend( ctx->tq_ctx, TQ_BLOCKER_C, TQ_EVENT_MUTEX_A_OBTAIN );

  for ( i = 0; i < RTEMS_ARRAY_SIZE( helper_provider ); ++i ) {
    TQSendAndWaitForExecutionStop(
      ctx->tq_ctx,
      helper_provider[ i ],
      TQ_EVENT_MUTEX_A_OBTAIN | TQ_EVENT_MUTEX_A_RELEASE
    );
  }

  if ( ctx->other ) {
    TQClearDone( ctx->tq_ctx, TQ_BLOCKER_D );
    Enqueue( ctx, TQ_BLOCKER_D, TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER );
    ++ctx->tq_ctx->how_many;
  }

  events = TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER | TQ_EVENT_MUTEX_A_RELEASE;
  TQClearDone( ctx->tq_ctx, TQ_BLOCKER_C );
  Enqueue( ctx, TQ_BLOCKER_C, events );
  TakeSnapshot( ctx );

  if ( IsSticky( ctx ) ) {
    TQEnqueueDone( ctx->tq_ctx );
  } else {
    TQClearDone( ctx->tq_ctx, TQ_BLOCKER_A );
    TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE_DONE );
    TQWaitForDone( ctx->tq_ctx, TQ_BLOCKER_A );
  }

  TQWaitForDone( ctx->tq_ctx, TQ_BLOCKER_C );

  if ( ctx->other ) {
    TQWaitForDone( ctx->tq_ctx, TQ_BLOCKER_D );
    TQWaitForExecutionStop( ctx->tq_ctx, TQ_BLOCKER_D );
  }

  TQWaitForExecutionStop( ctx->tq_ctx, TQ_BLOCKER_C );
  TQMutexObtain( ctx->tq_ctx, TQ_MUTEX_A );
  TQMutexRelease( ctx->tq_ctx, TQ_MUTEX_A );
  SetSelfPriority( PRIO_NORMAL );
}

/* clang-format off */

static const ScoreTqReqEnqueuePriorityFurtherHelping_Entry
ScoreTqReqEnqueuePriorityFurtherHelping_Entries[] = {
#if defined(RTEMS_SMP)
  { 0, 0, 0,
    ScoreTqReqEnqueuePriorityFurtherHelping_Post_PositionEligible_AsTwo,
    ScoreTqReqEnqueuePriorityFurtherHelping_Post_PositionFurtherHelping_Initial,
    ScoreTqReqEnqueuePriorityFurtherHelping_Post_QueueOrder_Kept }
#else
  { 1, 0, 0, ScoreTqReqEnqueuePriorityFurtherHelping_Post_PositionEligible_NA,
    ScoreTqReqEnqueuePriorityFurtherHelping_Post_PositionFurtherHelping_NA,
    ScoreTqReqEnqueuePriorityFurtherHelping_Post_QueueOrder_NA }
#endif
};

static const uint8_t
ScoreTqReqEnqueuePriorityFurtherHelping_Map[] = {
  0, 0
};

/* clang-format on */

static size_t ScoreTqReqEnqueuePriorityFurtherHelping_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  ScoreTqReqEnqueuePriorityFurtherHelping_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      ScoreTqReqEnqueuePriorityFurtherHelping_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture ScoreTqReqEnqueuePriorityFurtherHelping_Fixture = {
  .setup = ScoreTqReqEnqueuePriorityFurtherHelping_Setup_Wrap,
  .stop = NULL,
  .teardown = ScoreTqReqEnqueuePriorityFurtherHelping_Teardown_Wrap,
  .scope = ScoreTqReqEnqueuePriorityFurtherHelping_Scope,
  .initial_context = &ScoreTqReqEnqueuePriorityFurtherHelping_Instance
};

static const uint8_t ScoreTqReqEnqueuePriorityFurtherHelping_Weights[] = {
  2,
  1
};

static void ScoreTqReqEnqueuePriorityFurtherHelping_Skip(
  ScoreTqReqEnqueuePriorityFurtherHelping_Context *ctx,
  size_t                                           index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pcs[ 1 ] =
        ScoreTqReqEnqueuePriorityFurtherHelping_Pre_ThreadQueue_NA - 1;
      break;
  }
}

static inline ScoreTqReqEnqueuePriorityFurtherHelping_Entry
ScoreTqReqEnqueuePriorityFurtherHelping_PopEntry(
  ScoreTqReqEnqueuePriorityFurtherHelping_Context *ctx
)
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 2; ++i ) {
      index += ScoreTqReqEnqueuePriorityFurtherHelping_Weights[ i ] *
               ctx->Map.pcs[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return ScoreTqReqEnqueuePriorityFurtherHelping_Entries
    [ ScoreTqReqEnqueuePriorityFurtherHelping_Map[ index ] ];
}

static void ScoreTqReqEnqueuePriorityFurtherHelping_TestVariant(
  ScoreTqReqEnqueuePriorityFurtherHelping_Context *ctx
)
{
  ScoreTqReqEnqueuePriorityFurtherHelping_Pre_EligibleScheduler_Prepare(
    ctx,
    ctx->Map.pcs[ 0 ]
  );

  if ( ctx->Map.skip ) {
    ScoreTqReqEnqueuePriorityFurtherHelping_Skip( ctx, 0 );
    return;
  }

  ScoreTqReqEnqueuePriorityFurtherHelping_Pre_ThreadQueue_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  ScoreTqReqEnqueuePriorityFurtherHelping_Action( ctx );
  ScoreTqReqEnqueuePriorityFurtherHelping_Post_PositionEligible_Check(
    ctx,
    ctx->Map.entry.Post_PositionEligible
  );
  ScoreTqReqEnqueuePriorityFurtherHelping_Post_PositionFurtherHelping_Check(
    ctx,
    ctx->Map.entry.Post_PositionFurtherHelping
  );
  ScoreTqReqEnqueuePriorityFurtherHelping_Post_QueueOrder_Check(
    ctx,
    ctx->Map.entry.Post_QueueOrder
  );
}

static T_fixture_node ScoreTqReqEnqueuePriorityFurtherHelping_Node;

static T_remark ScoreTqReqEnqueuePriorityFurtherHelping_Remark = {
  .next = NULL,
  .remark = "ScoreTqReqEnqueuePriorityFurtherHelping"
};

void ScoreTqReqEnqueuePriorityFurtherHelping_Run(
  TQContext *tq_ctx,
  bool       home_only
)
{
  ScoreTqReqEnqueuePriorityFurtherHelping_Context *ctx;

  ctx = &ScoreTqReqEnqueuePriorityFurtherHelping_Instance;
  ctx->tq_ctx = tq_ctx;
  ctx->home_only = home_only;

  ctx = T_push_fixture(
    &ScoreTqReqEnqueuePriorityFurtherHelping_Node,
    &ScoreTqReqEnqueuePriorityFurtherHelping_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;

  for (
    ctx->Map.pcs[ 0 ] =
      ScoreTqReqEnqueuePriorityFurtherHelping_Pre_EligibleScheduler_More;
    ctx->Map.pcs[ 0 ] <
    ScoreTqReqEnqueuePriorityFurtherHelping_Pre_EligibleScheduler_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] =
        ScoreTqReqEnqueuePriorityFurtherHelping_Pre_ThreadQueue_Empty;
      ctx->Map.pcs[ 1 ] <
      ScoreTqReqEnqueuePriorityFurtherHelping_Pre_ThreadQueue_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = ScoreTqReqEnqueuePriorityFurtherHelping_PopEntry( ctx );
      ScoreTqReqEnqueuePriorityFurtherHelping_TestVariant( ctx );
    }
  }

  T_add_remark( &ScoreTqReqEnqueuePriorityFurtherHelping_Remark );
  T_pop_fixture();
}

/** @} */
