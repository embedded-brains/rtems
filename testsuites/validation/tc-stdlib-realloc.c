/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStdlibReqRealloc
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

#include <stdlib.h>

#include <rtems/test.h>

/**
 * @defgroup CStdlibReqRealloc spec:/c/stdlib/req/realloc
 *
 * @{
 */

typedef enum {
  CStdlibReqRealloc_Pre_Status_Ok,
  CStdlibReqRealloc_Pre_Status_NA
} CStdlibReqRealloc_Pre_Status;

typedef enum {
  CStdlibReqRealloc_Post_Status_Ok,
  CStdlibReqRealloc_Post_Status_NA
} CStdlibReqRealloc_Post_Status;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Status_NA : 1;
  uint8_t Post_Status : 1;
} CStdlibReqRealloc_Entry;

/**
 * @brief Test context for spec:/c/stdlib/req/realloc test case.
 */
typedef struct {
  /**
   * @brief This member specifies the `ptr` parameter value.
   */
  void *ptr;

  /**
   * @brief This member specifies the `size` parameter value.
   */
  size_t size;

  /**
   * @brief This member contains the return value.
   */
  void *retval;

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
    CStdlibReqRealloc_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStdlibReqRealloc_Context;

static CStdlibReqRealloc_Context
  CStdlibReqRealloc_Instance;

static const char * const CStdlibReqRealloc_PreDesc_Status[] = {
  "Ok",
  "NA"
};

static const char * const * const CStdlibReqRealloc_PreDesc[] = {
  CStdlibReqRealloc_PreDesc_Status,
  NULL
};

static void CStdlibReqRealloc_Pre_Status_Prepare(
  CStdlibReqRealloc_Pre_Status state
)
{
  switch ( state ) {
    case CStdlibReqRealloc_Pre_Status_Ok: {
      /*
       * TODO
       */
      /* TODOD */
      break;
    }

    case CStdlibReqRealloc_Pre_Status_NA:
      break;
  }
}

static void CStdlibReqRealloc_Post_Status_Check(
  CStdlibReqRealloc_Post_Status state
)
{
  switch ( state ) {
    case CStdlibReqRealloc_Post_Status_Ok: {
      /*
       * TODO
       */
      /* TODOD */
      break;
    }

    case CStdlibReqRealloc_Post_Status_NA:
      break;
  }
}

static void CStdlibReqRealloc_Action( CStdlibReqRealloc_Context *ctx )
{
  ctx->retval = realloc( ctx->ptr, ctx->size );
}

static const CStdlibReqRealloc_Entry
CStdlibReqRealloc_Entries[] = {
  { 0, 0, CStdlibReqRealloc_Post_Status_Ok }
};

static const uint8_t
CStdlibReqRealloc_Map[] = {
  0
};

static size_t CStdlibReqRealloc_Scope( void *arg, char *buf, size_t n )
{
  CStdlibReqRealloc_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStdlibReqRealloc_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStdlibReqRealloc_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CStdlibReqRealloc_Scope,
  .initial_context = &CStdlibReqRealloc_Instance
};

static inline CStdlibReqRealloc_Entry CStdlibReqRealloc_PopEntry(
  CStdlibReqRealloc_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStdlibReqRealloc_Entries[
    CStdlibReqRealloc_Map[ index ]
  ];
}

static void CStdlibReqRealloc_TestVariant( CStdlibReqRealloc_Context *ctx )
{
  CStdlibReqRealloc_Pre_Status_Prepare( ctx->Map.pcs[ 0 ] );
  CStdlibReqRealloc_Action( ctx );
  CStdlibReqRealloc_Post_Status_Check( ctx->Map.entry.Post_Status );
}

/**
 * @fn void T_case_body_CStdlibReqRealloc( void )
 */
T_TEST_CASE_FIXTURE( CStdlibReqRealloc, &CStdlibReqRealloc_Fixture )
{
  CStdlibReqRealloc_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CStdlibReqRealloc_Pre_Status_Ok;
    ctx->Map.pcs[ 0 ] < CStdlibReqRealloc_Pre_Status_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CStdlibReqRealloc_PopEntry( ctx );
    CStdlibReqRealloc_TestVariant( ctx );
  }
}

/** @} */
