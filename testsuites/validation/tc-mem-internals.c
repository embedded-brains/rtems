/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsMallocValInternals
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

#include <rtems.h>
#include <stdlib.h>
#include <rtems/malloc.h>
#include <rtems/score/smpimpl.h>
#include <rtems/score/threaddispatch.h>

#include "ts-config.h"
#include "tx-heap.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsMallocValInternals spec:/rtems/malloc/val/internals
 *
 * @ingroup TestsuitesValidationHeap0
 *
 * @brief Tests the internal paths of the C Program Heap.
 *
 * This test case performs the following actions:
 *
 * - Allocate blocks from the C Program Heap with a boundary and without
 *   processor blocks.
 *
 * - Allocate a block from the C Program Heap with a dirty helper.
 *
 * - Free blocks so that a bin is full and so that the processor has no spare
 *   descriptor.
 *
 * - Initialize the processor blocks on a heap without descriptors, on an
 *   exhausted heap, and on heaps with increasing free sizes.
 *
 * - Free two processor blocks of this processor on another processor. Allocate
 *   blocks of their size classes again.
 *
 * - Free a processor block of a size class with a full bin while the C Program
 *   Heap has no spare descriptor.
 *
 * - Free a block without a processor block while each descriptor of this
 *   processor has a used block.
 *
 * - Get the information of the RTEMS Workspace.
 *
 * @{
 */

/**
 * @brief Test context for spec:/rtems/malloc/val/internals test case.
 */
typedef struct {
  /**
   * @brief This member contains the heap of a test action.
   */
  HeapContext heap;

  /**
   * @brief This member contains blocks.
   */
  void *blocks[ 64 ];

  /**
   * @brief This member contains the block which a processor keeps.
   */
  void *kept;

  /**
   * @brief This member contains the processor affinity of the runner task.
   */
  cpu_set_t affinity;

  /**
   * @brief This member contains the second block which a processor keeps.
   */
  void *kept2;

  /**
   * @brief This member contains the blocks of the exhaustion of the C Program
   *   Heap.
   */
  void *exhaust[ 512 ];

  /**
   * @brief This member counts the calls of the dirty helper.
   */
  size_t dirty_calls;
} RtemsMallocValInternals_Context;

static RtemsMallocValInternals_Context RtemsMallocValInternals_Instance;

typedef RtemsMallocValInternals_Context Context;

void *_Malloc_Allocate_unbound( size_t size );

static Context *dirty_context;

static void RunBoundary( Context *ctx )
{
  void *ptr;

  (void) ctx;
  ptr = rtems_heap_allocate_aligned_with_boundary( 0, 0, 0 );
  T_not_null( ptr );
  free( ptr );
  ptr = rtems_heap_allocate_aligned_with_boundary( 32, 16, 64 );
  T_not_null( ptr );
  T_eq_uptr( (uintptr_t) ptr % 64, 0 );
  free( ptr );
  T_null( rtems_heap_allocate_aligned_with_boundary( 32, 16, 24 ) );
  ptr = rtems_heap_allocate_aligned_with_boundary( 32, 0, 64 );
  T_not_null( ptr );
  T_le_uptr( (uintptr_t) ptr % 64 + 32, 64 );
  free( ptr );
  T_null( rtems_heap_allocate_aligned_with_boundary( 64, 0, 32 ) );
  /* With a guard word, a request of the maximum size has no bin */
  ptr = malloc( 4096 );
  T_not_null( ptr );
  free( ptr );
  T_null( _Malloc_Allocate_unbound( 0 ) );
  ptr = _Malloc_Allocate_unbound( 24 );
  T_not_null( ptr );
  free( ptr );
  ptr = NULL;
  RTEMS_OBFUSCATE_VARIABLE( ptr );
  free( ptr );
}

static void Dirty( void *ptr, size_t size )
{
  (void) ptr;
  (void) size;
  ++dirty_context->dirty_calls;
}

static void RunDirty( Context *ctx )
{
  rtems_malloc_dirtier_t saved;
  void                  *ptr;

  dirty_context = ctx;
  ctx->dirty_calls = 0;
  saved = rtems_malloc_dirty_helper;
  rtems_malloc_dirty_helper = Dirty;
  ptr = rtems_malloc( 8192 );
  rtems_malloc_dirty_helper = saved;
  T_not_null( ptr );
  T_eq_sz( ctx->dirty_calls, 1 );
  free( ptr );
}

static void RunBins( Context *ctx )
{
  void  *first;
  void  *second;
  size_t i;
  size_t n;
  size_t size;

  first = malloc( 40 );
  T_assert_not_null( first );
  free( first );
  first = malloc( 40 );
  T_assert_not_null( first );
  second = malloc( 40 );
  T_assert_not_null( second );
  free( second );
  free( first );
  second = malloc( 40 );
  T_assert_not_null( second );
  free( second );

  /* Bind one block in each size class and take it out of its bin */
  for ( i = 0; i < RTEMS_ARRAY_SIZE( ctx->blocks ); ++i ) {
    ctx->blocks[ i ] = malloc( 8 * ( i + 1 ) );
    T_quiet_not_null( ctx->blocks[ i ] );
  }

  for ( i = 0; i < RTEMS_ARRAY_SIZE( ctx->blocks ); ++i ) {
    free( ctx->blocks[ i ] );
  }

  for ( i = 0; i < RTEMS_ARRAY_SIZE( ctx->blocks ); ++i ) {
    ctx->blocks[ i ] = malloc( 8 * ( i + 1 ) );
    T_quiet_not_null( ctx->blocks[ i ] );
  }

  second = malloc( 48 );
  T_assert_not_null( second );
  free( second );

  for ( i = 0; i < RTEMS_ARRAY_SIZE( ctx->blocks ); ++i ) {
    free( ctx->blocks[ i ] );
  }

  /* The bin of a block of no class size is the class below its size */
  first = aligned_alloc( 16, 72 );
  T_assert_not_null( first );
  free( first );

  /* One bound block in each size class uses up the spare descriptors */
  for (
    i = 0, size = 8; i < RTEMS_ARRAY_SIZE( ctx->blocks ) && size <= 4096; ++i
  ) {
    ctx->blocks[ i ] = malloc( size );
    T_quiet_not_null( ctx->blocks[ i ] );
    size += size / 4 > 8 ? size / 4 : 8;
  }

  n = i;

  for ( i = 0; i < n; ++i ) {
    free( ctx->blocks[ i ] );
  }

  first = malloc( 32 );
  T_assert_not_null( first );
  second = realloc( first, 4000 );
  T_assert_not_null( second );
  free( second );
}

#if defined( RTEMS_SMP )
static char full_bin_reserve[ 64 * 1024 ] RTEMS_ALIGNED( RBHEAP_ALIGNMENT );

static void FreeKeptBlock( void *arg )
{
  Context *ctx;

  ctx = arg;
  free( ctx->kept );
  free( ctx->kept2 );
}

static size_t ExhaustCProgramHeap( Context *ctx )
{
  return HeapExhaustAndRecord(
    RTEMS_Malloc_Heap,
    ctx->exhaust,
    RTEMS_ARRAY_SIZE( ctx->exhaust )
  );
}

static void UseUpSpare( Context *ctx, char *reserve, size_t reserve_size )
{
  T_true( HeapUseUpSpare(
    RTEMS_Malloc_Heap,
    ctx->exhaust[ 0 ],
    reserve,
    reserve_size
  ) );
}

static void FreeExhaustion( Context *ctx, size_t n )
{
  HeapFreeRecorded( RTEMS_Malloc_Heap, ctx->exhaust, n );
}

static void PinToProcessor( Context *ctx )
{
  cpu_set_t         set;
  rtems_status_code sc;

  sc = rtems_task_get_affinity(
    RTEMS_SELF,
    sizeof( ctx->affinity ),
    &ctx->affinity
  );
  T_rsc_success( sc );
  CPU_ZERO( &set );
  CPU_SET( (int) rtems_scheduler_get_processor(), &set );
  sc = rtems_task_set_affinity( RTEMS_SELF, sizeof( set ), &set );
  T_rsc_success( sc );
}

static void Unpin( Context *ctx )
{
  rtems_status_code sc;

  sc = rtems_task_set_affinity(
    RTEMS_SELF,
    sizeof( ctx->affinity ),
    &ctx->affinity
  );
  T_rsc_success( sc );
}

static void RunNoSpare( Context *ctx )
{
  void  *unbound;
  void  *ptr;
  size_t size;

  PinToProcessor( ctx );
  unbound = _Malloc_Allocate_unbound( 24 );
  T_assert_not_null( unbound );

  /* Each bin gets a block, so each descriptor of the processor is bound */
  for ( size = 8; size <= 4096; size += 8 ) {
    ptr = malloc( size );
    T_quiet_not_null( ptr );
    free( ptr );
  }

  /* No descriptor is spare, so the block goes to the heap */
  free( unbound );
  Unpin( ctx );
}

static void RunFullBin( Context *ctx )
{
  void     *primer;
  void     *second;
  void     *refill;
  void     *probe;
  uintptr_t addr;
  uintptr_t size;
  size_t    n;

  PinToProcessor( ctx );

  /* The bin of the size class gets a block */
  primer = malloc( 16 );
  T_assert_not_null( primer );
  addr = (uintptr_t) primer;
  free( primer );

  /* A block of the bin is a processor block */
  second = malloc( 16 );
  T_assert_eq_uptr( (uintptr_t) second, addr );

  /* The empty bin gets a block, so it holds as many blocks as its capacity */
  refill = malloc( 16 );
  T_assert_not_null( refill );
  probe = refill;
  RTEMS_OBFUSCATE_VARIABLE( probe );
  free( refill );
  T_assert_true( _RBHeap_Size_of_block( RTEMS_Malloc_Heap, probe, &size ) );

  n = ExhaustCProgramHeap( ctx );
  T_assert_gt_sz( n, 1 );
  UseUpSpare( ctx, full_bin_reserve, sizeof( full_bin_reserve ) );

  /* The heap has no descriptor for the block, so it goes into the full bin */
  probe = second;
  RTEMS_OBFUSCATE_VARIABLE( probe );
  free( second );
  T_eq_uptr( RTEMS_Malloc_Heap->spare, 0 );
  T_true( _RBHeap_Size_of_block( RTEMS_Malloc_Heap, probe, &size ) );
  FreeExhaustion( ctx, n );
  Unpin( ctx );
}
#else
static void RunNoSpare( Context *ctx )
{
  (void) ctx;
}

static void RunFullBin( Context *ctx )
{
  (void) ctx;
}
#endif

static void RunKept( Context *ctx )
{
#if defined( RTEMS_SMP )
  RBHeap_control  *heap;
  uintptr_t        addr;
  uintptr_t        addr2;
  uintptr_t        old_size;
  uint32_t         other;
  Per_CPU_Control *cpu_self;

  if ( rtems_scheduler_get_processor_maximum() < 2 ) {
    return;
  }

  heap = RTEMS_Malloc_Heap;
  ctx->kept = malloc( 24 );
  T_assert_not_null( ctx->kept );
  free( ctx->kept );
  ctx->kept = malloc( 24 );
  T_assert_not_null( ctx->kept );
  addr = (uintptr_t) ctx->kept;
  ctx->kept2 = malloc( 40 );
  T_assert_not_null( ctx->kept2 );
  free( ctx->kept2 );
  ctx->kept2 = malloc( 40 );
  T_assert_not_null( ctx->kept2 );
  addr2 = (uintptr_t) ctx->kept2;
  cpu_self = _Thread_Dispatch_disable();
  other = ( _Per_CPU_Get_index( cpu_self ) + 1 ) %
          rtems_scheduler_get_processor_maximum();
  _SMP_Unicast_action( other, FreeKeptBlock, ctx );
  _Thread_Dispatch_enable( cpu_self );

  /* The blocks went into the kept list, so they stay used blocks */
  T_true( _RBHeap_Size_of_block( heap, ctx->kept, &old_size ) );
  T_true( _RBHeap_Size_of_block( heap, ctx->kept2, &old_size ) );

  /* A miss drains one kept block, the second one waits for the next */
  ctx->kept2 = malloc( 40 );
  T_eq_uptr( (uintptr_t) ctx->kept2, addr2 );
  ctx->kept = malloc( 24 );
  T_eq_uptr( (uintptr_t) ctx->kept, addr );
  free( ctx->kept );
  free( ctx->kept2 );
#else
  (void) ctx;
#endif
}

static void RunInitialize( Context *ctx )
{
#if defined( RTEMS_SMP )
  RBHeap_control *heap;
  uint32_t        cpus;
  uintptr_t       left;
  uintptr_t       largest;

  heap = &ctx->heap.heap;
  cpus = rtems_configuration_get_maximum_processors();

  /* Without descriptors, the heap has no processor blocks */
  HeapReset( &ctx->heap );
  T_true( _RBHeap_Initialize_processor_blocks( heap, cpus, 0, 4096 ) );
  T_false( _RBHeap_Has_processor_blocks( heap ) );

  /* An exhausted heap has no storage for the processor blocks */
  HeapExhaust( heap );
  T_false( _RBHeap_Initialize_processor_blocks( heap, cpus, 16, 64 ) );
  T_false( _RBHeap_Has_processor_blocks( heap ) );

  /*
   * Sixteen descriptors serve the eight size classes up to 64 bytes.  Each
   * free size below the need of the processor blocks fails at one of the
   * allocations.  The last size suffices.
   */
  for ( left = 64;; left += 64 ) {
    HeapReset( &ctx->heap );
    largest = HeapGetLargestAllocatableSize( heap );
    T_assert_lt_uptr( left, largest );
    T_quiet_not_null( _RBHeap_Allocate( heap, largest - left ) );

    if ( _RBHeap_Initialize_processor_blocks( heap, cpus, 16, 64 ) ) {
      break;
    }

    T_quiet_false( _RBHeap_Has_processor_blocks( heap ) );
  }

  T_true( _RBHeap_Has_processor_blocks( heap ) );
  T_true( _RBHeap_Walk( heap ) );
#else
  (void) ctx;
#endif
}

static T_fixture RtemsMallocValInternals_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &RtemsMallocValInternals_Instance
};

/**
 * @brief Allocate blocks from the C Program Heap with a boundary and without
 *   processor blocks.
 */
static void RtemsMallocValInternals_Action_0(
  RtemsMallocValInternals_Context *ctx
)
{
  RunBoundary( ctx );
}

/**
 * @brief Allocate a block from the C Program Heap with a dirty helper.
 */
static void RtemsMallocValInternals_Action_1(
  RtemsMallocValInternals_Context *ctx
)
{
  RunDirty( ctx );
}

/**
 * @brief Free blocks so that a bin is full and so that the processor has no
 *   spare descriptor.
 */
static void RtemsMallocValInternals_Action_2(
  RtemsMallocValInternals_Context *ctx
)
{
  RunBins( ctx );
}

/**
 * @brief Initialize the processor blocks on a heap without descriptors, on an
 *   exhausted heap, and on heaps with increasing free sizes.
 */
static void RtemsMallocValInternals_Action_3(
  RtemsMallocValInternals_Context *ctx
)
{
  RunInitialize( ctx );
}

/**
 * @brief Free two processor blocks of this processor on another processor.
 *   Allocate blocks of their size classes again.
 */
static void RtemsMallocValInternals_Action_4(
  RtemsMallocValInternals_Context *ctx
)
{
  RunKept( ctx );
}

/**
 * @brief Free a processor block of a size class with a full bin while the C
 *   Program Heap has no spare descriptor.
 */
static void RtemsMallocValInternals_Action_5(
  RtemsMallocValInternals_Context *ctx
)
{
  RunFullBin( ctx );
}

/**
 * @brief Free a block without a processor block while each descriptor of this
 *   processor has a used block.
 */
static void RtemsMallocValInternals_Action_6(
  RtemsMallocValInternals_Context *ctx
)
{
  RunNoSpare( ctx );
}

/**
 * @brief Get the information of the RTEMS Workspace.
 */
static void RtemsMallocValInternals_Action_7( void )
{
  Heap_Information_block info;

  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  T_false( rtems_workspace_get_information( NULL ) );
  T_true( rtems_workspace_get_information( &info ) );
  #pragma GCC diagnostic pop
}

/**
 * @fn void T_case_body_RtemsMallocValInternals( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsMallocValInternals,
  &RtemsMallocValInternals_Fixture
)
{
  RtemsMallocValInternals_Context *ctx;

  ctx = T_fixture_context();

  RtemsMallocValInternals_Action_0( ctx );
  RtemsMallocValInternals_Action_1( ctx );
  RtemsMallocValInternals_Action_2( ctx );
  RtemsMallocValInternals_Action_3( ctx );
  RtemsMallocValInternals_Action_4( ctx );
  RtemsMallocValInternals_Action_5( ctx );
  RtemsMallocValInternals_Action_6( ctx );
  RtemsMallocValInternals_Action_7();
}

/** @} */
