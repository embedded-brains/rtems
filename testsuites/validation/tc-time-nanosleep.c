/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CTimeReqNanosleep
 */

/*
 * Copyright (C) 2026 OHB System AG
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <rtems.h>
#include <stdint.h>
#include <time.h>
#include <rtems/test-scheduler.h>
#include <rtems/score/timecounter.h>
#include <rtems/score/watchdogimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CTimeReqNanosleep spec:/c/time/req/nanosleep
 *
 * @ingroup TestsuitesValidationNoClock1
 *
 * @{
 */

typedef enum {
  CTimeReqNanosleep_Pre_RQTp_Valid,
  CTimeReqNanosleep_Pre_RQTp_Null,
  CTimeReqNanosleep_Pre_RQTp_NA
} CTimeReqNanosleep_Pre_RQTp;

typedef enum {
  CTimeReqNanosleep_Pre_RQTpNSec_Zero,
  CTimeReqNanosleep_Pre_RQTpNSec_Positive,
  CTimeReqNanosleep_Pre_RQTpNSec_Invalid,
  CTimeReqNanosleep_Pre_RQTpNSec_NA
} CTimeReqNanosleep_Pre_RQTpNSec;

typedef enum {
  CTimeReqNanosleep_Pre_RQTpSec_Negative,
  CTimeReqNanosleep_Pre_RQTpSec_Zero,
  CTimeReqNanosleep_Pre_RQTpSec_Future,
  CTimeReqNanosleep_Pre_RQTpSec_FarFuture,
  CTimeReqNanosleep_Pre_RQTpSec_NA
} CTimeReqNanosleep_Pre_RQTpSec;

typedef enum {
  CTimeReqNanosleep_Pre_RMTp_Valid,
  CTimeReqNanosleep_Pre_RMTp_Null,
  CTimeReqNanosleep_Pre_RMTp_NA
} CTimeReqNanosleep_Pre_RMTp;

typedef enum {
  CTimeReqNanosleep_Post_Status_Zero,
  CTimeReqNanosleep_Post_Status_MinusOne,
  CTimeReqNanosleep_Post_Status_NA
} CTimeReqNanosleep_Post_Status;

typedef enum {
  CTimeReqNanosleep_Post_Errno_Nop,
  CTimeReqNanosleep_Post_Errno_Inval,
  CTimeReqNanosleep_Post_Errno_NA
} CTimeReqNanosleep_Post_Errno;

typedef enum {
  CTimeReqNanosleep_Post_Timer_Inactive,
  CTimeReqNanosleep_Post_Timer_Monotonic,
  CTimeReqNanosleep_Post_Timer_NA
} CTimeReqNanosleep_Post_Timer;

typedef enum {
  CTimeReqNanosleep_Post_Expire_Last,
  CTimeReqNanosleep_Post_Expire_Relative,
  CTimeReqNanosleep_Post_Expire_NA
} CTimeReqNanosleep_Post_Expire;

typedef enum {
  CTimeReqNanosleep_Post_Scheduler_Block,
  CTimeReqNanosleep_Post_Scheduler_BlockUnblock,
  CTimeReqNanosleep_Post_Scheduler_NA
} CTimeReqNanosleep_Post_Scheduler;

typedef enum {
  CTimeReqNanosleep_Post_RMTp_Zero,
  CTimeReqNanosleep_Post_RMTp_NA
} CTimeReqNanosleep_Post_RMTp;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_RQTp_NA : 1;
  uint16_t Pre_RQTpNSec_NA : 1;
  uint16_t Pre_RQTpSec_NA : 1;
  uint16_t Pre_RMTp_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Errno : 2;
  uint16_t Post_Timer : 2;
  uint16_t Post_Expire : 2;
  uint16_t Post_Scheduler : 2;
  uint16_t Post_RMTp : 1;
} CTimeReqNanosleep_Entry;

/**
 * @brief Test context for spec:/c/time/req/nanosleep test case.
 */
typedef struct {
  /**
   * @brief This member provides the scheduler operation records.
   */
  T_scheduler_log_4 scheduler_log;

  /**
   * @brief This member contains the CLOCK_MONOTONIC value before the
   *   nanosleep() call.
   */
  struct timespec now_monotonic;

  /**
   * @brief This member contains the time of day before the test.
   */
  rtems_time_of_day tod_saved;

  /**
   * @brief This member contains the worker task identifier.
   */
  rtems_id worker_id;

  /**
   * @brief This member contains the timer information of the worker task.
   */
  TaskTimerInfo timer_info;

  /**
   * @brief This member provides the object referenced by the `rqtp` parameter.
   */
  struct timespec rqtp_obj;

  /**
   * @brief If this member is true, then the worker task sets the `tv_sec`
   *   member of the object referenced by the `rqtp` parameter so that the
   *   interval ends at the implementation limit plus
   *   CTimeReqNanosleep_Context::rqtp_sec_beyond_limit seconds.
   */
  bool rqtp_sec_at_limit;

  /**
   * @brief This member specifies the seconds by which the interval ends past
   *   the implementation limit.
   */
  time_t rqtp_sec_beyond_limit;

  /**
   * @brief This member provides the object referenced by the `rmtp` parameter.
   */
  struct timespec rmtp_obj;

  /**
   * @brief This member contains the return value of the nanosleep() call.
   */
  int status;

  /**
   * @brief This member contains the errno value after the nanosleep() call.
   */
  int errno_value;

  /**
   * @brief This member specifies the `rqtp` parameter value.
   */
  const struct timespec *rqtp;

  /**
   * @brief This member specifies the `rmtp` parameter value.
   */
  struct timespec *rmtp;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 4 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 4 ];

    /**
     * @brief This member defines the representative index for the next action.
     */
    size_t rep;

    /**
     * @brief If this member is true, then the current run of the
     *   representatives should be skipped.
     */
    bool skip_rep;

    /**
     * @brief This member records the representatives which ran.
     */
    bool rep_ran[ 19 ];

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
    CTimeReqNanosleep_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CTimeReqNanosleep_Context;

static CTimeReqNanosleep_Context CTimeReqNanosleep_Instance;

static const char *const CTimeReqNanosleep_PreDesc_RQTp[] =
  { "Valid", "Null", "NA" };

static const char *const CTimeReqNanosleep_PreDesc_RQTpNSec[] = {
  "Zero",
  "PositiveOne",
  "PositiveMax",
  "InvalidMinusOne",
  "InvalidOneBillion",
  "NA"
};

static const char *const CTimeReqNanosleep_PreDesc_RQTpSec[] = {
  "Negative",
  "Zero",
  "FutureOne",
  "FutureLimit",
  "FarFutureLimit",
  "FarFutureMax",
  "NA"
};

static const char *const CTimeReqNanosleep_PreDesc_RMTp[] =
  { "Valid", "Null", "NA" };

static const char *const *const CTimeReqNanosleep_PreDesc[] = {
  CTimeReqNanosleep_PreDesc_RQTp,
  CTimeReqNanosleep_PreDesc_RQTpNSec,
  CTimeReqNanosleep_PreDesc_RQTpSec,
  CTimeReqNanosleep_PreDesc_RMTp,
  NULL
};

typedef enum {
  CTimeReqNanosleep_Pre_RQTpNSec_Positive_One,
  CTimeReqNanosleep_Pre_RQTpNSec_Positive_Max
} CTimeReqNanosleep_Pre_RQTpNSec_Positive_Rep;

typedef enum {
  CTimeReqNanosleep_Pre_RQTpNSec_Invalid_MinusOne,
  CTimeReqNanosleep_Pre_RQTpNSec_Invalid_OneBillion
} CTimeReqNanosleep_Pre_RQTpNSec_Invalid_Rep;

typedef enum {
  CTimeReqNanosleep_Pre_RQTpSec_Future_One,
  CTimeReqNanosleep_Pre_RQTpSec_Future_Limit
} CTimeReqNanosleep_Pre_RQTpSec_Future_Rep;

typedef enum {
  CTimeReqNanosleep_Pre_RQTpSec_FarFuture_Limit,
  CTimeReqNanosleep_Pre_RQTpSec_FarFuture_Max
} CTimeReqNanosleep_Pre_RQTpSec_FarFuture_Rep;

static const uint8_t CTimeReqNanosleep_PreRepOffset_RQTp[] = { 0, 1, 2, 3 };

static const uint8_t CTimeReqNanosleep_PreRepOffset_RQTpNSec[] =
  { 0, 1, 3, 5, 6 };

static const uint8_t CTimeReqNanosleep_PreRepOffset_RQTpSec[] =
  { 0, 1, 2, 4, 6, 7 };

static const uint8_t CTimeReqNanosleep_PreRepOffset_RMTp[] = { 0, 1, 2, 3 };

static const uint8_t *const CTimeReqNanosleep_PreRepOffset[] = {
  CTimeReqNanosleep_PreRepOffset_RQTp,
  CTimeReqNanosleep_PreRepOffset_RQTpNSec,
  CTimeReqNanosleep_PreRepOffset_RQTpSec,
  CTimeReqNanosleep_PreRepOffset_RMTp
};

static size_t CTimeReqNanosleep_GetRepresentativeCountOf(
  const CTimeReqNanosleep_Context *ctx,
  size_t                           index
)
{
  const uint8_t *offsets;
  size_t         state;

  offsets = CTimeReqNanosleep_PreRepOffset[ index ];
  state = ctx->Map.pcs[ index ];

  return offsets[ state + 1 ] - offsets[ state ];
}

static size_t CTimeReqNanosleep_GetRepresentative(
  const CTimeReqNanosleep_Context *ctx,
  size_t                           index
)
{
  size_t count;

  count = CTimeReqNanosleep_GetRepresentativeCountOf( ctx, index );

  if ( ctx->Map.rep < count ) {
    return ctx->Map.rep;
  }

  return count - 1;
}

static size_t CTimeReqNanosleep_GetRepresentativeCount(
  const CTimeReqNanosleep_Context *ctx
)
{
  size_t count;
  size_t i;

  count = 1;

  for ( i = 0; i < 4; ++i ) {
    size_t count_of_condition;

    count_of_condition = CTimeReqNanosleep_GetRepresentativeCountOf( ctx, i );

    if ( count_of_condition > count ) {
      count = count_of_condition;
    }
  }

  return count;
}

static CTimeReqNanosleep_Pre_RQTpNSec_Positive_Rep
CTimeReqNanosleep_Pre_RQTpNSec_Positive_GetRepresentative(
  const CTimeReqNanosleep_Context *ctx
)
{
  return (
    CTimeReqNanosleep_Pre_RQTpNSec_Positive_Rep
  ) CTimeReqNanosleep_GetRepresentative( ctx, 1 );
}

static CTimeReqNanosleep_Pre_RQTpNSec_Invalid_Rep
CTimeReqNanosleep_Pre_RQTpNSec_Invalid_GetRepresentative(
  const CTimeReqNanosleep_Context *ctx
)
{
  return (
    CTimeReqNanosleep_Pre_RQTpNSec_Invalid_Rep
  ) CTimeReqNanosleep_GetRepresentative( ctx, 1 );
}

static CTimeReqNanosleep_Pre_RQTpSec_Future_Rep
CTimeReqNanosleep_Pre_RQTpSec_Future_GetRepresentative(
  const CTimeReqNanosleep_Context *ctx
)
{
  return (
    CTimeReqNanosleep_Pre_RQTpSec_Future_Rep
  ) CTimeReqNanosleep_GetRepresentative( ctx, 2 );
}

static CTimeReqNanosleep_Pre_RQTpSec_FarFuture_Rep
CTimeReqNanosleep_Pre_RQTpSec_FarFuture_GetRepresentative(
  const CTimeReqNanosleep_Context *ctx
)
{
  return (
    CTimeReqNanosleep_Pre_RQTpSec_FarFuture_Rep
  ) CTimeReqNanosleep_GetRepresentative( ctx, 2 );
}

static const uint8_t CTimeReqNanosleep_PreRepBase[] = { 0, 3, 9, 16 };

static const uint8_t CTimeReqNanosleep_PreRepStates[] = { 2, 3, 4, 2 };

static void CTimeReqNanosleep_ResetRepresentatives(
  CTimeReqNanosleep_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 19; ++i ) {
    ctx->Map.rep_ran[ i ] = false;
  }
}

static void CTimeReqNanosleep_MarkRepresentatives(
  CTimeReqNanosleep_Context *ctx
)
{
  size_t i;

  if ( ctx->Map.skip_rep ) {
    return;
  }

  for ( i = 0; i < 4; ++i ) {
    size_t rep;

    rep = CTimeReqNanosleep_PreRepBase[ i ] +
          CTimeReqNanosleep_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
          CTimeReqNanosleep_GetRepresentative( ctx, i );
    ctx->Map.rep_ran[ rep ] = true;
  }
}

static void CTimeReqNanosleep_CheckRepresentatives(
  const CTimeReqNanosleep_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 4; ++i ) {
    const uint8_t *offsets;
    const bool    *ran;
    size_t         state;

    offsets = CTimeReqNanosleep_PreRepOffset[ i ];
    ran = &ctx->Map.rep_ran[ CTimeReqNanosleep_PreRepBase[ i ] ];

    for ( state = 0; state < CTimeReqNanosleep_PreRepStates[ i ]; ++state ) {
      size_t k;
      bool   any;

      any = false;

      for ( k = offsets[ state ]; k < offsets[ state + 1 ]; ++k ) {
        any = any || ran[ k ];
      }

      if ( any ) {
        for ( k = offsets[ state ]; k < offsets[ state + 1 ]; ++k ) {
          T_quiet_true(
            ran[ k ],
            "representative %s did not run",
            CTimeReqNanosleep_PreDesc[ i ][ k ]
          );
        }
      }
    }
  }
}

typedef CTimeReqNanosleep_Context Context;

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    T_scheduler_log *log;
    uint32_t         counter;

    SuspendSelf();

    log = T_scheduler_record_4( &ctx->scheduler_log );
    T_null( log );

    counter = GetTimecountCounter();
    _Timecounter_Nanouptime( &ctx->now_monotonic );
    SetTimecountCounter( counter );

    if ( ctx->rqtp_sec_at_limit ) {
      time_t carry;

      if ( ctx->now_monotonic.tv_nsec + ctx->rqtp_obj.tv_nsec >= 1000000000 ) {
        carry = 1;
      } else {
        carry = 0;
      }

      ctx->rqtp_obj.tv_sec = WATCHDOG_MAX_SECONDS - ctx->now_monotonic.tv_sec -
                             carry + ctx->rqtp_sec_beyond_limit;
    }

    errno = -1;
    ctx->status = nanosleep( ctx->rqtp, ctx->rmtp );
    ctx->errno_value = errno;

    (void) T_scheduler_record( NULL );
  }
}

static void CTimeReqNanosleep_Pre_RQTp_Prepare(
  CTimeReqNanosleep_Context *ctx,
  CTimeReqNanosleep_Pre_RQTp state
)
{
  switch ( state ) {
    case CTimeReqNanosleep_Pre_RQTp_Valid: {
      /*
       * While the `rqtp` parameter references an object of type struct
       * timespec.
       */
      ctx->rqtp = &ctx->rqtp_obj;
      break;
    }

    case CTimeReqNanosleep_Pre_RQTp_Null: {
      /*
       * While the `rqtp` parameter is equal to NULL.
       */
      ctx->rqtp = NULL;
      break;
    }

    case CTimeReqNanosleep_Pre_RQTp_NA:
      break;
  }
}

static void CTimeReqNanosleep_Pre_RQTpNSec_Prepare(
  CTimeReqNanosleep_Context     *ctx,
  CTimeReqNanosleep_Pre_RQTpNSec state
)
{
  switch ( state ) {
    case CTimeReqNanosleep_Pre_RQTpNSec_Zero: {
      /*
       * While the `tv_nsec` member of the object referenced by the `rqtp`
       * parameter is equal to zero.
       */
      ctx->rqtp_obj.tv_nsec = 0;
      break;
    }

    case CTimeReqNanosleep_Pre_RQTpNSec_Positive: {
      /*
       * While the `tv_nsec` member of the object referenced by the `rqtp`
       * parameter is greater than zero and less than one billion.
       */
      switch (
        CTimeReqNanosleep_Pre_RQTpNSec_Positive_GetRepresentative( ctx )
      ) {
        case CTimeReqNanosleep_Pre_RQTpNSec_Positive_One: {
          ctx->rqtp_obj.tv_nsec = 1;
          break;
        }

        case CTimeReqNanosleep_Pre_RQTpNSec_Positive_Max: {
          ctx->rqtp_obj.tv_nsec = 999999999;
          break;
        }
      }
      break;
    }

    case CTimeReqNanosleep_Pre_RQTpNSec_Invalid: {
      /*
       * While the `tv_nsec` member of the object referenced by the `rqtp`
       * parameter is less than zero or greater than or equal to one billion.
       */
      switch (
        CTimeReqNanosleep_Pre_RQTpNSec_Invalid_GetRepresentative( ctx )
      ) {
        case CTimeReqNanosleep_Pre_RQTpNSec_Invalid_MinusOne: {
          ctx->rqtp_obj.tv_nsec = -1;
          break;
        }

        case CTimeReqNanosleep_Pre_RQTpNSec_Invalid_OneBillion: {
          ctx->rqtp_obj.tv_nsec = 1000000000;
          break;
        }
      }
      break;
    }

    case CTimeReqNanosleep_Pre_RQTpNSec_NA:
      break;
  }
}

static void CTimeReqNanosleep_Pre_RQTpSec_Prepare(
  CTimeReqNanosleep_Context    *ctx,
  CTimeReqNanosleep_Pre_RQTpSec state
)
{
  switch ( state ) {
    case CTimeReqNanosleep_Pre_RQTpSec_Negative: {
      /*
       * While the `tv_sec` member of the object referenced by the `rqtp`
       * parameter is negative.
       */
      ctx->rqtp_obj.tv_sec = -1;
      break;
    }

    case CTimeReqNanosleep_Pre_RQTpSec_Zero: {
      /*
       * While the `tv_sec` member of the object referenced by the `rqtp`
       * parameter is equal to zero.
       */
      ctx->rqtp_obj.tv_sec = 0;
      break;
    }

    case CTimeReqNanosleep_Pre_RQTpSec_Future: {
      /*
       * While the `tv_sec` member of the object referenced by the `rqtp`
       * parameter is positive and specifies an interval which ends at a time
       * point which is within the implementation limits.
       */
      switch (
        CTimeReqNanosleep_Pre_RQTpSec_Future_GetRepresentative( ctx )
      ) {
        case CTimeReqNanosleep_Pre_RQTpSec_Future_One: {
          ctx->rqtp_obj.tv_sec = 1;
          break;
        }

        case CTimeReqNanosleep_Pre_RQTpSec_Future_Limit: {
          ctx->rqtp_sec_at_limit = true;
          ctx->rqtp_sec_beyond_limit = 0;
          break;
        }
      }
      break;
    }

    case CTimeReqNanosleep_Pre_RQTpSec_FarFuture: {
      /*
       * While the `tv_sec` member of the object referenced by the `rqtp`
       * parameter specifies an interval which ends at a time point which is
       * past the implementation limit.
       */
      switch (
        CTimeReqNanosleep_Pre_RQTpSec_FarFuture_GetRepresentative( ctx )
      ) {
        case CTimeReqNanosleep_Pre_RQTpSec_FarFuture_Limit: {
          ctx->rqtp_sec_at_limit = true;
          ctx->rqtp_sec_beyond_limit = 1;
          break;
        }

        case CTimeReqNanosleep_Pre_RQTpSec_FarFuture_Max: {
          ctx->rqtp_obj.tv_sec = INT64_MAX;
          break;
        }
      }
      break;
    }

    case CTimeReqNanosleep_Pre_RQTpSec_NA:
      break;
  }
}

static void CTimeReqNanosleep_Pre_RMTp_Prepare(
  CTimeReqNanosleep_Context *ctx,
  CTimeReqNanosleep_Pre_RMTp state
)
{
  switch ( state ) {
    case CTimeReqNanosleep_Pre_RMTp_Valid: {
      /*
       * While the `rmtp` parameter references an object of type struct
       * timespec.
       */
      ctx->rmtp = &ctx->rmtp_obj;
      break;
    }

    case CTimeReqNanosleep_Pre_RMTp_Null: {
      /*
       * While the `rmtp` parameter is equal to NULL.
       */
      ctx->rmtp = NULL;
      break;
    }

    case CTimeReqNanosleep_Pre_RMTp_NA:
      break;
  }
}

static void CTimeReqNanosleep_Post_Status_Check(
  CTimeReqNanosleep_Context    *ctx,
  CTimeReqNanosleep_Post_Status state
)
{
  switch ( state ) {
    case CTimeReqNanosleep_Post_Status_Zero: {
      /*
       * The return value of nanosleep() shall be equal to zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CTimeReqNanosleep_Post_Status_MinusOne: {
      /*
       * The return value of nanosleep() shall be equal to minus one.
       */
      T_eq_int( ctx->status, -1 );
      break;
    }

    case CTimeReqNanosleep_Post_Status_NA:
      break;
  }
}

static void CTimeReqNanosleep_Post_Errno_Check(
  CTimeReqNanosleep_Context   *ctx,
  CTimeReqNanosleep_Post_Errno state
)
{
  switch ( state ) {
    case CTimeReqNanosleep_Post_Errno_Nop: {
      /*
       * The errno value shall not be changed by the nanosleep() call.
       */
      T_eq_int( ctx->errno_value, -1 );
      break;
    }

    case CTimeReqNanosleep_Post_Errno_Inval: {
      /*
       * The errno value shall be equal to EINVAL.
       */
      T_eq_int( ctx->errno_value, EINVAL );
      break;
    }

    case CTimeReqNanosleep_Post_Errno_NA:
      break;
  }
}

static void CTimeReqNanosleep_Post_Timer_Check(
  CTimeReqNanosleep_Context   *ctx,
  CTimeReqNanosleep_Post_Timer state
)
{
  switch ( state ) {
    case CTimeReqNanosleep_Post_Timer_Inactive: {
      /*
       * The timer of the calling task shall be inactive.
       */
      T_eq_int( ctx->timer_info.state, TASK_TIMER_INACTIVE );
      break;
    }

    case CTimeReqNanosleep_Post_Timer_Monotonic: {
      /*
       * The timer of the calling task shall be active using the
       * CLOCK_MONOTONIC.
       */
      T_eq_int( ctx->timer_info.state, TASK_TIMER_MONOTONIC );
      break;
    }

    case CTimeReqNanosleep_Post_Timer_NA:
      break;
  }
}

static void CTimeReqNanosleep_Post_Expire_Check(
  CTimeReqNanosleep_Context    *ctx,
  CTimeReqNanosleep_Post_Expire state
)
{
  struct timespec expire;

  switch ( state ) {
    case CTimeReqNanosleep_Post_Expire_Last: {
      /*
       * The timer of the calling task shall expire at the last valid time
       * point of the CLOCK_MONOTONIC.
       */
      T_eq_u64( ctx->timer_info.expire_ticks, 0xffffffffffffffff );
      break;
    }

    case CTimeReqNanosleep_Post_Expire_Relative: {
      /*
       * The timer of the calling task shall expire at the time point specified
       * by the sum of the current time of the CLOCK_MONOTONIC and the interval
       * specified by the `rqtp` parameter.
       */
      expire = ctx->now_monotonic;
      expire.tv_sec += ctx->rqtp_obj.tv_sec;
      expire.tv_nsec += ctx->rqtp_obj.tv_nsec;

      if ( expire.tv_nsec >= 1000000000 ) {
        ++expire.tv_sec;
        expire.tv_nsec -= 1000000000;
      }

      T_eq_i64( ctx->timer_info.expire_timespec.tv_sec, expire.tv_sec );
      T_eq_long( ctx->timer_info.expire_timespec.tv_nsec, expire.tv_nsec );
      break;
    }

    case CTimeReqNanosleep_Post_Expire_NA:
      break;
  }
}

static void CTimeReqNanosleep_Post_Scheduler_Check(
  CTimeReqNanosleep_Context       *ctx,
  CTimeReqNanosleep_Post_Scheduler state
)
{
  switch ( state ) {
    case CTimeReqNanosleep_Post_Scheduler_Block: {
      /*
       * The calling task shall be blocked by the scheduler exactly once by the
       * nanosleep() call.
       */
      T_eq_sz( ctx->scheduler_log.header.recorded, 1 );
      T_eq_int( ctx->scheduler_log.events[ 0 ].operation, T_SCHEDULER_BLOCK );
      break;
    }

    case CTimeReqNanosleep_Post_Scheduler_BlockUnblock: {
      /*
       * The calling task shall be blocked exactly once by the scheduler and
       * then unblocked in the same thread dispatch critical section by the
       * nanosleep() call.
       */
      T_eq_sz( ctx->scheduler_log.header.recorded, 2 );
      T_eq_int( ctx->scheduler_log.events[ 0 ].operation, T_SCHEDULER_BLOCK );
      T_eq_int(
        ctx->scheduler_log.events[ 1 ].operation,
        T_SCHEDULER_UNBLOCK
      );
      break;
    }

    case CTimeReqNanosleep_Post_Scheduler_NA:
      break;
  }
}

static void CTimeReqNanosleep_Post_RMTp_Check(
  CTimeReqNanosleep_Context  *ctx,
  CTimeReqNanosleep_Post_RMTp state
)
{
  switch ( state ) {
    case CTimeReqNanosleep_Post_RMTp_Zero: {
      /*
       * The object referenced by the `rmtp` parameter shall be cleared to zero
       * after the return of the nanosleep() call.
       */
      T_eq_i64( ctx->rmtp_obj.tv_sec, 0 );
      T_eq_long( ctx->rmtp_obj.tv_nsec, 0 );
      break;
    }

    case CTimeReqNanosleep_Post_RMTp_NA:
      break;
  }
}

static void CTimeReqNanosleep_Setup( CTimeReqNanosleep_Context *ctx )
{
  rtems_time_of_day now = { 1988, 1, 1, 0, 0, 0, 0 };
  T_rsc_success( rtems_clock_get_tod( &ctx->tod_saved ) );
  T_rsc_success( rtems_clock_set( &now ) );
  SetSelfPriority( PRIO_NORMAL );
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( ctx->worker_id, Worker, ctx );
}

static void CTimeReqNanosleep_Setup_Wrap( void *arg )
{
  CTimeReqNanosleep_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CTimeReqNanosleep_Setup( ctx );
}

static void CTimeReqNanosleep_Teardown( CTimeReqNanosleep_Context *ctx )
{
  DeleteTask( ctx->worker_id );
  RestoreRunnerPriority();
  T_rsc_success( rtems_clock_set( &ctx->tod_saved ) );
}

static void CTimeReqNanosleep_Teardown_Wrap( void *arg )
{
  CTimeReqNanosleep_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CTimeReqNanosleep_Teardown( ctx );
}

static void CTimeReqNanosleep_Prepare( CTimeReqNanosleep_Context *ctx )
{
  ctx->status = -1;
  ctx->errno_value = 0;
  ctx->rqtp_obj.tv_sec = -1;
  ctx->rqtp_obj.tv_nsec = -1;
  ctx->rmtp_obj.tv_sec = -1;
  ctx->rmtp_obj.tv_nsec = -1;
  ctx->rqtp_sec_at_limit = false;
}

static void CTimeReqNanosleep_Action( CTimeReqNanosleep_Context *ctx )
{
  ResumeTask( ctx->worker_id );
  (void) T_scheduler_record( NULL );
  GetTaskTimerInfo( ctx->worker_id, &ctx->timer_info );
  ClockTick();
  FinalClockTick();
}

/* clang-format off */

static const CTimeReqNanosleep_Entry
CTimeReqNanosleep_Entries[] = {
  { 1, 0, 0, 0, 0, CTimeReqNanosleep_Post_Status_NA,
    CTimeReqNanosleep_Post_Errno_NA, CTimeReqNanosleep_Post_Timer_NA,
    CTimeReqNanosleep_Post_Expire_NA, CTimeReqNanosleep_Post_Scheduler_NA,
    CTimeReqNanosleep_Post_RMTp_NA },
  { 0, 0, 0, 0, 0, CTimeReqNanosleep_Post_Status_MinusOne,
    CTimeReqNanosleep_Post_Errno_Inval, CTimeReqNanosleep_Post_Timer_Inactive,
    CTimeReqNanosleep_Post_Expire_NA,
    CTimeReqNanosleep_Post_Scheduler_BlockUnblock,
    CTimeReqNanosleep_Post_RMTp_Zero },
  { 0, 0, 0, 0, 0, CTimeReqNanosleep_Post_Status_MinusOne,
    CTimeReqNanosleep_Post_Errno_Inval, CTimeReqNanosleep_Post_Timer_Inactive,
    CTimeReqNanosleep_Post_Expire_NA,
    CTimeReqNanosleep_Post_Scheduler_BlockUnblock,
    CTimeReqNanosleep_Post_RMTp_NA },
  { 0, 0, 0, 0, 0, CTimeReqNanosleep_Post_Status_Zero,
    CTimeReqNanosleep_Post_Errno_Nop, CTimeReqNanosleep_Post_Timer_Monotonic,
    CTimeReqNanosleep_Post_Expire_Relative,
    CTimeReqNanosleep_Post_Scheduler_Block, CTimeReqNanosleep_Post_RMTp_Zero },
  { 0, 0, 0, 0, 0, CTimeReqNanosleep_Post_Status_Zero,
    CTimeReqNanosleep_Post_Errno_Nop, CTimeReqNanosleep_Post_Timer_Monotonic,
    CTimeReqNanosleep_Post_Expire_Relative,
    CTimeReqNanosleep_Post_Scheduler_Block, CTimeReqNanosleep_Post_RMTp_NA },
  { 0, 0, 0, 0, 0, CTimeReqNanosleep_Post_Status_Zero,
    CTimeReqNanosleep_Post_Errno_Nop, CTimeReqNanosleep_Post_Timer_Monotonic,
    CTimeReqNanosleep_Post_Expire_Last, CTimeReqNanosleep_Post_Scheduler_Block,
    CTimeReqNanosleep_Post_RMTp_Zero },
  { 0, 0, 0, 0, 0, CTimeReqNanosleep_Post_Status_Zero,
    CTimeReqNanosleep_Post_Errno_Nop, CTimeReqNanosleep_Post_Timer_Monotonic,
    CTimeReqNanosleep_Post_Expire_Last, CTimeReqNanosleep_Post_Scheduler_Block,
    CTimeReqNanosleep_Post_RMTp_NA },
  { 0, 0, 0, 0, 0, CTimeReqNanosleep_Post_Status_Zero,
    CTimeReqNanosleep_Post_Errno_Nop, CTimeReqNanosleep_Post_Timer_Inactive,
    CTimeReqNanosleep_Post_Expire_NA,
    CTimeReqNanosleep_Post_Scheduler_BlockUnblock,
    CTimeReqNanosleep_Post_RMTp_Zero },
  { 0, 0, 0, 0, 0, CTimeReqNanosleep_Post_Status_Zero,
    CTimeReqNanosleep_Post_Errno_Nop, CTimeReqNanosleep_Post_Timer_Inactive,
    CTimeReqNanosleep_Post_Expire_NA,
    CTimeReqNanosleep_Post_Scheduler_BlockUnblock,
    CTimeReqNanosleep_Post_RMTp_NA },
  { 0, 0, 1, 1, 0, CTimeReqNanosleep_Post_Status_MinusOne,
    CTimeReqNanosleep_Post_Errno_Inval, CTimeReqNanosleep_Post_Timer_Inactive,
    CTimeReqNanosleep_Post_Expire_NA,
    CTimeReqNanosleep_Post_Scheduler_BlockUnblock,
    CTimeReqNanosleep_Post_RMTp_Zero },
  { 0, 0, 1, 1, 0, CTimeReqNanosleep_Post_Status_MinusOne,
    CTimeReqNanosleep_Post_Errno_Inval, CTimeReqNanosleep_Post_Timer_Inactive,
    CTimeReqNanosleep_Post_Expire_NA,
    CTimeReqNanosleep_Post_Scheduler_BlockUnblock,
    CTimeReqNanosleep_Post_RMTp_NA }
};

static const uint8_t
CTimeReqNanosleep_Map[] = {
  1, 2, 7, 8, 3, 4, 5, 6, 1, 2, 3, 4, 3, 4, 5, 6, 1, 2, 1, 2, 1, 2, 1, 2, 9,
  10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t CTimeReqNanosleep_Scope( void *arg, char *buf, size_t n )
{
  CTimeReqNanosleep_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    size_t pcd[ 4 ];
    size_t i;

    for ( i = 0; i < 4; ++i ) {
      pcd[ i ] = CTimeReqNanosleep_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
                 CTimeReqNanosleep_GetRepresentative( ctx, i );
    }

    return T_get_scope( CTimeReqNanosleep_PreDesc, buf, n, pcd );
  }

  return 0;
}

static T_fixture CTimeReqNanosleep_Fixture = {
  .setup = CTimeReqNanosleep_Setup_Wrap,
  .stop = NULL,
  .teardown = CTimeReqNanosleep_Teardown_Wrap,
  .scope = CTimeReqNanosleep_Scope,
  .initial_context = &CTimeReqNanosleep_Instance
};

static inline CTimeReqNanosleep_Entry CTimeReqNanosleep_PopEntry(
  CTimeReqNanosleep_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CTimeReqNanosleep_Entries[ CTimeReqNanosleep_Map[ index ] ];
}

static void CTimeReqNanosleep_SetPreConditionStates(
  CTimeReqNanosleep_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_RQTpNSec_NA ) {
    ctx->Map.pcs[ 1 ] = CTimeReqNanosleep_Pre_RQTpNSec_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_RQTpSec_NA ) {
    ctx->Map.pcs[ 2 ] = CTimeReqNanosleep_Pre_RQTpSec_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
}

static void CTimeReqNanosleep_TestVariant( CTimeReqNanosleep_Context *ctx )
{
  CTimeReqNanosleep_Pre_RQTp_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CTimeReqNanosleep_Pre_RQTpNSec_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CTimeReqNanosleep_Pre_RQTpSec_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CTimeReqNanosleep_Pre_RMTp_Prepare( ctx, ctx->Map.pcs[ 3 ] );

  if ( ctx->Map.skip_rep ) {
    return;
  }

  CTimeReqNanosleep_Action( ctx );
  CTimeReqNanosleep_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CTimeReqNanosleep_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
  CTimeReqNanosleep_Post_Timer_Check( ctx, ctx->Map.entry.Post_Timer );
  CTimeReqNanosleep_Post_Expire_Check( ctx, ctx->Map.entry.Post_Expire );
  CTimeReqNanosleep_Post_Scheduler_Check( ctx, ctx->Map.entry.Post_Scheduler );
  CTimeReqNanosleep_Post_RMTp_Check( ctx, ctx->Map.entry.Post_RMTp );
}

/**
 * @fn void T_case_body_CTimeReqNanosleep( void )
 */
T_TEST_CASE_FIXTURE( CTimeReqNanosleep, &CTimeReqNanosleep_Fixture )
{
  CTimeReqNanosleep_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  CTimeReqNanosleep_ResetRepresentatives( ctx );

  for (
    ctx->Map.pci[ 0 ] = CTimeReqNanosleep_Pre_RQTp_Valid;
    ctx->Map.pci[ 0 ] < CTimeReqNanosleep_Pre_RQTp_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CTimeReqNanosleep_Pre_RQTpNSec_Zero;
      ctx->Map.pci[ 1 ] < CTimeReqNanosleep_Pre_RQTpNSec_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CTimeReqNanosleep_Pre_RQTpSec_Negative;
        ctx->Map.pci[ 2 ] < CTimeReqNanosleep_Pre_RQTpSec_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CTimeReqNanosleep_Pre_RMTp_Valid;
          ctx->Map.pci[ 3 ] < CTimeReqNanosleep_Pre_RMTp_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = CTimeReqNanosleep_PopEntry( ctx );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          CTimeReqNanosleep_SetPreConditionStates( ctx );

          for (
            ctx->Map.rep = 0;
            ctx->Map.rep < CTimeReqNanosleep_GetRepresentativeCount( ctx );
            ++ctx->Map.rep
          ) {
            ctx->Map.skip_rep = false;
            CTimeReqNanosleep_Prepare( ctx );
            CTimeReqNanosleep_TestVariant( ctx );

            CTimeReqNanosleep_MarkRepresentatives( ctx );
          }
        }
      }
    }
  }

  CTimeReqNanosleep_CheckRepresentatives( ctx );
}

/** @} */
