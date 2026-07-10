/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CTimeReqTimespecGet
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

#include <time.h>

#include <rtems/test.h>

/**
 * @defgroup CTimeReqTimespecGet spec:/c/time/req/timespec-get
 *
 * @{
 */

typedef enum {
  CTimeReqTimespecGet_Pre_Ts_Valid,
  CTimeReqTimespecGet_Pre_Ts_Null,
  CTimeReqTimespecGet_Pre_Ts_NA
} CTimeReqTimespecGet_Pre_Ts;

typedef enum {
  CTimeReqTimespecGet_Pre_Base_TimeUtc,
  CTimeReqTimespecGet_Pre_Base_Invalid,
  CTimeReqTimespecGet_Pre_Base_NA
} CTimeReqTimespecGet_Pre_Base;

typedef enum {
  CTimeReqTimespecGet_Post_Result_Base,
  CTimeReqTimespecGet_Post_Result_Zero,
  CTimeReqTimespecGet_Post_Result_NA
} CTimeReqTimespecGet_Post_Result;

typedef enum {
  CTimeReqTimespecGet_Post_TsObj_Set,
  CTimeReqTimespecGet_Post_TsObj_Unchanged,
  CTimeReqTimespecGet_Post_TsObj_NA
} CTimeReqTimespecGet_Post_TsObj;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Ts_NA : 1;
  uint8_t Pre_Base_NA : 1;
  uint8_t Post_Result : 2;
  uint8_t Post_TsObj : 2;
} CTimeReqTimespecGet_Entry;

/**
 * @brief Test context for spec:/c/time/req/timespec-get test case.
 */
typedef struct {
  /**
   * @brief This member contains the return value of the timespec_get() call.
   */
  int result;

  /**
   * @brief This member specifies the `ts` parameter value.
   */
  struct timespec *ts;

  /**
   * @brief This member provides the object referenced by the `ts` parameter.
   */
  struct timespec ts_obj;

  /**
   * @brief This member specifies the `base` parameter value.
   */
  int base;

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
    CTimeReqTimespecGet_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CTimeReqTimespecGet_Context;

static CTimeReqTimespecGet_Context
  CTimeReqTimespecGet_Instance;

static const char * const CTimeReqTimespecGet_PreDesc_Ts[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const CTimeReqTimespecGet_PreDesc_Base[] = {
  "TimeUtc",
  "Invalid",
  "NA"
};

static const char * const * const CTimeReqTimespecGet_PreDesc[] = {
  CTimeReqTimespecGet_PreDesc_Ts,
  CTimeReqTimespecGet_PreDesc_Base,
  NULL
};

static void CTimeReqTimespecGet_Pre_Ts_Prepare(
  CTimeReqTimespecGet_Context *ctx,
  CTimeReqTimespecGet_Pre_Ts   state
)
{
  switch ( state ) {
    case CTimeReqTimespecGet_Pre_Ts_Valid: {
      /*
       * While the `ts` parameter references an object of type struct timespec.
       */
      ctx->ts = &ctx->ts_obj;
      break;
    }

    case CTimeReqTimespecGet_Pre_Ts_Null: {
      /*
       * While the `ts` parameter is equal to NULL.
       */
      ctx->ts = NULL;
      break;
    }

    case CTimeReqTimespecGet_Pre_Ts_NA:
      break;
  }
}

static void CTimeReqTimespecGet_Pre_Base_Prepare(
  CTimeReqTimespecGet_Context *ctx,
  CTimeReqTimespecGet_Pre_Base state
)
{
  switch ( state ) {
    case CTimeReqTimespecGet_Pre_Base_TimeUtc: {
      /*
       * While the `base` parameter is equal to TIME_UTC.
       */
      ctx->base = TIME_UTC;
      break;
    }

    case CTimeReqTimespecGet_Pre_Base_Invalid: {
      /*
       * While the `base` parameter is not equal to TIME_UTC.
       */
      ctx->base = -1;
      break;
    }

    case CTimeReqTimespecGet_Pre_Base_NA:
      break;
  }
}

static void CTimeReqTimespecGet_Post_Result_Check(
  CTimeReqTimespecGet_Context    *ctx,
  CTimeReqTimespecGet_Post_Result state
)
{
  switch ( state ) {
    case CTimeReqTimespecGet_Post_Result_Base: {
      /*
       * The return value of timespec_get() shall be equal to TIME_UTC.
       */
      T_eq_int( ctx->result, TIME_UTC );
      break;
    }

    case CTimeReqTimespecGet_Post_Result_Zero: {
      /*
       * The return value of timespec_get() shall be equal to zero.
       */
      T_eq_int( ctx->result, 0 );
      break;
    }

    case CTimeReqTimespecGet_Post_Result_NA:
      break;
  }
}

static void CTimeReqTimespecGet_Post_TsObj_Check(
  CTimeReqTimespecGet_Context   *ctx,
  CTimeReqTimespecGet_Post_TsObj state
)
{
  switch ( state ) {
    case CTimeReqTimespecGet_Post_TsObj_Set: {
      /*
       * The object referenced by the `ts` parameter shall be set to the
       * current time based on TIME_UTC.
       */
      T_gt_i64( ctx->ts_obj.tv_sec, 0 );
      break;
    }

    case CTimeReqTimespecGet_Post_TsObj_Unchanged: {
      /*
       * The object referenced by the `ts` parameter shall not be changed by
       * the directive call.
       */
      T_eq_long( ctx->ts_obj.tv_sec, -1 );
      T_eq_long( ctx->ts_obj.tv_nsec, -1 );
      break;
    }

    case CTimeReqTimespecGet_Post_TsObj_NA:
      break;
  }
}

static void CTimeReqTimespecGet_Setup( void )
{
  
}

static void CTimeReqTimespecGet_Setup_Wrap( void *arg )
{
  CTimeReqTimespecGet_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CTimeReqTimespecGet_Setup();
}

static void CTimeReqTimespecGet_Teardown( void )
{
  
}

static void CTimeReqTimespecGet_Teardown_Wrap( void *arg )
{
  CTimeReqTimespecGet_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CTimeReqTimespecGet_Teardown();
}

static void CTimeReqTimespecGet_Prepare( CTimeReqTimespecGet_Context *ctx )
{
  ctx->result = -1;
  ctx->ts_obj.tv_sec = -1;
  ctx->ts_obj.tv_nsec = -1;
}

static void CTimeReqTimespecGet_Action( CTimeReqTimespecGet_Context *ctx )
{
  ctx->result = timespec_get( ctx->ts, ctx->base );
}

static const CTimeReqTimespecGet_Entry
CTimeReqTimespecGet_Entries[] = {
  { 0, 0, 0, CTimeReqTimespecGet_Post_Result_Zero,
    CTimeReqTimespecGet_Post_TsObj_Unchanged },
  { 0, 0, 0, CTimeReqTimespecGet_Post_Result_Base,
    CTimeReqTimespecGet_Post_TsObj_Set },
  { 0, 0, 0, CTimeReqTimespecGet_Post_Result_Base,
    CTimeReqTimespecGet_Post_TsObj_NA }
};

static const uint8_t
CTimeReqTimespecGet_Map[] = {
  1, 0, 2, 0
};

static size_t CTimeReqTimespecGet_Scope( void *arg, char *buf, size_t n )
{
  CTimeReqTimespecGet_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CTimeReqTimespecGet_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CTimeReqTimespecGet_Fixture = {
  .setup = CTimeReqTimespecGet_Setup_Wrap,
  .stop = NULL,
  .teardown = CTimeReqTimespecGet_Teardown_Wrap,
  .scope = CTimeReqTimespecGet_Scope,
  .initial_context = &CTimeReqTimespecGet_Instance
};

static inline CTimeReqTimespecGet_Entry CTimeReqTimespecGet_PopEntry(
  CTimeReqTimespecGet_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CTimeReqTimespecGet_Entries[
    CTimeReqTimespecGet_Map[ index ]
  ];
}

static void CTimeReqTimespecGet_TestVariant( CTimeReqTimespecGet_Context *ctx )
{
  CTimeReqTimespecGet_Pre_Ts_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CTimeReqTimespecGet_Pre_Base_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CTimeReqTimespecGet_Action( ctx );
  CTimeReqTimespecGet_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
  CTimeReqTimespecGet_Post_TsObj_Check( ctx, ctx->Map.entry.Post_TsObj );
}

/**
 * @fn void T_case_body_CTimeReqTimespecGet( void )
 */
T_TEST_CASE_FIXTURE( CTimeReqTimespecGet, &CTimeReqTimespecGet_Fixture )
{
  CTimeReqTimespecGet_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CTimeReqTimespecGet_Pre_Ts_Valid;
    ctx->Map.pcs[ 0 ] < CTimeReqTimespecGet_Pre_Ts_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CTimeReqTimespecGet_Pre_Base_TimeUtc;
      ctx->Map.pcs[ 1 ] < CTimeReqTimespecGet_Pre_Base_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CTimeReqTimespecGet_PopEntry( ctx );
      CTimeReqTimespecGet_Prepare( ctx );
      CTimeReqTimespecGet_TestVariant( ctx );
    }
  }
}

/** @} */
