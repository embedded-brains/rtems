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
#include <stdint.h>
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
    bool rep_ran[ 21 ];

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

static const char *const RtemsWorkspaceReqAllocate_PreDesc_Bytes[] = {
  "Zero",
  "NonZeroOne",
  "NonZeroMinMinusOne",
  "NonZeroMin",
  "NonZeroMinPlusOne",
  "NonZeroUnitMinusOne",
  "NonZeroUnit",
  "NonZeroUnitPlusOne",
  "NonZeroSlab",
  "NonZeroMax",
  "NonZeroAboveMax",
  "NonZeroSizeMax",
  "NA"
};

static const char *const RtemsWorkspaceReqAllocate_PreDesc_Fit[] =
  { "YesEmpty", "YesExact", "NoExhausted", "NoShort", "NA" };

static const char *const *const RtemsWorkspaceReqAllocate_PreDesc[] = {
  RtemsWorkspaceReqAllocate_PreDesc_Pointer,
  RtemsWorkspaceReqAllocate_PreDesc_Bytes,
  RtemsWorkspaceReqAllocate_PreDesc_Fit,
  NULL
};

typedef enum {
  RtemsWorkspaceReqAllocate_Pre_Bytes_NonZero_One,
  RtemsWorkspaceReqAllocate_Pre_Bytes_NonZero_MinMinusOne,
  RtemsWorkspaceReqAllocate_Pre_Bytes_NonZero_Min,
  RtemsWorkspaceReqAllocate_Pre_Bytes_NonZero_MinPlusOne,
  RtemsWorkspaceReqAllocate_Pre_Bytes_NonZero_UnitMinusOne,
  RtemsWorkspaceReqAllocate_Pre_Bytes_NonZero_Unit,
  RtemsWorkspaceReqAllocate_Pre_Bytes_NonZero_UnitPlusOne,
  RtemsWorkspaceReqAllocate_Pre_Bytes_NonZero_Slab,
  RtemsWorkspaceReqAllocate_Pre_Bytes_NonZero_Max,
  RtemsWorkspaceReqAllocate_Pre_Bytes_NonZero_AboveMax,
  RtemsWorkspaceReqAllocate_Pre_Bytes_NonZero_SizeMax
} RtemsWorkspaceReqAllocate_Pre_Bytes_NonZero_Rep;

typedef enum {
  RtemsWorkspaceReqAllocate_Pre_Fit_Yes_Empty,
  RtemsWorkspaceReqAllocate_Pre_Fit_Yes_Exact
} RtemsWorkspaceReqAllocate_Pre_Fit_Yes_Rep;

typedef enum {
  RtemsWorkspaceReqAllocate_Pre_Fit_No_Exhausted,
  RtemsWorkspaceReqAllocate_Pre_Fit_No_Short
} RtemsWorkspaceReqAllocate_Pre_Fit_No_Rep;

static const uint8_t RtemsWorkspaceReqAllocate_PreRepOffset_Pointer[] =
  { 0, 1, 2, 3 };

static const uint8_t RtemsWorkspaceReqAllocate_PreRepOffset_Bytes[] =
  { 0, 1, 12, 13 };

static const uint8_t RtemsWorkspaceReqAllocate_PreRepOffset_Fit[] =
  { 0, 2, 4, 5 };

static const uint8_t *const RtemsWorkspaceReqAllocate_PreRepOffset[] = {
  RtemsWorkspaceReqAllocate_PreRepOffset_Pointer,
  RtemsWorkspaceReqAllocate_PreRepOffset_Bytes,
  RtemsWorkspaceReqAllocate_PreRepOffset_Fit
};

static size_t RtemsWorkspaceReqAllocate_GetRepresentativeCountOf(
  const RtemsWorkspaceReqAllocate_Context *ctx,
  size_t                                   index
)
{
  const uint8_t *offsets;
  size_t         state;

  offsets = RtemsWorkspaceReqAllocate_PreRepOffset[ index ];
  state = ctx->Map.pcs[ index ];

  return offsets[ state + 1 ] - offsets[ state ];
}

static size_t RtemsWorkspaceReqAllocate_GetRepresentative(
  const RtemsWorkspaceReqAllocate_Context *ctx,
  size_t                                   index
)
{
  size_t count;

  count = RtemsWorkspaceReqAllocate_GetRepresentativeCountOf( ctx, index );

  if ( ctx->Map.rep < count ) {
    return ctx->Map.rep;
  }

  return count - 1;
}

static size_t RtemsWorkspaceReqAllocate_GetRepresentativeCount(
  const RtemsWorkspaceReqAllocate_Context *ctx
)
{
  size_t count;
  size_t i;

  count = 1;

  for ( i = 0; i < 3; ++i ) {
    size_t count_of_condition;

    count_of_condition = RtemsWorkspaceReqAllocate_GetRepresentativeCountOf(
      ctx,
      i
    );

    if ( count_of_condition > count ) {
      count = count_of_condition;
    }
  }

  return count;
}

static RtemsWorkspaceReqAllocate_Pre_Bytes_NonZero_Rep
RtemsWorkspaceReqAllocate_Pre_Bytes_NonZero_GetRepresentative(
  const RtemsWorkspaceReqAllocate_Context *ctx
)
{
  return (
    RtemsWorkspaceReqAllocate_Pre_Bytes_NonZero_Rep
  ) RtemsWorkspaceReqAllocate_GetRepresentative( ctx, 1 );
}

static RtemsWorkspaceReqAllocate_Pre_Fit_Yes_Rep
RtemsWorkspaceReqAllocate_Pre_Fit_Yes_GetRepresentative(
  const RtemsWorkspaceReqAllocate_Context *ctx
)
{
  return (
    RtemsWorkspaceReqAllocate_Pre_Fit_Yes_Rep
  ) RtemsWorkspaceReqAllocate_GetRepresentative( ctx, 2 );
}

static RtemsWorkspaceReqAllocate_Pre_Fit_No_Rep
RtemsWorkspaceReqAllocate_Pre_Fit_No_GetRepresentative(
  const RtemsWorkspaceReqAllocate_Context *ctx
)
{
  return (
    RtemsWorkspaceReqAllocate_Pre_Fit_No_Rep
  ) RtemsWorkspaceReqAllocate_GetRepresentative( ctx, 2 );
}

static const uint8_t RtemsWorkspaceReqAllocate_PreRepBase[] = { 0, 3, 16 };

static const uint8_t RtemsWorkspaceReqAllocate_PreRepStates[] = { 2, 2, 2 };

static void RtemsWorkspaceReqAllocate_ResetRepresentatives(
  RtemsWorkspaceReqAllocate_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 21; ++i ) {
    ctx->Map.rep_ran[ i ] = false;
  }
}

static void RtemsWorkspaceReqAllocate_MarkRepresentatives(
  RtemsWorkspaceReqAllocate_Context *ctx
)
{
  size_t i;

  if ( ctx->Map.skip_rep ) {
    return;
  }

  for ( i = 0; i < 3; ++i ) {
    size_t rep;

    rep = RtemsWorkspaceReqAllocate_PreRepBase[ i ] +
          RtemsWorkspaceReqAllocate_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
          RtemsWorkspaceReqAllocate_GetRepresentative( ctx, i );
    ctx->Map.rep_ran[ rep ] = true;
  }
}

static void RtemsWorkspaceReqAllocate_CheckRepresentatives(
  const RtemsWorkspaceReqAllocate_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 3; ++i ) {
    const uint8_t *offsets;
    const bool    *ran;
    size_t         state;

    offsets = RtemsWorkspaceReqAllocate_PreRepOffset[ i ];
    ran = &ctx->Map.rep_ran[ RtemsWorkspaceReqAllocate_PreRepBase[ i ] ];

    for (
      state = 0; state < RtemsWorkspaceReqAllocate_PreRepStates[ i ]; ++state
    ) {
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
            RtemsWorkspaceReqAllocate_PreDesc[ i ][ k ]
          );
        }
      }
    }
  }
}

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
      switch (
        RtemsWorkspaceReqAllocate_Pre_Bytes_NonZero_GetRepresentative( ctx )
      ) {
        case RtemsWorkspaceReqAllocate_Pre_Bytes_NonZero_One: {
          ctx->bytes = 1;
          break;
        }

        case RtemsWorkspaceReqAllocate_Pre_Bytes_NonZero_MinMinusOne: {
          ctx->bytes = RBHEAP_MIN_SIZE - 1;
          break;
        }

        case RtemsWorkspaceReqAllocate_Pre_Bytes_NonZero_Min: {
          ctx->bytes = RBHEAP_MIN_SIZE;
          break;
        }

        case RtemsWorkspaceReqAllocate_Pre_Bytes_NonZero_MinPlusOne: {
          ctx->bytes = RBHEAP_MIN_SIZE + 1;
          break;
        }

        case RtemsWorkspaceReqAllocate_Pre_Bytes_NonZero_UnitMinusOne: {
          ctx->bytes = RBHEAP_MIN_SIZE + RBHEAP_ALIGNMENT - 1;
          break;
        }

        case RtemsWorkspaceReqAllocate_Pre_Bytes_NonZero_Unit: {
          ctx->bytes = RBHEAP_MIN_SIZE + RBHEAP_ALIGNMENT;
          break;
        }

        case RtemsWorkspaceReqAllocate_Pre_Bytes_NonZero_UnitPlusOne: {
          ctx->bytes = RBHEAP_MIN_SIZE + RBHEAP_ALIGNMENT + 1;
          break;
        }

        case RtemsWorkspaceReqAllocate_Pre_Bytes_NonZero_Slab: {
          ctx->bytes = RBHEAP_SLAB_SIZE;
          break;
        }

        case RtemsWorkspaceReqAllocate_Pre_Bytes_NonZero_Max: {
          ctx->bytes = RBHEAP_MAX_SIZE;
          break;
        }

        case RtemsWorkspaceReqAllocate_Pre_Bytes_NonZero_AboveMax: {
          ctx->bytes = RBHEAP_MAX_SIZE + 1;
          break;
        }

        case RtemsWorkspaceReqAllocate_Pre_Bytes_NonZero_SizeMax: {
          ctx->bytes = SIZE_MAX;
          break;
        }
      }
      break;
    }

    case RtemsWorkspaceReqAllocate_Pre_Bytes_NA:
      break;
  }
}

static void RtemsWorkspaceReqAllocate_Pre_Fit_Prepare(
  RtemsWorkspaceReqAllocate_Context *ctx,
  RtemsWorkspaceReqAllocate_Pre_Fit  state
)
{
  switch ( state ) {
    case RtemsWorkspaceReqAllocate_Pre_Fit_Yes: {
      /*
       * While the RTEMS Workspace has a free block which satisfies the
       * request.
       */
      switch (
        RtemsWorkspaceReqAllocate_Pre_Fit_Yes_GetRepresentative( ctx )
      ) {
        case RtemsWorkspaceReqAllocate_Pre_Fit_Yes_Empty: {
          if (
            ctx->bytes > HeapGetLargestAllocatableSize( &_Workspace_RBHeap )
          ) {
            ctx->Map.skip_rep = true;
          }
          break;
        }

        case RtemsWorkspaceReqAllocate_Pre_Fit_Yes_Exact: {
          if (
            ctx->bytes > HeapGetLargestAllocatableSize( &_Workspace_RBHeap )
          ) {
            ctx->Map.skip_rep = true;
          } else {
            HeapLeaveFreeBlock(
              &_Workspace_RBHeap,
              ctx->bytes,
              RBHEAP_ALIGNMENT
            );
          }
          break;
        }
      }
      break;
    }

    case RtemsWorkspaceReqAllocate_Pre_Fit_No: {
      /*
       * While the RTEMS Workspace has no free block which satisfies the
       * request.
       */
      switch (
        RtemsWorkspaceReqAllocate_Pre_Fit_No_GetRepresentative( ctx )
      ) {
        case RtemsWorkspaceReqAllocate_Pre_Fit_No_Exhausted: {
          HeapExhaust( &_Workspace_RBHeap );
          break;
        }

        case RtemsWorkspaceReqAllocate_Pre_Fit_No_Short: {
          if (
            ctx->bytes >= RBHEAP_MIN_SIZE + RBHEAP_ALIGNMENT &&
            ctx->bytes <= HeapGetLargestAllocatableSize( &_Workspace_RBHeap )
          ) {
            HeapLeaveFreeBlock(
              &_Workspace_RBHeap,
              ctx->bytes - RBHEAP_ALIGNMENT,
              RBHEAP_ALIGNMENT
            );
          } else {
            HeapExhaust( &_Workspace_RBHeap );
          }
          break;
        }
      }
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
    size_t pcd[ 3 ];
    size_t i;

    for ( i = 0; i < 3; ++i ) {
      pcd[ i ] = RtemsWorkspaceReqAllocate_PreRepOffset[ i ]
                                                       [ ctx->Map.pcs[ i ] ] +
                 RtemsWorkspaceReqAllocate_GetRepresentative( ctx, i );
    }

    return T_get_scope( RtemsWorkspaceReqAllocate_PreDesc, buf, n, pcd );
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
  RtemsWorkspaceReqAllocate_Pre_Fit_Prepare( ctx, ctx->Map.pcs[ 2 ] );

  if ( ctx->Map.skip_rep ) {
    return;
  }

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
  RtemsWorkspaceReqAllocate_ResetRepresentatives( ctx );

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

        for (
          ctx->Map.rep = 0;
          ctx->Map.rep <
          RtemsWorkspaceReqAllocate_GetRepresentativeCount( ctx );
          ++ctx->Map.rep
        ) {
          ctx->Map.skip_rep = false;
          RtemsWorkspaceReqAllocate_Prepare( ctx );
          RtemsWorkspaceReqAllocate_TestVariant( ctx );

          RtemsWorkspaceReqAllocate_MarkRepresentatives( ctx );
        }
      }
    }
  }

  RtemsWorkspaceReqAllocate_CheckRepresentatives( ctx );
}

/** @} */
