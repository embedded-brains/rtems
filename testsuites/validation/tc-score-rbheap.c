/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreRbheapValRbheap
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

#include <string.h>
#include <rtems/config.h>
#include <rtems/score/heapinfo.h>
#include <rtems/score/percpu.h>
#include <rtems/score/rbheap.h>
#include <rtems/score/smpimpl.h>
#include <rtems/score/threaddispatch.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreRbheapValRbheap spec:/score/rbheap/val/rbheap
 *
 * @ingroup TestsuitesValidationHeap0
 *
 * @brief Tests the red-black tree heap.
 *
 * This test case performs the following actions:
 *
 * - Run a random sequence of allocations, frees and resizes on a heap which
 *   takes its slabs from its memory area.
 *
 *   - Check that the heap stayed consistent, that the used blocks kept their
 *     content and that the heap holds its whole memory again.
 *
 * - Run a random sequence of allocations, frees and resizes on a heap which
 *   takes its slabs from a slab source and on a heap with added descriptors.
 *
 *   - Check that the heap stayed consistent and that it holds its whole memory
 *     again.
 *
 * - Extend a heap by an area after its area, by an adjacent area and by an
 *   area before its area. Run a random sequence of operations on it.
 *
 *   - Check that the heap stayed consistent and that it holds its whole memory
 *     again.
 *
 * - Free and allocate blocks of a heap with processor blocks and of a heap
 *   without processor blocks.
 *
 * - Free the processor blocks of the current processor on the other processors
 *   while the current processor allocates them from its kept list.
 *
 * - Get the information and the statistics of a heap with blocks.
 *
 * - Initialize heaps with invalid memory areas.
 *
 * @{
 */

#define AREA_SIZE ( 32 * RBHEAP_SLAB_SIZE )

#define SLAB_COUNT 8

#define BLOCK_COUNT 256

#define STEP_COUNT 3000

typedef struct {
  unsigned char *begin;
  size_t         size;
} Block;

/**
 * @brief Test context for spec:/score/rbheap/val/rbheap test case.
 */
typedef struct {
  /**
   * @brief This member contains the heap.
   */
  RBHeap_control heap;

  /**
   * @brief This member contains the blocks of the random sequence.
   */
  Block blocks[ BLOCK_COUNT ];

  /**
   * @brief This member contains the count of slabs which the slab source
   *   returned.
   */
  size_t slab_count;

  /**
   * @brief This member provides the slabs of the slab source.
   */
  char slabs[ SLAB_COUNT ][ RBHEAP_SLAB_SIZE ] RTEMS_ALIGNED(
    RBHEAP_SLAB_SIZE
  );

  /**
   * @brief This member provides the memory area of the heap.
   */
  char area[ AREA_SIZE ] RTEMS_ALIGNED( RBHEAP_SLAB_SIZE );
} ScoreRbheapValRbheap_Context;

static ScoreRbheapValRbheap_Context ScoreRbheapValRbheap_Instance;

typedef ScoreRbheapValRbheap_Context Context;

static Context *slab_source_context;

static void *SlabSource( void )
{
  Context *ctx;

  ctx = slab_source_context;

  if ( ctx->slab_count + 1 >= SLAB_COUNT ) {
    return NULL;
  }

  ++ctx->slab_count;
  return &ctx->slabs[ ctx->slab_count ];
}

static uint32_t Random( uint32_t *state )
{
  *state = *state * 1103515245 + 12345;
  return *state >> 8;
}

static unsigned char Tag( const Context *ctx, const Block *block )
{
  return (unsigned char) ( ( block - &ctx->blocks[ 0 ] ) + 1 );
}

static void Fill( const Context *ctx, Block *block )
{
  memset( block->begin, Tag( ctx, block ), block->size );
}

static void CheckContent( const Context *ctx, const Block *block )
{
  unsigned char tag;
  size_t        i;

  tag = Tag( ctx, block );

  for ( i = 0; i < block->size; ++i ) {
    if ( block->begin[ i ] != tag ) {
      T_quiet_eq_uint( block->begin[ i ], tag );
      break;
    }
  }
}

static size_t RandomSize( uint32_t *state )
{
  uint32_t r;

  r = Random( state ) % 16;

  if ( r < 10 ) {
    return 1 + Random( state ) % 64;
  }

  if ( r < 14 ) {
    return 1 + Random( state ) % 512;
  }

  return 1 + Random( state ) % ( 2 * RBHEAP_SLAB_SIZE );
}

static void CheckReserve( const RBHeap_control *heap )
{
  T_quiet_ge_uptr( heap->capacity, 2 * heap->entries + 3 );
}

static void Allocate( Context *ctx, Block *block, uint32_t *state )
{
  size_t    size;
  uintptr_t alignment;
  void     *ptr;

  size = RandomSize( state );

  switch ( Random( state ) % 8 ) {
    case 0:
      alignment = 64;
      break;
    case 1:
      alignment = RBHEAP_SLAB_SIZE;
      break;
    case 2:
      alignment = 16;
      break;
    default:
      alignment = 0;
      break;
  }

  if ( alignment != 0 ) {
    ptr = _RBHeap_Allocate_aligned( &ctx->heap, size, alignment );
  } else {
    ptr = _RBHeap_Allocate( &ctx->heap, size );
  }

  if ( ptr != NULL ) {
    T_quiet_eq_uptr( (uintptr_t) ptr % ( alignment + ( alignment == 0 ) ), 0 );
    block->begin = ptr;
    block->size = size;
    Fill( ctx, block );
    CheckReserve( &ctx->heap );
  }
}

static void Resize( Context *ctx, Block *block, uint32_t *state )
{
  size_t               size;
  uintptr_t            old_size;
  RBHeap_resize_status status;

  size = RandomSize( state );
  status = _RBHeap_Resize( &ctx->heap, block->begin, size, &old_size );
  T_quiet_ge_uptr( old_size, block->size );

  if ( status == RBHEAP_RESIZE_SUCCESSFUL ) {
    if ( size < block->size ) {
      block->size = size;
    }

    CheckContent( ctx, block );
    block->size = size;
    Fill( ctx, block );
  } else {
    T_quiet_eq_int( status, RBHEAP_RESIZE_UNSATISFIED );
    CheckContent( ctx, block );
  }
}

static void Free( Context *ctx, Block *block )
{
  CheckContent( ctx, block );
  T_quiet_true( _RBHeap_Free( &ctx->heap, block->begin ) );
  block->begin = NULL;
}

static void RunWorkload( Context *ctx, uint32_t seed )
{
  uint32_t state;
  size_t   step;
  size_t   i;

  state = seed;
  memset( ctx->blocks, 0, sizeof( ctx->blocks ) );

  for ( step = 0; step < STEP_COUNT; ++step ) {
    Block   *block;
    uint32_t r;

    r = Random( &state );
    block = &ctx->blocks[ r % BLOCK_COUNT ];

    if ( block->begin == NULL ) {
      Allocate( ctx, block, &state );
    } else if ( ( r / BLOCK_COUNT ) % 3 == 0 ) {
      Resize( ctx, block, &state );
    } else {
      Free( ctx, block );
    }

    if ( step % 16 == 0 ) {
      T_quiet_true( _RBHeap_Walk( &ctx->heap ) );
    }
  }

  for ( i = 0; i < BLOCK_COUNT; ++i ) {
    Block *block;

    block = &ctx->blocks[ i ];

    if ( block->begin != NULL ) {
      Free( ctx, block );
    }
  }
}

static void CheckEmpty( const Context *ctx )
{
  Heap_Statistics stats;
  uintptr_t       before;
  void           *ptr;

  T_true( _RBHeap_Walk( &ctx->heap ) );
  _RBHeap_Get_statistics( &ctx->heap, &stats );
  T_eq_u32( stats.used_blocks, 0 );
  before = stats.free_size;

  /* Further frees release the slabs which the reserve does not need */
  ptr = _RBHeap_Allocate( RTEMS_DECONST( RBHeap_control *, &ctx->heap ), 1 );
  T_not_null( ptr );
  T_true( _RBHeap_Free( RTEMS_DECONST( RBHeap_control *, &ctx->heap ), ptr ) );
  _RBHeap_Get_statistics( &ctx->heap, &stats );
  T_ge_uptr( stats.free_size, before );
}

static void RunProcessorBlocks( Context *ctx )
{
#if defined( RTEMS_SMP )
  RBHeap_control        *heap;
  Heap_Information_block info;
  uint32_t               cpus;
  uintptr_t              used;
  uintptr_t              old_size;
  void                  *ptr[ 3 ];
  size_t                 i;
  bool                   ok;

  heap = &ctx->heap;
  cpus = rtems_configuration_get_maximum_processors();
  ok = _RBHeap_Initialize( heap, ctx->area, sizeof( ctx->area ) );
  T_assert_true( ok );

  /* A heap without processor blocks frees each block itself */
  T_false( _RBHeap_Has_processor_blocks( heap ) );
  T_null( _RBHeap_Allocate_from_processor( heap, 32 ) );
  T_eq_uptr( _RBHeap_Round_for_processor( heap, 20 ), 20 );
  ptr[ 0 ] = _RBHeap_Allocate( heap, 32 );
  T_assert_not_null( ptr[ 0 ] );
  T_true( _RBHeap_Free_to_processor( heap, ptr[ 0 ] ) );
  T_true( _RBHeap_Free_to_processor( heap, NULL ) );
  _RBHeap_Get_information( heap, &info );
  used = info.Used.number;
  _RBHeap_Get_processor_information( heap, &info );
  T_eq_uptr( info.Used.number, used );

  /* Sixteen descriptors serve the eight size classes up to 64 bytes */
  T_true( _RBHeap_Initialize_processor_blocks( heap, cpus, 16, 64 ) );
  T_true( _RBHeap_Has_processor_blocks( heap ) );
  T_true( _RBHeap_Free_to_processor( heap, NULL ) );

  /* A request of no size or above the maximum size has no size class */
  T_null( _RBHeap_Allocate_from_processor( heap, 0 ) );
  T_null( _RBHeap_Allocate_from_processor( heap, 128 ) );
  T_eq_uptr( _RBHeap_Round_for_processor( heap, 0 ), 0 );
  T_eq_uptr( _RBHeap_Round_for_processor( heap, 128 ), 128 );
  T_ge_uptr( _RBHeap_Round_for_processor( heap, 20 ), 20 );

  for ( i = 0; i < RTEMS_ARRAY_SIZE( ptr ); ++i ) {
    ptr[ i ] = _RBHeap_Allocate( heap, 32 );
    T_assert_not_null( ptr[ i ] );
  }

  /* An address which begins no block is invalid */
  T_false( _RBHeap_Free_to_processor( heap, &ctx->area[ 1 ] ) );

  /* The free binds the block, so it stays a used block of the heap */
  _RBHeap_Get_information( heap, &info );
  used = info.Used.number;
  T_true( _RBHeap_Free_to_processor( heap, ptr[ 0 ] ) );
  T_false( _RBHeap_Free( heap, ptr[ 0 ] ) );
  T_eq_int(
    _RBHeap_Resize( heap, ptr[ 0 ], 64, &old_size ),
    RBHEAP_RESIZE_BOUND
  );
  _RBHeap_Get_information( heap, &info );
  T_eq_uptr( info.Used.number, used );
  _RBHeap_Get_processor_information( heap, &info );
  T_eq_uptr( info.Used.number, used - 1 );

  /* The bin of the size class is full, so the heap frees the next block */
  T_true( _RBHeap_Free_to_processor( heap, ptr[ 1 ] ) );
  T_false( _RBHeap_Size_of_block( heap, ptr[ 1 ], &old_size ) );

  /* A free of a block in a bin of the processor is invalid */
  T_false( _RBHeap_Free_to_processor( heap, ptr[ 0 ] ) );

  /* The allocation takes the block out of its bin */
  T_eq_ptr( _RBHeap_Allocate_from_processor( heap, 32 ), ptr[ 0 ] );
  T_null( _RBHeap_Allocate_from_processor( heap, 32 ) );
  T_true( _RBHeap_Size_of_block( heap, ptr[ 0 ], &old_size ) );
  T_true( _RBHeap_Processor_keeps( heap, old_size, old_size ) );
  T_false( _RBHeap_Processor_keeps( heap, 1, old_size ) );
  T_false( _RBHeap_Processor_keeps( heap, old_size + 1, old_size ) );

  /* The free of the processor block puts it into its bin */
  T_true( _RBHeap_Free_to_processor( heap, ptr[ 0 ] ) );
  T_true( _RBHeap_Free( heap, ptr[ 2 ] ) );
  T_true( _RBHeap_Walk( heap ) );
#else
  (void) ctx;
#endif
}

#if defined( RTEMS_SMP )
#define KEPT_BLOCKS 24

#define KEPT_ROUNDS 64

typedef struct {
  Context *ctx;
  size_t   first;
  size_t   step;
} KeptPusher;

static void *kept_blocks[ KEPT_BLOCKS ];

static KeptPusher kept_pushers[ CPU_MAXIMUM_PROCESSORS ];

static Per_CPU_Job_context kept_job_contexts[ CPU_MAXIMUM_PROCESSORS ];

static Per_CPU_Job kept_jobs[ CPU_MAXIMUM_PROCESSORS ];

static void FreeKeptBlocks( void *arg )
{
  KeptPusher *pusher;
  size_t      i;

  pusher = arg;

  for ( i = pusher->first; i < KEPT_BLOCKS; i += pusher->step ) {
    T_quiet_true(
      _RBHeap_Free_to_processor( &pusher->ctx->heap, kept_blocks[ i ] )
    );
  }
}

static bool KeptJobDone( uint32_t k )
{
  return _Atomic_Load_ulong( &kept_jobs[ k ].done, ATOMIC_ORDER_ACQUIRE ) ==
         PER_CPU_JOB_DONE;
}
#endif

static void RunKeptRace( Context *ctx )
{
#if defined( RTEMS_SMP )
  RBHeap_control  *heap;
  Per_CPU_Control *cpu_self;
  uint32_t         cpus;
  uint32_t         self;
  uint32_t         k;
  size_t           round;
  size_t           got;
  size_t           i;
  bool             ok;

  cpus = _SMP_Get_processor_maximum();

  if ( cpus < 2 ) {
    return;
  }

  heap = &ctx->heap;
  ok = _RBHeap_Initialize( heap, ctx->area, sizeof( ctx->area ) );
  T_assert_true( ok );
  ok = _RBHeap_Initialize_processor_blocks(
    heap,
    rtems_configuration_get_maximum_processors(),
    2 * KEPT_BLOCKS,
    64
  );
  T_assert_true( ok );
  cpu_self = _Thread_Dispatch_disable();
  self = _Per_CPU_Get_index( cpu_self );

  /* The current processor binds each block */
  for ( i = 0; i < KEPT_BLOCKS; ++i ) {
    kept_blocks[ i ] = _RBHeap_Allocate( heap, 16 );
    T_quiet_not_null( kept_blocks[ i ] );
    T_quiet_true( _RBHeap_Free_to_processor( heap, kept_blocks[ i ] ) );
    T_quiet_eq_ptr(
      _RBHeap_Allocate_from_processor( heap, 16 ),
      kept_blocks[ i ]
    );
  }

  for ( k = 1; k < cpus; ++k ) {
    kept_pushers[ k ].ctx = ctx;
    kept_pushers[ k ].first = k - 1;
    kept_pushers[ k ].step = cpus - 1;
    kept_job_contexts[ k ].handler = FreeKeptBlocks;
    kept_job_contexts[ k ].arg = &kept_pushers[ k ];
    kept_jobs[ k ].context = &kept_job_contexts[ k ];
  }

  for ( round = 0; round < KEPT_ROUNDS; ++round ) {
    for ( k = 1; k < cpus; ++k ) {
      _Per_CPU_Submit_job(
        _Per_CPU_Get_by_index( ( self + k ) % cpus ),
        &kept_jobs[ k ]
      );
    }

    got = 0;

    /*
     * The exchange of the kept list by an allocation races against the push
     * of a free.  The delay of the round shifts the allocations against the
     * pushes.
     */
    for ( k = 1; k < cpus; ++k ) {
      while ( !KeptJobDone( k ) ) {
        for ( i = 0; i < 8 * round; ++i ) {
          RTEMS_COMPILER_MEMORY_BARRIER();
        }

        if ( _RBHeap_Allocate_from_processor( heap, 16 ) != NULL ) {
          ++got;
        }
      }
    }

    /* Each allocation takes one block of the kept list */
    for ( ; got < KEPT_BLOCKS; ++got ) {
      T_quiet_not_null( _RBHeap_Allocate_from_processor( heap, 16 ) );
    }

    T_quiet_null( _RBHeap_Allocate_from_processor( heap, 16 ) );
  }

  _Thread_Dispatch_enable( cpu_self );
  T_true( _RBHeap_Walk( heap ) );
#else
  (void) ctx;
#endif
}

static void RunInformation( Context *ctx )
{
  Heap_Information_block info;
  Heap_Statistics        stats;
  void                  *ptr[ 3 ];
  uintptr_t              size;
  uintptr_t              used;
  bool                   ok;

  ok = _RBHeap_Initialize( &ctx->heap, ctx->area, sizeof( ctx->area ) );
  T_assert_true( ok );
  ptr[ 0 ] = _RBHeap_Allocate( &ctx->heap, 100 );
  ptr[ 1 ] = _RBHeap_Allocate( &ctx->heap, 200 );
  ptr[ 2 ] = _RBHeap_Allocate( &ctx->heap, 300 );
  T_true( _RBHeap_Free( &ctx->heap, ptr[ 1 ] ) );
  _RBHeap_Get_information( &ctx->heap, &info );
  _RBHeap_Get_statistics( &ctx->heap, &stats );
  T_eq_uptr( info.Used.number, 2 );
  T_eq_uptr( info.Free.number, 2 );
  T_eq_uptr( info.Free.total, stats.free_size );
  T_ge_uptr( info.Free.largest, 200 );
  T_eq_uptr( info.Free.largest, _RBHeap_Get_largest_free_size( &ctx->heap ) );
  T_eq_uptr( info.Used.largest, 0 );
  T_true( _RBHeap_Size_of_block( &ctx->heap, ptr[ 0 ], &size ) );
  T_ge_uptr( size, 100 );
  used = size;
  T_true( _RBHeap_Size_of_block( &ctx->heap, ptr[ 2 ], &size ) );
  T_ge_uptr( size, 300 );
  used += size;
  T_eq_uptr( info.Used.total, used + 2 * RBHEAP_GUARD_SIZE );
  T_false( _RBHeap_Size_of_block( &ctx->heap, ptr[ 1 ], &size ) );
  T_eq_uptr( _RBHeap_Get_releasable_size( &ctx->heap, 0 ), 0 );
  T_eq_uptr( _RBHeap_Get_releasable_size( &ctx->heap, 100 ), 0 );
  T_true( _RBHeap_Free( &ctx->heap, ptr[ 0 ] ) );
  T_true( _RBHeap_Free( &ctx->heap, ptr[ 2 ] ) );
}

static void RunInitialize( Context *ctx )
{
  T_false( _RBHeap_Initialize( &ctx->heap, ctx->area, RBHEAP_SLAB_SIZE / 2 ) );
  T_false( _RBHeap_Initialize( &ctx->heap, ctx->area + 1, RBHEAP_SLAB_SIZE ) );
  T_true( _RBHeap_Initialize( &ctx->heap, ctx->area + 64, UINTPTR_MAX ) );
  T_le_uptr( ctx->heap.area_end - ctx->heap.area_begin, RBHEAP_MAX_SIZE );
  T_false(
    _RBHeap_Initialize( &ctx->heap, (void *) ( UINTPTR_MAX - 15 ), 64 )
  );
  _RBHeap_Initialize_empty( &ctx->heap );
  T_null( _RBHeap_Allocate( &ctx->heap, 1 ) );
  T_true( _RBHeap_Walk( &ctx->heap ) );
}

static T_fixture ScoreRbheapValRbheap_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &ScoreRbheapValRbheap_Instance
};

/**
 * @brief Run a random sequence of allocations, frees and resizes on a heap
 *   which takes its slabs from its memory area.
 */
static void ScoreRbheapValRbheap_Action_0( ScoreRbheapValRbheap_Context *ctx )
{
  bool ok;

  ok = _RBHeap_Initialize( &ctx->heap, ctx->area, sizeof( ctx->area ) );
  T_assert_true( ok );
  RunWorkload( ctx, 1 );
  RunWorkload( ctx, 2 );

  /*
   * Check that the heap stayed consistent, that the used blocks kept their
   * content and that the heap holds its whole memory again.
   */
  CheckEmpty( ctx );
}

/**
 * @brief Run a random sequence of allocations, frees and resizes on a heap
 *   which takes its slabs from a slab source and on a heap with added
 *   descriptors.
 */
static void ScoreRbheapValRbheap_Action_1( ScoreRbheapValRbheap_Context *ctx )
{
  bool ok;

  slab_source_context = ctx;
  ctx->slab_count = 0;
  ok = _RBHeap_Initialize_with_slab(
    &ctx->heap,
    ctx->area,
    sizeof( ctx->area ),
    &ctx->slabs[ 0 ],
    SlabSource
  );
  T_assert_true( ok );
  RunWorkload( ctx, 3 );
  CheckEmpty( ctx );

  ok = _RBHeap_Initialize( &ctx->heap, ctx->area, sizeof( ctx->area ) / 2 );
  T_assert_true( ok );
  ok = _RBHeap_Add_descriptors( &ctx->heap, ctx->slabs, sizeof( ctx->slabs ) );
  T_assert_true( ok );
  RunWorkload( ctx, 4 );

  /*
   * Check that the heap stayed consistent and that it holds its whole memory
   * again.
   */
  CheckEmpty( ctx );
}

/**
 * @brief Extend a heap by an area after its area, by an adjacent area and by
 *   an area before its area. Run a random sequence of operations on it.
 */
static void ScoreRbheapValRbheap_Action_2( ScoreRbheapValRbheap_Context *ctx )
{
  bool      ok;
  uintptr_t quarter;
  char     *area;

  quarter = sizeof( ctx->area ) / 4;
  area = ctx->area;
  ok = _RBHeap_Initialize( &ctx->heap, area + quarter, quarter );
  T_assert_true( ok );
  ok = _RBHeap_Extend( &ctx->heap, area + 3 * quarter, quarter );
  T_true( ok );
  ok = _RBHeap_Extend( &ctx->heap, area + 2 * quarter, quarter );
  T_true( ok );
  ok = _RBHeap_Extend( &ctx->heap, area, quarter / 2 );
  T_true( ok );
  ok = _RBHeap_Extend( &ctx->heap, area + quarter, quarter );
  T_false( ok );
  T_true( _RBHeap_Walk( &ctx->heap ) );
  RunWorkload( ctx, 5 );

  /*
   * Check that the heap stayed consistent and that it holds its whole memory
   * again.
   */
  CheckEmpty( ctx );
}

/**
 * @brief Free and allocate blocks of a heap with processor blocks and of a
 *   heap without processor blocks.
 */
static void ScoreRbheapValRbheap_Action_3( ScoreRbheapValRbheap_Context *ctx )
{
  RunProcessorBlocks( ctx );
}

/**
 * @brief Free the processor blocks of the current processor on the other
 *   processors while the current processor allocates them from its kept list.
 */
static void ScoreRbheapValRbheap_Action_4( ScoreRbheapValRbheap_Context *ctx )
{
  RunKeptRace( ctx );
}

/**
 * @brief Get the information and the statistics of a heap with blocks.
 */
static void ScoreRbheapValRbheap_Action_5( ScoreRbheapValRbheap_Context *ctx )
{
  RunInformation( ctx );
}

/**
 * @brief Initialize heaps with invalid memory areas.
 */
static void ScoreRbheapValRbheap_Action_6( ScoreRbheapValRbheap_Context *ctx )
{
  RunInitialize( ctx );
}

/**
 * @fn void T_case_body_ScoreRbheapValRbheap( void )
 */
T_TEST_CASE_FIXTURE( ScoreRbheapValRbheap, &ScoreRbheapValRbheap_Fixture )
{
  ScoreRbheapValRbheap_Context *ctx;

  ctx = T_fixture_context();

  ScoreRbheapValRbheap_Action_0( ctx );
  ScoreRbheapValRbheap_Action_1( ctx );
  ScoreRbheapValRbheap_Action_2( ctx );
  ScoreRbheapValRbheap_Action_3( ctx );
  ScoreRbheapValRbheap_Action_4( ctx );
  ScoreRbheapValRbheap_Action_5( ctx );
  ScoreRbheapValRbheap_Action_6( ctx );
}

/** @} */
