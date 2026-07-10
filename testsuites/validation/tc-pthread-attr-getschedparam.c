/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqAttrGetschedparam
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
 * @defgroup CPthreadReqAttrGetschedparam \
 *   spec:/c/pthread/req/attr-getschedparam
 *
 * @{
 */

typedef enum {
  CPthreadReqAttrGetschedparam_Pre_Status_Ok,
  CPthreadReqAttrGetschedparam_Pre_Status_NA
} CPthreadReqAttrGetschedparam_Pre_Status;

typedef enum {
  CPthreadReqAttrGetschedparam_Post_Status_Ok,
  CPthreadReqAttrGetschedparam_Post_Status_NA
} CPthreadReqAttrGetschedparam_Post_Status;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Status_NA : 1;
  uint8_t Post_Status : 1;
} CPthreadReqAttrGetschedparam_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/attr-getschedparam test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `attr` parameter value.
   */
  const pthread_attr_t *attr;

  /**
   * @brief This member specifies the `param` parameter value.
   */
  struct sched_param *param;

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
    CPthreadReqAttrGetschedparam_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqAttrGetschedparam_Context;

static CPthreadReqAttrGetschedparam_Context
  CPthreadReqAttrGetschedparam_Instance;

static const char * const CPthreadReqAttrGetschedparam_PreDesc_Status[] = {
  "Ok",
  "NA"
};

static const char * const * const CPthreadReqAttrGetschedparam_PreDesc[] = {
  CPthreadReqAttrGetschedparam_PreDesc_Status,
  NULL
};

static void CPthreadReqAttrGetschedparam_Pre_Status_Prepare(
  CPthreadReqAttrGetschedparam_Pre_Status state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetschedparam_Pre_Status_Ok: {
      /*
       * TODO
       */
      /* TODOD */
      break;
    }

    case CPthreadReqAttrGetschedparam_Pre_Status_NA:
      break;
  }
}

static void CPthreadReqAttrGetschedparam_Post_Status_Check(
  CPthreadReqAttrGetschedparam_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqAttrGetschedparam_Post_Status_Ok: {
      /*
       * TODO
       */
      /* TODOD */
      break;
    }

    case CPthreadReqAttrGetschedparam_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrGetschedparam_Action(
  CPthreadReqAttrGetschedparam_Context *ctx
)
{
  ctx->retval = pthread_attr_getschedparam( ctx->attr, ctx->param );
}

static const CPthreadReqAttrGetschedparam_Entry
CPthreadReqAttrGetschedparam_Entries[] = {
  { 0, 0, CPthreadReqAttrGetschedparam_Post_Status_Ok }
};

static const uint8_t
CPthreadReqAttrGetschedparam_Map[] = {
  0
};

static size_t CPthreadReqAttrGetschedparam_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  CPthreadReqAttrGetschedparam_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqAttrGetschedparam_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqAttrGetschedparam_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqAttrGetschedparam_Scope,
  .initial_context = &CPthreadReqAttrGetschedparam_Instance
};

static inline CPthreadReqAttrGetschedparam_Entry
CPthreadReqAttrGetschedparam_PopEntry(
  CPthreadReqAttrGetschedparam_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqAttrGetschedparam_Entries[
    CPthreadReqAttrGetschedparam_Map[ index ]
  ];
}

static void CPthreadReqAttrGetschedparam_TestVariant(
  CPthreadReqAttrGetschedparam_Context *ctx
)
{
  CPthreadReqAttrGetschedparam_Pre_Status_Prepare( ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrGetschedparam_Action( ctx );
  CPthreadReqAttrGetschedparam_Post_Status_Check( ctx->Map.entry.Post_Status );
}

/**
 * @fn void T_case_body_CPthreadReqAttrGetschedparam( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqAttrGetschedparam,
  &CPthreadReqAttrGetschedparam_Fixture
)
{
  CPthreadReqAttrGetschedparam_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqAttrGetschedparam_Pre_Status_Ok;
    ctx->Map.pcs[ 0 ] < CPthreadReqAttrGetschedparam_Pre_Status_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CPthreadReqAttrGetschedparam_PopEntry( ctx );
    CPthreadReqAttrGetschedparam_TestVariant( ctx );
  }
}

/** @} */
