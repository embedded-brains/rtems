/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqEnqueuePriority
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

#include <string.h>
#include <rtems/score/priorityimpl.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/threadqimpl.h>

#include "tr-tq-enqueue-priority.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreTqReqEnqueuePriority spec:/score/tq/req/enqueue-priority
 *
 * @ingroup TestsuitesValidationNoClock2
 *
 * @{
 */

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_EligibleScheduler_NA : 1;
  uint32_t Pre_QueueHome_NA : 1;
  uint32_t Pre_QueueFirstHelping_NA : 1;
  uint32_t Pre_QueueSecondHelping_NA : 1;
  uint32_t Pre_QueueIneligible_NA : 1;
  uint32_t Pre_QueueOrder_NA : 1;
  uint32_t Post_PositionHome : 2;
  uint32_t Post_PositionFirstHelping : 3;
  uint32_t Post_PositionSecondHelping : 3;
  uint32_t Post_PositionOtherHelping : 2;
  uint32_t Post_QueueOrder : 1;
} ScoreTqReqEnqueuePriority_Entry;

typedef enum {
  ROLE_HOME,
  ROLE_FIRST,
  ROLE_SECOND,
  ROLE_INELIGIBLE,
  ROLE_OTHER,
  ROLE_INELIGIBLE_LAST,
  ROLE_COUNT
} Role;

#define ROLE_ORDER_COUNT 4

typedef enum { PLACE_NONE, PLACE_INITIAL, PLACE_FIRST, PLACE_SECOND } Place;

/**
 * @brief Test context for spec:/score/tq/req/enqueue-priority test case.
 */
typedef struct {
  /**
   * @brief This member specifies the count of helping schedulers of the
   *   enqueueing thread.
   */
  size_t helping;

  /**
   * @brief This member specifies the highest priority of the priority queue of
   *   each role. PRIO_INVALID means that the thread queue contains no priority
   *   queue for the role.
   */
  rtems_task_priority queue_priority[ ROLE_COUNT ];

  /**
   * @brief This member specifies the creation order of the priority queues of
   *   the roles.
   */
  Role order[ ROLE_ORDER_COUNT ];

  /**
   * @brief This member is true for a role, if the priority queue of the role
   *   also contains the lower thread.
   */
  bool with_lower[ ROLE_COUNT ];

  /**
   * @brief This member contains the priority queues of the thread queue while
   *   the enqueueing thread waits.
   */
  TQSnapshot snapshot;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTqReqEnqueuePriority_Run() parameter.
   */
  TQContext *tq_ctx;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreTqReqEnqueuePriority_Run() parameter.
   */
  bool home_only;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 6 ];

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
    bool rep_ran[ 55 ];

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
    ScoreTqReqEnqueuePriority_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} ScoreTqReqEnqueuePriority_Context;

static ScoreTqReqEnqueuePriority_Context ScoreTqReqEnqueuePriority_Instance;

static const char *const
  ScoreTqReqEnqueuePriority_PreDesc_EligibleScheduler[] =
    { "Home", "One", "Two", "More", "NA" };

static const char *const ScoreTqReqEnqueuePriority_PreDesc_QueueHome[] =
  { "Empty", "HighAlone", "HighLower", "Equal", "LowAlone", "LowLower", "NA" };

static const char *const
  ScoreTqReqEnqueuePriority_PreDesc_QueueFirstHelping[] = {
    "Empty",
    "High",
    "EqualAlone",
    "EqualLower",
    "LowAlone",
    "LowLower",
    "NA"
};

static const char *const
  ScoreTqReqEnqueuePriority_PreDesc_QueueSecondHelping[] = {
    "Empty",
    "High",
    "EqualAlone",
    "EqualLower",
    "LowAlone",
    "LowLower",
    "NA"
};

static const char *const ScoreTqReqEnqueuePriority_PreDesc_QueueIneligible[] =
  { "None", "One", "More", "NA" };

static const char *const ScoreTqReqEnqueuePriority_PreDesc_QueueOrder[] = {
  "HomeFirstSecondIneligible",
  "HomeFirstIneligibleSecond",
  "HomeSecondFirstIneligible",
  "HomeSecondIneligibleFirst",
  "HomeIneligibleFirstSecond",
  "HomeIneligibleSecondFirst",
  "FirstHomeSecondIneligible",
  "FirstHomeIneligibleSecond",
  "FirstSecondHomeIneligible",
  "FirstSecondIneligibleHome",
  "FirstIneligibleHomeSecond",
  "FirstIneligibleSecondHome",
  "SecondHomeFirstIneligible",
  "SecondHomeIneligibleFirst",
  "SecondFirstHomeIneligible",
  "SecondFirstIneligibleHome",
  "SecondIneligibleHomeFirst",
  "SecondIneligibleFirstHome",
  "IneligibleHomeFirstSecond",
  "IneligibleHomeSecondFirst",
  "IneligibleFirstHomeSecond",
  "IneligibleFirstSecondHome",
  "IneligibleSecondHomeFirst",
  "IneligibleSecondFirstHome",
  "NA"
};

static const char *const *const ScoreTqReqEnqueuePriority_PreDesc[] = {
  ScoreTqReqEnqueuePriority_PreDesc_EligibleScheduler,
  ScoreTqReqEnqueuePriority_PreDesc_QueueHome,
  ScoreTqReqEnqueuePriority_PreDesc_QueueFirstHelping,
  ScoreTqReqEnqueuePriority_PreDesc_QueueSecondHelping,
  ScoreTqReqEnqueuePriority_PreDesc_QueueIneligible,
  ScoreTqReqEnqueuePriority_PreDesc_QueueOrder,
  NULL
};

typedef enum {
  ScoreTqReqEnqueuePriority_Pre_QueueHome_High_Alone,
  ScoreTqReqEnqueuePriority_Pre_QueueHome_High_Lower
} ScoreTqReqEnqueuePriority_Pre_QueueHome_High_Rep;

typedef enum {
  ScoreTqReqEnqueuePriority_Pre_QueueHome_Low_Alone,
  ScoreTqReqEnqueuePriority_Pre_QueueHome_Low_Lower
} ScoreTqReqEnqueuePriority_Pre_QueueHome_Low_Rep;

typedef enum {
  ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_Equal_Alone,
  ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_Equal_Lower
} ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_Equal_Rep;

typedef enum {
  ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_Low_Alone,
  ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_Low_Lower
} ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_Low_Rep;

typedef enum {
  ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_Equal_Alone,
  ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_Equal_Lower
} ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_Equal_Rep;

typedef enum {
  ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_Low_Alone,
  ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_Low_Lower
} ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_Low_Rep;

static const uint8_t
  ScoreTqReqEnqueuePriority_PreRepOffset_EligibleScheduler[] =
    { 0, 1, 2, 3, 4, 5 };

static const uint8_t ScoreTqReqEnqueuePriority_PreRepOffset_QueueHome[] =
  { 0, 1, 3, 4, 6, 7 };

static const uint8_t
  ScoreTqReqEnqueuePriority_PreRepOffset_QueueFirstHelping[] =
    { 0, 1, 2, 4, 6, 7 };

static const uint8_t
  ScoreTqReqEnqueuePriority_PreRepOffset_QueueSecondHelping[] =
    { 0, 1, 2, 4, 6, 7 };

static const uint8_t ScoreTqReqEnqueuePriority_PreRepOffset_QueueIneligible[] =
  { 0, 1, 2, 3, 4 };

static const uint8_t ScoreTqReqEnqueuePriority_PreRepOffset_QueueOrder[] = {
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  14,
  15,
  16,
  17,
  18,
  19,
  20,
  21,
  22,
  23,
  24,
  25
};

static const uint8_t *const ScoreTqReqEnqueuePriority_PreRepOffset[] = {
  ScoreTqReqEnqueuePriority_PreRepOffset_EligibleScheduler,
  ScoreTqReqEnqueuePriority_PreRepOffset_QueueHome,
  ScoreTqReqEnqueuePriority_PreRepOffset_QueueFirstHelping,
  ScoreTqReqEnqueuePriority_PreRepOffset_QueueSecondHelping,
  ScoreTqReqEnqueuePriority_PreRepOffset_QueueIneligible,
  ScoreTqReqEnqueuePriority_PreRepOffset_QueueOrder
};

static size_t ScoreTqReqEnqueuePriority_GetRepresentativeCountOf(
  const ScoreTqReqEnqueuePriority_Context *ctx,
  size_t                                   index
)
{
  const uint8_t *offsets;
  size_t         state;

  offsets = ScoreTqReqEnqueuePriority_PreRepOffset[ index ];
  state = ctx->Map.pcs[ index ];

  return offsets[ state + 1 ] - offsets[ state ];
}

static size_t ScoreTqReqEnqueuePriority_GetRepresentative(
  const ScoreTqReqEnqueuePriority_Context *ctx,
  size_t                                   index
)
{
  size_t count;

  count = ScoreTqReqEnqueuePriority_GetRepresentativeCountOf( ctx, index );

  if ( ctx->Map.rep < count ) {
    return ctx->Map.rep;
  }

  return count - 1;
}

static size_t ScoreTqReqEnqueuePriority_GetRepresentativeCount(
  const ScoreTqReqEnqueuePriority_Context *ctx
)
{
  size_t count;
  size_t i;

  count = 1;

  for ( i = 0; i < 6; ++i ) {
    size_t count_of_condition;

    count_of_condition = ScoreTqReqEnqueuePriority_GetRepresentativeCountOf(
      ctx,
      i
    );

    if ( count_of_condition > count ) {
      count = count_of_condition;
    }
  }

  return count;
}

static ScoreTqReqEnqueuePriority_Pre_QueueHome_High_Rep
ScoreTqReqEnqueuePriority_Pre_QueueHome_High_GetRepresentative(
  const ScoreTqReqEnqueuePriority_Context *ctx
)
{
  return (
    ScoreTqReqEnqueuePriority_Pre_QueueHome_High_Rep
  ) ScoreTqReqEnqueuePriority_GetRepresentative( ctx, 1 );
}

static ScoreTqReqEnqueuePriority_Pre_QueueHome_Low_Rep
ScoreTqReqEnqueuePriority_Pre_QueueHome_Low_GetRepresentative(
  const ScoreTqReqEnqueuePriority_Context *ctx
)
{
  return (
    ScoreTqReqEnqueuePriority_Pre_QueueHome_Low_Rep
  ) ScoreTqReqEnqueuePriority_GetRepresentative( ctx, 1 );
}

static ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_Equal_Rep
ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_Equal_GetRepresentative(
  const ScoreTqReqEnqueuePriority_Context *ctx
)
{
  return (
    ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_Equal_Rep
  ) ScoreTqReqEnqueuePriority_GetRepresentative( ctx, 2 );
}

static ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_Low_Rep
ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_Low_GetRepresentative(
  const ScoreTqReqEnqueuePriority_Context *ctx
)
{
  return (
    ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_Low_Rep
  ) ScoreTqReqEnqueuePriority_GetRepresentative( ctx, 2 );
}

static ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_Equal_Rep
ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_Equal_GetRepresentative(
  const ScoreTqReqEnqueuePriority_Context *ctx
)
{
  return (
    ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_Equal_Rep
  ) ScoreTqReqEnqueuePriority_GetRepresentative( ctx, 3 );
}

static ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_Low_Rep
ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_Low_GetRepresentative(
  const ScoreTqReqEnqueuePriority_Context *ctx
)
{
  return (
    ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_Low_Rep
  ) ScoreTqReqEnqueuePriority_GetRepresentative( ctx, 3 );
}

static const uint8_t ScoreTqReqEnqueuePriority_PreRepBase[] =
  { 0, 5, 12, 19, 26, 30 };

static const uint8_t ScoreTqReqEnqueuePriority_PreRepStates[] =
  { 4, 4, 4, 4, 3, 24 };

static void ScoreTqReqEnqueuePriority_ResetRepresentatives(
  ScoreTqReqEnqueuePriority_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 55; ++i ) {
    ctx->Map.rep_ran[ i ] = false;
  }
}

static void ScoreTqReqEnqueuePriority_MarkRepresentatives(
  ScoreTqReqEnqueuePriority_Context *ctx
)
{
  size_t i;

  if ( ctx->Map.skip_rep ) {
    return;
  }

  for ( i = 0; i < 6; ++i ) {
    size_t rep;

    rep = ScoreTqReqEnqueuePriority_PreRepBase[ i ] +
          ScoreTqReqEnqueuePriority_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
          ScoreTqReqEnqueuePriority_GetRepresentative( ctx, i );
    ctx->Map.rep_ran[ rep ] = true;
  }
}

static void ScoreTqReqEnqueuePriority_CheckRepresentatives(
  const ScoreTqReqEnqueuePriority_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 6; ++i ) {
    const uint8_t *offsets;
    const bool    *ran;
    size_t         state;

    offsets = ScoreTqReqEnqueuePriority_PreRepOffset[ i ];
    ran = &ctx->Map.rep_ran[ ScoreTqReqEnqueuePriority_PreRepBase[ i ] ];

    for (
      state = 0; state < ScoreTqReqEnqueuePriority_PreRepStates[ i ]; ++state
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
            ScoreTqReqEnqueuePriority_PreDesc[ i ][ k ]
          );
        }
      }
    }
  }
}

typedef ScoreTqReqEnqueuePriority_Context Context;

static const TQWorkerKind role_worker[ ROLE_COUNT ] = {
  TQ_BLOCKER_B,
  TQ_WORKER_F,
  TQ_HELPER_C,
  TQ_BLOCKER_D,
  TQ_HELPER_B,
  TQ_WORKER_G
};

static const rtems_id role_scheduler_blocks[ ROLE_COUNT ] = {
  SCHEDULER_A_ID,
  SCHEDULER_C_ID,
  SCHEDULER_B_ID,
  SCHEDULER_D_ID,
  SCHEDULER_E_ID,
  SCHEDULER_F_ID
};

/*
 * With two or three processors, scheduler B owns the only processor which
 * scheduler A does not own, so scheduler B is the first helping scheduler.
 */
static const rtems_id role_scheduler_two[ ROLE_COUNT ] = {
  SCHEDULER_A_ID,
  SCHEDULER_B_ID,
  SCHEDULER_C_ID,
  SCHEDULER_D_ID,
  SCHEDULER_E_ID,
  SCHEDULER_F_ID
};

static const TQWorkerKind helper_provider[ 3 ] =
  { TQ_BLOCKER_E, TQ_HELPER_A, TQ_HELPER_B };

/*
 * The third helper provider and the worker of ROLE_OTHER are needed only for
 * a third helping scheduler, which the map skips.
 */
#define LOWER_WORKER TQ_HELPER_B

static bool HasTwoProcessors( void )
{
  return rtems_scheduler_get_processor_maximum() >= 2;
}

static bool HasFourProcessors( void )
{
  return rtems_scheduler_get_processor_maximum() >= 4;
}

static bool HasFiveProcessors( void )
{
  return rtems_scheduler_get_processor_maximum() >= 5;
}

static bool HasEightProcessors( void )
{
  return rtems_scheduler_get_processor_maximum() >= 8;
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

/*
 * A thread which waits in a sticky enqueue occupies a processor of its home
 * scheduler, so each role uses a scheduler of its own.  The runner owns the
 * thread queue and uses the only processor of scheduler A.
 */
static rtems_id RoleScheduler( const Context *ctx, Role role )
{
  if ( !IsSticky( ctx ) ) {
    if ( !HasFourProcessors() ) {
      return role_scheduler_two[ role ];
    }

    return role_scheduler_blocks[ role ];
  }

  switch ( role ) {
    case ROLE_HOME:
      return SCHEDULER_C_ID;
    case ROLE_FIRST:
      return SCHEDULER_B_ID;
    case ROLE_SECOND:
      return SCHEDULER_E_ID;
    case ROLE_INELIGIBLE:
      return HasEightProcessors() ? SCHEDULER_H_ID : SCHEDULER_D_ID;
    case ROLE_OTHER:
      return SCHEDULER_F_ID;
    default:
      return SCHEDULER_G_ID;
  }
}

#if defined( RTEMS_SMP )
static void SetupSticky( Context *ctx )
{
  TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_C, SCHEDULER_C_ID, PRIO_HIGH );
  TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_B, SCHEDULER_C_ID, PRIO_LOW );
  TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_E, SCHEDULER_B_ID, PRIO_HIGH );
  TQSetScheduler( ctx->tq_ctx, TQ_WORKER_F, SCHEDULER_B_ID, PRIO_LOW );

  if ( HasFiveProcessors() ) {
    TQSetScheduler( ctx->tq_ctx, TQ_HELPER_A, SCHEDULER_E_ID, PRIO_HIGH );
    TQSetScheduler( ctx->tq_ctx, TQ_HELPER_C, SCHEDULER_E_ID, PRIO_LOW );
  }

  if ( HasEightProcessors() ) {
    TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_D, SCHEDULER_H_ID, PRIO_LOW );
  }
}
#endif

/*
 * With less than eight processors, the priority queue of the ineligible
 * scheduler uses processor 3, which scheduler C owns otherwise.
 */
static bool BorrowsProcessor( const Context *ctx )
{
  return IsSticky( ctx ) && !HasEightProcessors() &&
         ctx->queue_priority[ ROLE_INELIGIBLE ] != PRIO_INVALID;
}

static void BorrowProcessor( Context *ctx )
{
  if ( BorrowsProcessor( ctx ) ) {
    RemoveProcessor( SCHEDULER_C_ID, 3 );
    AddProcessor( SCHEDULER_D_ID, 3 );
    TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_D, SCHEDULER_D_ID, PRIO_LOW );
  }
}

static void ReturnProcessor( Context *ctx )
{
  if ( BorrowsProcessor( ctx ) ) {
    TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_D, SCHEDULER_A_ID, PRIO_LOW );
    RemoveProcessor( SCHEDULER_D_ID, 3 );
    AddProcessor( SCHEDULER_C_ID, 3 );
  }
}

static void SetOrder( Context *ctx, Role a, Role b, Role c, Role d )
{
  ctx->order[ 0 ] = a;
  ctx->order[ 1 ] = b;
  ctx->order[ 2 ] = c;
  ctx->order[ 3 ] = d;
}

static bool HasQueue( const Context *ctx, Role role )
{
  return ctx->queue_priority[ role ] != PRIO_INVALID;
}

static Role LowerRole( const Context *ctx )
{
  size_t i;

  for ( i = 0; i < ROLE_COUNT; ++i ) {
    if ( ctx->with_lower[ i ] ) {
      return (Role) i;
    }
  }

  return ROLE_COUNT;
}

static bool HasLower( const Context *ctx )
{
  return LowerRole( ctx ) != ROLE_COUNT;
}

static void SetLowerScheduler( Context *ctx, rtems_id scheduler_id )
{
#if defined( RTEMS_SMP )
  TQSetScheduler( ctx->tq_ctx, LOWER_WORKER, scheduler_id, PRIO_LOW );
#else
  (void) scheduler_id;
  TQSetPriority( ctx->tq_ctx, LOWER_WORKER, PRIO_LOW );
#endif
}

static void EnqueueLower( Context *ctx )
{
  Role role;

  role = LowerRole( ctx );

  if ( role == ROLE_COUNT ) {
    return;
  }

  SetLowerScheduler( ctx, RoleScheduler( ctx, role ) );
  TQClearDone( ctx->tq_ctx, LOWER_WORKER );
  Enqueue( ctx, LOWER_WORKER, TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER );
  ++ctx->tq_ctx->how_many;
}

static void ReturnLower( Context *ctx )
{
  if ( HasLower( ctx ) ) {
    TQWaitForDone( ctx->tq_ctx, LOWER_WORKER );
    TQWaitForExecutionStop( ctx->tq_ctx, LOWER_WORKER );
    SetLowerScheduler( ctx, SCHEDULER_A_ID );
  }
}

static bool IsEligible( const Context *ctx, Role role )
{
  return role == ROLE_HOME || ( role == ROLE_FIRST && ctx->helping >= 1 ) ||
         ( role == ROLE_SECOND && ctx->helping >= 2 ) ||
         ( role == ROLE_OTHER && ctx->helping >= 3 );
}

/*
 * The snapshot reads the priority queues of the thread queue in the order of
 * the thread queue while the enqueueing thread waits.
 */
static void TakeSnapshot( Context *ctx )
{
  Thread_queue_Queue *queue;

  queue = ctx->tq_ctx->worker_tcb[ TQ_BLOCKER_C ]->Wait.queue;
  T_assert_not_null( queue );
  TQTakeSnapshot( &ctx->snapshot, queue );
}

static size_t FindQueue( const Context *ctx, Role role )
{
  return TQSnapshotFind( &ctx->snapshot, RoleScheduler( ctx, role ) );
}

static void CheckPosition( Context *ctx, Role role, Place place )
{
  const rtems_tcb               *enqueued;
  const rtems_tcb               *waiter;
  const rtems_tcb               *lower;
  const TQPriorityQueueSnapshot *queue;
  size_t                         count;
  size_t                         index;
  size_t                         i;

  enqueued = ctx->tq_ctx->worker_tcb[ TQ_BLOCKER_C ];
  waiter = ctx->tq_ctx->worker_tcb[ role_worker[ role ] ];
  lower = ctx->tq_ctx->worker_tcb[ LOWER_WORKER ];
  count = ctx->with_lower[ role ] ? 3 : 2;
  index = FindQueue( ctx, role );

  if ( place == PLACE_NONE ) {
    if ( index != SIZE_MAX ) {
      T_false( TQSnapshotContains( &ctx->snapshot.queue[ index ], enqueued ) );
    }

    return;
  }

  T_ne_sz( index, SIZE_MAX );

  if ( index == SIZE_MAX ) {
    return;
  }

  queue = &ctx->snapshot.queue[ index ];

  switch ( place ) {
    case PLACE_INITIAL:
      T_eq_sz( queue->count, 1 );
      T_eq_ptr( queue->thread[ 0 ], enqueued );

      for ( i = 0; i < ROLE_COUNT; ++i ) {
        if ( HasQueue( ctx, (Role) i ) ) {
          T_lt_sz( FindQueue( ctx, (Role) i ), index );
        }
      }

      for ( i = ROLE_HOME; i < role; ++i ) {
        if ( IsEligible( ctx, (Role) i ) ) {
          T_lt_sz( FindQueue( ctx, (Role) i ), index );
        }
      }

      break;
    case PLACE_FIRST:
      T_eq_sz( queue->count, count );
      T_eq_ptr( queue->thread[ 0 ], enqueued );
      T_eq_ptr( queue->thread[ 1 ], waiter );
      break;
    default:
      T_eq_int( place, PLACE_SECOND );
      T_eq_sz( queue->count, count );
      T_eq_ptr( queue->thread[ 0 ], waiter );
      T_eq_ptr( queue->thread[ 1 ], enqueued );
      break;
  }

  if ( count == 3 ) {
    T_eq_ptr( queue->thread[ 2 ], lower );
  }
}

static void CheckOrder( Context *ctx )
{
  Role   roles[ ROLE_COUNT ];
  size_t count;
  size_t last;
  size_t i;

  memcpy( roles, ctx->order, sizeof( ctx->order ) );
  roles[ ROLE_ORDER_COUNT ] = ROLE_INELIGIBLE_LAST;
  roles[ ROLE_ORDER_COUNT + 1 ] = ROLE_OTHER;
  count = 0;
  last = 0;

  for ( i = 0; i < ROLE_COUNT; ++i ) {
    Role role;

    role = roles[ i ];

    if ( HasQueue( ctx, role ) ) {
      size_t index;

      index = FindQueue( ctx, role );
      T_ne_sz( index, SIZE_MAX );

      if ( count > 0 ) {
        T_gt_sz( index, last );
      }

      last = index;
      ++count;
    } else if ( IsEligible( ctx, role ) ) {
      ++count;
    }
  }

  T_eq_sz( ctx->snapshot.count, count );
}

static void AddHelper( Context *ctx, size_t index )
{
  TQSendAndWaitForExecutionStop(
    ctx->tq_ctx,
    helper_provider[ index ],
    TQ_EVENT_MUTEX_A_OBTAIN | TQ_EVENT_MUTEX_A_RELEASE
  );
}

static void ScoreTqReqEnqueuePriority_Pre_EligibleScheduler_Prepare(
  ScoreTqReqEnqueuePriority_Context              *ctx,
  ScoreTqReqEnqueuePriority_Pre_EligibleScheduler state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriority_Pre_EligibleScheduler_Home: {
      /*
       * While the enqueueing thread has no helping scheduler.
       */
      ctx->helping = 0;

      if ( IsSticky( ctx ) && !HasFourProcessors() ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_EligibleScheduler_One: {
      /*
       * While the enqueueing thread has exactly one helping scheduler.
       */
      ctx->helping = 1;

      if (
        !HasFourProcessors() && ( IsSticky( ctx ) || !HasTwoProcessors() )
      ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_EligibleScheduler_Two: {
      /*
       * While the enqueueing thread has exactly two helping schedulers.
       */
      ctx->helping = 2;

      if (
        !HasFourProcessors() || ( IsSticky( ctx ) && !HasFiveProcessors() )
      ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_EligibleScheduler_More: {
      /*
       * While the enqueueing thread has at least three helping schedulers,
       * while the thread queue has no priority queue associated with a helping
       * scheduler of the enqueueing thread other than the first and the second
       * helping scheduler.
       */
      ctx->helping = 3;
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_EligibleScheduler_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriority_Pre_QueueHome_Prepare(
  ScoreTqReqEnqueuePriority_Context      *ctx,
  ScoreTqReqEnqueuePriority_Pre_QueueHome state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriority_Pre_QueueHome_Empty: {
      /*
       * While the thread queue has no priority queue associated with the home
       * scheduler of the enqueueing thread.
       */
      ctx->queue_priority[ ROLE_HOME ] = PRIO_INVALID;
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueHome_High: {
      /*
       * While the thread queue has a priority queue associated with the home
       * scheduler of the enqueueing thread, while the highest priority of the
       * priority queue is higher than the eligible priority of the enqueueing
       * thread with respect to this home scheduler.
       */
      switch (
        ScoreTqReqEnqueuePriority_Pre_QueueHome_High_GetRepresentative( ctx )
      ) {
        case ScoreTqReqEnqueuePriority_Pre_QueueHome_High_Alone: {
          ctx->with_lower[ ROLE_HOME ] = false;
          break;
        }

        case ScoreTqReqEnqueuePriority_Pre_QueueHome_High_Lower: {
          if ( HasLower( ctx ) ) {
            ctx->Map.skip_rep = true;
          } else {
            ctx->with_lower[ ROLE_HOME ] = true;
          }
          break;
        }
      }

      ctx->queue_priority[ ROLE_HOME ] = PRIO_VERY_HIGH;

      if ( IsSticky( ctx ) ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueHome_Equal: {
      /*
       * While the thread queue has a priority queue associated with the home
       * scheduler of the enqueueing thread, while the highest priority of the
       * priority queue is equal to the eligible priority of the enqueueing
       * thread with respect to this home scheduler.
       */
      ctx->queue_priority[ ROLE_HOME ] = PRIO_HIGH;
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueHome_Low: {
      /*
       * While the thread queue has a priority queue associated with the home
       * scheduler of the enqueueing thread, while the highest priority of the
       * priority queue is lower than the eligible priority of the enqueueing
       * thread with respect to this home scheduler.
       */
      switch (
        ScoreTqReqEnqueuePriority_Pre_QueueHome_Low_GetRepresentative( ctx )
      ) {
        case ScoreTqReqEnqueuePriority_Pre_QueueHome_Low_Alone: {
          ctx->with_lower[ ROLE_HOME ] = false;
          break;
        }

        case ScoreTqReqEnqueuePriority_Pre_QueueHome_Low_Lower: {
          if ( HasLower( ctx ) ) {
            ctx->Map.skip_rep = true;
          } else {
            ctx->with_lower[ ROLE_HOME ] = true;
          }
          break;
        }
      }

      ctx->queue_priority[ ROLE_HOME ] = PRIO_NORMAL;

      if ( IsSticky( ctx ) ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueHome_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_Prepare(
  ScoreTqReqEnqueuePriority_Context              *ctx,
  ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_Empty: {
      /*
       * While the thread queue has no priority queue associated with the first
       * helping scheduler of the enqueueing thread.
       */
      ctx->queue_priority[ ROLE_FIRST ] = PRIO_INVALID;
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_High: {
      /*
       * While the thread queue has a priority queue associated with the first
       * helping scheduler of the enqueueing thread, while the highest priority
       * of the priority queue is higher than the eligible priority of the
       * enqueueing thread with respect to this first helping scheduler.
       */
      ctx->queue_priority[ ROLE_FIRST ] = PRIO_VERY_HIGH;

      if ( ctx->home_only ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_Equal: {
      /*
       * While the thread queue has a priority queue associated with the first
       * helping scheduler of the enqueueing thread, while the highest priority
       * of the priority queue is equal to the eligible priority of the
       * enqueueing thread with respect to this first helping scheduler.
       */
      switch (
        ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_Equal_GetRepresentative(
          ctx
        )
      ) {
        case ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_Equal_Alone: {
          ctx->with_lower[ ROLE_FIRST ] = false;
          break;
        }

        case ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_Equal_Lower: {
          if ( HasLower( ctx ) ) {
            ctx->Map.skip_rep = true;
          } else {
            ctx->with_lower[ ROLE_FIRST ] = true;
          }
          break;
        }
      }

      ctx->queue_priority[ ROLE_FIRST ] = PRIO_HIGH;

      if ( ctx->home_only || IsSticky( ctx ) ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_Low: {
      /*
       * While the thread queue has a priority queue associated with the first
       * helping scheduler of the enqueueing thread, while the highest priority
       * of the priority queue is lower than the eligible priority of the
       * enqueueing thread with respect to this first helping scheduler.
       */
      switch (
        ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_Low_GetRepresentative(
          ctx
        )
      ) {
        case ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_Low_Alone: {
          ctx->with_lower[ ROLE_FIRST ] = false;
          break;
        }

        case ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_Low_Lower: {
          if ( HasLower( ctx ) ) {
            ctx->Map.skip_rep = true;
          } else {
            ctx->with_lower[ ROLE_FIRST ] = true;
          }
          break;
        }
      }

      ctx->queue_priority[ ROLE_FIRST ] = PRIO_NORMAL;

      if ( ctx->home_only || IsSticky( ctx ) ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_Prepare(
  ScoreTqReqEnqueuePriority_Context               *ctx,
  ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_Empty: {
      /*
       * While the thread queue has no priority queue associated with the
       * second helping scheduler of the enqueueing thread.
       */
      ctx->queue_priority[ ROLE_SECOND ] = PRIO_INVALID;
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_High: {
      /*
       * While the thread queue has a priority queue associated with the second
       * helping scheduler of the enqueueing thread, while the highest priority
       * of the priority queue is higher than the eligible priority of the
       * enqueueing thread with respect to this second helping scheduler.
       */
      ctx->queue_priority[ ROLE_SECOND ] = PRIO_VERY_HIGH;

      if ( ctx->home_only ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_Equal: {
      /*
       * While the thread queue has a priority queue associated with the second
       * helping scheduler of the enqueueing thread, while the highest priority
       * of the priority queue is equal to the eligible priority of the
       * enqueueing thread with respect to this second helping scheduler.
       */
      switch (
        ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_Equal_GetRepresentative(
          ctx
        )
      ) {
        case ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_Equal_Alone: {
          ctx->with_lower[ ROLE_SECOND ] = false;
          break;
        }

        case ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_Equal_Lower: {
          if ( HasLower( ctx ) ) {
            ctx->Map.skip_rep = true;
          } else {
            ctx->with_lower[ ROLE_SECOND ] = true;
          }
          break;
        }
      }

      ctx->queue_priority[ ROLE_SECOND ] = PRIO_HIGH;

      if ( ctx->home_only || IsSticky( ctx ) ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_Low: {
      /*
       * While the thread queue has a priority queue associated with the second
       * helping scheduler of the enqueueing thread, while the highest priority
       * of the priority queue is lower than the eligible priority of the
       * enqueueing thread with respect to this second helping scheduler.
       */
      switch (
        ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_Low_GetRepresentative(
          ctx
        )
      ) {
        case ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_Low_Alone: {
          ctx->with_lower[ ROLE_SECOND ] = false;
          break;
        }

        case ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_Low_Lower: {
          if ( HasLower( ctx ) ) {
            ctx->Map.skip_rep = true;
          } else {
            ctx->with_lower[ ROLE_SECOND ] = true;
          }
          break;
        }
      }

      ctx->queue_priority[ ROLE_SECOND ] = PRIO_NORMAL;

      if ( ctx->home_only || IsSticky( ctx ) ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriority_Pre_QueueIneligible_Prepare(
  ScoreTqReqEnqueuePriority_Context            *ctx,
  ScoreTqReqEnqueuePriority_Pre_QueueIneligible state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriority_Pre_QueueIneligible_None: {
      /*
       * While the thread queue has no priority queue associated with an
       * ineligible scheduler of the enqueueing thread.
       */
      ctx->queue_priority[ ROLE_INELIGIBLE ] = PRIO_INVALID;
      ctx->queue_priority[ ROLE_INELIGIBLE_LAST ] = PRIO_INVALID;
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueIneligible_One: {
      /*
       * While the thread queue has exactly one priority queue associated with
       * an ineligible scheduler of the enqueueing thread.
       */
      ctx->queue_priority[ ROLE_INELIGIBLE ] = PRIO_LOW;
      ctx->queue_priority[ ROLE_INELIGIBLE_LAST ] = PRIO_INVALID;

      if (
        !HasFourProcessors() || ctx->home_only ||
        ( IsSticky( ctx ) && !HasEightProcessors() &&
          HasQueue( ctx, ROLE_HOME ) )
      ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueIneligible_More: {
      /*
       * While the thread queue has at least two priority queues associated
       * with ineligible schedulers of the enqueueing thread, while each of
       * these priority queues except one is positioned after each other
       * priority queue of the thread queue.
       */
      ctx->queue_priority[ ROLE_INELIGIBLE ] = PRIO_LOW;
      ctx->queue_priority[ ROLE_INELIGIBLE_LAST ] = PRIO_LOW;
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueIneligible_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriority_Pre_QueueOrder_Prepare(
  ScoreTqReqEnqueuePriority_Context       *ctx,
  ScoreTqReqEnqueuePriority_Pre_QueueOrder state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriority_Pre_QueueOrder_HomeFirstSecondIneligible: {
      /*
       * While the priority queues of the thread queue associated with the home
       * scheduler, the first helping scheduler, the second helping scheduler,
       * and an ineligible scheduler of the enqueueing thread are positioned in
       * this order, where a priority queue which the thread queue does not
       * contain is left out.
       */
      SetOrder( ctx, ROLE_HOME, ROLE_FIRST, ROLE_SECOND, ROLE_INELIGIBLE );
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueOrder_HomeFirstIneligibleSecond: {
      /*
       * While the priority queues of the thread queue associated with the home
       * scheduler, the first helping scheduler, an ineligible scheduler, and
       * the second helping scheduler of the enqueueing thread are positioned
       * in this order, where a priority queue which the thread queue does not
       * contain is left out.
       */
      SetOrder( ctx, ROLE_HOME, ROLE_FIRST, ROLE_INELIGIBLE, ROLE_SECOND );
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueOrder_HomeSecondFirstIneligible: {
      /*
       * While the priority queues of the thread queue associated with the home
       * scheduler, the second helping scheduler, the first helping scheduler,
       * and an ineligible scheduler of the enqueueing thread are positioned in
       * this order, where a priority queue which the thread queue does not
       * contain is left out.
       */
      SetOrder( ctx, ROLE_HOME, ROLE_SECOND, ROLE_FIRST, ROLE_INELIGIBLE );
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueOrder_HomeSecondIneligibleFirst: {
      /*
       * While the priority queues of the thread queue associated with the home
       * scheduler, the second helping scheduler, an ineligible scheduler, and
       * the first helping scheduler of the enqueueing thread are positioned in
       * this order, where a priority queue which the thread queue does not
       * contain is left out.
       */
      SetOrder( ctx, ROLE_HOME, ROLE_SECOND, ROLE_INELIGIBLE, ROLE_FIRST );
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueOrder_HomeIneligibleFirstSecond: {
      /*
       * While the priority queues of the thread queue associated with the home
       * scheduler, an ineligible scheduler, the first helping scheduler, and
       * the second helping scheduler of the enqueueing thread are positioned
       * in this order, where a priority queue which the thread queue does not
       * contain is left out.
       */
      SetOrder( ctx, ROLE_HOME, ROLE_INELIGIBLE, ROLE_FIRST, ROLE_SECOND );
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueOrder_HomeIneligibleSecondFirst: {
      /*
       * While the priority queues of the thread queue associated with the home
       * scheduler, an ineligible scheduler, the second helping scheduler, and
       * the first helping scheduler of the enqueueing thread are positioned in
       * this order, where a priority queue which the thread queue does not
       * contain is left out.
       */
      SetOrder( ctx, ROLE_HOME, ROLE_INELIGIBLE, ROLE_SECOND, ROLE_FIRST );
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueOrder_FirstHomeSecondIneligible: {
      /*
       * While the priority queues of the thread queue associated with the
       * first helping scheduler, the home scheduler, the second helping
       * scheduler, and an ineligible scheduler of the enqueueing thread are
       * positioned in this order, where a priority queue which the thread
       * queue does not contain is left out.
       */
      SetOrder( ctx, ROLE_FIRST, ROLE_HOME, ROLE_SECOND, ROLE_INELIGIBLE );
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueOrder_FirstHomeIneligibleSecond: {
      /*
       * While the priority queues of the thread queue associated with the
       * first helping scheduler, the home scheduler, an ineligible scheduler,
       * and the second helping scheduler of the enqueueing thread are
       * positioned in this order, where a priority queue which the thread
       * queue does not contain is left out.
       */
      SetOrder( ctx, ROLE_FIRST, ROLE_HOME, ROLE_INELIGIBLE, ROLE_SECOND );
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueOrder_FirstSecondHomeIneligible: {
      /*
       * While the priority queues of the thread queue associated with the
       * first helping scheduler, the second helping scheduler, the home
       * scheduler, and an ineligible scheduler of the enqueueing thread are
       * positioned in this order, where a priority queue which the thread
       * queue does not contain is left out.
       */
      SetOrder( ctx, ROLE_FIRST, ROLE_SECOND, ROLE_HOME, ROLE_INELIGIBLE );
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueOrder_FirstSecondIneligibleHome: {
      /*
       * While the priority queues of the thread queue associated with the
       * first helping scheduler, the second helping scheduler, an ineligible
       * scheduler, and the home scheduler of the enqueueing thread are
       * positioned in this order, where a priority queue which the thread
       * queue does not contain is left out.
       */
      SetOrder( ctx, ROLE_FIRST, ROLE_SECOND, ROLE_INELIGIBLE, ROLE_HOME );
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueOrder_FirstIneligibleHomeSecond: {
      /*
       * While the priority queues of the thread queue associated with the
       * first helping scheduler, an ineligible scheduler, the home scheduler,
       * and the second helping scheduler of the enqueueing thread are
       * positioned in this order, where a priority queue which the thread
       * queue does not contain is left out.
       */
      SetOrder( ctx, ROLE_FIRST, ROLE_INELIGIBLE, ROLE_HOME, ROLE_SECOND );
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueOrder_FirstIneligibleSecondHome: {
      /*
       * While the priority queues of the thread queue associated with the
       * first helping scheduler, an ineligible scheduler, the second helping
       * scheduler, and the home scheduler of the enqueueing thread are
       * positioned in this order, where a priority queue which the thread
       * queue does not contain is left out.
       */
      SetOrder( ctx, ROLE_FIRST, ROLE_INELIGIBLE, ROLE_SECOND, ROLE_HOME );
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueOrder_SecondHomeFirstIneligible: {
      /*
       * While the priority queues of the thread queue associated with the
       * second helping scheduler, the home scheduler, the first helping
       * scheduler, and an ineligible scheduler of the enqueueing thread are
       * positioned in this order, where a priority queue which the thread
       * queue does not contain is left out.
       */
      SetOrder( ctx, ROLE_SECOND, ROLE_HOME, ROLE_FIRST, ROLE_INELIGIBLE );
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueOrder_SecondHomeIneligibleFirst: {
      /*
       * While the priority queues of the thread queue associated with the
       * second helping scheduler, the home scheduler, an ineligible scheduler,
       * and the first helping scheduler of the enqueueing thread are
       * positioned in this order, where a priority queue which the thread
       * queue does not contain is left out.
       */
      SetOrder( ctx, ROLE_SECOND, ROLE_HOME, ROLE_INELIGIBLE, ROLE_FIRST );
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueOrder_SecondFirstHomeIneligible: {
      /*
       * While the priority queues of the thread queue associated with the
       * second helping scheduler, the first helping scheduler, the home
       * scheduler, and an ineligible scheduler of the enqueueing thread are
       * positioned in this order, where a priority queue which the thread
       * queue does not contain is left out.
       */
      SetOrder( ctx, ROLE_SECOND, ROLE_FIRST, ROLE_HOME, ROLE_INELIGIBLE );
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueOrder_SecondFirstIneligibleHome: {
      /*
       * While the priority queues of the thread queue associated with the
       * second helping scheduler, the first helping scheduler, an ineligible
       * scheduler, and the home scheduler of the enqueueing thread are
       * positioned in this order, where a priority queue which the thread
       * queue does not contain is left out.
       */
      SetOrder( ctx, ROLE_SECOND, ROLE_FIRST, ROLE_INELIGIBLE, ROLE_HOME );
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueOrder_SecondIneligibleHomeFirst: {
      /*
       * While the priority queues of the thread queue associated with the
       * second helping scheduler, an ineligible scheduler, the home scheduler,
       * and the first helping scheduler of the enqueueing thread are
       * positioned in this order, where a priority queue which the thread
       * queue does not contain is left out.
       */
      SetOrder( ctx, ROLE_SECOND, ROLE_INELIGIBLE, ROLE_HOME, ROLE_FIRST );
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueOrder_SecondIneligibleFirstHome: {
      /*
       * While the priority queues of the thread queue associated with the
       * second helping scheduler, an ineligible scheduler, the first helping
       * scheduler, and the home scheduler of the enqueueing thread are
       * positioned in this order, where a priority queue which the thread
       * queue does not contain is left out.
       */
      SetOrder( ctx, ROLE_SECOND, ROLE_INELIGIBLE, ROLE_FIRST, ROLE_HOME );
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueOrder_IneligibleHomeFirstSecond: {
      /*
       * While the priority queues of the thread queue associated with an
       * ineligible scheduler, the home scheduler, the first helping scheduler,
       * and the second helping scheduler of the enqueueing thread are
       * positioned in this order, where a priority queue which the thread
       * queue does not contain is left out.
       */
      SetOrder( ctx, ROLE_INELIGIBLE, ROLE_HOME, ROLE_FIRST, ROLE_SECOND );
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueOrder_IneligibleHomeSecondFirst: {
      /*
       * While the priority queues of the thread queue associated with an
       * ineligible scheduler, the home scheduler, the second helping
       * scheduler, and the first helping scheduler of the enqueueing thread
       * are positioned in this order, where a priority queue which the thread
       * queue does not contain is left out.
       */
      SetOrder( ctx, ROLE_INELIGIBLE, ROLE_HOME, ROLE_SECOND, ROLE_FIRST );
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueOrder_IneligibleFirstHomeSecond: {
      /*
       * While the priority queues of the thread queue associated with an
       * ineligible scheduler, the first helping scheduler, the home scheduler,
       * and the second helping scheduler of the enqueueing thread are
       * positioned in this order, where a priority queue which the thread
       * queue does not contain is left out.
       */
      SetOrder( ctx, ROLE_INELIGIBLE, ROLE_FIRST, ROLE_HOME, ROLE_SECOND );
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueOrder_IneligibleFirstSecondHome: {
      /*
       * While the priority queues of the thread queue associated with an
       * ineligible scheduler, the first helping scheduler, the second helping
       * scheduler, and the home scheduler of the enqueueing thread are
       * positioned in this order, where a priority queue which the thread
       * queue does not contain is left out.
       */
      SetOrder( ctx, ROLE_INELIGIBLE, ROLE_FIRST, ROLE_SECOND, ROLE_HOME );
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueOrder_IneligibleSecondHomeFirst: {
      /*
       * While the priority queues of the thread queue associated with an
       * ineligible scheduler, the second helping scheduler, the home
       * scheduler, and the first helping scheduler of the enqueueing thread
       * are positioned in this order, where a priority queue which the thread
       * queue does not contain is left out.
       */
      SetOrder( ctx, ROLE_INELIGIBLE, ROLE_SECOND, ROLE_HOME, ROLE_FIRST );
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueOrder_IneligibleSecondFirstHome: {
      /*
       * While the priority queues of the thread queue associated with an
       * ineligible scheduler, the second helping scheduler, the first helping
       * scheduler, and the home scheduler of the enqueueing thread are
       * positioned in this order, where a priority queue which the thread
       * queue does not contain is left out.
       */
      SetOrder( ctx, ROLE_INELIGIBLE, ROLE_SECOND, ROLE_FIRST, ROLE_HOME );
      break;
    }

    case ScoreTqReqEnqueuePriority_Pre_QueueOrder_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriority_Post_PositionHome_Check(
  ScoreTqReqEnqueuePriority_Context          *ctx,
  ScoreTqReqEnqueuePriority_Post_PositionHome state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriority_Post_PositionHome_Initial: {
      /*
       * A priority queue which contains exactly the enqueueing thread and
       * which is associated with the home scheduler of the enqueueing thread
       * shall be positioned after each priority queue which the thread queue
       * contained before the enqueue.
       */
      CheckPosition( ctx, ROLE_HOME, PLACE_INITIAL );
      break;
    }

    case ScoreTqReqEnqueuePriority_Post_PositionHome_First: {
      /*
       * The enqueueing thread shall be enqueued as the first thread of the
       * priority queue associated with the home scheduler of the enqueueing
       * thread.
       */
      CheckPosition( ctx, ROLE_HOME, PLACE_FIRST );
      break;
    }

    case ScoreTqReqEnqueuePriority_Post_PositionHome_Second: {
      /*
       * The enqueueing thread shall be enqueued in the priority queue
       * associated with the home scheduler of the enqueueing thread after each
       * thread of the priority queue with a priority higher than or equal to
       * the eligible priority of the enqueueing thread with respect to this
       * home scheduler and before each thread of the priority queue with a
       * lower priority.
       */
      CheckPosition( ctx, ROLE_HOME, PLACE_SECOND );
      break;
    }

    case ScoreTqReqEnqueuePriority_Post_PositionHome_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Check(
  ScoreTqReqEnqueuePriority_Context                  *ctx,
  ScoreTqReqEnqueuePriority_Post_PositionFirstHelping state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Absent: {
      /*
       * The enqueueing thread shall not be enqueued in a priority queue
       * associated with the first helping scheduler of the enqueueing thread.
       */
      CheckPosition( ctx, ROLE_FIRST, PLACE_NONE );
      break;
    }

    case ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Initial: {
      /*
       * A priority queue which contains exactly the enqueueing thread and
       * which is associated with the first helping scheduler of the enqueueing
       * thread shall be positioned after each priority queue which the thread
       * queue contained before the enqueue and after the priority queue
       * associated with the home scheduler of the enqueueing thread.
       */
      CheckPosition( ctx, ROLE_FIRST, PLACE_INITIAL );
      break;
    }

    case ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_First: {
      /*
       * The enqueueing thread shall be enqueued as the first thread of the
       * priority queue associated with the first helping scheduler of the
       * enqueueing thread.
       */
      CheckPosition( ctx, ROLE_FIRST, PLACE_FIRST );
      break;
    }

    case ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Second: {
      /*
       * The enqueueing thread shall be enqueued in the priority queue
       * associated with the first helping scheduler of the enqueueing thread
       * after each thread of the priority queue with a priority higher than or
       * equal to the eligible priority of the enqueueing thread with respect
       * to this first helping scheduler and before each thread of the priority
       * queue with a lower priority.
       */
      CheckPosition( ctx, ROLE_FIRST, PLACE_SECOND );
      break;
    }

    case ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Check(
  ScoreTqReqEnqueuePriority_Context                   *ctx,
  ScoreTqReqEnqueuePriority_Post_PositionSecondHelping state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Absent: {
      /*
       * The enqueueing thread shall not be enqueued in a priority queue
       * associated with the second helping scheduler of the enqueueing thread.
       */
      CheckPosition( ctx, ROLE_SECOND, PLACE_NONE );
      break;
    }

    case ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Initial: {
      /*
       * A priority queue which contains exactly the enqueueing thread and
       * which is associated with the second helping scheduler of the
       * enqueueing thread shall be positioned after each priority queue which
       * the thread queue contained before the enqueue and after the priority
       * queues associated with the home scheduler and the first helping
       * scheduler of the enqueueing thread.
       */
      CheckPosition( ctx, ROLE_SECOND, PLACE_INITIAL );
      break;
    }

    case ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_First: {
      /*
       * The enqueueing thread shall be enqueued as the first thread of the
       * priority queue associated with the second helping scheduler of the
       * enqueueing thread.
       */
      CheckPosition( ctx, ROLE_SECOND, PLACE_FIRST );
      break;
    }

    case ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Second: {
      /*
       * The enqueueing thread shall be enqueued in the priority queue
       * associated with the second helping scheduler of the enqueueing thread
       * after each thread of the priority queue with a priority higher than or
       * equal to the eligible priority of the enqueueing thread with respect
       * to this second helping scheduler and before each thread of the
       * priority queue with a lower priority.
       */
      CheckPosition( ctx, ROLE_SECOND, PLACE_SECOND );
      break;
    }

    case ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Check(
  ScoreTqReqEnqueuePriority_Context                  *ctx,
  ScoreTqReqEnqueuePriority_Post_PositionOtherHelping state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent: {
      /*
       * The enqueueing thread shall not be enqueued in a priority queue
       * associated with a helping scheduler of the enqueueing thread other
       * than the first and the second helping scheduler.
       */
      CheckPosition( ctx, ROLE_OTHER, PLACE_NONE );
      break;
    }

    case ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Initial: {
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
      CheckPosition( ctx, ROLE_OTHER, PLACE_INITIAL );
      break;
    }

    case ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriority_Post_QueueOrder_Check(
  ScoreTqReqEnqueuePriority_Context        *ctx,
  ScoreTqReqEnqueuePriority_Post_QueueOrder state
)
{
  switch ( state ) {
    case ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept: {
      /*
       * The order of the priority queues which the thread queue contained
       * before the enqueue shall not change.
       */
      CheckOrder( ctx );
      break;
    }

    case ScoreTqReqEnqueuePriority_Post_QueueOrder_NA:
      break;
  }
}

static void ScoreTqReqEnqueuePriority_Setup(
  ScoreTqReqEnqueuePriority_Context *ctx
)
{
  TQReset( ctx->tq_ctx );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_A, PRIO_VERY_HIGH );
  TQSetPriority( ctx->tq_ctx, TQ_BLOCKER_C, PRIO_HIGH );

  #if defined( RTEMS_SMP )
  if ( IsSticky( ctx ) ) {
    if ( HasFourProcessors() ) {
      SetupSticky( ctx );
    }
  } else if ( HasFourProcessors() ) {
    RemoveProcessor( SCHEDULER_C_ID, 3 );
    AddProcessor( SCHEDULER_D_ID, 3 );
    TQSetScheduler( ctx->tq_ctx, TQ_WORKER_F, SCHEDULER_C_ID, PRIO_LOW );
    TQSetScheduler( ctx->tq_ctx, TQ_HELPER_C, SCHEDULER_B_ID, PRIO_LOW );
    TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_D, SCHEDULER_D_ID, PRIO_LOW );
    TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_E, SCHEDULER_C_ID, PRIO_HIGH );
    TQSetScheduler( ctx->tq_ctx, TQ_HELPER_A, SCHEDULER_B_ID, PRIO_HIGH );
  } else if ( HasTwoProcessors() ) {
    TQSetScheduler( ctx->tq_ctx, TQ_WORKER_F, SCHEDULER_B_ID, PRIO_LOW );
    TQSetScheduler( ctx->tq_ctx, TQ_BLOCKER_E, SCHEDULER_B_ID, PRIO_HIGH );
  }
  #endif
}

static void ScoreTqReqEnqueuePriority_Setup_Wrap( void *arg )
{
  ScoreTqReqEnqueuePriority_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqEnqueuePriority_Setup( ctx );
}

static void ScoreTqReqEnqueuePriority_Teardown(
  ScoreTqReqEnqueuePriority_Context *ctx
)
{
  TQReset( ctx->tq_ctx );

  #if defined( RTEMS_SMP )
  TQSetScheduler( ctx->tq_ctx, TQ_WORKER_F, SCHEDULER_A_ID, PRIO_LOW );

  if ( HasFourProcessors() && !IsSticky( ctx ) ) {
    RemoveProcessor( SCHEDULER_D_ID, 3 );
    AddProcessor( SCHEDULER_C_ID, 3 );
  }
  #endif
}

static void ScoreTqReqEnqueuePriority_Teardown_Wrap( void *arg )
{
  ScoreTqReqEnqueuePriority_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  ScoreTqReqEnqueuePriority_Teardown( ctx );
}

static void ScoreTqReqEnqueuePriority_Prepare(
  ScoreTqReqEnqueuePriority_Context *ctx
)
{
  ctx->queue_priority[ ROLE_OTHER ] = PRIO_INVALID;
  memset( ctx->with_lower, 0, sizeof( ctx->with_lower ) );
}

static void ScoreTqReqEnqueuePriority_Action(
  ScoreTqReqEnqueuePriority_Context *ctx
)
{
  rtems_event_set events;
  size_t          i;

  SetSelfPriority( PRIO_VERY_LOW );
  ctx->tq_ctx->how_many = 1;
  BorrowProcessor( ctx );

  if ( IsSticky( ctx ) ) {
    TQEnqueuePrepare( ctx->tq_ctx );
  } else {
    TQSend( ctx->tq_ctx, TQ_BLOCKER_A, TQ_EVENT_ENQUEUE_PREPARE );
  }

  if ( ctx->helping > 0 ) {
    TQSend( ctx->tq_ctx, TQ_BLOCKER_C, TQ_EVENT_MUTEX_A_OBTAIN );

    for ( i = 0; i < ctx->helping; ++i ) {
      AddHelper( ctx, i );
    }
  }

  for ( i = 0; i <= ROLE_ORDER_COUNT; ++i ) {
    Role role;

    if ( i < ROLE_ORDER_COUNT ) {
      role = ctx->order[ i ];
    } else {
      role = ROLE_INELIGIBLE_LAST;
    }

    if ( HasQueue( ctx, role ) ) {
      TQWorkerKind worker;

      worker = role_worker[ role ];
      TQSetPriority( ctx->tq_ctx, worker, ctx->queue_priority[ role ] );
      TQClearDone( ctx->tq_ctx, worker );
      Enqueue( ctx, worker, TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER );
      ++ctx->tq_ctx->how_many;
    }
  }

  EnqueueLower( ctx );
  events = TQ_EVENT_ENQUEUE | TQ_EVENT_SURRENDER;

  if ( ctx->helping > 0 ) {
    events |= TQ_EVENT_MUTEX_A_RELEASE;
  }

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

  for ( i = 0; i < ROLE_COUNT; ++i ) {
    if ( HasQueue( ctx, (Role) i ) ) {
      TQWaitForDone( ctx->tq_ctx, role_worker[ i ] );
      TQWaitForExecutionStop( ctx->tq_ctx, role_worker[ i ] );
    }
  }

  TQWaitForExecutionStop( ctx->tq_ctx, TQ_BLOCKER_C );
  ReturnLower( ctx );

  if ( ctx->helping > 0 ) {
    TQMutexObtain( ctx->tq_ctx, TQ_MUTEX_A );
    TQMutexRelease( ctx->tq_ctx, TQ_MUTEX_A );
  }

  ReturnProcessor( ctx );

  SetSelfPriority( PRIO_NORMAL );
}

/* clang-format off */

static const ScoreTqReqEnqueuePriority_Entry
ScoreTqReqEnqueuePriority_Entries[] = {
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#if defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_Second,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Second,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Second,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_Second,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Second,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_First,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_Second,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_First,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Second,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_First,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Second,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Second,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_Second,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_First,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_First,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_First,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Second,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_First,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_First,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_First,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Second,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_Second,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Second,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Second,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Second,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_Second,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Second,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_Second,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Second,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_First,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_First,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_First,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_Second,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_First,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_First,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Second,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Second,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_First,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_First,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Second,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_Second,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_First,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_Second,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_First,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_First,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Second,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_First,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Second,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_First,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_First,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_First,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_First,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_First,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_First,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_First,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_First,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Second,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_Second,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Second,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Second,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_Second,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_Second,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_First,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_First,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_First,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_First,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_First,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_Second,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_First,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_Initial,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept },
#else
  { 1, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_NA,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_NA },
#endif
  { 0, 0, 0, 0, 0, 0, 0, ScoreTqReqEnqueuePriority_Post_PositionHome_First,
    ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
    ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept }
};

static const uint8_t
ScoreTqReqEnqueuePriority_Map[] = {
  44, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 45, 2, 2, 2, 2, 2, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 40, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 2, 2, 2, 2, 34, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 34, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 40, 8, 8, 8,
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 2, 2, 2, 2, 34,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 34, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 46, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 8, 2, 2, 2, 2, 41, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 41, 2,
  2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 42, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 42, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2,
  29, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 29, 2, 2, 2, 2, 2, 2, 2, 2, 2, 29, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2,
  2, 2, 2, 2, 29, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 29, 2, 2, 2, 2, 2, 2, 2, 2, 2, 29, 2, 2, 2, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 2, 2, 2, 2, 2, 2, 35, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 35, 2, 2, 2, 2, 2, 2, 2, 2, 2, 35, 2, 2, 2,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 30, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 30, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 30, 2,
  2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 2, 2, 2, 2, 2, 12, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 12, 2, 2, 12, 2, 2, 12, 2, 2, 12, 2, 2, 2, 2,
  2, 2, 2, 12, 2, 12, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 2, 2, 2, 2, 2, 12, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 12, 2, 2, 12, 2, 2, 12, 2, 2,
  12, 2, 2, 2, 2, 2, 2, 2, 12, 2, 12, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 17, 2, 2, 2,
  2, 2, 17, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 17, 2, 2, 17,
  2, 2, 17, 2, 2, 17, 2, 2, 2, 2, 2, 2, 2, 17, 2, 17, 2, 2, 2, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 30, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 30, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 30, 2, 2, 2, 2, 2, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 12, 2, 2, 2, 2, 2, 12, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 12, 2, 2, 12, 2, 2, 12, 2, 2, 12, 2, 2, 2, 2, 2, 2, 2, 12, 2,
  12, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 2, 2, 2, 2, 2, 12, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 12, 2, 2, 12, 2, 2, 12, 2, 2, 12, 2, 2, 2,
  2, 2, 2, 2, 12, 2, 12, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 17, 2, 2, 2, 2, 2, 17, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 17, 2, 2, 17, 2, 2, 17, 2,
  2, 17, 2, 2, 2, 2, 2, 2, 2, 17, 2, 17, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 36, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  36, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 36, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 18, 2, 2, 2, 2, 2, 18, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 18, 2, 2, 18, 2, 2, 18, 2, 2, 18, 2, 2, 2, 2, 2, 2, 2, 18, 2, 18, 2, 2, 2,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 18, 2, 2, 2, 2, 2, 18, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 18, 2, 2, 18, 2, 2, 18, 2, 2, 18, 2, 2, 2, 2, 2, 2, 2, 18,
  2, 18, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 2, 2, 2, 2, 2, 25, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 25, 2, 2, 25, 2, 2, 25, 2, 2, 25, 2, 2,
  2, 2, 2, 2, 2, 25, 2, 25, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 43, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  31, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 31, 2, 2, 2, 2, 2, 31, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 31, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  31, 2, 2, 2, 2, 2, 31, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 37, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 37, 2, 2,
  2, 2, 2, 37, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 32, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 32, 2, 2, 2, 2, 2, 2, 2, 2, 2, 32, 2,
  2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  2, 2, 2, 2, 2, 2, 2, 2, 13, 2, 2, 2, 2, 2, 13, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 13, 2, 13, 2, 2, 2, 13, 2, 13, 2, 2, 2, 13, 2, 13, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2,
  2, 2, 2, 2, 2, 13, 2, 2, 2, 2, 2, 13, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 13, 2, 13, 2, 2, 2, 13, 2, 13, 2, 2, 2, 13, 2, 13, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2,
  2, 2, 19, 2, 2, 2, 2, 2, 19, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 19, 2, 19, 2, 2, 2, 19, 2, 19, 2, 2, 2, 19, 2, 19, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 32, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  32, 2, 2, 2, 2, 2, 2, 2, 2, 2, 32, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 13, 2, 2, 2,
  2, 2, 13, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 13, 2, 13, 2,
  2, 2, 13, 2, 13, 2, 2, 2, 13, 2, 13, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 13, 2, 2, 2, 2, 2,
  13, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 13, 2, 13, 2, 2, 2,
  13, 2, 13, 2, 2, 2, 13, 2, 13, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 19, 2, 2, 2, 2, 2, 19, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 19, 2, 19, 2, 2, 2, 19, 2,
  19, 2, 2, 2, 19, 2, 19, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 38, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 38, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  38, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 20, 2, 2, 2, 2, 2, 20, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 20, 2, 20, 2, 2, 2, 20, 2, 20, 2, 2, 2, 20,
  2, 20, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 2, 2, 2, 2, 2, 2, 2, 2, 20, 2, 2, 2, 2, 2, 20, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 20, 2, 20, 2, 2, 2, 20, 2, 20, 2, 2, 2, 20, 2, 20,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2,
  2, 2, 2, 2, 2, 2, 26, 2, 2, 2, 2, 2, 26, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 26, 2, 26, 2, 2, 2, 26, 2, 26, 2, 2, 2, 26, 2, 26, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 33, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 33,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 33, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 14, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 14, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 14, 2, 14, 2, 2,
  2, 2, 2, 2, 2, 14, 2, 2, 14, 2, 2, 14, 2, 2, 14, 2, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 14, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 14, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 14, 2, 14, 2, 2, 2, 2,
  2, 2, 2, 14, 2, 2, 14, 2, 2, 14, 2, 2, 14, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 21, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 21, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 21, 2, 21, 2, 2, 2, 2, 2, 2,
  2, 21, 2, 2, 21, 2, 2, 21, 2, 2, 21, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 15, 2, 2, 2, 2, 2, 15, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 15, 2, 2, 15, 2, 2, 15, 2, 2, 15, 2, 2,
  2, 2, 2, 2, 2, 15, 2, 15, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 2, 4, 2, 2, 2, 4, 2, 4, 2, 2, 2, 4, 2, 4, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 4, 2, 4, 2, 2, 2, 4, 2, 4, 2, 2, 2, 4, 2, 4, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 5, 2,
  5, 2, 2, 2, 5, 2, 5, 2, 2, 2, 5, 2, 5, 2, 2, 2, 2, 2, 2, 2, 2, 2, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 15, 2, 2, 2, 2, 2, 15,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 15, 2, 2, 15, 2, 2, 15,
  2, 2, 15, 2, 2, 2, 2, 2, 2, 2, 15, 2, 15, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 2, 4, 2, 2, 2, 4, 2, 4, 2, 2,
  2, 4, 2, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 2, 4, 2, 2, 2, 4, 2, 4, 2, 2, 2, 4, 2, 4, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 5, 2, 5, 2, 2, 2, 5, 2, 5, 2, 2, 2, 5, 2, 5, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 22, 2,
  2, 2, 2, 2, 22, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 22, 2,
  2, 22, 2, 2, 22, 2, 2, 22, 2, 2, 2, 2, 2, 2, 2, 22, 2, 22, 2, 2, 2, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 6, 2, 6, 2, 2,
  2, 6, 2, 6, 2, 2, 2, 6, 2, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 6, 2, 6, 2, 2, 2, 6, 2, 6, 2, 2,
  2, 6, 2, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 9, 2, 9, 2, 2, 2, 9, 2, 9, 2, 2, 2, 9, 2, 9, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
  9, 9, 9, 9, 9, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 33, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 33, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 33, 2, 2, 2, 2,
  2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2,
  2, 14, 2, 2, 2, 2, 2, 2, 2, 2, 2, 14, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 14, 2, 14, 2, 2, 2, 2, 2, 2, 2, 14, 2, 2, 14, 2, 2, 14, 2, 2, 14, 2, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 14,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 14, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 14,
  2, 14, 2, 2, 2, 2, 2, 2, 2, 14, 2, 2, 14, 2, 2, 14, 2, 2, 14, 2, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 21, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 21, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 21, 2, 21,
  2, 2, 2, 2, 2, 2, 2, 21, 2, 2, 21, 2, 2, 21, 2, 2, 21, 2, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 15, 2, 2, 2, 2, 2, 15,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 15, 2, 2, 15, 2, 2, 15,
  2, 2, 15, 2, 2, 2, 2, 2, 2, 2, 15, 2, 15, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 2, 4, 2, 2, 2, 4, 2, 4, 2, 2,
  2, 4, 2, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 2, 4, 2, 2, 2, 4, 2, 4, 2, 2, 2, 4, 2, 4, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 5, 2, 5, 2, 2, 2, 5, 2, 5, 2, 2, 2, 5, 2, 5, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 15, 2,
  2, 2, 2, 2, 15, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 15, 2,
  2, 15, 2, 2, 15, 2, 2, 15, 2, 2, 2, 2, 2, 2, 2, 15, 2, 15, 2, 2, 2, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 2, 4, 2, 2,
  2, 4, 2, 4, 2, 2, 2, 4, 2, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 2, 4, 2, 2, 2, 4, 2, 4, 2, 2,
  2, 4, 2, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 5, 2, 5, 2, 2, 2, 5, 2, 5, 2, 2, 2, 5, 2, 5, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 5, 5, 5, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 22, 2, 2, 2, 2, 2, 22, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 22, 2, 2, 22, 2, 2, 22, 2, 2, 22, 2, 2, 2, 2, 2, 2, 2, 22, 2, 22, 2,
  2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  6, 2, 6, 2, 2, 2, 6, 2, 6, 2, 2, 2, 6, 2, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 6, 2, 6, 2, 2, 2,
  6, 2, 6, 2, 2, 2, 6, 2, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 6, 6, 6, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 9, 2, 9, 2, 2, 2, 9, 2, 9, 2, 2, 2,
  9, 2, 9, 2, 2, 2, 2, 2, 2, 2, 2, 2, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
  9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 39, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 39, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  39, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 2, 2, 23, 2, 2, 2, 2, 2, 2, 2, 2, 2, 23, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 23, 2, 23, 2, 2, 2, 2, 2, 2, 2, 23, 2, 2, 23, 2, 2, 23,
  2, 2, 23, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 2, 2, 23, 2, 2, 2, 2, 2, 2, 2, 2, 2, 23, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 23, 2, 23, 2, 2, 2, 2, 2, 2, 2, 23, 2, 2, 23, 2, 2, 23, 2, 2,
  23, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 2, 2, 27, 2, 2, 2, 2, 2, 2, 2, 2, 2, 27, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 27, 2, 27, 2, 2, 2, 2, 2, 2, 2, 27, 2, 2, 27, 2, 2, 27, 2, 2, 27, 2,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 24,
  2, 2, 2, 2, 2, 24, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 24,
  2, 2, 24, 2, 2, 24, 2, 2, 24, 2, 2, 2, 2, 2, 2, 2, 24, 2, 24, 2, 2, 2, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 2, 7, 2,
  2, 2, 7, 2, 7, 2, 2, 2, 7, 2, 7, 2, 2, 2, 2, 2, 2, 2, 2, 2, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 2, 7, 2, 2, 2, 7, 2, 7, 2,
  2, 2, 7, 2, 7, 2, 2, 2, 2, 2, 2, 2, 2, 2, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 10, 2, 10, 2, 2, 2, 10, 2, 10, 2, 2, 2, 10, 2,
  10, 2, 2, 2, 2, 2, 2, 2, 2, 2, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 24, 2, 2, 2, 2, 2, 24, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 24, 2, 2, 24, 2, 2, 24, 2, 2,
  24, 2, 2, 2, 2, 2, 2, 2, 24, 2, 24, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 2, 7, 2, 2, 2, 7, 2, 7, 2, 2, 2, 7,
  2, 7, 2, 2, 2, 2, 2, 2, 2, 2, 2, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 7, 2, 7, 2, 2, 2, 7, 2, 7, 2, 2, 2, 7, 2, 7, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  7, 7, 7, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 10, 2, 10, 2, 2, 2, 10, 2, 10, 2, 2, 2, 10, 2, 10, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
  10, 10, 10, 10, 10, 10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 28, 2, 2, 2, 2, 2, 28, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 28, 2, 2, 28, 2, 2, 28, 2, 2, 28, 2, 2, 2, 2, 2, 2, 2, 28,
  2, 28, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 11, 2, 11, 2, 2, 2, 11, 2, 11, 2, 2, 2, 11, 2, 11, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
  11, 11, 11, 11, 11, 11, 11, 11, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 11, 2, 11, 2, 2, 2, 11, 2, 11, 2, 2, 2, 11, 2, 11,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
  11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 16, 2, 16, 2, 2, 2, 16, 2, 16, 2,
  2, 2, 16, 2, 16, 2, 2, 2, 2, 2, 2, 2, 2, 2, 16, 16, 16, 16, 16, 16, 16, 16,
  16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

/* clang-format on */

static size_t ScoreTqReqEnqueuePriority_Scope( void *arg, char *buf, size_t n )
{
  ScoreTqReqEnqueuePriority_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    size_t pcd[ 6 ];
    size_t i;

    for ( i = 0; i < 6; ++i ) {
      pcd[ i ] = ScoreTqReqEnqueuePriority_PreRepOffset[ i ]
                                                       [ ctx->Map.pcs[ i ] ] +
                 ScoreTqReqEnqueuePriority_GetRepresentative( ctx, i );
    }

    return T_get_scope( ScoreTqReqEnqueuePriority_PreDesc, buf, n, pcd );
  }

  return 0;
}

static T_fixture ScoreTqReqEnqueuePriority_Fixture = {
  .setup = ScoreTqReqEnqueuePriority_Setup_Wrap,
  .stop = NULL,
  .teardown = ScoreTqReqEnqueuePriority_Teardown_Wrap,
  .scope = ScoreTqReqEnqueuePriority_Scope,
  .initial_context = &ScoreTqReqEnqueuePriority_Instance
};

static const uint16_t ScoreTqReqEnqueuePriority_Weights[] =
  { 4608, 1152, 288, 72, 24, 1 };

static void ScoreTqReqEnqueuePriority_Skip(
  ScoreTqReqEnqueuePriority_Context *ctx,
  size_t                             index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pcs[ 1 ] = ScoreTqReqEnqueuePriority_Pre_QueueHome_NA - 1;
      /* Fall through */
    case 2:
      ctx->Map.pcs[ 2 ] = ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_NA -
                          1;
      /* Fall through */
    case 3:
      ctx->Map.pcs[ 3 ] = ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_NA -
                          1;
      /* Fall through */
    case 4:
      ctx->Map.pcs[ 4 ] = ScoreTqReqEnqueuePriority_Pre_QueueIneligible_NA - 1;
      /* Fall through */
    case 5:
      ctx->Map.pcs[ 5 ] = ScoreTqReqEnqueuePriority_Pre_QueueOrder_NA - 1;
      break;
  }
}

static inline ScoreTqReqEnqueuePriority_Entry
ScoreTqReqEnqueuePriority_PopEntry( ScoreTqReqEnqueuePriority_Context *ctx )
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 6; ++i ) {
      index += ScoreTqReqEnqueuePriority_Weights[ i ] * ctx->Map.pcs[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return ScoreTqReqEnqueuePriority_Entries
    [ ScoreTqReqEnqueuePriority_Map[ index ] ];
}

static void ScoreTqReqEnqueuePriority_TestVariant(
  ScoreTqReqEnqueuePriority_Context *ctx
)
{
  ScoreTqReqEnqueuePriority_Pre_EligibleScheduler_Prepare(
    ctx,
    ctx->Map.pcs[ 0 ]
  );

  if ( ctx->Map.skip ) {
    ScoreTqReqEnqueuePriority_Skip( ctx, 0 );
    return;
  }

  ScoreTqReqEnqueuePriority_Pre_QueueHome_Prepare( ctx, ctx->Map.pcs[ 1 ] );

  if ( ctx->Map.skip ) {
    ScoreTqReqEnqueuePriority_Skip( ctx, 1 );
    return;
  }

  ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_Prepare(
    ctx,
    ctx->Map.pcs[ 2 ]
  );

  if ( ctx->Map.skip ) {
    ScoreTqReqEnqueuePriority_Skip( ctx, 2 );
    return;
  }

  ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_Prepare(
    ctx,
    ctx->Map.pcs[ 3 ]
  );

  if ( ctx->Map.skip ) {
    ScoreTqReqEnqueuePriority_Skip( ctx, 3 );
    return;
  }

  ScoreTqReqEnqueuePriority_Pre_QueueIneligible_Prepare(
    ctx,
    ctx->Map.pcs[ 4 ]
  );

  if ( ctx->Map.skip ) {
    ScoreTqReqEnqueuePriority_Skip( ctx, 4 );
    return;
  }

  ScoreTqReqEnqueuePriority_Pre_QueueOrder_Prepare( ctx, ctx->Map.pcs[ 5 ] );

  if ( ctx->Map.skip_rep ) {
    return;
  }

  ScoreTqReqEnqueuePriority_Action( ctx );
  ScoreTqReqEnqueuePriority_Post_PositionHome_Check(
    ctx,
    ctx->Map.entry.Post_PositionHome
  );
  ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Check(
    ctx,
    ctx->Map.entry.Post_PositionFirstHelping
  );
  ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Check(
    ctx,
    ctx->Map.entry.Post_PositionSecondHelping
  );
  ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Check(
    ctx,
    ctx->Map.entry.Post_PositionOtherHelping
  );
  ScoreTqReqEnqueuePriority_Post_QueueOrder_Check(
    ctx,
    ctx->Map.entry.Post_QueueOrder
  );
}

static T_fixture_node ScoreTqReqEnqueuePriority_Node;

static T_remark ScoreTqReqEnqueuePriority_Remark = {
  .next = NULL,
  .remark = "ScoreTqReqEnqueuePriority"
};

void ScoreTqReqEnqueuePriority_Run( TQContext *tq_ctx, bool home_only )
{
  ScoreTqReqEnqueuePriority_Context *ctx;

  ctx = &ScoreTqReqEnqueuePriority_Instance;
  ctx->tq_ctx = tq_ctx;
  ctx->home_only = home_only;

  ctx = T_push_fixture(
    &ScoreTqReqEnqueuePriority_Node,
    &ScoreTqReqEnqueuePriority_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;
  ScoreTqReqEnqueuePriority_ResetRepresentatives( ctx );

  for (
    ctx->Map.pcs[ 0 ] = ScoreTqReqEnqueuePriority_Pre_EligibleScheduler_Home;
    ctx->Map.pcs[ 0 ] < ScoreTqReqEnqueuePriority_Pre_EligibleScheduler_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = ScoreTqReqEnqueuePriority_Pre_QueueHome_Empty;
      ctx->Map.pcs[ 1 ] < ScoreTqReqEnqueuePriority_Pre_QueueHome_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] =
          ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_Empty;
        ctx->Map.pcs[ 2 ] < ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] =
            ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_Empty;
          ctx->Map.pcs[ 3 ] <
          ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] =
              ScoreTqReqEnqueuePriority_Pre_QueueIneligible_None;
            ctx->Map.pcs[ 4 ] <
            ScoreTqReqEnqueuePriority_Pre_QueueIneligible_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            for (
              ctx->Map.pcs[ 5 ] =
                ScoreTqReqEnqueuePriority_Pre_QueueOrder_HomeFirstSecondIneligible;
              ctx->Map.pcs[ 5 ] < ScoreTqReqEnqueuePriority_Pre_QueueOrder_NA;
              ++ctx->Map.pcs[ 5 ]
            ) {
              ctx->Map.entry = ScoreTqReqEnqueuePriority_PopEntry( ctx );

              if ( ctx->Map.entry.Skip ) {
                continue;
              }

              for (
                ctx->Map.rep = 0;
                ctx->Map.rep <
                ScoreTqReqEnqueuePriority_GetRepresentativeCount( ctx );
                ++ctx->Map.rep
              ) {
                ctx->Map.skip_rep = false;
                ScoreTqReqEnqueuePriority_Prepare( ctx );
                ScoreTqReqEnqueuePriority_TestVariant( ctx );

                if ( ctx->Map.skip ) {
                  break;
                }

                ScoreTqReqEnqueuePriority_MarkRepresentatives( ctx );
              }
            }
          }
        }
      }
    }
  }

  ScoreTqReqEnqueuePriority_CheckRepresentatives( ctx );

  T_add_remark( &ScoreTqReqEnqueuePriority_Remark );
  T_pop_fixture();
}

/** @} */
