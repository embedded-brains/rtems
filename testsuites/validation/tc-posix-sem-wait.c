/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CSemaphoreReqWait
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

#include "tr-sem-seize-wait.h"
#include "tx-support.h"
#include "tx-thread-queue.h"

#include <rtems/test.h>

/**
 * @defgroup CSemaphoreReqWait spec:/c/semaphore/req/wait
 *
 * @ingroup TestsuitesValidationNoClock1
 *
 * @{
 */

typedef enum {
  CSemaphoreReqWait_Pre_Sem_Obj,
  CSemaphoreReqWait_Pre_Sem_Null,
  CSemaphoreReqWait_Pre_Sem_NA
} CSemaphoreReqWait_Pre_Sem;

typedef enum {
  CSemaphoreReqWait_Pre_Initialized_Yes,
  CSemaphoreReqWait_Pre_Initialized_No,
  CSemaphoreReqWait_Pre_Initialized_NA
} CSemaphoreReqWait_Pre_Initialized;

typedef enum {
  CSemaphoreReqWait_Post_Status_MinusOne,
  CSemaphoreReqWait_Post_Status_NA
} CSemaphoreReqWait_Post_Status;

typedef enum {
  CSemaphoreReqWait_Post_Errno_Inval,
  CSemaphoreReqWait_Post_Errno_NA
} CSemaphoreReqWait_Post_Errno;

typedef enum {
  CSemaphoreReqWait_Post_Sem_Nop,
  CSemaphoreReqWait_Post_Sem_NA
} CSemaphoreReqWait_Post_Sem;

typedef enum {
  CSemaphoreReqWait_Post_Action_SemSeizeWait,
  CSemaphoreReqWait_Post_Action_NA
} CSemaphoreReqWait_Post_Action;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Sem_NA : 1;
  uint8_t Pre_Initialized_NA : 1;
  uint8_t Post_Status : 1;
  uint8_t Post_Errno : 1;
  uint8_t Post_Sem : 1;
  uint8_t Post_Action : 1;
} CSemaphoreReqWait_Entry;

/**
 * @brief Test context for spec:/c/semaphore/req/wait test case.
 */
typedef struct {
  /**
   * @brief This member contains the thread queue test context.
   */
  TQSemContext tq_sem_ctx;

  /**
   * @brief This member specifies the `sem` parameter value.
   */
  sem_t *sem;

  /**
   * @brief This member contains the return value of the sem_wait() call.
   */
  int status;

  /**
   * @brief This member contains the return value of the errno after the
   *   sem_wait() call.
   */
  int errno_value;

  /**
   * @brief This member contains the semaphore object.
   */
  sem_t sem_obj;

  /**
   * @brief This member contains a copy of the semaphore object before the
   *   sem_wait() call.
   */
  sem_t sem_before;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 2 ];

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
    CSemaphoreReqWait_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CSemaphoreReqWait_Context;

static CSemaphoreReqWait_Context CSemaphoreReqWait_Instance;

static const char *const CSemaphoreReqWait_PreDesc_Sem[] =
  { "Obj", "Null", "NA" };

static const char *const CSemaphoreReqWait_PreDesc_Initialized[] =
  { "Yes", "No", "NA" };

static const char *const *const CSemaphoreReqWait_PreDesc[] = {
  CSemaphoreReqWait_PreDesc_Sem,
  CSemaphoreReqWait_PreDesc_Initialized,
  NULL
};

typedef CSemaphoreReqWait_Context Context;

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

static void CSemaphoreReqWait_Pre_Sem_Prepare(
  CSemaphoreReqWait_Context *ctx,
  CSemaphoreReqWait_Pre_Sem  state
)
{
  switch ( state ) {
    case CSemaphoreReqWait_Pre_Sem_Obj: {
      /*
       * While the `sem` parameter references an object of type sem_t.
       */
      T_eq_int( sem_init( &ctx->sem_obj, 0, 1 ), 0 );
      ctx->sem = &ctx->sem_obj;
      break;
    }

    case CSemaphoreReqWait_Pre_Sem_Null: {
      /*
       * While the `sem` parameter is NULL.
       */
      ctx->sem = NULL;
      break;
    }

    case CSemaphoreReqWait_Pre_Sem_NA:
      break;
  }
}

static void CSemaphoreReqWait_Pre_Initialized_Prepare(
  CSemaphoreReqWait_Context        *ctx,
  CSemaphoreReqWait_Pre_Initialized state
)
{
  switch ( state ) {
    case CSemaphoreReqWait_Pre_Initialized_Yes: {
      /*
       * While the object referenced by the `sem` parameter is an initialized
       * counting semaphore with a value of one.
       */
      /* The object was initialized by the arrangement of pre-condition Sem */
      break;
    }

    case CSemaphoreReqWait_Pre_Initialized_No: {
      /*
       * While the object referenced by the `sem` parameter is not an
       * initialized counting semaphore.
       */
      T_eq_int( sem_destroy( &ctx->sem_obj ), 0 );
      break;
    }

    case CSemaphoreReqWait_Pre_Initialized_NA:
      break;
  }
}

static void CSemaphoreReqWait_Post_Status_Check(
  CSemaphoreReqWait_Context    *ctx,
  CSemaphoreReqWait_Post_Status state
)
{
  switch ( state ) {
    case CSemaphoreReqWait_Post_Status_MinusOne: {
      /*
       * The return value of sem_wait() shall be minus one.
       */
      T_eq_int( ctx->status, -1 );
      break;
    }

    case CSemaphoreReqWait_Post_Status_NA:
      break;
  }
}

static void CSemaphoreReqWait_Post_Errno_Check(
  CSemaphoreReqWait_Context   *ctx,
  CSemaphoreReqWait_Post_Errno state
)
{
  switch ( state ) {
    case CSemaphoreReqWait_Post_Errno_Inval: {
      /*
       * The value of errno shall be EINVAL.
       */
      T_eq_int( ctx->errno_value, EINVAL );
      break;
    }

    case CSemaphoreReqWait_Post_Errno_NA:
      break;
  }
}

static void CSemaphoreReqWait_Post_Sem_Check(
  CSemaphoreReqWait_Context *ctx,
  CSemaphoreReqWait_Post_Sem state
)
{
  switch ( state ) {
    case CSemaphoreReqWait_Post_Sem_Nop: {
      /*
       * The object referenced by the `sem` parameter shall not be modified.
       */
      sem_t expected;

      memcpy( &expected, &ctx->sem_before, sizeof( expected ) );
      ExpectReleasedLock( ctx, &expected );
      T_eq_mem( &ctx->sem_obj, &expected, sizeof( ctx->sem_obj ) );
      break;
    }

    case CSemaphoreReqWait_Post_Sem_NA:
      break;
  }
}

static void CSemaphoreReqWait_Post_Action_Check(
  CSemaphoreReqWait_Context    *ctx,
  CSemaphoreReqWait_Post_Action state
)
{
  switch ( state ) {
    case CSemaphoreReqWait_Post_Action_SemSeizeWait: {
      /*
       * The calling thread shall wait to seize the counting semaphore
       * referenced by the `sem` parameter as specified by
       * spec:/score/sem/req/seize-wait where the thread queue of the semaphore
       * uses the priority discipline and the directive waits without a
       * timeout.
       */
      T_eq_int( sem_post( ctx->sem ), 0 );
      ctx->tq_sem_ctx.base.thread_queue_object = &ctx->sem_obj;
      ctx->tq_sem_ctx.base.wait = TQ_WAIT_FOREVER;
      ScoreSemReqSeizeWait_Run( &ctx->tq_sem_ctx );
      break;
    }

    case CSemaphoreReqWait_Post_Action_NA:
      break;
  }
}

static void CSemaphoreReqWait_Setup( CSemaphoreReqWait_Context *ctx )
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

static void CSemaphoreReqWait_Setup_Wrap( void *arg )
{
  CSemaphoreReqWait_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CSemaphoreReqWait_Setup( ctx );
}

static void CSemaphoreReqWait_Teardown( CSemaphoreReqWait_Context *ctx )
{
  TQDestroy( &ctx->tq_sem_ctx.base );
  RestoreRunnerPriority();
}

static void CSemaphoreReqWait_Teardown_Wrap( void *arg )
{
  CSemaphoreReqWait_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CSemaphoreReqWait_Teardown( ctx );
}

static void CSemaphoreReqWait_Prepare( CSemaphoreReqWait_Context *ctx )
{
  memset( &ctx->sem_obj, 0, sizeof( ctx->sem_obj ) );
}

static void CSemaphoreReqWait_Action( CSemaphoreReqWait_Context *ctx )
{
  memcpy( &ctx->sem_before, &ctx->sem_obj, sizeof( ctx->sem_before ) );
  errno = ERRNO_POISON;
  ctx->status = sem_wait( ctx->sem );
  ctx->errno_value = errno;
}

static void CSemaphoreReqWait_Cleanup( CSemaphoreReqWait_Context *ctx )
{
  if ( ctx->sem_obj._flags != 0 ) {
    T_eq_int( sem_destroy( &ctx->sem_obj ), 0 );
  }
}

/* clang-format off */

static const CSemaphoreReqWait_Entry
CSemaphoreReqWait_Entries[] = {
  { 0, 0, 0, CSemaphoreReqWait_Post_Status_NA, CSemaphoreReqWait_Post_Errno_NA,
    CSemaphoreReqWait_Post_Sem_NA, CSemaphoreReqWait_Post_Action_SemSeizeWait },
  { 0, 0, 0, CSemaphoreReqWait_Post_Status_MinusOne,
    CSemaphoreReqWait_Post_Errno_Inval, CSemaphoreReqWait_Post_Sem_Nop,
    CSemaphoreReqWait_Post_Action_NA },
  { 0, 0, 1, CSemaphoreReqWait_Post_Status_MinusOne,
    CSemaphoreReqWait_Post_Errno_Inval, CSemaphoreReqWait_Post_Sem_NA,
    CSemaphoreReqWait_Post_Action_NA },
  { 1, 0, 0, CSemaphoreReqWait_Post_Status_NA, CSemaphoreReqWait_Post_Errno_NA,
    CSemaphoreReqWait_Post_Sem_NA, CSemaphoreReqWait_Post_Action_NA }
};

static const uint8_t
CSemaphoreReqWait_Map[] = {
  0, 1, 2, 3
};

/* clang-format on */

static size_t CSemaphoreReqWait_Scope( void *arg, char *buf, size_t n )
{
  CSemaphoreReqWait_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CSemaphoreReqWait_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CSemaphoreReqWait_Fixture = {
  .setup = CSemaphoreReqWait_Setup_Wrap,
  .stop = NULL,
  .teardown = CSemaphoreReqWait_Teardown_Wrap,
  .scope = CSemaphoreReqWait_Scope,
  .initial_context = &CSemaphoreReqWait_Instance
};

static inline CSemaphoreReqWait_Entry CSemaphoreReqWait_PopEntry(
  CSemaphoreReqWait_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CSemaphoreReqWait_Entries[ CSemaphoreReqWait_Map[ index ] ];
}

static void CSemaphoreReqWait_SetPreConditionStates(
  CSemaphoreReqWait_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Initialized_NA ) {
    ctx->Map.pcs[ 1 ] = CSemaphoreReqWait_Pre_Initialized_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }
}

static void CSemaphoreReqWait_TestVariant( CSemaphoreReqWait_Context *ctx )
{
  CSemaphoreReqWait_Pre_Sem_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CSemaphoreReqWait_Pre_Initialized_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CSemaphoreReqWait_Action( ctx );
  CSemaphoreReqWait_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CSemaphoreReqWait_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
  CSemaphoreReqWait_Post_Sem_Check( ctx, ctx->Map.entry.Post_Sem );
  CSemaphoreReqWait_Post_Action_Check( ctx, ctx->Map.entry.Post_Action );
}

/**
 * @fn void T_case_body_CSemaphoreReqWait( void )
 */
T_TEST_CASE_FIXTURE( CSemaphoreReqWait, &CSemaphoreReqWait_Fixture )
{
  CSemaphoreReqWait_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CSemaphoreReqWait_Pre_Sem_Obj;
    ctx->Map.pci[ 0 ] < CSemaphoreReqWait_Pre_Sem_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CSemaphoreReqWait_Pre_Initialized_Yes;
      ctx->Map.pci[ 1 ] < CSemaphoreReqWait_Pre_Initialized_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      ctx->Map.entry = CSemaphoreReqWait_PopEntry( ctx );

      if ( ctx->Map.entry.Skip ) {
        continue;
      }

      CSemaphoreReqWait_SetPreConditionStates( ctx );
      CSemaphoreReqWait_Prepare( ctx );
      CSemaphoreReqWait_TestVariant( ctx );
      CSemaphoreReqWait_Cleanup( ctx );
    }
  }
}

/** @} */
