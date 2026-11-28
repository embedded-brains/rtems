/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CReqMallocUsableSize
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

#include <malloc.h>
#include <stdlib.h>

#include "tx-heap.h"

#include <rtems/test.h>

/**
 * @defgroup CReqMallocUsableSize spec:/c/req/malloc-usable-size
 *
 * @ingroup TestsuitesValidation0
 *
 * @{
 */

typedef enum {
  CReqMallocUsableSize_Pre_Ptr_Null,
  CReqMallocUsableSize_Pre_Ptr_Used,
  CReqMallocUsableSize_Pre_Ptr_Invalid,
  CReqMallocUsableSize_Pre_Ptr_NA
} CReqMallocUsableSize_Pre_Ptr;

typedef enum {
  CReqMallocUsableSize_Post_Value_Zero,
  CReqMallocUsableSize_Post_Value_Usable,
  CReqMallocUsableSize_Post_Value_NA
} CReqMallocUsableSize_Post_Value;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Ptr_NA : 1;
  uint8_t Post_Value : 2;
} CReqMallocUsableSize_Entry;

#define REQUEST_SIZE 13

/**
 * @brief Test context for spec:/c/req/malloc-usable-size test case.
 */
typedef struct {
  /**
   * @brief This member contains the heap of the test.
   */
  HeapContext heap;

  /**
   * @brief This member contains the begin address of the block after the block
   *   referenced by the `ptr` parameter.
   */
  void *next;

  /**
   * @brief This member contains the return value of the malloc_usable_size()
   *   call.
   */
  size_t value;

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
    CReqMallocUsableSize_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CReqMallocUsableSize_Context;

static CReqMallocUsableSize_Context CReqMallocUsableSize_Instance;

static const char *const CReqMallocUsableSize_PreDesc_Ptr[] =
  { "Null", "Used", "Invalid", "NA" };

static const char *const *const CReqMallocUsableSize_PreDesc[] = {
  CReqMallocUsableSize_PreDesc_Ptr,
  NULL
};

static void CReqMallocUsableSize_Pre_Ptr_Prepare(
  CReqMallocUsableSize_Context *ctx,
  CReqMallocUsableSize_Pre_Ptr  state
)
{
  switch ( state ) {
    case CReqMallocUsableSize_Pre_Ptr_Null: {
      /*
       * While the `ptr` parameter is equal to NULL.
       */
      ctx->ptr = NULL;
      break;
    }

    case CReqMallocUsableSize_Pre_Ptr_Used: {
      /*
       * While the `ptr` parameter is the begin address of a used block of the
       * C Program Heap.
       */
      ctx->ptr = malloc( REQUEST_SIZE );
      T_quiet_not_null( ctx->ptr );
      ctx->next = malloc( 1 );
      T_quiet_not_null( ctx->next );
      T_quiet_gt_uptr( (uintptr_t) ctx->next, (uintptr_t) ctx->ptr );
      break;
    }

    case CReqMallocUsableSize_Pre_Ptr_Invalid: {
      /*
       * While the `ptr` parameter is not equal to NULL, while the `ptr`
       * parameter is not the begin address of a used block of the C Program
       * Heap.
       */
      ctx->ptr = HeapGetArea();
      break;
    }

    case CReqMallocUsableSize_Pre_Ptr_NA:
      break;
  }
}

static void CReqMallocUsableSize_Post_Value_Check(
  CReqMallocUsableSize_Context   *ctx,
  CReqMallocUsableSize_Post_Value state
)
{
  switch ( state ) {
    case CReqMallocUsableSize_Post_Value_Zero: {
      /*
       * The return value of malloc_usable_size() shall be equal to zero.
       */
      T_eq_sz( ctx->value, 0 );
      break;
    }

    case CReqMallocUsableSize_Post_Value_Usable: {
      /*
       * The return value of malloc_usable_size() shall be a size of at least
       * the size of the request which returned the used block referenced by
       * the `ptr` parameter, up to the end of this block.
       */
      T_ge_sz( ctx->value, REQUEST_SIZE );
      T_le_uptr( (uintptr_t) ctx->ptr + ctx->value, (uintptr_t) ctx->next );
      break;
    }

    case CReqMallocUsableSize_Post_Value_NA:
      break;
  }
}

static void CReqMallocUsableSize_Setup( CReqMallocUsableSize_Context *ctx )
{
  HeapInstall( &ctx->heap );
}

static void CReqMallocUsableSize_Setup_Wrap( void *arg )
{
  CReqMallocUsableSize_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CReqMallocUsableSize_Setup( ctx );
}

static void CReqMallocUsableSize_Teardown( CReqMallocUsableSize_Context *ctx )
{
  HeapRestore( &ctx->heap );
}

static void CReqMallocUsableSize_Teardown_Wrap( void *arg )
{
  CReqMallocUsableSize_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CReqMallocUsableSize_Teardown( ctx );
}

static void CReqMallocUsableSize_Prepare( CReqMallocUsableSize_Context *ctx )
{
  HeapReset( &ctx->heap );
}

static void CReqMallocUsableSize_Action( CReqMallocUsableSize_Context *ctx )
{
  ctx->value = malloc_usable_size( ctx->ptr );
}

/* clang-format off */

static const CReqMallocUsableSize_Entry
CReqMallocUsableSize_Entries[] = {
  { 0, 0, CReqMallocUsableSize_Post_Value_Zero },
  { 0, 0, CReqMallocUsableSize_Post_Value_Usable }
};

static const uint8_t
CReqMallocUsableSize_Map[] = {
  0, 1, 0
};

/* clang-format on */

static size_t CReqMallocUsableSize_Scope( void *arg, char *buf, size_t n )
{
  CReqMallocUsableSize_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CReqMallocUsableSize_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CReqMallocUsableSize_Fixture = {
  .setup = CReqMallocUsableSize_Setup_Wrap,
  .stop = NULL,
  .teardown = CReqMallocUsableSize_Teardown_Wrap,
  .scope = CReqMallocUsableSize_Scope,
  .initial_context = &CReqMallocUsableSize_Instance
};

static inline CReqMallocUsableSize_Entry CReqMallocUsableSize_PopEntry(
  CReqMallocUsableSize_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CReqMallocUsableSize_Entries[ CReqMallocUsableSize_Map[ index ] ];
}

static void CReqMallocUsableSize_TestVariant(
  CReqMallocUsableSize_Context *ctx
)
{
  CReqMallocUsableSize_Pre_Ptr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CReqMallocUsableSize_Action( ctx );
  CReqMallocUsableSize_Post_Value_Check( ctx, ctx->Map.entry.Post_Value );
}

/**
 * @fn void T_case_body_CReqMallocUsableSize( void )
 */
T_TEST_CASE_FIXTURE( CReqMallocUsableSize, &CReqMallocUsableSize_Fixture )
{
  CReqMallocUsableSize_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CReqMallocUsableSize_Pre_Ptr_Null;
    ctx->Map.pcs[ 0 ] < CReqMallocUsableSize_Pre_Ptr_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CReqMallocUsableSize_PopEntry( ctx );
    CReqMallocUsableSize_Prepare( ctx );
    CReqMallocUsableSize_TestVariant( ctx );
  }
}

/** @} */
