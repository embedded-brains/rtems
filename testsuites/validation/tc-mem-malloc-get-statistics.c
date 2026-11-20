/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsMallocReqGetStatistics
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
#include <string.h>
#include <rtems/malloc.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsMallocReqGetStatistics spec:/rtems/malloc/req/get-statistics
 *
 * @ingroup TestsuitesValidationNonQual0
 *
 * @{
 */

typedef enum {
  RtemsMallocReqGetStatistics_Pre_Stats_Valid,
  RtemsMallocReqGetStatistics_Pre_Stats_Null,
  RtemsMallocReqGetStatistics_Pre_Stats_NA
} RtemsMallocReqGetStatistics_Pre_Stats;

typedef enum {
  RtemsMallocReqGetStatistics_Post_Status_Zero,
  RtemsMallocReqGetStatistics_Post_Status_NegativeOne,
  RtemsMallocReqGetStatistics_Post_Status_NA
} RtemsMallocReqGetStatistics_Post_Status;

typedef enum {
  RtemsMallocReqGetStatistics_Post_Stats_Heap,
  RtemsMallocReqGetStatistics_Post_Stats_NA
} RtemsMallocReqGetStatistics_Post_Stats;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Stats_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_Stats : 1;
} RtemsMallocReqGetStatistics_Entry;

/**
 * @brief Test context for spec:/rtems/malloc/req/get-statistics test case.
 */
typedef struct {
  /**
   * @brief This member provides the object referenced by the `stats`
   *   parameter.
   */
  rtems_malloc_statistics stats;

  /**
   * @brief This member contains the statistics of the heap before the
   *   operations of the test-prepare.
   */
  rtems_malloc_statistics before;

  /**
   * @brief This member references the first block which the test-prepare
   *   allocates.
   */
  void *first;

  /**
   * @brief This member references the second block which the test-prepare
   *   allocates.
   */
  void *second;

  /**
   * @brief This member provides the `stats` parameter value.
   */
  rtems_malloc_statistics *stats_ptr;

  /**
   * @brief This member contains the return value of the directive call.
   */
  int status;

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
    RtemsMallocReqGetStatistics_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsMallocReqGetStatistics_Context;

static RtemsMallocReqGetStatistics_Context
  RtemsMallocReqGetStatistics_Instance;

static const char *const RtemsMallocReqGetStatistics_PreDesc_Stats[] =
  { "Valid", "Null", "NA" };

static const char *const *const RtemsMallocReqGetStatistics_PreDesc[] = {
  RtemsMallocReqGetStatistics_PreDesc_Stats,
  NULL
};

#define ALLOC_SIZE 64

static void RtemsMallocReqGetStatistics_Pre_Stats_Prepare(
  RtemsMallocReqGetStatistics_Context  *ctx,
  RtemsMallocReqGetStatistics_Pre_Stats state
)
{
  switch ( state ) {
    case RtemsMallocReqGetStatistics_Pre_Stats_Valid: {
      /*
       * While the `stats` parameter references an object of type
       * rtems_malloc_statistics.
       */
      ctx->stats_ptr = &ctx->stats;
      break;
    }

    case RtemsMallocReqGetStatistics_Pre_Stats_Null: {
      /*
       * While the `stats` parameter is equal to NULL.
       */
      ctx->stats_ptr = NULL;
      break;
    }

    case RtemsMallocReqGetStatistics_Pre_Stats_NA:
      break;
  }
}

static void RtemsMallocReqGetStatistics_Post_Status_Check(
  RtemsMallocReqGetStatistics_Context    *ctx,
  RtemsMallocReqGetStatistics_Post_Status state
)
{
  switch ( state ) {
    case RtemsMallocReqGetStatistics_Post_Status_Zero: {
      /*
       * The return value of malloc_get_statistics() shall be equal to zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case RtemsMallocReqGetStatistics_Post_Status_NegativeOne: {
      /*
       * The return value of malloc_get_statistics() shall be equal to minus
       * one.
       */
      T_eq_int( ctx->status, -1 );
      break;
    }

    case RtemsMallocReqGetStatistics_Post_Status_NA:
      break;
  }
}

static void RtemsMallocReqGetStatistics_Post_Stats_Check(
  RtemsMallocReqGetStatistics_Context   *ctx,
  RtemsMallocReqGetStatistics_Post_Stats state
)
{
  uint64_t allocated;
  uint64_t freed;

  switch ( state ) {
    case RtemsMallocReqGetStatistics_Post_Stats_Heap: {
      /*
       * The object referenced by the `stats` parameter shall be equal to the
       * statistics of the heap which serves malloc().
       */
      T_eq_u32( ctx->stats.allocs, ctx->before.allocs + 3 );
      T_eq_u32( ctx->stats.frees, ctx->before.frees + 1 );
      T_eq_u32( ctx->stats.failed_allocs, ctx->before.failed_allocs + 1 );
      T_eq_u32( ctx->stats.resizes, ctx->before.resizes + 1 );
      T_eq_u32( ctx->stats.used_blocks, ctx->before.used_blocks + 2 );
      T_eq_sz( ctx->stats.size, ctx->before.size );
      allocated = ctx->stats.lifetime_allocated -
                  ctx->before.lifetime_allocated;
      freed = ctx->stats.lifetime_freed - ctx->before.lifetime_freed;
      T_ge_u64( allocated, 3 * ALLOC_SIZE );
      T_ge_u64( freed, ALLOC_SIZE );
      T_eq_u64(
        ctx->before.free_size - ctx->stats.free_size,
        allocated - freed
      );
      T_le_u64( ctx->stats.min_free_size, ctx->before.free_size - allocated );
      T_le_u32( ctx->stats.free_blocks, ctx->stats.max_free_blocks );
      T_ge_u32( ctx->stats.max_free_blocks, ctx->before.max_free_blocks );
      break;
    }

    case RtemsMallocReqGetStatistics_Post_Stats_NA:
      break;
  }
}

static void RtemsMallocReqGetStatistics_Prepare(
  RtemsMallocReqGetStatistics_Context *ctx
)
{
  void *ptr;
  int   rv;

  rv = malloc_get_statistics( &ctx->before );
  T_assert_eq_int( rv, 0 );
  ctx->first = malloc( ALLOC_SIZE );
  T_assert_not_null( ctx->first );
  ctx->second = malloc( ALLOC_SIZE );
  T_assert_not_null( ctx->second );
  ptr = malloc( ALLOC_SIZE );
  T_assert_not_null( ptr );
  free( ptr );
  ptr = realloc( ctx->first, ALLOC_SIZE );
  T_assert_eq_ptr( ptr, ctx->first );
  ptr = malloc( ctx->before.size );
  T_assert_null( ptr );
  memset( &ctx->stats, 0xff, sizeof( ctx->stats ) );
}

static void RtemsMallocReqGetStatistics_Action(
  RtemsMallocReqGetStatistics_Context *ctx
)
{
  ctx->status = malloc_get_statistics( ctx->stats_ptr );
}

static void RtemsMallocReqGetStatistics_Cleanup(
  RtemsMallocReqGetStatistics_Context *ctx
)
{
  free( ctx->second );
  free( ctx->first );
}

/* clang-format off */

static const RtemsMallocReqGetStatistics_Entry
RtemsMallocReqGetStatistics_Entries[] = {
  { 0, 0, RtemsMallocReqGetStatistics_Post_Status_Zero,
    RtemsMallocReqGetStatistics_Post_Stats_Heap },
  { 0, 0, RtemsMallocReqGetStatistics_Post_Status_NegativeOne,
    RtemsMallocReqGetStatistics_Post_Stats_NA }
};

static const uint8_t
RtemsMallocReqGetStatistics_Map[] = {
  0, 1
};

/* clang-format on */

static size_t RtemsMallocReqGetStatistics_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  RtemsMallocReqGetStatistics_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsMallocReqGetStatistics_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsMallocReqGetStatistics_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsMallocReqGetStatistics_Scope,
  .initial_context = &RtemsMallocReqGetStatistics_Instance
};

static inline RtemsMallocReqGetStatistics_Entry
RtemsMallocReqGetStatistics_PopEntry(
  RtemsMallocReqGetStatistics_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsMallocReqGetStatistics_Entries
    [ RtemsMallocReqGetStatistics_Map[ index ] ];
}

static void RtemsMallocReqGetStatistics_TestVariant(
  RtemsMallocReqGetStatistics_Context *ctx
)
{
  RtemsMallocReqGetStatistics_Pre_Stats_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsMallocReqGetStatistics_Action( ctx );
  RtemsMallocReqGetStatistics_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  RtemsMallocReqGetStatistics_Post_Stats_Check(
    ctx,
    ctx->Map.entry.Post_Stats
  );
}

/**
 * @fn void T_case_body_RtemsMallocReqGetStatistics( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsMallocReqGetStatistics,
  &RtemsMallocReqGetStatistics_Fixture
)
{
  RtemsMallocReqGetStatistics_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsMallocReqGetStatistics_Pre_Stats_Valid;
    ctx->Map.pcs[ 0 ] < RtemsMallocReqGetStatistics_Pre_Stats_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = RtemsMallocReqGetStatistics_PopEntry( ctx );
    RtemsMallocReqGetStatistics_Prepare( ctx );
    RtemsMallocReqGetStatistics_TestVariant( ctx );
    RtemsMallocReqGetStatistics_Cleanup( ctx );
  }
}

/** @} */
