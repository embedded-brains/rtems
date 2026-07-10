/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CStdlibReqPosixMemalign
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
 * @defgroup CStdlibReqPosixMemalign spec:/c/stdlib/req/posix-memalign
 *
 * @ingroup TestsuitesValidation0
 *
 * @{
 */

typedef enum {
  CStdlibReqPosixMemalign_Pre_Memptr_Valid,
  CStdlibReqPosixMemalign_Pre_Memptr_Null,
  CStdlibReqPosixMemalign_Pre_Memptr_NA
} CStdlibReqPosixMemalign_Pre_Memptr;

typedef enum {
  CStdlibReqPosixMemalign_Pre_Alignment_Tiny,
  CStdlibReqPosixMemalign_Pre_Alignment_NotPower2,
  CStdlibReqPosixMemalign_Pre_Alignment_Huge,
  CStdlibReqPosixMemalign_Pre_Alignment_Valid,
  CStdlibReqPosixMemalign_Pre_Alignment_NA
} CStdlibReqPosixMemalign_Pre_Alignment;

typedef enum {
  CStdlibReqPosixMemalign_Pre_Size_Huge,
  CStdlibReqPosixMemalign_Pre_Size_Zero,
  CStdlibReqPosixMemalign_Pre_Size_Valid,
  CStdlibReqPosixMemalign_Pre_Size_NA
} CStdlibReqPosixMemalign_Pre_Size;

typedef enum {
  CStdlibReqPosixMemalign_Post_Status_Zero,
  CStdlibReqPosixMemalign_Post_Status_EINVAL,
  CStdlibReqPosixMemalign_Post_Status_ENOMEM,
  CStdlibReqPosixMemalign_Post_Status_NA
} CStdlibReqPosixMemalign_Post_Status;

typedef enum {
  CStdlibReqPosixMemalign_Post_MemptrVar_AreaBegin,
  CStdlibReqPosixMemalign_Post_MemptrVar_Null,
  CStdlibReqPosixMemalign_Post_MemptrVar_Nop,
  CStdlibReqPosixMemalign_Post_MemptrVar_NA
} CStdlibReqPosixMemalign_Post_MemptrVar;

typedef enum {
  CStdlibReqPosixMemalign_Post_Alignment_Valid,
  CStdlibReqPosixMemalign_Post_Alignment_NA
} CStdlibReqPosixMemalign_Post_Alignment;

typedef enum {
  CStdlibReqPosixMemalign_Post_Size_Valid,
  CStdlibReqPosixMemalign_Post_Size_NA
} CStdlibReqPosixMemalign_Post_Size;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Memptr_NA : 1;
  uint16_t Pre_Alignment_NA : 1;
  uint16_t Pre_Size_NA : 1;
  uint16_t Post_Status : 2;
  uint16_t Post_MemptrVar : 2;
  uint16_t Post_Alignment : 1;
  uint16_t Post_Size : 1;
} CStdlibReqPosixMemalign_Entry;

/**
 * @brief Test context for spec:/c/stdlib/req/posix-memalign test case.
 */
typedef struct {
  /**
   * @brief This member provides a memory support context.
   */
  MemoryContext mem_ctx;

  /**
   * @brief This member provides the object referenced by the memptr parameter.
   */
  void *memptr_obj;

  /**
   * @brief This member contains the return value of the directive call.
   */
  int status;

  /**
   * @brief This member specifies if the memptr parameter value.
   */
  void **memptr;

  /**
   * @brief This member specifies if the alignment parameter value.
   */
  size_t alignment;

  /**
   * @brief This member specifies if the size parameter value.
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
    CStdlibReqPosixMemalign_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CStdlibReqPosixMemalign_Context;

static CStdlibReqPosixMemalign_Context
  CStdlibReqPosixMemalign_Instance;

static const char * const CStdlibReqPosixMemalign_PreDesc_Memptr[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const CStdlibReqPosixMemalign_PreDesc_Alignment[] = {
  "Tiny",
  "NotPower2",
  "Huge",
  "Valid",
  "NA"
};

static const char * const CStdlibReqPosixMemalign_PreDesc_Size[] = {
  "Huge",
  "Zero",
  "Valid",
  "NA"
};

static const char * const * const CStdlibReqPosixMemalign_PreDesc[] = {
  CStdlibReqPosixMemalign_PreDesc_Memptr,
  CStdlibReqPosixMemalign_PreDesc_Alignment,
  CStdlibReqPosixMemalign_PreDesc_Size,
  NULL
};

static void CStdlibReqPosixMemalign_Pre_Memptr_Prepare(
  CStdlibReqPosixMemalign_Context   *ctx,
  CStdlibReqPosixMemalign_Pre_Memptr state
)
{
  switch ( state ) {
    case CStdlibReqPosixMemalign_Pre_Memptr_Valid: {
      /*
       * While the memptr parameter references an object of type `void *`.
       */
      ctx->memptr = &ctx->memptr_obj;
      break;
    }

    case CStdlibReqPosixMemalign_Pre_Memptr_Null: {
      /*
       * While the memptr parameter is equal to NULL.
       */
      ctx->memptr = NULL;
      break;
    }

    case CStdlibReqPosixMemalign_Pre_Memptr_NA:
      break;
  }
}

static void CStdlibReqPosixMemalign_Pre_Alignment_Prepare(
  CStdlibReqPosixMemalign_Context      *ctx,
  CStdlibReqPosixMemalign_Pre_Alignment state
)
{
  switch ( state ) {
    case CStdlibReqPosixMemalign_Pre_Alignment_Tiny: {
      /*
       * While the alignment parameter is less than sizeof( void * ).
       */
      ctx->alignment = sizeof( void * ) - 1;
      break;
    }

    case CStdlibReqPosixMemalign_Pre_Alignment_NotPower2: {
      /*
       * While the alignment parameter is greater than or equal to sizeof( void
       * * ), while the alignment parameter is not a power of two.
       */
      ctx->alignment = sizeof( void * ) + 1;
      break;
    }

    case CStdlibReqPosixMemalign_Pre_Alignment_Huge: {
      /*
       * While the alignment parameter is greater than or equal to sizeof( void
       * * ), while the alignment parameter is a power of two, while the
       * alignment parameter is too large to allocate a memory area with the
       * specified alignment.
       */
      ctx->alignment = SIZE_MAX / 2 + 1;
      break;
    }

    case CStdlibReqPosixMemalign_Pre_Alignment_Valid: {
      /*
       * While the alignment parameter is greater than or equal to sizeof( void
       * * ), while the alignment parameter is a power of two, while the
       * alignment parameter is small enough to allocate a memory area with the
       * specified alignment.
       */
      ctx->alignment = 128;
      break;
    }

    case CStdlibReqPosixMemalign_Pre_Alignment_NA:
      break;
  }
}

static void CStdlibReqPosixMemalign_Pre_Size_Prepare(
  CStdlibReqPosixMemalign_Context *ctx,
  CStdlibReqPosixMemalign_Pre_Size state
)
{
  switch ( state ) {
    case CStdlibReqPosixMemalign_Pre_Size_Huge: {
      /*
       * While the size parameter is not equal to zero, while the size
       * parameter is too large to allocate a memory area with the specified
       * size.
       */
      ctx->size = SIZE_MAX;
      break;
    }

    case CStdlibReqPosixMemalign_Pre_Size_Zero: {
      /*
       * While the size parameter is equal to zero.
       */
      ctx->size = 0;
      break;
    }

    case CStdlibReqPosixMemalign_Pre_Size_Valid: {
      /*
       * While the size parameter is not equal to zero, while the size
       * parameter is small enough to allocate a memory area with the specified
       * size.
       */
      ctx->size = sizeof( uint64_t );
      break;
    }

    case CStdlibReqPosixMemalign_Pre_Size_NA:
      break;
  }
}

static void CStdlibReqPosixMemalign_Post_Status_Check(
  CStdlibReqPosixMemalign_Context    *ctx,
  CStdlibReqPosixMemalign_Post_Status state
)
{
  switch ( state ) {
    case CStdlibReqPosixMemalign_Post_Status_Zero: {
      /*
       * The return value of posix_memalign() shall be equal to zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CStdlibReqPosixMemalign_Post_Status_EINVAL: {
      /*
       * The return value of posix_memalign() shall be equal to EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CStdlibReqPosixMemalign_Post_Status_ENOMEM: {
      /*
       * The return value of posix_memalign() shall be equal to ENOMEM.
       */
      T_eq_int( ctx->status, ENOMEM );
      break;
    }

    case CStdlibReqPosixMemalign_Post_Status_NA:
      break;
  }
}

static void CStdlibReqPosixMemalign_Post_MemptrVar_Check(
  CStdlibReqPosixMemalign_Context       *ctx,
  CStdlibReqPosixMemalign_Post_MemptrVar state
)
{
  switch ( state ) {
    case CStdlibReqPosixMemalign_Post_MemptrVar_AreaBegin: {
      /*
       * The value of the object referenced by the memptr parameter shall be
       * set to the begin address of the allocated memory area after the return
       * of the posix_memalign() call.
       */
      T_eq_ptr( ctx->memptr, &ctx->memptr_obj );
      T_not_null( ctx->memptr_obj );
      break;
    }

    case CStdlibReqPosixMemalign_Post_MemptrVar_Null: {
      /*
       * The value of the object referenced by the memptr parameter shall be
       * set to NULL after the return of the posix_memalign() call.
       */
      T_eq_ptr( ctx->memptr, &ctx->memptr_obj );
      T_null( ctx->memptr_obj );
      break;
    }

    case CStdlibReqPosixMemalign_Post_MemptrVar_Nop: {
      /*
       * Objects referenced by the memptr parameter in past calls to
       * posix_memalign() shall not be accessed by the posix_memalign() call.
       */
      T_eq_uptr( (uintptr_t) ctx->memptr_obj, 1 );
      break;
    }

    case CStdlibReqPosixMemalign_Post_MemptrVar_NA:
      break;
  }
}

static void CStdlibReqPosixMemalign_Post_Alignment_Check(
  CStdlibReqPosixMemalign_Context       *ctx,
  CStdlibReqPosixMemalign_Post_Alignment state
)
{
  switch ( state ) {
    case CStdlibReqPosixMemalign_Post_Alignment_Valid: {
      /*
       * The begin address of the allocated memory area shall be an integral
       * multiple of the alignment parameter.
       */
      T_eq_uptr( (uintptr_t) ctx->memptr_obj % 128, 0 );
      break;
    }

    case CStdlibReqPosixMemalign_Post_Alignment_NA:
      break;
  }
}

static void CStdlibReqPosixMemalign_Post_Size_Check(
  CStdlibReqPosixMemalign_Context  *ctx,
  CStdlibReqPosixMemalign_Post_Size state
)
{
  void     *ptr;
  int       eno;
  uintptr_t a;
  uintptr_t b;
  uintptr_t size;

  switch ( state ) {
    case CStdlibReqPosixMemalign_Post_Size_Valid: {
      /*
       * The size of the allocated memory area shall greater than or equal to
       * the size parameter.
       */
      /* Assume that the next allocation is done from adjacent memory */
      ptr = ctx->memptr_obj;
      eno = posix_memalign( &ptr, ctx->alignment, ctx->size );
      T_eq_int( eno, 0 );
      T_not_null( ptr );
      a = (uintptr_t) ptr;
      b = (uintptr_t) ctx->memptr_obj;
      size = a < b ? b - a : a - b;
      T_ge_uptr( size, ctx->size );
      break;
    }

    case CStdlibReqPosixMemalign_Post_Size_NA:
      break;
  }
}

static void CStdlibReqPosixMemalign_Setup(
  CStdlibReqPosixMemalign_Context *ctx
)
{
  MemorySave( &ctx->mem_ctx );
}

static void CStdlibReqPosixMemalign_Setup_Wrap( void *arg )
{
  CStdlibReqPosixMemalign_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CStdlibReqPosixMemalign_Setup( ctx );
}

static void CStdlibReqPosixMemalign_Teardown(
  CStdlibReqPosixMemalign_Context *ctx
)
{
  MemoryRestore( &ctx->mem_ctx );
}

static void CStdlibReqPosixMemalign_Teardown_Wrap( void *arg )
{
  CStdlibReqPosixMemalign_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CStdlibReqPosixMemalign_Teardown( ctx );
}

static void CStdlibReqPosixMemalign_Prepare(
  CStdlibReqPosixMemalign_Context *ctx
)
{
  ctx->memptr_obj = (void *)(uintptr_t) 1;
}

static void CStdlibReqPosixMemalign_Action(
  CStdlibReqPosixMemalign_Context *ctx
)
{
  ctx->status = posix_memalign( ctx->memptr, ctx->alignment, ctx->size );
}

static const CStdlibReqPosixMemalign_Entry
CStdlibReqPosixMemalign_Entries[] = {
  { 0, 0, 0, 0, CStdlibReqPosixMemalign_Post_Status_EINVAL,
    CStdlibReqPosixMemalign_Post_MemptrVar_Nop,
    CStdlibReqPosixMemalign_Post_Alignment_NA,
    CStdlibReqPosixMemalign_Post_Size_NA },
  { 0, 0, 0, 0, CStdlibReqPosixMemalign_Post_Status_EINVAL,
    CStdlibReqPosixMemalign_Post_MemptrVar_Null,
    CStdlibReqPosixMemalign_Post_Alignment_NA,
    CStdlibReqPosixMemalign_Post_Size_NA },
  { 0, 0, 0, 0, CStdlibReqPosixMemalign_Post_Status_ENOMEM,
    CStdlibReqPosixMemalign_Post_MemptrVar_Null,
    CStdlibReqPosixMemalign_Post_Alignment_NA,
    CStdlibReqPosixMemalign_Post_Size_NA },
  { 0, 0, 0, 0, CStdlibReqPosixMemalign_Post_Status_Zero,
    CStdlibReqPosixMemalign_Post_MemptrVar_Null,
    CStdlibReqPosixMemalign_Post_Alignment_Valid,
    CStdlibReqPosixMemalign_Post_Size_NA },
  { 0, 0, 0, 0, CStdlibReqPosixMemalign_Post_Status_Zero,
    CStdlibReqPosixMemalign_Post_MemptrVar_AreaBegin,
    CStdlibReqPosixMemalign_Post_Alignment_Valid,
    CStdlibReqPosixMemalign_Post_Size_Valid }
};

static const uint8_t
CStdlibReqPosixMemalign_Map[] = {
  1, 1, 1, 1, 1, 1, 2, 3, 2, 2, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static size_t CStdlibReqPosixMemalign_Scope( void *arg, char *buf, size_t n )
{
  CStdlibReqPosixMemalign_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      CStdlibReqPosixMemalign_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture CStdlibReqPosixMemalign_Fixture = {
  .setup = CStdlibReqPosixMemalign_Setup_Wrap,
  .stop = NULL,
  .teardown = CStdlibReqPosixMemalign_Teardown_Wrap,
  .scope = CStdlibReqPosixMemalign_Scope,
  .initial_context = &CStdlibReqPosixMemalign_Instance
};

static inline CStdlibReqPosixMemalign_Entry CStdlibReqPosixMemalign_PopEntry(
  CStdlibReqPosixMemalign_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CStdlibReqPosixMemalign_Entries[
    CStdlibReqPosixMemalign_Map[ index ]
  ];
}

static void CStdlibReqPosixMemalign_TestVariant(
  CStdlibReqPosixMemalign_Context *ctx
)
{
  CStdlibReqPosixMemalign_Pre_Memptr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CStdlibReqPosixMemalign_Pre_Alignment_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CStdlibReqPosixMemalign_Pre_Size_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CStdlibReqPosixMemalign_Action( ctx );
  CStdlibReqPosixMemalign_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CStdlibReqPosixMemalign_Post_MemptrVar_Check(
    ctx,
    ctx->Map.entry.Post_MemptrVar
  );
  CStdlibReqPosixMemalign_Post_Alignment_Check(
    ctx,
    ctx->Map.entry.Post_Alignment
  );
  CStdlibReqPosixMemalign_Post_Size_Check( ctx, ctx->Map.entry.Post_Size );
}

/**
 * @fn void T_case_body_CStdlibReqPosixMemalign( void )
 */
T_TEST_CASE_FIXTURE(
  CStdlibReqPosixMemalign,
  &CStdlibReqPosixMemalign_Fixture
)
{
  CStdlibReqPosixMemalign_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = CStdlibReqPosixMemalign_Pre_Memptr_Valid;
    ctx->Map.pcs[ 0 ] < CStdlibReqPosixMemalign_Pre_Memptr_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = CStdlibReqPosixMemalign_Pre_Alignment_Tiny;
      ctx->Map.pcs[ 1 ] < CStdlibReqPosixMemalign_Pre_Alignment_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = CStdlibReqPosixMemalign_Pre_Size_Huge;
        ctx->Map.pcs[ 2 ] < CStdlibReqPosixMemalign_Pre_Size_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = CStdlibReqPosixMemalign_PopEntry( ctx );
        CStdlibReqPosixMemalign_Prepare( ctx );
        CStdlibReqPosixMemalign_TestVariant( ctx );
      }
    }
  }
}

/** @} */
