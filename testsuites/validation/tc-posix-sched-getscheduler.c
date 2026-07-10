/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CSchedReqGetscheduler
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

#include <sched.h>

#include <rtems/test.h>

/**
 * @defgroup CSchedReqGetscheduler spec:/c/sched/req/getscheduler
 *
 * @{
 */

typedef enum {
  CSchedReqGetscheduler_Pre_Status_Ok,
  CSchedReqGetscheduler_Pre_Status_NA
} CSchedReqGetscheduler_Pre_Status;

typedef enum {
  CSchedReqGetscheduler_Post_Status_Ok,
  CSchedReqGetscheduler_Post_Status_NA
} CSchedReqGetscheduler_Post_Status;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Status_NA : 1;
  uint8_t Post_Status : 1;
} CSchedReqGetscheduler_Entry;

/**
 * @brief Test context for spec:/c/sched/req/getscheduler test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `pid` parameter value.
   */
  pid_t pid;

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
    CSchedReqGetscheduler_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CSchedReqGetscheduler_Context;

static CSchedReqGetscheduler_Context
  CSchedReqGetscheduler_Instance;

static const char * const CSchedReqGetscheduler_PreDesc_Status[] = {
  "Ok",
  "NA"
};

static const char * const * const CSchedReqGetscheduler_PreDesc[] = {
  CSchedReqGetscheduler_PreDesc_Status,
  NULL
};

static void CSchedReqGetscheduler_Pre_Status_Prepare(
  CSchedReqGetscheduler_Pre_Status state
)
{
  switch ( state ) {
    case CSchedReqGetscheduler_Pre_Status_Ok: {
      /*
       * TODO
       */
      /* TODOD */
      break;
    }

    case CSchedReqGetscheduler_Pre_Status_NA:
      break;
  }
}

static void CSchedReqGetscheduler_Post_Status_Check(
  CSchedReqGetscheduler_Post_Status state
)
{
  switch ( state ) {
    case CSchedReqGetscheduler_Post_Status_Ok: {
      /*
       * TODO
       */
      /* TODOD */
      break;
    }

    case CSchedReqGetscheduler_Post_Status_NA:
      break;
  }
}

static void CSchedReqGetscheduler_Action( CSchedReqGetscheduler_Context *ctx )
{
  ctx->retval = sched_getscheduler( ctx->pid );
}

static const CSchedReqGetscheduler_Entry
CSchedReqGetscheduler_Entries[] = {
  { 0, 0, CSchedReqGetscheduler_Post_Status_Ok }
};

static const uint8_t
CSchedReqGetscheduler_Map[] = {
  0
};

static size_t CSchedReqGetscheduler_Scope( void *arg, char *buf, size_t n )
{
  CSchedReqGetscheduler_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CSchedReqGetscheduler_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CSchedReqGetscheduler_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CSchedReqGetscheduler_Scope,
  .initial_context = &CSchedReqGetscheduler_Instance
};

static inline CSchedReqGetscheduler_Entry CSchedReqGetscheduler_PopEntry(
  CSchedReqGetscheduler_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CSchedReqGetscheduler_Entries[
    CSchedReqGetscheduler_Map[ index ]
  ];
}

static void CSchedReqGetscheduler_TestVariant(
  CSchedReqGetscheduler_Context *ctx
)
{
  CSchedReqGetscheduler_Pre_Status_Prepare( ctx->Map.pcs[ 0 ] );
  CSchedReqGetscheduler_Action( ctx );
  CSchedReqGetscheduler_Post_Status_Check( ctx->Map.entry.Post_Status );
}

/**
 * @fn void T_case_body_CSchedReqGetscheduler( void )
 */
T_TEST_CASE_FIXTURE( CSchedReqGetscheduler, &CSchedReqGetscheduler_Fixture )
{
  CSchedReqGetscheduler_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CSchedReqGetscheduler_Pre_Status_Ok;
    ctx->Map.pcs[ 0 ] < CSchedReqGetscheduler_Pre_Status_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CSchedReqGetscheduler_PopEntry( ctx );
    CSchedReqGetscheduler_TestVariant( ctx );
  }
}

/** @} */
