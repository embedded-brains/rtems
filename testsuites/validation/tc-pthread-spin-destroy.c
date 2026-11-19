/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqSpinDestroy
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

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqSpinDestroy spec:/c/pthread/req/spin-destroy
 *
 * @ingroup TestsuitesValidationNoClock1
 *
 * @{
 */

typedef enum {
  CPthreadReqSpinDestroy_Pre_Lock_Initialized,
  CPthreadReqSpinDestroy_Pre_Lock_NA
} CPthreadReqSpinDestroy_Pre_Lock;

typedef enum {
  CPthreadReqSpinDestroy_Post_Status_Ok,
  CPthreadReqSpinDestroy_Post_Status_NA
} CPthreadReqSpinDestroy_Post_Status;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Lock_NA : 1;
  uint8_t Post_Status : 1;
} CPthreadReqSpinDestroy_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/spin-destroy test case.
 */
typedef struct {
  /**
   * @brief This member provides the object referenced by the `lock` parameter.
   */
  pthread_spinlock_t lock_obj;

  /**
   * @brief This member contains the return value of the pthread_spin_destroy()
   *   call.
   */
  int status;

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
    CPthreadReqSpinDestroy_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqSpinDestroy_Context;

static CPthreadReqSpinDestroy_Context CPthreadReqSpinDestroy_Instance;

static const char *const CPthreadReqSpinDestroy_PreDesc_Lock[] = {
  "Initialized",
  "NA"
};

static const char *const *const CPthreadReqSpinDestroy_PreDesc[] = {
  CPthreadReqSpinDestroy_PreDesc_Lock,
  NULL
};

static void CPthreadReqSpinDestroy_Pre_Lock_Prepare(
  CPthreadReqSpinDestroy_Context *ctx,
  CPthreadReqSpinDestroy_Pre_Lock state
)
{
  switch ( state ) {
    case CPthreadReqSpinDestroy_Pre_Lock_Initialized: {
      /*
       * While the object referenced by the `lock` parameter is an initialized
       * spin lock which is unlocked.
       */
      T_eq_int( pthread_spin_init( &ctx->lock_obj, 0 ), 0 );
      break;
    }

    case CPthreadReqSpinDestroy_Pre_Lock_NA:
      break;
  }
}

static void CPthreadReqSpinDestroy_Post_Status_Check(
  CPthreadReqSpinDestroy_Context    *ctx,
  CPthreadReqSpinDestroy_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqSpinDestroy_Post_Status_Ok: {
      /*
       * The return value of pthread_spin_destroy() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqSpinDestroy_Post_Status_NA:
      break;
  }
}

static void CPthreadReqSpinDestroy_Action(
  CPthreadReqSpinDestroy_Context *ctx
)
{
  ctx->status = pthread_spin_destroy( &ctx->lock_obj );
}

/* clang-format off */

static const CPthreadReqSpinDestroy_Entry
CPthreadReqSpinDestroy_Entries[] = {
  { 0, 0, CPthreadReqSpinDestroy_Post_Status_Ok }
};

static const uint8_t
CPthreadReqSpinDestroy_Map[] = {
  0
};

/* clang-format on */

static size_t CPthreadReqSpinDestroy_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqSpinDestroy_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CPthreadReqSpinDestroy_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CPthreadReqSpinDestroy_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqSpinDestroy_Scope,
  .initial_context = &CPthreadReqSpinDestroy_Instance
};

static inline CPthreadReqSpinDestroy_Entry CPthreadReqSpinDestroy_PopEntry(
  CPthreadReqSpinDestroy_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqSpinDestroy_Entries[ CPthreadReqSpinDestroy_Map[ index ] ];
}

static void CPthreadReqSpinDestroy_TestVariant(
  CPthreadReqSpinDestroy_Context *ctx
)
{
  CPthreadReqSpinDestroy_Pre_Lock_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqSpinDestroy_Action( ctx );
  CPthreadReqSpinDestroy_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
}

/**
 * @fn void T_case_body_CPthreadReqSpinDestroy( void )
 */
T_TEST_CASE_FIXTURE( CPthreadReqSpinDestroy, &CPthreadReqSpinDestroy_Fixture )
{
  CPthreadReqSpinDestroy_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqSpinDestroy_Pre_Lock_Initialized;
    ctx->Map.pcs[ 0 ] < CPthreadReqSpinDestroy_Pre_Lock_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CPthreadReqSpinDestroy_PopEntry( ctx );
    CPthreadReqSpinDestroy_TestVariant( ctx );
  }
}

/** @} */
