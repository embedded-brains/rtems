/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsWorkspaceReqGetStatistics
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

#include <rtems.h>
#include <stdint.h>
#include <string.h>
#include <rtems/score/wkspace.h>

#include "tx-heap.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsWorkspaceReqGetStatistics \
 *   spec:/rtems/workspace/req/get-statistics
 *
 * @ingroup TestsuitesValidationHeap0
 *
 * @{
 */

typedef enum {
  RtemsWorkspaceReqGetStatistics_Pre_Stats_Valid,
  RtemsWorkspaceReqGetStatistics_Pre_Stats_Null,
  RtemsWorkspaceReqGetStatistics_Pre_Stats_NA
} RtemsWorkspaceReqGetStatistics_Pre_Stats;

typedef enum {
  RtemsWorkspaceReqGetStatistics_Post_Status_Ok,
  RtemsWorkspaceReqGetStatistics_Post_Status_Error,
  RtemsWorkspaceReqGetStatistics_Post_Status_NA
} RtemsWorkspaceReqGetStatistics_Post_Status;

typedef enum {
  RtemsWorkspaceReqGetStatistics_Post_StatsVar_Set,
  RtemsWorkspaceReqGetStatistics_Post_StatsVar_Nop,
  RtemsWorkspaceReqGetStatistics_Post_StatsVar_NA
} RtemsWorkspaceReqGetStatistics_Post_StatsVar;

typedef enum {
  RtemsWorkspaceReqGetStatistics_Post_AllocatorLock_Once,
  RtemsWorkspaceReqGetStatistics_Post_AllocatorLock_Nop,
  RtemsWorkspaceReqGetStatistics_Post_AllocatorLock_NA
} RtemsWorkspaceReqGetStatistics_Post_AllocatorLock;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Stats_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_StatsVar : 2;
  uint8_t Post_AllocatorLock : 2;
} RtemsWorkspaceReqGetStatistics_Entry;

/**
 * @brief Test context for spec:/rtems/workspace/req/get-statistics test case.
 */
typedef struct {
  /**
   * @brief This member contains the heap of the test.
   */
  HeapContext heap;

  /**
   * @brief This member contains the count of the obtains of the object
   *   allocator mutex during the call.
   */
  unsigned int lock_count;

  /**
   * @brief This member contains the return value of the
   *   rtems_workspace_get_statistics() call.
   */
  bool status;

  /**
   * @brief This member provides the object referenced by the `stats`
   *   parameter.
   */
  rtems_heap_statistics stats;

  /**
   * @brief This member specifies the `stats` parameter value.
   */
  rtems_heap_statistics *stats_ptr;

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
    RtemsWorkspaceReqGetStatistics_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsWorkspaceReqGetStatistics_Context;

static RtemsWorkspaceReqGetStatistics_Context
  RtemsWorkspaceReqGetStatistics_Instance;

static const char *const RtemsWorkspaceReqGetStatistics_PreDesc_Stats[] =
  { "Valid", "Null", "NA" };

static const char *const *const RtemsWorkspaceReqGetStatistics_PreDesc[] = {
  RtemsWorkspaceReqGetStatistics_PreDesc_Stats,
  NULL
};

static void RtemsWorkspaceReqGetStatistics_Pre_Stats_Prepare(
  RtemsWorkspaceReqGetStatistics_Context  *ctx,
  RtemsWorkspaceReqGetStatistics_Pre_Stats state
)
{
  switch ( state ) {
    case RtemsWorkspaceReqGetStatistics_Pre_Stats_Valid: {
      /*
       * While the `stats` parameter references an object of type
       * rtems_heap_statistics.
       */
      ctx->stats_ptr = &ctx->stats;
      break;
    }

    case RtemsWorkspaceReqGetStatistics_Pre_Stats_Null: {
      /*
       * While the `stats` parameter is equal to NULL.
       */
      ctx->stats_ptr = NULL;
      break;
    }

    case RtemsWorkspaceReqGetStatistics_Pre_Stats_NA:
      break;
  }
}

static void RtemsWorkspaceReqGetStatistics_Post_Status_Check(
  RtemsWorkspaceReqGetStatistics_Context    *ctx,
  RtemsWorkspaceReqGetStatistics_Post_Status state
)
{
  switch ( state ) {
    case RtemsWorkspaceReqGetStatistics_Post_Status_Ok: {
      /*
       * The return value of rtems_workspace_get_statistics() shall be true.
       */
      T_true( ctx->status );
      break;
    }

    case RtemsWorkspaceReqGetStatistics_Post_Status_Error: {
      /*
       * The return value of rtems_workspace_get_statistics() shall be false.
       */
      T_false( ctx->status );
      break;
    }

    case RtemsWorkspaceReqGetStatistics_Post_Status_NA:
      break;
  }
}

static void RtemsWorkspaceReqGetStatistics_Post_StatsVar_Check(
  RtemsWorkspaceReqGetStatistics_Context      *ctx,
  RtemsWorkspaceReqGetStatistics_Post_StatsVar state
)
{
  Heap_Statistics stats;

  switch ( state ) {
    case RtemsWorkspaceReqGetStatistics_Post_StatsVar_Set: {
      /*
       * The object referenced by the `stats` parameter shall contain the
       * statistics of the RTEMS Workspace with a
       * rtems_heap_statistics::resizes member of zero.
       */
      _RBHeap_Get_statistics( &_Workspace_RBHeap, &stats );
      T_eq_u64( ctx->stats.lifetime_allocated, stats.lifetime_allocated );
      T_eq_u64( ctx->stats.lifetime_freed, stats.lifetime_freed );
      T_eq_sz( ctx->stats.size, stats.size );
      T_eq_sz( ctx->stats.free_size, stats.free_size );
      T_eq_sz(
        ctx->stats.largest_free_size,
        _RBHeap_Get_largest_free_size( &_Workspace_RBHeap )
      );
      T_eq_sz( ctx->stats.min_free_size, stats.min_free_size );
      T_eq_u32( ctx->stats.free_blocks, stats.free_blocks );
      T_eq_u32( ctx->stats.max_free_blocks, stats.max_free_blocks );
      T_eq_u32( ctx->stats.used_blocks, stats.used_blocks );
      T_eq_u32( ctx->stats.allocs, stats.allocs );
      T_eq_u32( ctx->stats.failed_allocs, stats.failed_allocs );
      T_eq_u32( ctx->stats.frees, stats.frees );
      T_eq_u32( ctx->stats.resizes, 0 );
      break;
    }

    case RtemsWorkspaceReqGetStatistics_Post_StatsVar_Nop: {
      /*
       * Objects referenced by the `stats` parameter in past calls to
       * rtems_workspace_get_statistics() shall not be accessed by the
       * rtems_workspace_get_statistics() call.
       */
      T_eq_u32( ctx->stats.allocs, UINT32_MAX );
      break;
    }

    case RtemsWorkspaceReqGetStatistics_Post_StatsVar_NA:
      break;
  }
}

static void RtemsWorkspaceReqGetStatistics_Post_AllocatorLock_Check(
  RtemsWorkspaceReqGetStatistics_Context           *ctx,
  RtemsWorkspaceReqGetStatistics_Post_AllocatorLock state
)
{
  switch ( state ) {
    case RtemsWorkspaceReqGetStatistics_Post_AllocatorLock_Once: {
      /*
       * The rtems_workspace_get_statistics() directive shall obtain the object
       * allocator mutex exactly once.
       */
      T_eq_uint( ctx->lock_count, 1 );
      break;
    }

    case RtemsWorkspaceReqGetStatistics_Post_AllocatorLock_Nop: {
      /*
       * The rtems_workspace_get_statistics() directive shall not obtain the
       * object allocator mutex.
       */
      T_eq_uint( ctx->lock_count, 0 );
      break;
    }

    case RtemsWorkspaceReqGetStatistics_Post_AllocatorLock_NA:
      break;
  }
}

static void RtemsWorkspaceReqGetStatistics_Setup(
  RtemsWorkspaceReqGetStatistics_Context *ctx
)
{
  WorkspaceInstall( &ctx->heap );
}

static void RtemsWorkspaceReqGetStatistics_Setup_Wrap( void *arg )
{
  RtemsWorkspaceReqGetStatistics_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsWorkspaceReqGetStatistics_Setup( ctx );
}

static void RtemsWorkspaceReqGetStatistics_Teardown(
  RtemsWorkspaceReqGetStatistics_Context *ctx
)
{
  WorkspaceRestore( &ctx->heap );
}

static void RtemsWorkspaceReqGetStatistics_Teardown_Wrap( void *arg )
{
  RtemsWorkspaceReqGetStatistics_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsWorkspaceReqGetStatistics_Teardown( ctx );
}

static void RtemsWorkspaceReqGetStatistics_Prepare(
  RtemsWorkspaceReqGetStatistics_Context *ctx
)
{
  void     *ptr;
  uintptr_t old_size;

  memset( &ctx->stats, 0xff, sizeof( ctx->stats ) );
  WorkspaceReset( &ctx->heap );
  ptr = _RBHeap_Allocate( &_Workspace_RBHeap, 1 );
  T_quiet_not_null( ptr );
  T_quiet_eq_int(
    _RBHeap_Resize( &_Workspace_RBHeap, ptr, 2 * RBHEAP_MIN_SIZE, &old_size ),
    RBHEAP_RESIZE_SUCCESSFUL
  );
  T_quiet_null( _RBHeap_Allocate( &_Workspace_RBHeap, HEAP_AREA_SIZE ) );
  ptr = _RBHeap_Allocate( &_Workspace_RBHeap, 1 );
  T_quiet_true( _RBHeap_Free( &_Workspace_RBHeap, ptr ) );
}

static void RtemsWorkspaceReqGetStatistics_Action(
  RtemsWorkspaceReqGetStatistics_Context *ctx
)
{
  CallCounterObserve( &AllocatorLockCounter, rtems_task_self() );
  ctx->status = rtems_workspace_get_statistics( ctx->stats_ptr );
  ctx->lock_count = CallCounterGet( &AllocatorLockCounter );
  CallCounterObserve( &AllocatorLockCounter, 0 );
}

/* clang-format off */

static const RtemsWorkspaceReqGetStatistics_Entry
RtemsWorkspaceReqGetStatistics_Entries[] = {
  { 0, 0, RtemsWorkspaceReqGetStatistics_Post_Status_Ok,
    RtemsWorkspaceReqGetStatistics_Post_StatsVar_Set,
    RtemsWorkspaceReqGetStatistics_Post_AllocatorLock_Once },
  { 0, 0, RtemsWorkspaceReqGetStatistics_Post_Status_Error,
    RtemsWorkspaceReqGetStatistics_Post_StatsVar_Nop,
    RtemsWorkspaceReqGetStatistics_Post_AllocatorLock_Nop }
};

static const uint8_t
RtemsWorkspaceReqGetStatistics_Map[] = {
  0, 1
};

/* clang-format on */

static size_t RtemsWorkspaceReqGetStatistics_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  RtemsWorkspaceReqGetStatistics_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsWorkspaceReqGetStatistics_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsWorkspaceReqGetStatistics_Fixture = {
  .setup = RtemsWorkspaceReqGetStatistics_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsWorkspaceReqGetStatistics_Teardown_Wrap,
  .scope = RtemsWorkspaceReqGetStatistics_Scope,
  .initial_context = &RtemsWorkspaceReqGetStatistics_Instance
};

static inline RtemsWorkspaceReqGetStatistics_Entry
RtemsWorkspaceReqGetStatistics_PopEntry(
  RtemsWorkspaceReqGetStatistics_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsWorkspaceReqGetStatistics_Entries
    [ RtemsWorkspaceReqGetStatistics_Map[ index ] ];
}

static void RtemsWorkspaceReqGetStatistics_TestVariant(
  RtemsWorkspaceReqGetStatistics_Context *ctx
)
{
  RtemsWorkspaceReqGetStatistics_Pre_Stats_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsWorkspaceReqGetStatistics_Action( ctx );
  RtemsWorkspaceReqGetStatistics_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  RtemsWorkspaceReqGetStatistics_Post_StatsVar_Check(
    ctx,
    ctx->Map.entry.Post_StatsVar
  );
  RtemsWorkspaceReqGetStatistics_Post_AllocatorLock_Check(
    ctx,
    ctx->Map.entry.Post_AllocatorLock
  );
}

/**
 * @fn void T_case_body_RtemsWorkspaceReqGetStatistics( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsWorkspaceReqGetStatistics,
  &RtemsWorkspaceReqGetStatistics_Fixture
)
{
  RtemsWorkspaceReqGetStatistics_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsWorkspaceReqGetStatistics_Pre_Stats_Valid;
    ctx->Map.pcs[ 0 ] < RtemsWorkspaceReqGetStatistics_Pre_Stats_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = RtemsWorkspaceReqGetStatistics_PopEntry( ctx );
    RtemsWorkspaceReqGetStatistics_Prepare( ctx );
    RtemsWorkspaceReqGetStatistics_TestVariant( ctx );
  }
}

/** @} */
