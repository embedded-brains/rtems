/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsWorkspaceReqAllocate
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
 * @defgroup RtemsWorkspaceReqAllocate spec:/rtems/workspace/req/allocate
 *
 * @ingroup TestsuitesValidationHeap0
 *
 * @{
 */

typedef enum {
  RtemsWorkspaceReqAllocate_Pre_Pointer_Valid,
  RtemsWorkspaceReqAllocate_Pre_Pointer_Null,
  RtemsWorkspaceReqAllocate_Pre_Pointer_NA
} RtemsWorkspaceReqAllocate_Pre_Pointer;

typedef enum {
  RtemsWorkspaceReqAllocate_Pre_Bytes_Zero,
  RtemsWorkspaceReqAllocate_Pre_Bytes_NonZero,
  RtemsWorkspaceReqAllocate_Pre_Bytes_NA
} RtemsWorkspaceReqAllocate_Pre_Bytes;

typedef enum {
  RtemsWorkspaceReqAllocate_Pre_Fit_Yes,
  RtemsWorkspaceReqAllocate_Pre_Fit_No,
  RtemsWorkspaceReqAllocate_Pre_Fit_NA
} RtemsWorkspaceReqAllocate_Pre_Fit;

typedef enum {
  RtemsWorkspaceReqAllocate_Post_Status_True,
  RtemsWorkspaceReqAllocate_Post_Status_False,
  RtemsWorkspaceReqAllocate_Post_Status_NA
} RtemsWorkspaceReqAllocate_Post_Status;

typedef enum {
  RtemsWorkspaceReqAllocate_Post_PointerVar_Set,
  RtemsWorkspaceReqAllocate_Post_PointerVar_Nop,
  RtemsWorkspaceReqAllocate_Post_PointerVar_NA
} RtemsWorkspaceReqAllocate_Post_PointerVar;

typedef enum {
  RtemsWorkspaceReqAllocate_Post_Heap_Allocated,
  RtemsWorkspaceReqAllocate_Post_Heap_Nop,
  RtemsWorkspaceReqAllocate_Post_Heap_NA
} RtemsWorkspaceReqAllocate_Post_Heap;

typedef enum {
  RtemsWorkspaceReqAllocate_Post_AllocatorLock_Once,
  RtemsWorkspaceReqAllocate_Post_AllocatorLock_Nop,
  RtemsWorkspaceReqAllocate_Post_AllocatorLock_NA
} RtemsWorkspaceReqAllocate_Post_AllocatorLock;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Pointer_NA : 1;
  uint16_t Pre_Bytes_NA : 1;
  uint16_t Pre_Fit_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_PointerVar : 2;
  uint16_t Post_Heap : 2;
  uint16_t Post_AllocatorLock : 2;
} RtemsWorkspaceReqAllocate_Entry;

/**
 * @brief Test context for spec:/rtems/workspace/req/allocate test case.
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
   * @brief This member contains the return value of the
   *   rtems_workspace_allocate() call.
   */
  bool status;

  /**
   * @brief This member provides the object referenced by the `pointer`
   *   parameter.
   */
  void *ptr;

  /**
   * @brief This member specifies the `bytes` parameter value.
   */
  size_t bytes;

  /**
   * @brief This member specifies the `pointer` parameter value.
   */
  void **pointer;

  struct {
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
    RtemsWorkspaceReqAllocate_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsWorkspaceReqAllocate_Context;

static RtemsWorkspaceReqAllocate_Context RtemsWorkspaceReqAllocate_Instance;

static const char *const RtemsWorkspaceReqAllocate_PreDesc_Pointer[] =
  { "Valid", "Null", "NA" };

static const char *const RtemsWorkspaceReqAllocate_PreDesc_Bytes[] =
  { "Zero", "NonZero", "NA" };

static const char *const RtemsWorkspaceReqAllocate_PreDesc_Fit[] =
  { "Yes", "No", "NA" };

static const char *const *const RtemsWorkspaceReqAllocate_PreDesc[] = {
  RtemsWorkspaceReqAllocate_PreDesc_Pointer,
  RtemsWorkspaceReqAllocate_PreDesc_Bytes,
  RtemsWorkspaceReqAllocate_PreDesc_Fit,
  NULL
};

static void RtemsWorkspaceReqAllocate_Pre_Pointer_Prepare(
  RtemsWorkspaceReqAllocate_Context    *ctx,
  RtemsWorkspaceReqAllocate_Pre_Pointer state
)
{
  switch ( state ) {
    case RtemsWorkspaceReqAllocate_Pre_Pointer_Valid: {
      /*
       * While the `pointer` parameter references an object of type `void *`.
       */
      ctx->pointer = &ctx->ptr;
      break;
    }

    case RtemsWorkspaceReqAllocate_Pre_Pointer_Null: {
      /*
       * While the `pointer` parameter is equal to NULL.
       */
      ctx->pointer = NULL;
      break;
    }

    case RtemsWorkspaceReqAllocate_Pre_Pointer_NA:
      break;
  }
}

static void RtemsWorkspaceReqAllocate_Pre_Bytes_Prepare(
  RtemsWorkspaceReqAllocate_Context  *ctx,
  RtemsWorkspaceReqAllocate_Pre_Bytes state
)
{
  switch ( state ) {
    case RtemsWorkspaceReqAllocate_Pre_Bytes_Zero: {
      /*
       * While the `bytes` parameter is equal to zero.
       */
      ctx->bytes = 0;
      break;
    }

    case RtemsWorkspaceReqAllocate_Pre_Bytes_NonZero: {
      /*
       * While the `bytes` parameter is not equal to zero.
       */
      ctx->bytes = 1;
      break;
    }

    case RtemsWorkspaceReqAllocate_Pre_Bytes_NA:
      break;
  }
}

static void RtemsWorkspaceReqAllocate_Pre_Fit_Prepare(
  RtemsWorkspaceReqAllocate_Pre_Fit state
)
{
  switch ( state ) {
    case RtemsWorkspaceReqAllocate_Pre_Fit_Yes: {
      /*
       * While the RTEMS Workspace has a free block which satisfies the
       * request.
       */
      /* The heap is empty */
      break;
    }

    case RtemsWorkspaceReqAllocate_Pre_Fit_No: {
      /*
       * While the RTEMS Workspace has no free block which satisfies the
       * request.
       */
      HeapExhaust( &_Workspace_RBHeap );
      break;
    }

    case RtemsWorkspaceReqAllocate_Pre_Fit_NA:
      break;
  }
}

static void RtemsWorkspaceReqAllocate_Post_Status_Check(
  RtemsWorkspaceReqAllocate_Context    *ctx,
  RtemsWorkspaceReqAllocate_Post_Status state
)
{
  switch ( state ) {
    case RtemsWorkspaceReqAllocate_Post_Status_True: {
      /*
       * The return value of rtems_workspace_allocate() shall be true.
       */
      T_true( ctx->status );
      break;
    }

    case RtemsWorkspaceReqAllocate_Post_Status_False: {
      /*
       * The return value of rtems_workspace_allocate() shall be false.
       */
      T_false( ctx->status );
      break;
    }

    case RtemsWorkspaceReqAllocate_Post_Status_NA:
      break;
  }
}

static void RtemsWorkspaceReqAllocate_Post_PointerVar_Check(
  RtemsWorkspaceReqAllocate_Context        *ctx,
  RtemsWorkspaceReqAllocate_Post_PointerVar state
)
{
  uintptr_t size;

  switch ( state ) {
    case RtemsWorkspaceReqAllocate_Post_PointerVar_Set: {
      /*
       * The value of the object referenced by the `pointer` parameter shall be
       * the begin address of a used block of the RTEMS Workspace of at least
       * the size specified by the `bytes` parameter.
       */
      T_true( _RBHeap_Size_of_block( &_Workspace_RBHeap, ctx->ptr, &size ) );
      T_ge_uptr( size, ctx->bytes );
      break;
    }

    case RtemsWorkspaceReqAllocate_Post_PointerVar_Nop: {
      /*
       * Objects referenced by the `pointer` parameter in past calls to
       * rtems_workspace_allocate() shall not be accessed by the
       * rtems_workspace_allocate() call.
       */
      T_eq_ptr( ctx->ptr, &ctx->ptr );
      break;
    }

    case RtemsWorkspaceReqAllocate_Post_PointerVar_NA:
      break;
  }
}

static void RtemsWorkspaceReqAllocate_Post_Heap_Check(
  RtemsWorkspaceReqAllocate_Context  *ctx,
  RtemsWorkspaceReqAllocate_Post_Heap state
)
{
  switch ( state ) {
    case RtemsWorkspaceReqAllocate_Post_Heap_Allocated: {
      /*
       * The RTEMS Workspace shall have one more used block.
       */
      T_eq_u32( ctx->after.used_blocks, ctx->before.used_blocks + 1 );
      break;
    }

    case RtemsWorkspaceReqAllocate_Post_Heap_Nop: {
      /*
       * The used blocks and the free blocks of the RTEMS Workspace shall not
       * be changed by the rtems_workspace_allocate() call.
       */
      T_eq_u32( ctx->after.used_blocks, ctx->before.used_blocks );
      T_eq_u32( ctx->after.free_blocks, ctx->before.free_blocks );
      T_eq_uptr( ctx->after.free_size, ctx->before.free_size );
      break;
    }

    case RtemsWorkspaceReqAllocate_Post_Heap_NA:
      break;
  }
}

static void RtemsWorkspaceReqAllocate_Post_AllocatorLock_Check(
  RtemsWorkspaceReqAllocate_Context           *ctx,
  RtemsWorkspaceReqAllocate_Post_AllocatorLock state
)
{
  switch ( state ) {
    case RtemsWorkspaceReqAllocate_Post_AllocatorLock_Once: {
      /*
       * The rtems_workspace_allocate() directive shall obtain the object
       * allocator mutex exactly once.
       */
      T_eq_uint( ctx->lock_count, 1 );
      break;
    }

    case RtemsWorkspaceReqAllocate_Post_AllocatorLock_Nop: {
      /*
       * The rtems_workspace_allocate() directive shall not obtain the object
       * allocator mutex.
       */
      T_eq_uint( ctx->lock_count, 0 );
      break;
    }

    case RtemsWorkspaceReqAllocate_Post_AllocatorLock_NA:
      break;
  }
}

static void RtemsWorkspaceReqAllocate_Setup(
  RtemsWorkspaceReqAllocate_Context *ctx
)
{
  WorkspaceInstall( &ctx->heap );
}

static void RtemsWorkspaceReqAllocate_Setup_Wrap( void *arg )
{
  RtemsWorkspaceReqAllocate_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsWorkspaceReqAllocate_Setup( ctx );
}

static void RtemsWorkspaceReqAllocate_Teardown(
  RtemsWorkspaceReqAllocate_Context *ctx
)
{
  WorkspaceRestore( &ctx->heap );
}

static void RtemsWorkspaceReqAllocate_Teardown_Wrap( void *arg )
{
  RtemsWorkspaceReqAllocate_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsWorkspaceReqAllocate_Teardown( ctx );
}

static void RtemsWorkspaceReqAllocate_Prepare(
  RtemsWorkspaceReqAllocate_Context *ctx
)
{
  ctx->ptr = &ctx->ptr;
  WorkspaceReset( &ctx->heap );
}

static void RtemsWorkspaceReqAllocate_Action(
  RtemsWorkspaceReqAllocate_Context *ctx
)
{
  HeapTakeSnapshot( &_Workspace_RBHeap, &ctx->before );
  CallCounterObserve( &AllocatorLockCounter, rtems_task_self() );
  ctx->status = rtems_workspace_allocate( ctx->bytes, ctx->pointer );
  ctx->lock_count = CallCounterGet( &AllocatorLockCounter );
  CallCounterObserve( &AllocatorLockCounter, 0 );
  HeapTakeSnapshot( &_Workspace_RBHeap, &ctx->after );
}

/* clang-format off */

static const RtemsWorkspaceReqAllocate_Entry
RtemsWorkspaceReqAllocate_Entries[] = {
  { 0, 0, 0, 0, RtemsWorkspaceReqAllocate_Post_Status_False,
    RtemsWorkspaceReqAllocate_Post_PointerVar_Nop,
    RtemsWorkspaceReqAllocate_Post_Heap_Nop,
    RtemsWorkspaceReqAllocate_Post_AllocatorLock_Nop },
  { 0, 0, 0, 0, RtemsWorkspaceReqAllocate_Post_Status_True,
    RtemsWorkspaceReqAllocate_Post_PointerVar_Set,
    RtemsWorkspaceReqAllocate_Post_Heap_Allocated,
    RtemsWorkspaceReqAllocate_Post_AllocatorLock_Once },
  { 0, 0, 0, 0, RtemsWorkspaceReqAllocate_Post_Status_False,
    RtemsWorkspaceReqAllocate_Post_PointerVar_Nop,
    RtemsWorkspaceReqAllocate_Post_Heap_Nop,
    RtemsWorkspaceReqAllocate_Post_AllocatorLock_Once }
};

static const uint8_t
RtemsWorkspaceReqAllocate_Map[] = {
  0, 0, 1, 2, 0, 0, 0, 0
};

/* clang-format on */

static size_t RtemsWorkspaceReqAllocate_Scope( void *arg, char *buf, size_t n )
{
  RtemsWorkspaceReqAllocate_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsWorkspaceReqAllocate_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsWorkspaceReqAllocate_Fixture = {
  .setup = RtemsWorkspaceReqAllocate_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsWorkspaceReqAllocate_Teardown_Wrap,
  .scope = RtemsWorkspaceReqAllocate_Scope,
  .initial_context = &RtemsWorkspaceReqAllocate_Instance
};

static inline RtemsWorkspaceReqAllocate_Entry
RtemsWorkspaceReqAllocate_PopEntry( RtemsWorkspaceReqAllocate_Context *ctx )
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsWorkspaceReqAllocate_Entries
    [ RtemsWorkspaceReqAllocate_Map[ index ] ];
}

static void RtemsWorkspaceReqAllocate_TestVariant(
  RtemsWorkspaceReqAllocate_Context *ctx
)
{
  RtemsWorkspaceReqAllocate_Pre_Pointer_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsWorkspaceReqAllocate_Pre_Bytes_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsWorkspaceReqAllocate_Pre_Fit_Prepare( ctx->Map.pcs[ 2 ] );
  RtemsWorkspaceReqAllocate_Action( ctx );
  RtemsWorkspaceReqAllocate_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  RtemsWorkspaceReqAllocate_Post_PointerVar_Check(
    ctx,
    ctx->Map.entry.Post_PointerVar
  );
  RtemsWorkspaceReqAllocate_Post_Heap_Check( ctx, ctx->Map.entry.Post_Heap );
  RtemsWorkspaceReqAllocate_Post_AllocatorLock_Check(
    ctx,
    ctx->Map.entry.Post_AllocatorLock
  );
}

/**
 * @fn void T_case_body_RtemsWorkspaceReqAllocate( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsWorkspaceReqAllocate,
  &RtemsWorkspaceReqAllocate_Fixture
)
{
  RtemsWorkspaceReqAllocate_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsWorkspaceReqAllocate_Pre_Pointer_Valid;
    ctx->Map.pcs[ 0 ] < RtemsWorkspaceReqAllocate_Pre_Pointer_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsWorkspaceReqAllocate_Pre_Bytes_Zero;
      ctx->Map.pcs[ 1 ] < RtemsWorkspaceReqAllocate_Pre_Bytes_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsWorkspaceReqAllocate_Pre_Fit_Yes;
        ctx->Map.pcs[ 2 ] < RtemsWorkspaceReqAllocate_Pre_Fit_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = RtemsWorkspaceReqAllocate_PopEntry( ctx );
        RtemsWorkspaceReqAllocate_Prepare( ctx );
        RtemsWorkspaceReqAllocate_TestVariant( ctx );
      }
    }
  }
}

/** @} */
