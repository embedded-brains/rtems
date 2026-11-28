/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CReqPosixMemalign
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

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#include "tx-heap.h"

#include <rtems/test.h>

/**
 * @defgroup CReqPosixMemalign spec:/c/req/posix-memalign
 *
 * @ingroup TestsuitesValidation0
 *
 * @{
 */

typedef enum {
  CReqPosixMemalign_Pre_Memptr_Valid,
  CReqPosixMemalign_Pre_Memptr_Null,
  CReqPosixMemalign_Pre_Memptr_NA
} CReqPosixMemalign_Pre_Memptr;

typedef enum {
  CReqPosixMemalign_Pre_Alignment_Tiny,
  CReqPosixMemalign_Pre_Alignment_NotPower2,
  CReqPosixMemalign_Pre_Alignment_Valid,
  CReqPosixMemalign_Pre_Alignment_NA
} CReqPosixMemalign_Pre_Alignment;

typedef enum {
  CReqPosixMemalign_Pre_Size_Zero,
  CReqPosixMemalign_Pre_Size_NonZero,
  CReqPosixMemalign_Pre_Size_NA
} CReqPosixMemalign_Pre_Size;

typedef enum {
  CReqPosixMemalign_Pre_Fit_Yes,
  CReqPosixMemalign_Pre_Fit_No,
  CReqPosixMemalign_Pre_Fit_NA
} CReqPosixMemalign_Pre_Fit;

typedef enum {
  CReqPosixMemalign_Post_Status_Zero,
  CReqPosixMemalign_Post_Status_EINVAL,
  CReqPosixMemalign_Post_Status_ENOMEM,
  CReqPosixMemalign_Post_Status_NA
} CReqPosixMemalign_Post_Status;

typedef enum {
  CReqPosixMemalign_Post_MemptrVar_AreaBegin,
  CReqPosixMemalign_Post_MemptrVar_Null,
  CReqPosixMemalign_Post_MemptrVar_Nop,
  CReqPosixMemalign_Post_MemptrVar_NA
} CReqPosixMemalign_Post_MemptrVar;

typedef enum {
  CReqPosixMemalign_Post_Alignment_Valid,
  CReqPosixMemalign_Post_Alignment_NA
} CReqPosixMemalign_Post_Alignment;

typedef enum {
  CReqPosixMemalign_Post_Size_Valid,
  CReqPosixMemalign_Post_Size_NA
} CReqPosixMemalign_Post_Size;

typedef enum {
  CReqPosixMemalign_Post_Heap_Allocated,
  CReqPosixMemalign_Post_Heap_Nop,
  CReqPosixMemalign_Post_Heap_NA
} CReqPosixMemalign_Post_Heap;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Memptr_NA : 1;
  uint16_t Pre_Alignment_NA : 1;
  uint16_t Pre_Size_NA : 1;
  uint16_t Pre_Fit_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_MemptrVar : 2;
  uint16_t Post_Alignment : 1;
  uint16_t Post_Size : 1;
  uint16_t Post_Heap : 2;
} CReqPosixMemalign_Entry;

/**
 * @brief Test context for spec:/c/req/posix-memalign test case.
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
   * @brief This member provides the object referenced by the `memptr`
   *   parameter.
   */
  void *memptr_obj;

  /**
   * @brief This member contains the return value of the posix_memalign() call.
   */
  int status;

  /**
   * @brief This member specifies the `memptr` parameter value.
   */
  void **memptr;

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
    bool rep_ran[ 34 ];

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
    CReqPosixMemalign_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CReqPosixMemalign_Context;

static CReqPosixMemalign_Context CReqPosixMemalign_Instance;

static const char *const CReqPosixMemalign_PreDesc_Memptr[] =
  { "Valid", "Null", "NA" };

static const char *const CReqPosixMemalign_PreDesc_Alignment[] = {
  "TinyZero",
  "TinyOne",
  "TinyPointerMinusOne",
  "NotPower2PointerPlusOne",
  "NotPower2ThreePointers",
  "NotPower2HeapPlusPointer",
  "NotPower2SizeMax",
  "ValidPointer",
  "ValidTwicePointer",
  "ValidHeap",
  "ValidTwiceHeap",
  "ValidSlab",
  "NA"
};

static const char *const CReqPosixMemalign_PreDesc_Size[] = {
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

static const char *const CReqPosixMemalign_PreDesc_Fit[] =
  { "YesEmpty", "YesExact", "NoExhausted", "NoShort", "NA" };

static const char *const *const CReqPosixMemalign_PreDesc[] = {
  CReqPosixMemalign_PreDesc_Memptr,
  CReqPosixMemalign_PreDesc_Alignment,
  CReqPosixMemalign_PreDesc_Size,
  CReqPosixMemalign_PreDesc_Fit,
  NULL
};

typedef enum {
  CReqPosixMemalign_Pre_Alignment_Tiny_Zero,
  CReqPosixMemalign_Pre_Alignment_Tiny_One,
  CReqPosixMemalign_Pre_Alignment_Tiny_PointerMinusOne
} CReqPosixMemalign_Pre_Alignment_Tiny_Rep;

typedef enum {
  CReqPosixMemalign_Pre_Alignment_NotPower2_PointerPlusOne,
  CReqPosixMemalign_Pre_Alignment_NotPower2_ThreePointers,
  CReqPosixMemalign_Pre_Alignment_NotPower2_HeapPlusPointer,
  CReqPosixMemalign_Pre_Alignment_NotPower2_SizeMax
} CReqPosixMemalign_Pre_Alignment_NotPower2_Rep;

typedef enum {
  CReqPosixMemalign_Pre_Alignment_Valid_Pointer,
  CReqPosixMemalign_Pre_Alignment_Valid_TwicePointer,
  CReqPosixMemalign_Pre_Alignment_Valid_Heap,
  CReqPosixMemalign_Pre_Alignment_Valid_TwiceHeap,
  CReqPosixMemalign_Pre_Alignment_Valid_Slab
} CReqPosixMemalign_Pre_Alignment_Valid_Rep;

typedef enum {
  CReqPosixMemalign_Pre_Size_NonZero_One,
  CReqPosixMemalign_Pre_Size_NonZero_MinMinusOne,
  CReqPosixMemalign_Pre_Size_NonZero_Min,
  CReqPosixMemalign_Pre_Size_NonZero_MinPlusOne,
  CReqPosixMemalign_Pre_Size_NonZero_UnitMinusOne,
  CReqPosixMemalign_Pre_Size_NonZero_Unit,
  CReqPosixMemalign_Pre_Size_NonZero_UnitPlusOne,
  CReqPosixMemalign_Pre_Size_NonZero_Slab,
  CReqPosixMemalign_Pre_Size_NonZero_Max,
  CReqPosixMemalign_Pre_Size_NonZero_AboveMax,
  CReqPosixMemalign_Pre_Size_NonZero_SizeMax
} CReqPosixMemalign_Pre_Size_NonZero_Rep;

typedef enum {
  CReqPosixMemalign_Pre_Fit_Yes_Empty,
  CReqPosixMemalign_Pre_Fit_Yes_Exact
} CReqPosixMemalign_Pre_Fit_Yes_Rep;

typedef enum {
  CReqPosixMemalign_Pre_Fit_No_Exhausted,
  CReqPosixMemalign_Pre_Fit_No_Short
} CReqPosixMemalign_Pre_Fit_No_Rep;

static const uint8_t CReqPosixMemalign_PreRepOffset_Memptr[] = { 0, 1, 2, 3 };

static const uint8_t CReqPosixMemalign_PreRepOffset_Alignment[] =
  { 0, 3, 7, 12, 13 };

static const uint8_t CReqPosixMemalign_PreRepOffset_Size[] = { 0, 1, 12, 13 };

static const uint8_t CReqPosixMemalign_PreRepOffset_Fit[] = { 0, 2, 4, 5 };

static const uint8_t *const CReqPosixMemalign_PreRepOffset[] = {
  CReqPosixMemalign_PreRepOffset_Memptr,
  CReqPosixMemalign_PreRepOffset_Alignment,
  CReqPosixMemalign_PreRepOffset_Size,
  CReqPosixMemalign_PreRepOffset_Fit
};

static size_t CReqPosixMemalign_GetRepresentativeCountOf(
  const CReqPosixMemalign_Context *ctx,
  size_t                           index
)
{
  const uint8_t *offsets;
  size_t         state;

  offsets = CReqPosixMemalign_PreRepOffset[ index ];
  state = ctx->Map.pcs[ index ];

  return offsets[ state + 1 ] - offsets[ state ];
}

static size_t CReqPosixMemalign_GetRepresentative(
  const CReqPosixMemalign_Context *ctx,
  size_t                           index
)
{
  size_t count;

  count = CReqPosixMemalign_GetRepresentativeCountOf( ctx, index );

  if ( ctx->Map.rep < count ) {
    return ctx->Map.rep;
  }

  return count - 1;
}

static size_t CReqPosixMemalign_GetRepresentativeCount(
  const CReqPosixMemalign_Context *ctx
)
{
  size_t count;
  size_t i;

  count = 1;

  for ( i = 0; i < 4; ++i ) {
    size_t count_of_condition;

    count_of_condition = CReqPosixMemalign_GetRepresentativeCountOf( ctx, i );

    if ( count_of_condition > count ) {
      count = count_of_condition;
    }
  }

  return count;
}

static CReqPosixMemalign_Pre_Alignment_Tiny_Rep
CReqPosixMemalign_Pre_Alignment_Tiny_GetRepresentative(
  const CReqPosixMemalign_Context *ctx
)
{
  return (
    CReqPosixMemalign_Pre_Alignment_Tiny_Rep
  ) CReqPosixMemalign_GetRepresentative( ctx, 1 );
}

static CReqPosixMemalign_Pre_Alignment_NotPower2_Rep
CReqPosixMemalign_Pre_Alignment_NotPower2_GetRepresentative(
  const CReqPosixMemalign_Context *ctx
)
{
  return (
    CReqPosixMemalign_Pre_Alignment_NotPower2_Rep
  ) CReqPosixMemalign_GetRepresentative( ctx, 1 );
}

static CReqPosixMemalign_Pre_Alignment_Valid_Rep
CReqPosixMemalign_Pre_Alignment_Valid_GetRepresentative(
  const CReqPosixMemalign_Context *ctx
)
{
  return (
    CReqPosixMemalign_Pre_Alignment_Valid_Rep
  ) CReqPosixMemalign_GetRepresentative( ctx, 1 );
}

static CReqPosixMemalign_Pre_Size_NonZero_Rep
CReqPosixMemalign_Pre_Size_NonZero_GetRepresentative(
  const CReqPosixMemalign_Context *ctx
)
{
  return (
    CReqPosixMemalign_Pre_Size_NonZero_Rep
  ) CReqPosixMemalign_GetRepresentative( ctx, 2 );
}

static CReqPosixMemalign_Pre_Fit_Yes_Rep
CReqPosixMemalign_Pre_Fit_Yes_GetRepresentative(
  const CReqPosixMemalign_Context *ctx
)
{
  return (
    CReqPosixMemalign_Pre_Fit_Yes_Rep
  ) CReqPosixMemalign_GetRepresentative( ctx, 3 );
}

static CReqPosixMemalign_Pre_Fit_No_Rep
CReqPosixMemalign_Pre_Fit_No_GetRepresentative(
  const CReqPosixMemalign_Context *ctx
)
{
  return (
    CReqPosixMemalign_Pre_Fit_No_Rep
  ) CReqPosixMemalign_GetRepresentative( ctx, 3 );
}

static const uint8_t CReqPosixMemalign_PreRepBase[] = { 0, 3, 16, 29 };

static const uint8_t CReqPosixMemalign_PreRepStates[] = { 2, 3, 2, 2 };

static void CReqPosixMemalign_ResetRepresentatives(
  CReqPosixMemalign_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 34; ++i ) {
    ctx->Map.rep_ran[ i ] = false;
  }
}

static void CReqPosixMemalign_MarkRepresentatives(
  CReqPosixMemalign_Context *ctx
)
{
  size_t i;

  if ( ctx->Map.skip_rep ) {
    return;
  }

  for ( i = 0; i < 4; ++i ) {
    size_t rep;

    rep = CReqPosixMemalign_PreRepBase[ i ] +
          CReqPosixMemalign_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
          CReqPosixMemalign_GetRepresentative( ctx, i );
    ctx->Map.rep_ran[ rep ] = true;
  }
}

static void CReqPosixMemalign_CheckRepresentatives(
  const CReqPosixMemalign_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < 4; ++i ) {
    const uint8_t *offsets;
    const bool    *ran;
    size_t         state;

    offsets = CReqPosixMemalign_PreRepOffset[ i ];
    ran = &ctx->Map.rep_ran[ CReqPosixMemalign_PreRepBase[ i ] ];

    for ( state = 0; state < CReqPosixMemalign_PreRepStates[ i ]; ++state ) {
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
            CReqPosixMemalign_PreDesc[ i ][ k ]
          );
        }
      }
    }
  }
}

static void CReqPosixMemalign_Pre_Memptr_Prepare(
  CReqPosixMemalign_Context   *ctx,
  CReqPosixMemalign_Pre_Memptr state
)
{
  switch ( state ) {
    case CReqPosixMemalign_Pre_Memptr_Valid: {
      /*
       * While the `memptr` parameter references an object of type `void *`.
       */
      ctx->memptr = &ctx->memptr_obj;
      break;
    }

    case CReqPosixMemalign_Pre_Memptr_Null: {
      /*
       * While the `memptr` parameter is equal to NULL.
       */
      ctx->memptr = NULL;
      break;
    }

    case CReqPosixMemalign_Pre_Memptr_NA:
      break;
  }
}

static void CReqPosixMemalign_Pre_Alignment_Prepare(
  CReqPosixMemalign_Context      *ctx,
  CReqPosixMemalign_Pre_Alignment state
)
{
  switch ( state ) {
    case CReqPosixMemalign_Pre_Alignment_Tiny: {
      /*
       * While the `alignment` parameter is less than `sizeof( void * )`.
       */
      switch (
        CReqPosixMemalign_Pre_Alignment_Tiny_GetRepresentative( ctx )
      ) {
        case CReqPosixMemalign_Pre_Alignment_Tiny_Zero: {
          ctx->alignment = 0;
          break;
        }

        case CReqPosixMemalign_Pre_Alignment_Tiny_One: {
          ctx->alignment = 1;
          break;
        }

        case CReqPosixMemalign_Pre_Alignment_Tiny_PointerMinusOne: {
          ctx->alignment = sizeof( void * ) - 1;
          break;
        }
      }
      break;
    }

    case CReqPosixMemalign_Pre_Alignment_NotPower2: {
      /*
       * While the `alignment` parameter is greater than or equal to `sizeof(
       * void * )`, while the `alignment` parameter is not a power of two.
       */
      switch (
        CReqPosixMemalign_Pre_Alignment_NotPower2_GetRepresentative( ctx )
      ) {
        case CReqPosixMemalign_Pre_Alignment_NotPower2_PointerPlusOne: {
          ctx->alignment = sizeof( void * ) + 1;
          break;
        }

        case CReqPosixMemalign_Pre_Alignment_NotPower2_ThreePointers: {
          ctx->alignment = 3 * sizeof( void * );
          break;
        }

        case CReqPosixMemalign_Pre_Alignment_NotPower2_HeapPlusPointer: {
          ctx->alignment = RBHEAP_ALIGNMENT + sizeof( void * );
          break;
        }

        case CReqPosixMemalign_Pre_Alignment_NotPower2_SizeMax: {
          ctx->alignment = SIZE_MAX;
          break;
        }
      }
      break;
    }

    case CReqPosixMemalign_Pre_Alignment_Valid: {
      /*
       * While the `alignment` parameter is greater than or equal to `sizeof(
       * void * )`, while the `alignment` parameter is a power of two.
       */
      switch (
        CReqPosixMemalign_Pre_Alignment_Valid_GetRepresentative( ctx )
      ) {
        case CReqPosixMemalign_Pre_Alignment_Valid_Pointer: {
          ctx->alignment = sizeof( void * );
          break;
        }

        case CReqPosixMemalign_Pre_Alignment_Valid_TwicePointer: {
          ctx->alignment = 2 * sizeof( void * );
          break;
        }

        case CReqPosixMemalign_Pre_Alignment_Valid_Heap: {
          ctx->alignment = RBHEAP_ALIGNMENT;
          break;
        }

        case CReqPosixMemalign_Pre_Alignment_Valid_TwiceHeap: {
          ctx->alignment = 2 * RBHEAP_ALIGNMENT;
          break;
        }

        case CReqPosixMemalign_Pre_Alignment_Valid_Slab: {
          ctx->alignment = RBHEAP_SLAB_SIZE;
          break;
        }
      }
      break;
    }

    case CReqPosixMemalign_Pre_Alignment_NA:
      break;
  }
}

static void CReqPosixMemalign_Pre_Size_Prepare(
  CReqPosixMemalign_Context *ctx,
  CReqPosixMemalign_Pre_Size state
)
{
  switch ( state ) {
    case CReqPosixMemalign_Pre_Size_Zero: {
      /*
       * While the `size` parameter is equal to zero.
       */
      ctx->size = 0;
      break;
    }

    case CReqPosixMemalign_Pre_Size_NonZero: {
      /*
       * While the `size` parameter is not equal to zero.
       */
      switch ( CReqPosixMemalign_Pre_Size_NonZero_GetRepresentative( ctx ) ) {
        case CReqPosixMemalign_Pre_Size_NonZero_One: {
          ctx->size = 1;
          break;
        }

        case CReqPosixMemalign_Pre_Size_NonZero_MinMinusOne: {
          ctx->size = RBHEAP_MIN_SIZE - 1;
          break;
        }

        case CReqPosixMemalign_Pre_Size_NonZero_Min: {
          ctx->size = RBHEAP_MIN_SIZE;
          break;
        }

        case CReqPosixMemalign_Pre_Size_NonZero_MinPlusOne: {
          ctx->size = RBHEAP_MIN_SIZE + 1;
          break;
        }

        case CReqPosixMemalign_Pre_Size_NonZero_UnitMinusOne: {
          ctx->size = RBHEAP_MIN_SIZE + RBHEAP_ALIGNMENT - 1;
          break;
        }

        case CReqPosixMemalign_Pre_Size_NonZero_Unit: {
          ctx->size = RBHEAP_MIN_SIZE + RBHEAP_ALIGNMENT;
          break;
        }

        case CReqPosixMemalign_Pre_Size_NonZero_UnitPlusOne: {
          ctx->size = RBHEAP_MIN_SIZE + RBHEAP_ALIGNMENT + 1;
          break;
        }

        case CReqPosixMemalign_Pre_Size_NonZero_Slab: {
          ctx->size = RBHEAP_SLAB_SIZE;
          break;
        }

        case CReqPosixMemalign_Pre_Size_NonZero_Max: {
          ctx->size = RBHEAP_MAX_SIZE;
          break;
        }

        case CReqPosixMemalign_Pre_Size_NonZero_AboveMax: {
          ctx->size = RBHEAP_MAX_SIZE + 1;
          break;
        }

        case CReqPosixMemalign_Pre_Size_NonZero_SizeMax: {
          ctx->size = SIZE_MAX;
          break;
        }
      }
      break;
    }

    case CReqPosixMemalign_Pre_Size_NA:
      break;
  }
}

static void CReqPosixMemalign_Pre_Fit_Prepare(
  CReqPosixMemalign_Context *ctx,
  CReqPosixMemalign_Pre_Fit  state
)
{
  switch ( state ) {
    case CReqPosixMemalign_Pre_Fit_Yes: {
      /*
       * While the C Program Heap has a free block which satisfies the request.
       */
      switch ( CReqPosixMemalign_Pre_Fit_Yes_GetRepresentative( ctx ) ) {
        case CReqPosixMemalign_Pre_Fit_Yes_Empty: {
          if ( ctx->size > HeapGetLargestAllocatableSize( &ctx->heap.heap ) ) {
            ctx->Map.skip_rep = true;
          }
          break;
        }

        case CReqPosixMemalign_Pre_Fit_Yes_Exact: {
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

    case CReqPosixMemalign_Pre_Fit_No: {
      /*
       * While the C Program Heap has no free block which satisfies the
       * request.
       */
      switch ( CReqPosixMemalign_Pre_Fit_No_GetRepresentative( ctx ) ) {
        case CReqPosixMemalign_Pre_Fit_No_Exhausted: {
          HeapExhaust( &ctx->heap.heap );
          break;
        }

        case CReqPosixMemalign_Pre_Fit_No_Short: {
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

    case CReqPosixMemalign_Pre_Fit_NA:
      break;
  }
}

static void CReqPosixMemalign_Post_Status_Check(
  CReqPosixMemalign_Context    *ctx,
  CReqPosixMemalign_Post_Status state
)
{
  switch ( state ) {
    case CReqPosixMemalign_Post_Status_Zero: {
      /*
       * The return value of posix_memalign() shall be equal to zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CReqPosixMemalign_Post_Status_EINVAL: {
      /*
       * The return value of posix_memalign() shall be equal to EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CReqPosixMemalign_Post_Status_ENOMEM: {
      /*
       * The return value of posix_memalign() shall be equal to ENOMEM.
       */
      T_eq_int( ctx->status, ENOMEM );
      break;
    }

    case CReqPosixMemalign_Post_Status_NA:
      break;
  }
}

static void CReqPosixMemalign_Post_MemptrVar_Check(
  CReqPosixMemalign_Context       *ctx,
  CReqPosixMemalign_Post_MemptrVar state
)
{
  uintptr_t size;

  switch ( state ) {
    case CReqPosixMemalign_Post_MemptrVar_AreaBegin: {
      /*
       * The value of the object referenced by the `memptr` parameter shall be
       * the begin address of a used block of the C Program Heap.
       */
      T_not_null( ctx->memptr_obj );
      T_true(
        _RBHeap_Size_of_block( &ctx->heap.heap, ctx->memptr_obj, &size )
      );
      break;
    }

    case CReqPosixMemalign_Post_MemptrVar_Null: {
      /*
       * The value of the object referenced by the `memptr` parameter shall be
       * NULL.
       */
      T_null( ctx->memptr_obj );
      break;
    }

    case CReqPosixMemalign_Post_MemptrVar_Nop: {
      /*
       * Objects referenced by the `memptr` parameter in past calls to
       * posix_memalign() shall not be accessed by the posix_memalign() call.
       */
      T_eq_uptr( (uintptr_t) ctx->memptr_obj, 1 );
      break;
    }

    case CReqPosixMemalign_Post_MemptrVar_NA:
      break;
  }
}

static void CReqPosixMemalign_Post_Alignment_Check(
  CReqPosixMemalign_Context       *ctx,
  CReqPosixMemalign_Post_Alignment state
)
{
  switch ( state ) {
    case CReqPosixMemalign_Post_Alignment_Valid: {
      /*
       * The begin address of the used block shall be an integral multiple of
       * the `alignment` parameter.
       */
      T_eq_uptr( (uintptr_t) ctx->memptr_obj % ctx->alignment, 0 );
      break;
    }

    case CReqPosixMemalign_Post_Alignment_NA:
      break;
  }
}

static void CReqPosixMemalign_Post_Size_Check(
  CReqPosixMemalign_Context  *ctx,
  CReqPosixMemalign_Post_Size state
)
{
  uintptr_t size;

  switch ( state ) {
    case CReqPosixMemalign_Post_Size_Valid: {
      /*
       * The size of the used block shall be greater than or equal to the
       * `size` parameter.
       */
      T_true(
        _RBHeap_Size_of_block( &ctx->heap.heap, ctx->memptr_obj, &size )
      );
      T_ge_uptr( size, ctx->size );
      break;
    }

    case CReqPosixMemalign_Post_Size_NA:
      break;
  }
}

static void CReqPosixMemalign_Post_Heap_Check(
  CReqPosixMemalign_Context  *ctx,
  CReqPosixMemalign_Post_Heap state
)
{
  switch ( state ) {
    case CReqPosixMemalign_Post_Heap_Allocated: {
      /*
       * The C Program Heap shall have one more used block.
       */
      T_eq_u32( ctx->after.used_blocks, ctx->before.used_blocks + 1 );
      break;
    }

    case CReqPosixMemalign_Post_Heap_Nop: {
      /*
       * The used blocks and the free blocks of the C Program Heap shall not be
       * changed by the posix_memalign() call.
       */
      T_eq_u32( ctx->after.used_blocks, ctx->before.used_blocks );
      T_eq_u32( ctx->after.free_blocks, ctx->before.free_blocks );
      T_eq_uptr( ctx->after.free_size, ctx->before.free_size );
      break;
    }

    case CReqPosixMemalign_Post_Heap_NA:
      break;
  }
}

static void CReqPosixMemalign_Setup( CReqPosixMemalign_Context *ctx )
{
  HeapInstall( &ctx->heap );
}

static void CReqPosixMemalign_Setup_Wrap( void *arg )
{
  CReqPosixMemalign_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CReqPosixMemalign_Setup( ctx );
}

static void CReqPosixMemalign_Teardown( CReqPosixMemalign_Context *ctx )
{
  HeapRestore( &ctx->heap );
}

static void CReqPosixMemalign_Teardown_Wrap( void *arg )
{
  CReqPosixMemalign_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CReqPosixMemalign_Teardown( ctx );
}

static void CReqPosixMemalign_Prepare( CReqPosixMemalign_Context *ctx )
{
  ctx->memptr_obj = (void *) (uintptr_t) 1;
  HeapReset( &ctx->heap );
}

static void CReqPosixMemalign_Action( CReqPosixMemalign_Context *ctx )
{
  HeapTakeSnapshot( &ctx->heap.heap, &ctx->before );
  ctx->status = posix_memalign( ctx->memptr, ctx->alignment, ctx->size );
  HeapTakeSnapshot( &ctx->heap.heap, &ctx->after );
}

/* clang-format off */

static const CReqPosixMemalign_Entry
CReqPosixMemalign_Entries[] = {
  { 0, 0, 0, 0, 0, CReqPosixMemalign_Post_Status_EINVAL,
    CReqPosixMemalign_Post_MemptrVar_Nop, CReqPosixMemalign_Post_Alignment_NA,
    CReqPosixMemalign_Post_Size_NA, CReqPosixMemalign_Post_Heap_Nop },
  { 0, 0, 0, 0, 0, CReqPosixMemalign_Post_Status_EINVAL,
    CReqPosixMemalign_Post_MemptrVar_Null, CReqPosixMemalign_Post_Alignment_NA,
    CReqPosixMemalign_Post_Size_NA, CReqPosixMemalign_Post_Heap_Nop },
  { 0, 0, 0, 0, 0, CReqPosixMemalign_Post_Status_Zero,
    CReqPosixMemalign_Post_MemptrVar_Null, CReqPosixMemalign_Post_Alignment_NA,
    CReqPosixMemalign_Post_Size_NA, CReqPosixMemalign_Post_Heap_Nop },
  { 0, 0, 0, 0, 0, CReqPosixMemalign_Post_Status_Zero,
    CReqPosixMemalign_Post_MemptrVar_AreaBegin,
    CReqPosixMemalign_Post_Alignment_Valid, CReqPosixMemalign_Post_Size_Valid,
    CReqPosixMemalign_Post_Heap_Allocated },
  { 0, 0, 0, 0, 0, CReqPosixMemalign_Post_Status_ENOMEM,
    CReqPosixMemalign_Post_MemptrVar_Null, CReqPosixMemalign_Post_Alignment_NA,
    CReqPosixMemalign_Post_Size_NA, CReqPosixMemalign_Post_Heap_Nop }
};

static const uint8_t
CReqPosixMemalign_Map[] = {
  1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t CReqPosixMemalign_Scope( void *arg, char *buf, size_t n )
{
  CReqPosixMemalign_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    size_t pcd[ 4 ];
    size_t i;

    for ( i = 0; i < 4; ++i ) {
      pcd[ i ] = CReqPosixMemalign_PreRepOffset[ i ][ ctx->Map.pcs[ i ] ] +
                 CReqPosixMemalign_GetRepresentative( ctx, i );
    }

    return T_get_scope( CReqPosixMemalign_PreDesc, buf, n, pcd );
  }

  return 0;
}

static T_fixture CReqPosixMemalign_Fixture = {
  .setup = CReqPosixMemalign_Setup_Wrap,
  .stop = NULL,
  .teardown = CReqPosixMemalign_Teardown_Wrap,
  .scope = CReqPosixMemalign_Scope,
  .initial_context = &CReqPosixMemalign_Instance
};

static inline CReqPosixMemalign_Entry CReqPosixMemalign_PopEntry(
  CReqPosixMemalign_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CReqPosixMemalign_Entries[ CReqPosixMemalign_Map[ index ] ];
}

static void CReqPosixMemalign_TestVariant( CReqPosixMemalign_Context *ctx )
{
  CReqPosixMemalign_Pre_Memptr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CReqPosixMemalign_Pre_Alignment_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CReqPosixMemalign_Pre_Size_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CReqPosixMemalign_Pre_Fit_Prepare( ctx, ctx->Map.pcs[ 3 ] );

  if ( ctx->Map.skip_rep ) {
    return;
  }

  CReqPosixMemalign_Action( ctx );
  CReqPosixMemalign_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CReqPosixMemalign_Post_MemptrVar_Check( ctx, ctx->Map.entry.Post_MemptrVar );
  CReqPosixMemalign_Post_Alignment_Check( ctx, ctx->Map.entry.Post_Alignment );
  CReqPosixMemalign_Post_Size_Check( ctx, ctx->Map.entry.Post_Size );
  CReqPosixMemalign_Post_Heap_Check( ctx, ctx->Map.entry.Post_Heap );
}

/**
 * @fn void T_case_body_CReqPosixMemalign( void )
 */
T_TEST_CASE_FIXTURE( CReqPosixMemalign, &CReqPosixMemalign_Fixture )
{
  CReqPosixMemalign_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  CReqPosixMemalign_ResetRepresentatives( ctx );

  for (
    ctx->Map.pcs[ 0 ] = CReqPosixMemalign_Pre_Memptr_Valid;
    ctx->Map.pcs[ 0 ] < CReqPosixMemalign_Pre_Memptr_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CReqPosixMemalign_Pre_Alignment_Tiny;
      ctx->Map.pcs[ 1 ] < CReqPosixMemalign_Pre_Alignment_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = CReqPosixMemalign_Pre_Size_Zero;
        ctx->Map.pcs[ 2 ] < CReqPosixMemalign_Pre_Size_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = CReqPosixMemalign_Pre_Fit_Yes;
          ctx->Map.pcs[ 3 ] < CReqPosixMemalign_Pre_Fit_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          ctx->Map.entry = CReqPosixMemalign_PopEntry( ctx );

          for (
            ctx->Map.rep = 0;
            ctx->Map.rep < CReqPosixMemalign_GetRepresentativeCount( ctx );
            ++ctx->Map.rep
          ) {
            ctx->Map.skip_rep = false;
            CReqPosixMemalign_Prepare( ctx );
            CReqPosixMemalign_TestVariant( ctx );

            CReqPosixMemalign_MarkRepresentatives( ctx );
          }
        }
      }
    }
  }

  CReqPosixMemalign_CheckRepresentatives( ctx );
}

/** @} */
