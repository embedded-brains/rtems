/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CReqAlignedAlloc
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
 * @defgroup CReqAlignedAlloc spec:/c/req/aligned-alloc
 *
 * @ingroup TestsuitesValidation0
 *
 * @{
 */

typedef enum {
  CReqAlignedAlloc_Pre_Alignment_Zero,
  CReqAlignedAlloc_Pre_Alignment_NotPower2,
  CReqAlignedAlloc_Pre_Alignment_Power2,
  CReqAlignedAlloc_Pre_Alignment_NA
} CReqAlignedAlloc_Pre_Alignment;

typedef enum {
  CReqAlignedAlloc_Pre_Size_Zero,
  CReqAlignedAlloc_Pre_Size_NonZero,
  CReqAlignedAlloc_Pre_Size_NA
} CReqAlignedAlloc_Pre_Size;

typedef enum {
  CReqAlignedAlloc_Pre_Fit_Yes,
  CReqAlignedAlloc_Pre_Fit_No,
  CReqAlignedAlloc_Pre_Fit_NA
} CReqAlignedAlloc_Pre_Fit;

typedef enum {
  CReqAlignedAlloc_Post_Status_Block,
  CReqAlignedAlloc_Post_Status_Null,
  CReqAlignedAlloc_Post_Status_NA
} CReqAlignedAlloc_Post_Status;

typedef enum {
  CReqAlignedAlloc_Post_Errno_Nop,
  CReqAlignedAlloc_Post_Errno_EINVAL,
  CReqAlignedAlloc_Post_Errno_ENOMEM,
  CReqAlignedAlloc_Post_Errno_NA
} CReqAlignedAlloc_Post_Errno;

typedef enum {
  CReqAlignedAlloc_Post_Alignment_Valid,
  CReqAlignedAlloc_Post_Alignment_NA
} CReqAlignedAlloc_Post_Alignment;

typedef enum {
  CReqAlignedAlloc_Post_Size_Valid,
  CReqAlignedAlloc_Post_Size_NA
} CReqAlignedAlloc_Post_Size;

typedef enum {
  CReqAlignedAlloc_Post_Heap_Allocated,
  CReqAlignedAlloc_Post_Heap_Nop,
  CReqAlignedAlloc_Post_Heap_NA
} CReqAlignedAlloc_Post_Heap;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Alignment_NA : 1;
  uint16_t Pre_Size_NA : 1;
  uint16_t Pre_Fit_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Errno : 2;
  uint16_t Post_Alignment : 1;
  uint16_t Post_Size : 1;
  uint16_t Post_Heap : 2;
} CReqAlignedAlloc_Entry;

#define ERRNO_SENTINEL -1

/**
 * @brief Test context for spec:/c/req/aligned-alloc test case.
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
   * @brief This member contains the return value of the aligned_alloc() call.
   */
  void *ptr;

  /**
   * @brief This member contains the value of errno after the aligned_alloc()
   *   call.
   */
  int errno_value;

  /**
   * @brief This member specifies the `alignment` parameter value.
   */
  size_t alignment;

  /**
   * @brief This member specifies the `size` parameter value.
   */
  size_t size;

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
    bool rep_ran[ 31 ];

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
    CReqAlignedAlloc_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CReqAlignedAlloc_Context;

static CReqAlignedAlloc_Context CReqAlignedAlloc_Instance;

static const char *const CReqAlignedAlloc_PreDesc_Alignment[] = {
  "Zero",
  "NotPower2Three",
  "NotPower2PointerPlusOne",
  "NotPower2HeapPlusOne",
  "NotPower2TwiceHeapMinusOne",
  "NotPower2SizeMax",
  "Power2One",
  "Power2Two",
  "Power2Pointer",
  "Power2Heap",
  "Power2TwiceHeap",
  "Power2Slab",
  "NA"
};

static const char *const CReqAlignedAlloc_PreDesc_Size[] = {
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

static const char *const CReqAlignedAlloc_PreDesc_Fit[] =
  { "YesEmpty", "YesExact", "NoExhausted", "NoShort", "NA" };

static const char *const *const CReqAlignedAlloc_PreDesc[] = {
  CReqAlignedAlloc_PreDesc_Alignment,
  CReqAlignedAlloc_PreDesc_Size,
  CReqAlignedAlloc_PreDesc_Fit,
  NULL
};

typedef enum {
  CReqAlignedAlloc_Pre_Alignment_NotPower2_Three,
  CReqAlignedAlloc_Pre_Alignment_NotPower2_PointerPlusOne,
  CReqAlignedAlloc_Pre_Alignment_NotPower2_HeapPlusOne,
  CReqAlignedAlloc_Pre_Alignment_NotPower2_TwiceHeapMinusOne,
  CReqAlignedAlloc_Pre_Alignment_NotPower2_SizeMax
} CReqAlignedAlloc_Pre_Alignment_NotPower2_Rep;

typedef enum {
  CReqAlignedAlloc_Pre_Alignment_Power2_One,
  CReqAlignedAlloc_Pre_Alignment_Power2_Two,
  CReqAlignedAlloc_Pre_Alignment_Power2_Pointer,
  CReqAlignedAlloc_Pre_Alignment_Power2_Heap,
  CReqAlignedAlloc_Pre_Alignment_Power2_TwiceHeap,
  CReqAlignedAlloc_Pre_Alignment_Power2_Slab
} CReqAlignedAlloc_Pre_Alignment_Power2_Rep;

typedef enum {
  CReqAlignedAlloc_Pre_Size_NonZero_One,
  CReqAlignedAlloc_Pre_Size_NonZero_MinMinusOne,
  CReqAlignedAlloc_Pre_Size_NonZero_Min,
  CReqAlignedAlloc_Pre_Size_NonZero_MinPlusOne,
  CReqAlignedAlloc_Pre_Size_NonZero_UnitMinusOne,
  CReqAlignedAlloc_Pre_Size_NonZero_Unit,
  CReqAlignedAlloc_Pre_Size_NonZero_UnitPlusOne,
  CReqAlignedAlloc_Pre_Size_NonZero_Slab,
  CReqAlignedAlloc_Pre_Size_NonZero_Max,
  CReqAlignedAlloc_Pre_Size_NonZero_AboveMax,
  CReqAlignedAlloc_Pre_Size_NonZero_SizeMax
} CReqAlignedAlloc_Pre_Size_NonZero_Rep;

typedef enum {
  CReqAlignedAlloc_Pre_Fit_Yes_Empty,
  CReqAlignedAlloc_Pre_Fit_Yes_Exact
} CReqAlignedAlloc_Pre_Fit_Yes_Rep;

typedef enum {
  CReqAlignedAlloc_Pre_Fit_No_Exhausted,
  CReqAlignedAlloc_Pre_Fit_No_Short
} CReqAlignedAlloc_Pre_Fit_No_Rep;

static const uint8_t CReqAlignedAlloc_PreRepOffset_Alignment[] =
  { 0, 1, 6, 12, 13 };

static const uint8_t CReqAlignedAlloc_PreRepOffset_Size[] = { 0, 1, 12, 13 };

static const uint8_t CReqAlignedAlloc_PreRepOffset_Fit[] = { 0, 2, 4, 5 };

static const uint8_t *const CReqAlignedAlloc_PreRepOffset[] = {
  CReqAlignedAlloc_PreRepOffset_Alignment,
  CReqAlignedAlloc_PreRepOffset_Size,
  CReqAlignedAlloc_PreRepOffset_Fit
};

static size_t CReqAlignedAlloc_GetRepresentativeCountOf(
  const CReqAlignedAlloc_Context *ctx,
  size_t                          index
)
{
  const uint8_t *offsets;
  size_t         state;

  offsets = CReqAlignedAlloc_PreRepOffset[ index ];
  state = ctx->Map.pcs[ index ];

  return offsets[ state + 1 ] - offsets[ state ];
}

static size_t CReqAlignedAlloc_GetRepresentative(
  const CReqAlignedAlloc_Context *ctx,
  size_t                          index
)
{
  size_t count;

  count = CReqAlignedAlloc_GetRepresentativeCountOf( ctx, index );

  if ( ctx->Map.rep < count ) {
    return ctx->Map.rep;
  }

  return count - 1;
}

static size_t CReqAlignedAlloc_GetRepresentativeCount(
  const CReqAlignedAlloc_Context *ctx
)
{
  size_t count;
  size_t i;

  count = 1;

  for ( i = 0; i < 3; ++i ) {
    size_t count_of_condition;

    count_of_condition = CReqAlignedAlloc_GetRepresentativeCountOf( ctx, i );

    if ( count_of_condition > count ) {
      count = count_of_condition;
    }
  }

  return count;
}

static CReqAlignedAlloc_Pre_Alignment_NotPower2_Rep
CReqAlignedAlloc_Pre_Alignment_NotPower2_GetRepresentative(
  const CReqAlignedAlloc_Context *ctx
)
{
  return (
    CReqAlignedAlloc_Pre_Alignment_NotPower2_Rep
  ) CReqAlignedAlloc_GetRepresentative( ctx, 0 );
}

static CReqAlignedAlloc_Pre_Alignment_Power2_Rep
CReqAlignedAlloc_Pre_Alignment_Power2_GetRepresentative(
  const CReqAlignedAlloc_Context *ctx
)
{
  return (
    CReqAlignedAlloc_Pre_Alignment_Power2_Rep
  ) CReqAlignedAlloc_GetRepresentative( ctx, 0 );
}

static CReqAlignedAlloc_Pre_Size_NonZero_Rep
CReqAlignedAlloc_Pre_Size_NonZero_GetRepresentative(
  const CReqAlignedAlloc_Context *ctx
)
{
  return (
    CReqAlignedAlloc_Pre_Size_NonZero_Rep
  ) CReqAlignedAlloc_GetRepresentative( ctx, 1 );
}

static CReqAlignedAlloc_Pre_Fit_Yes_Rep
CReqAlignedAlloc_Pre_Fit_Yes_GetRepresentative(
  const CReqAlignedAlloc_Context *ctx
)
{
  return (
    CReqAlignedAlloc_Pre_Fit_Yes_Rep
  ) CReqAlignedAlloc_GetRepresentative( ctx, 2 );
}

static CReqAlignedAlloc_Pre_Fit_No_Rep
CReqAlignedAlloc_Pre_Fit_No_GetRepresentative(
  const CReqAlignedAlloc_Context *ctx
)
{
  return (
    CReqAlignedAlloc_Pre_Fit_No_Rep
  ) CReqAlignedAlloc_GetRepresentative( ctx, 2 );
}

static const uint8_t CReqAlignedAlloc_PreRepBase[] = { 0, 13, 26 };

static const uint8_t CReqAlignedAlloc_PreRepStates[] = { 3, 2, 2 };

static void CReqAlignedAlloc_ResetRepresentatives(
  CReqAlignedAlloc_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 31; ++i ) {
    ctx->Map.rep_ran[ i ] = false;
  }
}

static void CReqAlignedAlloc_MarkRepresentatives(
  CReqAlignedAlloc_Context *ctx
)
{
  size_t i;

  if ( ctx->Map.skip_rep ) {
    return;
  }

  for ( i = 0; i < 3; ++i ) {
    size_t rep;

    rep = CReqAlignedAlloc_PreRepBase[ i ] +
          CReqAlignedAlloc_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
          CReqAlignedAlloc_GetRepresentative( ctx, i );
    ctx->Map.rep_ran[ rep ] = true;
  }
}

static void CReqAlignedAlloc_CheckRepresentatives(
  const CReqAlignedAlloc_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 3; ++i ) {
    const uint8_t *offsets;
    const bool    *ran;
    size_t         state;

    offsets = CReqAlignedAlloc_PreRepOffset[ i ];
    ran = &ctx->Map.rep_ran[ CReqAlignedAlloc_PreRepBase[ i ] ];

    for ( state = 0; state < CReqAlignedAlloc_PreRepStates[ i ]; ++state ) {
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
            CReqAlignedAlloc_PreDesc[ i ][ k ]
          );
        }
      }
    }
  }
}

static void CReqAlignedAlloc_Pre_Alignment_Prepare(
  CReqAlignedAlloc_Context      *ctx,
  CReqAlignedAlloc_Pre_Alignment state
)
{
  switch ( state ) {
    case CReqAlignedAlloc_Pre_Alignment_Zero: {
      /*
       * While the `alignment` parameter is equal to zero.
       */
      ctx->alignment = 0;
      break;
    }

    case CReqAlignedAlloc_Pre_Alignment_NotPower2: {
      /*
       * While the `alignment` parameter is not equal to zero, while the
       * `alignment` parameter is not a power of two.
       */
      switch (
        CReqAlignedAlloc_Pre_Alignment_NotPower2_GetRepresentative( ctx )
      ) {
        case CReqAlignedAlloc_Pre_Alignment_NotPower2_Three: {
          ctx->alignment = 3;
          break;
        }

        case CReqAlignedAlloc_Pre_Alignment_NotPower2_PointerPlusOne: {
          ctx->alignment = sizeof( void * ) + 1;
          break;
        }

        case CReqAlignedAlloc_Pre_Alignment_NotPower2_HeapPlusOne: {
          ctx->alignment = RBHEAP_ALIGNMENT + 1;
          break;
        }

        case CReqAlignedAlloc_Pre_Alignment_NotPower2_TwiceHeapMinusOne: {
          ctx->alignment = 2 * RBHEAP_ALIGNMENT - 1;
          break;
        }

        case CReqAlignedAlloc_Pre_Alignment_NotPower2_SizeMax: {
          ctx->alignment = SIZE_MAX;
          break;
        }
      }
      break;
    }

    case CReqAlignedAlloc_Pre_Alignment_Power2: {
      /*
       * While the `alignment` parameter is a power of two.
       */
      switch (
        CReqAlignedAlloc_Pre_Alignment_Power2_GetRepresentative( ctx )
      ) {
        case CReqAlignedAlloc_Pre_Alignment_Power2_One: {
          ctx->alignment = 1;
          break;
        }

        case CReqAlignedAlloc_Pre_Alignment_Power2_Two: {
          ctx->alignment = 2;
          break;
        }

        case CReqAlignedAlloc_Pre_Alignment_Power2_Pointer: {
          ctx->alignment = sizeof( void * );
          break;
        }

        case CReqAlignedAlloc_Pre_Alignment_Power2_Heap: {
          ctx->alignment = RBHEAP_ALIGNMENT;
          break;
        }

        case CReqAlignedAlloc_Pre_Alignment_Power2_TwiceHeap: {
          ctx->alignment = 2 * RBHEAP_ALIGNMENT;
          break;
        }

        case CReqAlignedAlloc_Pre_Alignment_Power2_Slab: {
          ctx->alignment = RBHEAP_SLAB_SIZE;
          break;
        }
      }
      break;
    }

    case CReqAlignedAlloc_Pre_Alignment_NA:
      break;
  }
}

static void CReqAlignedAlloc_Pre_Size_Prepare(
  CReqAlignedAlloc_Context *ctx,
  CReqAlignedAlloc_Pre_Size state
)
{
  switch ( state ) {
    case CReqAlignedAlloc_Pre_Size_Zero: {
      /*
       * While the `size` parameter is equal to zero.
       */
      ctx->size = 0;
      break;
    }

    case CReqAlignedAlloc_Pre_Size_NonZero: {
      /*
       * While the `size` parameter is not equal to zero.
       */
      switch ( CReqAlignedAlloc_Pre_Size_NonZero_GetRepresentative( ctx ) ) {
        case CReqAlignedAlloc_Pre_Size_NonZero_One: {
          ctx->size = 1;
          break;
        }

        case CReqAlignedAlloc_Pre_Size_NonZero_MinMinusOne: {
          ctx->size = RBHEAP_MIN_SIZE - 1;
          break;
        }

        case CReqAlignedAlloc_Pre_Size_NonZero_Min: {
          ctx->size = RBHEAP_MIN_SIZE;
          break;
        }

        case CReqAlignedAlloc_Pre_Size_NonZero_MinPlusOne: {
          ctx->size = RBHEAP_MIN_SIZE + 1;
          break;
        }

        case CReqAlignedAlloc_Pre_Size_NonZero_UnitMinusOne: {
          ctx->size = RBHEAP_MIN_SIZE + RBHEAP_ALIGNMENT - 1;
          break;
        }

        case CReqAlignedAlloc_Pre_Size_NonZero_Unit: {
          ctx->size = RBHEAP_MIN_SIZE + RBHEAP_ALIGNMENT;
          break;
        }

        case CReqAlignedAlloc_Pre_Size_NonZero_UnitPlusOne: {
          ctx->size = RBHEAP_MIN_SIZE + RBHEAP_ALIGNMENT + 1;
          break;
        }

        case CReqAlignedAlloc_Pre_Size_NonZero_Slab: {
          ctx->size = RBHEAP_SLAB_SIZE;
          break;
        }

        case CReqAlignedAlloc_Pre_Size_NonZero_Max: {
          ctx->size = RBHEAP_MAX_SIZE;
          break;
        }

        case CReqAlignedAlloc_Pre_Size_NonZero_AboveMax: {
          ctx->size = RBHEAP_MAX_SIZE + 1;
          break;
        }

        case CReqAlignedAlloc_Pre_Size_NonZero_SizeMax: {
          ctx->size = SIZE_MAX;
          break;
        }
      }
      break;
    }

    case CReqAlignedAlloc_Pre_Size_NA:
      break;
  }
}

static void CReqAlignedAlloc_Pre_Fit_Prepare(
  CReqAlignedAlloc_Context *ctx,
  CReqAlignedAlloc_Pre_Fit  state
)
{
  switch ( state ) {
    case CReqAlignedAlloc_Pre_Fit_Yes: {
      /*
       * While the C Program Heap has a free block which satisfies the request.
       */
      switch ( CReqAlignedAlloc_Pre_Fit_Yes_GetRepresentative( ctx ) ) {
        case CReqAlignedAlloc_Pre_Fit_Yes_Empty: {
          if ( ctx->size > HeapGetLargestAllocatableSize( &ctx->heap.heap ) ) {
            ctx->Map.skip_rep = true;
          }
          break;
        }

        case CReqAlignedAlloc_Pre_Fit_Yes_Exact: {
          if ( ctx->size > HeapGetLargestAllocatableSize( &ctx->heap.heap ) ) {
            ctx->Map.skip_rep = true;
          } else {
            HeapLeaveFreeBlock(
              &ctx->heap.heap,
              ctx->size,
              HeapFitAlignment( ctx->alignment )
            );
          }
          break;
        }
      }
      break;
    }

    case CReqAlignedAlloc_Pre_Fit_No: {
      /*
       * While the C Program Heap has no free block which satisfies the
       * request.
       */
      switch ( CReqAlignedAlloc_Pre_Fit_No_GetRepresentative( ctx ) ) {
        case CReqAlignedAlloc_Pre_Fit_No_Exhausted: {
          HeapExhaust( &ctx->heap.heap );
          break;
        }

        case CReqAlignedAlloc_Pre_Fit_No_Short: {
          if (
            ctx->size >= RBHEAP_MIN_SIZE + RBHEAP_ALIGNMENT &&
            ctx->size <= HeapGetLargestAllocatableSize( &ctx->heap.heap )
          ) {
            HeapLeaveFreeBlock(
              &ctx->heap.heap,
              ctx->size - RBHEAP_ALIGNMENT,
              HeapFitAlignment( ctx->alignment )
            );
          } else {
            HeapExhaust( &ctx->heap.heap );
          }
          break;
        }
      }
      break;
    }

    case CReqAlignedAlloc_Pre_Fit_NA:
      break;
  }
}

static void CReqAlignedAlloc_Post_Status_Check(
  CReqAlignedAlloc_Context    *ctx,
  CReqAlignedAlloc_Post_Status state
)
{
  switch ( state ) {
    case CReqAlignedAlloc_Post_Status_Block: {
      /*
       * The return value of aligned_alloc() shall be the begin address of a
       * used block of the C Program Heap.
       */
      T_not_null( ctx->ptr );
      break;
    }

    case CReqAlignedAlloc_Post_Status_Null: {
      /*
       * The return value of aligned_alloc() shall be equal to NULL.
       */
      T_null( ctx->ptr );
      break;
    }

    case CReqAlignedAlloc_Post_Status_NA:
      break;
  }
}

static void CReqAlignedAlloc_Post_Errno_Check(
  CReqAlignedAlloc_Context   *ctx,
  CReqAlignedAlloc_Post_Errno state
)
{
  switch ( state ) {
    case CReqAlignedAlloc_Post_Errno_Nop: {
      /*
       * The value of errno shall not be changed by the aligned_alloc() call.
       */
      T_eq_int( ctx->errno_value, ERRNO_SENTINEL );
      break;
    }

    case CReqAlignedAlloc_Post_Errno_EINVAL: {
      /*
       * The value of errno shall be equal to EINVAL.
       */
      T_eq_int( ctx->errno_value, EINVAL );
      break;
    }

    case CReqAlignedAlloc_Post_Errno_ENOMEM: {
      /*
       * The value of errno shall be equal to ENOMEM.
       */
      T_eq_int( ctx->errno_value, ENOMEM );
      break;
    }

    case CReqAlignedAlloc_Post_Errno_NA:
      break;
  }
}

static void CReqAlignedAlloc_Post_Alignment_Check(
  CReqAlignedAlloc_Context       *ctx,
  CReqAlignedAlloc_Post_Alignment state
)
{
  switch ( state ) {
    case CReqAlignedAlloc_Post_Alignment_Valid: {
      /*
       * The begin address of the used block shall be an integral multiple of
       * the `alignment` parameter.
       */
      T_eq_uptr( (uintptr_t) ctx->ptr % ctx->alignment, 0 );
      break;
    }

    case CReqAlignedAlloc_Post_Alignment_NA:
      break;
  }
}

static void CReqAlignedAlloc_Post_Size_Check(
  CReqAlignedAlloc_Context  *ctx,
  CReqAlignedAlloc_Post_Size state
)
{
  uintptr_t size;

  switch ( state ) {
    case CReqAlignedAlloc_Post_Size_Valid: {
      /*
       * The size of the used block shall be greater than or equal to the
       * `size` parameter.
       */
      T_true( _RBHeap_Size_of_block( &ctx->heap.heap, ctx->ptr, &size ) );
      T_ge_uptr( size, ctx->size );
      break;
    }

    case CReqAlignedAlloc_Post_Size_NA:
      break;
  }
}

static void CReqAlignedAlloc_Post_Heap_Check(
  CReqAlignedAlloc_Context  *ctx,
  CReqAlignedAlloc_Post_Heap state
)
{
  switch ( state ) {
    case CReqAlignedAlloc_Post_Heap_Allocated: {
      /*
       * The C Program Heap shall have one more used block.
       */
      T_eq_u32( ctx->after.used_blocks, ctx->before.used_blocks + 1 );
      break;
    }

    case CReqAlignedAlloc_Post_Heap_Nop: {
      /*
       * The used blocks and the free blocks of the C Program Heap shall not be
       * changed by the aligned_alloc() call.
       */
      T_eq_u32( ctx->after.used_blocks, ctx->before.used_blocks );
      T_eq_u32( ctx->after.free_blocks, ctx->before.free_blocks );
      T_eq_uptr( ctx->after.free_size, ctx->before.free_size );
      break;
    }

    case CReqAlignedAlloc_Post_Heap_NA:
      break;
  }
}

static void CReqAlignedAlloc_Setup( CReqAlignedAlloc_Context *ctx )
{
  HeapInstall( &ctx->heap );
}

static void CReqAlignedAlloc_Setup_Wrap( void *arg )
{
  CReqAlignedAlloc_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CReqAlignedAlloc_Setup( ctx );
}

static void CReqAlignedAlloc_Teardown( CReqAlignedAlloc_Context *ctx )
{
  HeapRestore( &ctx->heap );
}

static void CReqAlignedAlloc_Teardown_Wrap( void *arg )
{
  CReqAlignedAlloc_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CReqAlignedAlloc_Teardown( ctx );
}

static void CReqAlignedAlloc_Prepare( CReqAlignedAlloc_Context *ctx )
{
  HeapReset( &ctx->heap );
}

static void CReqAlignedAlloc_Action( CReqAlignedAlloc_Context *ctx )
{
  HeapTakeSnapshot( &ctx->heap.heap, &ctx->before );
  errno = ERRNO_SENTINEL;
  ctx->ptr = aligned_alloc( ctx->alignment, ctx->size );
  ctx->errno_value = errno;
  HeapTakeSnapshot( &ctx->heap.heap, &ctx->after );
}

/* clang-format off */

static const CReqAlignedAlloc_Entry
CReqAlignedAlloc_Entries[] = {
  { 0, 0, 0, 0, CReqAlignedAlloc_Post_Status_Null,
    CReqAlignedAlloc_Post_Errno_EINVAL, CReqAlignedAlloc_Post_Alignment_NA,
    CReqAlignedAlloc_Post_Size_NA, CReqAlignedAlloc_Post_Heap_Nop },
  { 0, 0, 0, 0, CReqAlignedAlloc_Post_Status_Null,
    CReqAlignedAlloc_Post_Errno_Nop, CReqAlignedAlloc_Post_Alignment_NA,
    CReqAlignedAlloc_Post_Size_NA, CReqAlignedAlloc_Post_Heap_Nop },
  { 0, 0, 0, 0, CReqAlignedAlloc_Post_Status_Block,
    CReqAlignedAlloc_Post_Errno_Nop, CReqAlignedAlloc_Post_Alignment_Valid,
    CReqAlignedAlloc_Post_Size_Valid, CReqAlignedAlloc_Post_Heap_Allocated },
  { 0, 0, 0, 0, CReqAlignedAlloc_Post_Status_Null,
    CReqAlignedAlloc_Post_Errno_ENOMEM, CReqAlignedAlloc_Post_Alignment_NA,
    CReqAlignedAlloc_Post_Size_NA, CReqAlignedAlloc_Post_Heap_Nop }
};

static const uint8_t
CReqAlignedAlloc_Map[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 3
};

/* clang-format on */

static size_t CReqAlignedAlloc_Scope( void *arg, char *buf, size_t n )
{
  CReqAlignedAlloc_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    size_t pcd[ 3 ];
    size_t i;

    for ( i = 0; i < 3; ++i ) {
      pcd[ i ] = CReqAlignedAlloc_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
                 CReqAlignedAlloc_GetRepresentative( ctx, i );
    }

    return T_get_scope( CReqAlignedAlloc_PreDesc, buf, n, pcd );
  }

  return 0;
}

static T_fixture CReqAlignedAlloc_Fixture = {
  .setup = CReqAlignedAlloc_Setup_Wrap,
  .stop = NULL,
  .teardown = CReqAlignedAlloc_Teardown_Wrap,
  .scope = CReqAlignedAlloc_Scope,
  .initial_context = &CReqAlignedAlloc_Instance
};

static inline CReqAlignedAlloc_Entry CReqAlignedAlloc_PopEntry(
  CReqAlignedAlloc_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CReqAlignedAlloc_Entries[ CReqAlignedAlloc_Map[ index ] ];
}

static void CReqAlignedAlloc_TestVariant( CReqAlignedAlloc_Context *ctx )
{
  CReqAlignedAlloc_Pre_Alignment_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CReqAlignedAlloc_Pre_Size_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CReqAlignedAlloc_Pre_Fit_Prepare( ctx, ctx->Map.pcs[ 2 ] );

  if ( ctx->Map.skip_rep ) {
    return;
  }

  CReqAlignedAlloc_Action( ctx );
  CReqAlignedAlloc_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CReqAlignedAlloc_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
  CReqAlignedAlloc_Post_Alignment_Check( ctx, ctx->Map.entry.Post_Alignment );
  CReqAlignedAlloc_Post_Size_Check( ctx, ctx->Map.entry.Post_Size );
  CReqAlignedAlloc_Post_Heap_Check( ctx, ctx->Map.entry.Post_Heap );
}

/**
 * @fn void T_case_body_CReqAlignedAlloc( void )
 */
T_TEST_CASE_FIXTURE( CReqAlignedAlloc, &CReqAlignedAlloc_Fixture )
{
  CReqAlignedAlloc_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  CReqAlignedAlloc_ResetRepresentatives( ctx );

  for (
    ctx->Map.pcs[ 0 ] = CReqAlignedAlloc_Pre_Alignment_Zero;
    ctx->Map.pcs[ 0 ] < CReqAlignedAlloc_Pre_Alignment_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CReqAlignedAlloc_Pre_Size_Zero;
      ctx->Map.pcs[ 1 ] < CReqAlignedAlloc_Pre_Size_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = CReqAlignedAlloc_Pre_Fit_Yes;
        ctx->Map.pcs[ 2 ] < CReqAlignedAlloc_Pre_Fit_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = CReqAlignedAlloc_PopEntry( ctx );

        for (
          ctx->Map.rep = 0;
          ctx->Map.rep < CReqAlignedAlloc_GetRepresentativeCount( ctx );
          ++ctx->Map.rep
        ) {
          ctx->Map.skip_rep = false;
          CReqAlignedAlloc_Prepare( ctx );
          CReqAlignedAlloc_TestVariant( ctx );

          CReqAlignedAlloc_MarkRepresentatives( ctx );
        }
      }
    }
  }

  CReqAlignedAlloc_CheckRepresentatives( ctx );
}

/** @} */
