/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CSemaphoreReqInit
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
#include <rtems/posix/semaphoreimpl.h>

#include <rtems/test.h>

/**
 * @defgroup CSemaphoreReqInit spec:/c/semaphore/req/init
 *
 * @ingroup TestsuitesValidationNoClock1
 *
 * @{
 */

typedef enum {
  CSemaphoreReqInit_Pre_Sem_Valid,
  CSemaphoreReqInit_Pre_Sem_Null,
  CSemaphoreReqInit_Pre_Sem_NA
} CSemaphoreReqInit_Pre_Sem;

typedef enum {
  CSemaphoreReqInit_Pre_Pshared_Zero,
  CSemaphoreReqInit_Pre_Pshared_NonZero,
  CSemaphoreReqInit_Pre_Pshared_NA
} CSemaphoreReqInit_Pre_Pshared;

typedef enum {
  CSemaphoreReqInit_Pre_Value_Valid,
  CSemaphoreReqInit_Pre_Value_Max,
  CSemaphoreReqInit_Pre_Value_TooBig,
  CSemaphoreReqInit_Pre_Value_NA
} CSemaphoreReqInit_Pre_Value;

typedef enum {
  CSemaphoreReqInit_Post_Status_Ok,
  CSemaphoreReqInit_Post_Status_Error,
  CSemaphoreReqInit_Post_Status_NA
} CSemaphoreReqInit_Post_Status;

typedef enum {
  CSemaphoreReqInit_Post_Errno_Nop,
  CSemaphoreReqInit_Post_Errno_Inval,
  CSemaphoreReqInit_Post_Errno_NA
} CSemaphoreReqInit_Post_Errno;

typedef enum {
  CSemaphoreReqInit_Post_Object_Initialized,
  CSemaphoreReqInit_Post_Object_Nop,
  CSemaphoreReqInit_Post_Object_NA
} CSemaphoreReqInit_Post_Object;

typedef enum {
  CSemaphoreReqInit_Post_Count_Valid,
  CSemaphoreReqInit_Post_Count_Max,
  CSemaphoreReqInit_Post_Count_NA
} CSemaphoreReqInit_Post_Count;

typedef enum {
  CSemaphoreReqInit_Post_Queue_Empty,
  CSemaphoreReqInit_Post_Queue_NA
} CSemaphoreReqInit_Post_Queue;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Sem_NA : 1;
  uint16_t Pre_Pshared_NA : 1;
  uint16_t Pre_Value_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Errno : 2;
  uint16_t Post_Object : 2;
  uint16_t Post_Count : 2;
  uint16_t Post_Queue : 1;
} CSemaphoreReqInit_Entry;

/**
 * @brief Test context for spec:/c/semaphore/req/init test case.
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
   * @brief This member specifies the `pshared` parameter value.
   */
  int pshared;

  /**
   * @brief This member specifies the `value` parameter value.
   */
  unsigned int value;

  /**
   * @brief This member contains the return value of the sem_init() call.
   */
  int status;

  /**
   * @brief This member contains the value of errno after the sem_init() call.
   */
  int errno_value;

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
    CSemaphoreReqInit_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CSemaphoreReqInit_Context;

static CSemaphoreReqInit_Context CSemaphoreReqInit_Instance;

static const char *const CSemaphoreReqInit_PreDesc_Sem[] =
  { "Valid", "Null", "NA" };

static const char *const CSemaphoreReqInit_PreDesc_Pshared[] =
  { "Zero", "NonZero", "NA" };

static const char *const CSemaphoreReqInit_PreDesc_Value[] =
  { "Valid", "Max", "TooBig", "NA" };

static const char *const *const CSemaphoreReqInit_PreDesc[] = {
  CSemaphoreReqInit_PreDesc_Sem,
  CSemaphoreReqInit_PreDesc_Pshared,
  CSemaphoreReqInit_PreDesc_Value,
  NULL
};

static Sem_Control *SemOf( CSemaphoreReqInit_Context *ctx )
{
  return _Sem_Get( &ctx->sem_obj._Semaphore );
}

static void CSemaphoreReqInit_Pre_Sem_Prepare(
  CSemaphoreReqInit_Context *ctx,
  CSemaphoreReqInit_Pre_Sem  state
)
{
  switch ( state ) {
    case CSemaphoreReqInit_Pre_Sem_Valid: {
      /*
       * While the `sem` parameter references an object of type sem_t.
       */
      ctx->sem = &ctx->sem_obj;
      memset( &ctx->sem_obj, 0xff, sizeof( ctx->sem_obj ) );
      break;
    }

    case CSemaphoreReqInit_Pre_Sem_Null: {
      /*
       * While the `sem` parameter is equal to NULL.
       */
      ctx->sem = NULL;
      break;
    }

    case CSemaphoreReqInit_Pre_Sem_NA:
      break;
  }
}

static void CSemaphoreReqInit_Pre_Pshared_Prepare(
  CSemaphoreReqInit_Context    *ctx,
  CSemaphoreReqInit_Pre_Pshared state
)
{
  switch ( state ) {
    case CSemaphoreReqInit_Pre_Pshared_Zero: {
      /*
       * While the `pshared` parameter is equal to zero.
       */
      ctx->pshared = 0;
      break;
    }

    case CSemaphoreReqInit_Pre_Pshared_NonZero: {
      /*
       * While the `pshared` parameter is not equal to zero.
       */
      ctx->pshared = 1;
      break;
    }

    case CSemaphoreReqInit_Pre_Pshared_NA:
      break;
  }
}

static void CSemaphoreReqInit_Pre_Value_Prepare(
  CSemaphoreReqInit_Context  *ctx,
  CSemaphoreReqInit_Pre_Value state
)
{
  switch ( state ) {
    case CSemaphoreReqInit_Pre_Value_Valid: {
      /*
       * While the `value` parameter is less than SEM_VALUE_MAX.
       */
      ctx->value = 1;
      break;
    }

    case CSemaphoreReqInit_Pre_Value_Max: {
      /*
       * While the `value` parameter is equal to SEM_VALUE_MAX.
       */
      ctx->value = SEM_VALUE_MAX;
      break;
    }

    case CSemaphoreReqInit_Pre_Value_TooBig: {
      /*
       * While the `value` parameter is greater than SEM_VALUE_MAX.
       */
      ctx->value = (unsigned int) SEM_VALUE_MAX + 1u;
      break;
    }

    case CSemaphoreReqInit_Pre_Value_NA:
      break;
  }
}

static void CSemaphoreReqInit_Post_Status_Check(
  CSemaphoreReqInit_Context    *ctx,
  CSemaphoreReqInit_Post_Status state
)
{
  switch ( state ) {
    case CSemaphoreReqInit_Post_Status_Ok: {
      /*
       * The return value of sem_init() shall be zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CSemaphoreReqInit_Post_Status_Error: {
      /*
       * The return value of sem_init() shall be -1.
       */
      T_eq_int( ctx->status, -1 );
      break;
    }

    case CSemaphoreReqInit_Post_Status_NA:
      break;
  }
}

static void CSemaphoreReqInit_Post_Errno_Check(
  CSemaphoreReqInit_Context   *ctx,
  CSemaphoreReqInit_Post_Errno state
)
{
  switch ( state ) {
    case CSemaphoreReqInit_Post_Errno_Nop: {
      /*
       * The errno shall not be changed by the directive call.
       */
      T_eq_int( ctx->errno_value, -1 );
      break;
    }

    case CSemaphoreReqInit_Post_Errno_Inval: {
      /*
       * The errno shall be equal to EINVAL.
       */
      T_eq_int( ctx->errno_value, EINVAL );
      break;
    }

    case CSemaphoreReqInit_Post_Errno_NA:
      break;
  }
}

static void CSemaphoreReqInit_Post_Object_Check(
  CSemaphoreReqInit_Context    *ctx,
  CSemaphoreReqInit_Post_Object state
)
{
  switch ( state ) {
    case CSemaphoreReqInit_Post_Object_Initialized: {
      /*
       * The object referenced by the `sem` parameter shall be an initialized
       * counting semaphore.
       */
      T_eq_ulong(
        ctx->sem_obj._flags,
        (uintptr_t) &ctx->sem_obj ^ POSIX_SEMAPHORE_MAGIC
      );
      break;
    }

    case CSemaphoreReqInit_Post_Object_Nop: {
      /*
       * The object referenced by the `sem` parameter shall not be modified.
       */
      sem_t expected;

      memset( &expected, 0xff, sizeof( expected ) );
      T_eq_mem( &ctx->sem_obj, &expected, sizeof( expected ) );
      break;
    }

    case CSemaphoreReqInit_Post_Object_NA:
      break;
  }
}

static void CSemaphoreReqInit_Post_Count_Check(
  CSemaphoreReqInit_Context   *ctx,
  CSemaphoreReqInit_Post_Count state
)
{
  switch ( state ) {
    case CSemaphoreReqInit_Post_Count_Valid: {
      /*
       * The count of the counting semaphore referenced by the `sem` parameter
       * shall be equal to the value of the `value` parameter.
       */
      T_eq_u32( SemOf( ctx )->count, ctx->value );
      break;
    }

    case CSemaphoreReqInit_Post_Count_Max: {
      /*
       * The count of the counting semaphore referenced by the `sem` parameter
       * shall be SEM_VALUE_MAX.
       */
      T_eq_u32( SemOf( ctx )->count, SEM_VALUE_MAX );
      break;
    }

    case CSemaphoreReqInit_Post_Count_NA:
      break;
  }
}

static void CSemaphoreReqInit_Post_Queue_Check(
  CSemaphoreReqInit_Context   *ctx,
  CSemaphoreReqInit_Post_Queue state
)
{
  switch ( state ) {
    case CSemaphoreReqInit_Post_Queue_Empty: {
      /*
       * The counting semaphore referenced by the `sem` parameter shall have no
       * waiting thread.
       */
      T_null( SemOf( ctx )->Queue.Queue.heads );
      break;
    }

    case CSemaphoreReqInit_Post_Queue_NA:
      break;
  }
}

static void CSemaphoreReqInit_Prepare( CSemaphoreReqInit_Context *ctx )
{
  ctx->value = 1;
}

static void CSemaphoreReqInit_Action( CSemaphoreReqInit_Context *ctx )
{
  errno = -1;
  ctx->status = sem_init( ctx->sem, ctx->pshared, ctx->value );
  ctx->errno_value = errno;
}

/* clang-format off */

static const CSemaphoreReqInit_Entry
CSemaphoreReqInit_Entries[] = {
  { 1, 0, 0, 0, CSemaphoreReqInit_Post_Status_NA,
    CSemaphoreReqInit_Post_Errno_NA, CSemaphoreReqInit_Post_Object_NA,
    CSemaphoreReqInit_Post_Count_NA, CSemaphoreReqInit_Post_Queue_NA },
  { 0, 0, 0, 0, CSemaphoreReqInit_Post_Status_Ok,
    CSemaphoreReqInit_Post_Errno_Nop,
    CSemaphoreReqInit_Post_Object_Initialized,
    CSemaphoreReqInit_Post_Count_Valid, CSemaphoreReqInit_Post_Queue_Empty },
  { 0, 0, 0, 0, CSemaphoreReqInit_Post_Status_Ok,
    CSemaphoreReqInit_Post_Errno_Nop,
    CSemaphoreReqInit_Post_Object_Initialized,
    CSemaphoreReqInit_Post_Count_Max, CSemaphoreReqInit_Post_Queue_Empty },
  { 0, 0, 0, 0, CSemaphoreReqInit_Post_Status_Error,
    CSemaphoreReqInit_Post_Errno_Inval, CSemaphoreReqInit_Post_Object_Nop,
    CSemaphoreReqInit_Post_Count_NA, CSemaphoreReqInit_Post_Queue_NA },
  { 0, 0, 0, 1, CSemaphoreReqInit_Post_Status_Error,
    CSemaphoreReqInit_Post_Errno_Inval, CSemaphoreReqInit_Post_Object_NA,
    CSemaphoreReqInit_Post_Count_NA, CSemaphoreReqInit_Post_Queue_NA }
};

static const uint8_t
CSemaphoreReqInit_Map[] = {
  1, 2, 3, 1, 2, 3, 4, 0, 0, 4, 0, 0
};

/* clang-format on */

static size_t CSemaphoreReqInit_Scope( void *arg, char *buf, size_t n )
{
  CSemaphoreReqInit_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CSemaphoreReqInit_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CSemaphoreReqInit_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CSemaphoreReqInit_Scope,
  .initial_context = &CSemaphoreReqInit_Instance
};

static inline CSemaphoreReqInit_Entry CSemaphoreReqInit_PopEntry(
  CSemaphoreReqInit_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CSemaphoreReqInit_Entries[ CSemaphoreReqInit_Map[ index ] ];
}

static void CSemaphoreReqInit_SetPreConditionStates(
  CSemaphoreReqInit_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];

  if ( ctx->Map.entry.Pre_Value_NA ) {
    ctx->Map.pcs[ 2 ] = CSemaphoreReqInit_Pre_Value_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }
}

static void CSemaphoreReqInit_TestVariant( CSemaphoreReqInit_Context *ctx )
{
  CSemaphoreReqInit_Pre_Sem_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CSemaphoreReqInit_Pre_Pshared_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CSemaphoreReqInit_Pre_Value_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CSemaphoreReqInit_Action( ctx );
  CSemaphoreReqInit_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CSemaphoreReqInit_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
  CSemaphoreReqInit_Post_Object_Check( ctx, ctx->Map.entry.Post_Object );
  CSemaphoreReqInit_Post_Count_Check( ctx, ctx->Map.entry.Post_Count );
  CSemaphoreReqInit_Post_Queue_Check( ctx, ctx->Map.entry.Post_Queue );
}

/**
 * @fn void T_case_body_CSemaphoreReqInit( void )
 */
T_TEST_CASE_FIXTURE( CSemaphoreReqInit, &CSemaphoreReqInit_Fixture )
{
  CSemaphoreReqInit_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CSemaphoreReqInit_Pre_Sem_Valid;
    ctx->Map.pci[ 0 ] < CSemaphoreReqInit_Pre_Sem_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CSemaphoreReqInit_Pre_Pshared_Zero;
      ctx->Map.pci[ 1 ] < CSemaphoreReqInit_Pre_Pshared_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CSemaphoreReqInit_Pre_Value_Valid;
        ctx->Map.pci[ 2 ] < CSemaphoreReqInit_Pre_Value_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = CSemaphoreReqInit_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        CSemaphoreReqInit_SetPreConditionStates( ctx );
        CSemaphoreReqInit_Prepare( ctx );
        CSemaphoreReqInit_TestVariant( ctx );
      }
    }
  }
}

/** @} */
