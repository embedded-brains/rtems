/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CSemaphoreReqTrywait
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
#include <limits.h>
#include <semaphore.h>
#include <string.h>

#include "tr-sem-seize-try.h"
#include "tx-support.h"
#include "tx-thread-queue.h"

#include <rtems/test.h>

/**
 * @defgroup CSemaphoreReqTrywait spec:/c/semaphore/req/trywait
 *
 * @ingroup TestsuitesValidationNoClock1
 *
 * @{
 */

typedef enum {
  CSemaphoreReqTrywait_Pre_Sem_Null,
  CSemaphoreReqTrywait_Pre_Sem_Invalid,
  CSemaphoreReqTrywait_Pre_Sem_Init,
  CSemaphoreReqTrywait_Pre_Sem_NA
} CSemaphoreReqTrywait_Pre_Sem;

typedef enum {
  CSemaphoreReqTrywait_Post_Status_Error,
  CSemaphoreReqTrywait_Post_Status_NA
} CSemaphoreReqTrywait_Post_Status;

typedef enum {
  CSemaphoreReqTrywait_Post_Errno_Inval,
  CSemaphoreReqTrywait_Post_Errno_NA
} CSemaphoreReqTrywait_Post_Errno;

typedef enum {
  CSemaphoreReqTrywait_Post_Object_Nop,
  CSemaphoreReqTrywait_Post_Object_NA
} CSemaphoreReqTrywait_Post_Object;

typedef enum {
  CSemaphoreReqTrywait_Post_Action_SemSeizeTry,
  CSemaphoreReqTrywait_Post_Action_NA
} CSemaphoreReqTrywait_Post_Action;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Sem_NA : 1;
  uint8_t Post_Status : 1;
  uint8_t Post_Errno : 1;
  uint8_t Post_Object : 1;
  uint8_t Post_Action : 1;
} CSemaphoreReqTrywait_Entry;

/**
 * @brief Test context for spec:/c/semaphore/req/trywait test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `sem` parameter value.
   */
  sem_t *sem;

  /**
   * @brief This member contains the return value of the sem_trywait() call.
   */
  int status;

  /**
   * @brief This member contains the value of errno after the sem_trywait()
   *   call.
   */
  int errno_value;

  /**
   * @brief This member provides the storage for a semaphore.
   */
  sem_t sem_storage;

  /**
   * @brief This member contains a copy of the object referenced by the `sem`
   *   parameter before the call.
   */
  sem_t sem_before;

  /**
   * @brief This member is true, if the sem_storage member contains an
   *   initialized semaphore.
   */
  bool sem_initialized;

  /**
   * @brief This member contains the thread queue test context.
   */
  TQSemContext tq_sem_ctx;

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
    CSemaphoreReqTrywait_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CSemaphoreReqTrywait_Context;

static CSemaphoreReqTrywait_Context CSemaphoreReqTrywait_Instance;

static const char *const CSemaphoreReqTrywait_PreDesc_Sem[] =
  { "Null", "Invalid", "Init", "NA" };

static const char *const *const CSemaphoreReqTrywait_PreDesc[] = {
  CSemaphoreReqTrywait_PreDesc_Sem,
  NULL
};

static void CSemaphoreReqTrywait_Pre_Sem_Prepare(
  CSemaphoreReqTrywait_Context *ctx,
  CSemaphoreReqTrywait_Pre_Sem  state
)
{
  switch ( state ) {
    case CSemaphoreReqTrywait_Pre_Sem_Null: {
      /*
       * While the `sem` parameter is NULL.
       */
      ctx->sem = NULL;
      ctx->sem_initialized = false;
      break;
    }

    case CSemaphoreReqTrywait_Pre_Sem_Invalid: {
      /*
       * While the counting semaphore referenced by the `sem` parameter is
       * invalid.
       */
      T_eq_int( sem_init( &ctx->sem_storage, 0, 1 ), 0 );
      T_eq_int( sem_destroy( &ctx->sem_storage ), 0 );
      ctx->sem = &ctx->sem_storage;
      ctx->sem_initialized = false;
      break;
    }

    case CSemaphoreReqTrywait_Pre_Sem_Init: {
      /*
       * While the object referenced by the `sem` parameter is an initialized
       * counting semaphore.
       */
      T_eq_int( sem_init( &ctx->sem_storage, 0, 1 ), 0 );
      ctx->sem = &ctx->sem_storage;
      ctx->sem_initialized = true;

      ctx->tq_sem_ctx.base.discipline = TQ_PRIORITY;
      break;
    }

    case CSemaphoreReqTrywait_Pre_Sem_NA:
      break;
  }
}

static void CSemaphoreReqTrywait_Post_Status_Check(
  CSemaphoreReqTrywait_Context    *ctx,
  CSemaphoreReqTrywait_Post_Status state
)
{
  switch ( state ) {
    case CSemaphoreReqTrywait_Post_Status_Error: {
      /*
       * The return value of sem_trywait() shall be -1.
       */
      T_eq_int( ctx->status, -1 );
      break;
    }

    case CSemaphoreReqTrywait_Post_Status_NA:
      break;
  }
}

static void CSemaphoreReqTrywait_Post_Errno_Check(
  CSemaphoreReqTrywait_Context   *ctx,
  CSemaphoreReqTrywait_Post_Errno state
)
{
  switch ( state ) {
    case CSemaphoreReqTrywait_Post_Errno_Inval: {
      /*
       * The errno shall be equal to EINVAL.
       */
      T_eq_int( ctx->errno_value, EINVAL );
      break;
    }

    case CSemaphoreReqTrywait_Post_Errno_NA:
      break;
  }
}

static void CSemaphoreReqTrywait_Post_Object_Check(
  CSemaphoreReqTrywait_Context    *ctx,
  CSemaphoreReqTrywait_Post_Object state
)
{
  switch ( state ) {
    case CSemaphoreReqTrywait_Post_Object_Nop: {
      /*
       * The object referenced by the `sem` parameter shall not be modified.
       */
      T_eq_mem(
        &ctx->sem_storage,
        &ctx->sem_before,
        sizeof( ctx->sem_before )
      );
      break;
    }

    case CSemaphoreReqTrywait_Post_Object_NA:
      break;
  }
}

static void CSemaphoreReqTrywait_Post_Action_Check(
  CSemaphoreReqTrywait_Context    *ctx,
  CSemaphoreReqTrywait_Post_Action state
)
{
  switch ( state ) {
    case CSemaphoreReqTrywait_Post_Action_SemSeizeTry: {
      /*
       * The calling thread shall attempt to seize the counting semaphore
       * referenced by the `sem` parameter as specified by
       * spec:/score/sem/req/seize-try.
       */
      T_eq_int( sem_post( ctx->sem ), 0 );
      ctx->tq_sem_ctx.base.thread_queue_object = ctx->sem;
      ctx->tq_sem_ctx.base.wait = TQ_NO_WAIT;
      ScoreSemReqSeizeTry_Run( &ctx->tq_sem_ctx );
      break;
    }

    case CSemaphoreReqTrywait_Post_Action_NA:
      break;
  }
}

static void CSemaphoreReqTrywait_Setup( CSemaphoreReqTrywait_Context *ctx )
{
  memset( ctx, 0, sizeof( *ctx ) );
  ctx->tq_sem_ctx.base.enqueue_prepare = TQEnqueuePrepareDefault;
  ctx->tq_sem_ctx.base.enqueue_done = TQEnqueueDoneDefault;
  ctx->tq_sem_ctx.base.enqueue = TQEnqueuePOSIXSem;
  ctx->tq_sem_ctx.base.surrender = TQSurrenderPOSIXSem;
  ctx->tq_sem_ctx.base.convert_status = TQConvertStatusPOSIX;
  ctx->tq_sem_ctx.variant = TQ_SEM_COUNTING;
  ctx->tq_sem_ctx.get_count = TQSemGetCountPOSIX;
  ctx->tq_sem_ctx.set_count = TQSemSetCountPOSIX;
  TQInitialize( &ctx->tq_sem_ctx.base );
}

static void CSemaphoreReqTrywait_Setup_Wrap( void *arg )
{
  CSemaphoreReqTrywait_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CSemaphoreReqTrywait_Setup( ctx );
}

static void CSemaphoreReqTrywait_Teardown( CSemaphoreReqTrywait_Context *ctx )
{
  TQDestroy( &ctx->tq_sem_ctx.base );
}

static void CSemaphoreReqTrywait_Teardown_Wrap( void *arg )
{
  CSemaphoreReqTrywait_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CSemaphoreReqTrywait_Teardown( ctx );
}

static void CSemaphoreReqTrywait_Prepare( CSemaphoreReqTrywait_Context *ctx )
{
  ctx->sem_initialized = false;
}

static void CSemaphoreReqTrywait_Action( CSemaphoreReqTrywait_Context *ctx )
{
  memcpy( &ctx->sem_before, &ctx->sem_storage, sizeof( ctx->sem_before ) );
  errno = -1;
  ctx->status = sem_trywait( ctx->sem );
  ctx->errno_value = errno;
}

static void CSemaphoreReqTrywait_Cleanup( CSemaphoreReqTrywait_Context *ctx )
{
  if ( ctx->sem_initialized ) {
    T_eq_int( sem_destroy( &ctx->sem_storage ), 0 );
  }
}

/* clang-format off */

static const CSemaphoreReqTrywait_Entry
CSemaphoreReqTrywait_Entries[] = {
  { 0, 0, CSemaphoreReqTrywait_Post_Status_Error,
    CSemaphoreReqTrywait_Post_Errno_Inval, CSemaphoreReqTrywait_Post_Object_NA,
    CSemaphoreReqTrywait_Post_Action_NA },
  { 0, 0, CSemaphoreReqTrywait_Post_Status_Error,
    CSemaphoreReqTrywait_Post_Errno_Inval,
    CSemaphoreReqTrywait_Post_Object_Nop, CSemaphoreReqTrywait_Post_Action_NA },
  { 0, 0, CSemaphoreReqTrywait_Post_Status_NA,
    CSemaphoreReqTrywait_Post_Errno_NA, CSemaphoreReqTrywait_Post_Object_NA,
    CSemaphoreReqTrywait_Post_Action_SemSeizeTry }
};

static const uint8_t
CSemaphoreReqTrywait_Map[] = {
  0, 1, 2
};

/* clang-format on */

static size_t CSemaphoreReqTrywait_Scope( void *arg, char *buf, size_t n )
{
  CSemaphoreReqTrywait_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CSemaphoreReqTrywait_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CSemaphoreReqTrywait_Fixture = {
  .setup = CSemaphoreReqTrywait_Setup_Wrap,
  .stop = NULL,
  .teardown = CSemaphoreReqTrywait_Teardown_Wrap,
  .scope = CSemaphoreReqTrywait_Scope,
  .initial_context = &CSemaphoreReqTrywait_Instance
};

static inline CSemaphoreReqTrywait_Entry CSemaphoreReqTrywait_PopEntry(
  CSemaphoreReqTrywait_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CSemaphoreReqTrywait_Entries[ CSemaphoreReqTrywait_Map[ index ] ];
}

static void CSemaphoreReqTrywait_TestVariant(
  CSemaphoreReqTrywait_Context *ctx
)
{
  CSemaphoreReqTrywait_Pre_Sem_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CSemaphoreReqTrywait_Action( ctx );
  CSemaphoreReqTrywait_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CSemaphoreReqTrywait_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
  CSemaphoreReqTrywait_Post_Object_Check( ctx, ctx->Map.entry.Post_Object );
  CSemaphoreReqTrywait_Post_Action_Check( ctx, ctx->Map.entry.Post_Action );
}

/**
 * @fn void T_case_body_CSemaphoreReqTrywait( void )
 */
T_TEST_CASE_FIXTURE( CSemaphoreReqTrywait, &CSemaphoreReqTrywait_Fixture )
{
  CSemaphoreReqTrywait_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CSemaphoreReqTrywait_Pre_Sem_Null;
    ctx->Map.pcs[ 0 ] < CSemaphoreReqTrywait_Pre_Sem_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CSemaphoreReqTrywait_PopEntry( ctx );
    CSemaphoreReqTrywait_Prepare( ctx );
    CSemaphoreReqTrywait_TestVariant( ctx );
    CSemaphoreReqTrywait_Cleanup( ctx );
  }
}

/** @} */
