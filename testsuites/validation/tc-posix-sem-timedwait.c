/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CSemaphoreReqTimedwait
 */

/*
 * Copyright (C) 2026 OHB System AG
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

#include <errno.h>
#include <semaphore.h>
#include <string.h>
#include <time.h>

#include "tx-support.h"
#include "tx-thread-queue.h"

#include <rtems/test.h>

/**
 * @defgroup CSemaphoreReqTimedwait spec:/c/semaphore/req/timedwait
 *
 * @ingroup TestsuitesValidationNoClock1
 *
 * @{
 */

typedef enum {
  CSemaphoreReqTimedwait_Pre_Sem_Obj,
  CSemaphoreReqTimedwait_Pre_Sem_Null,
  CSemaphoreReqTimedwait_Pre_Sem_NA
} CSemaphoreReqTimedwait_Pre_Sem;

typedef enum {
  CSemaphoreReqTimedwait_Pre_Initialized_Yes,
  CSemaphoreReqTimedwait_Pre_Initialized_No,
  CSemaphoreReqTimedwait_Pre_Initialized_NA
} CSemaphoreReqTimedwait_Pre_Initialized;

typedef enum {
  CSemaphoreReqTimedwait_Pre_Value_Zero,
  CSemaphoreReqTimedwait_Pre_Value_One,
  CSemaphoreReqTimedwait_Pre_Value_NA
} CSemaphoreReqTimedwait_Pre_Value;

typedef enum {
  CSemaphoreReqTimedwait_Pre_Abstime_Future,
  CSemaphoreReqTimedwait_Pre_Abstime_Past,
  CSemaphoreReqTimedwait_Pre_Abstime_Invalid,
  CSemaphoreReqTimedwait_Pre_Abstime_Null,
  CSemaphoreReqTimedwait_Pre_Abstime_NA
} CSemaphoreReqTimedwait_Pre_Abstime;

typedef enum {
  CSemaphoreReqTimedwait_Post_Status_Ok,
  CSemaphoreReqTimedwait_Post_Status_MinusOne,
  CSemaphoreReqTimedwait_Post_Status_NA
} CSemaphoreReqTimedwait_Post_Status;

typedef enum {
  CSemaphoreReqTimedwait_Post_Errno_Inval,
  CSemaphoreReqTimedwait_Post_Errno_Timedout,
  CSemaphoreReqTimedwait_Post_Errno_Nop,
  CSemaphoreReqTimedwait_Post_Errno_NA
} CSemaphoreReqTimedwait_Post_Errno;

typedef enum {
  CSemaphoreReqTimedwait_Post_Sem_Decremented,
  CSemaphoreReqTimedwait_Post_Sem_Nop,
  CSemaphoreReqTimedwait_Post_Sem_NA
} CSemaphoreReqTimedwait_Post_Sem;

typedef enum {
  CSemaphoreReqTimedwait_Post_Wake_Zero,
  CSemaphoreReqTimedwait_Post_Wake_NA
} CSemaphoreReqTimedwait_Post_Wake;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Sem_NA : 1;
  uint16_t Pre_Initialized_NA : 1;
  uint16_t Pre_Value_NA : 1;
  uint16_t Pre_Abstime_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Errno : 2;
  uint16_t Post_Sem : 2;
  uint16_t Post_Wake : 1;
} CSemaphoreReqTimedwait_Entry;

/**
 * @brief Test context for spec:/c/semaphore/req/timedwait test case.
 */
typedef struct {
  /**
   * @brief This member contains the thread queue test context.
   */
  TQSemContext tq_sem_ctx;

  /**
   * @brief This member is true, if the value of the semaphore is zero.
   */
  bool value_zero;

  /**
   * @brief This member is true, if the absolute time is in the future.
   */
  bool abstime_future;

  /**
   * @brief This member specifies the `sem` parameter value.
   */
  sem_t *sem;

  /**
   * @brief This member specifies the `abs_timeout` parameter value.
   */
  const struct timespec *abstime;

  /**
   * @brief This member contains the return value of the sem_timedwait() call.
   */
  int status;

  /**
   * @brief This member contains the return value of the errno after the
   *   sem_timedwait() call.
   */
  int errno_value;

  /**
   * @brief This member contains the semaphore object.
   */
  sem_t sem_obj;

  /**
   * @brief This member contains a copy of the semaphore object before the
   *   sem_timedwait() call.
   */
  sem_t sem_before;

  /**
   * @brief This member contains the object referenced by the `abs_timeout`
   *   parameter.
   */
  struct timespec abstime_obj;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 4 ];

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
    CSemaphoreReqTimedwait_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CSemaphoreReqTimedwait_Context;

static CSemaphoreReqTimedwait_Context CSemaphoreReqTimedwait_Instance;

static const char *const CSemaphoreReqTimedwait_PreDesc_Sem[] =
  { "Obj", "Null", "NA" };

static const char *const CSemaphoreReqTimedwait_PreDesc_Initialized[] =
  { "Yes", "No", "NA" };

static const char *const CSemaphoreReqTimedwait_PreDesc_Value[] =
  { "Zero", "One", "NA" };

static const char *const CSemaphoreReqTimedwait_PreDesc_Abstime[] =
  { "Future", "Past", "Invalid", "Null", "NA" };

static const char *const *const CSemaphoreReqTimedwait_PreDesc[] = {
  CSemaphoreReqTimedwait_PreDesc_Sem,
  CSemaphoreReqTimedwait_PreDesc_Initialized,
  CSemaphoreReqTimedwait_PreDesc_Value,
  CSemaphoreReqTimedwait_PreDesc_Abstime,
  NULL
};

typedef CSemaphoreReqTimedwait_Context Context;

/*
 * In SMP configurations, the functions acquire and release the ticket lock of
 * the thread queue of the semaphore, which advances its counters.  The checks
 * of the semaphore object expect a released lock and take its counters from
 * the object.
 */
static void ExpectReleasedLock( const Context *ctx, sem_t *expected )
{
  const struct _Ticket_lock_Control *lock;

  lock = &ctx->sem_obj._Semaphore._Queue._Lock;
  T_eq_uint( lock->_next_ticket, lock->_now_serving );
  expected->_Semaphore._Queue._Lock = *lock;
}

#define ERRNO_POISON -1

static void CSemaphoreReqTimedwait_Pre_Sem_Prepare(
  CSemaphoreReqTimedwait_Context *ctx,
  CSemaphoreReqTimedwait_Pre_Sem  state
)
{
  switch ( state ) {
    case CSemaphoreReqTimedwait_Pre_Sem_Obj: {
      /*
       * While the `sem` parameter references an object of type sem_t.
       */
      T_eq_int( sem_init( &ctx->sem_obj, 0, 0 ), 0 );
      ctx->sem = &ctx->sem_obj;
      break;
    }

    case CSemaphoreReqTimedwait_Pre_Sem_Null: {
      /*
       * While the `sem` parameter is NULL.
       */
      ctx->sem = NULL;
      break;
    }

    case CSemaphoreReqTimedwait_Pre_Sem_NA:
      break;
  }
}

static void CSemaphoreReqTimedwait_Pre_Initialized_Prepare(
  CSemaphoreReqTimedwait_Context        *ctx,
  CSemaphoreReqTimedwait_Pre_Initialized state
)
{
  switch ( state ) {
    case CSemaphoreReqTimedwait_Pre_Initialized_Yes: {
      /*
       * While the object referenced by the `sem` parameter is an initialized
       * counting semaphore.
       */
      /* The object was initialized by the arrangement of pre-condition Sem */
      break;
    }

    case CSemaphoreReqTimedwait_Pre_Initialized_No: {
      /*
       * While the object referenced by the `sem` parameter is not an
       * initialized counting semaphore.
       */
      T_eq_int( sem_destroy( &ctx->sem_obj ), 0 );
      break;
    }

    case CSemaphoreReqTimedwait_Pre_Initialized_NA:
      break;
  }
}

static void CSemaphoreReqTimedwait_Pre_Value_Prepare(
  CSemaphoreReqTimedwait_Context  *ctx,
  CSemaphoreReqTimedwait_Pre_Value state
)
{
  switch ( state ) {
    case CSemaphoreReqTimedwait_Pre_Value_Zero: {
      /*
       * While the value of the counting semaphore is zero.
       */
      ctx->sem_obj._Semaphore._count = 0;
      ctx->value_zero = true;
      break;
    }

    case CSemaphoreReqTimedwait_Pre_Value_One: {
      /*
       * While the value of the counting semaphore is one.
       */
      ctx->sem_obj._Semaphore._count = 1;
      break;
    }

    case CSemaphoreReqTimedwait_Pre_Value_NA:
      break;
  }
}

static void CSemaphoreReqTimedwait_Pre_Abstime_Prepare(
  CSemaphoreReqTimedwait_Context    *ctx,
  CSemaphoreReqTimedwait_Pre_Abstime state
)
{
  switch ( state ) {
    case CSemaphoreReqTimedwait_Pre_Abstime_Future: {
      /*
       * While the `abs_timeout` parameter references a valid absolute time
       * which is in the future.
       */
      T_eq_int( clock_gettime( CLOCK_REALTIME, &ctx->abstime_obj ), 0 );
      ++ctx->abstime_obj.tv_sec;
      ctx->abstime = &ctx->abstime_obj;
      ctx->abstime_future = true;
      break;
    }

    case CSemaphoreReqTimedwait_Pre_Abstime_Past: {
      /*
       * While the `abs_timeout` parameter references a valid absolute time
       * which is not in the future.
       */
      ctx->abstime_obj.tv_sec = 0;
      ctx->abstime_obj.tv_nsec = 0;
      ctx->abstime = &ctx->abstime_obj;
      break;
    }

    case CSemaphoreReqTimedwait_Pre_Abstime_Invalid: {
      /*
       * While the `abs_timeout` parameter references an absolute time with a
       * nanoseconds value which is not in the range from zero to 999999999.
       */
      ctx->abstime_obj.tv_sec = 0;
      ctx->abstime_obj.tv_nsec = 1000000000;
      ctx->abstime = &ctx->abstime_obj;
      break;
    }

    case CSemaphoreReqTimedwait_Pre_Abstime_Null: {
      /*
       * While the `abs_timeout` parameter is NULL.
       */
      ctx->abstime = NULL;
      break;
    }

    case CSemaphoreReqTimedwait_Pre_Abstime_NA:
      break;
  }
}

static void CSemaphoreReqTimedwait_Post_Status_Check(
  CSemaphoreReqTimedwait_Context    *ctx,
  CSemaphoreReqTimedwait_Post_Status state
)
{
  switch ( state ) {
    case CSemaphoreReqTimedwait_Post_Status_Ok: {
      /*
       * The return value of sem_timedwait() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CSemaphoreReqTimedwait_Post_Status_MinusOne: {
      /*
       * The return value of sem_timedwait() shall be minus one.
       */
      T_eq_int( ctx->status, -1 );
      break;
    }

    case CSemaphoreReqTimedwait_Post_Status_NA:
      break;
  }
}

static void CSemaphoreReqTimedwait_Post_Errno_Check(
  CSemaphoreReqTimedwait_Context   *ctx,
  CSemaphoreReqTimedwait_Post_Errno state
)
{
  switch ( state ) {
    case CSemaphoreReqTimedwait_Post_Errno_Inval: {
      /*
       * The value of errno shall be EINVAL.
       */
      T_eq_int( ctx->errno_value, EINVAL );
      break;
    }

    case CSemaphoreReqTimedwait_Post_Errno_Timedout: {
      /*
       * The value of errno shall be ETIMEDOUT.
       */
      T_eq_int( ctx->errno_value, ETIMEDOUT );
      break;
    }

    case CSemaphoreReqTimedwait_Post_Errno_Nop: {
      /*
       * The value of errno shall not be modified.
       */
      T_eq_int( ctx->errno_value, ERRNO_POISON );
      break;
    }

    case CSemaphoreReqTimedwait_Post_Errno_NA:
      break;
  }
}

static void CSemaphoreReqTimedwait_Post_Sem_Check(
  CSemaphoreReqTimedwait_Context *ctx,
  CSemaphoreReqTimedwait_Post_Sem state
)
{
  switch ( state ) {
    case CSemaphoreReqTimedwait_Post_Sem_Decremented: {
      /*
       * The value of the counting semaphore referenced by the `sem` parameter
       * shall be decremented by one, and the object shall not be modified
       * otherwise.
       */
      sem_t expected;

      memcpy( &expected, &ctx->sem_before, sizeof( expected ) );
      expected._Semaphore._count = 0;
      ExpectReleasedLock( ctx, &expected );
      T_eq_mem( &ctx->sem_obj, &expected, sizeof( ctx->sem_obj ) );
      break;
    }

    case CSemaphoreReqTimedwait_Post_Sem_Nop: {
      /*
       * The object referenced by the `sem` parameter shall not be modified.
       */
      sem_t expected;

      memcpy( &expected, &ctx->sem_before, sizeof( expected ) );
      ExpectReleasedLock( ctx, &expected );
      T_eq_mem( &ctx->sem_obj, &expected, sizeof( ctx->sem_obj ) );
      break;
    }

    case CSemaphoreReqTimedwait_Post_Sem_NA:
      break;
  }
}

static void CSemaphoreReqTimedwait_Post_Wake_Check(
  CSemaphoreReqTimedwait_Context  *ctx,
  CSemaphoreReqTimedwait_Post_Wake state
)
{
  switch ( state ) {
    case CSemaphoreReqTimedwait_Post_Wake_Zero: {
      /*
       * The return value of sem_timedwait() for the blocked thread shall be
       * zero after another thread posted the semaphore.
       */
      T_eq_int(
        ctx->tq_sem_ctx.base.status[ TQ_BLOCKER_A ],
        TQConvertStatus( &ctx->tq_sem_ctx.base, STATUS_SUCCESSFUL )
      );
      break;
    }

    case CSemaphoreReqTimedwait_Post_Wake_NA:
      break;
  }
}

static void CSemaphoreReqTimedwait_Setup( CSemaphoreReqTimedwait_Context *ctx )
{
  SetSelfPriority( PRIO_NORMAL );
  memset( ctx, 0, sizeof( *ctx ) );
  ctx->tq_sem_ctx.base.enqueue_variant = TQ_ENQUEUE_BLOCKS;
  ctx->tq_sem_ctx.base.discipline = TQ_PRIORITY;
  ctx->tq_sem_ctx.base.deadlock = TQ_DEADLOCK_FATAL;
  ctx->tq_sem_ctx.base.convert_status = TQConvertStatusPOSIX;
  ctx->tq_sem_ctx.base.enqueue_prepare = TQEnqueuePrepareDefault;
  ctx->tq_sem_ctx.base.enqueue_done = TQEnqueueDoneDefault;
  ctx->tq_sem_ctx.base.enqueue = TQEnqueuePOSIXSem;
  ctx->tq_sem_ctx.base.surrender = TQSurrenderPOSIXSem;
  ctx->tq_sem_ctx.variant = TQ_SEM_COUNTING;
  ctx->tq_sem_ctx.get_count = TQSemGetCountPOSIX;
  ctx->tq_sem_ctx.set_count = TQSemSetCountPOSIX;
  TQInitialize( &ctx->tq_sem_ctx.base );
}

static void CSemaphoreReqTimedwait_Setup_Wrap( void *arg )
{
  CSemaphoreReqTimedwait_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CSemaphoreReqTimedwait_Setup( ctx );
}

static void CSemaphoreReqTimedwait_Teardown(
  CSemaphoreReqTimedwait_Context *ctx
)
{
  TQDestroy( &ctx->tq_sem_ctx.base );
  RestoreRunnerPriority();
}

static void CSemaphoreReqTimedwait_Teardown_Wrap( void *arg )
{
  CSemaphoreReqTimedwait_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CSemaphoreReqTimedwait_Teardown( ctx );
}

static void CSemaphoreReqTimedwait_Prepare(
  CSemaphoreReqTimedwait_Context *ctx
)
{
  memset( &ctx->sem_obj, 0, sizeof( ctx->sem_obj ) );
  ctx->value_zero = false;
  ctx->abstime_future = false;
}

static void CSemaphoreReqTimedwait_Action(
  CSemaphoreReqTimedwait_Context *ctx
)
{
  memcpy( &ctx->sem_before, &ctx->sem_obj, sizeof( ctx->sem_before ) );
  errno = ERRNO_POISON;

  if (
    ctx->sem == &ctx->sem_obj && ctx->sem_obj._flags != 0 && ctx->value_zero &&
    ctx->abstime_future
  ) {
    ctx->tq_sem_ctx.base.thread_queue_object = &ctx->sem_obj;
    TQSendAndWaitForExecutionStop(
      &ctx->tq_sem_ctx.base,
      TQ_BLOCKER_A,
      TQ_EVENT_ENQUEUE_TIMED | TQ_EVENT_RUNNER_SYNC
    );
    T_eq_int( sem_post( &ctx->sem_obj ), 0 );
    TQSynchronizeRunner();
  } else {
    ctx->status = sem_timedwait( ctx->sem, ctx->abstime );
    ctx->errno_value = errno;
  }
}

static void CSemaphoreReqTimedwait_Cleanup(
  CSemaphoreReqTimedwait_Context *ctx
)
{
  if ( ctx->sem_obj._flags != 0 ) {
    T_eq_int( sem_destroy( &ctx->sem_obj ), 0 );
  }
}

/* clang-format off */

static const CSemaphoreReqTimedwait_Entry
CSemaphoreReqTimedwait_Entries[] = {
  { 1, 0, 0, 0, 0, CSemaphoreReqTimedwait_Post_Status_NA,
    CSemaphoreReqTimedwait_Post_Errno_NA, CSemaphoreReqTimedwait_Post_Sem_NA,
    CSemaphoreReqTimedwait_Post_Wake_NA },
  { 0, 0, 0, 0, 0, CSemaphoreReqTimedwait_Post_Status_MinusOne,
    CSemaphoreReqTimedwait_Post_Errno_Inval,
    CSemaphoreReqTimedwait_Post_Sem_Nop, CSemaphoreReqTimedwait_Post_Wake_NA },
  { 0, 0, 0, 0, 0, CSemaphoreReqTimedwait_Post_Status_Ok,
    CSemaphoreReqTimedwait_Post_Errno_Nop,
    CSemaphoreReqTimedwait_Post_Sem_Decremented,
    CSemaphoreReqTimedwait_Post_Wake_NA },
  { 0, 0, 0, 0, 0, CSemaphoreReqTimedwait_Post_Status_NA,
    CSemaphoreReqTimedwait_Post_Errno_NA, CSemaphoreReqTimedwait_Post_Sem_NA,
    CSemaphoreReqTimedwait_Post_Wake_Zero },
  { 0, 0, 0, 0, 0, CSemaphoreReqTimedwait_Post_Status_MinusOne,
    CSemaphoreReqTimedwait_Post_Errno_Timedout,
    CSemaphoreReqTimedwait_Post_Sem_Nop, CSemaphoreReqTimedwait_Post_Wake_NA },
  { 0, 0, 1, 1, 1, CSemaphoreReqTimedwait_Post_Status_MinusOne,
    CSemaphoreReqTimedwait_Post_Errno_Inval,
    CSemaphoreReqTimedwait_Post_Sem_NA, CSemaphoreReqTimedwait_Post_Wake_NA }
};

static const uint8_t
CSemaphoreReqTimedwait_Map[] = {
  3, 4, 1, 1, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t CSemaphoreReqTimedwait_Scope( void *arg, char *buf, size_t n )
{
  CSemaphoreReqTimedwait_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CSemaphoreReqTimedwait_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CSemaphoreReqTimedwait_Fixture = {
  .setup = CSemaphoreReqTimedwait_Setup_Wrap,
  .stop = NULL,
  .teardown = CSemaphoreReqTimedwait_Teardown_Wrap,
  .scope = CSemaphoreReqTimedwait_Scope,
  .initial_context = &CSemaphoreReqTimedwait_Instance
};

static inline CSemaphoreReqTimedwait_Entry CSemaphoreReqTimedwait_PopEntry(
  CSemaphoreReqTimedwait_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CSemaphoreReqTimedwait_Entries[ CSemaphoreReqTimedwait_Map[ index ] ];
}

static void CSemaphoreReqTimedwait_SetPreConditionStates(
  CSemaphoreReqTimedwait_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Initialized_NA ) {
    ctx->Map.pcs[ 1 ] = CSemaphoreReqTimedwait_Pre_Initialized_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_Value_NA ) {
    ctx->Map.pcs[ 2 ] = CSemaphoreReqTimedwait_Pre_Value_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_Abstime_NA ) {
    ctx->Map.pcs[ 3 ] = CSemaphoreReqTimedwait_Pre_Abstime_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }
}

static void CSemaphoreReqTimedwait_TestVariant(
  CSemaphoreReqTimedwait_Context *ctx
)
{
  CSemaphoreReqTimedwait_Pre_Sem_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CSemaphoreReqTimedwait_Pre_Initialized_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CSemaphoreReqTimedwait_Pre_Value_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CSemaphoreReqTimedwait_Pre_Abstime_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CSemaphoreReqTimedwait_Action( ctx );
  CSemaphoreReqTimedwait_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CSemaphoreReqTimedwait_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
  CSemaphoreReqTimedwait_Post_Sem_Check( ctx, ctx->Map.entry.Post_Sem );
  CSemaphoreReqTimedwait_Post_Wake_Check( ctx, ctx->Map.entry.Post_Wake );
}

/**
 * @fn void T_case_body_CSemaphoreReqTimedwait( void )
 */
T_TEST_CASE_FIXTURE( CSemaphoreReqTimedwait, &CSemaphoreReqTimedwait_Fixture )
{
  CSemaphoreReqTimedwait_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CSemaphoreReqTimedwait_Pre_Sem_Obj;
    ctx->Map.pci[ 0 ] < CSemaphoreReqTimedwait_Pre_Sem_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CSemaphoreReqTimedwait_Pre_Initialized_Yes;
      ctx->Map.pci[ 1 ] < CSemaphoreReqTimedwait_Pre_Initialized_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CSemaphoreReqTimedwait_Pre_Value_Zero;
        ctx->Map.pci[ 2 ] < CSemaphoreReqTimedwait_Pre_Value_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CSemaphoreReqTimedwait_Pre_Abstime_Future;
          ctx->Map.pci[ 3 ] < CSemaphoreReqTimedwait_Pre_Abstime_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = CSemaphoreReqTimedwait_PopEntry( ctx );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          CSemaphoreReqTimedwait_SetPreConditionStates( ctx );
          CSemaphoreReqTimedwait_Prepare( ctx );
          CSemaphoreReqTimedwait_TestVariant( ctx );
          CSemaphoreReqTimedwait_Cleanup( ctx );
        }
      }
    }
  }
}

/** @} */
