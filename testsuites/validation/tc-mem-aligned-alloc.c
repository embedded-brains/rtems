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

static const char *const CReqAlignedAlloc_PreDesc_Alignment[] =
  { "Zero", "NotPower2", "Power2", "NA" };

static const char *const CReqAlignedAlloc_PreDesc_Size[] =
  { "Zero", "NonZero", "NA" };

static const char *const CReqAlignedAlloc_PreDesc_Fit[] =
  { "Yes", "No", "NA" };

static const char *const *const CReqAlignedAlloc_PreDesc[] = {
  CReqAlignedAlloc_PreDesc_Alignment,
  CReqAlignedAlloc_PreDesc_Size,
  CReqAlignedAlloc_PreDesc_Fit,
  NULL
};

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
       * While the `alignment` parameter is greater than or equal to `sizeof(
       * void * )`, while the `alignment` parameter is not a power of two.
       */
      ctx->alignment = sizeof( void * ) + 1;
      break;
    }

    case CReqAlignedAlloc_Pre_Alignment_Power2: {
      /*
       * While the `alignment` parameter is greater than or equal to `sizeof(
       * void * )`, while the `alignment` parameter is a power of two.
       */
      ctx->alignment = 128;
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
      ctx->size = sizeof( uint64_t );
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
      /* The heap is empty */
      break;
    }

    case CReqAlignedAlloc_Pre_Fit_No: {
      /*
       * While the C Program Heap has no free block which satisfies the
       * request.
       */
      HeapExhaust( &ctx->heap.heap );
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
    return T_get_scope( CReqAlignedAlloc_PreDesc, buf, n, ctx->Map.pcs );
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
        CReqAlignedAlloc_Prepare( ctx );
        CReqAlignedAlloc_TestVariant( ctx );
      }
    }
  }
}

/** @} */
