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

static const char *const CReqPosixMemalign_PreDesc_Alignment[] =
  { "Tiny", "NotPower2", "Valid", "NA" };

static const char *const CReqPosixMemalign_PreDesc_Size[] =
  { "Zero", "NonZero", "NA" };

static const char *const CReqPosixMemalign_PreDesc_Fit[] =
  { "Yes", "No", "NA" };

static const char *const *const CReqPosixMemalign_PreDesc[] = {
  CReqPosixMemalign_PreDesc_Memptr,
  CReqPosixMemalign_PreDesc_Alignment,
  CReqPosixMemalign_PreDesc_Size,
  CReqPosixMemalign_PreDesc_Fit,
  NULL
};

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
      ctx->alignment = sizeof( void * ) - 1;
      break;
    }

    case CReqPosixMemalign_Pre_Alignment_NotPower2: {
      /*
       * While the `alignment` parameter is greater than or equal to `sizeof(
       * void * )`, while the `alignment` parameter is not a power of two.
       */
      ctx->alignment = sizeof( void * ) + 1;
      break;
    }

    case CReqPosixMemalign_Pre_Alignment_Valid: {
      /*
       * While the `alignment` parameter is greater than or equal to `sizeof(
       * void * )`, while the `alignment` parameter is a power of two.
       */
      ctx->alignment = 128;
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
      ctx->size = sizeof( uint64_t );
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
      /* The heap is empty */
      break;
    }

    case CReqPosixMemalign_Pre_Fit_No: {
      /*
       * While the C Program Heap has no free block which satisfies the
       * request.
       */
      HeapExhaust( &ctx->heap.heap );
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
    return T_get_scope( CReqPosixMemalign_PreDesc, buf, n, ctx->Map.pcs );
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
          CReqPosixMemalign_Prepare( ctx );
          CReqPosixMemalign_TestVariant( ctx );
        }
      }
    }
  }
}

/** @} */
