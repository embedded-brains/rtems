/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqSpinLock
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

#include <pthread.h>
#include <rtems.h>
#include <rtems/score/isrlevel.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqSpinLock spec:/c/pthread/req/spin-lock
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqSpinLock_Pre_Interrupts_Enabled,
  CPthreadReqSpinLock_Pre_Interrupts_Disabled,
  CPthreadReqSpinLock_Pre_Interrupts_NA
} CPthreadReqSpinLock_Pre_Interrupts;

typedef enum {
  CPthreadReqSpinLock_Pre_Lock_Available,
  CPthreadReqSpinLock_Pre_Lock_Owned,
  CPthreadReqSpinLock_Pre_Lock_NA
} CPthreadReqSpinLock_Pre_Lock;

typedef enum {
  CPthreadReqSpinLock_Post_Status_Ok,
  CPthreadReqSpinLock_Post_Status_NA
} CPthreadReqSpinLock_Post_Status;

typedef enum {
  CPthreadReqSpinLock_Post_Interrupts_Disabled,
  CPthreadReqSpinLock_Post_Interrupts_NA
} CPthreadReqSpinLock_Post_Interrupts;

typedef enum {
  CPthreadReqSpinLock_Post_Owner_Caller,
  CPthreadReqSpinLock_Post_Owner_NA
} CPthreadReqSpinLock_Post_Owner;

typedef enum {
  CPthreadReqSpinLock_Post_Wait_UntilReleased,
  CPthreadReqSpinLock_Post_Wait_NA
} CPthreadReqSpinLock_Post_Wait;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Interrupts_NA : 1;
  uint8_t Pre_Lock_NA : 1;
  uint8_t Post_Status : 1;
  uint8_t Post_Interrupts : 1;
  uint8_t Post_Owner : 1;
  uint8_t Post_Wait : 1;
} CPthreadReqSpinLock_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/spin-lock test case.
 */
typedef struct {
  /**
   * @brief This member contains the return value of the pthread_spin_lock()
   *   call.
   */
  int status;

  /**
   * @brief This member contains the spin lock object.
   */
  pthread_spinlock_t lock_obj;

  /**
   * @brief This member is true, if the maskable interrupts shall be disabled
   *   before the call.
   */
  bool disable_interrupts;

  /**
   * @brief This member is true, if the spin lock shall be owned by a thread
   *   executing on another processor.
   */
  bool owned;

  /**
   * @brief This member is true, if the maskable interrupts were enabled after
   *   the call.
   */
  bool enabled_locked;

  /**
   * @brief This member contains the next ticket of the spin lock after the
   *   call.
   */
  unsigned int next_ticket;

  /**
   * @brief This member contains the ticket now serving of the spin lock after
   *   the call.
   */
  unsigned int now_serving;

  /**
   * @brief This member contains the identifier of the worker task.
   */
  rtems_id worker_id;

  /**
   * @brief This member is true, if the worker shall call the function.
   */
  volatile bool go;

  /**
   * @brief This member is true, if the function call returned.
   */
  volatile bool acquired;

  /**
   * @brief This member is true, if the function call of the worker returned
   *   before the owner released the spin lock.
   */
  bool acquired_before_release;

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
    CPthreadReqSpinLock_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqSpinLock_Context;

static CPthreadReqSpinLock_Context CPthreadReqSpinLock_Instance;

static const char *const CPthreadReqSpinLock_PreDesc_Interrupts[] =
  { "Enabled", "Disabled", "NA" };

static const char *const CPthreadReqSpinLock_PreDesc_Lock[] =
  { "Available", "Owned", "NA" };

static const char *const *const CPthreadReqSpinLock_PreDesc[] = {
  CPthreadReqSpinLock_PreDesc_Interrupts,
  CPthreadReqSpinLock_PreDesc_Lock,
  NULL
};

typedef CPthreadReqSpinLock_Context Context;

static void Lock( Context *ctx )
{
  rtems_interrupt_level level;
  int                   unlock_status;

  level = 0;

  if ( ctx->disable_interrupts ) {
    rtems_interrupt_local_disable( level );
  }

  ctx->status = pthread_spin_lock( &ctx->lock_obj );
  ctx->acquired = true;
  ctx->enabled_locked = AreInterruptsEnabled();
  ctx->next_ticket = ctx->lock_obj._Lock._next_ticket;
  ctx->now_serving = ctx->lock_obj._Lock._now_serving;
  unlock_status = pthread_spin_unlock( &ctx->lock_obj );

  if ( ctx->disable_interrupts ) {
    rtems_interrupt_local_enable( level );
  }

  T_quiet_eq_int( unlock_status, 0 );
}

#if defined( RTEMS_SMP )
static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    (void) ReceiveAnyEvents();

    while ( !ctx->go ) {
      /* Wait */
    }

    Lock( ctx );
  }
}

static void LockOnOtherProcessor( Context *ctx )
{
  int owner_lock_status;
  int owner_unlock_status;

  ctx->go = false;
  SendEvents( ctx->worker_id, RTEMS_EVENT_0 );

  /*
   * Between the lock and unlock calls, the runner executes with maskable
   * interrupts disabled, so it accesses only the context variables.
   */
  owner_lock_status = pthread_spin_lock( &ctx->lock_obj );
  ctx->go = true;
  TicketLockWaitForOthers(
    (const SMP_ticket_lock_Control *) &ctx->lock_obj._Lock,
    1
  );
  ctx->acquired_before_release = ctx->acquired;
  owner_unlock_status = pthread_spin_unlock( &ctx->lock_obj );

  WaitForExecutionStop( ctx->worker_id );
  T_quiet_eq_int( owner_lock_status, 0 );
  T_quiet_eq_int( owner_unlock_status, 0 );
}
#endif

static void CPthreadReqSpinLock_Pre_Interrupts_Prepare(
  CPthreadReqSpinLock_Context       *ctx,
  CPthreadReqSpinLock_Pre_Interrupts state
)
{
  switch ( state ) {
    case CPthreadReqSpinLock_Pre_Interrupts_Enabled: {
      /*
       * While maskable interrupts are enabled.
       */
      ctx->disable_interrupts = false;
      break;
    }

    case CPthreadReqSpinLock_Pre_Interrupts_Disabled: {
      /*
       * While maskable interrupts are disabled.
       */
      ctx->disable_interrupts = true;
      break;
    }

    case CPthreadReqSpinLock_Pre_Interrupts_NA:
      break;
  }
}

static void CPthreadReqSpinLock_Pre_Lock_Prepare(
  CPthreadReqSpinLock_Context *ctx,
  CPthreadReqSpinLock_Pre_Lock state
)
{
  switch ( state ) {
    case CPthreadReqSpinLock_Pre_Lock_Available: {
      /*
       * While the spin lock referenced by the `lock` parameter is not owned by
       * a thread.
       */
      ctx->owned = false;
      break;
    }

    case CPthreadReqSpinLock_Pre_Lock_Owned: {
      /*
       * While the spin lock referenced by the `lock` parameter is owned by a
       * thread executing on another processor.
       */
      ctx->owned = true;
      break;
    }

    case CPthreadReqSpinLock_Pre_Lock_NA:
      break;
  }
}

static void CPthreadReqSpinLock_Post_Status_Check(
  CPthreadReqSpinLock_Context    *ctx,
  CPthreadReqSpinLock_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqSpinLock_Post_Status_Ok: {
      /*
       * The return value of pthread_spin_lock() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqSpinLock_Post_Status_NA:
      break;
  }
}

static void CPthreadReqSpinLock_Post_Interrupts_Check(
  CPthreadReqSpinLock_Context        *ctx,
  CPthreadReqSpinLock_Post_Interrupts state
)
{
  switch ( state ) {
    case CPthreadReqSpinLock_Post_Interrupts_Disabled: {
      /*
       * The maskable interrupts shall be disabled.
       */
      T_false( ctx->enabled_locked );
      break;
    }

    case CPthreadReqSpinLock_Post_Interrupts_NA:
      break;
  }
}

static void CPthreadReqSpinLock_Post_Owner_Check(
  CPthreadReqSpinLock_Context   *ctx,
  CPthreadReqSpinLock_Post_Owner state
)
{
  switch ( state ) {
    case CPthreadReqSpinLock_Post_Owner_Caller: {
      /*
       * The spin lock referenced by the `lock` parameter shall be owned by the
       * calling thread.
       */
      T_eq_uint( ctx->next_ticket, ctx->now_serving + 1 );
      break;
    }

    case CPthreadReqSpinLock_Post_Owner_NA:
      break;
  }
}

static void CPthreadReqSpinLock_Post_Wait_Check(
  CPthreadReqSpinLock_Context  *ctx,
  CPthreadReqSpinLock_Post_Wait state
)
{
  switch ( state ) {
    case CPthreadReqSpinLock_Post_Wait_UntilReleased: {
      /*
       * The calling thread shall wait until the owner releases the spin lock.
       */
      T_false( ctx->acquired_before_release );
      T_true( ctx->acquired );
      break;
    }

    case CPthreadReqSpinLock_Post_Wait_NA:
      break;
  }
}

static void CPthreadReqSpinLock_Setup( CPthreadReqSpinLock_Context *ctx )
{
  #if defined( RTEMS_SMP )
  ctx->worker_id = CreateTask( "WORK", PRIO_NORMAL );
  SetScheduler( ctx->worker_id, SCHEDULER_B_ID, PRIO_NORMAL );
  StartTask( ctx->worker_id, Worker, ctx );
  #else
  (void) ctx;
  #endif
}

static void CPthreadReqSpinLock_Setup_Wrap( void *arg )
{
  CPthreadReqSpinLock_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqSpinLock_Setup( ctx );
}

static void CPthreadReqSpinLock_Teardown( CPthreadReqSpinLock_Context *ctx )
{
  #if defined( RTEMS_SMP )
  DeleteTask( ctx->worker_id );
  #else
  (void) ctx;
  #endif
}

static void CPthreadReqSpinLock_Teardown_Wrap( void *arg )
{
  CPthreadReqSpinLock_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqSpinLock_Teardown( ctx );
}

static void CPthreadReqSpinLock_Prepare( CPthreadReqSpinLock_Context *ctx )
{
  T_eq_int( pthread_spin_init( &ctx->lock_obj, PTHREAD_PROCESS_PRIVATE ), 0 );
  ctx->acquired = false;
}

static void CPthreadReqSpinLock_Action( CPthreadReqSpinLock_Context *ctx )
{
  if ( ctx->owned ) {
  #if defined( RTEMS_SMP )
    LockOnOtherProcessor( ctx );
  #endif
  } else {
    Lock( ctx );
  }
}

static void CPthreadReqSpinLock_Cleanup( CPthreadReqSpinLock_Context *ctx )
{
  T_eq_int( pthread_spin_destroy( &ctx->lock_obj ), 0 );
}

/* clang-format off */

static const CPthreadReqSpinLock_Entry
CPthreadReqSpinLock_Entries[] = {
#if defined(RTEMS_SMP)
  { 0, 0, 0, CPthreadReqSpinLock_Post_Status_Ok,
    CPthreadReqSpinLock_Post_Interrupts_Disabled,
    CPthreadReqSpinLock_Post_Owner_Caller, CPthreadReqSpinLock_Post_Wait_NA },
#else
  { 0, 0, 0, CPthreadReqSpinLock_Post_Status_Ok,
    CPthreadReqSpinLock_Post_Interrupts_Disabled,
    CPthreadReqSpinLock_Post_Owner_NA, CPthreadReqSpinLock_Post_Wait_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, CPthreadReqSpinLock_Post_Status_Ok,
    CPthreadReqSpinLock_Post_Interrupts_Disabled,
    CPthreadReqSpinLock_Post_Owner_Caller,
    CPthreadReqSpinLock_Post_Wait_UntilReleased }
#else
  { 1, 0, 0, CPthreadReqSpinLock_Post_Status_NA,
    CPthreadReqSpinLock_Post_Interrupts_NA, CPthreadReqSpinLock_Post_Owner_NA,
    CPthreadReqSpinLock_Post_Wait_NA }
#endif
};

static const uint8_t
CPthreadReqSpinLock_Map[] = {
  0, 1, 0, 1
};

/* clang-format on */

static size_t CPthreadReqSpinLock_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqSpinLock_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CPthreadReqSpinLock_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CPthreadReqSpinLock_Fixture = {
  .setup = CPthreadReqSpinLock_Setup_Wrap,
  .stop = NULL,
  .teardown = CPthreadReqSpinLock_Teardown_Wrap,
  .scope = CPthreadReqSpinLock_Scope,
  .initial_context = &CPthreadReqSpinLock_Instance
};

static inline CPthreadReqSpinLock_Entry CPthreadReqSpinLock_PopEntry(
  CPthreadReqSpinLock_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqSpinLock_Entries[ CPthreadReqSpinLock_Map[ index ] ];
}

static void CPthreadReqSpinLock_TestVariant( CPthreadReqSpinLock_Context *ctx )
{
  CPthreadReqSpinLock_Pre_Interrupts_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqSpinLock_Pre_Lock_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqSpinLock_Action( ctx );
  CPthreadReqSpinLock_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqSpinLock_Post_Interrupts_Check(
    ctx,
    ctx->Map.entry.Post_Interrupts
  );
  CPthreadReqSpinLock_Post_Owner_Check( ctx, ctx->Map.entry.Post_Owner );
  CPthreadReqSpinLock_Post_Wait_Check( ctx, ctx->Map.entry.Post_Wait );
}

/**
 * @fn void T_case_body_CPthreadReqSpinLock( void )
 */
T_TEST_CASE_FIXTURE( CPthreadReqSpinLock, &CPthreadReqSpinLock_Fixture )
{
  CPthreadReqSpinLock_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqSpinLock_Pre_Interrupts_Enabled;
    ctx->Map.pcs[ 0 ] < CPthreadReqSpinLock_Pre_Interrupts_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqSpinLock_Pre_Lock_Available;
      ctx->Map.pcs[ 1 ] < CPthreadReqSpinLock_Pre_Lock_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CPthreadReqSpinLock_PopEntry( ctx );

      if ( ctx->Map.entry.Skip ) {
        continue;
      }

      CPthreadReqSpinLock_Prepare( ctx );
      CPthreadReqSpinLock_TestVariant( ctx );
      CPthreadReqSpinLock_Cleanup( ctx );
    }
  }
}

/** @} */
