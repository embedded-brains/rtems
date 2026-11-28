/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CSemaphoreReqGetvalue
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

#include <rtems/test.h>

/**
 * @defgroup CSemaphoreReqGetvalue spec:/c/semaphore/req/getvalue
 *
 * @ingroup TestsuitesValidationNoClock1
 *
 * @{
 */

typedef enum {
  CSemaphoreReqGetvalue_Pre_Sem_Obj,
  CSemaphoreReqGetvalue_Pre_Sem_Null,
  CSemaphoreReqGetvalue_Pre_Sem_NA
} CSemaphoreReqGetvalue_Pre_Sem;

typedef enum {
  CSemaphoreReqGetvalue_Pre_Initialized_Yes,
  CSemaphoreReqGetvalue_Pre_Initialized_No,
  CSemaphoreReqGetvalue_Pre_Initialized_NA
} CSemaphoreReqGetvalue_Pre_Initialized;

typedef enum {
  CSemaphoreReqGetvalue_Pre_Value_Zero,
  CSemaphoreReqGetvalue_Pre_Value_One,
  CSemaphoreReqGetvalue_Pre_Value_Max,
  CSemaphoreReqGetvalue_Pre_Value_NA
} CSemaphoreReqGetvalue_Pre_Value;

typedef enum {
  CSemaphoreReqGetvalue_Post_Status_Ok,
  CSemaphoreReqGetvalue_Post_Status_MinusOne,
  CSemaphoreReqGetvalue_Post_Status_NA
} CSemaphoreReqGetvalue_Post_Status;

typedef enum {
  CSemaphoreReqGetvalue_Post_Errno_Inval,
  CSemaphoreReqGetvalue_Post_Errno_Nop,
  CSemaphoreReqGetvalue_Post_Errno_NA
} CSemaphoreReqGetvalue_Post_Errno;

typedef enum {
  CSemaphoreReqGetvalue_Post_Sval_Zero,
  CSemaphoreReqGetvalue_Post_Sval_One,
  CSemaphoreReqGetvalue_Post_Sval_Max,
  CSemaphoreReqGetvalue_Post_Sval_Nop,
  CSemaphoreReqGetvalue_Post_Sval_NA
} CSemaphoreReqGetvalue_Post_Sval;

typedef enum {
  CSemaphoreReqGetvalue_Post_Sem_Nop,
  CSemaphoreReqGetvalue_Post_Sem_NA
} CSemaphoreReqGetvalue_Post_Sem;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Sem_NA : 1;
  uint16_t Pre_Initialized_NA : 1;
  uint16_t Pre_Value_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Errno : 2;
  uint16_t Post_Sval : 3;
  uint16_t Post_Sem : 1;
} CSemaphoreReqGetvalue_Entry;

/**
 * @brief Test context for spec:/c/semaphore/req/getvalue test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `sem` parameter value.
   */
  sem_t *sem;

  /**
   * @brief This member contains the return value of the sem_getvalue() call.
   */
  int status;

  /**
   * @brief This member contains the return value of the errno after the
   *   sem_getvalue() call.
   */
  int errno_value;

  /**
   * @brief This member contains the semaphore object.
   */
  sem_t sem_obj;

  /**
   * @brief This member contains a copy of the semaphore object before the
   *   sem_getvalue() call.
   */
  sem_t sem_before;

  /**
   * @brief This member contains the object referenced by the `sval` parameter.
   */
  int sval_obj;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 3 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 3 ];

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
    CSemaphoreReqGetvalue_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CSemaphoreReqGetvalue_Context;

static CSemaphoreReqGetvalue_Context CSemaphoreReqGetvalue_Instance;

static const char *const CSemaphoreReqGetvalue_PreDesc_Sem[] =
  { "Obj", "Null", "NA" };

static const char *const CSemaphoreReqGetvalue_PreDesc_Initialized[] =
  { "Yes", "No", "NA" };

static const char *const CSemaphoreReqGetvalue_PreDesc_Value[] =
  { "Zero", "One", "Max", "NA" };

static const char *const *const CSemaphoreReqGetvalue_PreDesc[] = {
  CSemaphoreReqGetvalue_PreDesc_Sem,
  CSemaphoreReqGetvalue_PreDesc_Initialized,
  CSemaphoreReqGetvalue_PreDesc_Value,
  NULL
};

typedef CSemaphoreReqGetvalue_Context Context;

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

#define SVAL_POISON -1

static void CSemaphoreReqGetvalue_Pre_Sem_Prepare(
  CSemaphoreReqGetvalue_Context *ctx,
  CSemaphoreReqGetvalue_Pre_Sem  state
)
{
  switch ( state ) {
    case CSemaphoreReqGetvalue_Pre_Sem_Obj: {
      /*
       * While the `sem` parameter references an object of type sem_t.
       */
      T_eq_int( sem_init( &ctx->sem_obj, 0, 0 ), 0 );
      ctx->sem = &ctx->sem_obj;
      break;
    }

    case CSemaphoreReqGetvalue_Pre_Sem_Null: {
      /*
       * While the `sem` parameter is NULL.
       */
      ctx->sem = NULL;
      break;
    }

    case CSemaphoreReqGetvalue_Pre_Sem_NA:
      break;
  }
}

static void CSemaphoreReqGetvalue_Pre_Initialized_Prepare(
  CSemaphoreReqGetvalue_Context        *ctx,
  CSemaphoreReqGetvalue_Pre_Initialized state
)
{
  switch ( state ) {
    case CSemaphoreReqGetvalue_Pre_Initialized_Yes: {
      /*
       * While the object referenced by the `sem` parameter is an initialized
       * counting semaphore.
       */
      /* The object was initialized by the arrangement of pre-condition Sem */
      break;
    }

    case CSemaphoreReqGetvalue_Pre_Initialized_No: {
      /*
       * While the object referenced by the `sem` parameter is not an
       * initialized counting semaphore.
       */
      T_eq_int( sem_destroy( &ctx->sem_obj ), 0 );
      break;
    }

    case CSemaphoreReqGetvalue_Pre_Initialized_NA:
      break;
  }
}

static void CSemaphoreReqGetvalue_Pre_Value_Prepare(
  CSemaphoreReqGetvalue_Context  *ctx,
  CSemaphoreReqGetvalue_Pre_Value state
)
{
  switch ( state ) {
    case CSemaphoreReqGetvalue_Pre_Value_Zero: {
      /*
       * While the value of the counting semaphore is zero.
       */
      ctx->sem_obj._Semaphore._count = 0;
      break;
    }

    case CSemaphoreReqGetvalue_Pre_Value_One: {
      /*
       * While the value of the counting semaphore is one.
       */
      ctx->sem_obj._Semaphore._count = 1;
      break;
    }

    case CSemaphoreReqGetvalue_Pre_Value_Max: {
      /*
       * While the value of the counting semaphore is SEM_VALUE_MAX.
       */
      ctx->sem_obj._Semaphore._count = SEM_VALUE_MAX;
      break;
    }

    case CSemaphoreReqGetvalue_Pre_Value_NA:
      break;
  }
}

static void CSemaphoreReqGetvalue_Post_Status_Check(
  CSemaphoreReqGetvalue_Context    *ctx,
  CSemaphoreReqGetvalue_Post_Status state
)
{
  switch ( state ) {
    case CSemaphoreReqGetvalue_Post_Status_Ok: {
      /*
       * The return value of sem_getvalue() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CSemaphoreReqGetvalue_Post_Status_MinusOne: {
      /*
       * The return value of sem_getvalue() shall be minus one.
       */
      T_eq_int( ctx->status, -1 );
      break;
    }

    case CSemaphoreReqGetvalue_Post_Status_NA:
      break;
  }
}

static void CSemaphoreReqGetvalue_Post_Errno_Check(
  CSemaphoreReqGetvalue_Context   *ctx,
  CSemaphoreReqGetvalue_Post_Errno state
)
{
  switch ( state ) {
    case CSemaphoreReqGetvalue_Post_Errno_Inval: {
      /*
       * The value of errno shall be EINVAL.
       */
      T_eq_int( ctx->errno_value, EINVAL );
      break;
    }

    case CSemaphoreReqGetvalue_Post_Errno_Nop: {
      /*
       * The value of errno shall not be modified.
       */
      T_eq_int( ctx->errno_value, ERRNO_POISON );
      break;
    }

    case CSemaphoreReqGetvalue_Post_Errno_NA:
      break;
  }
}

static void CSemaphoreReqGetvalue_Post_Sval_Check(
  CSemaphoreReqGetvalue_Context  *ctx,
  CSemaphoreReqGetvalue_Post_Sval state
)
{
  switch ( state ) {
    case CSemaphoreReqGetvalue_Post_Sval_Zero: {
      /*
       * The object referenced by the `sval` parameter shall be zero.
       */
      T_eq_int( ctx->sval_obj, 0 );
      break;
    }

    case CSemaphoreReqGetvalue_Post_Sval_One: {
      /*
       * The object referenced by the `sval` parameter shall be one.
       */
      T_eq_int( ctx->sval_obj, 1 );
      break;
    }

    case CSemaphoreReqGetvalue_Post_Sval_Max: {
      /*
       * The object referenced by the `sval` parameter shall be SEM_VALUE_MAX.
       */
      T_eq_int( ctx->sval_obj, SEM_VALUE_MAX );
      break;
    }

    case CSemaphoreReqGetvalue_Post_Sval_Nop: {
      /*
       * The object referenced by the `sval` parameter shall not be modified.
       */
      T_eq_int( ctx->sval_obj, SVAL_POISON );
      break;
    }

    case CSemaphoreReqGetvalue_Post_Sval_NA:
      break;
  }
}

static void CSemaphoreReqGetvalue_Post_Sem_Check(
  CSemaphoreReqGetvalue_Context *ctx,
  CSemaphoreReqGetvalue_Post_Sem state
)
{
  switch ( state ) {
    case CSemaphoreReqGetvalue_Post_Sem_Nop: {
      /*
       * The object referenced by the `sem` parameter shall not be modified.
       */
      sem_t expected;

      memcpy( &expected, &ctx->sem_before, sizeof( expected ) );
      ExpectReleasedLock( ctx, &expected );
      T_eq_mem( &ctx->sem_obj, &expected, sizeof( ctx->sem_obj ) );
      break;
    }

    case CSemaphoreReqGetvalue_Post_Sem_NA:
      break;
  }
}

static void CSemaphoreReqGetvalue_Prepare( CSemaphoreReqGetvalue_Context *ctx )
{
  memset( &ctx->sem_obj, 0, sizeof( ctx->sem_obj ) );
  ctx->sval_obj = SVAL_POISON;
}

static void CSemaphoreReqGetvalue_Action( CSemaphoreReqGetvalue_Context *ctx )
{
  memcpy( &ctx->sem_before, &ctx->sem_obj, sizeof( ctx->sem_before ) );
  errno = ERRNO_POISON;
  ctx->status = sem_getvalue( ctx->sem, &ctx->sval_obj );
  ctx->errno_value = errno;
}

static void CSemaphoreReqGetvalue_Cleanup( CSemaphoreReqGetvalue_Context *ctx )
{
  if ( ctx->sem != NULL && ctx->sem_obj._flags != 0 ) {
    T_eq_int( sem_destroy( &ctx->sem_obj ), 0 );
  }
}

/* clang-format off */

static const CSemaphoreReqGetvalue_Entry
CSemaphoreReqGetvalue_Entries[] = {
  { 1, 0, 0, 0, CSemaphoreReqGetvalue_Post_Status_NA,
    CSemaphoreReqGetvalue_Post_Errno_NA, CSemaphoreReqGetvalue_Post_Sval_NA,
    CSemaphoreReqGetvalue_Post_Sem_NA },
  { 0, 0, 0, 0, CSemaphoreReqGetvalue_Post_Status_MinusOne,
    CSemaphoreReqGetvalue_Post_Errno_Inval,
    CSemaphoreReqGetvalue_Post_Sval_Nop, CSemaphoreReqGetvalue_Post_Sem_Nop },
  { 0, 0, 0, 0, CSemaphoreReqGetvalue_Post_Status_Ok,
    CSemaphoreReqGetvalue_Post_Errno_Nop, CSemaphoreReqGetvalue_Post_Sval_Zero,
    CSemaphoreReqGetvalue_Post_Sem_Nop },
  { 0, 0, 0, 0, CSemaphoreReqGetvalue_Post_Status_Ok,
    CSemaphoreReqGetvalue_Post_Errno_Nop, CSemaphoreReqGetvalue_Post_Sval_One,
    CSemaphoreReqGetvalue_Post_Sem_Nop },
  { 0, 0, 0, 0, CSemaphoreReqGetvalue_Post_Status_Ok,
    CSemaphoreReqGetvalue_Post_Errno_Nop, CSemaphoreReqGetvalue_Post_Sval_Max,
    CSemaphoreReqGetvalue_Post_Sem_Nop },
  { 0, 0, 1, 1, CSemaphoreReqGetvalue_Post_Status_MinusOne,
    CSemaphoreReqGetvalue_Post_Errno_Inval,
    CSemaphoreReqGetvalue_Post_Sval_Nop, CSemaphoreReqGetvalue_Post_Sem_NA }
};

static const uint8_t
CSemaphoreReqGetvalue_Map[] = {
  2, 3, 4, 1, 1, 1, 5, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t CSemaphoreReqGetvalue_Scope( void *arg, char *buf, size_t n )
{
  CSemaphoreReqGetvalue_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CSemaphoreReqGetvalue_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CSemaphoreReqGetvalue_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CSemaphoreReqGetvalue_Scope,
  .initial_context = &CSemaphoreReqGetvalue_Instance
};

static inline CSemaphoreReqGetvalue_Entry CSemaphoreReqGetvalue_PopEntry(
  CSemaphoreReqGetvalue_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CSemaphoreReqGetvalue_Entries[ CSemaphoreReqGetvalue_Map[ index ] ];
}

static void CSemaphoreReqGetvalue_SetPreConditionStates(
  CSemaphoreReqGetvalue_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Initialized_NA ) {
    ctx->Map.pcs[ 1 ] = CSemaphoreReqGetvalue_Pre_Initialized_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_Value_NA ) {
    ctx->Map.pcs[ 2 ] = CSemaphoreReqGetvalue_Pre_Value_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }
}

static void CSemaphoreReqGetvalue_TestVariant(
  CSemaphoreReqGetvalue_Context *ctx
)
{
  CSemaphoreReqGetvalue_Pre_Sem_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CSemaphoreReqGetvalue_Pre_Initialized_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CSemaphoreReqGetvalue_Pre_Value_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CSemaphoreReqGetvalue_Action( ctx );
  CSemaphoreReqGetvalue_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CSemaphoreReqGetvalue_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
  CSemaphoreReqGetvalue_Post_Sval_Check( ctx, ctx->Map.entry.Post_Sval );
  CSemaphoreReqGetvalue_Post_Sem_Check( ctx, ctx->Map.entry.Post_Sem );
}

/**
 * @fn void T_case_body_CSemaphoreReqGetvalue( void )
 */
T_TEST_CASE_FIXTURE( CSemaphoreReqGetvalue, &CSemaphoreReqGetvalue_Fixture )
{
  CSemaphoreReqGetvalue_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CSemaphoreReqGetvalue_Pre_Sem_Obj;
    ctx->Map.pci[ 0 ] < CSemaphoreReqGetvalue_Pre_Sem_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CSemaphoreReqGetvalue_Pre_Initialized_Yes;
      ctx->Map.pci[ 1 ] < CSemaphoreReqGetvalue_Pre_Initialized_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CSemaphoreReqGetvalue_Pre_Value_Zero;
        ctx->Map.pci[ 2 ] < CSemaphoreReqGetvalue_Pre_Value_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = CSemaphoreReqGetvalue_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        CSemaphoreReqGetvalue_SetPreConditionStates( ctx );
        CSemaphoreReqGetvalue_Prepare( ctx );
        CSemaphoreReqGetvalue_TestVariant( ctx );
        CSemaphoreReqGetvalue_Cleanup( ctx );
      }
    }
  }
}

/** @} */
