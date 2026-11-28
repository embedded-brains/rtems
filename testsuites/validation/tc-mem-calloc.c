/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CReqCalloc
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
#include <string.h>

#include "tx-heap.h"

#include <rtems/test.h>

/**
 * @defgroup CReqCalloc spec:/c/req/calloc
 *
 * @ingroup TestsuitesValidation0
 *
 * @{
 */

typedef enum {
  CReqCalloc_Pre_Nmemb_Zero,
  CReqCalloc_Pre_Nmemb_NonZero,
  CReqCalloc_Pre_Nmemb_NA
} CReqCalloc_Pre_Nmemb;

typedef enum {
  CReqCalloc_Pre_Size_Zero,
  CReqCalloc_Pre_Size_NonZero,
  CReqCalloc_Pre_Size_NA
} CReqCalloc_Pre_Size;

typedef enum {
  CReqCalloc_Pre_Product_Valid,
  CReqCalloc_Pre_Product_Overflow,
  CReqCalloc_Pre_Product_NA
} CReqCalloc_Pre_Product;

typedef enum {
  CReqCalloc_Pre_Fit_Yes,
  CReqCalloc_Pre_Fit_No,
  CReqCalloc_Pre_Fit_NA
} CReqCalloc_Pre_Fit;

typedef enum {
  CReqCalloc_Post_Status_Block,
  CReqCalloc_Post_Status_Null,
  CReqCalloc_Post_Status_NA
} CReqCalloc_Post_Status;

typedef enum {
  CReqCalloc_Post_Errno_Nop,
  CReqCalloc_Post_Errno_ENOMEM,
  CReqCalloc_Post_Errno_NA
} CReqCalloc_Post_Errno;

typedef enum {
  CReqCalloc_Post_Alignment_Heap,
  CReqCalloc_Post_Alignment_NA
} CReqCalloc_Post_Alignment;

typedef enum {
  CReqCalloc_Post_Content_Zero,
  CReqCalloc_Post_Content_NA
} CReqCalloc_Post_Content;

typedef enum {
  CReqCalloc_Post_Heap_Allocated,
  CReqCalloc_Post_Heap_Nop,
  CReqCalloc_Post_Heap_NA
} CReqCalloc_Post_Heap;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Nmemb_NA : 1;
  uint16_t Pre_Size_NA : 1;
  uint16_t Pre_Product_NA : 1;
  uint16_t Pre_Fit_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Errno : 2;
  uint16_t Post_Alignment : 1;
  uint16_t Post_Content : 1;
  uint16_t Post_Heap : 2;
} CReqCalloc_Entry;

#define ERRNO_SENTINEL -1

/**
 * @brief Test context for spec:/c/req/calloc test case.
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
   * @brief This member contains the return value of the calloc() call.
   */
  void *ptr;

  /**
   * @brief This member contains the value of errno after the calloc() call.
   */
  int errno_value;

  /**
   * @brief This member specifies the `nmemb` parameter value.
   */
  size_t nmemb;

  /**
   * @brief This member specifies the `size` parameter value.
   */
  size_t size;

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
    CReqCalloc_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CReqCalloc_Context;

static CReqCalloc_Context CReqCalloc_Instance;

static const char *const CReqCalloc_PreDesc_Nmemb[] = {
  "Zero",
  "NonZeroTwo",
  "NonZeroOne",
  "NonZeroLarge",
  "NonZeroSizeMax",
  "NA"
};

static const char *const CReqCalloc_PreDesc_Size[] = {
  "Zero",
  "NonZeroOne",
  "NonZeroMinMinusOne",
  "NonZeroMin",
  "NonZeroMinPlusOne",
  "NonZeroSlab",
  "NonZeroSizeMax",
  "NA"
};

static const char *const CReqCalloc_PreDesc_Product[] =
  { "ValidGiven", "ValidLimit", "OverflowJustAbove", "OverflowMax", "NA" };

static const char *const CReqCalloc_PreDesc_Fit[] =
  { "YesEmpty", "YesExact", "NoExhausted", "NoShort", "NA" };

static const char *const *const CReqCalloc_PreDesc[] = {
  CReqCalloc_PreDesc_Nmemb,
  CReqCalloc_PreDesc_Size,
  CReqCalloc_PreDesc_Product,
  CReqCalloc_PreDesc_Fit,
  NULL
};

typedef enum {
  CReqCalloc_Pre_Nmemb_NonZero_Two,
  CReqCalloc_Pre_Nmemb_NonZero_One,
  CReqCalloc_Pre_Nmemb_NonZero_Large,
  CReqCalloc_Pre_Nmemb_NonZero_SizeMax
} CReqCalloc_Pre_Nmemb_NonZero_Rep;

typedef enum {
  CReqCalloc_Pre_Size_NonZero_One,
  CReqCalloc_Pre_Size_NonZero_MinMinusOne,
  CReqCalloc_Pre_Size_NonZero_Min,
  CReqCalloc_Pre_Size_NonZero_MinPlusOne,
  CReqCalloc_Pre_Size_NonZero_Slab,
  CReqCalloc_Pre_Size_NonZero_SizeMax
} CReqCalloc_Pre_Size_NonZero_Rep;

typedef enum {
  CReqCalloc_Pre_Product_Valid_Given,
  CReqCalloc_Pre_Product_Valid_Limit
} CReqCalloc_Pre_Product_Valid_Rep;

typedef enum {
  CReqCalloc_Pre_Product_Overflow_JustAbove,
  CReqCalloc_Pre_Product_Overflow_Max
} CReqCalloc_Pre_Product_Overflow_Rep;

typedef enum {
  CReqCalloc_Pre_Fit_Yes_Empty,
  CReqCalloc_Pre_Fit_Yes_Exact
} CReqCalloc_Pre_Fit_Yes_Rep;

typedef enum {
  CReqCalloc_Pre_Fit_No_Exhausted,
  CReqCalloc_Pre_Fit_No_Short
} CReqCalloc_Pre_Fit_No_Rep;

static const uint8_t CReqCalloc_PreRepOffset_Nmemb[] = { 0, 1, 5, 6 };

static const uint8_t CReqCalloc_PreRepOffset_Size[] = { 0, 1, 7, 8 };

static const uint8_t CReqCalloc_PreRepOffset_Product[] = { 0, 2, 4, 5 };

static const uint8_t CReqCalloc_PreRepOffset_Fit[] = { 0, 2, 4, 5 };

static const uint8_t *const CReqCalloc_PreRepOffset[] = {
  CReqCalloc_PreRepOffset_Nmemb,
  CReqCalloc_PreRepOffset_Size,
  CReqCalloc_PreRepOffset_Product,
  CReqCalloc_PreRepOffset_Fit
};

static size_t CReqCalloc_GetRepresentativeCountOf(
  const CReqCalloc_Context *ctx,
  size_t                    index
)
{
  const uint8_t *offsets;
  size_t         state;

  offsets = CReqCalloc_PreRepOffset[ index ];
  state = ctx->Map.pcs[ index ];

  return offsets[ state + 1 ] - offsets[ state ];
}

static size_t CReqCalloc_GetRepresentative(
  const CReqCalloc_Context *ctx,
  size_t                    index
)
{
  size_t count;

  count = CReqCalloc_GetRepresentativeCountOf( ctx, index );

  if ( ctx->Map.rep < count ) {
    return ctx->Map.rep;
  }

  return count - 1;
}

static size_t CReqCalloc_GetRepresentativeCount(
  const CReqCalloc_Context *ctx
)
{
  size_t count;
  size_t i;

  count = 1;

  for ( i = 0; i < 4; ++i ) {
    size_t count_of_condition;

    count_of_condition = CReqCalloc_GetRepresentativeCountOf( ctx, i );

    if ( count_of_condition > count ) {
      count = count_of_condition;
    }
  }

  return count;
}

static CReqCalloc_Pre_Nmemb_NonZero_Rep
CReqCalloc_Pre_Nmemb_NonZero_GetRepresentative( const CReqCalloc_Context *ctx )
{
  return (
    CReqCalloc_Pre_Nmemb_NonZero_Rep
  ) CReqCalloc_GetRepresentative( ctx, 0 );
}

static CReqCalloc_Pre_Size_NonZero_Rep
CReqCalloc_Pre_Size_NonZero_GetRepresentative( const CReqCalloc_Context *ctx )
{
  return (
    CReqCalloc_Pre_Size_NonZero_Rep
  ) CReqCalloc_GetRepresentative( ctx, 1 );
}

static CReqCalloc_Pre_Product_Valid_Rep
CReqCalloc_Pre_Product_Valid_GetRepresentative( const CReqCalloc_Context *ctx )
{
  return (
    CReqCalloc_Pre_Product_Valid_Rep
  ) CReqCalloc_GetRepresentative( ctx, 2 );
}

static CReqCalloc_Pre_Product_Overflow_Rep
CReqCalloc_Pre_Product_Overflow_GetRepresentative(
  const CReqCalloc_Context *ctx
)
{
  return (
    CReqCalloc_Pre_Product_Overflow_Rep
  ) CReqCalloc_GetRepresentative( ctx, 2 );
}

static CReqCalloc_Pre_Fit_Yes_Rep CReqCalloc_Pre_Fit_Yes_GetRepresentative(
  const CReqCalloc_Context *ctx
)
{
  return (CReqCalloc_Pre_Fit_Yes_Rep) CReqCalloc_GetRepresentative( ctx, 3 );
}

static CReqCalloc_Pre_Fit_No_Rep CReqCalloc_Pre_Fit_No_GetRepresentative(
  const CReqCalloc_Context *ctx
)
{
  return (CReqCalloc_Pre_Fit_No_Rep) CReqCalloc_GetRepresentative( ctx, 3 );
}

static const uint8_t CReqCalloc_PreRepBase[] = { 0, 6, 14, 19 };

static const uint8_t CReqCalloc_PreRepStates[] = { 2, 2, 2, 2 };

static void CReqCalloc_ResetRepresentatives( CReqCalloc_Context *ctx )
{
  size_t i;

  for ( i = 0; i < 24; ++i ) {
    ctx->Map.rep_ran[ i ] = false;
  }
}

static void CReqCalloc_MarkRepresentatives( CReqCalloc_Context *ctx )
{
  size_t i;

  if ( ctx->Map.skip_rep ) {
    return;
  }

  for ( i = 0; i < 4; ++i ) {
    size_t rep;

    rep = CReqCalloc_PreRepBase[ i ] +
          CReqCalloc_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
          CReqCalloc_GetRepresentative( ctx, i );
    ctx->Map.rep_ran[ rep ] = true;
  }
}

static void CReqCalloc_CheckRepresentatives( const CReqCalloc_Context *ctx )
{
  size_t i;

  for ( i = 0; i < 4; ++i ) {
    const uint8_t *offsets;
    const bool    *ran;
    size_t         state;

    offsets = CReqCalloc_PreRepOffset[ i ];
    ran = &ctx->Map.rep_ran[ CReqCalloc_PreRepBase[ i ] ];

    for ( state = 0; state < CReqCalloc_PreRepStates[ i ]; ++state ) {
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
            CReqCalloc_PreDesc[ i ][ k ]
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

static void CReqCalloc_Pre_Nmemb_Prepare(
  CReqCalloc_Context  *ctx,
  CReqCalloc_Pre_Nmemb state
)
{
  switch ( state ) {
    case CReqCalloc_Pre_Nmemb_Zero: {
      /*
       * While the `nmemb` parameter is equal to zero.
       */
      ctx->nmemb = 0;
      break;
    }

    case CReqCalloc_Pre_Nmemb_NonZero: {
      /*
       * While the `nmemb` parameter is not equal to zero.
       */
      switch ( CReqCalloc_Pre_Nmemb_NonZero_GetRepresentative( ctx ) ) {
        case CReqCalloc_Pre_Nmemb_NonZero_Two: {
          ctx->nmemb = 2;
          break;
        }

        case CReqCalloc_Pre_Nmemb_NonZero_One: {
          ctx->nmemb = 1;
          break;
        }

        case CReqCalloc_Pre_Nmemb_NonZero_Large: {
          ctx->nmemb = 65536;
          break;
        }

        case CReqCalloc_Pre_Nmemb_NonZero_SizeMax: {
          ctx->nmemb = SIZE_MAX;
          break;
        }
      }
      break;
    }

    case CReqCalloc_Pre_Nmemb_NA:
      break;
  }
}

static void CReqCalloc_Pre_Size_Prepare(
  CReqCalloc_Context *ctx,
  CReqCalloc_Pre_Size state
)
{
  switch ( state ) {
    case CReqCalloc_Pre_Size_Zero: {
      /*
       * While the `size` parameter is equal to zero.
       */
      ctx->size = 0;
      break;
    }

    case CReqCalloc_Pre_Size_NonZero: {
      /*
       * While the `size` parameter is not equal to zero.
       */
      switch ( CReqCalloc_Pre_Size_NonZero_GetRepresentative( ctx ) ) {
        case CReqCalloc_Pre_Size_NonZero_One: {
          ctx->size = 1;
          break;
        }

        case CReqCalloc_Pre_Size_NonZero_MinMinusOne: {
          ctx->size = RBHEAP_MIN_SIZE - 1;
          break;
        }

        case CReqCalloc_Pre_Size_NonZero_Min: {
          ctx->size = RBHEAP_MIN_SIZE;
          break;
        }

        case CReqCalloc_Pre_Size_NonZero_MinPlusOne: {
          ctx->size = RBHEAP_MIN_SIZE + 1;
          break;
        }

        case CReqCalloc_Pre_Size_NonZero_Slab: {
          ctx->size = RBHEAP_SLAB_SIZE;
          break;
        }

        case CReqCalloc_Pre_Size_NonZero_SizeMax: {
          ctx->size = SIZE_MAX;
          break;
        }
      }
      break;
    }

    case CReqCalloc_Pre_Size_NA:
      break;
  }
}

static void CReqCalloc_Pre_Product_Prepare(
  CReqCalloc_Context    *ctx,
  CReqCalloc_Pre_Product state
)
{
  switch ( state ) {
    case CReqCalloc_Pre_Product_Valid: {
      /*
       * While the product of the `nmemb` and the `size` parameters is less
       * than or equal to SIZE_MAX.
       */
      switch ( CReqCalloc_Pre_Product_Valid_GetRepresentative( ctx ) ) {
        case CReqCalloc_Pre_Product_Valid_Given: {
          if ( ctx->nmemb != 0 && ctx->size > SIZE_MAX / ctx->nmemb ) {
            ctx->Map.skip_rep = true;
          }
          break;
        }

        case CReqCalloc_Pre_Product_Valid_Limit: {
          if ( ctx->nmemb == 0 || ctx->size == 0 ) {
            ctx->Map.skip_rep = true;
          } else {
            ctx->size = SIZE_MAX / ctx->nmemb;
          }
          break;
        }
      }
      break;
    }

    case CReqCalloc_Pre_Product_Overflow: {
      /*
       * While the product of the `nmemb` and the `size` parameters is greater
       * than SIZE_MAX.
       */
      switch ( CReqCalloc_Pre_Product_Overflow_GetRepresentative( ctx ) ) {
        case CReqCalloc_Pre_Product_Overflow_JustAbove: {
          if ( ctx->nmemb < 2 ) {
            ctx->Map.skip_rep = true;
          } else {
            ctx->size = SIZE_MAX / ctx->nmemb + 1;
          }
          break;
        }

        case CReqCalloc_Pre_Product_Overflow_Max: {
          if ( ctx->nmemb < 2 ) {
            ctx->Map.skip_rep = true;
          } else {
            ctx->size = SIZE_MAX;
          }
          break;
        }
      }
      break;
    }

    case CReqCalloc_Pre_Product_NA:
      break;
  }
}

static void CReqCalloc_Pre_Fit_Prepare(
  CReqCalloc_Context *ctx,
  CReqCalloc_Pre_Fit  state
)
{
  switch ( state ) {
    case CReqCalloc_Pre_Fit_Yes: {
      /*
       * While the C Program Heap has a free block which satisfies the request.
       */
      switch ( CReqCalloc_Pre_Fit_Yes_GetRepresentative( ctx ) ) {
        case CReqCalloc_Pre_Fit_Yes_Empty: {
          if (
            ctx->nmemb * ctx->size >
            HeapGetLargestAllocatableSize( &ctx->heap.heap )
          ) {
            ctx->Map.skip_rep = true;
          }
          break;
        }

        case CReqCalloc_Pre_Fit_Yes_Exact: {
          if (
            ctx->nmemb * ctx->size >
            HeapGetLargestAllocatableSize( &ctx->heap.heap )
          ) {
            ctx->Map.skip_rep = true;
          } else {
            HeapLeaveFreeBlock(
              &ctx->heap.heap,
              ctx->nmemb * ctx->size,
              RBHEAP_ALIGNMENT
            );
          }
          break;
        }
      }
      break;
    }

    case CReqCalloc_Pre_Fit_No: {
      /*
       * While the C Program Heap has no free block which satisfies the
       * request.
       */
      switch ( CReqCalloc_Pre_Fit_No_GetRepresentative( ctx ) ) {
        case CReqCalloc_Pre_Fit_No_Exhausted: {
          HeapExhaust( &ctx->heap.heap );
          break;
        }

        case CReqCalloc_Pre_Fit_No_Short: {
          if (
            ctx->nmemb * ctx->size >= RBHEAP_MIN_SIZE + RBHEAP_ALIGNMENT &&
            ctx->nmemb * ctx->size <=
              HeapGetLargestAllocatableSize( &ctx->heap.heap )
          ) {
            HeapLeaveFreeBlock(
              &ctx->heap.heap,
              ctx->nmemb * ctx->size - RBHEAP_ALIGNMENT,
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

    case CReqCalloc_Pre_Fit_NA:
      break;
  }
}

static void CReqCalloc_Post_Status_Check(
  CReqCalloc_Context    *ctx,
  CReqCalloc_Post_Status state
)
{
  switch ( state ) {
    case CReqCalloc_Post_Status_Block: {
      /*
       * The return value of calloc() shall be the begin address of a used
       * block of the C Program Heap.
       */
      T_not_null( ctx->ptr );
      break;
    }

    case CReqCalloc_Post_Status_Null: {
      /*
       * The return value of calloc() shall be equal to NULL.
       */
      T_null( ctx->ptr );
      break;
    }

    case CReqCalloc_Post_Status_NA:
      break;
  }
}

static void CReqCalloc_Post_Errno_Check(
  CReqCalloc_Context   *ctx,
  CReqCalloc_Post_Errno state
)
{
  switch ( state ) {
    case CReqCalloc_Post_Errno_Nop: {
      /*
       * The value of errno shall not be changed by the calloc() call.
       */
      T_eq_int( ctx->errno_value, ERRNO_SENTINEL );
      break;
    }

    case CReqCalloc_Post_Errno_ENOMEM: {
      /*
       * The value of errno shall be equal to ENOMEM.
       */
      T_eq_int( ctx->errno_value, ENOMEM );
      break;
    }

    case CReqCalloc_Post_Errno_NA:
      break;
  }
}

static void CReqCalloc_Post_Alignment_Check(
  CReqCalloc_Context       *ctx,
  CReqCalloc_Post_Alignment state
)
{
  switch ( state ) {
    case CReqCalloc_Post_Alignment_Heap: {
      /*
       * The begin address of the used block shall be an integral multiple of
       * the heap alignment of the target architecture.
       */
      T_eq_uptr( (uintptr_t) ctx->ptr % CPU_HEAP_ALIGNMENT, 0 );
      break;
    }

    case CReqCalloc_Post_Alignment_NA:
      break;
  }
}

static void CReqCalloc_Post_Content_Check(
  CReqCalloc_Context     *ctx,
  CReqCalloc_Post_Content state
)
{
  uintptr_t size;

  switch ( state ) {
    case CReqCalloc_Post_Content_Zero: {
      /*
       * The used block shall begin with `nmemb` times `size` bytes of zero.
       */
      T_true( _RBHeap_Size_of_block( &ctx->heap.heap, ctx->ptr, &size ) );
      T_ge_uptr( size, ctx->nmemb * ctx->size );
      T_true( IsZero( ctx->ptr, ctx->nmemb * ctx->size ) );
      break;
    }

    case CReqCalloc_Post_Content_NA:
      break;
  }
}

static void CReqCalloc_Post_Heap_Check(
  CReqCalloc_Context  *ctx,
  CReqCalloc_Post_Heap state
)
{
  switch ( state ) {
    case CReqCalloc_Post_Heap_Allocated: {
      /*
       * The C Program Heap shall have one more used block.
       */
      T_eq_u32( ctx->after.used_blocks, ctx->before.used_blocks + 1 );
      break;
    }

    case CReqCalloc_Post_Heap_Nop: {
      /*
       * The used blocks and the free blocks of the C Program Heap shall not be
       * changed by the calloc() call.
       */
      T_eq_u32( ctx->after.used_blocks, ctx->before.used_blocks );
      T_eq_u32( ctx->after.free_blocks, ctx->before.free_blocks );
      T_eq_uptr( ctx->after.free_size, ctx->before.free_size );
      break;
    }

    case CReqCalloc_Post_Heap_NA:
      break;
  }
}

static void CReqCalloc_Setup( CReqCalloc_Context *ctx )
{
  HeapInstall( &ctx->heap );
}

static void CReqCalloc_Setup_Wrap( void *arg )
{
  CReqCalloc_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CReqCalloc_Setup( ctx );
}

static void CReqCalloc_Teardown( CReqCalloc_Context *ctx )
{
  HeapRestore( &ctx->heap );
}

static void CReqCalloc_Teardown_Wrap( void *arg )
{
  CReqCalloc_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CReqCalloc_Teardown( ctx );
}

static void CReqCalloc_Prepare( CReqCalloc_Context *ctx )
{
  memset( HeapGetArea(), 0xff, HEAP_AREA_SIZE );
  HeapReset( &ctx->heap );
}

static void CReqCalloc_Action( CReqCalloc_Context *ctx )
{
  HeapTakeSnapshot( &ctx->heap.heap, &ctx->before );
  errno = ERRNO_SENTINEL;
  ctx->ptr = calloc( ctx->nmemb, ctx->size );
  ctx->errno_value = errno;
  HeapTakeSnapshot( &ctx->heap.heap, &ctx->after );
}

/* clang-format off */

static const CReqCalloc_Entry
CReqCalloc_Entries[] = {
  { 0, 0, 0, 0, 0, CReqCalloc_Post_Status_Null, CReqCalloc_Post_Errno_Nop,
    CReqCalloc_Post_Alignment_NA, CReqCalloc_Post_Content_NA,
    CReqCalloc_Post_Heap_Nop },
  { 1, 0, 0, 0, 0, CReqCalloc_Post_Status_NA, CReqCalloc_Post_Errno_NA,
    CReqCalloc_Post_Alignment_NA, CReqCalloc_Post_Content_NA,
    CReqCalloc_Post_Heap_NA },
  { 0, 0, 0, 0, 0, CReqCalloc_Post_Status_Null, CReqCalloc_Post_Errno_ENOMEM,
    CReqCalloc_Post_Alignment_NA, CReqCalloc_Post_Content_NA,
    CReqCalloc_Post_Heap_Nop },
  { 0, 0, 0, 0, 0, CReqCalloc_Post_Status_Block, CReqCalloc_Post_Errno_Nop,
    CReqCalloc_Post_Alignment_Heap, CReqCalloc_Post_Content_Zero,
    CReqCalloc_Post_Heap_Allocated }
};

static const uint8_t
CReqCalloc_Map[] = {
  0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 3, 2, 2, 2
};

/* clang-format on */

static size_t CReqCalloc_Scope( void *arg, char *buf, size_t n )
{
  CReqCalloc_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    size_t pcd[ 4 ];
    size_t i;

    for ( i = 0; i < 4; ++i ) {
      pcd[ i ] = CReqCalloc_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
                 CReqCalloc_GetRepresentative( ctx, i );
    }

    return T_get_scope( CReqCalloc_PreDesc, buf, n, pcd );
  }

  return 0;
}

static T_fixture CReqCalloc_Fixture = {
  .setup = CReqCalloc_Setup_Wrap,
  .stop = NULL,
  .teardown = CReqCalloc_Teardown_Wrap,
  .scope = CReqCalloc_Scope,
  .initial_context = &CReqCalloc_Instance
};

static inline CReqCalloc_Entry CReqCalloc_PopEntry( CReqCalloc_Context *ctx )
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CReqCalloc_Entries[ CReqCalloc_Map[ index ] ];
}

static void CReqCalloc_TestVariant( CReqCalloc_Context *ctx )
{
  CReqCalloc_Pre_Nmemb_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CReqCalloc_Pre_Size_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CReqCalloc_Pre_Product_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CReqCalloc_Pre_Fit_Prepare( ctx, ctx->Map.pcs[ 3 ] );

  if ( ctx->Map.skip_rep ) {
    return;
  }

  CReqCalloc_Action( ctx );
  CReqCalloc_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CReqCalloc_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
  CReqCalloc_Post_Alignment_Check( ctx, ctx->Map.entry.Post_Alignment );
  CReqCalloc_Post_Content_Check( ctx, ctx->Map.entry.Post_Content );
  CReqCalloc_Post_Heap_Check( ctx, ctx->Map.entry.Post_Heap );
}

/**
 * @fn void T_case_body_CReqCalloc( void )
 */
T_TEST_CASE_FIXTURE( CReqCalloc, &CReqCalloc_Fixture )
{
  CReqCalloc_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  CReqCalloc_ResetRepresentatives( ctx );

  for (
    ctx->Map.pcs[ 0 ] = CReqCalloc_Pre_Nmemb_Zero;
    ctx->Map.pcs[ 0 ] < CReqCalloc_Pre_Nmemb_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CReqCalloc_Pre_Size_Zero;
      ctx->Map.pcs[ 1 ] < CReqCalloc_Pre_Size_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = CReqCalloc_Pre_Product_Valid;
        ctx->Map.pcs[ 2 ] < CReqCalloc_Pre_Product_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = CReqCalloc_Pre_Fit_Yes;
          ctx->Map.pcs[ 3 ] < CReqCalloc_Pre_Fit_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          ctx->Map.entry = CReqCalloc_PopEntry( ctx );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          for (
            ctx->Map.rep = 0;
            ctx->Map.rep < CReqCalloc_GetRepresentativeCount( ctx );
            ++ctx->Map.rep
          ) {
            ctx->Map.skip_rep = false;
            CReqCalloc_Prepare( ctx );
            CReqCalloc_TestVariant( ctx );

            CReqCalloc_MarkRepresentatives( ctx );
          }
        }
      }
    }
  }

  CReqCalloc_CheckRepresentatives( ctx );
}

/** @} */
