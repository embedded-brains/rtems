/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CSemaphoreReqDestroy
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
#include <rtems/posix/semaphoreimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CSemaphoreReqDestroy spec:/c/semaphore/req/destroy
 *
 * @ingroup TestsuitesValidationNoClock1
 *
 * @{
 */

typedef enum {
  CSemaphoreReqDestroy_Pre_Sem_Obj,
  CSemaphoreReqDestroy_Pre_Sem_Null,
  CSemaphoreReqDestroy_Pre_Sem_NA
} CSemaphoreReqDestroy_Pre_Sem;

typedef enum {
  CSemaphoreReqDestroy_Pre_Valid_Yes,
  CSemaphoreReqDestroy_Pre_Valid_No,
  CSemaphoreReqDestroy_Pre_Valid_NA
} CSemaphoreReqDestroy_Pre_Valid;

typedef enum {
  CSemaphoreReqDestroy_Pre_Named_Yes,
  CSemaphoreReqDestroy_Pre_Named_No,
  CSemaphoreReqDestroy_Pre_Named_NA
} CSemaphoreReqDestroy_Pre_Named;

typedef enum {
  CSemaphoreReqDestroy_Pre_Busy_Yes,
  CSemaphoreReqDestroy_Pre_Busy_No,
  CSemaphoreReqDestroy_Pre_Busy_NA
} CSemaphoreReqDestroy_Pre_Busy;

typedef enum {
  CSemaphoreReqDestroy_Post_Status_Ok,
  CSemaphoreReqDestroy_Post_Status_Error,
  CSemaphoreReqDestroy_Post_Status_NA
} CSemaphoreReqDestroy_Post_Status;

typedef enum {
  CSemaphoreReqDestroy_Post_Errno_Nop,
  CSemaphoreReqDestroy_Post_Errno_Inval,
  CSemaphoreReqDestroy_Post_Errno_Busy,
  CSemaphoreReqDestroy_Post_Errno_NA
} CSemaphoreReqDestroy_Post_Errno;

typedef enum {
  CSemaphoreReqDestroy_Post_Object_Destroyed,
  CSemaphoreReqDestroy_Post_Object_Nop,
  CSemaphoreReqDestroy_Post_Object_NA
} CSemaphoreReqDestroy_Post_Object;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Sem_NA : 1;
  uint16_t Pre_Valid_NA : 1;
  uint16_t Pre_Named_NA : 1;
  uint16_t Pre_Busy_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Errno : 2;
  uint16_t Post_Object : 2;
} CSemaphoreReqDestroy_Entry;

/**
 * @brief Test context for spec:/c/semaphore/req/destroy test case.
 */
typedef struct {
  /**
   * @brief This member provides the object referenced by the `sem` parameter.
   */
  sem_t sem_obj;

  /**
   * @brief This member specifies the `sem` parameter value.
   */
  sem_t *sem;

  /**
   * @brief This member is true, if the sem_obj member is a named semaphore.
   */
  bool named;

  /**
   * @brief This member contains the identifier of the thread which waits on
   *   the semaphore.
   */
  rtems_id worker_id;

  /**
   * @brief This member contains the return value of the sem_destroy() call.
   */
  int status;

  /**
   * @brief This member contains the value of errno after the sem_destroy()
   *   call.
   */
  int errno_value;

  /**
   * @brief This member contains a copy of the object referenced by the `sem`
   *   parameter before the call.
   */
  sem_t sem_before;

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
    CSemaphoreReqDestroy_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CSemaphoreReqDestroy_Context;

static CSemaphoreReqDestroy_Context CSemaphoreReqDestroy_Instance;

static const char *const CSemaphoreReqDestroy_PreDesc_Sem[] =
  { "Obj", "Null", "NA" };

static const char *const CSemaphoreReqDestroy_PreDesc_Valid[] =
  { "Yes", "No", "NA" };

static const char *const CSemaphoreReqDestroy_PreDesc_Named[] =
  { "Yes", "No", "NA" };

static const char *const CSemaphoreReqDestroy_PreDesc_Busy[] =
  { "Yes", "No", "NA" };

static const char *const *const CSemaphoreReqDestroy_PreDesc[] = {
  CSemaphoreReqDestroy_PreDesc_Sem,
  CSemaphoreReqDestroy_PreDesc_Valid,
  CSemaphoreReqDestroy_PreDesc_Named,
  CSemaphoreReqDestroy_PreDesc_Busy,
  NULL
};

static void WaitWorker( rtems_task_argument arg )
{
  CSemaphoreReqDestroy_Context *ctx;

  ctx = (CSemaphoreReqDestroy_Context *) arg;
  T_eq_int( sem_wait( &ctx->sem_obj ), 0 );
  SuspendSelf();
}

static void Action( CSemaphoreReqDestroy_Context *ctx )
{
  errno = -1;

  if ( ctx->sem != NULL ) {
    memcpy( &ctx->sem_before, ctx->sem, sizeof( ctx->sem_before ) );
  }

  ctx->status = sem_destroy( ctx->sem );
  ctx->errno_value = errno;
}

static void CSemaphoreReqDestroy_Pre_Sem_Prepare(
  CSemaphoreReqDestroy_Context *ctx,
  CSemaphoreReqDestroy_Pre_Sem  state
)
{
  switch ( state ) {
    case CSemaphoreReqDestroy_Pre_Sem_Obj: {
      /*
       * While the `sem` parameter references an object of type sem_t.
       */
      ctx->sem = &ctx->sem_obj;
      break;
    }

    case CSemaphoreReqDestroy_Pre_Sem_Null: {
      /*
       * While the `sem` parameter is equal to NULL.
       */
      ctx->sem = NULL;
      break;
    }

    case CSemaphoreReqDestroy_Pre_Sem_NA:
      break;
  }
}

static void CSemaphoreReqDestroy_Pre_Valid_Prepare(
  CSemaphoreReqDestroy_Context  *ctx,
  CSemaphoreReqDestroy_Pre_Valid state
)
{
  switch ( state ) {
    case CSemaphoreReqDestroy_Pre_Valid_Yes: {
      /*
       * While the object referenced by the `sem` parameter is an initialized
       * counting semaphore.
       */
      T_eq_int( sem_init( &ctx->sem_obj, 0, 1 ), 0 );
      break;
    }

    case CSemaphoreReqDestroy_Pre_Valid_No: {
      /*
       * While the object referenced by the `sem` parameter is not an
       * initialized counting semaphore.
       */
      memset( &ctx->sem_obj, 0, sizeof( ctx->sem_obj ) );
      break;
    }

    case CSemaphoreReqDestroy_Pre_Valid_NA:
      break;
  }
}

static void CSemaphoreReqDestroy_Pre_Named_Prepare(
  CSemaphoreReqDestroy_Context  *ctx,
  CSemaphoreReqDestroy_Pre_Named state
)
{
  switch ( state ) {
    case CSemaphoreReqDestroy_Pre_Named_Yes: {
      /*
       * While the counting semaphore referenced by the `sem` parameter is a
       * named semaphore.
       */
      _POSIX_Semaphore_Initialize( &ctx->sem_obj, "named", 1 );
      ctx->named = true;
      break;
    }

    case CSemaphoreReqDestroy_Pre_Named_No: {
      /*
       * While the counting semaphore referenced by the `sem` parameter is an
       * unnamed semaphore.
       */
      ctx->named = false;
      break;
    }

    case CSemaphoreReqDestroy_Pre_Named_NA:
      break;
  }
}

static void CSemaphoreReqDestroy_Pre_Busy_Prepare(
  CSemaphoreReqDestroy_Context *ctx,
  CSemaphoreReqDestroy_Pre_Busy state
)
{
  switch ( state ) {
    case CSemaphoreReqDestroy_Pre_Busy_Yes: {
      /*
       * While a thread waits on the counting semaphore referenced by the `sem`
       * parameter.
       */
      T_eq_int( sem_init( &ctx->sem_obj, 0, 0 ), 0 );
      ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
      StartTask( ctx->worker_id, WaitWorker, ctx );
      break;
    }

    case CSemaphoreReqDestroy_Pre_Busy_No: {
      /*
       * While no thread waits on the counting semaphore referenced by the
       * `sem` parameter.
       */
      ctx->worker_id = 0;
      break;
    }

    case CSemaphoreReqDestroy_Pre_Busy_NA:
      break;
  }
}

static void CSemaphoreReqDestroy_Post_Status_Check(
  CSemaphoreReqDestroy_Context    *ctx,
  CSemaphoreReqDestroy_Post_Status state
)
{
  switch ( state ) {
    case CSemaphoreReqDestroy_Post_Status_Ok: {
      /*
       * The return value of sem_destroy() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CSemaphoreReqDestroy_Post_Status_Error: {
      /*
       * The return value of sem_destroy() shall be -1.
       */
      T_eq_int( ctx->status, -1 );
      break;
    }

    case CSemaphoreReqDestroy_Post_Status_NA:
      break;
  }
}

static void CSemaphoreReqDestroy_Post_Errno_Check(
  CSemaphoreReqDestroy_Context   *ctx,
  CSemaphoreReqDestroy_Post_Errno state
)
{
  switch ( state ) {
    case CSemaphoreReqDestroy_Post_Errno_Nop: {
      /*
       * The errno shall not be changed by the directive call.
       */
      T_eq_int( ctx->errno_value, -1 );
      break;
    }

    case CSemaphoreReqDestroy_Post_Errno_Inval: {
      /*
       * The errno shall be equal to EINVAL.
       */
      T_eq_int( ctx->errno_value, EINVAL );
      break;
    }

    case CSemaphoreReqDestroy_Post_Errno_Busy: {
      /*
       * The errno shall be equal to EBUSY.
       */
      T_eq_int( ctx->errno_value, EBUSY );
      break;
    }

    case CSemaphoreReqDestroy_Post_Errno_NA:
      break;
  }
}

static void CSemaphoreReqDestroy_Post_Object_Check(
  CSemaphoreReqDestroy_Context    *ctx,
  CSemaphoreReqDestroy_Post_Object state
)
{
  switch ( state ) {
    case CSemaphoreReqDestroy_Post_Object_Destroyed: {
      /*
       * The counting semaphore referenced by the `sem` parameter shall no
       * longer be an initialized counting semaphore.
       */
      T_eq_ulong( ctx->sem->_flags, 0 );
      break;
    }

    case CSemaphoreReqDestroy_Post_Object_Nop: {
      /*
       * The object referenced by the `sem` parameter shall not be modified.
       */
      T_eq_mem( ctx->sem, &ctx->sem_before, sizeof( ctx->sem_before ) );
      break;
    }

    case CSemaphoreReqDestroy_Post_Object_NA:
      break;
  }
}

static void CSemaphoreReqDestroy_Setup( void )
{
  SetSelfPriority( PRIO_NORMAL );
}

static void CSemaphoreReqDestroy_Setup_Wrap( void *arg )
{
  CSemaphoreReqDestroy_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CSemaphoreReqDestroy_Setup();
}

static void CSemaphoreReqDestroy_Teardown( void )
{
  RestoreRunnerPriority();
}

static void CSemaphoreReqDestroy_Teardown_Wrap( void *arg )
{
  CSemaphoreReqDestroy_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CSemaphoreReqDestroy_Teardown();
}

static void CSemaphoreReqDestroy_Prepare( CSemaphoreReqDestroy_Context *ctx )
{
  ctx->worker_id = 0;
  ctx->named = false;
}

static void CSemaphoreReqDestroy_Action( CSemaphoreReqDestroy_Context *ctx )
{
  Action( ctx );
}

static void CSemaphoreReqDestroy_Cleanup( CSemaphoreReqDestroy_Context *ctx )
{
  if ( ctx->worker_id != 0 ) {
    T_eq_int( sem_post( &ctx->sem_obj ), 0 );
    DeleteTask( ctx->worker_id );
    ctx->worker_id = 0;
  }

  if ( ctx->named ) {
    _POSIX_Semaphore_Destroy( &ctx->sem_obj );
  } else if ( ctx->status != 0 ) {
    (void) sem_destroy( &ctx->sem_obj );
  }
}

/* clang-format off */

static const CSemaphoreReqDestroy_Entry
CSemaphoreReqDestroy_Entries[] = {
  { 1, 0, 0, 0, 0, CSemaphoreReqDestroy_Post_Status_NA,
    CSemaphoreReqDestroy_Post_Errno_NA, CSemaphoreReqDestroy_Post_Object_NA },
  { 0, 0, 0, 0, 1, CSemaphoreReqDestroy_Post_Status_Error,
    CSemaphoreReqDestroy_Post_Errno_Inval, CSemaphoreReqDestroy_Post_Object_Nop },
  { 0, 0, 0, 0, 0, CSemaphoreReqDestroy_Post_Status_Error,
    CSemaphoreReqDestroy_Post_Errno_Busy, CSemaphoreReqDestroy_Post_Object_Nop },
  { 0, 0, 0, 0, 0, CSemaphoreReqDestroy_Post_Status_Ok,
    CSemaphoreReqDestroy_Post_Errno_Nop,
    CSemaphoreReqDestroy_Post_Object_Destroyed },
  { 0, 0, 0, 1, 1, CSemaphoreReqDestroy_Post_Status_Error,
    CSemaphoreReqDestroy_Post_Errno_Inval, CSemaphoreReqDestroy_Post_Object_Nop },
  { 0, 0, 1, 1, 1, CSemaphoreReqDestroy_Post_Status_Error,
    CSemaphoreReqDestroy_Post_Errno_Inval, CSemaphoreReqDestroy_Post_Object_NA }
};

static const uint8_t
CSemaphoreReqDestroy_Map[] = {
  1, 0, 2, 3, 4, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t CSemaphoreReqDestroy_Scope( void *arg, char *buf, size_t n )
{
  CSemaphoreReqDestroy_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CSemaphoreReqDestroy_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CSemaphoreReqDestroy_Fixture = {
  .setup = CSemaphoreReqDestroy_Setup_Wrap,
  .stop = NULL,
  .teardown = CSemaphoreReqDestroy_Teardown_Wrap,
  .scope = CSemaphoreReqDestroy_Scope,
  .initial_context = &CSemaphoreReqDestroy_Instance
};

static inline CSemaphoreReqDestroy_Entry CSemaphoreReqDestroy_PopEntry(
  CSemaphoreReqDestroy_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CSemaphoreReqDestroy_Entries[ CSemaphoreReqDestroy_Map[ index ] ];
}

static void CSemaphoreReqDestroy_SetPreConditionStates(
  CSemaphoreReqDestroy_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Valid_NA ) {
    ctx->Map.pcs[ 1 ] = CSemaphoreReqDestroy_Pre_Valid_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_Named_NA ) {
    ctx->Map.pcs[ 2 ] = CSemaphoreReqDestroy_Pre_Named_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_Busy_NA ) {
    ctx->Map.pcs[ 3 ] = CSemaphoreReqDestroy_Pre_Busy_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }
}

static void CSemaphoreReqDestroy_TestVariant(
  CSemaphoreReqDestroy_Context *ctx
)
{
  CSemaphoreReqDestroy_Pre_Sem_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CSemaphoreReqDestroy_Pre_Valid_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CSemaphoreReqDestroy_Pre_Named_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CSemaphoreReqDestroy_Pre_Busy_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CSemaphoreReqDestroy_Action( ctx );
  CSemaphoreReqDestroy_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CSemaphoreReqDestroy_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
  CSemaphoreReqDestroy_Post_Object_Check( ctx, ctx->Map.entry.Post_Object );
}

/**
 * @fn void T_case_body_CSemaphoreReqDestroy( void )
 */
T_TEST_CASE_FIXTURE( CSemaphoreReqDestroy, &CSemaphoreReqDestroy_Fixture )
{
  CSemaphoreReqDestroy_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CSemaphoreReqDestroy_Pre_Sem_Obj;
    ctx->Map.pci[ 0 ] < CSemaphoreReqDestroy_Pre_Sem_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CSemaphoreReqDestroy_Pre_Valid_Yes;
      ctx->Map.pci[ 1 ] < CSemaphoreReqDestroy_Pre_Valid_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CSemaphoreReqDestroy_Pre_Named_Yes;
        ctx->Map.pci[ 2 ] < CSemaphoreReqDestroy_Pre_Named_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CSemaphoreReqDestroy_Pre_Busy_Yes;
          ctx->Map.pci[ 3 ] < CSemaphoreReqDestroy_Pre_Busy_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = CSemaphoreReqDestroy_PopEntry( ctx );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          CSemaphoreReqDestroy_SetPreConditionStates( ctx );
          CSemaphoreReqDestroy_Prepare( ctx );
          CSemaphoreReqDestroy_TestVariant( ctx );
          CSemaphoreReqDestroy_Cleanup( ctx );
        }
      }
    }
  }
}

/** @} */
