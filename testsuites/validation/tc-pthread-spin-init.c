/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqSpinInit
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
#include <string.h>

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqSpinInit spec:/c/pthread/req/spin-init
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CPthreadReqSpinInit_Pre_Pshared_Private,
  CPthreadReqSpinInit_Pre_Pshared_Shared,
  CPthreadReqSpinInit_Pre_Pshared_Invalid,
  CPthreadReqSpinInit_Pre_Pshared_NA
} CPthreadReqSpinInit_Pre_Pshared;

typedef enum {
  CPthreadReqSpinInit_Pre_Lock_Uninit,
  CPthreadReqSpinInit_Pre_Lock_Destroyed,
  CPthreadReqSpinInit_Pre_Lock_NA
} CPthreadReqSpinInit_Pre_Lock;

typedef enum {
  CPthreadReqSpinInit_Post_Status_Ok,
  CPthreadReqSpinInit_Post_Status_NA
} CPthreadReqSpinInit_Post_Status;

typedef enum {
  CPthreadReqSpinInit_Post_Lock_Unlocked,
  CPthreadReqSpinInit_Post_Lock_NA
} CPthreadReqSpinInit_Post_Lock;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Pshared_NA : 1;
  uint8_t Pre_Lock_NA : 1;
  uint8_t Post_Status : 1;
  uint8_t Post_Lock : 1;
} CPthreadReqSpinInit_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/spin-init test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `pshared` parameter value.
   */
  int pshared;

  /**
   * @brief This member contains the return value of the pthread_spin_init()
   *   call.
   */
  int status;

  /**
   * @brief This member contains the spin lock object.
   */
  pthread_spinlock_t lock_obj;

  /**
   * @brief This member contains a copy of the spin lock object before the
   *   pthread_spin_init() call.
   */
  pthread_spinlock_t lock_before;

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
    CPthreadReqSpinInit_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqSpinInit_Context;

static CPthreadReqSpinInit_Context CPthreadReqSpinInit_Instance;

static const char *const CPthreadReqSpinInit_PreDesc_Pshared[] =
  { "Private", "Shared", "Invalid", "NA" };

static const char *const CPthreadReqSpinInit_PreDesc_Lock[] =
  { "Uninit", "Destroyed", "NA" };

static const char *const *const CPthreadReqSpinInit_PreDesc[] = {
  CPthreadReqSpinInit_PreDesc_Pshared,
  CPthreadReqSpinInit_PreDesc_Lock,
  NULL
};

#define PSHARED_INVALID 123

static void CPthreadReqSpinInit_Pre_Pshared_Prepare(
  CPthreadReqSpinInit_Context    *ctx,
  CPthreadReqSpinInit_Pre_Pshared state
)
{
  switch ( state ) {
    case CPthreadReqSpinInit_Pre_Pshared_Private: {
      /*
       * While the `pshared` parameter is PTHREAD_PROCESS_PRIVATE.
       */
      ctx->pshared = PTHREAD_PROCESS_PRIVATE;
      break;
    }

    case CPthreadReqSpinInit_Pre_Pshared_Shared: {
      /*
       * While the `pshared` parameter is PTHREAD_PROCESS_SHARED.
       */
      ctx->pshared = PTHREAD_PROCESS_SHARED;
      break;
    }

    case CPthreadReqSpinInit_Pre_Pshared_Invalid: {
      /*
       * While the `pshared` parameter is not a valid process-shared attribute
       * value.
       */
      ctx->pshared = PSHARED_INVALID;
      break;
    }

    case CPthreadReqSpinInit_Pre_Pshared_NA:
      break;
  }
}

static void CPthreadReqSpinInit_Pre_Lock_Prepare(
  CPthreadReqSpinInit_Context *ctx,
  CPthreadReqSpinInit_Pre_Lock state
)
{
  switch ( state ) {
    case CPthreadReqSpinInit_Pre_Lock_Uninit: {
      /*
       * While the `lock` parameter references an object of type
       * pthread_spinlock_t which is not initialized.
       */
      memset( &ctx->lock_obj, 0xa5, sizeof( ctx->lock_obj ) );
      break;
    }

    case CPthreadReqSpinInit_Pre_Lock_Destroyed: {
      /*
       * While the `lock` parameter references a destroyed spin lock.
       */
      T_eq_int(
        pthread_spin_init( &ctx->lock_obj, PTHREAD_PROCESS_PRIVATE ),
        0
      );
      T_eq_int( pthread_spin_destroy( &ctx->lock_obj ), 0 );
      break;
    }

    case CPthreadReqSpinInit_Pre_Lock_NA:
      break;
  }
}

static void CPthreadReqSpinInit_Post_Status_Check(
  CPthreadReqSpinInit_Context    *ctx,
  CPthreadReqSpinInit_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqSpinInit_Post_Status_Ok: {
      /*
       * The return value of pthread_spin_init() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CPthreadReqSpinInit_Post_Status_NA:
      break;
  }
}

static void CPthreadReqSpinInit_Post_Lock_Check(
  CPthreadReqSpinInit_Context  *ctx,
  CPthreadReqSpinInit_Post_Lock state
)
{
  switch ( state ) {
    case CPthreadReqSpinInit_Post_Lock_Unlocked: {
      /*
       * The spin lock referenced by the `lock` parameter shall be unlocked.
       */
      pthread_spinlock_t expected;

      memcpy( &expected, &ctx->lock_before, sizeof( expected ) );
      expected._Lock._next_ticket = 0;
      expected._Lock._now_serving = 0;
      T_eq_mem( &ctx->lock_obj, &expected, sizeof( ctx->lock_obj ) );
      break;
    }

    case CPthreadReqSpinInit_Post_Lock_NA:
      break;
  }
}

static void CPthreadReqSpinInit_Action( CPthreadReqSpinInit_Context *ctx )
{
  memcpy( &ctx->lock_before, &ctx->lock_obj, sizeof( ctx->lock_before ) );
  ctx->status = pthread_spin_init( &ctx->lock_obj, ctx->pshared );
}

static void CPthreadReqSpinInit_Cleanup( CPthreadReqSpinInit_Context *ctx )
{
  T_eq_int( pthread_spin_destroy( &ctx->lock_obj ), 0 );
}

/* clang-format off */

static const CPthreadReqSpinInit_Entry
CPthreadReqSpinInit_Entries[] = {
#if defined(RTEMS_SMP)
  { 0, 0, 0, CPthreadReqSpinInit_Post_Status_Ok,
    CPthreadReqSpinInit_Post_Lock_Unlocked }
#else
  { 0, 0, 0, CPthreadReqSpinInit_Post_Status_Ok,
    CPthreadReqSpinInit_Post_Lock_NA }
#endif
};

static const uint8_t
CPthreadReqSpinInit_Map[] = {
  0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t CPthreadReqSpinInit_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqSpinInit_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CPthreadReqSpinInit_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CPthreadReqSpinInit_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqSpinInit_Scope,
  .initial_context = &CPthreadReqSpinInit_Instance
};

static inline CPthreadReqSpinInit_Entry CPthreadReqSpinInit_PopEntry(
  CPthreadReqSpinInit_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqSpinInit_Entries[ CPthreadReqSpinInit_Map[ index ] ];
}

static void CPthreadReqSpinInit_TestVariant( CPthreadReqSpinInit_Context *ctx )
{
  CPthreadReqSpinInit_Pre_Pshared_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqSpinInit_Pre_Lock_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqSpinInit_Action( ctx );
  CPthreadReqSpinInit_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CPthreadReqSpinInit_Post_Lock_Check( ctx, ctx->Map.entry.Post_Lock );
}

/**
 * @fn void T_case_body_CPthreadReqSpinInit( void )
 */
T_TEST_CASE_FIXTURE( CPthreadReqSpinInit, &CPthreadReqSpinInit_Fixture )
{
  CPthreadReqSpinInit_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqSpinInit_Pre_Pshared_Private;
    ctx->Map.pcs[ 0 ] < CPthreadReqSpinInit_Pre_Pshared_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqSpinInit_Pre_Lock_Uninit;
      ctx->Map.pcs[ 1 ] < CPthreadReqSpinInit_Pre_Lock_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CPthreadReqSpinInit_PopEntry( ctx );
      CPthreadReqSpinInit_TestVariant( ctx );
      CPthreadReqSpinInit_Cleanup( ctx );
    }
  }
}

/** @} */
