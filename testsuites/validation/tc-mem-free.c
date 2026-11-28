/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CReqFree
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

#include <stdlib.h>

#include "tx-heap.h"

#include <rtems/test.h>

/**
 * @defgroup CReqFree spec:/c/req/free
 *
 * @ingroup TestsuitesValidation0
 *
 * @{
 */

typedef enum {
  CReqFree_Pre_Ptr_Null,
  CReqFree_Pre_Ptr_Used,
  CReqFree_Pre_Ptr_Invalid,
  CReqFree_Pre_Ptr_NA
} CReqFree_Pre_Ptr;

typedef enum {
  CReqFree_Post_Heap_Freed,
  CReqFree_Post_Heap_Nop,
  CReqFree_Post_Heap_NA
} CReqFree_Post_Heap;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Ptr_NA : 1;
  uint8_t Post_Heap : 2;
} CReqFree_Entry;

/**
 * @brief Test context for spec:/c/req/free test case.
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
   * @brief This member contains the size of the block referenced by the `ptr`
   *   parameter.
   */
  uintptr_t block_size;

  /**
   * @brief This member specifies the `ptr` parameter value.
   */
  void *ptr;

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
    CReqFree_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CReqFree_Context;

static CReqFree_Context CReqFree_Instance;

static const char *const CReqFree_PreDesc_Ptr[] =
  { "Null", "Used", "Invalid", "NA" };

static const char *const *const CReqFree_PreDesc[] = {
  CReqFree_PreDesc_Ptr,
  NULL
};

static void CReqFree_Pre_Ptr_Prepare(
  CReqFree_Context *ctx,
  CReqFree_Pre_Ptr  state
)
{
  switch ( state ) {
    case CReqFree_Pre_Ptr_Null: {
      /*
       * While the `ptr` parameter is equal to NULL.
       */
      ctx->ptr = NULL;
      break;
    }

    case CReqFree_Pre_Ptr_Used: {
      /*
       * While the `ptr` parameter is the begin address of a used block of the
       * C Program Heap.
       */
      ctx->ptr = malloc( 1 );
      T_quiet_not_null( ctx->ptr );
      T_quiet_true(
        _RBHeap_Size_of_block( &ctx->heap.heap, ctx->ptr, &ctx->block_size )
      );
      ctx->block_size += RBHEAP_GUARD_SIZE;
      break;
    }

    case CReqFree_Pre_Ptr_Invalid: {
      /*
       * While the `ptr` parameter is not equal to NULL, while the `ptr`
       * parameter is not the begin address of a used block of the C Program
       * Heap.
       */
      ctx->ptr = HeapGetArea();
      break;
    }

    case CReqFree_Pre_Ptr_NA:
      break;
  }
}

static void CReqFree_Post_Heap_Check(
  CReqFree_Context  *ctx,
  CReqFree_Post_Heap state
)
{
  switch ( state ) {
    case CReqFree_Post_Heap_Freed: {
      /*
       * The memory area referenced by the `ptr` parameter shall be a free
       * block of the C Program Heap.
       */
      T_eq_u32( ctx->after.used_blocks + 1, ctx->before.used_blocks );
      T_eq_uptr(
        ctx->after.free_size,
        ctx->before.free_size + ctx->block_size
      );
      break;
    }

    case CReqFree_Post_Heap_Nop: {
      /*
       * The used blocks and the free blocks of the C Program Heap shall not be
       * changed by the free() call.
       */
      T_eq_u32( ctx->after.used_blocks, ctx->before.used_blocks );
      T_eq_u32( ctx->after.free_blocks, ctx->before.free_blocks );
      T_eq_uptr( ctx->after.free_size, ctx->before.free_size );
      break;
    }

    case CReqFree_Post_Heap_NA:
      break;
  }
}

static void CReqFree_Setup( CReqFree_Context *ctx )
{
  HeapInstall( &ctx->heap );
}

static void CReqFree_Setup_Wrap( void *arg )
{
  CReqFree_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CReqFree_Setup( ctx );
}

static void CReqFree_Teardown( CReqFree_Context *ctx )
{
  HeapRestore( &ctx->heap );
}

static void CReqFree_Teardown_Wrap( void *arg )
{
  CReqFree_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CReqFree_Teardown( ctx );
}

static void CReqFree_Prepare( CReqFree_Context *ctx )
{
  HeapReset( &ctx->heap );
}

static void CReqFree_Action( CReqFree_Context *ctx )
{
  HeapTakeSnapshot( &ctx->heap.heap, &ctx->before );
  free( ctx->ptr );
  HeapTakeSnapshot( &ctx->heap.heap, &ctx->after );
}

/* clang-format off */

static const CReqFree_Entry
CReqFree_Entries[] = {
  { 0, 0, CReqFree_Post_Heap_Nop },
  { 0, 0, CReqFree_Post_Heap_Freed },
  { 1, 0, CReqFree_Post_Heap_NA }
};

static const uint8_t
CReqFree_Map[] = {
  0, 1, 2
};

/* clang-format on */

static size_t CReqFree_Scope( void *arg, char *buf, size_t n )
{
  CReqFree_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CReqFree_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CReqFree_Fixture = {
  .setup = CReqFree_Setup_Wrap,
  .stop = NULL,
  .teardown = CReqFree_Teardown_Wrap,
  .scope = CReqFree_Scope,
  .initial_context = &CReqFree_Instance
};

static inline CReqFree_Entry CReqFree_PopEntry( CReqFree_Context *ctx )
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CReqFree_Entries[ CReqFree_Map[ index ] ];
}

static void CReqFree_TestVariant( CReqFree_Context *ctx )
{
  CReqFree_Pre_Ptr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CReqFree_Action( ctx );
  CReqFree_Post_Heap_Check( ctx, ctx->Map.entry.Post_Heap );
}

/**
 * @fn void T_case_body_CReqFree( void )
 */
T_TEST_CASE_FIXTURE( CReqFree, &CReqFree_Fixture )
{
  CReqFree_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CReqFree_Pre_Ptr_Null;
    ctx->Map.pcs[ 0 ] < CReqFree_Pre_Ptr_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CReqFree_PopEntry( ctx );

    if ( ctx->Map.entry.Skip ) {
      continue;
    }

    CReqFree_Prepare( ctx );
    CReqFree_TestVariant( ctx );
  }
}

/** @} */
