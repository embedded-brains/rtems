/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqSpinUnlock
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
#include <string.h>
#include <rtems/posix/spinlockimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqSpinUnlock spec:/c/pthread/req/spin-unlock
 *
 * @ingroup TestsuitesValidationNoClock1
 *
 * @{
 */

typedef enum {
  CPthreadReqSpinUnlock_Pre_Interrupts_Enabled,
  CPthreadReqSpinUnlock_Pre_Interrupts_Disabled,
  CPthreadReqSpinUnlock_Pre_Interrupts_NA
} CPthreadReqSpinUnlock_Pre_Interrupts;

typedef enum {
  CPthreadReqSpinUnlock_Pre_Lock_Locked,
  CPthreadReqSpinUnlock_Pre_Lock_NA
} CPthreadReqSpinUnlock_Pre_Lock;

typedef enum {
  CPthreadReqSpinUnlock_Post_Status_Ok,
  CPthreadReqSpinUnlock_Post_Status_NA
} CPthreadReqSpinUnlock_Post_Status;

typedef enum {
  CPthreadReqSpinUnlock_Post_Interrupts_Enabled,
  CPthreadReqSpinUnlock_Post_Interrupts_Disabled,
  CPthreadReqSpinUnlock_Post_Interrupts_NA
} CPthreadReqSpinUnlock_Post_Interrupts;

typedef enum {
  CPthreadReqSpinUnlock_Post_Lock_Unlocked,
  CPthreadReqSpinUnlock_Post_Lock_NA
} CPthreadReqSpinUnlock_Post_Lock;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Interrupts_NA : 1;
  uint8_t Pre_Lock_NA : 1;
  uint8_t Post_Status : 1;
  uint8_t Post_Interrupts : 2;
  uint8_t Post_Lock : 1;
} CPthreadReqSpinUnlock_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/spin-unlock test case.
 */
typedef struct {
  /**
   * @brief This member provides the object referenced by the `lock` parameter.
   */
  pthread_spinlock_t lock_obj;

  /**
   * @brief This member contains the return value of the pthread_spin_unlock()
   *   call.
   */
  int status;

  /**
   * @brief This member contains the interrupt level of the test.
   */
  rtems_interrupt_level level;

  /**
   * @brief This member is true, if the test disabled interrupts.
   */
  bool level_disabled;

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
    CPthreadReqSpinUnlock_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqSpinUnlock_Context;

static CPthreadReqSpinUnlock_Context CPthreadReqSpinUnlock_Instance;

static const char *const CPthreadReqSpinUnlock_PreDesc_Interrupts[] =
  { "Enabled", "Disabled", "NA" };

static const char *const CPthreadReqSpinUnlock_PreDesc_Lock[] = {
  "Locked",
  "NA"
};

static const char *const *const CPthreadReqSpinUnlock_PreDesc[] = {
  CPthreadReqSpinUnlock_PreDesc_Interrupts,
  CPthreadReqSpinUnlock_PreDesc_Lock,
  NULL
};

typedef CPthreadReqSpinUnlock_Context Context;

#if defined( RTEMS_SMP )
static const SMP_ticket_lock_Control *TicketLock( Context *ctx )
{
  return &_POSIX_Spinlock_Get( &ctx->lock_obj )->Lock;
}
#endif

static void CPthreadReqSpinUnlock_Pre_Interrupts_Prepare(
  CPthreadReqSpinUnlock_Context       *ctx,
  CPthreadReqSpinUnlock_Pre_Interrupts state
)
{
  switch ( state ) {
    case CPthreadReqSpinUnlock_Pre_Interrupts_Enabled: {
      /*
       * While maskable interrupts are enabled.
       */
      ctx->level_disabled = false;
      break;
    }

    case CPthreadReqSpinUnlock_Pre_Interrupts_Disabled: {
      /*
       * While maskable interrupts are disabled.
       */
      rtems_interrupt_local_disable( ctx->level );
      ctx->level_disabled = true;
      break;
    }

    case CPthreadReqSpinUnlock_Pre_Interrupts_NA:
      break;
  }
}

static void CPthreadReqSpinUnlock_Pre_Lock_Prepare(
  CPthreadReqSpinUnlock_Context *ctx,
  CPthreadReqSpinUnlock_Pre_Lock state
)
{
  switch ( state ) {
    case CPthreadReqSpinUnlock_Pre_Lock_Locked: {
      /*
       * While the object referenced by the `lock` parameter is an initialized
       * spin lock which is locked by the calling thread.
       */
      memset( &ctx->lock_obj, 0xff, sizeof( ctx->lock_obj ) );
      T_eq_int( pthread_spin_init( &ctx->lock_obj, 0 ), 0 );
      T_eq_int( pthread_spin_lock( &ctx->lock_obj ), 0 );
      break;
    }

    case CPthreadReqSpinUnlock_Pre_Lock_NA:
      break;
  }
}

static void CPthreadReqSpinUnlock_Post_Status_Check(
  CPthreadReqSpinUnlock_Context    *ctx,
  CPthreadReqSpinUnlock_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqSpinUnlock_Post_Status_Ok: {
      /*
       * The return value of pthread_spin_unlock() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqSpinUnlock_Post_Status_NA:
      break;
  }
}

static void CPthreadReqSpinUnlock_Post_Interrupts_Check(
  CPthreadReqSpinUnlock_Post_Interrupts state
)
{
  switch ( state ) {
    case CPthreadReqSpinUnlock_Post_Interrupts_Enabled: {
      /*
       * Maskable interrupts shall be enabled.
       */
      T_true( AreInterruptsEnabled() );
      break;
    }

    case CPthreadReqSpinUnlock_Post_Interrupts_Disabled: {
      /*
       * Maskable interrupts shall be disabled.
       */
      T_false( AreInterruptsEnabled() );
      break;
    }

    case CPthreadReqSpinUnlock_Post_Interrupts_NA:
      break;
  }
}

static void CPthreadReqSpinUnlock_Post_Lock_Check(
  CPthreadReqSpinUnlock_Context  *ctx,
  CPthreadReqSpinUnlock_Post_Lock state
)
{
  switch ( state ) {
    case CPthreadReqSpinUnlock_Post_Lock_Unlocked: {
      /*
       * The spin lock referenced by the `lock` parameter shall be unlocked.
       */
      #if defined( RTEMS_SMP )
      T_true( TicketLockIsAvailable( TicketLock( ctx ) ) );
      #else
      (void) ctx;
      #endif
      break;
    }

    case CPthreadReqSpinUnlock_Post_Lock_NA:
      break;
  }
}

static void CPthreadReqSpinUnlock_Setup( void )
{
  SetSelfPriority( PRIO_NORMAL );
}

static void CPthreadReqSpinUnlock_Setup_Wrap( void *arg )
{
  CPthreadReqSpinUnlock_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqSpinUnlock_Setup();
}

static void CPthreadReqSpinUnlock_Teardown( void )
{
  RestoreRunnerPriority();
}

static void CPthreadReqSpinUnlock_Teardown_Wrap( void *arg )
{
  CPthreadReqSpinUnlock_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CPthreadReqSpinUnlock_Teardown();
}

static void CPthreadReqSpinUnlock_Prepare( CPthreadReqSpinUnlock_Context *ctx )
{
  ctx->status = -1;
}

static void CPthreadReqSpinUnlock_Action( CPthreadReqSpinUnlock_Context *ctx )
{
  ctx->status = pthread_spin_unlock( &ctx->lock_obj );
}

static void CPthreadReqSpinUnlock_Cleanup( CPthreadReqSpinUnlock_Context *ctx )
{
  (void) pthread_spin_destroy( &ctx->lock_obj );

  if ( ctx->level_disabled ) {
    rtems_interrupt_local_enable( ctx->level );
  }
}

/* clang-format off */

static const CPthreadReqSpinUnlock_Entry
CPthreadReqSpinUnlock_Entries[] = {
#if defined(RTEMS_SMP)
  { 0, 0, 0, CPthreadReqSpinUnlock_Post_Status_Ok,
    CPthreadReqSpinUnlock_Post_Interrupts_Enabled,
    CPthreadReqSpinUnlock_Post_Lock_Unlocked },
#else
  { 0, 0, 0, CPthreadReqSpinUnlock_Post_Status_Ok,
    CPthreadReqSpinUnlock_Post_Interrupts_Enabled,
    CPthreadReqSpinUnlock_Post_Lock_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, CPthreadReqSpinUnlock_Post_Status_Ok,
    CPthreadReqSpinUnlock_Post_Interrupts_Disabled,
    CPthreadReqSpinUnlock_Post_Lock_Unlocked }
#else
  { 0, 0, 0, CPthreadReqSpinUnlock_Post_Status_Ok,
    CPthreadReqSpinUnlock_Post_Interrupts_Disabled,
    CPthreadReqSpinUnlock_Post_Lock_NA }
#endif
};

static const uint8_t
CPthreadReqSpinUnlock_Map[] = {
  0, 1
};

/* clang-format on */

static size_t CPthreadReqSpinUnlock_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqSpinUnlock_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CPthreadReqSpinUnlock_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CPthreadReqSpinUnlock_Fixture = {
  .setup = CPthreadReqSpinUnlock_Setup_Wrap,
  .stop = NULL,
  .teardown = CPthreadReqSpinUnlock_Teardown_Wrap,
  .scope = CPthreadReqSpinUnlock_Scope,
  .initial_context = &CPthreadReqSpinUnlock_Instance
};

static inline CPthreadReqSpinUnlock_Entry CPthreadReqSpinUnlock_PopEntry(
  CPthreadReqSpinUnlock_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqSpinUnlock_Entries[ CPthreadReqSpinUnlock_Map[ index ] ];
}

static void CPthreadReqSpinUnlock_TestVariant(
  CPthreadReqSpinUnlock_Context *ctx
)
{
  CPthreadReqSpinUnlock_Pre_Interrupts_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqSpinUnlock_Pre_Lock_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqSpinUnlock_Action( ctx );
  CPthreadReqSpinUnlock_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqSpinUnlock_Post_Interrupts_Check(
    ctx->Map.entry.Post_Interrupts
  );
  CPthreadReqSpinUnlock_Post_Lock_Check( ctx, ctx->Map.entry.Post_Lock );
}

/**
 * @fn void T_case_body_CPthreadReqSpinUnlock( void )
 */
T_TEST_CASE_FIXTURE( CPthreadReqSpinUnlock, &CPthreadReqSpinUnlock_Fixture )
{
  CPthreadReqSpinUnlock_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqSpinUnlock_Pre_Interrupts_Enabled;
    ctx->Map.pcs[ 0 ] < CPthreadReqSpinUnlock_Pre_Interrupts_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqSpinUnlock_Pre_Lock_Locked;
      ctx->Map.pcs[ 1 ] < CPthreadReqSpinUnlock_Pre_Lock_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CPthreadReqSpinUnlock_PopEntry( ctx );
      CPthreadReqSpinUnlock_Prepare( ctx );
      CPthreadReqSpinUnlock_TestVariant( ctx );
      CPthreadReqSpinUnlock_Cleanup( ctx );
    }
  }
}

/** @} */
