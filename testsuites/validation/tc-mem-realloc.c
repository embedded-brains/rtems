/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CReqRealloc
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
#include <stdlib.h>
#include <string.h>

#include "tx-heap.h"

#include <rtems/test.h>

/**
 * @defgroup CReqRealloc spec:/c/req/realloc
 *
 * @ingroup TestsuitesValidation0
 *
 * @{
 */

typedef enum {
  CReqRealloc_Pre_Ptr_Null,
  CReqRealloc_Pre_Ptr_Used,
  CReqRealloc_Pre_Ptr_Invalid,
  CReqRealloc_Pre_Ptr_NA
} CReqRealloc_Pre_Ptr;

typedef enum {
  CReqRealloc_Pre_Size_Zero,
  CReqRealloc_Pre_Size_Smaller,
  CReqRealloc_Pre_Size_Larger,
  CReqRealloc_Pre_Size_NA
} CReqRealloc_Pre_Size;

typedef enum {
  CReqRealloc_Pre_Neighbour_Suffices,
  CReqRealloc_Pre_Neighbour_Insufficient,
  CReqRealloc_Pre_Neighbour_NA
} CReqRealloc_Pre_Neighbour;

typedef enum {
  CReqRealloc_Pre_Fit_Yes,
  CReqRealloc_Pre_Fit_No,
  CReqRealloc_Pre_Fit_NA
} CReqRealloc_Pre_Fit;

typedef enum {
  CReqRealloc_Post_Status_Same,
  CReqRealloc_Post_Status_Other,
  CReqRealloc_Post_Status_Null,
  CReqRealloc_Post_Status_NA
} CReqRealloc_Post_Status;

typedef enum {
  CReqRealloc_Post_Errno_Nop,
  CReqRealloc_Post_Errno_ENOMEM,
  CReqRealloc_Post_Errno_EINVAL,
  CReqRealloc_Post_Errno_NA
} CReqRealloc_Post_Errno;

typedef enum {
  CReqRealloc_Post_Content_Kept,
  CReqRealloc_Post_Content_NA
} CReqRealloc_Post_Content;

typedef enum {
  CReqRealloc_Post_Size_Valid,
  CReqRealloc_Post_Size_NA
} CReqRealloc_Post_Size;

typedef enum {
  CReqRealloc_Post_Old_Kept,
  CReqRealloc_Post_Old_Freed,
  CReqRealloc_Post_Old_NA
} CReqRealloc_Post_Old;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Ptr_NA : 1;
  uint16_t Pre_Size_NA : 1;
  uint16_t Pre_Neighbour_NA : 1;
  uint16_t Pre_Fit_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Errno : 2;
  uint16_t Post_Content : 1;
  uint16_t Post_Size : 1;
  uint16_t Post_Old : 2;
} CReqRealloc_Entry;

#define ERRNO_SENTINEL -1

#define OLD_SIZE 32

#define PATTERN 0x5a

/**
 * @brief Test context for spec:/c/req/realloc test case.
 */
typedef struct {
  /**
   * @brief This member contains the heap of the test.
   */
  HeapContext heap;

  /**
   * @brief This member contains the return value of the realloc() call.
   */
  void *ptr;

  /**
   * @brief This member contains the block after the memory area referenced by
   *   the `ptr` parameter.
   */
  void *neighbour;

  /**
   * @brief This member contains the size of the block referenced by the `ptr`
   *   parameter.
   */
  uintptr_t old_size;

  /**
   * @brief This member contains the value of errno after the realloc() call.
   */
  int errno_value;

  /**
   * @brief This member specifies the `ptr` parameter value.
   */
  void *old;

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
    CReqRealloc_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CReqRealloc_Context;

static CReqRealloc_Context CReqRealloc_Instance;

static const char *const CReqRealloc_PreDesc_Ptr[] =
  { "Null", "Used", "Invalid", "NA" };

static const char *const CReqRealloc_PreDesc_Size[] =
  { "Zero", "Smaller", "Larger", "NA" };

static const char *const CReqRealloc_PreDesc_Neighbour[] =
  { "Suffices", "Insufficient", "NA" };

static const char *const CReqRealloc_PreDesc_Fit[] = { "Yes", "No", "NA" };

static const char *const *const CReqRealloc_PreDesc[] = {
  CReqRealloc_PreDesc_Ptr,
  CReqRealloc_PreDesc_Size,
  CReqRealloc_PreDesc_Neighbour,
  CReqRealloc_PreDesc_Fit,
  NULL
};

static bool HasPattern( const void *ptr, size_t size )
{
  const unsigned char *bytes;
  size_t               i;

  bytes = ptr;

  for ( i = 0; i < size; ++i ) {
    if ( bytes[ i ] != PATTERN ) {
      return false;
    }
  }

  return true;
}

static void CReqRealloc_Pre_Ptr_Prepare(
  CReqRealloc_Context *ctx,
  CReqRealloc_Pre_Ptr  state
)
{
  switch ( state ) {
    case CReqRealloc_Pre_Ptr_Null: {
      /*
       * While the `ptr` parameter is equal to NULL.
       */
      ctx->old = NULL;
      break;
    }

    case CReqRealloc_Pre_Ptr_Used: {
      /*
       * While the `ptr` parameter is the begin address of a used block of the
       * C Program Heap.
       */
      ctx->old = malloc( OLD_SIZE );
      T_quiet_not_null( ctx->old );
      memset( ctx->old, PATTERN, OLD_SIZE );
      T_quiet_true(
        _RBHeap_Size_of_block( &ctx->heap.heap, ctx->old, &ctx->old_size )
      );
      break;
    }

    case CReqRealloc_Pre_Ptr_Invalid: {
      /*
       * While the `ptr` parameter is not equal to NULL, while the `ptr`
       * parameter is not the begin address of a used block of the C Program
       * Heap.
       */
      ctx->old = HeapGetArea();
      break;
    }

    case CReqRealloc_Pre_Ptr_NA:
      break;
  }
}

static void CReqRealloc_Pre_Size_Prepare(
  CReqRealloc_Context *ctx,
  CReqRealloc_Pre_Size state
)
{
  switch ( state ) {
    case CReqRealloc_Pre_Size_Zero: {
      /*
       * While the `size` parameter is equal to zero.
       */
      ctx->size = 0;
      break;
    }

    case CReqRealloc_Pre_Size_Smaller: {
      /*
       * While the `size` parameter is not equal to zero, while the `size`
       * parameter is less than or equal to the size of the memory area
       * referenced by the `ptr` parameter.
       */
      ctx->size = OLD_SIZE / 2;
      break;
    }

    case CReqRealloc_Pre_Size_Larger: {
      /*
       * While the `size` parameter is greater than the size of the memory area
       * referenced by the `ptr` parameter.
       */
      ctx->size = 2 * OLD_SIZE;
      break;
    }

    case CReqRealloc_Pre_Size_NA:
      break;
  }
}

static void CReqRealloc_Pre_Neighbour_Prepare(
  CReqRealloc_Context      *ctx,
  CReqRealloc_Pre_Neighbour state
)
{
  switch ( state ) {
    case CReqRealloc_Pre_Neighbour_Suffices: {
      /*
       * While the C Program Heap has a free block which begins at the end of
       * the memory area referenced by the `ptr` parameter and which holds the
       * growth of the memory area.
       */
      /* The free block after the block is large */
      break;
    }

    case CReqRealloc_Pre_Neighbour_Insufficient: {
      /*
       * While the C Program Heap has no free block which begins at the end of
       * the memory area referenced by the `ptr` parameter and which holds the
       * growth of the memory area.
       */
      ctx->neighbour = malloc( 1 );
      T_quiet_not_null( ctx->neighbour );
      break;
    }

    case CReqRealloc_Pre_Neighbour_NA:
      break;
  }
}

static void CReqRealloc_Pre_Fit_Prepare(
  CReqRealloc_Context *ctx,
  CReqRealloc_Pre_Fit  state
)
{
  switch ( state ) {
    case CReqRealloc_Pre_Fit_Yes: {
      /*
       * While the C Program Heap has a free block which satisfies a request of
       * the size specified by the `size` parameter.
       */
      /* The heap has free blocks */
      break;
    }

    case CReqRealloc_Pre_Fit_No: {
      /*
       * While the C Program Heap has no free block which satisfies a request
       * of the size specified by the `size` parameter.
       */
      HeapExhaust( &ctx->heap.heap );
      break;
    }

    case CReqRealloc_Pre_Fit_NA:
      break;
  }
}

static void CReqRealloc_Post_Status_Check(
  CReqRealloc_Context    *ctx,
  CReqRealloc_Post_Status state
)
{
  switch ( state ) {
    case CReqRealloc_Post_Status_Same: {
      /*
       * The return value of realloc() shall be equal to the `ptr` parameter.
       */
      T_eq_ptr( ctx->ptr, ctx->old );
      break;
    }

    case CReqRealloc_Post_Status_Other: {
      /*
       * The return value of realloc() shall be the begin address of a used
       * block of the C Program Heap other than the memory area referenced by
       * the `ptr` parameter.
       */
      T_not_null( ctx->ptr );
      T_ne_ptr( ctx->ptr, ctx->old );
      break;
    }

    case CReqRealloc_Post_Status_Null: {
      /*
       * The return value of realloc() shall be equal to NULL.
       */
      T_null( ctx->ptr );
      break;
    }

    case CReqRealloc_Post_Status_NA:
      break;
  }
}

static void CReqRealloc_Post_Errno_Check(
  CReqRealloc_Context   *ctx,
  CReqRealloc_Post_Errno state
)
{
  switch ( state ) {
    case CReqRealloc_Post_Errno_Nop: {
      /*
       * The value of errno shall not be changed by the realloc() call.
       */
      T_eq_int( ctx->errno_value, ERRNO_SENTINEL );
      break;
    }

    case CReqRealloc_Post_Errno_ENOMEM: {
      /*
       * The value of errno shall be equal to ENOMEM.
       */
      T_eq_int( ctx->errno_value, ENOMEM );
      break;
    }

    case CReqRealloc_Post_Errno_EINVAL: {
      /*
       * The value of errno shall be equal to EINVAL.
       */
      T_eq_int( ctx->errno_value, EINVAL );
      break;
    }

    case CReqRealloc_Post_Errno_NA:
      break;
  }
}

static void CReqRealloc_Post_Content_Check(
  CReqRealloc_Context     *ctx,
  CReqRealloc_Post_Content state
)
{
  switch ( state ) {
    case CReqRealloc_Post_Content_Kept: {
      /*
       * The memory area referenced by the return value shall begin with the
       * content of the memory area referenced by the `ptr` parameter up to the
       * lesser of the old and the new size.
       */
      T_true(
        HasPattern( ctx->ptr, OLD_SIZE < ctx->size ? OLD_SIZE : ctx->size )
      );
      break;
    }

    case CReqRealloc_Post_Content_NA:
      break;
  }
}

static void CReqRealloc_Post_Size_Check(
  CReqRealloc_Context  *ctx,
  CReqRealloc_Post_Size state
)
{
  uintptr_t size;

  switch ( state ) {
    case CReqRealloc_Post_Size_Valid: {
      /*
       * The size of the used block referenced by the return value shall be
       * greater than or equal to the `size` parameter.
       */
      T_true( _RBHeap_Size_of_block( &ctx->heap.heap, ctx->ptr, &size ) );
      T_ge_uptr( size, ctx->size );
      break;
    }

    case CReqRealloc_Post_Size_NA:
      break;
  }
}

static void CReqRealloc_Post_Old_Check(
  CReqRealloc_Context *ctx,
  CReqRealloc_Post_Old state
)
{
  uintptr_t size;

  switch ( state ) {
    case CReqRealloc_Post_Old_Kept: {
      /*
       * The memory area referenced by the `ptr` parameter shall be a used
       * block of unchanged size and content.
       */
      T_true( _RBHeap_Size_of_block( &ctx->heap.heap, ctx->old, &size ) );
      T_eq_uptr( size, ctx->old_size );
      T_true( HasPattern( ctx->old, OLD_SIZE ) );
      break;
    }

    case CReqRealloc_Post_Old_Freed: {
      /*
       * The memory area referenced by the `ptr` parameter shall not be a used
       * block.
       */
      T_false( _RBHeap_Size_of_block( &ctx->heap.heap, ctx->old, &size ) );
      break;
    }

    case CReqRealloc_Post_Old_NA:
      break;
  }
}

static void CReqRealloc_Setup( CReqRealloc_Context *ctx )
{
  HeapInstall( &ctx->heap );
}

static void CReqRealloc_Setup_Wrap( void *arg )
{
  CReqRealloc_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CReqRealloc_Setup( ctx );
}

static void CReqRealloc_Teardown( CReqRealloc_Context *ctx )
{
  HeapRestore( &ctx->heap );
}

static void CReqRealloc_Teardown_Wrap( void *arg )
{
  CReqRealloc_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CReqRealloc_Teardown( ctx );
}

static void CReqRealloc_Prepare( CReqRealloc_Context *ctx )
{
  HeapReset( &ctx->heap );
}

static void CReqRealloc_Action( CReqRealloc_Context *ctx )
{
  errno = ERRNO_SENTINEL;
  ctx->ptr = realloc( ctx->old, ctx->size );
  ctx->errno_value = errno;
}

/* clang-format off */

static const CReqRealloc_Entry
CReqRealloc_Entries[] = {
  { 0, 0, 0, 0, 0, CReqRealloc_Post_Status_Null, CReqRealloc_Post_Errno_EINVAL,
    CReqRealloc_Post_Content_NA, CReqRealloc_Post_Size_NA,
    CReqRealloc_Post_Old_NA },
  { 0, 0, 0, 0, 0, CReqRealloc_Post_Status_Null, CReqRealloc_Post_Errno_Nop,
    CReqRealloc_Post_Content_NA, CReqRealloc_Post_Size_NA,
    CReqRealloc_Post_Old_NA },
  { 0, 0, 0, 0, 0, CReqRealloc_Post_Status_Other, CReqRealloc_Post_Errno_Nop,
    CReqRealloc_Post_Content_NA, CReqRealloc_Post_Size_Valid,
    CReqRealloc_Post_Old_NA },
  { 0, 0, 0, 0, 0, CReqRealloc_Post_Status_Null, CReqRealloc_Post_Errno_ENOMEM,
    CReqRealloc_Post_Content_NA, CReqRealloc_Post_Size_NA,
    CReqRealloc_Post_Old_NA },
  { 0, 0, 0, 0, 0, CReqRealloc_Post_Status_Same, CReqRealloc_Post_Errno_Nop,
    CReqRealloc_Post_Content_Kept, CReqRealloc_Post_Size_Valid,
    CReqRealloc_Post_Old_NA },
  { 1, 0, 0, 0, 0, CReqRealloc_Post_Status_NA, CReqRealloc_Post_Errno_NA,
    CReqRealloc_Post_Content_NA, CReqRealloc_Post_Size_NA,
    CReqRealloc_Post_Old_NA },
  { 0, 0, 0, 0, 0, CReqRealloc_Post_Status_Null, CReqRealloc_Post_Errno_Nop,
    CReqRealloc_Post_Content_NA, CReqRealloc_Post_Size_NA,
    CReqRealloc_Post_Old_Freed },
  { 1, 0, 0, 0, 0, CReqRealloc_Post_Status_NA, CReqRealloc_Post_Errno_NA,
    CReqRealloc_Post_Content_NA, CReqRealloc_Post_Size_NA,
    CReqRealloc_Post_Old_NA },
  { 0, 0, 0, 0, 0, CReqRealloc_Post_Status_Other, CReqRealloc_Post_Errno_Nop,
    CReqRealloc_Post_Content_Kept, CReqRealloc_Post_Size_Valid,
    CReqRealloc_Post_Old_Freed },
  { 0, 0, 0, 0, 0, CReqRealloc_Post_Status_Null, CReqRealloc_Post_Errno_ENOMEM,
    CReqRealloc_Post_Content_NA, CReqRealloc_Post_Size_NA,
    CReqRealloc_Post_Old_Kept }
};

static const uint8_t
CReqRealloc_Map[] = {
  1, 1, 1, 1, 2, 3, 2, 3, 2, 3, 2, 3, 6, 7, 6, 6, 4, 7, 4, 4, 4, 7, 8, 9, 5, 5,
  5, 5, 0, 0, 0, 0, 0, 0, 0, 0
};

/* clang-format on */

static size_t CReqRealloc_Scope( void *arg, char *buf, size_t n )
{
  CReqRealloc_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CReqRealloc_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CReqRealloc_Fixture = {
  .setup = CReqRealloc_Setup_Wrap,
  .stop = NULL,
  .teardown = CReqRealloc_Teardown_Wrap,
  .scope = CReqRealloc_Scope,
  .initial_context = &CReqRealloc_Instance
};

static inline CReqRealloc_Entry CReqRealloc_PopEntry(
  CReqRealloc_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CReqRealloc_Entries[ CReqRealloc_Map[ index ] ];
}

static void CReqRealloc_TestVariant( CReqRealloc_Context *ctx )
{
  CReqRealloc_Pre_Ptr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CReqRealloc_Pre_Size_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CReqRealloc_Pre_Neighbour_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CReqRealloc_Pre_Fit_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CReqRealloc_Action( ctx );
  CReqRealloc_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CReqRealloc_Post_Errno_Check( ctx, ctx->Map.entry.Post_Errno );
  CReqRealloc_Post_Content_Check( ctx, ctx->Map.entry.Post_Content );
  CReqRealloc_Post_Size_Check( ctx, ctx->Map.entry.Post_Size );
  CReqRealloc_Post_Old_Check( ctx, ctx->Map.entry.Post_Old );
}

/**
 * @fn void T_case_body_CReqRealloc( void )
 */
T_TEST_CASE_FIXTURE( CReqRealloc, &CReqRealloc_Fixture )
{
  CReqRealloc_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CReqRealloc_Pre_Ptr_Null;
    ctx->Map.pcs[ 0 ] < CReqRealloc_Pre_Ptr_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CReqRealloc_Pre_Size_Zero;
      ctx->Map.pcs[ 1 ] < CReqRealloc_Pre_Size_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = CReqRealloc_Pre_Neighbour_Suffices;
        ctx->Map.pcs[ 2 ] < CReqRealloc_Pre_Neighbour_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = CReqRealloc_Pre_Fit_Yes;
          ctx->Map.pcs[ 3 ] < CReqRealloc_Pre_Fit_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          ctx->Map.entry = CReqRealloc_PopEntry( ctx );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          CReqRealloc_Prepare( ctx );
          CReqRealloc_TestVariant( ctx );
        }
      }
    }
  }
}

/** @} */
