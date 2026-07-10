/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CTimeReqTimespecGetres
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

#include <stdint.h>
#include <time.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CTimeReqTimespecGetres spec:/c/time/req/timespec-getres
 *
 * @{
 */

typedef enum {
  CTimeReqTimespecGetres_Pre_Res_Valid,
  CTimeReqTimespecGetres_Pre_Res_Null,
  CTimeReqTimespecGetres_Pre_Res_NA
} CTimeReqTimespecGetres_Pre_Res;

typedef enum {
  CTimeReqTimespecGetres_Pre_Base_TimeUtc,
  CTimeReqTimespecGetres_Pre_Base_Invalid,
  CTimeReqTimespecGetres_Pre_Base_NA
} CTimeReqTimespecGetres_Pre_Base;

typedef enum {
  CTimeReqTimespecGetres_Pre_TimecounterFreq_Low,
  CTimeReqTimespecGetres_Pre_TimecounterFreq_High,
  CTimeReqTimespecGetres_Pre_TimecounterFreq_NA
} CTimeReqTimespecGetres_Pre_TimecounterFreq;

typedef enum {
  CTimeReqTimespecGetres_Post_Result_Base,
  CTimeReqTimespecGetres_Post_Result_Zero,
  CTimeReqTimespecGetres_Post_Result_NA
} CTimeReqTimespecGetres_Post_Result;

typedef enum {
  CTimeReqTimespecGetres_Post_ResObj_Set,
  CTimeReqTimespecGetres_Post_ResObj_Unchanged,
  CTimeReqTimespecGetres_Post_ResObj_NA
} CTimeReqTimespecGetres_Post_ResObj;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Res_NA : 1;
  uint8_t Pre_Base_NA : 1;
  uint8_t Pre_TimecounterFreq_NA : 1;
  uint8_t Post_Result : 2;
  uint8_t Post_ResObj : 2;
} CTimeReqTimespecGetres_Entry;

/**
 * @brief Test context for spec:/c/time/req/timespec-getres test case.
 */
typedef struct {
  /**
   * @brief This member contains the return value of the timespec_getres()
   *   call.
   */
  int result;

  /**
   * @brief This member specifies the ``ts`` parameter value.
   */
  struct timespec *res;

  /**
   * @brief This member provides the object referenced by the ``ts`` parameter.
   */
  struct timespec res_obj;

  /**
   * @brief This member specifies the ``base`` parameter value.
   */
  int base;

  /**
   * @brief This member provides the timecounter frequency to restore during
   *   test cleanup, or zero if the frequency was not changed.
   */
  uint64_t tc_freq;

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
    CTimeReqTimespecGetres_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CTimeReqTimespecGetres_Context;

static CTimeReqTimespecGetres_Context
  CTimeReqTimespecGetres_Instance;

static const char * const CTimeReqTimespecGetres_PreDesc_Res[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const CTimeReqTimespecGetres_PreDesc_Base[] = {
  "TimeUtc",
  "Invalid",
  "NA"
};

static const char * const CTimeReqTimespecGetres_PreDesc_TimecounterFreq[] = {
  "Low",
  "High",
  "NA"
};

static const char * const * const CTimeReqTimespecGetres_PreDesc[] = {
  CTimeReqTimespecGetres_PreDesc_Res,
  CTimeReqTimespecGetres_PreDesc_Base,
  CTimeReqTimespecGetres_PreDesc_TimecounterFreq,
  NULL
};

static void CTimeReqTimespecGetres_Pre_Res_Prepare(
  CTimeReqTimespecGetres_Context *ctx,
  CTimeReqTimespecGetres_Pre_Res  state
)
{
  switch ( state ) {
    case CTimeReqTimespecGetres_Pre_Res_Valid: {
      /*
       * While the ``ts`` parameter references an object of type timespec.
       */
      ctx->res = &ctx->res_obj;
      break;
    }

    case CTimeReqTimespecGetres_Pre_Res_Null: {
      /*
       * While the ``ts`` parameter is equal to NULL.
       */
      ctx->res = NULL;
      break;
    }

    case CTimeReqTimespecGetres_Pre_Res_NA:
      break;
  }
}

static void CTimeReqTimespecGetres_Pre_Base_Prepare(
  CTimeReqTimespecGetres_Context *ctx,
  CTimeReqTimespecGetres_Pre_Base state
)
{
  switch ( state ) {
    case CTimeReqTimespecGetres_Pre_Base_TimeUtc: {
      /*
       * While the ``base`` parameter is equal to TIME_UTC.
       */
      ctx->base = TIME_UTC;
      break;
    }

    case CTimeReqTimespecGetres_Pre_Base_Invalid: {
      /*
       * While the ``base`` parameter is not equal to TIME_UTC.
       */
      ctx->base = -1;
      break;
    }

    case CTimeReqTimespecGetres_Pre_Base_NA:
      break;
  }
}

static void CTimeReqTimespecGetres_Pre_TimecounterFreq_Prepare(
  CTimeReqTimespecGetres_Context            *ctx,
  CTimeReqTimespecGetres_Pre_TimecounterFreq state
)
{
  switch ( state ) {
    case CTimeReqTimespecGetres_Pre_TimecounterFreq_Low: {
      /*
       * While the timecounter frequency is low enough such that
       * `TOD_NANOSECONDS_PER_SECOND` divided by the frequency is greater than
       * zero.
       */
      /* Use default software timecounter frequency (1000000 Hz), which is
       * low enough so TOD_NANOSECONDS_PER_SECOND / freq > 0. */
      break;
    }

    case CTimeReqTimespecGetres_Pre_TimecounterFreq_High: {
      /*
       * While the timecounter frequency is high enough such that
       * `TOD_NANOSECONDS_PER_SECOND` divided by the frequency equals zero.
       */
      ctx->tc_freq = SetTimecounterFrequency( UINT64_C( 2000000000 ) );
      break;
    }

    case CTimeReqTimespecGetres_Pre_TimecounterFreq_NA:
      break;
  }
}

static void CTimeReqTimespecGetres_Post_Result_Check(
  CTimeReqTimespecGetres_Context    *ctx,
  CTimeReqTimespecGetres_Post_Result state
)
{
  switch ( state ) {
    case CTimeReqTimespecGetres_Post_Result_Base: {
      /*
       * The return value of timespec_getres() shall be equal to TIME_UTC.
       */
      T_eq_int( ctx->result, TIME_UTC );
      break;
    }

    case CTimeReqTimespecGetres_Post_Result_Zero: {
      /*
       * The return value of timespec_getres() shall be equal to zero.
       */
      T_eq_int( ctx->result, 0 );
      break;
    }

    case CTimeReqTimespecGetres_Post_Result_NA:
      break;
  }
}

static void CTimeReqTimespecGetres_Post_ResObj_Check(
  CTimeReqTimespecGetres_Context    *ctx,
  CTimeReqTimespecGetres_Post_ResObj state
)
{
  switch ( state ) {
    case CTimeReqTimespecGetres_Post_ResObj_Set: {
      /*
       * The object referenced by the ``ts`` parameter shall be set to the
       * resolution of the time base.
       */
      T_eq_long( ctx->res_obj.tv_sec, 0 );
      T_gt_long( ctx->res_obj.tv_nsec, 0 );
      break;
    }

    case CTimeReqTimespecGetres_Post_ResObj_Unchanged: {
      /*
       * The object referenced by the ``ts`` parameter shall not be changed by
       * the directive call.
       */
      T_eq_long( ctx->res_obj.tv_sec, -1 );
      T_eq_long( ctx->res_obj.tv_nsec, -1 );
      break;
    }

    case CTimeReqTimespecGetres_Post_ResObj_NA:
      break;
  }
}

static void CTimeReqTimespecGetres_Prepare(
  CTimeReqTimespecGetres_Context *ctx
)
{
  ctx->result = -1;
  ctx->res_obj.tv_sec = -1;
  ctx->res_obj.tv_nsec = -1;
  ctx->tc_freq = 0;
}

static void CTimeReqTimespecGetres_Action(
  CTimeReqTimespecGetres_Context *ctx
)
{
  ctx->result = timespec_getres( ctx->res, ctx->base );
}

static void CTimeReqTimespecGetres_Cleanup(
  CTimeReqTimespecGetres_Context *ctx
)
{
  if ( ctx->tc_freq != 0 ) {
    SetTimecounterFrequency( ctx->tc_freq );
    ctx->tc_freq = 0;
  }
}

static const CTimeReqTimespecGetres_Entry
CTimeReqTimespecGetres_Entries[] = {
  { 0, 0, 0, 1, CTimeReqTimespecGetres_Post_Result_Zero,
    CTimeReqTimespecGetres_Post_ResObj_Unchanged },
  { 0, 0, 0, 0, CTimeReqTimespecGetres_Post_Result_Base,
    CTimeReqTimespecGetres_Post_ResObj_Set },
  { 0, 0, 0, 1, CTimeReqTimespecGetres_Post_Result_Base,
    CTimeReqTimespecGetres_Post_ResObj_NA }
};

static const uint8_t
CTimeReqTimespecGetres_Map[] = {
  1, 1, 0, 0, 2, 2, 0, 0
};

static size_t CTimeReqTimespecGetres_Scope( void *arg, char *buf, size_t n )
{
  CTimeReqTimespecGetres_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CTimeReqTimespecGetres_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CTimeReqTimespecGetres_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CTimeReqTimespecGetres_Scope,
  .initial_context = &CTimeReqTimespecGetres_Instance
};

static inline CTimeReqTimespecGetres_Entry CTimeReqTimespecGetres_PopEntry(
  CTimeReqTimespecGetres_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CTimeReqTimespecGetres_Entries[
    CTimeReqTimespecGetres_Map[ index ]
  ];
}

static void CTimeReqTimespecGetres_SetPreConditionStates(
  CTimeReqTimespecGetres_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];

  if ( ctx->Map.entry.Pre_TimecounterFreq_NA ) {
    ctx->Map.pcs[ 2 ] = CTimeReqTimespecGetres_Pre_TimecounterFreq_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }
}

static void CTimeReqTimespecGetres_TestVariant(
  CTimeReqTimespecGetres_Context *ctx
)
{
  CTimeReqTimespecGetres_Pre_Res_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CTimeReqTimespecGetres_Pre_Base_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CTimeReqTimespecGetres_Pre_TimecounterFreq_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CTimeReqTimespecGetres_Action( ctx );
  CTimeReqTimespecGetres_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
  CTimeReqTimespecGetres_Post_ResObj_Check( ctx, ctx->Map.entry.Post_ResObj );
}

/**
 * @fn void T_case_body_CTimeReqTimespecGetres( void )
 */
T_TEST_CASE_FIXTURE( CTimeReqTimespecGetres, &CTimeReqTimespecGetres_Fixture )
{
  CTimeReqTimespecGetres_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CTimeReqTimespecGetres_Pre_Res_Valid;
    ctx->Map.pci[ 0 ] < CTimeReqTimespecGetres_Pre_Res_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CTimeReqTimespecGetres_Pre_Base_TimeUtc;
      ctx->Map.pci[ 1 ] < CTimeReqTimespecGetres_Pre_Base_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CTimeReqTimespecGetres_Pre_TimecounterFreq_Low;
        ctx->Map.pci[ 2 ] < CTimeReqTimespecGetres_Pre_TimecounterFreq_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = CTimeReqTimespecGetres_PopEntry( ctx );
        CTimeReqTimespecGetres_SetPreConditionStates( ctx );
        CTimeReqTimespecGetres_Prepare( ctx );
        CTimeReqTimespecGetres_TestVariant( ctx );
        CTimeReqTimespecGetres_Cleanup( ctx );
      }
    }
  }
}

/** @} */
