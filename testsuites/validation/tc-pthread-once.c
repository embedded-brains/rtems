/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CPthreadReqOnce
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
 * @defgroup CPthreadReqOnce spec:/c/pthread/req/once
 *
 * @{
 */

typedef enum {
  CPthreadReqOnce_Pre_Status_Ok,
  CPthreadReqOnce_Pre_Status_NA
} CPthreadReqOnce_Pre_Status;

typedef enum {
  CPthreadReqOnce_Post_Status_Ok,
  CPthreadReqOnce_Post_Status_NA
} CPthreadReqOnce_Post_Status;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Status_NA : 1;
  uint8_t Post_Status : 1;
} CPthreadReqOnce_Entry;

/**
 * @brief Test context for spec:/c/pthread/req/once test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `once_control` parameter value.
   */
  pthread_once_t *once_control;

  /**
   * @brief This member specifies the `init_routine` parameter value.
   */
  void ( *init_routine )( void );

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
    CPthreadReqOnce_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CPthreadReqOnce_Context;

static CPthreadReqOnce_Context
  CPthreadReqOnce_Instance;

static const char * const CPthreadReqOnce_PreDesc_Status[] = {
  "Ok",
  "NA"
};

static const char * const * const CPthreadReqOnce_PreDesc[] = {
  CPthreadReqOnce_PreDesc_Status,
  NULL
};

static void CPthreadReqOnce_Pre_Status_Prepare(
  CPthreadReqOnce_Pre_Status state
)
{
  switch ( state ) {
    case CPthreadReqOnce_Pre_Status_Ok: {
      /*
       * TODO
       */
      /* TODOD */
      break;
    }

    case CPthreadReqOnce_Pre_Status_NA:
      break;
  }
}

static void CPthreadReqOnce_Post_Status_Check(
  CPthreadReqOnce_Post_Status state
)
{
  switch ( state ) {
    case CPthreadReqOnce_Post_Status_Ok: {
      /*
       * TODO
       */
      /* TODOD */
      break;
    }

    case CPthreadReqOnce_Post_Status_NA:
      break;
  }
}

static void CPthreadReqOnce_Action( CPthreadReqOnce_Context *ctx )
{
  ctx->retval = pthread_once( ctx->once_control, ctx->init_routine );
}

static const CPthreadReqOnce_Entry
CPthreadReqOnce_Entries[] = {
  { 0, 0, CPthreadReqOnce_Post_Status_Ok }
};

static const uint8_t
CPthreadReqOnce_Map[] = {
  0
};

static size_t CPthreadReqOnce_Scope( void *arg, char *buf, size_t n )
{
  CPthreadReqOnce_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CPthreadReqOnce_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CPthreadReqOnce_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CPthreadReqOnce_Scope,
  .initial_context = &CPthreadReqOnce_Instance
};

static inline CPthreadReqOnce_Entry CPthreadReqOnce_PopEntry(
  CPthreadReqOnce_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CPthreadReqOnce_Entries[
    CPthreadReqOnce_Map[ index ]
  ];
}

static void CPthreadReqOnce_TestVariant( CPthreadReqOnce_Context *ctx )
{
  CPthreadReqOnce_Pre_Status_Prepare( ctx->Map.pcs[ 0 ] );
  CPthreadReqOnce_Action( ctx );
  CPthreadReqOnce_Post_Status_Check( ctx->Map.entry.Post_Status );
}

/**
 * @fn void T_case_body_CPthreadReqOnce( void )
 */
T_TEST_CASE_FIXTURE( CPthreadReqOnce, &CPthreadReqOnce_Fixture )
{
  CPthreadReqOnce_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CPthreadReqOnce_Pre_Status_Ok;
    ctx->Map.pcs[ 0 ] < CPthreadReqOnce_Pre_Status_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CPthreadReqOnce_PopEntry( ctx );
    CPthreadReqOnce_TestVariant( ctx );
  }
}

/** @} */
