/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqEnqueuePriorityIneligible
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

#include "tr-tq-enqueue-priority-ineligible.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreTqReqEnqueuePriorityIneligible \
 *   spec:/score/tq/req/enqueue-priority-ineligible
 *
 * @ingroup TestsuitesValidationNoClock2
 *
 * @{
 */

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_QueueIneligible_NA : 1;
  uint8_t Post_PositionEligible : 1;
  uint8_t Post_QueueOrder : 1;
} ScoreTqReqEnqueuePriorityIneligible_Entry;

/**
 * @brief Test context for spec:/score/tq/req/enqueue-priority-ineligible test
 *   case.
 */
typedef struct {
  /**
   * @brief This member specifies the workers which create the priority queues
   *   associated with the ineligible schedulers of the enqueueing thread in
   *   creation order.
   */
  TQWorkerKind order[ 2 ];

  /**
   * @brief This member contains the priority queues of the thread queue before
   *   the enqueue.
   */
  TQSnapshot before;

  /**
   * @brief This member contains the priority queues of the thread queue while
   *   the enqueueing thread waits.
   */
  TQSnapshot after;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTqReqEnqueuePriorityIneligible_Run() parameter.
   */
  TQContext *tq_ctx;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTqReqEnqueuePriorityIneligible_Run() parameter.
   */
  bool home_only;

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
    ScoreTqReqEnqueuePriorityIneligible_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreTqReqEnqueuePriorityIneligible_Context;

static ScoreTqReqEnqueuePriorityIneligible_Context
  ScoreTqReqEnqueuePriorityIneligible_Instance;

static const char *const
  ScoreTqReqEnqueuePriorityIneligible_PreDesc_QueueIneligible[] = {
    "More",
    "NA"
};

static const char *const *const ScoreTqReqEnqueuePriorityIneligible_PreDesc[] =
  { ScoreTqReqEnqueuePriorityIneligible_PreDesc_QueueIneligible, NULL };

typedef ScoreTqReqEnqueuePriorityIneligible_Context Context;

static bool IsSticky( const Context *ctx )
{
  return ctx->tq_ctx->enqueue_variant == TQ_ENQUEUE_STICKY;
}

static bool UsesProcessors( const Context *ctx )
{
  return !ctx->home_only && rtems_scheduler_get_processor_maximum() >= 4;
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

static Thread_queue_Queue *GetQueue( const Context *ctx )
{
  Thread_queue_Queue *queue;

  queue = ctx->tq_ctx->worker_tcb[ ctx->order[ 0 ] ]->Wait.queue;
  T_assert_not_null( queue );
  return queue;
}

/*
 * A thread which waits in a sticky enqueue occupies a processor of its home
 * scheduler, so the enqueueing thread uses scheduler C as its home scheduler.
 * The runner owns the thread queue and uses the only processor of scheduler
 * A.
 */
static rtems_id HomeScheduler( const Context *ctx )
{
  return IsSticky( ctx ) ? SCHEDULER_C_ID : SCHEDULER_A_ID;
}

/*
 * The thread queue contained no priority queue of the home scheduler, so the
 * map of the enqueue states a new priority queue after the other priority
 * queues.
 */
static void CheckEligible( const Context *ctx )
{
  const TQPriorityQueueSnapshot *queue;
  size_t                         index;

  index = TQSnapshotFind( &ctx->after, HomeScheduler( ctx ) );
  T_eq_sz( index, 2 );

  if ( index == SIZE_MAX ) {
    return;
  }

  queue = &ctx->after.queue[ index ];
  T_eq_sz( queue->count, 1 );
  T_eq_ptr( queue->thread[ 0 ], ctx->tq_ctx->worker_tcb[ TQ_BLOCKER_C ] );
}

static void CheckOrder( const Context *ctx )
{
  T_eq_sz( ctx->before.count, 2 );
  T_eq_sz( ctx->after.count, 3 );
  T_eq_ptr(
    ctx->before.queue[ 0 ].scheduler,
    ctx->after.queue[ 0 ].scheduler
  );
  T_eq_ptr(
    ctx->before.queue[ 1 ].scheduler,
    ctx->after.queue[ 1 ].scheduler
  );
  T_eq_ptr(
    ctx->after.queue[ 0 ].thread[ 0 ],
    ctx->tq_ctx->worker_tcb[ ctx->order[ 0 ] ]
  );
}

static void ScoreTqReqEnqueuePriorityIneligible_Pre_QueueIneligible_Prepare(
  ScoreTqReqEnqueuePriorityIneligible_Context            *ctx,
  ScoreTqReqEnqueuePriorityIneligible_Pre_QueueIneligible state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriorityIneligible_Pre_QueueIneligible_More: {
      /*
       * While the thread queue has at least two priority queues associated
       * with ineligible schedulers of the enqueueing thread.
       */
      ctx->order[ 0 ] = TQ_HELPER_C;
      ctx->order[ 1 ] = TQ_BLOCKER_D;

      if ( !UsesProcessors( ctx ) ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqEnqueuePriorityIneligible_Pre_QueueIneligible_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriorityIneligible_Post_PositionEligible_Check(
  ScoreTqReqEnqueuePriorityIneligible_Context              *ctx,
  ScoreTqReqEnqueuePriorityIneligible_Post_PositionEligible state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriorityIneligible_Post_PositionEligible_AsOne: {
      /*
       * The enqueueing thread shall be enqueued in the priority queues
       * associated with its eligible schedulers as specified by
       * spec:/score/tq/req/enqueue-priority for a thread queue which has
       * exactly one priority queue associated with an ineligible scheduler of
       * the enqueueing thread.
       */
      CheckEligible( ctx );
      break;
    }

    case ScoreTqReqEnqueuePriorityIneligible_Post_PositionEligible_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriorityIneligible_Post_QueueOrder_Check(
  ScoreTqReqEnqueuePriorityIneligible_Context        *ctx,
  ScoreTqReqEnqueuePriorityIneligible_Post_QueueOrder state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriorityIneligible_Post_QueueOrder_Kept: {
      /*
       * The order of the priority queues which the thread queue contained
       * before the enqueue shall not change.
       */
      CheckOrder( ctx );
      break;
    }

    case ScoreTqReqEnqueuePriorityIneligible_Post_QueueOrder_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriorityIneligible_Setup(
  ScoreTqReqEnqueuePriorityIneligible_Context *ctx
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
    TQSetScheduler( ctx->tq_ctx, TQ_HELPER_C, SCHEDULER_B_ID, PRIO_LOW );

    if ( IsSticky( ctx ) ) {
      TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_C, SCHEDULER_C_ID, PRIO_HIGH );
    }
  }
  #endif
}

static void ScoreTqReqEnqueuePriorityIneligible_Setup_Wrap( void *arg )
{
  ScoreTqReqEnqueuePriorityIneligible_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqEnqueuePriorityIneligible_Setup( ctx );
}

static void ScoreTqReqEnqueuePriorityIneligible_Teardown(
  ScoreTqReqEnqueuePriorityIneligible_Context *ctx
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

static void ScoreTqReqEnqueuePriorityIneligible_Teardown_Wrap( void *arg )
{
  ScoreTqReqEnqueuePriorityIneligible_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqEnqueuePriorityIneligible_Teardown( ctx );
}

static void ScoreTqReqEnqueuePriorityIneligible_Action(
  ScoreTqReqEnqueuePriorityIneligible_Context *ctx
)
{
  size_t i;

  SetSelfPriority( PRIO_VERY_LOW );
  ctx->tq_ctx->how_many = 1;

  if ( IsSticky( ctx ) ) {
    TQEnqueuePrepare( ctx->tq_ctx );
  } else {
    TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE_PREPARE );
  }

  for ( i = 0; i < RTEMS_ARRAY_SIZE( ctx->order ); ++i ) {
    TQClearDone( ctx->tq_ctx, ctx->order[ i ] );
    Enqueue( ctx, ctx->order[ i ], TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER );
    ++ctx->tq_ctx->how_many;
  }

  TQTakeSnapshot( &ctx->before, GetQueue( ctx ) );
  TQClearDone( ctx->tq_ctx, TQ_BLOCKER_C );
  Enqueue( ctx, TQ_BLOCKER_C, TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER );
  TQTakeSnapshot( &ctx->after, GetQueue( ctx ) );

  if ( IsSticky( ctx ) ) {
    TQEnqueueDone( ctx->tq_ctx );
  } else {
    TQClearDone( ctx->tq_ctx, TQ_BLOCKER_A );
    TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE_DONE );
    TQWaitForDone( ctx->tq_ctx, TQ_BLOCKER_A );
  }

  TQWaitForDone( ctx->tq_ctx, TQ_BLOCKER_C );

  for ( i = 0; i < RTEMS_ARRAY_SIZE( ctx->order ); ++i ) {
    TQWaitForDone( ctx->tq_ctx, ctx->order[ i ] );
    TQWaitForExecutionStop( ctx->tq_ctx, ctx->order[ i ] );
  }

  TQWaitForExecutionStop( ctx->tq_ctx, TQ_BLOCKER_C );
  SetSelfPriority( PRIO_NORMAL );
}

/* clang-format off */

static const ScoreTqReqEnqueuePriorityIneligible_Entry
ScoreTqReqEnqueuePriorityIneligible_Entries[] = {
#if defined(RTEMS_SMP)
  { 0, 0, ScoreTqReqEnqueuePriorityIneligible_Post_PositionEligible_AsOne,
    ScoreTqReqEnqueuePriorityIneligible_Post_QueueOrder_Kept }
#else
  { 1, 0, ScoreTqReqEnqueuePriorityIneligible_Post_PositionEligible_NA,
    ScoreTqReqEnqueuePriorityIneligible_Post_QueueOrder_NA }
#endif
};

static const uint8_t
ScoreTqReqEnqueuePriorityIneligible_Map[] = {
  0
};

/* clang-format on */

static size_t ScoreTqReqEnqueuePriorityIneligible_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  ScoreTqReqEnqueuePriorityIneligible_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      ScoreTqReqEnqueuePriorityIneligible_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture ScoreTqReqEnqueuePriorityIneligible_Fixture = {
  .setup = ScoreTqReqEnqueuePriorityIneligible_Setup_Wrap,
  .stop = NULL,
  .teardown = ScoreTqReqEnqueuePriorityIneligible_Teardown_Wrap,
  .scope = ScoreTqReqEnqueuePriorityIneligible_Scope,
  .initial_context = &ScoreTqReqEnqueuePriorityIneligible_Instance
};

static const uint8_t ScoreTqReqEnqueuePriorityIneligible_Weights[] = { 1 };

static void ScoreTqReqEnqueuePriorityIneligible_Skip(
  ScoreTqReqEnqueuePriorityIneligible_Context *ctx,
  size_t                                       index
)
{
  (void) ctx;
  (void) index;
}

static inline ScoreTqReqEnqueuePriorityIneligible_Entry
ScoreTqReqEnqueuePriorityIneligible_PopEntry(
  ScoreTqReqEnqueuePriorityIneligible_Context *ctx
)
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 1; ++i ) {
      index += ScoreTqReqEnqueuePriorityIneligible_Weights[ i ] *
               ctx->Map.pcs[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return ScoreTqReqEnqueuePriorityIneligible_Entries
    [ ScoreTqReqEnqueuePriorityIneligible_Map[ index ] ];
}

static void ScoreTqReqEnqueuePriorityIneligible_TestVariant(
  ScoreTqReqEnqueuePriorityIneligible_Context *ctx
)
{
  ScoreTqReqEnqueuePriorityIneligible_Pre_QueueIneligible_Prepare(
    ctx,
    ctx->Map.pcs[ 0 ]
  );

  if ( ctx->Map.skip ) {
    ScoreTqReqEnqueuePriorityIneligible_Skip( ctx, 0 );
    return;
  }

  ScoreTqReqEnqueuePriorityIneligible_Action( ctx );
  ScoreTqReqEnqueuePriorityIneligible_Post_PositionEligible_Check(
    ctx,
    ctx->Map.entry.Post_PositionEligible
  );
  ScoreTqReqEnqueuePriorityIneligible_Post_QueueOrder_Check(
    ctx,
    ctx->Map.entry.Post_QueueOrder
  );
}

static T_fixture_node ScoreTqReqEnqueuePriorityIneligible_Node;

static T_remark ScoreTqReqEnqueuePriorityIneligible_Remark = {
  .next = NULL,
  .remark = "ScoreTqReqEnqueuePriorityIneligible"
};

void ScoreTqReqEnqueuePriorityIneligible_Run(
  TQContext *tq_ctx,
  bool       home_only
)
{
  ScoreTqReqEnqueuePriorityIneligible_Context *ctx;

  ctx = &ScoreTqReqEnqueuePriorityIneligible_Instance;
  ctx->tq_ctx = tq_ctx;
  ctx->home_only = home_only;

  ctx = T_push_fixture(
    &ScoreTqReqEnqueuePriorityIneligible_Node,
    &ScoreTqReqEnqueuePriorityIneligible_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;

  for (
    ctx->Map.pcs[ 0 ] =
      ScoreTqReqEnqueuePriorityIneligible_Pre_QueueIneligible_More;
    ctx->Map.pcs[ 0 ] <
    ScoreTqReqEnqueuePriorityIneligible_Pre_QueueIneligible_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = ScoreTqReqEnqueuePriorityIneligible_PopEntry( ctx );
    ScoreTqReqEnqueuePriorityIneligible_TestVariant( ctx );
  }

  T_add_remark( &ScoreTqReqEnqueuePriorityIneligible_Remark );
  T_pop_fixture();
}

/** @} */
