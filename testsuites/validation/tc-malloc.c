/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStdlibReqMalloc
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
 * @defgroup CStdlibReqMalloc spec:/c/stdlib/req/malloc
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CStdlibReqMalloc_Pre_Size_Huge,
  CStdlibReqMalloc_Pre_Size_Zero,
  CStdlibReqMalloc_Pre_Size_Valid,
  CStdlibReqMalloc_Pre_Size_NA
} CStdlibReqMalloc_Pre_Size;

typedef enum {
  CStdlibReqMalloc_Post_Status_Null,
  CStdlibReqMalloc_Post_Status_AreaBegin,
  CStdlibReqMalloc_Post_Status_NA
} CStdlibReqMalloc_Post_Status;

typedef enum {
  CStdlibReqMalloc_Post_Errno_ENOMEM,
  CStdlibReqMalloc_Post_Errno_Nop,
  CStdlibReqMalloc_Post_Errno_NA
} CStdlibReqMalloc_Post_Errno;

typedef enum {
  CStdlibReqMalloc_Post_Alignment_Valid,
  CStdlibReqMalloc_Post_Alignment_NA
} CStdlibReqMalloc_Post_Alignment;

typedef enum {
  CStdlibReqMalloc_Post_Size_Valid,
  CStdlibReqMalloc_Post_Size_NA
} CStdlibReqMalloc_Post_Size;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Size_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_Errno : 2;
  uint8_t Post_Alignment : 1;
  uint8_t Post_Size : 1;
} CStdlibReqMalloc_Entry;

/**
 * @brief Test context for spec:/c/stdlib/req/malloc test case.
 */
typedef struct {
  /**
   * @brief This member provides a memory support context.
   */
  MemoryContext mem_ctx;

  /**
   * @brief This member contains the return value of the malloc() call.
   */
  void *ptr;

  /**
   * @brief This member specifies if the ``size`` parameter value.
   */
  size_t size;

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
    CStdlibReqMalloc_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStdlibReqMalloc_Context;

static CStdlibReqMalloc_Context
  CStdlibReqMalloc_Instance;

static const char * const CStdlibReqMalloc_PreDesc_Size[] = {
  "Huge",
  "Zero",
  "Valid",
  "NA"
};

static const char * const * const CStdlibReqMalloc_PreDesc[] = {
  CStdlibReqMalloc_PreDesc_Size,
  NULL
};

static void CStdlibReqMalloc_Pre_Size_Prepare(
  CStdlibReqMalloc_Context *ctx,
  CStdlibReqMalloc_Pre_Size state
)
{
  switch ( state ) {
    case CStdlibReqMalloc_Pre_Size_Huge: {
      /*
       * While the ``size`` parameter is not equal to zero, while the ``size``
       * parameter is too large to allocate a memory area with the specified
       * size.
       */
      ctx->size = SIZE_MAX;
      break;
    }

    case CStdlibReqMalloc_Pre_Size_Zero: {
      /*
       * While the ``size`` parameter is equal to zero.
       */
      ctx->size = 0;
      break;
    }

    case CStdlibReqMalloc_Pre_Size_Valid: {
      /*
       * While the ``size`` parameter is not equal to zero, while the ``size``
       * parameter is small enough to allocate a memory area with the specified
       * size.
       */
      ctx->size = 1;
      break;
    }

    case CStdlibReqMalloc_Pre_Size_NA:
      break;
  }
}

static void CStdlibReqMalloc_Post_Status_Check(
  CStdlibReqMalloc_Context    *ctx,
  CStdlibReqMalloc_Post_Status state
)
{
  switch ( state ) {
    case CStdlibReqMalloc_Post_Status_Null: {
      /*
       * The return value of malloc() shall be equal to NULL.
       */
      T_null( ctx->ptr );
      break;
    }

    case CStdlibReqMalloc_Post_Status_AreaBegin: {
      /*
       * The return value of malloc() shall be equal to the begin address of
       * the allocated memory area.
       */
      T_not_null( ctx->ptr );
      break;
    }

    case CStdlibReqMalloc_Post_Status_NA:
      break;
  }
}

static void CStdlibReqMalloc_Post_Errno_Check(
  CStdlibReqMalloc_Post_Errno state
)
{
  switch ( state ) {
    case CStdlibReqMalloc_Post_Errno_ENOMEM: {
      /*
       * The errno shall be set to ENOMEM.
       */
      T_eq_int( errno, ENOMEM );
      break;
    }

    case CStdlibReqMalloc_Post_Errno_Nop: {
      /*
       * The errno shall not be modified.
       */
      T_eq_int( errno, -1 );
      break;
    }

    case CStdlibReqMalloc_Post_Errno_NA:
      break;
  }
}

static void CStdlibReqMalloc_Post_Alignment_Check(
  CStdlibReqMalloc_Context       *ctx,
  CStdlibReqMalloc_Post_Alignment state
)
{
  switch ( state ) {
    case CStdlibReqMalloc_Post_Alignment_Valid: {
      /*
       * The begin address of the allocated memory area shall be an integral
       * multiple of the heap alignment of the target architecture.
       */
      T_eq_uptr( (uintptr_t) ctx->ptr % CPU_HEAP_ALIGNMENT, 0 );
      break;
    }

    case CStdlibReqMalloc_Post_Alignment_NA:
      break;
  }
}

static void CStdlibReqMalloc_Post_Size_Check(
  CStdlibReqMalloc_Context  *ctx,
  CStdlibReqMalloc_Post_Size state
)
{
  void     *ptr;
  uintptr_t a;
  uintptr_t b;
  uintptr_t size;

  switch ( state ) {
    case CStdlibReqMalloc_Post_Size_Valid: {
      /*
       * The size of the allocated memory area shall greater than or equal to
       * the ``size`` parameter.
       */
      /* Assume that the next allocation is done from adjacent memory */
      ptr = ctx->ptr;
      ctx->ptr = malloc( ctx->size );
      T_not_null( ptr );
      a = (uintptr_t) ptr;
      b = (uintptr_t) ctx->ptr;
      size = a < b ? b - a : a - b;
      T_ge_uptr( size, ctx->size );
      break;
    }

    case CStdlibReqMalloc_Post_Size_NA:
      break;
  }
}

static void CStdlibReqMalloc_Setup( CStdlibReqMalloc_Context *ctx )
{
  MemorySave( &ctx->mem_ctx );
}

static void CStdlibReqMalloc_Setup_Wrap( void *arg )
{
  CStdlibReqMalloc_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CStdlibReqMalloc_Setup( ctx );
}

static void CStdlibReqMalloc_Teardown( CStdlibReqMalloc_Context *ctx )
{
  MemoryRestore( &ctx->mem_ctx );
}

static void CStdlibReqMalloc_Teardown_Wrap( void *arg )
{
  CStdlibReqMalloc_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CStdlibReqMalloc_Teardown( ctx );
}

static void CStdlibReqMalloc_Action( CStdlibReqMalloc_Context *ctx )
{
  errno = -1;
  ctx->ptr = malloc( ctx->size );
}

static const CStdlibReqMalloc_Entry
CStdlibReqMalloc_Entries[] = {
  { 0, 0, CStdlibReqMalloc_Post_Status_Null,
    CStdlibReqMalloc_Post_Errno_ENOMEM, CStdlibReqMalloc_Post_Alignment_Valid,
    CStdlibReqMalloc_Post_Size_NA },
  { 0, 0, CStdlibReqMalloc_Post_Status_Null, CStdlibReqMalloc_Post_Errno_Nop,
    CStdlibReqMalloc_Post_Alignment_Valid, CStdlibReqMalloc_Post_Size_NA },
  { 0, 0, CStdlibReqMalloc_Post_Status_AreaBegin,
    CStdlibReqMalloc_Post_Errno_Nop, CStdlibReqMalloc_Post_Alignment_Valid,
    CStdlibReqMalloc_Post_Size_Valid }
};

static const uint8_t
CStdlibReqMalloc_Map[] = {
  0, 1, 2
};

static size_t CStdlibReqMalloc_Scope( void *arg, char *buf, size_t n )
{
  CStdlibReqMalloc_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CStdlibReqMalloc_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CStdlibReqMalloc_Fixture = {
  .setup = CStdlibReqMalloc_Setup_Wrap,
  .stop = NULL,
  .teardown = CStdlibReqMalloc_Teardown_Wrap,
  .scope = CStdlibReqMalloc_Scope,
  .initial_context = &CStdlibReqMalloc_Instance
};

static inline CStdlibReqMalloc_Entry CStdlibReqMalloc_PopEntry(
  CStdlibReqMalloc_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStdlibReqMalloc_Entries[
    CStdlibReqMalloc_Map[ index ]
  ];
}

static void CStdlibReqMalloc_TestVariant( CStdlibReqMalloc_Context *ctx )
{
  CStdlibReqMalloc_Pre_Size_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStdlibReqMalloc_Action( ctx );
  CStdlibReqMalloc_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CStdlibReqMalloc_Post_Errno_Check( ctx->Map.entry.Post_Errno );
  CStdlibReqMalloc_Post_Alignment_Check( ctx, ctx->Map.entry.Post_Alignment );
  CStdlibReqMalloc_Post_Size_Check( ctx, ctx->Map.entry.Post_Size );
}

/**
 * @fn void T_case_body_CStdlibReqMalloc( void )
 */
T_TEST_CASE_FIXTURE( CStdlibReqMalloc, &CStdlibReqMalloc_Fixture )
{
  CStdlibReqMalloc_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CStdlibReqMalloc_Pre_Size_Huge;
    ctx->Map.pcs[ 0 ] < CStdlibReqMalloc_Pre_Size_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = CStdlibReqMalloc_PopEntry( ctx );
    CStdlibReqMalloc_TestVariant( ctx );
  }
}

/** @} */
