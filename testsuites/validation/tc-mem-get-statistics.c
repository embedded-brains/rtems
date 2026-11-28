/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsMallocReqGetStatistics
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <rtems/malloc.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/threaddispatch.h>

#include "tx-heap.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsMallocReqGetStatistics spec:/rtems/malloc/req/get-statistics
 *
 * @ingroup TestsuitesValidationHeap0
 *
 * @{
 */

typedef enum {
  RtemsMallocReqGetStatistics_Pre_Stats_Valid,
  RtemsMallocReqGetStatistics_Pre_Stats_Null,
  RtemsMallocReqGetStatistics_Pre_Stats_NA
} RtemsMallocReqGetStatistics_Pre_Stats;

typedef enum {
  RtemsMallocReqGetStatistics_Pre_State_DispatchEnabled,
  RtemsMallocReqGetStatistics_Pre_State_DispatchDisabled,
  RtemsMallocReqGetStatistics_Pre_State_BeforeMultitasking,
  RtemsMallocReqGetStatistics_Pre_State_Terminated,
  RtemsMallocReqGetStatistics_Pre_State_NA
} RtemsMallocReqGetStatistics_Pre_State;

typedef enum {
  RtemsMallocReqGetStatistics_Post_Status_Ok,
  RtemsMallocReqGetStatistics_Post_Status_Inval,
  RtemsMallocReqGetStatistics_Post_Status_Proto,
  RtemsMallocReqGetStatistics_Post_Status_NA
} RtemsMallocReqGetStatistics_Post_Status;

typedef enum {
  RtemsMallocReqGetStatistics_Post_StatsVar_Set,
  RtemsMallocReqGetStatistics_Post_StatsVar_Zero,
  RtemsMallocReqGetStatistics_Post_StatsVar_Nop,
  RtemsMallocReqGetStatistics_Post_StatsVar_NA
} RtemsMallocReqGetStatistics_Post_StatsVar;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Stats_NA : 1;
  uint8_t Pre_State_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_StatsVar : 2;
} RtemsMallocReqGetStatistics_Entry;

/**
 * @brief Test context for spec:/rtems/malloc/req/get-statistics test case.
 */
typedef struct {
  /**
   * @brief This member contains the heap of the test.
   */
  HeapContext heap;

  /**
   * @brief This member contains the return value of the
   *   malloc_get_statistics() call.
   */
  int status;

  /**
   * @brief This member provides the object referenced by the `stats`
   *   parameter.
   */
  rtems_heap_statistics stats;

  /**
   * @brief This member specifies the system state of the call.
   */
  System_state_Codes system_state;

  /**
   * @brief This member is true, if the call has thread dispatching disabled.
   */
  bool dispatch_disabled;

  /**
   * @brief This member specifies the `stats` parameter value.
   */
  rtems_heap_statistics *stats_ptr;

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
    RtemsMallocReqGetStatistics_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsMallocReqGetStatistics_Context;

static RtemsMallocReqGetStatistics_Context
  RtemsMallocReqGetStatistics_Instance;

static const char *const RtemsMallocReqGetStatistics_PreDesc_Stats[] =
  { "Valid", "Null", "NA" };

static const char *const RtemsMallocReqGetStatistics_PreDesc_State[] = {
  "DispatchEnabled",
  "DispatchDisabled",
  "BeforeMultitasking",
  "Terminated",
  "NA"
};

static const char *const *const RtemsMallocReqGetStatistics_PreDesc[] = {
  RtemsMallocReqGetStatistics_PreDesc_Stats,
  RtemsMallocReqGetStatistics_PreDesc_State,
  NULL
};

static void RtemsMallocReqGetStatistics_Pre_Stats_Prepare(
  RtemsMallocReqGetStatistics_Context  *ctx,
  RtemsMallocReqGetStatistics_Pre_Stats state
)
{
  switch ( state ) {
    case RtemsMallocReqGetStatistics_Pre_Stats_Valid: {
      /*
       * While the `stats` parameter references an object of type
       * rtems_heap_statistics.
       */
      ctx->stats_ptr = &ctx->stats;
      break;
    }

    case RtemsMallocReqGetStatistics_Pre_Stats_Null: {
      /*
       * While the `stats` parameter is equal to NULL.
       */
      ctx->stats_ptr = NULL;
      break;
    }

    case RtemsMallocReqGetStatistics_Pre_Stats_NA:
      break;
  }
}

static void RtemsMallocReqGetStatistics_Pre_State_Prepare(
  RtemsMallocReqGetStatistics_Context  *ctx,
  RtemsMallocReqGetStatistics_Pre_State state
)
{
  switch ( state ) {
    case RtemsMallocReqGetStatistics_Pre_State_DispatchEnabled: {
      /*
       * While the system is up, while thread dispatching is enabled.
       */
      ctx->system_state = SYSTEM_STATE_UP;
      ctx->dispatch_disabled = false;
      break;
    }

    case RtemsMallocReqGetStatistics_Pre_State_DispatchDisabled: {
      /*
       * While the system is up, while thread dispatching is disabled.
       */
      ctx->system_state = SYSTEM_STATE_UP;
      ctx->dispatch_disabled = true;
      break;
    }

    case RtemsMallocReqGetStatistics_Pre_State_BeforeMultitasking: {
      /*
       * While the system is before multitasking.
       */
      ctx->system_state = SYSTEM_STATE_BEFORE_MULTITASKING;
      ctx->dispatch_disabled = false;
      break;
    }

    case RtemsMallocReqGetStatistics_Pre_State_Terminated: {
      /*
       * While the system is terminated.
       */
      ctx->system_state = SYSTEM_STATE_TERMINATED;
      ctx->dispatch_disabled = false;
      break;
    }

    case RtemsMallocReqGetStatistics_Pre_State_NA:
      break;
  }
}

static void RtemsMallocReqGetStatistics_Post_Status_Check(
  RtemsMallocReqGetStatistics_Context    *ctx,
  RtemsMallocReqGetStatistics_Post_Status state
)
{
  switch ( state ) {
    case RtemsMallocReqGetStatistics_Post_Status_Ok: {
      /*
       * The return value of malloc_get_statistics() shall be equal to zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case RtemsMallocReqGetStatistics_Post_Status_Inval: {
      /*
       * The return value of malloc_get_statistics() shall be equal to EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case RtemsMallocReqGetStatistics_Post_Status_Proto: {
      /*
       * The return value of malloc_get_statistics() shall be equal to EPROTO.
       */
      T_eq_int( ctx->status, EPROTO );
      break;
    }

    case RtemsMallocReqGetStatistics_Post_Status_NA:
      break;
  }
}

static void RtemsMallocReqGetStatistics_Post_StatsVar_Check(
  RtemsMallocReqGetStatistics_Context      *ctx,
  RtemsMallocReqGetStatistics_Post_StatsVar state
)
{
  Heap_Statistics stats;

  switch ( state ) {
    case RtemsMallocReqGetStatistics_Post_StatsVar_Set: {
      /*
       * The object referenced by the `stats` parameter shall contain the
       * statistics of the C Program Heap.
       */
      _RBHeap_Get_statistics( &ctx->heap.heap, &stats );
      T_eq_u64( ctx->stats.lifetime_allocated, stats.lifetime_allocated );
      T_eq_u64( ctx->stats.lifetime_freed, stats.lifetime_freed );
      T_eq_sz( ctx->stats.size, stats.size );
      T_eq_sz( ctx->stats.free_size, stats.free_size );
      T_eq_sz(
        ctx->stats.largest_free_size,
        _RBHeap_Get_largest_free_size( &ctx->heap.heap )
      );
      T_eq_sz( ctx->stats.min_free_size, stats.min_free_size );
      T_eq_u32( ctx->stats.free_blocks, stats.free_blocks );
      T_eq_u32( ctx->stats.max_free_blocks, stats.max_free_blocks );
      T_eq_u32( ctx->stats.used_blocks, stats.used_blocks );
      T_eq_u32( ctx->stats.allocs, stats.allocs );
      T_eq_u32( ctx->stats.failed_allocs, stats.failed_allocs );
      T_eq_u32( ctx->stats.frees, stats.frees );
      T_eq_u32( ctx->stats.resizes, stats.resizes );
      break;
    }

    case RtemsMallocReqGetStatistics_Post_StatsVar_Zero: {
      /*
       * Each member of the object referenced by the `stats` parameter shall be
       * equal to zero.
       */
      rtems_heap_statistics zero;

      memset( &zero, 0, sizeof( zero ) );
      T_eq_mem( &ctx->stats, &zero, sizeof( zero ) );
      break;
    }

    case RtemsMallocReqGetStatistics_Post_StatsVar_Nop: {
      /*
       * Objects referenced by the `stats` parameter in past calls to
       * malloc_get_statistics() shall not be accessed by the
       * malloc_get_statistics() call.
       */
      T_eq_u32( ctx->stats.allocs, UINT32_MAX );
      break;
    }

    case RtemsMallocReqGetStatistics_Post_StatsVar_NA:
      break;
  }
}

static void RtemsMallocReqGetStatistics_Setup(
  RtemsMallocReqGetStatistics_Context *ctx
)
{
  HeapInstall( &ctx->heap );
}

static void RtemsMallocReqGetStatistics_Setup_Wrap( void *arg )
{
  RtemsMallocReqGetStatistics_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsMallocReqGetStatistics_Setup( ctx );
}

static void RtemsMallocReqGetStatistics_Teardown(
  RtemsMallocReqGetStatistics_Context *ctx
)
{
  HeapRestore( &ctx->heap );
}

static void RtemsMallocReqGetStatistics_Teardown_Wrap( void *arg )
{
  RtemsMallocReqGetStatistics_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsMallocReqGetStatistics_Teardown( ctx );
}

static void RtemsMallocReqGetStatistics_Prepare(
  RtemsMallocReqGetStatistics_Context *ctx
)
{
  void     *ptr;
  uintptr_t old_size;

  memset( &ctx->stats, 0xff, sizeof( ctx->stats ) );
  HeapReset( &ctx->heap );
  ptr = _RBHeap_Allocate( &ctx->heap.heap, 1 );
  T_quiet_not_null( ptr );
  T_quiet_eq_int(
    _RBHeap_Resize( &ctx->heap.heap, ptr, 2 * RBHEAP_MIN_SIZE, &old_size ),
    RBHEAP_RESIZE_SUCCESSFUL
  );
  T_quiet_null( _RBHeap_Allocate( &ctx->heap.heap, HEAP_AREA_SIZE ) );
  ptr = _RBHeap_Allocate( &ctx->heap.heap, 1 );
  T_quiet_true( _RBHeap_Free( &ctx->heap.heap, ptr ) );
}

static void RtemsMallocReqGetStatistics_Action(
  RtemsMallocReqGetStatistics_Context *ctx
)
{
  Per_CPU_Control *cpu_self;
  ISR_Level        level;

  cpu_self = NULL;

  if ( ctx->dispatch_disabled ) {
    cpu_self = _Thread_Dispatch_disable();
  }

  _ISR_Local_disable( level );
  _System_state_Set( ctx->system_state );
  ctx->status = malloc_get_statistics( ctx->stats_ptr );
  _System_state_Set( SYSTEM_STATE_UP );
  _ISR_Local_enable( level );

  if ( cpu_self != NULL ) {
    _Thread_Dispatch_enable( cpu_self );
  }
}

/* clang-format off */

static const RtemsMallocReqGetStatistics_Entry
RtemsMallocReqGetStatistics_Entries[] = {
  { 0, 0, 0, RtemsMallocReqGetStatistics_Post_Status_Inval,
    RtemsMallocReqGetStatistics_Post_StatsVar_Nop },
  { 0, 0, 0, RtemsMallocReqGetStatistics_Post_Status_Ok,
    RtemsMallocReqGetStatistics_Post_StatsVar_Set },
  { 0, 0, 0, RtemsMallocReqGetStatistics_Post_Status_Proto,
    RtemsMallocReqGetStatistics_Post_StatsVar_Zero }
};

static const uint8_t
RtemsMallocReqGetStatistics_Map[] = {
  1, 2, 1, 1, 0, 0, 0, 0
};

/* clang-format on */

static size_t RtemsMallocReqGetStatistics_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  RtemsMallocReqGetStatistics_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsMallocReqGetStatistics_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsMallocReqGetStatistics_Fixture = {
  .setup = RtemsMallocReqGetStatistics_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsMallocReqGetStatistics_Teardown_Wrap,
  .scope = RtemsMallocReqGetStatistics_Scope,
  .initial_context = &RtemsMallocReqGetStatistics_Instance
};

static inline RtemsMallocReqGetStatistics_Entry
RtemsMallocReqGetStatistics_PopEntry(
  RtemsMallocReqGetStatistics_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsMallocReqGetStatistics_Entries
    [ RtemsMallocReqGetStatistics_Map[ index ] ];
}

static void RtemsMallocReqGetStatistics_TestVariant(
  RtemsMallocReqGetStatistics_Context *ctx
)
{
  RtemsMallocReqGetStatistics_Pre_Stats_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsMallocReqGetStatistics_Pre_State_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsMallocReqGetStatistics_Action( ctx );
  RtemsMallocReqGetStatistics_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  RtemsMallocReqGetStatistics_Post_StatsVar_Check(
    ctx,
    ctx->Map.entry.Post_StatsVar
  );
}

/**
 * @fn void T_case_body_RtemsMallocReqGetStatistics( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsMallocReqGetStatistics,
  &RtemsMallocReqGetStatistics_Fixture
)
{
  RtemsMallocReqGetStatistics_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsMallocReqGetStatistics_Pre_Stats_Valid;
    ctx->Map.pcs[ 0 ] < RtemsMallocReqGetStatistics_Pre_Stats_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] =
        RtemsMallocReqGetStatistics_Pre_State_DispatchEnabled;
      ctx->Map.pcs[ 1 ] < RtemsMallocReqGetStatistics_Pre_State_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = RtemsMallocReqGetStatistics_PopEntry( ctx );
      RtemsMallocReqGetStatistics_Prepare( ctx );
      RtemsMallocReqGetStatistics_TestVariant( ctx );
    }
  }
}

/** @} */
