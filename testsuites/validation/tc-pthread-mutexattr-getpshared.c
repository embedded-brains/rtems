/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqMutexattrGetpshared
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

#include <errno.h>
#include <pthread.h>
#include <string.h>

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqMutexattrGetpshared \
 *   spec:/c/pthread/req/mutexattr-getpshared
 *
 * @{
 */

typedef enum {
  CPthreadReqMutexattrGetpshared_Pre_Attr_Valid,
  CPthreadReqMutexattrGetpshared_Pre_Attr_Null,
  CPthreadReqMutexattrGetpshared_Pre_Attr_Invalid,
  CPthreadReqMutexattrGetpshared_Pre_Attr_NA
} CPthreadReqMutexattrGetpshared_Pre_Attr;

typedef enum {
  CPthreadReqMutexattrGetpshared_Pre_Pshared_Valid,
  CPthreadReqMutexattrGetpshared_Pre_Pshared_Null,
  CPthreadReqMutexattrGetpshared_Pre_Pshared_NA
} CPthreadReqMutexattrGetpshared_Pre_Pshared;

typedef enum {
  CPthreadReqMutexattrGetpshared_Post_Status_Ok,
  CPthreadReqMutexattrGetpshared_Post_Status_Einval,
  CPthreadReqMutexattrGetpshared_Post_Status_NA
} CPthreadReqMutexattrGetpshared_Post_Status;

typedef enum {
  CPthreadReqMutexattrGetpshared_Post_PsharedObj_Set,
  CPthreadReqMutexattrGetpshared_Post_PsharedObj_Nop,
  CPthreadReqMutexattrGetpshared_Post_PsharedObj_NA
} CPthreadReqMutexattrGetpshared_Post_PsharedObj;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Attr_NA : 1;
  uint8_t Pre_Pshared_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_PsharedObj : 2;
} CPthreadReqMutexattrGetpshared_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/mutexattr-getpshared test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `attr` parameter value.
   */
  const pthread_mutexattr_t *attr;

  /**
   * @brief This member specifies the `pshared` parameter value.
   */
  int *pshared;

  /**
   * @brief This member contains the return value.
   */
  int retval;

  /**
   * @brief This member contains the attribute object.
   */
  pthread_mutexattr_t attr_obj;

  /**
   * @brief This member contains the pshared value.
   */
  int pshared_obj;

  /**
   * @brief This member contains the pshared output object.
   */
  int pshared_value;

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
    CPthreadReqMutexattrGetpshared_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqMutexattrGetpshared_Context;

static CPthreadReqMutexattrGetpshared_Context
  CPthreadReqMutexattrGetpshared_Instance;

static const char * const CPthreadReqMutexattrGetpshared_PreDesc_Attr[] = {
  "Valid",
  "Null",
  "Invalid",
  "NA"
};

static const char * const CPthreadReqMutexattrGetpshared_PreDesc_Pshared[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const CPthreadReqMutexattrGetpshared_PreDesc[] = {
  CPthreadReqMutexattrGetpshared_PreDesc_Attr,
  CPthreadReqMutexattrGetpshared_PreDesc_Pshared,
  NULL
};

static void CPthreadReqMutexattrGetpshared_Pre_Attr_Prepare(
  CPthreadReqMutexattrGetpshared_Context *ctx,
  CPthreadReqMutexattrGetpshared_Pre_Attr state
)
{
  switch ( state ) {
    case CPthreadReqMutexattrGetpshared_Pre_Attr_Valid: {
      /*
       * While the mutex attributes object referenced by `attr` is valid.
       */
      ctx->attr = &ctx->attr_obj;
      pthread_mutexattr_init( &ctx->attr_obj );
      pthread_mutexattr_setpshared( &ctx->attr_obj, ctx->pshared_obj );
      break;
    }

    case CPthreadReqMutexattrGetpshared_Pre_Attr_Null: {
      /*
       * While the `attr` parameter is NULL.
       */
      ctx->attr = NULL;
      break;
    }

    case CPthreadReqMutexattrGetpshared_Pre_Attr_Invalid: {
      /*
       * While the mutex attributes object referenced by `attr` is invalid.
       */
      ctx->attr = &ctx->attr_obj;
      memset( &ctx->attr_obj, 0x0, sizeof( ctx->attr_obj ) );
      break;
    }

    case CPthreadReqMutexattrGetpshared_Pre_Attr_NA:
      break;
  }
}

static void CPthreadReqMutexattrGetpshared_Pre_Pshared_Prepare(
  CPthreadReqMutexattrGetpshared_Context    *ctx,
  CPthreadReqMutexattrGetpshared_Pre_Pshared state
)
{
  switch ( state ) {
    case CPthreadReqMutexattrGetpshared_Pre_Pshared_Valid: {
      /*
       * While the `pshared` parameter references an object of type `int`.
       */
      ctx->pshared_value = -1;
      ctx->pshared = &ctx->pshared_value;
      break;
    }

    case CPthreadReqMutexattrGetpshared_Pre_Pshared_Null: {
      /*
       * While the `pshared` parameter is NULL.
       */
      ctx->pshared = NULL;
      break;
    }

    case CPthreadReqMutexattrGetpshared_Pre_Pshared_NA:
      break;
  }
}

static void CPthreadReqMutexattrGetpshared_Post_Status_Check(
  CPthreadReqMutexattrGetpshared_Context    *ctx,
  CPthreadReqMutexattrGetpshared_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqMutexattrGetpshared_Post_Status_Ok: {
      /*
       * The return status shall be zero.
       */
      T_rsc_success( ctx->retval );
      break;
    }

    case CPthreadReqMutexattrGetpshared_Post_Status_Einval: {
      /*
       * The return status shall be EINVAL.
       */
      T_rsc( ctx->retval, EINVAL );
      break;
    }

    case CPthreadReqMutexattrGetpshared_Post_Status_NA:
      break;
  }
}

static void CPthreadReqMutexattrGetpshared_Post_PsharedObj_Check(
  CPthreadReqMutexattrGetpshared_Context        *ctx,
  CPthreadReqMutexattrGetpshared_Post_PsharedObj state
)
{
  switch ( state ) {
    case CPthreadReqMutexattrGetpshared_Post_PsharedObj_Set: {
      /*
       * The value of the object referenced by the `pshared` parameter shall be
       * set to the process-shared attribute of the mutex attributes object
       * referenced by the `attr` parameter.
       */
      T_eq_int( ctx->pshared_value, ctx->pshared_obj );
      break;
    }

    case CPthreadReqMutexattrGetpshared_Post_PsharedObj_Nop: {
      /*
       * The value of the object referenced by the `pshared` parameter shall
       * not be modified.
       */
      T_eq_int( ctx->pshared_value, -1 );
      break;
    }

    case CPthreadReqMutexattrGetpshared_Post_PsharedObj_NA:
      break;
  }
}

static void CPthreadReqMutexattrGetpshared_Prepare(
  CPthreadReqMutexattrGetpshared_Context *ctx
)
{
  ctx->pshared_obj = PTHREAD_PROCESS_SHARED;
}

static void CPthreadReqMutexattrGetpshared_Action(
  CPthreadReqMutexattrGetpshared_Context *ctx
)
{
  ctx->retval = pthread_mutexattr_getpshared( ctx->attr, ctx->pshared );
}

static const CPthreadReqMutexattrGetpshared_Entry
CPthreadReqMutexattrGetpshared_Entries[] = {
  { 0, 0, 0, CPthreadReqMutexattrGetpshared_Post_Status_Einval,
    CPthreadReqMutexattrGetpshared_Post_PsharedObj_NA },
  { 0, 0, 0, CPthreadReqMutexattrGetpshared_Post_Status_Einval,
    CPthreadReqMutexattrGetpshared_Post_PsharedObj_Nop },
  { 0, 0, 0, CPthreadReqMutexattrGetpshared_Post_Status_Ok,
    CPthreadReqMutexattrGetpshared_Post_PsharedObj_Set }
};

static const uint8_t
CPthreadReqMutexattrGetpshared_Map[] = {
  2, 0, 1, 0, 1, 0
};

static size_t CPthreadReqMutexattrGetpshared_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  CPthreadReqMutexattrGetpshared_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqMutexattrGetpshared_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqMutexattrGetpshared_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqMutexattrGetpshared_Scope,
  .initial_context = &CPthreadReqMutexattrGetpshared_Instance
};

static inline CPthreadReqMutexattrGetpshared_Entry
CPthreadReqMutexattrGetpshared_PopEntry(
  CPthreadReqMutexattrGetpshared_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqMutexattrGetpshared_Entries[
    CPthreadReqMutexattrGetpshared_Map[ index ]
  ];
}

static void CPthreadReqMutexattrGetpshared_TestVariant(
  CPthreadReqMutexattrGetpshared_Context *ctx
)
{
  CPthreadReqMutexattrGetpshared_Pre_Attr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CPthreadReqMutexattrGetpshared_Pre_Pshared_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CPthreadReqMutexattrGetpshared_Action( ctx );
  CPthreadReqMutexattrGetpshared_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  CPthreadReqMutexattrGetpshared_Post_PsharedObj_Check(
    ctx,
    ctx->Map.entry.Post_PsharedObj
  );
}

/**
 * @fn void T_case_body_CPthreadReqMutexattrGetpshared( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqMutexattrGetpshared,
  &CPthreadReqMutexattrGetpshared_Fixture
)
{
  CPthreadReqMutexattrGetpshared_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqMutexattrGetpshared_Pre_Attr_Valid;
    ctx->Map.pcs[ 0 ] < CPthreadReqMutexattrGetpshared_Pre_Attr_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CPthreadReqMutexattrGetpshared_Pre_Pshared_Valid;
      ctx->Map.pcs[ 1 ] < CPthreadReqMutexattrGetpshared_Pre_Pshared_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CPthreadReqMutexattrGetpshared_PopEntry( ctx );
      CPthreadReqMutexattrGetpshared_Prepare( ctx );
      CPthreadReqMutexattrGetpshared_TestVariant( ctx );
    }
  }
}

/** @} */
