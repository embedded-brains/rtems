/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CTimeReqClockGetres
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
#include <limits.h>
#include <string.h>
#include <time.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CTimeReqClockGetres spec:/c/time/req/clock-getres
 *
 * @{
 */

typedef enum {
  CTimeReqClockGetres_Pre_ClockId_Realtime,
  CTimeReqClockGetres_Pre_ClockId_ProcessCPUTime,
  CTimeReqClockGetres_Pre_ClockId_ThreadCPUTime,
  CTimeReqClockGetres_Pre_ClockId_Invalid,
  CTimeReqClockGetres_Pre_ClockId_NA
} CTimeReqClockGetres_Pre_ClockId;

typedef enum {
  CTimeReqClockGetres_Pre_Res_Valid,
  CTimeReqClockGetres_Pre_Res_Null,
  CTimeReqClockGetres_Pre_Res_NA
} CTimeReqClockGetres_Pre_Res;

typedef enum {
  CTimeReqClockGetres_Pre_TimecounterFreq_Low,
  CTimeReqClockGetres_Pre_TimecounterFreq_High,
  CTimeReqClockGetres_Pre_TimecounterFreq_NA
} CTimeReqClockGetres_Pre_TimecounterFreq;

typedef enum {
  CTimeReqClockGetres_Post_Status_Zero,
  CTimeReqClockGetres_Post_Status_MinusOne,
  CTimeReqClockGetres_Post_Status_NA
} CTimeReqClockGetres_Post_Status;

typedef enum {
  CTimeReqClockGetres_Post_Errno_Unchanged,
  CTimeReqClockGetres_Post_Errno_EINVAL,
  CTimeReqClockGetres_Post_Errno_NA
} CTimeReqClockGetres_Post_Errno;

typedef enum {
  CTimeReqClockGetres_Post_ResObj_Set,
  CTimeReqClockGetres_Post_ResObj_Unchanged,
  CTimeReqClockGetres_Post_ResObj_NA
} CTimeReqClockGetres_Post_ResObj;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_ClockId_NA : 1;
  uint16_t Pre_Res_NA : 1;
  uint16_t Pre_TimecounterFreq_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Errno : 2;
  uint16_t Post_ResObj : 2;
} CTimeReqClockGetres_Entry;

#include <rtems/timecounter.h>

static uint32_t GetTimecountHigh( struct timecounter *tc )
{
  (void) tc;
  return 0;
}

/**
 * @brief Test context for spec:/c/time/req/clock-getres test case.
 */
typedef struct {
  /**
   * @brief This member contains the return value of the clock_getres() call.
   */
  int status;

  /**
   * @brief This member specifies the ``clock_id`` parameter value.
   */
  clockid_t clock_id;

  /**
   * @brief This member specifies the ``res`` parameter value.
   */
  struct timespec *res;

  /**
   * @brief This member provides the object referenced by the ``res``
   *   parameter.
   */
  struct timespec res_obj;

  /**
   * @brief This member provides the high-frequency timecounter used to reach
   *   the `tv_nsec == 0` branch.
   */
  struct {
    struct timecounter base;
  } tc;

  /**
   * @brief This member provides the restored timecounter after a
   *   high-frequency test.
   */
  struct {
    struct timecounter base;
  } original_tc;

  /**
   * @brief If this member is true, then a custom timecounter was installed.
   */
  bool tc_installed;

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
    CTimeReqClockGetres_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CTimeReqClockGetres_Context;

static CTimeReqClockGetres_Context
  CTimeReqClockGetres_Instance;

static const char * const CTimeReqClockGetres_PreDesc_ClockId[] = {
  "Realtime",
  "ProcessCPUTime",
  "ThreadCPUTime",
  "Invalid",
  "NA"
};

static const char * const CTimeReqClockGetres_PreDesc_Res[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const CTimeReqClockGetres_PreDesc_TimecounterFreq[] = {
  "Low",
  "High",
  "NA"
};

static const char * const * const CTimeReqClockGetres_PreDesc[] = {
  CTimeReqClockGetres_PreDesc_ClockId,
  CTimeReqClockGetres_PreDesc_Res,
  CTimeReqClockGetres_PreDesc_TimecounterFreq,
  NULL
};

static void CTimeReqClockGetres_Pre_ClockId_Prepare(
  CTimeReqClockGetres_Context    *ctx,
  CTimeReqClockGetres_Pre_ClockId state
)
{
  switch ( state ) {
    case CTimeReqClockGetres_Pre_ClockId_Realtime: {
      /*
       * While the ``clock_id`` parameter is equal to CLOCK_REALTIME.
       */
      ctx->clock_id = CLOCK_REALTIME;
      break;
    }

    case CTimeReqClockGetres_Pre_ClockId_ProcessCPUTime: {
      /*
       * While the ``clock_id`` parameter is equal to CLOCK_PROCESS_CPUTIME_ID.
       */
      ctx->clock_id = CLOCK_PROCESS_CPUTIME_ID;
      break;
    }

    case CTimeReqClockGetres_Pre_ClockId_ThreadCPUTime: {
      /*
       * While the ``clock_id`` parameter is equal to CLOCK_THREAD_CPUTIME_ID.
       */
      ctx->clock_id = CLOCK_THREAD_CPUTIME_ID;
      break;
    }

    case CTimeReqClockGetres_Pre_ClockId_Invalid: {
      /*
       * While the ``clock_id`` parameter is an invalid clock identifier.
       */
      ctx->clock_id = INT_MAX;
      break;
    }

    case CTimeReqClockGetres_Pre_ClockId_NA:
      break;
  }
}

static void CTimeReqClockGetres_Pre_Res_Prepare(
  CTimeReqClockGetres_Context *ctx,
  CTimeReqClockGetres_Pre_Res  state
)
{
  switch ( state ) {
    case CTimeReqClockGetres_Pre_Res_Valid: {
      /*
       * While the ``res`` parameter references an object of type timespec.
       */
      ctx->res = &ctx->res_obj;
      break;
    }

    case CTimeReqClockGetres_Pre_Res_Null: {
      /*
       * While the ``res`` parameter is equal to NULL.
       */
      ctx->res = NULL;
      break;
    }

    case CTimeReqClockGetres_Pre_Res_NA:
      break;
  }
}

static void CTimeReqClockGetres_Pre_TimecounterFreq_Prepare(
  CTimeReqClockGetres_Context            *ctx,
  CTimeReqClockGetres_Pre_TimecounterFreq state
)
{
  switch ( state ) {
    case CTimeReqClockGetres_Pre_TimecounterFreq_Low: {
      /*
       * While the timecounter frequency is low enough such that
       * `TOD_NANOSECONDS_PER_SECOND` divided by the frequency is greater than
       * zero.
       */
      /* Use default software timecounter frequency (1000000 Hz), which is
       * low enough so TOD_NANOSECONDS_PER_SECOND / freq > 0. */
      break;
    }

    case CTimeReqClockGetres_Pre_TimecounterFreq_High: {
      /*
       * While the timecounter frequency is high enough such that
       * `TOD_NANOSECONDS_PER_SECOND` divided by the frequency equals zero.
       */
      ctx->tc.base.tc_frequency = UINT64_C( 2000000000 );
      rtems_timecounter_install( &ctx->tc.base );
      break;
    }

    case CTimeReqClockGetres_Pre_TimecounterFreq_NA:
      break;
  }
}

static void CTimeReqClockGetres_Post_Status_Check(
  CTimeReqClockGetres_Context    *ctx,
  CTimeReqClockGetres_Post_Status state
)
{
  switch ( state ) {
    case CTimeReqClockGetres_Post_Status_Zero: {
      /*
       * The return value of clock_getres() shall be equal to zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CTimeReqClockGetres_Post_Status_MinusOne: {
      /*
       * The return value of clock_getres() shall be equal to minus one.
       */
      T_eq_int( ctx->status, -1 );
      break;
    }

    case CTimeReqClockGetres_Post_Status_NA:
      break;
  }
}

static void CTimeReqClockGetres_Post_Errno_Check(
  CTimeReqClockGetres_Post_Errno state
)
{
  switch ( state ) {
    case CTimeReqClockGetres_Post_Errno_Unchanged: {
      /*
       * The errno shall not be changed by the directive call.
       */
      T_eq_int( errno, -1 );
      break;
    }

    case CTimeReqClockGetres_Post_Errno_EINVAL: {
      /*
       * The errno shall be equal to EINVAL.
       */
      T_eq_int( errno, EINVAL );
      break;
    }

    case CTimeReqClockGetres_Post_Errno_NA:
      break;
  }
}

static void CTimeReqClockGetres_Post_ResObj_Check(
  CTimeReqClockGetres_Context    *ctx,
  CTimeReqClockGetres_Post_ResObj state
)
{
  switch ( state ) {
    case CTimeReqClockGetres_Post_ResObj_Set: {
      /*
       * The object referenced by the ``res`` parameter shall be set to the
       * resolution of the clock.
       */
      T_eq_long( ctx->res_obj.tv_sec, 0 );
      T_gt_long( ctx->res_obj.tv_nsec, 0 );
      break;
    }

    case CTimeReqClockGetres_Post_ResObj_Unchanged: {
      /*
       * The object referenced by the ``res`` parameter shall not be changed by
       * the directive call.
       */
      T_eq_long( ctx->res_obj.tv_sec, -1 );
      T_eq_long( ctx->res_obj.tv_nsec, -1 );
      break;
    }

    case CTimeReqClockGetres_Post_ResObj_NA:
      break;
  }
}

static void CTimeReqClockGetres_Prepare( CTimeReqClockGetres_Context *ctx )
{
  errno = -1;
  ctx->status = -1;
  ctx->res_obj.tv_sec = -1;
  ctx->res_obj.tv_nsec = -1;
  ctx->tc_installed = false;
  memset( &ctx->tc, 0, sizeof( ctx->tc ) );
  ctx->tc.base.tc_get_timecount = GetTimecountHigh;
  ctx->tc.base.tc_counter_mask = 0xffffffff;
  ctx->tc.base.tc_quality = RTEMS_TIMECOUNTER_QUALITY_CLOCK_DRIVER + 1;
}

static void CTimeReqClockGetres_Action( CTimeReqClockGetres_Context *ctx )
{
  ctx->status = clock_getres( ctx->clock_id, ctx->res );
}

static void CTimeReqClockGetres_Cleanup( CTimeReqClockGetres_Context *ctx )
{
  if ( ctx->tc_installed ) {
    ctx->original_tc.base.tc_frequency = SOFTWARE_TIMECOUNTER_FREQUENCY;
    rtems_timecounter_install( &ctx->original_tc.base );
    ctx->tc_installed = false;
  }
}

static const CTimeReqClockGetres_Entry
CTimeReqClockGetres_Entries[] = {
  { 0, 0, 0, 1, CTimeReqClockGetres_Post_Status_MinusOne,
    CTimeReqClockGetres_Post_Errno_EINVAL, CTimeReqClockGetres_Post_ResObj_NA },
  { 0, 0, 0, 0, CTimeReqClockGetres_Post_Status_Zero,
    CTimeReqClockGetres_Post_Errno_Unchanged,
    CTimeReqClockGetres_Post_ResObj_Set },
  { 0, 0, 0, 1, CTimeReqClockGetres_Post_Status_MinusOne,
    CTimeReqClockGetres_Post_Errno_EINVAL,
    CTimeReqClockGetres_Post_ResObj_Unchanged }
};

static const uint8_t
CTimeReqClockGetres_Map[] = {
  1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 2, 2, 0, 0
};

static size_t CTimeReqClockGetres_Scope( void *arg, char *buf, size_t n )
{
  CTimeReqClockGetres_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CTimeReqClockGetres_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CTimeReqClockGetres_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = CTimeReqClockGetres_Scope,
  .initial_context = &CTimeReqClockGetres_Instance
};

static inline CTimeReqClockGetres_Entry CTimeReqClockGetres_PopEntry(
  CTimeReqClockGetres_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CTimeReqClockGetres_Entries[
    CTimeReqClockGetres_Map[ index ]
  ];
}

static void CTimeReqClockGetres_SetPreConditionStates(
  CTimeReqClockGetres_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];

  if ( ctx->Map.entry.Pre_TimecounterFreq_NA ) {
    ctx->Map.pcs[ 2 ] = CTimeReqClockGetres_Pre_TimecounterFreq_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }
}

static void CTimeReqClockGetres_TestVariant( CTimeReqClockGetres_Context *ctx )
{
  CTimeReqClockGetres_Pre_ClockId_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CTimeReqClockGetres_Pre_Res_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CTimeReqClockGetres_Pre_TimecounterFreq_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CTimeReqClockGetres_Action( ctx );
  CTimeReqClockGetres_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CTimeReqClockGetres_Post_Errno_Check( ctx->Map.entry.Post_Errno );
  CTimeReqClockGetres_Post_ResObj_Check( ctx, ctx->Map.entry.Post_ResObj );
}

/**
 * @fn void T_case_body_CTimeReqClockGetres( void )
 */
T_TEST_CASE_FIXTURE( CTimeReqClockGetres, &CTimeReqClockGetres_Fixture )
{
  CTimeReqClockGetres_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CTimeReqClockGetres_Pre_ClockId_Realtime;
    ctx->Map.pci[ 0 ] < CTimeReqClockGetres_Pre_ClockId_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CTimeReqClockGetres_Pre_Res_Valid;
      ctx->Map.pci[ 1 ] < CTimeReqClockGetres_Pre_Res_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CTimeReqClockGetres_Pre_TimecounterFreq_Low;
        ctx->Map.pci[ 2 ] < CTimeReqClockGetres_Pre_TimecounterFreq_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = CTimeReqClockGetres_PopEntry( ctx );
        CTimeReqClockGetres_SetPreConditionStates( ctx );
        CTimeReqClockGetres_Prepare( ctx );
        CTimeReqClockGetres_TestVariant( ctx );
        CTimeReqClockGetres_Cleanup( ctx );
      }
    }
  }
}

/** @} */
