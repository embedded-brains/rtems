/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqSpinLock
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <rtems.h>

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqSpinLock spec:/c/pthread/req/spin-lock
 *
 * @{
 */

typedef enum {
  CPthreadReqSpinLock_Pre_SpinLock_Valid,
  CPthreadReqSpinLock_Pre_SpinLock_NA
} CPthreadReqSpinLock_Pre_SpinLock;

typedef enum {
  CPthreadReqSpinLock_Post_Result_Ok,
  CPthreadReqSpinLock_Post_Result_NA
} CPthreadReqSpinLock_Post_Result;

typedef enum {
  CPthreadReqSpinLock_Post_LockState_Acquired,
  CPthreadReqSpinLock_Post_LockState_NA
} CPthreadReqSpinLock_Post_LockState;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_SpinLock_NA : 1;
  uint8_t Post_Result : 1;
  uint8_t Post_LockState : 1;
} CPthreadReqSpinLock_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/spin-lock test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `lock` parameter value.
   */
  pthread_spinlock_t *lock;

  /**
   * @brief This member contains the return value.
   */
  int retval;

  /**
   * @brief This member contains the spin lock.
   */
  pthread_spinlock_t spinlock;

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
    CPthreadReqSpinLock_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqSpinLock_Context;

static CPthreadReqSpinLock_Context
  CPthreadReqSpinLock_Instance;

static const char * const CPthreadReqSpinLock_PreDesc_SpinLock[] = {
  "Valid",
  "NA"
};

static const char * const * const CPthreadReqSpinLock_PreDesc[] = {
  CPthreadReqSpinLock_PreDesc_SpinLock,
  NULL
};

static void CPthreadReqSpinLock_Pre_SpinLock_Prepare(
  CPthreadReqSpinLock_Pre_SpinLock state
)
{
  switch ( state ) {
    case CPthreadReqSpinLock_Pre_SpinLock_Valid: {
      /*
       * While the spin lock object referenced by `lock` is initialized.
       */
      /* lock is initialized in test-prepare */
      T_true( _ISR_Get_level() == 0 );
      break;
    }

    case CPthreadReqSpinLock_Pre_SpinLock_NA:
      break;
  }
}

static void CPthreadReqSpinLock_Post_Result_Check(
  CPthreadReqSpinLock_Context    *ctx,
  CPthreadReqSpinLock_Post_Result state
)
{
  switch ( state ) {
    case CPthreadReqSpinLock_Post_Result_Ok: {
      /*
       * The return value shall be 0.
       */
      T_eq_int( ctx->retval, 0 );
      break;
    }

    case CPthreadReqSpinLock_Post_Result_NA:
      break;
  }
}

static void CPthreadReqSpinLock_Post_LockState_Check(
  CPthreadReqSpinLock_Post_LockState state
)
{
  switch ( state ) {
    case CPthreadReqSpinLock_Post_LockState_Acquired: {
      /*
       * The spin lock shall be acquired by the calling thread.
       */
      /* The lock state cannot be verified using public APIs without concurrent
       * execution. As a last resort, we verify that the ISR level is non-zero,
       * indicating the spin lock has successfully disabled interrupts. */
      T_true( _ISR_Get_level() != 0 );
      break;
    }

    case CPthreadReqSpinLock_Post_LockState_NA:
      break;
  }
}

static void CPthreadReqSpinLock_Prepare( CPthreadReqSpinLock_Context *ctx )
{
  ctx->lock = &ctx->spinlock;
  pthread_spin_init( ctx->lock, PTHREAD_PROCESS_PRIVATE );
}

static void CPthreadReqSpinLock_Action( CPthreadReqSpinLock_Context *ctx )
{
  ctx->retval = pthread_spin_lock( ctx->lock );
}

static void CPthreadReqSpinLock_Cleanup( CPthreadReqSpinLock_Context *ctx )
{
  pthread_spin_unlock( ctx->lock );
}

static const CPthreadReqSpinLock_Entry
CPthreadReqSpinLock_Entries[] = {
  { 0, 0, CPthreadReqSpinLock_Post_Result_Ok,
    CPthreadReqSpinLock_Post_LockState_Acquired }
};

static const uint8_t
CPthreadReqSpinLock_Map[] = {
  0
};

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
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
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
  return CPthreadReqSpinLock_Entries[
    CPthreadReqSpinLock_Map[ index ]
  ];
}

static void CPthreadReqSpinLock_TestVariant( CPthreadReqSpinLock_Context *ctx )
{
  CPthreadReqSpinLock_Pre_SpinLock_Prepare( ctx->Map.pcs[ 0 ] );
  CPthreadReqSpinLock_Action( ctx );
  CPthreadReqSpinLock_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
  CPthreadReqSpinLock_Post_LockState_Check( ctx->Map.entry.Post_LockState );
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
    ctx->Map.pcs[ 0 ] = CPthreadReqSpinLock_Pre_SpinLock_Valid;
    ctx->Map.pcs[ 0 ] < CPthreadReqSpinLock_Pre_SpinLock_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CPthreadReqSpinLock_PopEntry( ctx );
    CPthreadReqSpinLock_Prepare( ctx );
    CPthreadReqSpinLock_TestVariant( ctx );
    CPthreadReqSpinLock_Cleanup( ctx );
  }
}

/** @} */
