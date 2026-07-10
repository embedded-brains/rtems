/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStringReqStrncpy
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

#include <string.h>

#include <rtems/test.h>

/**
 * @defgroup CStringReqStrncpy spec:/c/string/req/strncpy
 *
 * @{
 */

typedef enum {
  CStringReqStrncpy_Pre_Status_Ok,
  CStringReqStrncpy_Pre_Status_NA
} CStringReqStrncpy_Pre_Status;

typedef enum {
  CStringReqStrncpy_Post_Status_Ok,
  CStringReqStrncpy_Post_Status_NA
} CStringReqStrncpy_Post_Status;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Status_NA : 1;
  uint8_t Post_Status : 1;
} CStringReqStrncpy_Entry;

/**
 * @brief Test context for spec:/c/string/req/strncpy test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `dest` parameter value.
   */
  char * dest;

  /**
   * @brief This member specifies the `src` parameter value.
   */
  const char * src;

  /**
   * @brief This member specifies the `n` parameter value.
   */
  size_t n;

  /**
   * @brief This member contains the return value.
   */
  char *retval;

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
    CStringReqStrncpy_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStringReqStrncpy_Context;

static CStringReqStrncpy_Context
  CStringReqStrncpy_Instance;

static const char * const CStringReqStrncpy_PreDesc_Status[] = {
  "Ok",
  "NA"
};

static const char * const * const CStringReqStrncpy_PreDesc[] = {
  CStringReqStrncpy_PreDesc_Status,
  NULL
};

static void CStringReqStrncpy_Pre_Status_Prepare(
  CStringReqStrncpy_Pre_Status state
)
{
  switch ( state ) {
    case CStringReqStrncpy_Pre_Status_Ok: {
      /*
       * TODO
       */
      /* TODOD */
      break;
    }

    case CStringReqStrncpy_Pre_Status_NA:
      break;
  }
}

static void CStringReqStrncpy_Post_Status_Check(
  CStringReqStrncpy_Post_Status state
)
{
  switch ( state ) {
    case CStringReqStrncpy_Post_Status_Ok: {
      /*
       * TODO
       */
      /* TODOD */
      break;
    }

    case CStringReqStrncpy_Post_Status_NA:
      break;
  }
}

static void CStringReqStrncpy_Action( CStringReqStrncpy_Context *ctx )
{
  ctx->retval = strncpy( ctx->dest, ctx->src, ctx->n );
}

static const CStringReqStrncpy_Entry
CStringReqStrncpy_Entries[] = {
  { 0, 0, CStringReqStrncpy_Post_Status_Ok }
};

static const uint8_t
CStringReqStrncpy_Map[] = {
  0
};

static size_t CStringReqStrncpy_Scope( void *arg, char *buf, size_t n )
{
  CStringReqStrncpy_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStringReqStrncpy_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStringReqStrncpy_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStringReqStrncpy_Scope,
  .initial_context = &CStringReqStrncpy_Instance
};

static inline CStringReqStrncpy_Entry CStringReqStrncpy_PopEntry(
  CStringReqStrncpy_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStringReqStrncpy_Entries[
    CStringReqStrncpy_Map[ index ]
  ];
}

static void CStringReqStrncpy_TestVariant( CStringReqStrncpy_Context *ctx )
{
  CStringReqStrncpy_Pre_Status_Prepare( ctx->Map.pcs[ 0 ] );
  CStringReqStrncpy_Action( ctx );
  CStringReqStrncpy_Post_Status_Check( ctx->Map.entry.Post_Status );
}

/**
 * @fn void T_case_body_CStringReqStrncpy( void )
 */
T_TEST_CASE_FIXTURE( CStringReqStrncpy, &CStringReqStrncpy_Fixture )
{
  CStringReqStrncpy_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CStringReqStrncpy_Pre_Status_Ok;
    ctx->Map.pcs[ 0 ] < CStringReqStrncpy_Pre_Status_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CStringReqStrncpy_PopEntry( ctx );
    CStringReqStrncpy_TestVariant( ctx );
  }
}

/** @} */
