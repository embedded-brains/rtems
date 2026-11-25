/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsWorkspaceReqFree
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
#include <rtems/score/wkspace.h>

#include "tx-heap.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsWorkspaceReqFree spec:/rtems/workspace/req/free
 *
 * @ingroup TestsuitesValidationHeap0
 *
 * @{
 */

typedef enum {
  RtemsWorkspaceReqFree_Pre_Pointer_Null,
  RtemsWorkspaceReqFree_Pre_Pointer_Used,
  RtemsWorkspaceReqFree_Pre_Pointer_Invalid,
  RtemsWorkspaceReqFree_Pre_Pointer_NA
} RtemsWorkspaceReqFree_Pre_Pointer;

typedef enum {
  RtemsWorkspaceReqFree_Post_Status_True,
  RtemsWorkspaceReqFree_Post_Status_NA
} RtemsWorkspaceReqFree_Post_Status;

typedef enum {
  RtemsWorkspaceReqFree_Post_Heap_Freed,
  RtemsWorkspaceReqFree_Post_Heap_Nop,
  RtemsWorkspaceReqFree_Post_Heap_NA
} RtemsWorkspaceReqFree_Post_Heap;

typedef enum {
  RtemsWorkspaceReqFree_Post_AllocatorLock_Once,
  RtemsWorkspaceReqFree_Post_AllocatorLock_Nop,
  RtemsWorkspaceReqFree_Post_AllocatorLock_NA
} RtemsWorkspaceReqFree_Post_AllocatorLock;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Pointer_NA : 1;
  uint8_t Post_Status : 1;
  uint8_t Post_Heap : 2;
  uint8_t Post_AllocatorLock : 2;
} RtemsWorkspaceReqFree_Entry;

/**
 * @brief Test context for spec:/rtems/workspace/req/free test case.
 */
typedef struct {
  /**
   * @brief This member contains the heap of the test.
   */
  HeapContext heap;

  /**
   * @brief This member contains the snapshot of the heap before the call.
   */
  HeapSnapshot before;

  /**
   * @brief This member contains the snapshot of the heap after the call.
   */
  HeapSnapshot after;

  /**
   * @brief This member contains the count of the obtains of the object
   *   allocator mutex during the call.
   */
  unsigned int lock_count;

  /**
   * @brief This member contains the return value of the rtems_workspace_free()
   *   call.
   */
  bool status;

  /**
   * @brief This member specifies the `pointer` parameter value.
   */
  void *pointer;

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
    RtemsWorkspaceReqFree_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsWorkspaceReqFree_Context;

static RtemsWorkspaceReqFree_Context RtemsWorkspaceReqFree_Instance;

static const char *const RtemsWorkspaceReqFree_PreDesc_Pointer[] =
  { "Null", "Used", "Invalid", "NA" };

static const char *const *const RtemsWorkspaceReqFree_PreDesc[] = {
  RtemsWorkspaceReqFree_PreDesc_Pointer,
  NULL
};

static void RtemsWorkspaceReqFree_Pre_Pointer_Prepare(
  RtemsWorkspaceReqFree_Context    *ctx,
  RtemsWorkspaceReqFree_Pre_Pointer state
)
{
  switch ( state ) {
    case RtemsWorkspaceReqFree_Pre_Pointer_Null: {
      /*
       * While the `pointer` parameter is equal to NULL.
       */
      ctx->pointer = NULL;
      break;
    }

    case RtemsWorkspaceReqFree_Pre_Pointer_Used: {
      /*
       * While the `pointer` parameter is the begin address of a used block of
       * the RTEMS Workspace.
       */
      ctx->pointer = _RBHeap_Allocate( &_Workspace_RBHeap, 1 );
      T_quiet_not_null( ctx->pointer );
      break;
    }

    case RtemsWorkspaceReqFree_Pre_Pointer_Invalid: {
      /*
       * While the `pointer` parameter is not equal to NULL, while the
       * `pointer` parameter is not the begin address of a used block of the
       * RTEMS Workspace.
       */
      ctx->pointer = HeapGetArea();
      break;
    }

    case RtemsWorkspaceReqFree_Pre_Pointer_NA:
      break;
  }
}

static void RtemsWorkspaceReqFree_Post_Status_Check(
  RtemsWorkspaceReqFree_Context    *ctx,
  RtemsWorkspaceReqFree_Post_Status state
)
{
  switch ( state ) {
    case RtemsWorkspaceReqFree_Post_Status_True: {
      /*
       * The return value of rtems_workspace_free() shall be true.
       */
      T_true( ctx->status );
      break;
    }

    case RtemsWorkspaceReqFree_Post_Status_NA:
      break;
  }
}

static void RtemsWorkspaceReqFree_Post_Heap_Check(
  RtemsWorkspaceReqFree_Context  *ctx,
  RtemsWorkspaceReqFree_Post_Heap state
)
{
  switch ( state ) {
    case RtemsWorkspaceReqFree_Post_Heap_Freed: {
      /*
       * The RTEMS Workspace shall have one used block less.
       */
      T_eq_u32( ctx->after.used_blocks + 1, ctx->before.used_blocks );
      break;
    }

    case RtemsWorkspaceReqFree_Post_Heap_Nop: {
      /*
       * The used blocks and the free blocks of the RTEMS Workspace shall not
       * be changed by the rtems_workspace_free() call.
       */
      T_eq_u32( ctx->after.used_blocks, ctx->before.used_blocks );
      T_eq_u32( ctx->after.free_blocks, ctx->before.free_blocks );
      T_eq_uptr( ctx->after.free_size, ctx->before.free_size );
      break;
    }

    case RtemsWorkspaceReqFree_Post_Heap_NA:
      break;
  }
}

static void RtemsWorkspaceReqFree_Post_AllocatorLock_Check(
  RtemsWorkspaceReqFree_Context           *ctx,
  RtemsWorkspaceReqFree_Post_AllocatorLock state
)
{
  switch ( state ) {
    case RtemsWorkspaceReqFree_Post_AllocatorLock_Once: {
      /*
       * The rtems_workspace_free() directive shall obtain the object allocator
       * mutex exactly once.
       */
      T_eq_uint( ctx->lock_count, 1 );
      break;
    }

    case RtemsWorkspaceReqFree_Post_AllocatorLock_Nop: {
      /*
       * The rtems_workspace_free() directive shall not obtain the object
       * allocator mutex.
       */
      T_eq_uint( ctx->lock_count, 0 );
      break;
    }

    case RtemsWorkspaceReqFree_Post_AllocatorLock_NA:
      break;
  }
}

static void RtemsWorkspaceReqFree_Setup( RtemsWorkspaceReqFree_Context *ctx )
{
  WorkspaceInstall( &ctx->heap );
}

static void RtemsWorkspaceReqFree_Setup_Wrap( void *arg )
{
  RtemsWorkspaceReqFree_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsWorkspaceReqFree_Setup( ctx );
}

static void RtemsWorkspaceReqFree_Teardown(
  RtemsWorkspaceReqFree_Context *ctx
)
{
  WorkspaceRestore( &ctx->heap );
}

static void RtemsWorkspaceReqFree_Teardown_Wrap( void *arg )
{
  RtemsWorkspaceReqFree_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsWorkspaceReqFree_Teardown( ctx );
}

static void RtemsWorkspaceReqFree_Prepare( RtemsWorkspaceReqFree_Context *ctx )
{
  WorkspaceReset( &ctx->heap );
}

static void RtemsWorkspaceReqFree_Action( RtemsWorkspaceReqFree_Context *ctx )
{
  HeapTakeSnapshot( &_Workspace_RBHeap, &ctx->before );
  CallCounterObserve( &AllocatorLockCounter, rtems_task_self() );
  ctx->status = rtems_workspace_free( ctx->pointer );
  ctx->lock_count = CallCounterGet( &AllocatorLockCounter );
  CallCounterObserve( &AllocatorLockCounter, 0 );
  HeapTakeSnapshot( &_Workspace_RBHeap, &ctx->after );
}

/* clang-format off */

static const RtemsWorkspaceReqFree_Entry
RtemsWorkspaceReqFree_Entries[] = {
  { 0, 0, RtemsWorkspaceReqFree_Post_Status_True,
    RtemsWorkspaceReqFree_Post_Heap_Nop,
    RtemsWorkspaceReqFree_Post_AllocatorLock_Once },
  { 0, 0, RtemsWorkspaceReqFree_Post_Status_True,
    RtemsWorkspaceReqFree_Post_Heap_Freed,
    RtemsWorkspaceReqFree_Post_AllocatorLock_Once },
  { 1, 0, RtemsWorkspaceReqFree_Post_Status_NA,
    RtemsWorkspaceReqFree_Post_Heap_NA,
    RtemsWorkspaceReqFree_Post_AllocatorLock_NA }
};

static const uint8_t
RtemsWorkspaceReqFree_Map[] = {
  0, 1, 2
};

/* clang-format on */

static size_t RtemsWorkspaceReqFree_Scope( void *arg, char *buf, size_t n )
{
  RtemsWorkspaceReqFree_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsWorkspaceReqFree_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsWorkspaceReqFree_Fixture = {
  .setup = RtemsWorkspaceReqFree_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsWorkspaceReqFree_Teardown_Wrap,
  .scope = RtemsWorkspaceReqFree_Scope,
  .initial_context = &RtemsWorkspaceReqFree_Instance
};

static inline RtemsWorkspaceReqFree_Entry RtemsWorkspaceReqFree_PopEntry(
  RtemsWorkspaceReqFree_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsWorkspaceReqFree_Entries[ RtemsWorkspaceReqFree_Map[ index ] ];
}

static void RtemsWorkspaceReqFree_TestVariant(
  RtemsWorkspaceReqFree_Context *ctx
)
{
  RtemsWorkspaceReqFree_Pre_Pointer_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsWorkspaceReqFree_Action( ctx );
  RtemsWorkspaceReqFree_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsWorkspaceReqFree_Post_Heap_Check( ctx, ctx->Map.entry.Post_Heap );
  RtemsWorkspaceReqFree_Post_AllocatorLock_Check(
    ctx,
    ctx->Map.entry.Post_AllocatorLock
  );
}

/**
 * @fn void T_case_body_RtemsWorkspaceReqFree( void )
 */
T_TEST_CASE_FIXTURE( RtemsWorkspaceReqFree, &RtemsWorkspaceReqFree_Fixture )
{
  RtemsWorkspaceReqFree_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsWorkspaceReqFree_Pre_Pointer_Null;
    ctx->Map.pcs[ 0 ] < RtemsWorkspaceReqFree_Pre_Pointer_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = RtemsWorkspaceReqFree_PopEntry( ctx );

    if ( ctx->Map.entry.Skip ) {
      continue;
    }

    RtemsWorkspaceReqFree_Prepare( ctx );
    RtemsWorkspaceReqFree_TestVariant( ctx );
  }
}

/** @} */
