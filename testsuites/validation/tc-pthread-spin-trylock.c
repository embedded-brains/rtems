/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqSpinTrylock
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
 * @defgroup CPthreadReqSpinTrylock spec:/c/pthread/req/spin-trylock
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqSpinTrylock_Pre_Interrupts_Enabled,
  CPthreadReqSpinTrylock_Pre_Interrupts_Disabled,
  CPthreadReqSpinTrylock_Pre_Interrupts_NA
} CPthreadReqSpinTrylock_Pre_Interrupts;

typedef enum {
  CPthreadReqSpinTrylock_Pre_Lock_Available,
  CPthreadReqSpinTrylock_Pre_Lock_Owned,
  CPthreadReqSpinTrylock_Pre_Lock_NA
} CPthreadReqSpinTrylock_Pre_Lock;

typedef enum {
  CPthreadReqSpinTrylock_Post_Status_Ok,
  CPthreadReqSpinTrylock_Post_Status_NA
} CPthreadReqSpinTrylock_Post_Status;

typedef enum {
  CPthreadReqSpinTrylock_Post_Interrupts_Disabled,
  CPthreadReqSpinTrylock_Post_Interrupts_NA
} CPthreadReqSpinTrylock_Post_Interrupts;

typedef enum {
  CPthreadReqSpinTrylock_Post_Owner_Caller,
  CPthreadReqSpinTrylock_Post_Owner_NA
} CPthreadReqSpinTrylock_Post_Owner;

typedef enum {
  CPthreadReqSpinTrylock_Post_Wait_UntilReleased,
  CPthreadReqSpinTrylock_Post_Wait_NA
} CPthreadReqSpinTrylock_Post_Wait;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Interrupts_NA : 1;
  uint8_t Pre_Lock_NA : 1;
  uint8_t Post_Status : 1;
  uint8_t Post_Interrupts : 1;
  uint8_t Post_Owner : 1;
  uint8_t Post_Wait : 1;
} CPthreadReqSpinTrylock_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/spin-trylock test case.
 */
typedef struct {
  /**
   * @brief This member contains the return value of the pthread_spin_trylock()
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
    CPthreadReqSpinTrylock_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqSpinTrylock_Context;

static CPthreadReqSpinTrylock_Context CPthreadReqSpinTrylock_Instance;

static const char *const CPthreadReqSpinTrylock_PreDesc_Interrupts[] =
  { "Enabled", "Disabled", "NA" };

static const char *const CPthreadReqSpinTrylock_PreDesc_Lock[] =
  { "Available", "Owned", "NA" };

static const char *const *const CPthreadReqSpinTrylock_PreDesc[] = {
  CPthreadReqSpinTrylock_PreDesc_Interrupts,
  CPthreadReqSpinTrylock_PreDesc_Lock,
  NULL
};

typedef CPthreadReqSpinTrylock_Context Context;

static void Lock( Context *ctx )
{
  rtems_interrupt_level level;
  int                   unlock_status;

  level = 0;

  if ( ctx->disable_interrupts ) {
    rtems_interrupt_local_disable( level );
  }

  ctx->status = pthread_spin_trylock( &ctx->lock_obj );
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

static void CPthreadReqSpinTrylock_Pre_Interrupts_Prepare(
  CPthreadReqSpinTrylock_Context       *ctx,
  CPthreadReqSpinTrylock_Pre_Interrupts state
)
{
  switch ( state ) {
    case CPthreadReqSpinTrylock_Pre_Interrupts_Enabled: {
      /*
       * While maskable interrupts are enabled.
       */
      ctx->disable_interrupts = false;
      break;
    }

    case CPthreadReqSpinTrylock_Pre_Interrupts_Disabled: {
      /*
       * While maskable interrupts are disabled.
       */
      ctx->disable_interrupts = true;
      break;
    }

    case CPthreadReqSpinTrylock_Pre_Interrupts_NA:
      break;
  }
}

static void CPthreadReqSpinTrylock_Pre_Lock_Prepare(
  CPthreadReqSpinTrylock_Context *ctx,
  CPthreadReqSpinTrylock_Pre_Lock state
)
{
  switch ( state ) {
    case CPthreadReqSpinTrylock_Pre_Lock_Available: {
      /*
       * While the spin lock referenced by the `lock` parameter is not owned by
       * a thread.
       */
      ctx->owned = false;
      break;
    }

    case CPthreadReqSpinTrylock_Pre_Lock_Owned: {
      /*
       * While the spin lock referenced by the `lock` parameter is owned by a
       * thread executing on another processor.
       */
      ctx->owned = true;
      break;
    }

    case CPthreadReqSpinTrylock_Pre_Lock_NA:
      break;
  }
}

static void CPthreadReqSpinTrylock_Post_Status_Check(
  CPthreadReqSpinTrylock_Context    *ctx,
  CPthreadReqSpinTrylock_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqSpinTrylock_Post_Status_Ok: {
      /*
       * The return value of pthread_spin_trylock() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqSpinTrylock_Post_Status_NA:
      break;
  }
}

static void CPthreadReqSpinTrylock_Post_Interrupts_Check(
  CPthreadReqSpinTrylock_Context        *ctx,
  CPthreadReqSpinTrylock_Post_Interrupts state
)
{
  switch ( state ) {
    case CPthreadReqSpinTrylock_Post_Interrupts_Disabled: {
      /*
       * The maskable interrupts shall be disabled.
       */
      T_false( ctx->enabled_locked );
      break;
    }

    case CPthreadReqSpinTrylock_Post_Interrupts_NA:
      break;
  }
}

static void CPthreadReqSpinTrylock_Post_Owner_Check(
  CPthreadReqSpinTrylock_Context   *ctx,
  CPthreadReqSpinTrylock_Post_Owner state
)
{
  switch ( state ) {
    case CPthreadReqSpinTrylock_Post_Owner_Caller: {
      /*
       * The spin lock referenced by the `lock` parameter shall be owned by the
       * calling thread.
       */
      T_eq_uint( ctx->next_ticket, ctx->now_serving + 1 );
      break;
    }

    case CPthreadReqSpinTrylock_Post_Owner_NA:
      break;
  }
}

static void CPthreadReqSpinTrylock_Post_Wait_Check(
  CPthreadReqSpinTrylock_Context  *ctx,
  CPthreadReqSpinTrylock_Post_Wait state
)
{
  switch ( state ) {
    case CPthreadReqSpinTrylock_Post_Wait_UntilReleased: {
      /*
       * The calling thread shall wait until the owner releases the spin lock.
       */
      T_false( ctx->acquired_before_release );
      T_true( ctx->acquired );
      break;
    }

    case CPthreadReqSpinTrylock_Post_Wait_NA:
      break;
  }
}

static void CPthreadReqSpinTrylock_Setup( CPthreadReqSpinTrylock_Context *ctx )
{
  #if defined( RTEMS_SMP )
  ctx->worker_id = CreateTask( "WORK", PRIO_NORMAL );
  SetScheduler( ctx->worker_id, SCHEDULER_B_ID, PRIO_NORMAL );
  StartTask( ctx->worker_id, Worker, ctx );
  #else
  (void) ctx;
  #endif
}

static void CPthreadReqSpinTrylock_Setup_Wrap( void *arg )
{
  CPthreadReqSpinTrylock_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqSpinTrylock_Setup( ctx );
}

static void CPthreadReqSpinTrylock_Teardown(
  CPthreadReqSpinTrylock_Context *ctx
)
{
  #if defined( RTEMS_SMP )
  DeleteTask( ctx->worker_id );
  #else
  (void) ctx;
  #endif
}

static void CPthreadReqSpinTrylock_Teardown_Wrap( void *arg )
{
  CPthreadReqSpinTrylock_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqSpinTrylock_Teardown( ctx );
}

static void CPthreadReqSpinTrylock_Prepare(
  CPthreadReqSpinTrylock_Context *ctx
)
{
  T_eq_int( pthread_spin_init( &ctx->lock_obj, PTHREAD_PROCESS_PRIVATE ), 0 );
  ctx->acquired = false;
}

static void CPthreadReqSpinTrylock_Action(
  CPthreadReqSpinTrylock_Context *ctx
)
{
  if ( ctx->owned ) {
  #if defined( RTEMS_SMP )
    LockOnOtherProcessor( ctx );
  #endif
  } else {
    Lock( ctx );
  }
}

static void CPthreadReqSpinTrylock_Cleanup(
  CPthreadReqSpinTrylock_Context *ctx
)
{
  T_eq_int( pthread_spin_destroy( &ctx->lock_obj ), 0 );
}

/* clang-format off */

static const CPthreadReqSpinTrylock_Entry
CPthreadReqSpinTrylock_Entries[] = {
#if defined(RTEMS_SMP)
  { 0, 0, 0, CPthreadReqSpinTrylock_Post_Status_Ok,
    CPthreadReqSpinTrylock_Post_Interrupts_Disabled,
    CPthreadReqSpinTrylock_Post_Owner_Caller,
    CPthreadReqSpinTrylock_Post_Wait_NA },
#else
  { 0, 0, 0, CPthreadReqSpinTrylock_Post_Status_Ok,
    CPthreadReqSpinTrylock_Post_Interrupts_Disabled,
    CPthreadReqSpinTrylock_Post_Owner_NA, CPthreadReqSpinTrylock_Post_Wait_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, CPthreadReqSpinTrylock_Post_Status_Ok,
    CPthreadReqSpinTrylock_Post_Interrupts_Disabled,
    CPthreadReqSpinTrylock_Post_Owner_Caller,
    CPthreadReqSpinTrylock_Post_Wait_UntilReleased }
#else
  { 1, 0, 0, CPthreadReqSpinTrylock_Post_Status_NA,
    CPthreadReqSpinTrylock_Post_Interrupts_NA,
    CPthreadReqSpinTrylock_Post_Owner_NA, CPthreadReqSpinTrylock_Post_Wait_NA }
#endif
};

static const uint8_t
CPthreadReqSpinTrylock_Map[] = {
  0, 1, 0, 1
};

/* clang-format on */

static size_t CPthreadReqSpinTrylock_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqSpinTrylock_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CPthreadReqSpinTrylock_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CPthreadReqSpinTrylock_Fixture = {
  .setup = CPthreadReqSpinTrylock_Setup_Wrap,
  .stop = NULL,
  .teardown = CPthreadReqSpinTrylock_Teardown_Wrap,
  .scope = CPthreadReqSpinTrylock_Scope,
  .initial_context = &CPthreadReqSpinTrylock_Instance
};

static inline CPthreadReqSpinTrylock_Entry CPthreadReqSpinTrylock_PopEntry(
  CPthreadReqSpinTrylock_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqSpinTrylock_Entries[ CPthreadReqSpinTrylock_Map[ index ] ];
}

static void CPthreadReqSpinTrylock_TestVariant(
  CPthreadReqSpinTrylock_Context *ctx
)
{
  CPthreadReqSpinTrylock_Pre_Interrupts_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqSpinTrylock_Pre_Lock_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqSpinTrylock_Action( ctx );
  CPthreadReqSpinTrylock_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqSpinTrylock_Post_Interrupts_Check(
    ctx,
    ctx->Map.entry.Post_Interrupts
  );
  CPthreadReqSpinTrylock_Post_Owner_Check( ctx, ctx->Map.entry.Post_Owner );
  CPthreadReqSpinTrylock_Post_Wait_Check( ctx, ctx->Map.entry.Post_Wait );
}

/**
 * @fn void T_case_body_CPthreadReqSpinTrylock( void )
 */
T_TEST_CASE_FIXTURE( CPthreadReqSpinTrylock, &CPthreadReqSpinTrylock_Fixture )
{
  CPthreadReqSpinTrylock_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqSpinTrylock_Pre_Interrupts_Enabled;
    ctx->Map.pcs[ 0 ] < CPthreadReqSpinTrylock_Pre_Interrupts_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqSpinTrylock_Pre_Lock_Available;
      ctx->Map.pcs[ 1 ] < CPthreadReqSpinTrylock_Pre_Lock_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CPthreadReqSpinTrylock_PopEntry( ctx );

      if ( ctx->Map.entry.Skip ) {
        continue;
      }

      CPthreadReqSpinTrylock_Prepare( ctx );
      CPthreadReqSpinTrylock_TestVariant( ctx );
      CPthreadReqSpinTrylock_Cleanup( ctx );
    }
  }
}

/** @} */
