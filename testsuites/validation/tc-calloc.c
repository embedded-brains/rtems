/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStdlibReqCalloc
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <stdlib.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CStdlibReqCalloc spec:/c/stdlib/req/calloc
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CStdlibReqCalloc_Pre_ElementCount_Huge,
  CStdlibReqCalloc_Pre_ElementCount_Zero,
  CStdlibReqCalloc_Pre_ElementCount_Valid,
  CStdlibReqCalloc_Pre_ElementCount_NA
} CStdlibReqCalloc_Pre_ElementCount;

typedef enum {
  CStdlibReqCalloc_Pre_ElementSize_Huge,
  CStdlibReqCalloc_Pre_ElementSize_Zero,
  CStdlibReqCalloc_Pre_ElementSize_Valid,
  CStdlibReqCalloc_Pre_ElementSize_NA
} CStdlibReqCalloc_Pre_ElementSize;

typedef enum {
  CStdlibReqCalloc_Post_Status_Null,
  CStdlibReqCalloc_Post_Status_AreaBegin,
  CStdlibReqCalloc_Post_Status_NA
} CStdlibReqCalloc_Post_Status;

typedef enum {
  CStdlibReqCalloc_Post_Errno_ENOMEM,
  CStdlibReqCalloc_Post_Errno_Nop,
  CStdlibReqCalloc_Post_Errno_NA
} CStdlibReqCalloc_Post_Errno;

typedef enum {
  CStdlibReqCalloc_Post_Alignment_Valid,
  CStdlibReqCalloc_Post_Alignment_NA
} CStdlibReqCalloc_Post_Alignment;

typedef enum {
  CStdlibReqCalloc_Post_Size_Valid,
  CStdlibReqCalloc_Post_Size_NA
} CStdlibReqCalloc_Post_Size;

typedef enum {
  CStdlibReqCalloc_Post_Content_Zero,
  CStdlibReqCalloc_Post_Content_NA
} CStdlibReqCalloc_Post_Content;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_ElementCount_NA : 1;
  uint16_t Pre_ElementSize_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_Errno : 2;
  uint16_t Post_Alignment : 1;
  uint16_t Post_Size : 1;
  uint16_t Post_Content : 1;
} CStdlibReqCalloc_Entry;

/**
 * @brief Test context for spec:/c/stdlib/req/calloc test case.
 */
typedef struct {
  /**
   * @brief This member provides a memory support context.
   */
  MemoryContext mem_ctx;

  /**
   * @brief This member contains the return value of the calloc() call.
   */
  void *ptr;

  /**
   * @brief This member specifies if the ``nelem`` parameter value.
   */
  size_t nelem;

  /**
   * @brief This member specifies if the ``elsize`` parameter value.
   */
  size_t elsize;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 2 ];

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
    CStdlibReqCalloc_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStdlibReqCalloc_Context;

static CStdlibReqCalloc_Context
  CStdlibReqCalloc_Instance;

static const char * const CStdlibReqCalloc_PreDesc_ElementCount[] = {
  "Huge",
  "Zero",
  "Valid",
  "NA"
};

static const char * const CStdlibReqCalloc_PreDesc_ElementSize[] = {
  "Huge",
  "Zero",
  "Valid",
  "NA"
};

static const char * const * const CStdlibReqCalloc_PreDesc[] = {
  CStdlibReqCalloc_PreDesc_ElementCount,
  CStdlibReqCalloc_PreDesc_ElementSize,
  NULL
};

static void CStdlibReqCalloc_Pre_ElementCount_Prepare(
  CStdlibReqCalloc_Context         *ctx,
  CStdlibReqCalloc_Pre_ElementCount state
)
{
  switch ( state ) {
    case CStdlibReqCalloc_Pre_ElementCount_Huge: {
      /*
       * While the ``nelem`` parameter is not equal to zero, while the
       * ``nelem`` parameter is too large to allocate a memory area with the
       * specified size.
       */
      ctx->nelem = SIZE_MAX;
      break;
    }

    case CStdlibReqCalloc_Pre_ElementCount_Zero: {
      /*
       * While the ``nelem`` parameter is equal to zero.
       */
      ctx->nelem = 0;
      break;
    }

    case CStdlibReqCalloc_Pre_ElementCount_Valid: {
      /*
       * While the ``nelem`` parameter is not equal to zero, while the
       * ``nelem`` parameter is small enough to allocate a memory area with the
       * specified size.
       */
      ctx->nelem = 1;
      break;
    }

    case CStdlibReqCalloc_Pre_ElementCount_NA:
      break;
  }
}

static void CStdlibReqCalloc_Pre_ElementSize_Prepare(
  CStdlibReqCalloc_Context        *ctx,
  CStdlibReqCalloc_Pre_ElementSize state
)
{
  switch ( state ) {
    case CStdlibReqCalloc_Pre_ElementSize_Huge: {
      /*
       * While the ``elsize`` parameter is not equal to zero, while the
       * ``elsize`` parameter is too large to allocate a memory area with the
       * specified size.
       */
      ctx->elsize = SIZE_MAX;
      break;
    }

    case CStdlibReqCalloc_Pre_ElementSize_Zero: {
      /*
       * While the ``elsize`` parameter is equal to zero.
       */
      ctx->elsize = 0;
      break;
    }

    case CStdlibReqCalloc_Pre_ElementSize_Valid: {
      /*
       * While the ``elsize`` parameter is not equal to zero, while the
       * ``elsize`` parameter is small enough to allocate a memory area with
       * the specified size.
       */
      ctx->elsize = sizeof( uint64_t );
      break;
    }

    case CStdlibReqCalloc_Pre_ElementSize_NA:
      break;
  }
}

static void CStdlibReqCalloc_Post_Status_Check(
  CStdlibReqCalloc_Context    *ctx,
  CStdlibReqCalloc_Post_Status state
)
{
  switch ( state ) {
    case CStdlibReqCalloc_Post_Status_Null: {
      /*
       * The return value of calloc() shall be equal to NULL.
       */
      T_null( ctx->ptr );
      break;
    }

    case CStdlibReqCalloc_Post_Status_AreaBegin: {
      /*
       * The return value of calloc() shall be equal to the begin address of
       * the allocated memory area.
       */
      T_not_null( ctx->ptr );
      break;
    }

    case CStdlibReqCalloc_Post_Status_NA:
      break;
  }
}

static void CStdlibReqCalloc_Post_Errno_Check(
  CStdlibReqCalloc_Post_Errno state
)
{
  switch ( state ) {
    case CStdlibReqCalloc_Post_Errno_ENOMEM: {
      /*
       * The errno shall be set to ENOMEM.
       */
      T_eq_int( errno, ENOMEM );
      break;
    }

    case CStdlibReqCalloc_Post_Errno_Nop: {
      /*
       * The errno shall not be modified.
       */
      T_eq_int( errno, -1 );
      break;
    }

    case CStdlibReqCalloc_Post_Errno_NA:
      break;
  }
}

static void CStdlibReqCalloc_Post_Alignment_Check(
  CStdlibReqCalloc_Context       *ctx,
  CStdlibReqCalloc_Post_Alignment state
)
{
  switch ( state ) {
    case CStdlibReqCalloc_Post_Alignment_Valid: {
      /*
       * The begin address of the allocated memory area shall be an integral
       * multiple of the heap alignment of the target architecture.
       */
      T_eq_uptr( (uintptr_t) ctx->ptr % CPU_HEAP_ALIGNMENT, 0 );
      break;
    }

    case CStdlibReqCalloc_Post_Alignment_NA:
      break;
  }
}

static void CStdlibReqCalloc_Post_Size_Check(
  CStdlibReqCalloc_Context  *ctx,
  CStdlibReqCalloc_Post_Size state
)
{
  void     *ptr;
  uintptr_t a;
  uintptr_t b;
  uintptr_t size;

  switch ( state ) {
    case CStdlibReqCalloc_Post_Size_Valid: {
      /*
       * The size of the allocated memory area shall greater than or equal to
       * the product of the ``nelem`` and ``elsize`` parameters.
       */
      /* Assume that the next allocation is done from adjacent memory */
      ptr = ctx->ptr;
      ctx->ptr = calloc( ctx->nelem, ctx->elsize );
      T_not_null( ptr );
      a = (uintptr_t) ptr;
      b = (uintptr_t) ctx->ptr;
      size = a < b ? b - a : a - b;
      T_ge_uptr( size, ctx->nelem * ctx->elsize );
      break;
    }

    case CStdlibReqCalloc_Post_Size_NA:
      break;
  }
}

static void CStdlibReqCalloc_Post_Content_Check(
  CStdlibReqCalloc_Context     *ctx,
  CStdlibReqCalloc_Post_Content state
)
{
  switch ( state ) {
    case CStdlibReqCalloc_Post_Content_Zero: {
      /*
       * The content of the allocated memory area shall be cleared to zero.
       */
      T_eq_u64( *(uint64_t *) ctx->ptr, 0 );
      break;
    }

    case CStdlibReqCalloc_Post_Content_NA:
      break;
  }
}

static void CStdlibReqCalloc_Setup( CStdlibReqCalloc_Context *ctx )
{
  MemorySave( &ctx->mem_ctx );
}

static void CStdlibReqCalloc_Setup_Wrap( void *arg )
{
  CStdlibReqCalloc_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CStdlibReqCalloc_Setup( ctx );
}

static void CStdlibReqCalloc_Teardown( CStdlibReqCalloc_Context *ctx )
{
  MemoryRestore( &ctx->mem_ctx );
}

static void CStdlibReqCalloc_Teardown_Wrap( void *arg )
{
  CStdlibReqCalloc_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CStdlibReqCalloc_Teardown( ctx );
}

static void CStdlibReqCalloc_Action( CStdlibReqCalloc_Context *ctx )
{
  errno = -1;
  ctx->ptr = calloc( ctx->nelem, ctx->elsize );
}

static const CStdlibReqCalloc_Entry
CStdlibReqCalloc_Entries[] = {
  { 0, 0, 0, CStdlibReqCalloc_Post_Status_Null,
    CStdlibReqCalloc_Post_Errno_Nop, CStdlibReqCalloc_Post_Alignment_Valid,
    CStdlibReqCalloc_Post_Size_NA, CStdlibReqCalloc_Post_Content_NA },
  { 0, 0, 0, CStdlibReqCalloc_Post_Status_Null,
    CStdlibReqCalloc_Post_Errno_ENOMEM, CStdlibReqCalloc_Post_Alignment_Valid,
    CStdlibReqCalloc_Post_Size_NA, CStdlibReqCalloc_Post_Content_NA },
  { 0, 0, 0, CStdlibReqCalloc_Post_Status_AreaBegin,
    CStdlibReqCalloc_Post_Errno_Nop, CStdlibReqCalloc_Post_Alignment_Valid,
    CStdlibReqCalloc_Post_Size_Valid, CStdlibReqCalloc_Post_Content_Zero }
};

static const uint8_t
CStdlibReqCalloc_Map[] = {
  1, 0, 1, 0, 0, 0, 1, 0, 2
};

static size_t CStdlibReqCalloc_Scope( void *arg, char *buf, size_t n )
{
  CStdlibReqCalloc_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStdlibReqCalloc_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStdlibReqCalloc_Fixture = {
  .setup = CStdlibReqCalloc_Setup_Wrap,
  .stop = NULL,
  .teardown = CStdlibReqCalloc_Teardown_Wrap,
  .scope = CStdlibReqCalloc_Scope,
  .initial_context = &CStdlibReqCalloc_Instance
};

static inline CStdlibReqCalloc_Entry CStdlibReqCalloc_PopEntry(
  CStdlibReqCalloc_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStdlibReqCalloc_Entries[
    CStdlibReqCalloc_Map[ index ]
  ];
}

static void CStdlibReqCalloc_TestVariant( CStdlibReqCalloc_Context *ctx )
{
  CStdlibReqCalloc_Pre_ElementCount_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStdlibReqCalloc_Pre_ElementSize_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CStdlibReqCalloc_Action( ctx );
  CStdlibReqCalloc_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CStdlibReqCalloc_Post_Errno_Check( ctx->Map.entry.Post_Errno );
  CStdlibReqCalloc_Post_Alignment_Check( ctx, ctx->Map.entry.Post_Alignment );
  CStdlibReqCalloc_Post_Size_Check( ctx, ctx->Map.entry.Post_Size );
  CStdlibReqCalloc_Post_Content_Check( ctx, ctx->Map.entry.Post_Content );
}

/**
 * @fn void T_case_body_CStdlibReqCalloc( void )
 */
T_TEST_CASE_FIXTURE( CStdlibReqCalloc, &CStdlibReqCalloc_Fixture )
{
  CStdlibReqCalloc_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CStdlibReqCalloc_Pre_ElementCount_Huge;
    ctx->Map.pcs[ 0 ] < CStdlibReqCalloc_Pre_ElementCount_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CStdlibReqCalloc_Pre_ElementSize_Huge;
      ctx->Map.pcs[ 1 ] < CStdlibReqCalloc_Pre_ElementSize_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = CStdlibReqCalloc_PopEntry( ctx );
      CStdlibReqCalloc_TestVariant( ctx );
    }
  }
}

/** @} */
