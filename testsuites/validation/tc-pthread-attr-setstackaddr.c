/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqAttrSetstackaddr
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
 * @defgroup CPthreadReqAttrSetstackaddr spec:/c/pthread/req/attr-setstackaddr
 *
 * @{
 */

typedef enum {
  CPthreadReqAttrSetstackaddr_Pre_Status_Ok,
  CPthreadReqAttrSetstackaddr_Pre_Status_NA
} CPthreadReqAttrSetstackaddr_Pre_Status;

typedef enum {
  CPthreadReqAttrSetstackaddr_Post_Status_Ok,
  CPthreadReqAttrSetstackaddr_Post_Status_NA
} CPthreadReqAttrSetstackaddr_Post_Status;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Status_NA : 1;
  uint8_t Post_Status : 1;
} CPthreadReqAttrSetstackaddr_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/attr-setstackaddr test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `attr` parameter value.
   */
  pthread_attr_t *attr;

  /**
   * @brief This member specifies the `stackaddr` parameter value.
   */
  void *stackaddr;

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
    CPthreadReqAttrSetstackaddr_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqAttrSetstackaddr_Context;

static CPthreadReqAttrSetstackaddr_Context
  CPthreadReqAttrSetstackaddr_Instance;

static const char * const CPthreadReqAttrSetstackaddr_PreDesc_Status[] = {
  "Ok",
  "NA"
};

static const char * const * const CPthreadReqAttrSetstackaddr_PreDesc[] = {
  CPthreadReqAttrSetstackaddr_PreDesc_Status,
  NULL
};

static void CPthreadReqAttrSetstackaddr_Pre_Status_Prepare(
  CPthreadReqAttrSetstackaddr_Pre_Status state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetstackaddr_Pre_Status_Ok: {
      /*
       * TODO
       */
      /* TODOD */
      break;
    }

    case CPthreadReqAttrSetstackaddr_Pre_Status_NA:
      break;
  }
}

static void CPthreadReqAttrSetstackaddr_Post_Status_Check(
  CPthreadReqAttrSetstackaddr_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqAttrSetstackaddr_Post_Status_Ok: {
      /*
       * TODO
       */
      /* TODOD */
      break;
    }

    case CPthreadReqAttrSetstackaddr_Post_Status_NA:
      break;
  }
}

static void CPthreadReqAttrSetstackaddr_Action(
  CPthreadReqAttrSetstackaddr_Context *ctx
)
{
  ctx->retval = pthread_attr_setstackaddr( ctx->attr, ctx->stackaddr );
}

static const CPthreadReqAttrSetstackaddr_Entry
CPthreadReqAttrSetstackaddr_Entries[] = {
  { 0, 0, CPthreadReqAttrSetstackaddr_Post_Status_Ok }
};

static const uint8_t
CPthreadReqAttrSetstackaddr_Map[] = {
  0
};

static size_t CPthreadReqAttrSetstackaddr_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  CPthreadReqAttrSetstackaddr_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CPthreadReqAttrSetstackaddr_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CPthreadReqAttrSetstackaddr_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqAttrSetstackaddr_Scope,
  .initial_context = &CPthreadReqAttrSetstackaddr_Instance
};

static inline CPthreadReqAttrSetstackaddr_Entry
CPthreadReqAttrSetstackaddr_PopEntry(
  CPthreadReqAttrSetstackaddr_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqAttrSetstackaddr_Entries[
    CPthreadReqAttrSetstackaddr_Map[ index ]
  ];
}

static void CPthreadReqAttrSetstackaddr_TestVariant(
  CPthreadReqAttrSetstackaddr_Context *ctx
)
{
  CPthreadReqAttrSetstackaddr_Pre_Status_Prepare( ctx->Map.pcs[ 0 ] );
  CPthreadReqAttrSetstackaddr_Action( ctx );
  CPthreadReqAttrSetstackaddr_Post_Status_Check( ctx->Map.entry.Post_Status );
}

/**
 * @fn void T_case_body_CPthreadReqAttrSetstackaddr( void )
 */
T_TEST_CASE_FIXTURE(
  CPthreadReqAttrSetstackaddr,
  &CPthreadReqAttrSetstackaddr_Fixture
)
{
  CPthreadReqAttrSetstackaddr_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqAttrSetstackaddr_Pre_Status_Ok;
    ctx->Map.pcs[ 0 ] < CPthreadReqAttrSetstackaddr_Pre_Status_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CPthreadReqAttrSetstackaddr_PopEntry( ctx );
    CPthreadReqAttrSetstackaddr_TestVariant( ctx );
  }
}

/** @} */
