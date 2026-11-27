/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqEnqueuePriorityInherit
 */

/*
 * Copyright (C) 2021, 2026 embedded brains GmbH & Co. KG
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

#include "tr-tq-enqueue-priority-inherit.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreTqReqEnqueuePriorityInherit \
 *   spec:/score/tq/req/enqueue-priority-inherit
 *
 * @ingroup TestsuitesValidationNoClock2
 *
 * @{
 */

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_EligibleScheduler_NA : 1;
  uint16_t Pre_PriorityForOwner_NA : 1;
  uint16_t Pre_SchedulerForOwner_NA : 1;
  uint16_t Pre_OwnerState_NA : 1;
  uint16_t Post_OwnerPriority : 2;
  uint16_t Post_OwnerScheduler : 2;
  uint16_t Post_OwnerOwnerPriority : 2;
  uint16_t Post_OwnerOwnerScheduler : 2;
} ScoreTqReqEnqueuePriorityInherit_Entry;

#define SCHEDULER_COUNT 2

/**
 * @brief Test context for spec:/score/tq/req/enqueue-priority-inherit test
 *   case.
 */
typedef struct {
  /**
   * @brief This member is true, if the enqueueing thread has a helping
   *   scheduler.
   */
  bool helping;

  /**
   * @brief This member is true, if a priority of the enqueueing thread is
   *   higher than the priority of the owner with respect to the same
   *   scheduler.
   */
  bool vital_priority;

  /**
   * @brief This member is true, if an eligible scheduler of the enqueueing
   *   thread is not an eligible scheduler of the owner.
   */
  bool vital_scheduler;

  /**
   * @brief This member specifies the home scheduler of the owner.
   */
  rtems_id owner_scheduler;

  /**
   * @brief This member specifies the priority of the owner with respect to its
   *   home scheduler.
   */
  rtems_task_priority owner_priority;

  /**
   * @brief This member specifies the priority of the owner with respect to
   *   scheduler B as a helping scheduler. PRIO_INVALID means that the owner
   *   has no such helping scheduler.
   */
  rtems_task_priority owner_helper_priority;

  /**
   * @brief This member contains the priorities of the enqueueing thread before
   *   the enqueue.
   */
  rtems_task_priority enqueue_priority[ SCHEDULER_COUNT ];

  /**
   * @brief This member contains the priorities of the owner before the
   *   enqueue.
   */
  rtems_task_priority owner_before[ SCHEDULER_COUNT ];

  /**
   * @brief This member contains the priorities of the owner after the enqueue.
   */
  rtems_task_priority owner_after[ SCHEDULER_COUNT ];

  /**
   * @brief This member contains the priorities of the owner of the owner
   *   before the enqueue.
   */
  rtems_task_priority owner_owner_before[ SCHEDULER_COUNT ];

  /**
   * @brief This member contains the priorities of the owner of the owner after
   *   the enqueue.
   */
  rtems_task_priority owner_owner_after[ SCHEDULER_COUNT ];

  /**
   * @brief This member specifies the events which let the owner of the owner
   *   obtain the mutex on which the owner waits.
   */
  rtems_event_set owner_obtain;

  /**
   * @brief This member specifies the events which let the owner of the owner
   *   release the mutex on which the owner waits.
   */
  rtems_event_set owner_release;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTqReqEnqueuePriorityInherit_Run() parameter.
   */
  TQContext *tq_ctx;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 4 ];

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
    ScoreTqReqEnqueuePriorityInherit_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreTqReqEnqueuePriorityInherit_Context;

static ScoreTqReqEnqueuePriorityInherit_Context
  ScoreTqReqEnqueuePriorityInherit_Instance;

static const char *const
  ScoreTqReqEnqueuePriorityInherit_PreDesc_EligibleScheduler[] =
    { "Home", "Helping", "NA" };

static const char *const
  ScoreTqReqEnqueuePriorityInherit_PreDesc_PriorityForOwner[] =
    { "Vital", "Dispensable", "NA" };

static const char *const
  ScoreTqReqEnqueuePriorityInherit_PreDesc_SchedulerForOwner[] =
    { "Vital", "Dispensable", "NA" };

static const char *const
  ScoreTqReqEnqueuePriorityInherit_PreDesc_OwnerState[] =
    { "NotEnqueued", "FIFO", "Priority", "PriorityInherit", "NA" };

static const char *const *const ScoreTqReqEnqueuePriorityInherit_PreDesc[] = {
  ScoreTqReqEnqueuePriorityInherit_PreDesc_EligibleScheduler,
  ScoreTqReqEnqueuePriorityInherit_PreDesc_PriorityForOwner,
  ScoreTqReqEnqueuePriorityInherit_PreDesc_SchedulerForOwner,
  ScoreTqReqEnqueuePriorityInherit_PreDesc_OwnerState,
  NULL
};

typedef ScoreTqReqEnqueuePriorityInherit_Context Context;

static const rtems_id scheduler_ids[ SCHEDULER_COUNT ] = {
  SCHEDULER_A_ID,
  SCHEDULER_B_ID
};

static void GetPriorities(
  const Context       *ctx,
  TQWorkerKind         worker,
  rtems_task_priority *priority
)
{
  size_t i;

  for ( i = 0; i < SCHEDULER_COUNT; ++i ) {
    priority[ i ] = GetPriorityByScheduler(
      ctx->tq_ctx->worker_id[ worker ],
      scheduler_ids[ i ]
    );
  }
}

/*
 * A lower value is a higher priority.  PRIO_INVALID is greater than every
 * valid priority.
 */
static bool IsHigher( rtems_task_priority a, rtems_task_priority b )
{
  return a < b;
}

static void CheckPriorityRaise(
  const Context             *ctx,
  const rtems_task_priority *before,
  const rtems_task_priority *after
)
{
  bool   raised;
  size_t i;

  raised = false;

  for ( i = 0; i < SCHEDULER_COUNT; ++i ) {
    if ( before[ i ] == PRIO_INVALID ) {
      continue;
    }

    if ( IsHigher( ctx->enqueue_priority[ i ], before[ i ] ) ) {
      T_eq_u32( after[ i ], ctx->enqueue_priority[ i ] );
      raised = true;
    } else {
      T_eq_u32( after[ i ], before[ i ] );
    }
  }

  T_true( raised );
}

static void CheckPriorityNop(
  const rtems_task_priority *before,
  const rtems_task_priority *after
)
{
  size_t i;

  for ( i = 0; i < SCHEDULER_COUNT; ++i ) {
    if ( before[ i ] != PRIO_INVALID ) {
      T_eq_u32( after[ i ], before[ i ] );
    }
  }
}

static void CheckSchedulerNewHelper(
  const Context             *ctx,
  const rtems_task_priority *before,
  const rtems_task_priority *after
)
{
  bool   added;
  size_t i;

  added = false;

  for ( i = 0; i < SCHEDULER_COUNT; ++i ) {
    if ( before[ i ] != PRIO_INVALID ) {
      continue;
    }

    T_eq_u32( after[ i ], ctx->enqueue_priority[ i ] );

    if ( ctx->enqueue_priority[ i ] != PRIO_INVALID ) {
      added = true;
    }
  }

  T_true( added );
}

static void CheckSchedulerNop(
  const rtems_task_priority *before,
  const rtems_task_priority *after
)
{
  size_t i;

  for ( i = 0; i < SCHEDULER_COUNT; ++i ) {
    if ( before[ i ] == PRIO_INVALID ) {
      T_eq_u32( after[ i ], PRIO_INVALID );
    }
  }
}

/*
 * The enqueueing thread has the priority PRIO_VERY_HIGH with respect to its
 * home scheduler A and the priority PRIO_NORMAL with respect to its helping
 * scheduler B.  The owner gets the home scheduler and the priorities which
 * give the states of the vital priority and the vital scheduler.
 */
static void Arrange( Context *ctx )
{
  ctx->owner_scheduler = SCHEDULER_A_ID;
  ctx->owner_priority = PRIO_VERY_HIGH;
  ctx->owner_helper_priority = PRIO_INVALID;

  if ( ctx->helping ) {
    if ( ctx->vital_priority && ctx->vital_scheduler ) {
      ctx->owner_scheduler = SCHEDULER_B_ID;
      ctx->owner_priority = PRIO_LOW;
    } else if ( ctx->vital_priority ) {
      ctx->owner_helper_priority = PRIO_LOW;
    } else if ( !ctx->vital_scheduler ) {
      ctx->owner_helper_priority = PRIO_HIGH;
    }
  } else if ( ctx->vital_priority ) {
    ctx->owner_priority = PRIO_HIGH;
  } else if ( ctx->vital_scheduler ) {
    ctx->owner_scheduler = SCHEDULER_B_ID;
  }
}

static void ScoreTqReqEnqueuePriorityInherit_Pre_EligibleScheduler_Prepare(
  ScoreTqReqEnqueuePriorityInherit_Context              *ctx,
  ScoreTqReqEnqueuePriorityInherit_Pre_EligibleScheduler state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriorityInherit_Pre_EligibleScheduler_Home: {
      /*
       * While the enqueueing thread has no helping scheduler.
       */
      ctx->helping = false;
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Pre_EligibleScheduler_Helping: {
      /*
       * While the enqueueing thread has at least one helping scheduler.
       */
      ctx->helping = true;

      if ( rtems_scheduler_get_processor_maximum() < 2 ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Pre_EligibleScheduler_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriorityInherit_Pre_PriorityForOwner_Prepare(
  ScoreTqReqEnqueuePriorityInherit_Context             *ctx,
  ScoreTqReqEnqueuePriorityInherit_Pre_PriorityForOwner state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriorityInherit_Pre_PriorityForOwner_Vital: {
      /*
       * While at least one priority of the enqueueing thread is higher than
       * the highest priority of the owner of the thread queue with respect to
       * the same scheduler.
       */
      ctx->vital_priority = true;
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Pre_PriorityForOwner_Dispensable: {
      /*
       * While each priority of the enqueueing thread is lower than or equal to
       * the highest priority of the owner of the thread queue with respect to
       * the same scheduler.
       */
      ctx->vital_priority = false;
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Pre_PriorityForOwner_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriorityInherit_Pre_SchedulerForOwner_Prepare(
  ScoreTqReqEnqueuePriorityInherit_Context              *ctx,
  ScoreTqReqEnqueuePriorityInherit_Pre_SchedulerForOwner state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriorityInherit_Pre_SchedulerForOwner_Vital: {
      /*
       * While at least one eligible scheduler of the enqueueing thread is not
       * an eligible scheduler of the owner of the thread queue.
       */
      ctx->vital_scheduler = true;

      if ( rtems_scheduler_get_processor_maximum() < 2 ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Pre_SchedulerForOwner_Dispensable: {
      /*
       * While all eligible schedulers of the enqueueing thread are an eligible
       * scheduler of the owner of the thread queue.
       */
      ctx->vital_scheduler = false;
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Pre_SchedulerForOwner_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState_Prepare(
  ScoreTqReqEnqueuePriorityInherit_Context       *ctx,
  ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState_NotEnqueued: {
      /*
       * While the owner of the thread queue is not enqueued on a thread queue.
       */
      ctx->owner_obtain = 0;
      ctx->owner_release = 0;
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState_FIFO: {
      /*
       * While the owner of the thread queue is enqueued on a thread queue in
       * FIFO order.
       */
      ctx->owner_obtain = TQ_EVENT_MUTEX_FIFO_OBTAIN;
      ctx->owner_release = TQ_EVENT_MUTEX_FIFO_RELEASE;
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState_Priority: {
      /*
       * While the owner of the thread queue is enqueued on a thread queue in
       * priority order.
       */
      ctx->owner_obtain = TQ_EVENT_MUTEX_NO_PROTOCOL_OBTAIN;
      ctx->owner_release = TQ_EVENT_MUTEX_NO_PROTOCOL_RELEASE;
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState_PriorityInherit: {
      /*
       * While the owner of the thread queue is enqueued on a thread queue in
       * priority order with priority inheritance.
       */
      ctx->owner_obtain = TQ_EVENT_MUTEX_C_OBTAIN;
      ctx->owner_release = TQ_EVENT_MUTEX_C_RELEASE;
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Check(
  ScoreTqReqEnqueuePriorityInherit_Context           *ctx,
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise: {
      /*
       * Each priority of the enqueueing thread which is higher than the
       * highest priority of the owner of the thread queue shall be made the
       * highest priority of the owner.
       */
      CheckPriorityRaise( ctx, ctx->owner_before, ctx->owner_after );
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop: {
      /*
       * The priorities of the owner of the thread queue shall not change.
       */
      CheckPriorityNop( ctx->owner_before, ctx->owner_after );
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Check(
  ScoreTqReqEnqueuePriorityInherit_Context            *ctx,
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper: {
      /*
       * Each eligible scheduler of the enqueueing thread which is not an
       * eligible scheduler of the owner of the thread queue shall be made a
       * helping scheduler of the owner with the priority of the enqueueing
       * thread.
       */
      CheckSchedulerNewHelper( ctx, ctx->owner_before, ctx->owner_after );
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop: {
      /*
       * The set of eligible schedulers of the owner of the thread queue shall
       * not change.
       */
      CheckSchedulerNop( ctx->owner_before, ctx->owner_after );
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Check(
  ScoreTqReqEnqueuePriorityInherit_Context                *ctx,
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Raise: {
      /*
       * Each priority of the enqueueing thread which is higher than the
       * highest priority of the owner of the thread queue on which the owner
       * of the thread queue is enqueued shall be made the highest priority of
       * the owner.
       */
      CheckPriorityRaise(
        ctx,
        ctx->owner_owner_before,
        ctx->owner_owner_after
      );
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop: {
      /*
       * The priorities of the owner of the thread queue on which the owner of
       * the thread queue is enqueued shall not change.
       */
      CheckPriorityNop( ctx->owner_owner_before, ctx->owner_owner_after );
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Check(
  ScoreTqReqEnqueuePriorityInherit_Context                 *ctx,
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NewHelper: {
      /*
       * Each eligible scheduler of the enqueueing thread which is not an
       * eligible scheduler of the owner of the thread queue on which the owner
       * of the thread queue is enqueued shall be made a helping scheduler of
       * the owner with the priority of the enqueueing thread.
       */
      CheckSchedulerNewHelper(
        ctx,
        ctx->owner_owner_before,
        ctx->owner_owner_after
      );
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop: {
      /*
       * The set of eligible schedulers of the owner of the thread queue on
       * which the owner of the thread queue is enqueued shall not change.
       */
      CheckSchedulerNop( ctx->owner_owner_before, ctx->owner_owner_after );
      break;
    }

    case ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriorityInherit_Setup(
  ScoreTqReqEnqueuePriorityInherit_Context *ctx
)
{
  TQReset( ctx->tq_ctx );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_A, PRIO_VERY_HIGH );
}

static void ScoreTqReqEnqueuePriorityInherit_Setup_Wrap( void *arg )
{
  ScoreTqReqEnqueuePriorityInherit_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqEnqueuePriorityInherit_Setup( ctx );
}

static void ScoreTqReqEnqueuePriorityInherit_Teardown(
  ScoreTqReqEnqueuePriorityInherit_Context *ctx
)
{
  TQReset( ctx->tq_ctx );
}

static void ScoreTqReqEnqueuePriorityInherit_Teardown_Wrap( void *arg )
{
  ScoreTqReqEnqueuePriorityInherit_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqEnqueuePriorityInherit_Teardown( ctx );
}

static void ScoreTqReqEnqueuePriorityInherit_Action(
  ScoreTqReqEnqueuePriorityInherit_Context *ctx
)
{
  Arrange( ctx );
  TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_MUTEX_A_OBTAIN );
  TQSetScheduler(
    ctx->tq_ctx,
    TQ_HELPER_A,
    ctx->owner_scheduler,
    ctx->owner_priority
  );
  TQSendAndWaitForExecutionStop(
    ctx->tq_ctx,
    TQ_HELPER_A,
    TQ_EVENT_MUTEX_B_OBTAIN | TQ_EVENT_ENQUEUE_PREPARE
  );

  if ( ctx->owner_helper_priority != PRIO_INVALID ) {
    TQSetScheduler(
      ctx->tq_ctx,
      TQ_HELPER_B,
      SCHEDULER_B_ID,
      ctx->owner_helper_priority
    );
    TQSendAndWaitForExecutionStop(
      ctx->tq_ctx,
      TQ_HELPER_B,
      TQ_EVENT_MUTEX_B_OBTAIN | TQ_EVENT_MUTEX_B_RELEASE
    );
  }

  if ( ctx->helping ) {
    TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_D, SCHEDULER_B_ID, PRIO_NORMAL );
    TQSendAndWaitForExecutionStop(
      ctx->tq_ctx,
      TQ_BLOCKER_D,
      TQ_EVENT_MUTEX_A_OBTAIN | TQ_EVENT_MUTEX_A_RELEASE
    );
  }

  if ( ctx->owner_obtain != 0 ) {
    TQSetScheduler(
      ctx->tq_ctx,
      TQ_HELPER_C,
      ctx->owner_scheduler,
      ctx->owner_priority
    );
    TQSendAndWaitForExecutionStop(
      ctx->tq_ctx,
      TQ_HELPER_C,
      ctx->owner_obtain
    );
    TQClearDone( ctx->tq_ctx, TQ_HELPER_A );
    TQSendAndWaitForExecutionStop(
      ctx->tq_ctx,
      TQ_HELPER_A,
      ctx->owner_obtain | ctx->owner_release
    );
  }

  GetPriorities( ctx, TQ_BLOCKER_A, ctx->enqueue_priority );
  GetPriorities( ctx, TQ_HELPER_A, ctx->owner_before );
  GetPriorities( ctx, TQ_HELPER_C, ctx->owner_owner_before );
  TQSendAndWaitForExecutionStop(
    ctx->tq_ctx,
    TQ_BLOCKER_A,
    TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER | TQ_EVENT_RUNNER_SYNC
  );
  GetPriorities( ctx, TQ_HELPER_A, ctx->owner_after );
  GetPriorities( ctx, TQ_HELPER_C, ctx->owner_owner_after );

  if ( ctx->owner_obtain != 0 ) {
    TQSend( ctx->tq_ctx, TQ_HELPER_C, ctx->owner_release );
    TQWaitForDone( ctx->tq_ctx, TQ_HELPER_A );
    TQWaitForExecutionStop( ctx->tq_ctx, TQ_HELPER_A );
  }

  TQSend( ctx->tq_ctx, TQ_HELPER_A, TQ_EVENT_ENQUEUE_DONE );
  TQSynchronizeRunner();

  TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_MUTEX_A_RELEASE );
  TQMutexObtain( ctx->tq_ctx, TQ_MUTEX_A );
  TQMutexRelease( ctx->tq_ctx, TQ_MUTEX_A );

  TQSend( ctx->tq_ctx, TQ_HELPER_A, TQ_EVENT_MUTEX_B_RELEASE );
  TQMutexObtain( ctx->tq_ctx, TQ_MUTEX_B );
  TQMutexRelease( ctx->tq_ctx, TQ_MUTEX_B );
}

/* clang-format off */

static const ScoreTqReqEnqueuePriorityInherit_Entry
ScoreTqReqEnqueuePriorityInherit_Entries[] = {
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
#else
  { 1, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NA },
#endif
  { 1, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NA },
  { 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
#else
  { 1, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NA },
#endif
  { 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
#else
  { 1, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
#else
  { 1, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NewHelper },
#else
  { 1, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NA },
#endif
  { 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NewHelper },
#else
  { 1, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Raise,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop }
#else
  { 1, 0, 0, 0, 0, ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_NA,
    ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NA }
#endif
};

static const uint8_t
ScoreTqReqEnqueuePriorityInherit_Map[] = {
  1, 1, 1, 1, 4, 4, 4, 8, 0, 0, 0, 7, 2, 2, 2, 2, 5, 5, 5, 9, 6, 6, 6, 10, 0,
  0, 0, 7, 3, 3, 3, 3
};

/* clang-format on */

static size_t ScoreTqReqEnqueuePriorityInherit_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  ScoreTqReqEnqueuePriorityInherit_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      ScoreTqReqEnqueuePriorityInherit_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture ScoreTqReqEnqueuePriorityInherit_Fixture = {
  .setup = ScoreTqReqEnqueuePriorityInherit_Setup_Wrap,
  .stop = NULL,
  .teardown = ScoreTqReqEnqueuePriorityInherit_Teardown_Wrap,
  .scope = ScoreTqReqEnqueuePriorityInherit_Scope,
  .initial_context = &ScoreTqReqEnqueuePriorityInherit_Instance
};

static const uint8_t ScoreTqReqEnqueuePriorityInherit_Weights[] =
  { 16, 8, 4, 1 };

static void ScoreTqReqEnqueuePriorityInherit_Skip(
  ScoreTqReqEnqueuePriorityInherit_Context *ctx,
  size_t                                    index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pcs[ 1 ] =
        ScoreTqReqEnqueuePriorityInherit_Pre_PriorityForOwner_NA - 1;
      /* Fall through */
    case 2:
      ctx->Map.pcs[ 2 ] =
        ScoreTqReqEnqueuePriorityInherit_Pre_SchedulerForOwner_NA - 1;
      /* Fall through */
    case 3:
      ctx->Map.pcs[ 3 ] = ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState_NA -
                          1;
      break;
  }
}

static inline ScoreTqReqEnqueuePriorityInherit_Entry
ScoreTqReqEnqueuePriorityInherit_PopEntry(
  ScoreTqReqEnqueuePriorityInherit_Context *ctx
)
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 4; ++i ) {
      index += ScoreTqReqEnqueuePriorityInherit_Weights[ i ] *
               ctx->Map.pcs[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return ScoreTqReqEnqueuePriorityInherit_Entries
    [ ScoreTqReqEnqueuePriorityInherit_Map[ index ] ];
}

static void ScoreTqReqEnqueuePriorityInherit_TestVariant(
  ScoreTqReqEnqueuePriorityInherit_Context *ctx
)
{
  ScoreTqReqEnqueuePriorityInherit_Pre_EligibleScheduler_Prepare(
    ctx,
    ctx->Map.pcs[ 0 ]
  );

  if ( ctx->Map.skip ) {
    ScoreTqReqEnqueuePriorityInherit_Skip( ctx, 0 );
    return;
  }

  ScoreTqReqEnqueuePriorityInherit_Pre_PriorityForOwner_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  ScoreTqReqEnqueuePriorityInherit_Pre_SchedulerForOwner_Prepare(
    ctx,
    ctx->Map.pcs[ 2 ]
  );

  if ( ctx->Map.skip ) {
    ScoreTqReqEnqueuePriorityInherit_Skip( ctx, 2 );
    return;
  }

  ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState_Prepare(
    ctx,
    ctx->Map.pcs[ 3 ]
  );
  ScoreTqReqEnqueuePriorityInherit_Action( ctx );
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Check(
    ctx,
    ctx->Map.entry.Post_OwnerPriority
  );
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Check(
    ctx,
    ctx->Map.entry.Post_OwnerScheduler
  );
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Check(
    ctx,
    ctx->Map.entry.Post_OwnerOwnerPriority
  );
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Check(
    ctx,
    ctx->Map.entry.Post_OwnerOwnerScheduler
  );
}

static T_fixture_node ScoreTqReqEnqueuePriorityInherit_Node;

static T_remark ScoreTqReqEnqueuePriorityInherit_Remark = {
  .next = NULL,
  .remark = "ScoreTqReqEnqueuePriorityInherit"
};

void ScoreTqReqEnqueuePriorityInherit_Run( TQContext *tq_ctx )
{
  ScoreTqReqEnqueuePriorityInherit_Context *ctx;

  ctx = &ScoreTqReqEnqueuePriorityInherit_Instance;
  ctx->tq_ctx = tq_ctx;

  ctx = T_push_fixture(
    &ScoreTqReqEnqueuePriorityInherit_Node,
    &ScoreTqReqEnqueuePriorityInherit_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;

  for (
    ctx->Map.pcs[ 0 ] =
      ScoreTqReqEnqueuePriorityInherit_Pre_EligibleScheduler_Home;
    ctx->Map.pcs[ 0 ] <
    ScoreTqReqEnqueuePriorityInherit_Pre_EligibleScheduler_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] =
        ScoreTqReqEnqueuePriorityInherit_Pre_PriorityForOwner_Vital;
      ctx->Map.pcs[ 1 ] <
      ScoreTqReqEnqueuePriorityInherit_Pre_PriorityForOwner_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] =
          ScoreTqReqEnqueuePriorityInherit_Pre_SchedulerForOwner_Vital;
        ctx->Map.pcs[ 2 ] <
        ScoreTqReqEnqueuePriorityInherit_Pre_SchedulerForOwner_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] =
            ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState_NotEnqueued;
          ctx->Map.pcs[ 3 ] <
          ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          ctx->Map.entry = ScoreTqReqEnqueuePriorityInherit_PopEntry( ctx );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          ScoreTqReqEnqueuePriorityInherit_TestVariant( ctx );
        }
      }
    }
  }

  T_add_remark( &ScoreTqReqEnqueuePriorityInherit_Remark );
  T_pop_fixture();
}

/** @} */
