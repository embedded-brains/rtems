/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsMallocValProcessorBlocks
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
#include <rtems.h>
#include <stdlib.h>
#include <rtems/malloc.h>
#include <rtems/score/smpimpl.h>
#include <rtems/score/threaddispatch.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsMallocValProcessorBlocks \
 *   spec:/rtems/malloc/val/processor-blocks
 *
 * @ingroup TestsuitesValidationHeap0
 *
 * @brief Tests the processor blocks of the C Program Heap.
 *
 * This test case performs the following actions:
 *
 * - Allocate a block of a size class of the processor blocks, free it and
 *   allocate a block of the same size class again.
 *
 *   - Check that the second allocation returned the block of the first one.
 *
 * - Allocate more blocks of one size class than a bin holds and free them.
 *   Allocate them again.
 *
 *   - Check that each block is a used block of the C Program Heap and free the
 *     blocks.
 *
 * - Resize a processor block within its size class and beyond it.
 *
 *   - Check that the shrink beyond the size class moved the block.
 *
 * - Allocate blocks of sizes beyond the processor blocks and free them.
 *
 * - Free a processor block of this processor on another processor. Allocate
 *   blocks on this processor afterwards.
 *
 *   - Check that the allocations succeeded and free the blocks.
 *
 * - Get the statistics of the C Program Heap with blocks in the bins.
 *
 *   - Check that a block in a bin counts as a free block.
 *
 * @{
 */

/**
 * @brief Test context for spec:/rtems/malloc/val/processor-blocks test case.
 */
typedef struct {
  /**
   * @brief This member contains a block.
   */
  void *ptr;

  /**
   * @brief This member contains the blocks of a test action.
   */
  void *blocks[ 96 ];
} RtemsMallocValProcessorBlocks_Context;

static RtemsMallocValProcessorBlocks_Context
  RtemsMallocValProcessorBlocks_Instance;

typedef RtemsMallocValProcessorBlocks_Context Context;

#if defined( RTEMS_SMP )
static void FreeBlock( void *arg )
{
  Context *ctx;

  ctx = arg;
  free( ctx->ptr );
}
#endif

static void FreeOnOtherProcessor( Context *ctx )
{
#if defined( RTEMS_SMP )
  Per_CPU_Control *cpu_self;

  cpu_self = _Thread_Dispatch_disable();
  _SMP_Unicast_action(
    ( _Per_CPU_Get_index( cpu_self ) + 1 ) %
      rtems_scheduler_get_processor_maximum(),
    FreeBlock,
    ctx
  );
  _Thread_Dispatch_enable( cpu_self );
#else
  free( ctx->ptr );
#endif
}

static T_fixture RtemsMallocValProcessorBlocks_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &RtemsMallocValProcessorBlocks_Instance
};

/**
 * @brief Allocate a block of a size class of the processor blocks, free it and
 *   allocate a block of the same size class again.
 */
static void RtemsMallocValProcessorBlocks_Action_0(
  RtemsMallocValProcessorBlocks_Context *ctx
)
{
  void     *ptr;
  uintptr_t addr;

  ptr = malloc( 24 );
  T_assert_not_null( ptr );
  addr = (uintptr_t) ptr;
  free( ptr );
  ctx->ptr = malloc( 24 );

  /*
   * Check that the second allocation returned the block of the first one.
   */
  T_eq_uptr( (uintptr_t) ctx->ptr, addr );
  free( ctx->ptr );
}

/**
 * @brief Allocate more blocks of one size class than a bin holds and free
 *   them. Allocate them again.
 */
static void RtemsMallocValProcessorBlocks_Action_1(
  RtemsMallocValProcessorBlocks_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < RTEMS_ARRAY_SIZE( ctx->blocks ); ++i ) {
    ctx->blocks[ i ] = malloc( 40 );
    T_quiet_not_null( ctx->blocks[ i ] );
  }

  for ( i = 0; i < RTEMS_ARRAY_SIZE( ctx->blocks ); ++i ) {
    free( ctx->blocks[ i ] );
  }

  for ( i = 0; i < RTEMS_ARRAY_SIZE( ctx->blocks ); ++i ) {
    ctx->blocks[ i ] = malloc( 40 );
    T_quiet_not_null( ctx->blocks[ i ] );
  }

  /*
   * Check that each block is a used block of the C Program Heap and free the
   * blocks.
   */
  for ( i = 0; i < RTEMS_ARRAY_SIZE( ctx->blocks ); ++i ) {
    T_quiet_ge_sz( malloc_usable_size( ctx->blocks[ i ] ), 40 );
    free( ctx->blocks[ i ] );
  }
}

/**
 * @brief Resize a processor block within its size class and beyond it.
 */
static void RtemsMallocValProcessorBlocks_Action_2(
  RtemsMallocValProcessorBlocks_Context *ctx
)
{
  void     *ptr;
  uintptr_t addr;

  ptr = malloc( 60 );
  T_assert_not_null( ptr );
  free( ptr );
  ptr = malloc( 60 );
  T_assert_not_null( ptr );
  addr = (uintptr_t) ptr;
  ctx->ptr = realloc( ptr, 58 );
  T_eq_uptr( (uintptr_t) ctx->ptr, addr );
  ctx->ptr = realloc( ctx->ptr, 2 );

  /*
   * Check that the shrink beyond the size class moved the block.
   */
  T_not_null( ctx->ptr );
  free( ctx->ptr );
}

/**
 * @brief Allocate blocks of sizes beyond the processor blocks and free them.
 */
static void RtemsMallocValProcessorBlocks_Action_3(
  RtemsMallocValProcessorBlocks_Context *ctx
)
{
  ctx->ptr = malloc( 8192 );
  T_assert_not_null( ctx->ptr );
  free( ctx->ptr );
  ctx->ptr = aligned_alloc( 64, 16 );
  T_assert_not_null( ctx->ptr );
  free( ctx->ptr );
}

/**
 * @brief Free a processor block of this processor on another processor.
 *   Allocate blocks on this processor afterwards.
 */
static void RtemsMallocValProcessorBlocks_Action_4(
  RtemsMallocValProcessorBlocks_Context *ctx
)
{
  uint32_t cpu_count;

  cpu_count = rtems_scheduler_get_processor_maximum();
  ctx->ptr = malloc( 24 );
  T_assert_not_null( ctx->ptr );
  free( ctx->ptr );
  ctx->ptr = malloc( 24 );
  T_assert_not_null( ctx->ptr );

  if ( cpu_count > 1 ) {
    FreeOnOtherProcessor( ctx );
  } else {
    free( ctx->ptr );
  }

  ctx->blocks[ 0 ] = malloc( 24 );
  ctx->blocks[ 1 ] = malloc( 24 );

  /*
   * Check that the allocations succeeded and free the blocks.
   */
  T_not_null( ctx->blocks[ 0 ] );
  T_not_null( ctx->blocks[ 1 ] );
  free( ctx->blocks[ 0 ] );
  free( ctx->blocks[ 1 ] );
}

/**
 * @brief Get the statistics of the C Program Heap with blocks in the bins.
 */
static void RtemsMallocValProcessorBlocks_Action_5( void )
{
  rtems_heap_statistics before;
  rtems_heap_statistics after;
  void                 *ptr;
  int                   status;

  status = malloc_get_statistics( &before );
  T_eq_int( status, 0 );
  ptr = malloc( 24 );
  T_assert_not_null( ptr );
  free( ptr );
  status = malloc_get_statistics( &after );
  T_eq_int( status, 0 );

  /*
   * Check that a block in a bin counts as a free block.
   */
  T_eq_u32( after.used_blocks, before.used_blocks );
}

/**
 * @fn void T_case_body_RtemsMallocValProcessorBlocks( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsMallocValProcessorBlocks,
  &RtemsMallocValProcessorBlocks_Fixture
)
{
  RtemsMallocValProcessorBlocks_Context *ctx;

  ctx = T_fixture_context();

  RtemsMallocValProcessorBlocks_Action_0( ctx );
  RtemsMallocValProcessorBlocks_Action_1( ctx );
  RtemsMallocValProcessorBlocks_Action_2( ctx );
  RtemsMallocValProcessorBlocks_Action_3( ctx );
  RtemsMallocValProcessorBlocks_Action_4( ctx );
  RtemsMallocValProcessorBlocks_Action_5();
}

/** @} */
