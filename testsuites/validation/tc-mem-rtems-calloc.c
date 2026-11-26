/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsMallocReqCalloc
 */

/*
 * Copyright (C) 2021, 2026 embedded brains GmbH & Co. KG
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

#include <stdint.h>
#include <string.h>
#include <rtems/malloc.h>

#include "tx-heap.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsMallocReqCalloc spec:/rtems/malloc/req/calloc
 *
 * @ingroup TestsuitesValidation0
 *
 * @{
 */

typedef enum {
  RtemsMallocReqCalloc_Pre_ElementCount_Zero,
  RtemsMallocReqCalloc_Pre_ElementCount_NonZero,
  RtemsMallocReqCalloc_Pre_ElementCount_NA
} RtemsMallocReqCalloc_Pre_ElementCount;

typedef enum {
  RtemsMallocReqCalloc_Pre_ElementSize_Zero,
  RtemsMallocReqCalloc_Pre_ElementSize_NonZero,
  RtemsMallocReqCalloc_Pre_ElementSize_NA
} RtemsMallocReqCalloc_Pre_ElementSize;

typedef enum {
  RtemsMallocReqCalloc_Pre_Product_Valid,
  RtemsMallocReqCalloc_Pre_Product_Overflow,
  RtemsMallocReqCalloc_Pre_Product_NA
} RtemsMallocReqCalloc_Pre_Product;

typedef enum {
  RtemsMallocReqCalloc_Pre_Fit_Yes,
  RtemsMallocReqCalloc_Pre_Fit_No,
  RtemsMallocReqCalloc_Pre_Fit_NA
} RtemsMallocReqCalloc_Pre_Fit;

typedef enum {
  RtemsMallocReqCalloc_Post_Status_Block,
  RtemsMallocReqCalloc_Post_Status_Null,
  RtemsMallocReqCalloc_Post_Status_NA
} RtemsMallocReqCalloc_Post_Status;

typedef enum {
  RtemsMallocReqCalloc_Post_Alignment_Heap,
  RtemsMallocReqCalloc_Post_Alignment_NA
} RtemsMallocReqCalloc_Post_Alignment;

typedef enum {
  RtemsMallocReqCalloc_Post_Content_Zero,
  RtemsMallocReqCalloc_Post_Content_NA
} RtemsMallocReqCalloc_Post_Content;

typedef enum {
  RtemsMallocReqCalloc_Post_Heap_Allocated,
  RtemsMallocReqCalloc_Post_Heap_Nop,
  RtemsMallocReqCalloc_Post_Heap_NA
} RtemsMallocReqCalloc_Post_Heap;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_ElementCount_NA : 1;
  uint16_t Pre_ElementSize_NA : 1;
  uint16_t Pre_Product_NA : 1;
  uint16_t Pre_Fit_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Alignment : 1;
  uint16_t Post_Content : 1;
  uint16_t Post_Heap : 2;
} RtemsMallocReqCalloc_Entry;

/**
 * @brief Test context for spec:/rtems/malloc/req/calloc test case.
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
   * @brief This member contains the return value of the rtems_calloc() call.
   */
  void *ptr;

  /**
   * @brief This member specifies the `nelem` parameter value.
   */
  size_t nelem;

  /**
   * @brief This member specifies the `elsize` parameter value.
   */
  size_t elsize;

  struct {
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
    bool rep_ran[ 24 ];

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
    RtemsMallocReqCalloc_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsMallocReqCalloc_Context;

static RtemsMallocReqCalloc_Context RtemsMallocReqCalloc_Instance;

static const char *const RtemsMallocReqCalloc_PreDesc_ElementCount[] = {
  "Zero",
  "NonZeroTwo",
  "NonZeroOne",
  "NonZeroLarge",
  "NonZeroSizeMax",
  "NA"
};

static const char *const RtemsMallocReqCalloc_PreDesc_ElementSize[] = {
  "Zero",
  "NonZeroOne",
  "NonZeroMinMinusOne",
  "NonZeroMin",
  "NonZeroMinPlusOne",
  "NonZeroSlab",
  "NonZeroSizeMax",
  "NA"
};

static const char *const RtemsMallocReqCalloc_PreDesc_Product[] =
  { "ValidGiven", "ValidLimit", "OverflowJustAbove", "OverflowMax", "NA" };

static const char *const RtemsMallocReqCalloc_PreDesc_Fit[] =
  { "YesEmpty", "YesExact", "NoExhausted", "NoShort", "NA" };

static const char *const *const RtemsMallocReqCalloc_PreDesc[] = {
  RtemsMallocReqCalloc_PreDesc_ElementCount,
  RtemsMallocReqCalloc_PreDesc_ElementSize,
  RtemsMallocReqCalloc_PreDesc_Product,
  RtemsMallocReqCalloc_PreDesc_Fit,
  NULL
};

typedef enum {
  RtemsMallocReqCalloc_Pre_ElementCount_NonZero_Two,
  RtemsMallocReqCalloc_Pre_ElementCount_NonZero_One,
  RtemsMallocReqCalloc_Pre_ElementCount_NonZero_Large,
  RtemsMallocReqCalloc_Pre_ElementCount_NonZero_SizeMax
} RtemsMallocReqCalloc_Pre_ElementCount_NonZero_Rep;

typedef enum {
  RtemsMallocReqCalloc_Pre_ElementSize_NonZero_One,
  RtemsMallocReqCalloc_Pre_ElementSize_NonZero_MinMinusOne,
  RtemsMallocReqCalloc_Pre_ElementSize_NonZero_Min,
  RtemsMallocReqCalloc_Pre_ElementSize_NonZero_MinPlusOne,
  RtemsMallocReqCalloc_Pre_ElementSize_NonZero_Slab,
  RtemsMallocReqCalloc_Pre_ElementSize_NonZero_SizeMax
} RtemsMallocReqCalloc_Pre_ElementSize_NonZero_Rep;

typedef enum {
  RtemsMallocReqCalloc_Pre_Product_Valid_Given,
  RtemsMallocReqCalloc_Pre_Product_Valid_Limit
} RtemsMallocReqCalloc_Pre_Product_Valid_Rep;

typedef enum {
  RtemsMallocReqCalloc_Pre_Product_Overflow_JustAbove,
  RtemsMallocReqCalloc_Pre_Product_Overflow_Max
} RtemsMallocReqCalloc_Pre_Product_Overflow_Rep;

typedef enum {
  RtemsMallocReqCalloc_Pre_Fit_Yes_Empty,
  RtemsMallocReqCalloc_Pre_Fit_Yes_Exact
} RtemsMallocReqCalloc_Pre_Fit_Yes_Rep;

typedef enum {
  RtemsMallocReqCalloc_Pre_Fit_No_Exhausted,
  RtemsMallocReqCalloc_Pre_Fit_No_Short
} RtemsMallocReqCalloc_Pre_Fit_No_Rep;

static const uint8_t RtemsMallocReqCalloc_PreRepOffset_ElementCount[] =
  { 0, 1, 5, 6 };

static const uint8_t RtemsMallocReqCalloc_PreRepOffset_ElementSize[] =
  { 0, 1, 7, 8 };

static const uint8_t RtemsMallocReqCalloc_PreRepOffset_Product[] =
  { 0, 2, 4, 5 };

static const uint8_t RtemsMallocReqCalloc_PreRepOffset_Fit[] = { 0, 2, 4, 5 };

static const uint8_t *const RtemsMallocReqCalloc_PreRepOffset[] = {
  RtemsMallocReqCalloc_PreRepOffset_ElementCount,
  RtemsMallocReqCalloc_PreRepOffset_ElementSize,
  RtemsMallocReqCalloc_PreRepOffset_Product,
  RtemsMallocReqCalloc_PreRepOffset_Fit
};

static size_t RtemsMallocReqCalloc_GetRepresentativeCountOf(
  const RtemsMallocReqCalloc_Context *ctx,
  size_t                              index
)
{
  const uint8_t *offsets;
  size_t         state;

  offsets = RtemsMallocReqCalloc_PreRepOffset[ index ];
  state = ctx->Map.pcs[ index ];

  return offsets[ state + 1 ] - offsets[ state ];
}

static size_t RtemsMallocReqCalloc_GetRepresentative(
  const RtemsMallocReqCalloc_Context *ctx,
  size_t                              index
)
{
  size_t count;

  count = RtemsMallocReqCalloc_GetRepresentativeCountOf( ctx, index );

  if ( ctx->Map.rep < count ) {
    return ctx->Map.rep;
  }

  return count - 1;
}

static size_t RtemsMallocReqCalloc_GetRepresentativeCount(
  const RtemsMallocReqCalloc_Context *ctx
)
{
  size_t count;
  size_t i;

  count = 1;

  for ( i = 0; i < 4; ++i ) {
    size_t count_of_condition;

    count_of_condition = RtemsMallocReqCalloc_GetRepresentativeCountOf(
      ctx,
      i
    );

    if ( count_of_condition > count ) {
      count = count_of_condition;
    }
  }

  return count;
}

static RtemsMallocReqCalloc_Pre_ElementCount_NonZero_Rep
RtemsMallocReqCalloc_Pre_ElementCount_NonZero_GetRepresentative(
  const RtemsMallocReqCalloc_Context *ctx
)
{
  return (
    RtemsMallocReqCalloc_Pre_ElementCount_NonZero_Rep
  ) RtemsMallocReqCalloc_GetRepresentative( ctx, 0 );
}

static RtemsMallocReqCalloc_Pre_ElementSize_NonZero_Rep
RtemsMallocReqCalloc_Pre_ElementSize_NonZero_GetRepresentative(
  const RtemsMallocReqCalloc_Context *ctx
)
{
  return (
    RtemsMallocReqCalloc_Pre_ElementSize_NonZero_Rep
  ) RtemsMallocReqCalloc_GetRepresentative( ctx, 1 );
}

static RtemsMallocReqCalloc_Pre_Product_Valid_Rep
RtemsMallocReqCalloc_Pre_Product_Valid_GetRepresentative(
  const RtemsMallocReqCalloc_Context *ctx
)
{
  return (
    RtemsMallocReqCalloc_Pre_Product_Valid_Rep
  ) RtemsMallocReqCalloc_GetRepresentative( ctx, 2 );
}

static RtemsMallocReqCalloc_Pre_Product_Overflow_Rep
RtemsMallocReqCalloc_Pre_Product_Overflow_GetRepresentative(
  const RtemsMallocReqCalloc_Context *ctx
)
{
  return (
    RtemsMallocReqCalloc_Pre_Product_Overflow_Rep
  ) RtemsMallocReqCalloc_GetRepresentative( ctx, 2 );
}

static RtemsMallocReqCalloc_Pre_Fit_Yes_Rep
RtemsMallocReqCalloc_Pre_Fit_Yes_GetRepresentative(
  const RtemsMallocReqCalloc_Context *ctx
)
{
  return (
    RtemsMallocReqCalloc_Pre_Fit_Yes_Rep
  ) RtemsMallocReqCalloc_GetRepresentative( ctx, 3 );
}

static RtemsMallocReqCalloc_Pre_Fit_No_Rep
RtemsMallocReqCalloc_Pre_Fit_No_GetRepresentative(
  const RtemsMallocReqCalloc_Context *ctx
)
{
  return (
    RtemsMallocReqCalloc_Pre_Fit_No_Rep
  ) RtemsMallocReqCalloc_GetRepresentative( ctx, 3 );
}

static const uint8_t RtemsMallocReqCalloc_PreRepBase[] = { 0, 6, 14, 19 };

static const uint8_t RtemsMallocReqCalloc_PreRepStates[] = { 2, 2, 2, 2 };

static void RtemsMallocReqCalloc_ResetRepresentatives(
  RtemsMallocReqCalloc_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 24; ++i ) {
    ctx->Map.rep_ran[ i ] = false;
  }
}

static void RtemsMallocReqCalloc_MarkRepresentatives(
  RtemsMallocReqCalloc_Context *ctx
)
{
  size_t i;

  if ( ctx->Map.skip_rep ) {
    return;
  }

  for ( i = 0; i < 4; ++i ) {
    size_t rep;

    rep = RtemsMallocReqCalloc_PreRepBase[ i ] +
          RtemsMallocReqCalloc_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
          RtemsMallocReqCalloc_GetRepresentative( ctx, i );
    ctx->Map.rep_ran[ rep ] = true;
  }
}

static void RtemsMallocReqCalloc_CheckRepresentatives(
  const RtemsMallocReqCalloc_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 4; ++i ) {
    const uint8_t *offsets;
    const bool    *ran;
    size_t         state;

    offsets = RtemsMallocReqCalloc_PreRepOffset[ i ];
    ran = &ctx->Map.rep_ran[ RtemsMallocReqCalloc_PreRepBase[ i ] ];

    for (
      state = 0; state < RtemsMallocReqCalloc_PreRepStates[ i ]; ++state
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
            RtemsMallocReqCalloc_PreDesc[ i ][ k ]
          );
        }
      }
    }
  }
}

static bool IsZero( const void *ptr, size_t size )
{
  const unsigned char *bytes;
  size_t               i;

  bytes = ptr;

  for ( i = 0; i < size; ++i ) {
    if ( bytes[ i ] != 0 ) {
      return false;
    }
  }

  return true;
}

static void RtemsMallocReqCalloc_Pre_ElementCount_Prepare(
  RtemsMallocReqCalloc_Context         *ctx,
  RtemsMallocReqCalloc_Pre_ElementCount state
)
{
  switch ( state ) {
    case RtemsMallocReqCalloc_Pre_ElementCount_Zero: {
      /*
       * While the `nelem` parameter is equal to zero.
       */
      ctx->nelem = 0;
      break;
    }

    case RtemsMallocReqCalloc_Pre_ElementCount_NonZero: {
      /*
       * While the `nelem` parameter is not equal to zero.
       */
      switch (
        RtemsMallocReqCalloc_Pre_ElementCount_NonZero_GetRepresentative( ctx )
      ) {
        case RtemsMallocReqCalloc_Pre_ElementCount_NonZero_Two: {
          ctx->nelem = 2;
          break;
        }

        case RtemsMallocReqCalloc_Pre_ElementCount_NonZero_One: {
          ctx->nelem = 1;
          break;
        }

        case RtemsMallocReqCalloc_Pre_ElementCount_NonZero_Large: {
          ctx->nelem = 65536;
          break;
        }

        case RtemsMallocReqCalloc_Pre_ElementCount_NonZero_SizeMax: {
          ctx->nelem = SIZE_MAX;
          break;
        }
      }
      break;
    }

    case RtemsMallocReqCalloc_Pre_ElementCount_NA:
      break;
  }
}

static void RtemsMallocReqCalloc_Pre_ElementSize_Prepare(
  RtemsMallocReqCalloc_Context        *ctx,
  RtemsMallocReqCalloc_Pre_ElementSize state
)
{
  switch ( state ) {
    case RtemsMallocReqCalloc_Pre_ElementSize_Zero: {
      /*
       * While the `elsize` parameter is equal to zero.
       */
      ctx->elsize = 0;
      break;
    }

    case RtemsMallocReqCalloc_Pre_ElementSize_NonZero: {
      /*
       * While the `elsize` parameter is not equal to zero.
       */
      switch (
        RtemsMallocReqCalloc_Pre_ElementSize_NonZero_GetRepresentative( ctx )
      ) {
        case RtemsMallocReqCalloc_Pre_ElementSize_NonZero_One: {
          ctx->elsize = 1;
          break;
        }

        case RtemsMallocReqCalloc_Pre_ElementSize_NonZero_MinMinusOne: {
          ctx->elsize = RBHEAP_MIN_SIZE - 1;
          break;
        }

        case RtemsMallocReqCalloc_Pre_ElementSize_NonZero_Min: {
          ctx->elsize = RBHEAP_MIN_SIZE;
          break;
        }

        case RtemsMallocReqCalloc_Pre_ElementSize_NonZero_MinPlusOne: {
          ctx->elsize = RBHEAP_MIN_SIZE + 1;
          break;
        }

        case RtemsMallocReqCalloc_Pre_ElementSize_NonZero_Slab: {
          ctx->elsize = RBHEAP_SLAB_SIZE;
          break;
        }

        case RtemsMallocReqCalloc_Pre_ElementSize_NonZero_SizeMax: {
          ctx->elsize = SIZE_MAX;
          break;
        }
      }
      break;
    }

    case RtemsMallocReqCalloc_Pre_ElementSize_NA:
      break;
  }
}

static void RtemsMallocReqCalloc_Pre_Product_Prepare(
  RtemsMallocReqCalloc_Context    *ctx,
  RtemsMallocReqCalloc_Pre_Product state
)
{
  switch ( state ) {
    case RtemsMallocReqCalloc_Pre_Product_Valid: {
      /*
       * While the product of the `nelem` and the `elsize` parameters is less
       * than or equal to SIZE_MAX.
       */
      switch (
        RtemsMallocReqCalloc_Pre_Product_Valid_GetRepresentative( ctx )
      ) {
        case RtemsMallocReqCalloc_Pre_Product_Valid_Given: {
          if ( ctx->nelem != 0 && ctx->elsize > SIZE_MAX / ctx->nelem ) {
            ctx->Map.skip_rep = true;
          }
          break;
        }

        case RtemsMallocReqCalloc_Pre_Product_Valid_Limit: {
          if ( ctx->nelem == 0 || ctx->elsize == 0 ) {
            ctx->Map.skip_rep = true;
          } else {
            ctx->elsize = SIZE_MAX / ctx->nelem;
          }
          break;
        }
      }
      break;
    }

    case RtemsMallocReqCalloc_Pre_Product_Overflow: {
      /*
       * While the product of the `nelem` and the `elsize` parameters is
       * greater than SIZE_MAX.
       */
      switch (
        RtemsMallocReqCalloc_Pre_Product_Overflow_GetRepresentative( ctx )
      ) {
        case RtemsMallocReqCalloc_Pre_Product_Overflow_JustAbove: {
          if ( ctx->nelem < 2 ) {
            ctx->Map.skip_rep = true;
          } else {
            ctx->elsize = SIZE_MAX / ctx->nelem + 1;
          }
          break;
        }

        case RtemsMallocReqCalloc_Pre_Product_Overflow_Max: {
          if ( ctx->nelem < 2 ) {
            ctx->Map.skip_rep = true;
          } else {
            ctx->elsize = SIZE_MAX;
          }
          break;
        }
      }
      break;
    }

    case RtemsMallocReqCalloc_Pre_Product_NA:
      break;
  }
}

static void RtemsMallocReqCalloc_Pre_Fit_Prepare(
  RtemsMallocReqCalloc_Context *ctx,
  RtemsMallocReqCalloc_Pre_Fit  state
)
{
  switch ( state ) {
    case RtemsMallocReqCalloc_Pre_Fit_Yes: {
      /*
       * While the C Program Heap has a free block which satisfies the request.
       */
      switch ( RtemsMallocReqCalloc_Pre_Fit_Yes_GetRepresentative( ctx ) ) {
        case RtemsMallocReqCalloc_Pre_Fit_Yes_Empty: {
          if (
            ctx->nelem * ctx->elsize >
            HeapGetLargestAllocatableSize( &ctx->heap.heap )
          ) {
            ctx->Map.skip_rep = true;
          }
          break;
        }

        case RtemsMallocReqCalloc_Pre_Fit_Yes_Exact: {
          if (
            ctx->nelem * ctx->elsize >
            HeapGetLargestAllocatableSize( &ctx->heap.heap )
          ) {
            ctx->Map.skip_rep = true;
          } else {
            HeapLeaveFreeBlock(
              &ctx->heap.heap,
              ctx->nelem * ctx->elsize,
              RBHEAP_ALIGNMENT
            );
          }
          break;
        }
      }
      break;
    }

    case RtemsMallocReqCalloc_Pre_Fit_No: {
      /*
       * While the C Program Heap has no free block which satisfies the
       * request.
       */
      switch ( RtemsMallocReqCalloc_Pre_Fit_No_GetRepresentative( ctx ) ) {
        case RtemsMallocReqCalloc_Pre_Fit_No_Exhausted: {
          HeapExhaust( &ctx->heap.heap );
          break;
        }

        case RtemsMallocReqCalloc_Pre_Fit_No_Short: {
          if (
            ctx->nelem * ctx->elsize >= RBHEAP_MIN_SIZE + RBHEAP_ALIGNMENT &&
            ctx->nelem * ctx->elsize <=
              HeapGetLargestAllocatableSize( &ctx->heap.heap )
          ) {
            HeapLeaveFreeBlock(
              &ctx->heap.heap,
              ctx->nelem * ctx->elsize - RBHEAP_ALIGNMENT,
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

    case RtemsMallocReqCalloc_Pre_Fit_NA:
      break;
  }
}

static void RtemsMallocReqCalloc_Post_Status_Check(
  RtemsMallocReqCalloc_Context    *ctx,
  RtemsMallocReqCalloc_Post_Status state
)
{
  switch ( state ) {
    case RtemsMallocReqCalloc_Post_Status_Block: {
      /*
       * The return value of rtems_calloc() shall be the begin address of a
       * used block of the C Program Heap.
       */
      T_not_null( ctx->ptr );
      break;
    }

    case RtemsMallocReqCalloc_Post_Status_Null: {
      /*
       * The return value of rtems_calloc() shall be equal to NULL.
       */
      T_null( ctx->ptr );
      break;
    }

    case RtemsMallocReqCalloc_Post_Status_NA:
      break;
  }
}

static void RtemsMallocReqCalloc_Post_Alignment_Check(
  RtemsMallocReqCalloc_Context       *ctx,
  RtemsMallocReqCalloc_Post_Alignment state
)
{
  switch ( state ) {
    case RtemsMallocReqCalloc_Post_Alignment_Heap: {
      /*
       * The begin address of the used block shall be an integral multiple of
       * the heap alignment of the target architecture.
       */
      T_eq_uptr( (uintptr_t) ctx->ptr % CPU_HEAP_ALIGNMENT, 0 );
      break;
    }

    case RtemsMallocReqCalloc_Post_Alignment_NA:
      break;
  }
}

static void RtemsMallocReqCalloc_Post_Content_Check(
  RtemsMallocReqCalloc_Context     *ctx,
  RtemsMallocReqCalloc_Post_Content state
)
{
  uintptr_t size;

  switch ( state ) {
    case RtemsMallocReqCalloc_Post_Content_Zero: {
      /*
       * The used block shall begin with `nelem` times `elsize` bytes of zero.
       */
      T_true( _RBHeap_Size_of_block( &ctx->heap.heap, ctx->ptr, &size ) );
      T_ge_uptr( size, ctx->nelem * ctx->elsize );
      T_true( IsZero( ctx->ptr, ctx->nelem * ctx->elsize ) );
      break;
    }

    case RtemsMallocReqCalloc_Post_Content_NA:
      break;
  }
}

static void RtemsMallocReqCalloc_Post_Heap_Check(
  RtemsMallocReqCalloc_Context  *ctx,
  RtemsMallocReqCalloc_Post_Heap state
)
{
  switch ( state ) {
    case RtemsMallocReqCalloc_Post_Heap_Allocated: {
      /*
       * The C Program Heap shall have one more used block.
       */
      T_eq_u32( ctx->after.used_blocks, ctx->before.used_blocks + 1 );
      break;
    }

    case RtemsMallocReqCalloc_Post_Heap_Nop: {
      /*
       * The used blocks and the free blocks of the C Program Heap shall not be
       * changed by the rtems_calloc() call.
       */
      T_eq_u32( ctx->after.used_blocks, ctx->before.used_blocks );
      T_eq_u32( ctx->after.free_blocks, ctx->before.free_blocks );
      T_eq_uptr( ctx->after.free_size, ctx->before.free_size );
      break;
    }

    case RtemsMallocReqCalloc_Post_Heap_NA:
      break;
  }
}

static void RtemsMallocReqCalloc_Setup( RtemsMallocReqCalloc_Context *ctx )
{
  HeapInstall( &ctx->heap );
}

static void RtemsMallocReqCalloc_Setup_Wrap( void *arg )
{
  RtemsMallocReqCalloc_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsMallocReqCalloc_Setup( ctx );
}

static void RtemsMallocReqCalloc_Teardown( RtemsMallocReqCalloc_Context *ctx )
{
  HeapRestore( &ctx->heap );
}

static void RtemsMallocReqCalloc_Teardown_Wrap( void *arg )
{
  RtemsMallocReqCalloc_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsMallocReqCalloc_Teardown( ctx );
}

static void RtemsMallocReqCalloc_Prepare( RtemsMallocReqCalloc_Context *ctx )
{
  memset( HeapGetArea(), 0xff, HEAP_AREA_SIZE );
  HeapReset( &ctx->heap );
}

static void RtemsMallocReqCalloc_Action( RtemsMallocReqCalloc_Context *ctx )
{
  HeapTakeSnapshot( &ctx->heap.heap, &ctx->before );
  ctx->ptr = rtems_calloc( ctx->nelem, ctx->elsize );
  HeapTakeSnapshot( &ctx->heap.heap, &ctx->after );
}

/* clang-format off */

static const RtemsMallocReqCalloc_Entry
RtemsMallocReqCalloc_Entries[] = {
  { 0, 0, 0, 0, 0, RtemsMallocReqCalloc_Post_Status_Null,
    RtemsMallocReqCalloc_Post_Alignment_NA,
    RtemsMallocReqCalloc_Post_Content_NA, RtemsMallocReqCalloc_Post_Heap_Nop },
  { 1, 0, 0, 0, 0, RtemsMallocReqCalloc_Post_Status_NA,
    RtemsMallocReqCalloc_Post_Alignment_NA,
    RtemsMallocReqCalloc_Post_Content_NA, RtemsMallocReqCalloc_Post_Heap_NA },
  { 0, 0, 0, 0, 0, RtemsMallocReqCalloc_Post_Status_Block,
    RtemsMallocReqCalloc_Post_Alignment_Heap,
    RtemsMallocReqCalloc_Post_Content_Zero,
    RtemsMallocReqCalloc_Post_Heap_Allocated }
};

static const uint8_t
RtemsMallocReqCalloc_Map[] = {
  0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 2, 0, 0, 0
};

/* clang-format on */

static size_t RtemsMallocReqCalloc_Scope( void *arg, char *buf, size_t n )
{
  RtemsMallocReqCalloc_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    size_t pcd[ 4 ];
    size_t i;

    for ( i = 0; i < 4; ++i ) {
      pcd[ i ] = RtemsMallocReqCalloc_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
                 RtemsMallocReqCalloc_GetRepresentative( ctx, i );
    }

    return T_get_scope( RtemsMallocReqCalloc_PreDesc, buf, n, pcd );
  }

  return 0;
}

static T_fixture RtemsMallocReqCalloc_Fixture = {
  .setup = RtemsMallocReqCalloc_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsMallocReqCalloc_Teardown_Wrap,
  .scope = RtemsMallocReqCalloc_Scope,
  .initial_context = &RtemsMallocReqCalloc_Instance
};

static inline RtemsMallocReqCalloc_Entry RtemsMallocReqCalloc_PopEntry(
  RtemsMallocReqCalloc_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsMallocReqCalloc_Entries[ RtemsMallocReqCalloc_Map[ index ] ];
}

static void RtemsMallocReqCalloc_TestVariant(
  RtemsMallocReqCalloc_Context *ctx
)
{
  RtemsMallocReqCalloc_Pre_ElementCount_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsMallocReqCalloc_Pre_ElementSize_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsMallocReqCalloc_Pre_Product_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsMallocReqCalloc_Pre_Fit_Prepare( ctx, ctx->Map.pcs[ 3 ] );

  if ( ctx->Map.skip_rep ) {
    return;
  }

  RtemsMallocReqCalloc_Action( ctx );
  RtemsMallocReqCalloc_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsMallocReqCalloc_Post_Alignment_Check(
    ctx,
    ctx->Map.entry.Post_Alignment
  );
  RtemsMallocReqCalloc_Post_Content_Check( ctx, ctx->Map.entry.Post_Content );
  RtemsMallocReqCalloc_Post_Heap_Check( ctx, ctx->Map.entry.Post_Heap );
}

/**
 * @fn void T_case_body_RtemsMallocReqCalloc( void )
 */
T_TEST_CASE_FIXTURE( RtemsMallocReqCalloc, &RtemsMallocReqCalloc_Fixture )
{
  RtemsMallocReqCalloc_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  RtemsMallocReqCalloc_ResetRepresentatives( ctx );

  for (
    ctx->Map.pcs[ 0 ] = RtemsMallocReqCalloc_Pre_ElementCount_Zero;
    ctx->Map.pcs[ 0 ] < RtemsMallocReqCalloc_Pre_ElementCount_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsMallocReqCalloc_Pre_ElementSize_Zero;
      ctx->Map.pcs[ 1 ] < RtemsMallocReqCalloc_Pre_ElementSize_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsMallocReqCalloc_Pre_Product_Valid;
        ctx->Map.pcs[ 2 ] < RtemsMallocReqCalloc_Pre_Product_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = RtemsMallocReqCalloc_Pre_Fit_Yes;
          ctx->Map.pcs[ 3 ] < RtemsMallocReqCalloc_Pre_Fit_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          ctx->Map.entry = RtemsMallocReqCalloc_PopEntry( ctx );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          for (
            ctx->Map.rep = 0;
            ctx->Map.rep < RtemsMallocReqCalloc_GetRepresentativeCount( ctx );
            ++ctx->Map.rep
          ) {
            ctx->Map.skip_rep = false;
            RtemsMallocReqCalloc_Prepare( ctx );
            RtemsMallocReqCalloc_TestVariant( ctx );

            RtemsMallocReqCalloc_MarkRepresentatives( ctx );
          }
        }
      }
    }
  }

  RtemsMallocReqCalloc_CheckRepresentatives( ctx );
}

/** @} */
