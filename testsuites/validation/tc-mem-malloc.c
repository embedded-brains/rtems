/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CReqMalloc
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
#include <stdlib.h>

#include "tx-heap.h"

#include <rtems/test.h>

/**
 * @defgroup CReqMalloc spec:/c/req/malloc
 *
 * @ingroup TestsuitesValidation0
 *
 * @{
 */

typedef enum {
  CReqMalloc_Pre_Size_Zero,
  CReqMalloc_Pre_Size_NonZero,
  CReqMalloc_Pre_Size_NA
} CReqMalloc_Pre_Size;

typedef enum {
  CReqMalloc_Pre_Fit_Yes,
  CReqMalloc_Pre_Fit_No,
  CReqMalloc_Pre_Fit_NA
} CReqMalloc_Pre_Fit;

typedef enum {
  CReqMalloc_Post_Status_Block,
  CReqMalloc_Post_Status_Null,
  CReqMalloc_Post_Status_NA
} CReqMalloc_Post_Status;

typedef enum {
  CReqMalloc_Post_Errno_Nop,
  CReqMalloc_Post_Errno_ENOMEM,
  CReqMalloc_Post_Errno_NA
} CReqMalloc_Post_Errno;

typedef enum {
  CReqMalloc_Post_Alignment_Heap,
  CReqMalloc_Post_Alignment_NA
} CReqMalloc_Post_Alignment;

typedef enum {
  CReqMalloc_Post_Size_Valid,
  CReqMalloc_Post_Size_NA
} CReqMalloc_Post_Size;

typedef enum {
  CReqMalloc_Post_Heap_Allocated,
  CReqMalloc_Post_Heap_Nop,
  CReqMalloc_Post_Heap_NA
} CReqMalloc_Post_Heap;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Size_NA : 1;
  uint16_t Pre_Fit_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Errno : 2;
  uint16_t Post_Alignment : 1;
  uint16_t Post_Size : 1;
  uint16_t Post_Heap : 2;
} CReqMalloc_Entry;

#define ERRNO_SENTINEL -1

/**
 * @brief Test context for spec:/c/req/malloc test case.
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
   * @brief This member contains the return value of the malloc() call.
   */
  void *ptr;

  /**
   * @brief This member contains the value of errno after the malloc() call.
   */
  int errno_value;

  /**
   * @brief This member specifies the `size` parameter value.
   */
  size_t size;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 2 ];

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
    bool rep_ran[ 18 ];

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
    CReqMalloc_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CReqMalloc_Context;

static CReqMalloc_Context CReqMalloc_Instance;

static const char *const CReqMalloc_PreDesc_Size[] = {
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

static const char *const CReqMalloc_PreDesc_Fit[] =
  { "YesEmpty", "YesExact", "NoExhausted", "NoShort", "NA" };

static const char *const *const CReqMalloc_PreDesc[] =
  { CReqMalloc_PreDesc_Size, CReqMalloc_PreDesc_Fit, NULL };

typedef enum {
  CReqMalloc_Pre_Size_NonZero_One,
  CReqMalloc_Pre_Size_NonZero_MinMinusOne,
  CReqMalloc_Pre_Size_NonZero_Min,
  CReqMalloc_Pre_Size_NonZero_MinPlusOne,
  CReqMalloc_Pre_Size_NonZero_UnitMinusOne,
  CReqMalloc_Pre_Size_NonZero_Unit,
  CReqMalloc_Pre_Size_NonZero_UnitPlusOne,
  CReqMalloc_Pre_Size_NonZero_Slab,
  CReqMalloc_Pre_Size_NonZero_Max,
  CReqMalloc_Pre_Size_NonZero_AboveMax,
  CReqMalloc_Pre_Size_NonZero_SizeMax
} CReqMalloc_Pre_Size_NonZero_Rep;

typedef enum {
  CReqMalloc_Pre_Fit_Yes_Empty,
  CReqMalloc_Pre_Fit_Yes_Exact
} CReqMalloc_Pre_Fit_Yes_Rep;

typedef enum {
  CReqMalloc_Pre_Fit_No_Exhausted,
  CReqMalloc_Pre_Fit_No_Short
} CReqMalloc_Pre_Fit_No_Rep;

static const uint8_t CReqMalloc_PreRepOffset_Size[] = { 0, 1, 12, 13 };

static const uint8_t CReqMalloc_PreRepOffset_Fit[] = { 0, 2, 4, 5 };

static const uint8_t *const CReqMalloc_PreRepOffset[] = {
  CReqMalloc_PreRepOffset_Size,
  CReqMalloc_PreRepOffset_Fit
};

static size_t CReqMalloc_GetRepresentativeCountOf(
  const CReqMalloc_Context *ctx,
  size_t                    index
)
{
  const uint8_t *offsets;
  size_t         state;

  offsets = CReqMalloc_PreRepOffset[ index ];
  state = ctx->Map.pcs[ index ];

  return offsets[ state + 1 ] - offsets[ state ];
}

static size_t CReqMalloc_GetRepresentative(
  const CReqMalloc_Context *ctx,
  size_t                    index
)
{
  size_t count;

  count = CReqMalloc_GetRepresentativeCountOf( ctx, index );

  if ( ctx->Map.rep < count ) {
    return ctx->Map.rep;
  }

  return count - 1;
}

static size_t CReqMalloc_GetRepresentativeCount(
  const CReqMalloc_Context *ctx
)
{
  size_t count;
  size_t i;

  count = 1;

  for ( i = 0; i < 2; ++i ) {
    size_t count_of_condition;

    count_of_condition = CReqMalloc_GetRepresentativeCountOf( ctx, i );

    if ( count_of_condition > count ) {
      count = count_of_condition;
    }
  }

  return count;
}

static CReqMalloc_Pre_Size_NonZero_Rep
CReqMalloc_Pre_Size_NonZero_GetRepresentative( const CReqMalloc_Context *ctx )
{
  return (
    CReqMalloc_Pre_Size_NonZero_Rep
  ) CReqMalloc_GetRepresentative( ctx, 0 );
}

static CReqMalloc_Pre_Fit_Yes_Rep CReqMalloc_Pre_Fit_Yes_GetRepresentative(
  const CReqMalloc_Context *ctx
)
{
  return (CReqMalloc_Pre_Fit_Yes_Rep) CReqMalloc_GetRepresentative( ctx, 1 );
}

static CReqMalloc_Pre_Fit_No_Rep CReqMalloc_Pre_Fit_No_GetRepresentative(
  const CReqMalloc_Context *ctx
)
{
  return (CReqMalloc_Pre_Fit_No_Rep) CReqMalloc_GetRepresentative( ctx, 1 );
}

static const uint8_t CReqMalloc_PreRepBase[] = { 0, 13 };

static const uint8_t CReqMalloc_PreRepStates[] = { 2, 2 };

static void CReqMalloc_ResetRepresentatives( CReqMalloc_Context *ctx )
{
  size_t i;

  for ( i = 0; i < 18; ++i ) {
    ctx->Map.rep_ran[ i ] = false;
  }
}

static void CReqMalloc_MarkRepresentatives( CReqMalloc_Context *ctx )
{
  size_t i;

  if ( ctx->Map.skip_rep ) {
    return;
  }

  for ( i = 0; i < 2; ++i ) {
    size_t rep;

    rep = CReqMalloc_PreRepBase[ i ] +
          CReqMalloc_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
          CReqMalloc_GetRepresentative( ctx, i );
    ctx->Map.rep_ran[ rep ] = true;
  }
}

static void CReqMalloc_CheckRepresentatives( const CReqMalloc_Context *ctx )
{
  size_t i;

  for ( i = 0; i < 2; ++i ) {
    const uint8_t *offsets;
    const bool    *ran;
    size_t         state;

    offsets = CReqMalloc_PreRepOffset[ i ];
    ran = &ctx->Map.rep_ran[ CReqMalloc_PreRepBase[ i ] ];

    for ( state = 0; state < CReqMalloc_PreRepStates[ i ]; ++state ) {
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
            CReqMalloc_PreDesc[ i ][ k ]
          );
        }
      }
    }
  }
}

static void CReqMalloc_Pre_Size_Prepare(
  CReqMalloc_Context *ctx,
  CReqMalloc_Pre_Size state
)
{
  switch ( state ) {
    case CReqMalloc_Pre_Size_Zero: {
      /*
       * While the `size` parameter is equal to zero.
       */
      ctx->size = 0;
      break;
    }

    case CReqMalloc_Pre_Size_NonZero: {
      /*
       * While the `size` parameter is not equal to zero.
       */
      switch ( CReqMalloc_Pre_Size_NonZero_GetRepresentative( ctx ) ) {
        case CReqMalloc_Pre_Size_NonZero_One: {
          ctx->size = 1;
          break;
        }

        case CReqMalloc_Pre_Size_NonZero_MinMinusOne: {
          ctx->size = RBHEAP_MIN_SIZE - 1;
          break;
        }

        case CReqMalloc_Pre_Size_NonZero_Min: {
          ctx->size = RBHEAP_MIN_SIZE;
          break;
        }

        case CReqMalloc_Pre_Size_NonZero_MinPlusOne: {
          ctx->size = RBHEAP_MIN_SIZE + 1;
          break;
        }

        case CReqMalloc_Pre_Size_NonZero_UnitMinusOne: {
          ctx->size = RBHEAP_MIN_SIZE + RBHEAP_ALIGNMENT - 1;
          break;
        }

        case CReqMalloc_Pre_Size_NonZero_Unit: {
          ctx->size = RBHEAP_MIN_SIZE + RBHEAP_ALIGNMENT;
          break;
        }

        case CReqMalloc_Pre_Size_NonZero_UnitPlusOne: {
          ctx->size = RBHEAP_MIN_SIZE + RBHEAP_ALIGNMENT + 1;
          break;
        }

        case CReqMalloc_Pre_Size_NonZero_Slab: {
          ctx->size = RBHEAP_SLAB_SIZE;
          break;
        }

        case CReqMalloc_Pre_Size_NonZero_Max: {
          ctx->size = RBHEAP_MAX_SIZE;
          break;
        }

        case CReqMalloc_Pre_Size_NonZero_AboveMax: {
          ctx->size = RBHEAP_MAX_SIZE + 1;
          break;
        }

        case CReqMalloc_Pre_Size_NonZero_SizeMax: {
          ctx->size = SIZE_MAX;
          break;
        }
      }
      break;
    }

    case CReqMalloc_Pre_Size_NA:
      break;
  }
}

static void CReqMalloc_Pre_Fit_Prepare(
  CReqMalloc_Context *ctx,
  CReqMalloc_Pre_Fit  state
)
{
  switch ( state ) {
    case CReqMalloc_Pre_Fit_Yes: {
      /*
       * While the C Program Heap has a free block which satisfies the request.
       */
      switch ( CReqMalloc_Pre_Fit_Yes_GetRepresentative( ctx ) ) {
        case CReqMalloc_Pre_Fit_Yes_Empty: {
          if ( ctx->size > HeapGetLargestAllocatableSize( &ctx->heap.heap ) ) {
            ctx->Map.skip_rep = true;
          }
          break;
        }

        case CReqMalloc_Pre_Fit_Yes_Exact: {
          if ( ctx->size > HeapGetLargestAllocatableSize( &ctx->heap.heap ) ) {
            ctx->Map.skip_rep = true;
          } else {
            HeapLeaveFreeBlock( &ctx->heap.heap, ctx->size, RBHEAP_ALIGNMENT );
          }
          break;
        }
      }
      break;
    }

    case CReqMalloc_Pre_Fit_No: {
      /*
       * While the C Program Heap has no free block which satisfies the
       * request.
       */
      switch ( CReqMalloc_Pre_Fit_No_GetRepresentative( ctx ) ) {
        case CReqMalloc_Pre_Fit_No_Exhausted: {
          HeapExhaust( &ctx->heap.heap );
          break;
        }

        case CReqMalloc_Pre_Fit_No_Short: {
          if (
            ctx->size >= RBHEAP_MIN_SIZE + RBHEAP_ALIGNMENT &&
            ctx->size <= HeapGetLargestAllocatableSize( &ctx->heap.heap )
          ) {
            HeapLeaveFreeBlock(
              &ctx->heap.heap,
              ctx->size - RBHEAP_ALIGNMENT,
              RBHEAP_ALIGNMENT
            );
          } else {
            HeapExhaust( &ctx->heap.heap );
          }
          break;
        }
      }
      break;
    }

    case CReqMalloc_Pre_Fit_NA:
      break;
  }
}

static void CReqMalloc_Post_Status_Check(
  CReqMalloc_Context    *ctx,
  CReqMalloc_Post_Status state
)
{
  switch ( state ) {
    case CReqMalloc_Post_Status_Block: {
      /*
       * The return value of malloc() shall be the begin address of a used
       * block of the C Program Heap.
       */
      T_not_null( ctx->ptr );
      break;
    }

    case CReqMalloc_Post_Status_Null: {
      /*
       * The return value of malloc() shall be equal to NULL.
       */
      T_null( ctx->ptr );
      break;
    }

    case CReqMalloc_Post_Status_NA:
      break;
  }
}

static void CReqMalloc_Post_Errno_Check(
  CReqMalloc_Context   *ctx,
  CReqMalloc_Post_Errno state
)
{
  switch ( state ) {
    case CReqMalloc_Post_Errno_Nop: {
      /*
       * The value of errno shall not be changed by the malloc() call.
       */
      T_eq_int( ctx->errno_value, ERRNO_SENTINEL );
      break;
    }

    case CReqMalloc_Post_Errno_ENOMEM: {
      /*
       * The value of errno shall be equal to ENOMEM.
       */
      T_eq_int( ctx->errno_value, ENOMEM );
      break;
    }

    case CReqMalloc_Post_Errno_NA:
      break;
  }
}

static void CReqMalloc_Post_Alignment_Check(
  CReqMalloc_Context       *ctx,
  CReqMalloc_Post_Alignment state
)
{
  switch ( state ) {
    case CReqMalloc_Post_Alignment_Heap: {
      /*
       * The begin address of the used block shall be an integral multiple of
       * the heap alignment of the target architecture.
       */
      T_eq_uptr( (uintptr_t) ctx->ptr % CPU_HEAP_ALIGNMENT, 0 );
      break;
    }

    case CReqMalloc_Post_Alignment_NA:
      break;
  }
}

static void CReqMalloc_Post_Size_Check(
  CReqMalloc_Context  *ctx,
  CReqMalloc_Post_Size state
)
{
  uintptr_t size;

  switch ( state ) {
    case CReqMalloc_Post_Size_Valid: {
      /*
       * The size of the used block shall be greater than or equal to the
       * `size` parameter.
       */
      T_true( _RBHeap_Size_of_block( &ctx->heap.heap, ctx->ptr, &size ) );
      T_ge_uptr( size, ctx->size );
      break;
    }

    case CReqMalloc_Post_Size_NA:
      break;
  }
}

static void CReqMalloc_Post_Heap_Check(
  CReqMalloc_Context  *ctx,
  CReqMalloc_Post_Heap state
)
{
  switch ( state ) {
    case CReqMalloc_Post_Heap_Allocated: {
      /*
       * The C Program Heap shall have one more used block.
       */
      T_eq_u32( ctx->after.used_blocks, ctx->before.used_blocks + 1 );
      break;
    }

    case CReqMalloc_Post_Heap_Nop: {
      /*
       * The used blocks and the free blocks of the C Program Heap shall not be
       * changed by the malloc() call.
       */
      T_eq_u32( ctx->after.used_blocks, ctx->before.used_blocks );
      T_eq_u32( ctx->after.free_blocks, ctx->before.free_blocks );
      T_eq_uptr( ctx->after.free_size, ctx->before.free_size );
      break;
    }

    case CReqMalloc_Post_Heap_NA:
      break;
  }
}

static void CReqMalloc_Setup( CReqMalloc_Context *ctx )
{
  HeapInstall( &ctx->heap );
}

static void CReqMalloc_Setup_Wrap( void *arg )
{
  CReqMalloc_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CReqMalloc_Setup( ctx );
}

static void CReqMalloc_Teardown( CReqMalloc_Context *ctx )
{
  HeapRestore( &ctx->heap );
}

static void CReqMalloc_Teardown_Wrap( void *arg )
{
  CReqMalloc_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CReqMalloc_Teardown( ctx );
}

static void CReqMalloc_Prepare( CReqMalloc_Context *ctx )
{
  HeapReset( &ctx->heap );
}

static void CReqMalloc_Action( CReqMalloc_Context *ctx )
{
  HeapTakeSnapshot( &ctx->heap.heap, &ctx->before );
  errno = ERRNO_SENTINEL;
  ctx->ptr = malloc( ctx->size );
  ctx->errno_value = errno;
  HeapTakeSnapshot( &ctx->heap.heap, &ctx->after );
}

/* clang-format off */

static const CReqMalloc_Entry
CReqMalloc_Entries[] = {
  { 0, 0, 0, CReqMalloc_Post_Status_Null, CReqMalloc_Post_Errno_Nop,
    CReqMalloc_Post_Alignment_NA, CReqMalloc_Post_Size_NA,
    CReqMalloc_Post_Heap_Nop },
  { 0, 0, 0, CReqMalloc_Post_Status_Block, CReqMalloc_Post_Errno_Nop,
    CReqMalloc_Post_Alignment_Heap, CReqMalloc_Post_Size_Valid,
    CReqMalloc_Post_Heap_Allocated },
  { 0, 0, 0, CReqMalloc_Post_Status_Null, CReqMalloc_Post_Errno_ENOMEM,
    CReqMalloc_Post_Alignment_NA, CReqMalloc_Post_Size_NA,
    CReqMalloc_Post_Heap_Nop }
};

static const uint8_t
CReqMalloc_Map[] = {
  0, 0, 1, 2
};

/* clang-format on */

static size_t CReqMalloc_Scope( void *arg, char *buf, size_t n )
{
  CReqMalloc_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    size_t pcd[ 2 ];
    size_t i;

    for ( i = 0; i < 2; ++i ) {
      pcd[ i ] = CReqMalloc_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
                 CReqMalloc_GetRepresentative( ctx, i );
    }

    return T_get_scope( CReqMalloc_PreDesc, buf, n, pcd );
  }

  return 0;
}

static T_fixture CReqMalloc_Fixture = {
  .setup = CReqMalloc_Setup_Wrap,
  .stop = NULL,
  .teardown = CReqMalloc_Teardown_Wrap,
  .scope = CReqMalloc_Scope,
  .initial_context = &CReqMalloc_Instance
};

static inline CReqMalloc_Entry CReqMalloc_PopEntry( CReqMalloc_Context *ctx )
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CReqMalloc_Entries[ CReqMalloc_Map[ index ] ];
}

static void CReqMalloc_TestVariant( CReqMalloc_Context *ctx )
{
  CReqMalloc_Pre_Size_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CReqMalloc_Pre_Fit_Prepare( ctx, ctx->Map.pcs[ 1 ] );

  if ( ctx->Map.skip_rep ) {
    return;
  }

  CReqMalloc_Action( ctx );
  CReqMalloc_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CReqMalloc_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
  CReqMalloc_Post_Alignment_Check( ctx, ctx->Map.entry.Post_Alignment );
  CReqMalloc_Post_Size_Check( ctx, ctx->Map.entry.Post_Size );
  CReqMalloc_Post_Heap_Check( ctx, ctx->Map.entry.Post_Heap );
}

/**
 * @fn void T_case_body_CReqMalloc( void )
 */
T_TEST_CASE_FIXTURE( CReqMalloc, &CReqMalloc_Fixture )
{
  CReqMalloc_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  CReqMalloc_ResetRepresentatives( ctx );

  for (
    ctx->Map.pcs[ 0 ] = CReqMalloc_Pre_Size_Zero;
    ctx->Map.pcs[ 0 ] < CReqMalloc_Pre_Size_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CReqMalloc_Pre_Fit_Yes;
      ctx->Map.pcs[ 1 ] < CReqMalloc_Pre_Fit_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CReqMalloc_PopEntry( ctx );

      for (
        ctx->Map.rep = 0;
        ctx->Map.rep < CReqMalloc_GetRepresentativeCount( ctx );
        ++ctx->Map.rep
      ) {
        ctx->Map.skip_rep = false;
        CReqMalloc_Prepare( ctx );
        CReqMalloc_TestVariant( ctx );

        CReqMalloc_MarkRepresentatives( ctx );
      }
    }
  }

  CReqMalloc_CheckRepresentatives( ctx );
}

/** @} */
