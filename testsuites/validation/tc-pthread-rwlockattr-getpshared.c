/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqRwlockattrGetpshared
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

#include <rtems/test.h>

/**
 * @defgroup CPthreadReqRwlockattrGetpshared \
 *   spec:/c/pthread/req/rwlockattr-getpshared
 *
 * @{
 */

typedef enum {
  CPthreadReqRwlockattrGetpshared_Pre_Status_Ok,
  CPthreadReqRwlockattrGetpshared_Pre_Status_NA
} CPthreadReqRwlockattrGetpshared_Pre_Status;

typedef enum {
  CPthreadReqRwlockattrGetpshared_Post_Status_Ok,
  CPthreadReqRwlockattrGetpshared_Post_Status_NA
} CPthreadReqRwlockattrGetpshared_Post_Status;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Status_NA : 1;
  uint8_t Post_Status : 1;
} CPthreadReqRwlockattrGetpshared_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/rwlockattr-getpshared test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `attr` parameter value.
   */
  const pthread_rwlockattr_t *attr;

  /**
   * @brief This member specifies the `pshared` parameter value.
   */
  int *pshared;

  /**
   * @brief This member contains the return value.
   */
  int retval;

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
    CPthreadReqRwlockattrGetpshared_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqRwlockattrGetpshared_Context;

static CPthreadReqRwlockattrGetpshared_Context
  CPthreadReqRwlockattrGetpshared_Instance;

static const char * const CPthreadReqRwlockattrGetpshared_PreDesc_Status[] = {
  "Ok",
  "NA"
};

static const char * const * const CPthreadReqRwlockattrGetpshared_PreDesc[] = {
  CPthreadReqRwlockattrGetpshared_PreDesc_Status,
  NULL
};

static void CPthreadReqRwlockattrGetpshared_Pre_Status_Prepare(
  CPthreadReqRwlockattrGetpshared_Pre_Status state
)
{
  switch ( state ) {
    case CPthreadReqRwlockattrGetpshared_Pre_Status_Ok: {
      /*
       * TODO
       */
      /* TODOD */
      break;
    }

    case CPthreadReqRwlockattrGetpshared_Pre_Status_NA:
      break;
  }
}

static void CPthreadReqRwlockattrGetpshared_Post_Status_Check(
  CPthreadReqRwlockattrGetpshared_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqRwlockattrGetpshared_Post_Status_Ok: {
      /*
       * TODO
       */
      /* TODOD */
      break;
    }

    case CPthreadReqRwlockattrGetpshared_Post_Status_NA:
      break;
  }
}

static void CPthreadReqRwlockattrGetpshared_Action(
  CPthreadReqRwlockattrGetpshared_Context *ctx
)
{
  ctx->retval = pthread_rwlockattr_getpshared( ctx->attr, ctx->pshared );
}

static const CPthreadReqRwlockattrGetpshared_Entry
CPthreadReqRwlockattrGetpshared_Entries[] = {
  { 0, 0, CPthreadReqRwlockattrGetpshared_Post_Status_Ok }
};

static const uint8_t
CPthreadReqRwlockattrGetpshared_Map[] = {
  0
};

static size_t CPthreadReqRwlockattrGetpshared_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  CPthreadReqRwlockattrGetpshared_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqRwlockattrGetpshared_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqRwlockattrGetpshared_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqRwlockattrGetpshared_Scope,
  .initial_context = &CPthreadReqRwlockattrGetpshared_Instance
};

static inline CPthreadReqRwlockattrGetpshared_Entry
CPthreadReqRwlockattrGetpshared_PopEntry(
  CPthreadReqRwlockattrGetpshared_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqRwlockattrGetpshared_Entries[
    CPthreadReqRwlockattrGetpshared_Map[ index ]
  ];
}

static void CPthreadReqRwlockattrGetpshared_TestVariant(
  CPthreadReqRwlockattrGetpshared_Context *ctx
)
{
  CPthreadReqRwlockattrGetpshared_Pre_Status_Prepare( ctx->Map.pcs[ 0 ] );
  CPthreadReqRwlockattrGetpshared_Action( ctx );
  CPthreadReqRwlockattrGetpshared_Post_Status_Check(
    ctx->Map.entry.Post_Status
  );
}

/**
 * @fn void T_case_body_CPthreadReqRwlockattrGetpshared( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqRwlockattrGetpshared,
  &CPthreadReqRwlockattrGetpshared_Fixture
)
{
  CPthreadReqRwlockattrGetpshared_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqRwlockattrGetpshared_Pre_Status_Ok;
    ctx->Map.pcs[ 0 ] < CPthreadReqRwlockattrGetpshared_Pre_Status_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CPthreadReqRwlockattrGetpshared_PopEntry( ctx );
    CPthreadReqRwlockattrGetpshared_TestVariant( ctx );
  }
}

/** @} */
