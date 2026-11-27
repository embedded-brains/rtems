/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreRbheapValRbheapEdges
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
#include <rtems/score/rbheap.h>
#include <rtems/score/rbtree.h>
#include <rtems/score/smpimpl.h>
#include <rtems/score/threaddispatch.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreRbheapValRbheapEdges spec:/score/rbheap/val/rbheap-edges
 *
 * @ingroup TestsuitesValidationHeap0
 *
 * @brief Tests the edge cases of the red-black tree heap.
 *
 * This test case performs the following actions:
 *
 * - Allocate blocks with a boundary and with invalid alignments and
 *   boundaries.
 *
 * - Allocate blocks with a boundary from free blocks which begin between two
 *   multiples of the boundary.
 *
 * - Allocate a block with a boundary while the free block of the best fit and
 *   its four successors cross a multiple of the boundary and a larger free
 *   block holds the block after the move.
 *
 * - Allocate blocks until the heap carves a slab. Then free and allocate one
 *   block several times.
 *
 * - Extend heaps without a spare descriptor by areas which hold no slab: right
 *   after the areas, right below the areas, at the top of the address space,
 *   and with a slab source.
 *
 * - Let the heap carve two slabs at the begin of a free block, each right
 *   after a used block. Free the blocks until the heap releases one of the
 *   slabs.
 *
 * - Let a heap with its slab outside of its area carve a slab inside the area.
 *   Free all blocks and add descriptors to the heap.
 *
 * - Return descriptors to two full slabs, so that both share a list of their
 *   count of descriptors in use. Return descriptors to the slab which is not
 *   the first of the list until it moves to another list.
 *
 * - Allocate an aligned block while the free block of the best fit does not
 *   hold it at its alignment and a successor of equal size does.
 *
 * - Allocate, resize and free with huge sizes and invalid blocks. Extend a
 *   heap by an area of a huge size.
 *
 * - Allocate blocks from a heap whose slab source returns no slab. Initialize
 *   heaps with slabs and invalid memory areas.
 *
 * - Add descriptors to heaps with valid and invalid memory areas.
 *
 * - Extend a heap by areas which fill parts of its holes, by areas next to its
 *   areas and by invalid areas.
 *
 * - Free blocks until the heap releases a slab and allocate blocks while the
 *   heap drains the slab.
 *
 * - Extend a heap by many small areas with gaps between them until the heap
 *   has no spare descriptor.
 *
 * - Extend and add descriptors to heaps at the ends of their areas, near the
 *   top of the address space, over used blocks and over holes.
 *
 * - Use up the spare descriptors of a heap with a slab source which returns no
 *   slab. Let the source return slabs. Resize a block, free a processor block
 *   on another processor, and free a processor block for a full bin.
 *
 * - Walk more heaps with descriptors which form invalid trees. Get the
 *   information of a heap with holes and extend heaps below their used blocks
 *   and below their added descriptors.
 *
 * - Walk heaps with descriptors which form invalid trees.
 *
 * - Free blocks until the heap drains a slab with descriptors in use. Extend
 *   the heap by areas with a gap until the heap has less than two spare
 *   descriptors. Grow a block into the free block after it.
 *
 * - Corrupt the descriptors and the counters of a heap one after the other and
 *   walk the heap.
 *
 * - Corrupt the slabs of descriptors, the slab lists and the counters of the
 *   descriptors of a heap with a drain one after the other and walk the heap.
 *
 * @{
 */

#define AREA_SIZE ( 32 * RBHEAP_SLAB_SIZE )

#define BLOCK_COUNT 1024

/**
 * @brief Test context for spec:/score/rbheap/val/rbheap-edges test case.
 */
typedef struct {
  /**
   * @brief This member contains the block which another processor frees.
   */
  void *remote;

  /**
   * @brief This member contains the heap.
   */
  RBHeap_control heap;

  /**
   * @brief This member contains blocks.
   */
  void *blocks[ BLOCK_COUNT ];

  /**
   * @brief This member provides the slabs of added descriptors.
   */
  char slabs[ 4 ][ RBHEAP_SLAB_SIZE ] RTEMS_ALIGNED( RBHEAP_SLAB_SIZE );

  /**
   * @brief This member provides the memory area of the heap.
   */
  char area[ AREA_SIZE ] RTEMS_ALIGNED( RBHEAP_SLAB_SIZE );
} ScoreRbheapValRbheapEdges_Context;

static ScoreRbheapValRbheapEdges_Context ScoreRbheapValRbheapEdges_Instance;

typedef ScoreRbheapValRbheapEdges_Context Context;

static void Init( Context *ctx, size_t offset, size_t size )
{
  bool ok;

  ok = _RBHeap_Initialize( &ctx->heap, &ctx->area[ offset ], size );
  T_quiet_true( ok );
}

static void RunBoundary( Context *ctx )
{
  RBHeap_control *heap;
  void           *ptr;

  heap = &ctx->heap;
  Init( ctx, 0, AREA_SIZE );
  T_null( _RBHeap_Allocate_aligned_with_boundary( heap, 8, 24, 0 ) );
  T_null( _RBHeap_Allocate_aligned_with_boundary( heap, 8, 8, 96 ) );
  T_null( _RBHeap_Allocate_aligned_with_boundary( heap, 8, 128, 64 ) );
  T_null( _RBHeap_Allocate_aligned_with_boundary( heap, 128, 8, 64 ) );
  ptr = _RBHeap_Allocate_aligned_with_boundary( heap, 32, 16, 64 );
  T_not_null( ptr );
  T_eq_uptr( (uintptr_t) ptr % 16, 0 );
  T_le_uptr( (uintptr_t) ptr % 64 + 32, 64 );
  ptr = _RBHeap_Allocate_aligned_with_boundary( heap, 32, 16, 0 );
  T_not_null( ptr );
  T_eq_uptr( (uintptr_t) ptr % 16, 0 );
  T_true( _RBHeap_Walk( heap ) );
}

#define CHUNK_SIZE 512

/*
 * Fills the heap with blocks of CHUNK_SIZE bytes and then with smaller blocks
 * until the heap has no free block.  Returns the count of chunks.
 */
static size_t Fill( Context *ctx )
{
  RBHeap_control *heap;
  uintptr_t       size;
  size_t          n;

  heap = &ctx->heap;
  Init( ctx, 0, AREA_SIZE );
  n = 0;

  while ( n < BLOCK_COUNT ) {
    ctx->blocks[ n ] = _RBHeap_Allocate(
      heap,
      CHUNK_SIZE - RBHEAP_GUARD_SIZE
    );

    if ( ctx->blocks[ n ] == NULL ) {
      break;
    }

    ++n;
  }

  for ( size = CHUNK_SIZE / 2; size >= RBHEAP_ALIGNMENT; size /= 2 ) {
    while ( _RBHeap_Allocate( heap, size - RBHEAP_GUARD_SIZE ) != NULL ) {
      /* Use the rest of the free memory */
    }
  }

  return n;
}

/*
 * Returns the index of the first chunk after the start index which begins at
 * the remainder modulo the modulus.  The chunk and the count minus one chunks
 * after it are adjacent, and their neighbours stay used.
 */
static size_t FindChunks(
  const Context *ctx,
  size_t         n,
  size_t         start,
  size_t         count,
  uintptr_t      modulus,
  uintptr_t      remainder
)
{
  size_t i;

  for ( i = start + 1; i + count < n; ++i ) {
    uintptr_t begin;
    size_t    j;
    bool      adjacent;

    begin = (uintptr_t) ctx->blocks[ i ];

    if ( begin % modulus != remainder ) {
      continue;
    }

    adjacent = (uintptr_t) ctx->blocks[ i - 1 ] + CHUNK_SIZE == begin;

    for ( j = 1; j <= count; ++j ) {
      adjacent = adjacent &&
                 (uintptr_t) ctx->blocks[ i + j ] == begin + j * CHUNK_SIZE;
    }

    if ( adjacent ) {
      return i;
    }
  }

  T_quiet_true( false );
  return 0;
}

static void FreeChunks( Context *ctx, size_t i, size_t count )
{
  size_t j;

  for ( j = 0; j < count; ++j ) {
    T_quiet_true( _RBHeap_Free( &ctx->heap, ctx->blocks[ i + j ] ) );
  }
}

static void RunBoundaryPlace( Context *ctx )
{
  RBHeap_control *heap;
  size_t          n;
  size_t          i;
  size_t          j;
  void           *ptr;

  heap = &ctx->heap;
  n = Fill( ctx );
  T_eq_uptr( _RBHeap_Get_largest_free_size( heap ), 0 );

  /* The free block lies between two multiples of the boundary */
  i = FindChunks( ctx, n, 0, 1, 2 * CHUNK_SIZE, CHUNK_SIZE );
  FreeChunks( ctx, i, 1 );
  T_eq_uptr( _RBHeap_Get_largest_free_size( heap ), CHUNK_SIZE );
  ptr = _RBHeap_Allocate_aligned_with_boundary( heap, 64, 0, 2 * CHUNK_SIZE );
  T_eq_ptr( ptr, ctx->blocks[ i ] );
  T_true( _RBHeap_Free( heap, ptr ) );

  /* At the begin of the free block, the block would cross a multiple */
  j = FindChunks( ctx, n, i + 1, 3, 2 * CHUNK_SIZE, CHUNK_SIZE );
  FreeChunks( ctx, j, 3 );
  T_eq_uptr( _RBHeap_Get_largest_free_size( heap ), 3 * CHUNK_SIZE );
  ptr = _RBHeap_Allocate_aligned_with_boundary(
    heap,
    3 * CHUNK_SIZE / 2,
    0,
    2 * CHUNK_SIZE
  );
  T_eq_ptr( ptr, ctx->blocks[ j + 1 ] );
  T_true( _RBHeap_Walk( heap ) );
}

static void RunBoundaryPadded( Context *ctx )
{
  RBHeap_control *heap;
  size_t          n;
  size_t          i;
  size_t          j;
  size_t          k;
  void           *ptr;

  heap = &ctx->heap;
  n = Fill( ctx );
  i = 0;

  /*
   * Each of these free blocks begins between two multiples of the boundary.
   * The block crosses the next multiple at the begin of the free block and
   * does not fit after the move.
   */
  for ( j = 0; j <= RBHEAP_FIND_SUCCESSORS; ++j ) {
    i = FindChunks( ctx, n, i, 2, 2 * CHUNK_SIZE, CHUNK_SIZE );
    FreeChunks( ctx, i, 2 );
    i += 2;
  }

  /* This free block holds the block after the move */
  k = FindChunks( ctx, n, i, 4, 2 * CHUNK_SIZE, CHUNK_SIZE );
  FreeChunks( ctx, k, 4 );
  ptr = _RBHeap_Allocate_aligned_with_boundary(
    heap,
    3 * CHUNK_SIZE / 2,
    0,
    2 * CHUNK_SIZE
  );
  T_eq_ptr( ptr, ctx->blocks[ k + 1 ] );
  T_true( _RBHeap_Walk( heap ) );
}

static void RunHysteresis( Context *ctx )
{
  RBHeap_control *heap;
  uintptr_t       slabs;
  uintptr_t       capacity;
  size_t          n;
  size_t          i;

  heap = &ctx->heap;
  Init( ctx, 0, AREA_SIZE );
  slabs = heap->area_slab_count;
  n = 0;

  while ( heap->area_slab_count == slabs ) {
    T_quiet_lt_sz( n, BLOCK_COUNT );
    ctx->blocks[ n ] = _RBHeap_Allocate( heap, 1 );
    T_quiet_not_null( ctx->blocks[ n ] );
    ++n;
  }

  slabs = heap->area_slab_count;
  capacity = heap->capacity;

  for ( i = 0; i < 8; ++i ) {
    T_quiet_true( _RBHeap_Free( heap, ctx->blocks[ n - 1 ] ) );
    T_eq_uptr( heap->area_slab_count, slabs );
    ctx->blocks[ n - 1 ] = _RBHeap_Allocate( heap, 1 );
    T_quiet_not_null( ctx->blocks[ n - 1 ] );
    T_eq_uptr( heap->area_slab_count, slabs );
    T_eq_uptr( heap->capacity, capacity );
  }

  T_true( _RBHeap_Walk( heap ) );
}

static void RunAlignedSuccessor( Context *ctx )
{
  RBHeap_control *heap;
  size_t          n;
  size_t          i;
  size_t          j;
  void           *ptr;

  heap = &ctx->heap;
  n = Fill( ctx );

  /* The free block of the best fit has the lower begin */
  i = FindChunks( ctx, n, 0, 2, 2 * CHUNK_SIZE, CHUNK_SIZE );
  j = FindChunks( ctx, n, i + 2, 2, 2 * CHUNK_SIZE, 0 );
  FreeChunks( ctx, i, 2 );
  FreeChunks( ctx, j, 2 );
  ptr = _RBHeap_Allocate_aligned(
    heap,
    2 * CHUNK_SIZE - RBHEAP_GUARD_SIZE,
    2 * CHUNK_SIZE
  );
  T_eq_ptr( ptr, ctx->blocks[ j ] );
  T_true( _RBHeap_Walk( heap ) );
}

static void RunHuge( Context *ctx )
{
  RBHeap_control *heap;
  void           *ptr;
  uintptr_t       old_size;
  uintptr_t       begin;

  heap = &ctx->heap;
  Init( ctx, 0, AREA_SIZE );
  T_null( _RBHeap_Allocate( heap, UINTPTR_MAX ) );
  T_null( _RBHeap_Allocate_aligned( heap, 8, UINTPTR_MAX / 2 + 1 ) );
  ptr = _RBHeap_Allocate( heap, 8 );
  T_assert_not_null( ptr );
  T_eq_int(
    _RBHeap_Resize( heap, ptr, UINTPTR_MAX, &old_size ),
    RBHEAP_RESIZE_UNSATISFIED
  );
  T_eq_int(
    _RBHeap_Resize( heap, ptr, 1, &old_size ),
    RBHEAP_RESIZE_SUCCESSFUL
  );
  T_eq_int(
    _RBHeap_Resize( heap, ctx->area, 1, &old_size ),
    RBHEAP_RESIZE_INVALID_BLOCK
  );
  T_eq_uptr( old_size, 0 );
  T_false( _RBHeap_Free( heap, ctx->area ) );
  T_false( _RBHeap_Size_of_block( heap, ctx->area, &old_size ) );
  T_true( _RBHeap_Free( heap, NULL ) );
  T_true( _RBHeap_Walk( heap ) );

  /*
   * The heap has spare descriptors, so the extension writes nothing into the
   * area.  No allocation takes a block of it.
   */
  Init( ctx, 0, AREA_SIZE );
  begin = (uintptr_t) &ctx->area[ AREA_SIZE ] + RBHEAP_SLAB_SIZE;
  T_true( _RBHeap_Extend( heap, (void *) begin, UINTPTR_MAX ) );
  T_eq_uptr(
    heap->area_end,
    ( begin + RBHEAP_MAX_SIZE ) & ~( (uintptr_t) RBHEAP_ALIGNMENT - 1 )
  );
  T_true( _RBHeap_Walk( heap ) );
}

static void *NoSlab( void )
{
  return NULL;
}

static void RunSlabSource( Context *ctx )
{
  RBHeap_control *heap;
  size_t          i;
  bool            ok;

  heap = &ctx->heap;
  ok = _RBHeap_Initialize_with_slab(
    heap,
    ctx->area,
    AREA_SIZE,
    ctx->slabs[ 0 ],
    NoSlab
  );
  T_assert_true( ok );

  for ( i = 0; i < BLOCK_COUNT; ++i ) {
    ctx->blocks[ i ] = _RBHeap_Allocate( heap, 8 );

    if ( ctx->blocks[ i ] == NULL ) {
      break;
    }
  }

  T_lt_sz( i, BLOCK_COUNT );
  T_ge_uptr( heap->capacity, 2 * heap->entries + 3 );
  T_true( _RBHeap_Walk( heap ) );
  T_false( _RBHeap_Initialize_with_slab(
    heap,
    (void *) ( UINTPTR_MAX - 15 ),
    64,
    ctx->slabs[ 0 ],
    NoSlab
  ) );
  T_false(
    _RBHeap_Initialize_with_slab( heap, ctx->area, 4, ctx->slabs[ 0 ], NULL )
  );
  ok = _RBHeap_Initialize_with_slab(
    heap,
    ctx->area,
    UINTPTR_MAX,
    ctx->slabs[ 0 ],
    NULL
  );
  T_true( ok );
}

static void RunAddDescriptors( Context *ctx )
{
  RBHeap_control *heap;
  uintptr_t       quarter;

  heap = &ctx->heap;
  quarter = AREA_SIZE / 4;
  Init( ctx, quarter, quarter );
  T_false( _RBHeap_Add_descriptors( heap, ctx->slabs[ 0 ], 8 ) );
  T_false(
    _RBHeap_Add_descriptors( heap, &ctx->area[ quarter ], RBHEAP_SLAB_SIZE )
  );
  T_true( _RBHeap_Extend( heap, &ctx->area[ 3 * quarter ], quarter ) );
  T_true( _RBHeap_Add_descriptors(
    heap,
    &ctx->area[ 2 * quarter ],
    RBHEAP_SLAB_SIZE
  ) );
  T_true( _RBHeap_Add_descriptors( heap, ctx->slabs[ 1 ], RBHEAP_SLAB_SIZE ) );
  T_true( _RBHeap_Add_descriptors( heap, ctx->slabs[ 0 ], RBHEAP_SLAB_SIZE ) );
  T_true(
    _RBHeap_Add_descriptors( heap, &ctx->area[ 0 ], 2 * RBHEAP_SLAB_SIZE )
  );
  T_false(
    _RBHeap_Extend( heap, &ctx->area[ 2 * quarter ], RBHEAP_SLAB_SIZE )
  );
  T_true( _RBHeap_Walk( heap ) );

  /*
   * The capacity of the added slabs exceeds the need, but the slabs lie
   * outside of the areas, so the heap frees none of them.
   */
  T_gt_uptr( heap->capacity, 2 * heap->entries + 3 + 2 * RBHEAP_SLAB_SLOTS );
  T_eq_uptr( heap->area_slab_count, 0 );
  T_eq_uptr( _RBHeap_Get_releasable_size( heap, 0 ), 0 );
}

static void RunExtend( Context *ctx )
{
  RBHeap_control *heap;
  uintptr_t       quarter;
  uintptr_t       s;
  char           *a;

  heap = &ctx->heap;
  quarter = AREA_SIZE / 4;
  s = quarter / 8;
  a = ctx->area;
  Init( ctx, quarter, quarter );
  T_false( _RBHeap_Extend( heap, a, 0 ) );
  T_false( _RBHeap_Extend( heap, (void *) ( UINTPTR_MAX - 3 ), 16 ) );
  T_false( _RBHeap_Extend( heap, a + quarter + s, s ) );
  T_true( _RBHeap_Extend( heap, a + 3 * quarter, quarter ) );
  T_true( _RBHeap_Extend( heap, a + 2 * quarter + 2 * s, s ) );
  T_true( _RBHeap_Extend( heap, a + 2 * quarter + 4 * s, s ) );
  T_true( _RBHeap_Extend( heap, a + 3 * quarter - s, s ) );
  T_true( _RBHeap_Extend( heap, a + 2 * quarter, s ) );
  T_true( _RBHeap_Extend( heap, a, s ) );
  T_true( _RBHeap_Extend( heap, a + quarter - s, s ) );
  T_true( _RBHeap_Walk( heap ) );
}

static void FreeEverySecond( Context *ctx, size_t count )
{
  size_t i;

  for ( i = 0; i < count; i += 2 ) {
    T_quiet_true( _RBHeap_Free( &ctx->heap, ctx->blocks[ i ] ) );
    ctx->blocks[ i ] = NULL;
  }
}

static size_t AllocateMany( Context *ctx, uintptr_t size )
{
  size_t i;

  for ( i = 0; i < BLOCK_COUNT; ++i ) {
    ctx->blocks[ i ] = _RBHeap_Allocate( &ctx->heap, size );

    if ( ctx->blocks[ i ] == NULL ) {
      break;
    }
  }

  return i;
}

static void RunDrain( Context *ctx )
{
  RBHeap_control *heap;
  size_t          count;
  size_t          i;
  size_t          pass;
  bool            cancelled;
  bool            drained;

  heap = &ctx->heap;
  Init( ctx, 0, AREA_SIZE );
  count = AllocateMany( ctx, 8 );
  T_gt_sz( count, 2 * RBHEAP_SLAB_SLOTS );
  cancelled = false;
  drained = false;

  /*
   * The first pass leaves the descriptors of the free blocks in their slabs.
   * The second pass merges free blocks, so that the descriptors in use
   * spread over the slabs and a drain takes several calls.
   */
  for ( pass = 0; pass < 2 && !drained; ++pass ) {
    for ( i = 1 - pass; i < count; i += 2 ) {
      T_quiet_true( _RBHeap_Free( heap, ctx->blocks[ i ] ) );
      ctx->blocks[ i ] = NULL;

      if ( heap->draining != NULL ) {
        void *ptr;

        drained = true;
        T_eq_uptr(
          _RBHeap_Get_releasable_size( heap, 0 ),
          _RBHeap_Get_releasable_size( heap, 1 )
        );
        ptr = _RBHeap_Allocate( heap, 8 );

        while ( ptr != NULL && heap->draining != NULL ) {
          ctx->blocks[ i ] = ptr;
          ptr = _RBHeap_Allocate( heap, 8 );
        }

        if ( ptr != NULL ) {
          cancelled = true;
          T_quiet_true( _RBHeap_Free( heap, ptr ) );
        }

        break;
      }
    }
  }

  T_true( drained );
  T_true( cancelled );
  T_true( _RBHeap_Walk( heap ) );
  T_ge_uptr( _RBHeap_Get_releasable_size( heap, 2 * count ), 0 );
}

static void ExtendUntil( Context *ctx, uintptr_t *next, uintptr_t spare )
{
  RBHeap_control *heap;

  heap = &ctx->heap;

  while ( heap->spare > spare && *next + 128 <= AREA_SIZE ) {
    T_quiet_true( _RBHeap_Extend( heap, &ctx->area[ *next ], 64 ) );
    *next += 128;
  }
}

static void RunExtendMany( Context *ctx )
{
  RBHeap_control *heap;
  void           *other;
  void           *third;
  uintptr_t       next;
  uintptr_t       old_size;

  heap = &ctx->heap;

  /*
   * The heap begins two slabs above the area.  The free block after the
   * blocks holds one slab at a multiple of the slab size.
   */
  Init( ctx, 2 * RBHEAP_SLAB_SIZE, 3 * RBHEAP_SLAB_SIZE );
  other = _RBHeap_Allocate( heap, 16 );
  T_assert_not_null( other );
  third = _RBHeap_Allocate( heap, 16 );
  T_assert_not_null( third );

  /* Each extension leaves a hole and adds a free block */
  next = 8 * RBHEAP_SLAB_SIZE;
  ExtendUntil( ctx, &next, 1 );
  T_le_uptr( heap->spare, 1 );

  /* The new slab comes from the free block after the blocks */
  T_true( _RBHeap_Extend( heap, &ctx->area[ next ], 64 ) );
  next += 128;
  T_true( _RBHeap_Walk( heap ) );
  ExtendUntil( ctx, &next, 1 );

  /* An area of a slab provides the slab itself */
  next = ( next + 2 * RBHEAP_SLAB_SIZE - 1 ) & ~( RBHEAP_SLAB_SIZE - 1 );
  T_true( _RBHeap_Extend( heap, &ctx->area[ next ], RBHEAP_SLAB_SIZE ) );
  next += 2 * RBHEAP_SLAB_SIZE;
  T_true( _RBHeap_Walk( heap ) );
  ExtendUntil( ctx, &next, 1 );

  /* An area right below the heap merges with no free block */
  if ( heap->spare == 1 ) {
    T_true(
      _RBHeap_Extend( heap, &ctx->area[ 2 * RBHEAP_SLAB_SIZE - 64 ], 64 )
    );
  }

  T_eq_uptr( heap->spare, 0 );

  /* No free block holds a slab, so the heap has no descriptor */
  T_false( _RBHeap_Extend( heap, &ctx->area[ next ], 64 ) );
  T_eq_int(
    _RBHeap_Resize( heap, other, 8, &old_size ),
    RBHEAP_RESIZE_SUCCESSFUL
  );
  T_true( _RBHeap_Walk( heap ) );
  T_eq_uptr( _RBHeap_Get_releasable_size( heap, 0 ), 0 );
  (void) third;
}

static void Link(
  RBHeap_descriptor *desc,
  RBHeap_descriptor *parent,
  RBHeap_descriptor *left,
  RBHeap_descriptor *right,
  int                color
)
{
  desc->node.rbn_left = left != NULL ? &left->node : NULL;
  desc->node.rbn_right = right != NULL ? &right->node : NULL;
  desc->node.rbn_parent_color = (uintptr_t) ( parent != NULL ? &parent->node
                                                             : NULL ) |
                                (uintptr_t) color;
}

static void RunFakeWalk( Context *ctx )
{
  RBHeap_control    *heap;
  RBHeap_descriptor *desc;

  heap = &ctx->heap;
  desc = (RBHeap_descriptor *) &ctx->slabs[ 2 ][ 0 ];
  memset( heap, 0, sizeof( *heap ) );
  memset( desc, 0, 3 * sizeof( *desc ) );

  /* A red node has a red right child */
  Link( &desc[ 0 ], NULL, &desc[ 1 ], NULL, RTEMS_RB_BLACK );
  Link( &desc[ 1 ], &desc[ 0 ], NULL, &desc[ 2 ], RTEMS_RB_RED );
  Link( &desc[ 2 ], &desc[ 1 ], NULL, NULL, RTEMS_RB_RED );
  heap->free_root.rbh_root = &desc[ 0 ].node;
  T_false( _RBHeap_Walk( heap ) );

  /* The root of the free tree is red */
  Link( &desc[ 0 ], NULL, NULL, NULL, RTEMS_RB_RED );
  T_false( _RBHeap_Walk( heap ) );

  /* A used node of size zero begins at the end of the previous block */
  heap->free_root.rbh_root = NULL;
  heap->area_begin = 0x1000;
  heap->area_end = 0x2000;
  Link( &desc[ 0 ], NULL, NULL, NULL, RTEMS_RB_BLACK );
  desc[ 0 ].begin = 0x1000;
  desc[ 0 ].size = 0;
  heap->used_root.rbh_root = &desc[ 0 ].node;
  T_false( _RBHeap_Walk( heap ) );
}

static void RunAreaEdges( Context *ctx )
{
  RBHeap_control *heap;
  char           *a;
  void           *ptr;
  bool            ok;

  heap = &ctx->heap;
  a = ctx->area;

  /* The heap has no used block, so no block precedes or follows an area */
  ok = _RBHeap_Initialize_with_slab(
    heap,
    &a[ 4 * RBHEAP_SLAB_SIZE ],
    RBHEAP_SLAB_SIZE,
    ctx->slabs[ 0 ],
    NULL
  );
  T_assert_true( ok );
  T_true( _RBHeap_Extend( heap, &a[ 5 * RBHEAP_SLAB_SIZE ], 64 ) );
  T_true( _RBHeap_Extend( heap, &a[ 4 * RBHEAP_SLAB_SIZE - 64 ], 64 ) );
  T_true( _RBHeap_Walk( heap ) );

  /* A single used block which is no slab */
  ptr = _RBHeap_Allocate( heap, 16 );
  T_assert_not_null( ptr );
  T_true( _RBHeap_Add_descriptors( heap, ctx->slabs[ 1 ], RBHEAP_SLAB_SIZE ) );
  T_true( _RBHeap_Add_descriptors( heap, ctx->slabs[ 2 ], RBHEAP_SLAB_SIZE ) );
  T_true( _RBHeap_Walk( heap ) );

  /* The slab fills the whole area, so the last used block ends the area */
  Init( ctx, 0, RBHEAP_SLAB_SIZE );
  T_eq_uptr( heap->free_size, 0 );
  T_true( _RBHeap_Extend( heap, &a[ RBHEAP_SLAB_SIZE ], 64 ) );
  T_true( _RBHeap_Walk( heap ) );

  /* A free block precedes the slab of an area which is not slab aligned */
  Init( ctx, 64, 2 * RBHEAP_SLAB_SIZE - 64 );
  T_true( _RBHeap_Extend( heap, &a[ 0 ], 64 ) );
  T_true( _RBHeap_Walk( heap ) );

  /* Descriptors over a used block, beyond a hole and near the top */
  T_false( _RBHeap_Add_descriptors( heap, &a[ 0 ], 2 * RBHEAP_SLAB_SIZE ) );
  T_true( _RBHeap_Extend( heap, &a[ 6 * RBHEAP_SLAB_SIZE ], 64 ) );
  T_false( _RBHeap_Add_descriptors(
    heap,
    &a[ 2 * RBHEAP_SLAB_SIZE ],
    5 * RBHEAP_SLAB_SIZE
  ) );
  T_false( _RBHeap_Add_descriptors(
    heap,
    (void *) ( UINTPTR_MAX - RBHEAP_SLAB_SIZE / 2 ),
    RBHEAP_SLAB_SIZE
  ) );
  T_true( _RBHeap_Walk( heap ) );

  /* Areas near the top of the address space and below a slab boundary */
  T_false( _RBHeap_Initialize(
    heap,
    (void *) ( UINTPTR_MAX - 4 * RBHEAP_ALIGNMENT + 1 ),
    2 * RBHEAP_ALIGNMENT
  ) );
  T_false( _RBHeap_Initialize( heap, &a[ 64 ], 128 ) );
  T_false( _RBHeap_Initialize_with_slab(
    heap,
    (void *) ( UINTPTR_MAX - RBHEAP_ALIGNMENT + 2 ),
    2 * RBHEAP_ALIGNMENT,
    ctx->slabs[ 0 ],
    NULL
  ) );
}

static bool source_enabled;

static size_t source_next;

static Context *source_context;

static void *ControlledSource( void )
{
  if ( !source_enabled || source_next >= 4 ) {
    return NULL;
  }

  ++source_next;
  return source_context->slabs[ source_next - 1 ];
}

static void *source_pads[ 4 ];

static size_t source_pad;

static void UseUpSpare( Context *ctx, uintptr_t *next )
{
  RBHeap_control *heap;

  heap = &ctx->heap;
  source_enabled = false;
  ExtendUntil( ctx, next, 1 );

  if ( heap->spare == 1 ) {
    uintptr_t old_size;

    /* The free rest of a pad block before a used block takes one */
    T_eq_int(
      _RBHeap_Resize( heap, source_pads[ source_pad ], 8, &old_size ),
      RBHEAP_RESIZE_SUCCESSFUL
    );
    ++source_pad;
  }

  T_eq_uptr( heap->spare, 0 );
  source_enabled = true;
}

#if defined( RTEMS_SMP )
static void FreeOnProcessor( void *arg )
{
  Context *ctx;

  ctx = arg;
  T_quiet_true( _RBHeap_Free_to_processor( &ctx->heap, ctx->remote ) );
}
#endif

#define SHORT_BEGIN ( 16 * RBHEAP_SLAB_SIZE )

#define SHORT_SIZE ( 4 * RBHEAP_SLAB_SIZE )

typedef enum { SHORT_BELOW, SHORT_ABOVE } ShortSide;

/*
 * Prepares a heap at SHORT_BEGIN without a spare descriptor.  Used blocks lie
 * at both ends of its area.  One free block of a slab begins at a multiple of
 * the slab size.  The extensions which use up the spare descriptors lie on
 * the side.
 */
static void PrepareShort( Context *ctx, ShortSide side, bool source )
{
  RBHeap_control *heap;
  void           *small;
  void           *slab_block;
  void           *ptr;
  uintptr_t       next;
  uintptr_t       last;
  uintptr_t       old_size;
  bool            ok;

  heap = &ctx->heap;

  if ( source ) {
    source_context = ctx;
    source_enabled = true;
    source_next = 1;
    ok = _RBHeap_Initialize_with_slab(
      heap,
      &ctx->area[ SHORT_BEGIN ],
      SHORT_SIZE,
      ctx->slabs[ 0 ],
      ControlledSource
    );
    T_quiet_true( ok );
  } else {
    Init( ctx, SHORT_BEGIN, SHORT_SIZE );
  }

  /* The small block lies between used blocks */
  small = _RBHeap_Allocate( heap, 64 - RBHEAP_GUARD_SIZE );
  T_quiet_not_null( small );
  ptr = _RBHeap_Allocate(
    heap,
    ( RBHEAP_SLAB_SIZE - (uintptr_t) small % RBHEAP_SLAB_SIZE ) - 64 -
      RBHEAP_GUARD_SIZE
  );
  T_quiet_not_null( ptr );
  slab_block = _RBHeap_Allocate_aligned(
    heap,
    RBHEAP_SLAB_SIZE - RBHEAP_GUARD_SIZE,
    RBHEAP_SLAB_SIZE
  );
  T_quiet_not_null( slab_block );

  while ( _RBHeap_Allocate( heap, RBHEAP_SLAB_SIZE - RBHEAP_GUARD_SIZE ) ) {
    /* Use the area up to its end */
  }

  T_quiet_true( _RBHeap_Free( heap, slab_block ) );

  if ( side == SHORT_BELOW ) {
    next = 0;
    last = SHORT_BEGIN - 2 * 128;
  } else {
    next = SHORT_BEGIN + SHORT_SIZE + 2 * 128;
    last = AREA_SIZE - 128;
  }

  while ( heap->spare >= 2 && next <= last ) {
    T_quiet_true( _RBHeap_Extend( heap, &ctx->area[ next ], 64 ) );
    next += 128;
  }

  /* The free rest of the shrunk block takes the last spare descriptor */
  if ( heap->spare == 1 ) {
    T_quiet_eq_int(
      _RBHeap_Resize( heap, small, RBHEAP_MIN_SIZE, &old_size ),
      RBHEAP_RESIZE_SUCCESSFUL
    );
  }

  T_quiet_eq_uptr( heap->spare, 0 );
  T_quiet_true( _RBHeap_Walk( heap ) );
}

static void RunExtendShort( Context *ctx )
{
  RBHeap_control *heap;
  uintptr_t       top;
  uintptr_t       capacity;

  heap = &ctx->heap;

  /* The area merges below, the free block of the slab provides the slab */
  PrepareShort( ctx, SHORT_BELOW, false );
  capacity = heap->capacity;
  T_true( _RBHeap_Extend( heap, &ctx->area[ SHORT_BEGIN + SHORT_SIZE ], 64 ) );
  T_eq_uptr( heap->capacity, capacity + RBHEAP_SLAB_SLOTS - 1 );
  T_true( _RBHeap_Walk( heap ) );

  /* The area merges above */
  PrepareShort( ctx, SHORT_ABOVE, false );
  capacity = heap->capacity;
  T_true( _RBHeap_Extend( heap, &ctx->area[ SHORT_BEGIN - 64 ], 64 ) );
  T_eq_uptr( heap->capacity, capacity + RBHEAP_SLAB_SLOTS - 1 );
  T_true( _RBHeap_Walk( heap ) );

  /* The slab source provides the slab */
  PrepareShort( ctx, SHORT_BELOW, true );
  T_true( _RBHeap_Extend( heap, &ctx->area[ SHORT_BEGIN + SHORT_SIZE ], 64 ) );
  T_eq_sz( source_next, 2 );
  T_true( _RBHeap_Walk( heap ) );
  source_enabled = false;

  /*
   * At the top of the address space, the begin of a slab wraps.  The heap
   * writes nothing into the area.
   */
  PrepareShort( ctx, SHORT_BELOW, false );
  top = ~(uintptr_t) 127;
  capacity = heap->capacity;
  T_true( _RBHeap_Extend( heap, (void *) top, 64 ) );
  T_eq_uptr( heap->capacity, capacity + RBHEAP_SLAB_SLOTS - 1 );
  T_eq_uptr( heap->area_end, top + 64 );
  T_true( _RBHeap_Walk( heap ) );
}

static void RunSourceSpare( Context *ctx )
{
  RBHeap_control *heap;
  void           *bound[ 2 ];
  void           *other;
  void           *third;
  uintptr_t       next;
  uintptr_t       old_size;
  bool            ok;
#if defined( RTEMS_SMP )
  Per_CPU_Control *cpu_self;
  uint32_t         cpus;
#endif

  heap = &ctx->heap;
  source_context = ctx;
  source_enabled = false;
  source_next = 1;
  ok = _RBHeap_Initialize_with_slab(
    heap,
    &ctx->area[ 2 * RBHEAP_SLAB_SIZE ],
    2 * RBHEAP_SLAB_SIZE,
    ctx->slabs[ 0 ],
    ControlledSource
  );
  T_assert_true( ok );

#if defined( RTEMS_SMP )
  cpus = rtems_configuration_get_maximum_processors();
  T_assert_true( _RBHeap_Initialize_processor_blocks( heap, cpus, 16, 64 ) );
  cpu_self = _Thread_Dispatch_disable();
#endif

  bound[ 0 ] = _RBHeap_Allocate( heap, 16 );
  T_assert_not_null( bound[ 0 ] );
  bound[ 1 ] = _RBHeap_Allocate( heap, 16 );
  T_assert_not_null( bound[ 1 ] );
  other = _RBHeap_Allocate( heap, 16 );
  T_assert_not_null( other );

  for (
    source_pad = 0; source_pad < RTEMS_ARRAY_SIZE( source_pads ); ++source_pad
  ) {
    source_pads[ source_pad ] = _RBHeap_Allocate( heap, 16 );
    T_assert_not_null( source_pads[ source_pad ] );
  }

  source_pad = 0;
  third = _RBHeap_Allocate( heap, 16 );
  T_assert_not_null( third );

#if defined( RTEMS_SMP )
  /* The first block becomes a used processor block, the second one binned */
  T_true( _RBHeap_Free_to_processor( heap, bound[ 0 ] ) );
  T_eq_ptr( _RBHeap_Allocate_from_processor( heap, 16 ), bound[ 0 ] );
  T_true( _RBHeap_Free_to_processor( heap, bound[ 1 ] ) );
#endif

  next = 8 * RBHEAP_SLAB_SIZE;

  /* The source provides the slab of the descriptor of the free rest */
  UseUpSpare( ctx, &next );
  T_eq_int(
    _RBHeap_Resize( heap, other, 8, &old_size ),
    RBHEAP_RESIZE_SUCCESSFUL
  );
  T_gt_uptr( heap->spare, 0 );

#if defined( RTEMS_SMP )
  /* The block of a free on another processor goes into the kept list */
  if ( cpus > 1 ) {
    ctx->remote = bound[ 0 ];
    _SMP_Unicast_action(
      ( _Per_CPU_Get_index( cpu_self ) + 1 ) % cpus,
      FreeOnProcessor,
      ctx
    );
    T_true( _RBHeap_Size_of_block( heap, bound[ 0 ], &old_size ) );
  }

  /*
   * The source provides the slab of the free block of a processor block for
   * a full bin.
   */
  T_eq_ptr( _RBHeap_Allocate_from_processor( heap, 16 ), bound[ 1 ] );
  T_true( _RBHeap_Free_to_processor( heap, third ) );
  UseUpSpare( ctx, &next );
  T_true( _RBHeap_Free_to_processor( heap, bound[ 1 ] ) );
  T_false( _RBHeap_Size_of_block( heap, bound[ 1 ], &old_size ) );
  _Thread_Dispatch_enable( cpu_self );
#endif

  T_true( _RBHeap_Walk( heap ) );
  (void) third;
}

static void RunFakeWalkMore( Context *ctx )
{
  RBHeap_control    *heap;
  RBHeap_descriptor *desc;

  heap = &ctx->heap;
  desc = (RBHeap_descriptor *) &ctx->slabs[ 2 ][ 0 ];
  memset( heap, 0, sizeof( *heap ) );
  memset( desc, 0, 4 * sizeof( *desc ) );

  /* The left child names another parent */
  Link( &desc[ 0 ], NULL, &desc[ 1 ], &desc[ 2 ], RTEMS_RB_BLACK );
  Link( &desc[ 1 ], &desc[ 2 ], NULL, NULL, RTEMS_RB_BLACK );
  Link( &desc[ 2 ], &desc[ 0 ], NULL, NULL, RTEMS_RB_BLACK );
  heap->free_root.rbh_root = &desc[ 0 ].node;
  T_false( _RBHeap_Walk( heap ) );

  /* A red node has a red left child */
  Link( &desc[ 0 ], NULL, &desc[ 1 ], NULL, RTEMS_RB_BLACK );
  Link( &desc[ 1 ], &desc[ 0 ], &desc[ 2 ], NULL, RTEMS_RB_RED );
  Link( &desc[ 2 ], &desc[ 1 ], NULL, NULL, RTEMS_RB_RED );
  T_false( _RBHeap_Walk( heap ) );

  /* Two used blocks overlap */
  heap->free_root.rbh_root = NULL;
  heap->area_begin = 0x1000;
  heap->area_end = 0x2000;
  Link( &desc[ 0 ], NULL, NULL, &desc[ 1 ], RTEMS_RB_BLACK );
  Link( &desc[ 1 ], &desc[ 0 ], NULL, NULL, RTEMS_RB_RED );
  desc[ 0 ].begin = 0x1000;
  desc[ 0 ].size = 0x100;
  desc[ 1 ].begin = 0x1080;
  desc[ 1 ].size = 0x100;
  heap->used_root.rbh_root = &desc[ 0 ].node;
  T_false( _RBHeap_Walk( heap ) );
}

static void RunHoles( Context *ctx )
{
  RBHeap_control        *heap;
  Heap_Information_block info;
  char                  *a;
  void                  *ptr[ 3 ];
  size_t                 i;

  heap = &ctx->heap;
  a = ctx->area;

  /* Blocks after the slab give the root of the used tree a left child */
  Init( ctx, 64, 6 * RBHEAP_SLAB_SIZE - 64 );

  for ( i = 0; i < RTEMS_ARRAY_SIZE( ptr ); ++i ) {
    ptr[ i ] = _RBHeap_Allocate( heap, RBHEAP_SLAB_SIZE );
    T_assert_not_null( ptr[ i ] );
  }

  T_true( _RBHeap_Extend( heap, &a[ 0 ], 64 ) );

  /* An extension leaves a hole for the information */
  T_true( _RBHeap_Extend( heap, &a[ 8 * RBHEAP_SLAB_SIZE ], 64 ) );
  _RBHeap_Get_information( heap, &info );
  T_eq_uptr( info.Used.number, 3 );

  /* Descriptors above the areas, then an area below them */
  T_true( _RBHeap_Add_descriptors(
    heap,
    &a[ 12 * RBHEAP_SLAB_SIZE ],
    RBHEAP_SLAB_SIZE
  ) );
  T_true( _RBHeap_Extend( heap, &a[ 10 * RBHEAP_SLAB_SIZE ], 64 ) );
  T_true( _RBHeap_Walk( heap ) );
}

static RBHeap_descriptor *Node( RBTree_Node *node )
{
  return (RBHeap_descriptor *) node;
}

static void FlipColor( RBTree_Node *node )
{
  node->rbn_parent_color ^= RTEMS_RB_COLOR_MASK;
}

static void CheckCorrupt(
  RBHeap_control *heap,
  uintptr_t      *field,
  uintptr_t       value
)
{
  uintptr_t saved;

  saved = *field;
  *field = value;
  T_false( _RBHeap_Walk( heap ) );
  *field = saved;
  T_true( _RBHeap_Walk( heap ) );
}

static void CheckCorruptU32( RBHeap_control *heap, uint32_t *field )
{
  ++( *field );
  T_false( _RBHeap_Walk( heap ) );
  --( *field );
}

#define DRAIN_RESERVE ( 4 * RBHEAP_SLAB_SIZE )

/*
 * Fills the heap with small blocks and frees blocks until the heap drains a
 * slab with descriptors in use.  The area leaves DRAIN_RESERVE bytes of the
 * memory for extensions.  Returns the count of blocks.
 */
static size_t PrepareDrainInUse( Context *ctx )
{
  RBHeap_control *heap;
  size_t          n;
  size_t          i;

  heap = &ctx->heap;
  Init( ctx, 0, AREA_SIZE - DRAIN_RESERVE );

  for ( n = 0; n < BLOCK_COUNT; ++n ) {
    ctx->blocks[ n ] = _RBHeap_Allocate( heap, RBHEAP_MIN_SIZE );

    if ( ctx->blocks[ n ] == NULL ) {
      break;
    }
  }

  /* Free the blocks 1 and 2 of each four, then the block 3 */
  for ( i = 0; i < 2 * n; ++i ) {
    size_t j;

    j = i % n;

    if (
      ctx->blocks[ j ] != NULL &&
      ( ( i < n && ( j % 4 == 1 || j % 4 == 2 ) ) || ( i >= n && j % 4 == 3 ) )
    ) {
      T_quiet_true( _RBHeap_Free( heap, ctx->blocks[ j ] ) );
      ctx->blocks[ j ] = NULL;

      if ( heap->draining != NULL && heap->draining->live > 0 ) {
        break;
      }
    }
  }

  T_assert_not_null( heap->draining );
  T_assert_gt_u32( heap->draining->live, 0 );
  return n;
}

/*
 * Lets the heap drain a slab with descriptors in use.  The extensions take
 * the spare descriptors.  The heap cancels the drain before an extension
 * takes a spare descriptor which the drain needs.
 */
static void RunExtendDrain( Context *ctx )
{
  RBHeap_control *heap;
  uintptr_t       begin;
  uintptr_t       old_size;
  size_t          n;
  size_t          i;

  heap = &ctx->heap;
  n = PrepareDrainInUse( ctx );
  T_true( _RBHeap_Walk( heap ) );

  while ( heap->spare >= 2 ) {
    begin = heap->area_end + RBHEAP_ALIGNMENT;
    T_quiet_le_uptr(
      begin + RBHEAP_ALIGNMENT,
      (uintptr_t) &ctx->area[ AREA_SIZE ]
    );
    T_quiet_true( _RBHeap_Extend( heap, (void *) begin, RBHEAP_ALIGNMENT ) );
  }

  T_null( heap->draining );
  T_true( _RBHeap_Walk( heap ) );

  for ( i = 0; i + 1 < n; i += 4 ) {
    if ( ctx->blocks[ i ] != NULL && ctx->blocks[ i + 1 ] == NULL ) {
      break;
    }
  }

  T_assert_lt_sz( i + 1, n );
  T_true( _RBHeap_Size_of_block( heap, ctx->blocks[ i ], &old_size ) );
  T_eq_int(
    _RBHeap_Resize(
      heap,
      ctx->blocks[ i ],
      old_size + RBHEAP_ALIGNMENT,
      &old_size
    ),
    RBHEAP_RESIZE_SUCCESSFUL
  );
  T_true( _RBHeap_Walk( heap ) );
}

/* Changes the field, checks that the walk fails, and restores the field */
#define CHECK_CORRUPT( heap, field, value ) \
  do {                                      \
    __typeof__( field ) saved_ = ( field ); \
    ( field ) = ( value );                  \
    T_false( _RBHeap_Walk( heap ) );        \
    ( field ) = saved_;                     \
    T_true( _RBHeap_Walk( heap ) );         \
  } while ( 0 )

static RBHeap_descriptor *Slot( RBHeap_slab *slab, uintptr_t index )
{
  return (RBHeap_descriptor *) slab + 1 + index;
}

static RBHeap_slab *SlabOf( const RBHeap_descriptor *desc )
{
  return (RBHeap_slab *) ( (uintptr_t) desc & ~( RBHEAP_SLAB_SIZE - 1 ) );
}

static uintptr_t IndexOf( const RBHeap_descriptor *desc )
{
  return (uintptr_t) ( desc - Slot( SlabOf( desc ), 0 ) );
}

static bool IsOwn( const RBHeap_descriptor *desc )
{
  return (uintptr_t) desc - desc->begin < desc->size;
}

typedef struct {
  uintptr_t size;
  uintptr_t begin;
} FreeKey;

static bool FreeLess( const void *key, const RBTree_Node *node )
{
  const FreeKey           *k;
  const RBHeap_descriptor *desc;

  k = key;
  desc = (const RBHeap_descriptor *) node;
  return k->size < desc->size ||
         ( k->size == desc->size && k->begin < desc->begin );
}

/*
 * Returns a used descriptor in a slab which has no descriptor in use above
 * it in its slab, else NULL.  With below set, the block lies below its
 * descriptor.
 */
static RBHeap_descriptor *TopUsed( RBHeap_control *heap, bool below )
{
  RBTree_Node *node;

  for (
    node = _RBTree_Minimum( &heap->used_root ); node != NULL;
    node = _RBTree_Successor( node )
  ) {
    RBHeap_descriptor *desc;
    RBHeap_slab       *slab;
    uintptr_t          i;

    desc = Node( node );

    if ( IsOwn( desc ) || ( desc->begin & 1 ) != 0 ) {
      continue;
    }

    if ( below ) {
      if ( desc->begin < (uintptr_t) desc ) {
        return desc;
      }

      continue;
    }

    slab = SlabOf( desc );

    for ( i = IndexOf( desc ) + 1; i < slab->untouched; ++i ) {
      if ( Slot( slab, i )->size != 0 ) {
        break;
      }
    }

    if ( i == slab->untouched ) {
      return desc;
    }
  }

  return NULL;
}

/* Returns a used descriptor whose block begins at the end of the entry before */
static RBHeap_descriptor *UsedAfterEntry( RBHeap_control *heap )
{
  RBTree_Node *node;

  for (
    node = _RBTree_Minimum( &heap->used_root ); node != NULL;
    node = _RBTree_Successor( node )
  ) {
    RBHeap_descriptor *prev;
    RBHeap_descriptor *desc;

    desc = Node( node );

    if ( IsOwn( desc ) || _RBTree_Predecessor( node ) == NULL ) {
      continue;
    }

    prev = Node( _RBTree_Predecessor( node ) );

    if ( prev->begin + prev->size == desc->begin ) {
      return desc;
    }
  }

  return NULL;
}

/* Returns true if the slab has two returned descriptors */
static bool HasTwoReturned( const RBHeap_slab *slab )
{
  return slab->returned != NULL && slab->returned->begin != 0;
}

/*
 * Returns a slab of the partial list or of the occupied lists with two
 * returned descriptors, else NULL.
 */
static RBHeap_slab *ListedWithReturned( RBHeap_control *heap )
{
  RBHeap_slab *slab;
  size_t       i;

  for ( slab = heap->partial; slab != NULL; slab = slab->next ) {
    if ( HasTwoReturned( slab ) ) {
      return slab;
    }
  }

  for ( i = 0; i < RBHEAP_SLAB_CLASSES; ++i ) {
    for ( slab = heap->occupied[ i ]; slab != NULL; slab = slab->next ) {
      if ( HasTwoReturned( slab ) ) {
        return slab;
      }
    }
  }

  return NULL;
}

/*
 * Brings the heap into a state with two full added slabs, a listed slab
 * with two returned descriptors, and two added slabs without a descriptor in
 * use.  The first allocations take the descriptors of the first added slabs.
 * The frees leave the blocks of these descriptors.
 */
static void PrepareCorruptSlabs( Context *ctx )
{
  RBHeap_control *heap;
  size_t          n;
  size_t          i;

  heap = &ctx->heap;
  Init( ctx, 0, AREA_SIZE );
  ctx->blocks[ 0 ] = _RBHeap_Allocate( heap, RBHEAP_MIN_SIZE );
  T_quiet_not_null( ctx->blocks[ 0 ] );
  T_quiet_true(
    _RBHeap_Add_descriptors( heap, ctx->slabs[ 0 ], 2 * RBHEAP_SLAB_SIZE )
  );
  n = 3 * RBHEAP_SLAB_SLOTS;

  for ( i = 1; i < n; ++i ) {
    ctx->blocks[ i ] = _RBHeap_Allocate( heap, RBHEAP_MIN_SIZE );
    T_quiet_not_null( ctx->blocks[ i ] );
  }

  for ( i = 2 * RBHEAP_SLAB_SLOTS + 4; i + 1 < n; i += 4 ) {
    T_quiet_true( _RBHeap_Free( heap, ctx->blocks[ i ] ) );
    T_quiet_true( _RBHeap_Free( heap, ctx->blocks[ i + 1 ] ) );
  }

  T_quiet_true(
    _RBHeap_Add_descriptors( heap, ctx->slabs[ 2 ], 2 * RBHEAP_SLAB_SIZE )
  );
}

/* Returns a slab of the occupied lists or the empty list, else NULL */
static RBHeap_slab *ListedInArea( RBHeap_control *heap )
{
  size_t i;

  for ( i = 0; i < RBHEAP_SLAB_CLASSES; ++i ) {
    if ( heap->occupied[ i ] != NULL ) {
      return heap->occupied[ i ];
    }
  }

  return heap->empty;
}

static void RunCorruptWalkSlabs( Context *ctx )
{
  RBHeap_control    *heap;
  RBHeap_slab       *full;
  RBHeap_slab       *added;
  RBHeap_slab       *occupied;
  RBHeap_slab       *draining;
  RBHeap_descriptor *desc;
  RBHeap_descriptor *other;
  RBHeap_descriptor *copy;
  RBHeap_descriptor  saved;
  FreeKey            key;
  uintptr_t          i;

  heap = &ctx->heap;
  PrepareCorruptSlabs( ctx );
  T_assert_true( _RBHeap_Walk( heap ) );
  full = (RBHeap_slab *) ctx->slabs[ 0 ];
  T_assert_eq_u32( full->live, RBHEAP_SLAB_SLOTS - 1 );
  added = heap->partial;
  T_assert_true(
    added == (RBHeap_slab *) ctx->slabs[ 2 ] ||
    added == (RBHeap_slab *) ctx->slabs[ 3 ]
  );
  T_assert_eq_u32( added->live, 0 );
  T_assert_not_null( added->next );
  occupied = ListedWithReturned( heap );
  T_assert_not_null( occupied );

  /* A slab without a descriptor in use */
  CHECK_CORRUPT( heap, added->untouched, 0 );
  CHECK_CORRUPT( heap, added->untouched, RBHEAP_SLAB_SLOTS + 1 );
  CHECK_CORRUPT( heap, added->live, RBHEAP_SLAB_SLOTS - 1 );
  CHECK_CORRUPT( heap, added->next->prev, added->next );
  CHECK_CORRUPT( heap, heap->empty, added );
  CHECK_CORRUPT( heap, heap->home, added );

  /* The list of the returned descriptors */
  desc = occupied->returned;
  other = (RBHeap_descriptor *) desc->begin;
  CHECK_CORRUPT( heap, occupied->returned, other );
  CHECK_CORRUPT( heap, desc->begin, (uintptr_t) Slot( added, 1 ) );
  CHECK_CORRUPT( heap, desc->begin, (uintptr_t) Slot( occupied, 0 ) );
  CHECK_CORRUPT(
    heap,
    desc->begin,
    (uintptr_t) Slot( occupied, occupied->untouched )
  );
  CHECK_CORRUPT( heap, desc->begin, (uintptr_t) other + 1 );

  for ( i = 1; Slot( occupied, i )->size == 0; ++i ) {
    /* Find a descriptor in use */
  }

  CHECK_CORRUPT( heap, desc->begin, (uintptr_t) Slot( occupied, i ) );

  while ( (RBHeap_descriptor *) other->begin != NULL ) {
    other = (RBHeap_descriptor *) other->begin;
  }

  CHECK_CORRUPT( heap, other->begin, (uintptr_t) occupied->returned );
  CHECK_CORRUPT( heap, occupied->prev, occupied );

  /* The full slab */
  CHECK_CORRUPT( heap, full->returned, desc );
  CHECK_CORRUPT( heap, Slot( full, 0 )->begin, (uintptr_t) full );

  /* The descriptors of the trees */
  desc = TopUsed( heap, false );
  T_assert_not_null( desc );
  CHECK_CORRUPT( heap, SlabOf( desc )->untouched, (uint32_t) IndexOf( desc ) );
  desc = UsedAfterEntry( heap );
  T_assert_not_null( desc );
  CHECK_CORRUPT( heap, desc->begin, desc->begin - RBHEAP_ALIGNMENT );
  desc = TopUsed( heap, true );
  T_assert_not_null( desc );
  CHECK_CORRUPT(
    heap,
    desc->size,
    (uintptr_t) desc - desc->begin + RBHEAP_ALIGNMENT
  );

  desc = Node( _RBTree_Minimum( &heap->free_root ) );
  copy = (RBHeap_descriptor *) ( (uintptr_t) Slot( added, 1 ) +
                                 sizeof( uintptr_t ) );
  copy->begin = desc->begin;
  copy->size = desc->size;
  _RBTree_Replace_node( &heap->free_root, &desc->node, &copy->node );
  added->untouched = 3;
  T_false( _RBHeap_Walk( heap ) );
  added->untouched = 1;
  _RBTree_Replace_node( &heap->free_root, &copy->node, &desc->node );
  T_true( _RBHeap_Walk( heap ) );

  copy = Slot( added, 1 );
  saved = *copy;
  copy->begin = desc->begin;
  copy->size = desc->size;
  key.size = desc->size;
  key.begin = desc->begin;
  _RBTree_Set_off_tree( &copy->node );
  (void)
    _RBTree_Insert_inline( &heap->free_root, &copy->node, &key, FreeLess );
  added->untouched = 2;
  added->live = 1;
  T_false( _RBHeap_Walk( heap ) );
  _RBTree_Extract( &heap->free_root, &copy->node );
  added->untouched = 1;
  added->live = 0;
  *copy = saved;
  T_true( _RBHeap_Walk( heap ) );

  /* The counters */
  copy = Slot( added, 1 );
  saved = *copy;
  copy->size = 1;
  added->untouched = 2;
  added->live = 1;
  T_false( _RBHeap_Walk( heap ) );
  added->untouched = 1;
  added->live = 0;
  *copy = saved;
  T_true( _RBHeap_Walk( heap ) );
  CHECK_CORRUPT( heap, heap->entries, heap->entries + 1 );
  CHECK_CORRUPT( heap, heap->spare, heap->spare + 1 );
  CHECK_CORRUPT(
    heap,
    heap->capacity,
    heap->capacity + RBHEAP_SLAB_SLOTS - 1
  );
  CHECK_CORRUPT(
    heap,
    heap->capacity,
    heap->capacity - ( RBHEAP_SLAB_SLOTS - 1 )
  );
  CHECK_CORRUPT( heap, heap->empty_count, heap->empty_count + 1 );
  CHECK_CORRUPT( heap, heap->area_slab_count, heap->area_slab_count + 1 );
  CHECK_CORRUPT( heap, heap->slab_source, ControlledSource );

  /* The draining slab */
  (void) PrepareDrainInUse( ctx );
  T_quiet_true(
    _RBHeap_Add_descriptors( heap, ctx->slabs[ 0 ], RBHEAP_SLAB_SIZE )
  );
  T_assert_true( _RBHeap_Walk( heap ) );
  draining = heap->draining;
  T_assert_gt_u32( heap->drain_slot, 1 );
  added = heap->partial;
  T_assert_eq_ptr( added, (RBHeap_slab *) ctx->slabs[ 0 ] );
  occupied = ListedInArea( heap );
  T_assert_not_null( occupied );
  CHECK_CORRUPT( heap, heap->draining, added );
  i = heap->drain_slot;
  heap->drain_slot = 1;
  CHECK_CORRUPT( heap, heap->draining, occupied );
  heap->drain_slot = (uint32_t) i;
  T_true( _RBHeap_Walk( heap ) );
  CHECK_CORRUPT( heap, heap->home, draining );
  CHECK_CORRUPT( heap, draining->live, draining->live + 1 );
  CHECK_CORRUPT( heap, heap->drain_slot, 0 );
  CHECK_CORRUPT( heap, heap->drain_slot, draining->untouched + 1 );
  CHECK_CORRUPT( heap, heap->drain_slot, draining->untouched );
  CHECK_CORRUPT( heap, heap->spare, draining->live );
  desc = Slot( draining, 0 );
  CHECK_CORRUPT( heap, desc->size, RBHEAP_ALIGNMENT );
}

static void RunCorruptWalk( Context *ctx )
{
  RBHeap_control    *heap;
  RBTree_Node       *root;
  RBHeap_descriptor *desc;
  RBTree_Node       *node;
  size_t             i;

  heap = &ctx->heap;
  Init( ctx, 0, AREA_SIZE );

  for ( i = 0; i < 24; ++i ) {
    ctx->blocks[ i ] = _RBHeap_Allocate( heap, 8 + 8 * i );
    T_quiet_not_null( ctx->blocks[ i ] );
  }

  FreeEverySecond( ctx, 24 );
  T_assert_true( _RBHeap_Walk( heap ) );

  root = _RBTree_Root( &heap->used_root );
  FlipColor( root );
  T_false( _RBHeap_Walk( heap ) );
  FlipColor( root );
  CheckCorrupt(
    heap,
    &root->rbn_parent_color,
    root->rbn_parent_color | (uintptr_t) root->rbn_left
  );

  /* The parent of a child becomes its sibling, which keeps the walk finite */
  node = root->rbn_left;
  CheckCorrupt(
    heap,
    &node->rbn_parent_color,
    ( node->rbn_parent_color & RTEMS_RB_COLOR_MASK ) |
      (uintptr_t) root->rbn_right
  );
  node = root->rbn_right;
  CheckCorrupt(
    heap,
    &node->rbn_parent_color,
    ( node->rbn_parent_color & RTEMS_RB_COLOR_MASK ) |
      (uintptr_t) root->rbn_left
  );

  node = _RBTree_Minimum( &heap->used_root );
  FlipColor( node );
  T_false( _RBHeap_Walk( heap ) );
  FlipColor( node );

  while ( node != NULL && _RBTree_Color( node ) != RTEMS_RB_RED ) {
    node = _RBTree_Successor( node );
  }

  if ( node != NULL && node->rbn_left != NULL ) {
    FlipColor( node->rbn_left );
    T_false( _RBHeap_Walk( heap ) );
    FlipColor( node->rbn_left );
  }

  if ( node != NULL && node->rbn_right != NULL ) {
    FlipColor( node->rbn_right );
    T_false( _RBHeap_Walk( heap ) );
    FlipColor( node->rbn_right );
  }

  desc = Node( _RBTree_Minimum( &heap->free_root ) );
  CheckCorrupt( heap, &desc->size, 0 );
  CheckCorrupt( heap, &desc->begin, desc->begin + 1 );
  CheckCorrupt( heap, &desc->size, desc->size + 1 );
  desc = Node( _RBTree_Maximum( &heap->free_root ) );
  CheckCorrupt( heap, &desc->size, 8 );
  CheckCorrupt( heap, &desc->begin, desc->begin + 8 );

  desc = Node( _RBTree_Maximum( &heap->used_root ) );
  CheckCorrupt( heap, &desc->size, 0 );
  CheckCorrupt( heap, &desc->size, desc->size + AREA_SIZE );
  CheckCorrupt( heap, &desc->begin, desc->begin - 8 );
  CheckCorrupt( heap, &heap->area_end, heap->area_end + 8 );
  CheckCorrupt( heap, &heap->free_size, heap->free_size + 8 );
  CheckCorruptU32( heap, &heap->stats.free_blocks );
  CheckCorruptU32( heap, &heap->stats.used_blocks );
  ++heap->stats.lifetime_freed;
  T_false( _RBHeap_Walk( heap ) );
  --heap->stats.lifetime_freed;
  T_true( _RBHeap_Walk( heap ) );
}

/* Returns the begin of the largest free block. */
static uintptr_t LargestFreeBegin( const RBHeap_control *heap )
{
  return Node( _RBTree_Maximum( &heap->free_root ) )->begin;
}

/*
 * Allocates small blocks until the next allocation needs a new slab.  Frees
 * a block between used blocks, so that a later allocation takes its place.
 * Allocates a filler block which ends at a multiple of the slab size.  Then
 * the next allocation lets the heap carve a slab at the begin of the free
 * block after the filler, and takes the place of the freed block.
 */
static size_t CarveSlabNext( Context *ctx, size_t n )
{
  RBHeap_control *heap;
  uintptr_t       begin;
  uintptr_t       filler;
  uintptr_t       slabs;
  size_t          hole;

  heap = &ctx->heap;

  while ( heap->capacity >= 2 * ( heap->entries + 1 ) + 3 ) {
    T_quiet_lt_sz( n, BLOCK_COUNT );
    ctx->blocks[ n ] = _RBHeap_Allocate( heap, RBHEAP_MIN_SIZE );
    T_quiet_not_null( ctx->blocks[ n ] );
    ++n;
  }

  hole = n - 2;
  T_quiet_true( _RBHeap_Free( heap, ctx->blocks[ hole ] ) );
  ctx->blocks[ hole ] = NULL;

  begin = LargestFreeBegin( heap );
  filler = RTEMS_ALIGN_UP( begin, RBHEAP_SLAB_SIZE ) - begin;

  if ( filler < 4 * RBHEAP_SLAB_SLOTS ) {
    filler += RBHEAP_SLAB_SIZE;
  }

  ctx->blocks[ n ] = _RBHeap_Allocate( heap, filler - RBHEAP_GUARD_SIZE );
  T_quiet_eq_uptr( (uintptr_t) ctx->blocks[ n ], begin );
  ++n;

  slabs = heap->area_slab_count;
  ctx->blocks[ hole ] = _RBHeap_Allocate( heap, RBHEAP_MIN_SIZE );
  T_quiet_not_null( ctx->blocks[ hole ] );
  T_eq_uptr( heap->area_slab_count, slabs + 1 );
  T_eq_uptr( LargestFreeBegin( heap ), begin + filler + RBHEAP_SLAB_SIZE );
  return n;
}

static void RunSlabNext( Context *ctx )
{
  RBHeap_control *heap;
  size_t          n;
  size_t          i;

  heap = &ctx->heap;
  Init( ctx, 0, AREA_SIZE );
  n = CarveSlabNext( ctx, 0 );
  n = CarveSlabNext( ctx, n );
  T_eq_uptr( heap->area_slab_count, 2 );

  for ( i = 0; i < n; ++i ) {
    T_quiet_true( _RBHeap_Free( heap, ctx->blocks[ i ] ) );
  }

  T_eq_uptr( heap->area_slab_count, 1 );
  T_true( _RBHeap_Walk( heap ) );
}

/*
 * The slab of the heap lies outside of its area, so the slab inside the area
 * is no home slab.  After all frees, this slab is the only entry of the
 * heap, and the addition of descriptors clears the heap.
 */
static void RunClearAreaSlab( Context *ctx )
{
  RBHeap_control *heap;
  size_t          n;
  size_t          i;
  bool            ok;

  heap = &ctx->heap;
  ok = _RBHeap_Initialize_with_slab(
    heap,
    ctx->area,
    AREA_SIZE,
    ctx->slabs[ 0 ],
    NULL
  );
  T_assert_true( ok );
  n = 0;

  while ( heap->area_slab_count == 0 ) {
    T_quiet_lt_sz( n, BLOCK_COUNT );
    ctx->blocks[ n ] = _RBHeap_Allocate( heap, RBHEAP_MIN_SIZE );
    T_quiet_not_null( ctx->blocks[ n ] );
    ++n;
  }

  for ( i = 0; i < n; ++i ) {
    T_quiet_true( _RBHeap_Free( heap, ctx->blocks[ i ] ) );
  }

  T_eq_uptr( heap->area_slab_count, 1 );
  T_eq_uptr( heap->entries, 1 );
  T_true( _RBHeap_Add_descriptors( heap, ctx->slabs[ 1 ], RBHEAP_SLAB_SIZE ) );
  T_eq_uptr( heap->area_slab_count, 0 );
  T_eq_uptr( heap->entries, 0 );
  T_eq_uptr( heap->free_size, heap->area_end - heap->area_begin );
  T_true( _RBHeap_Walk( heap ) );
}

#define MOVE_BLOCKS 700

static RBHeap_slab *SlabOfBlock( const RBHeap_control *heap, void *block )
{
  RBTree_Node *node;

  node = _RBTree_Minimum( &heap->used_root );

  while ( node != NULL ) {
    RBHeap_descriptor *desc;

    desc = Node( node );

    if ( desc->begin == (uintptr_t) block ) {
      return (RBHeap_slab *) ( (uintptr_t) desc &
                               ~( (uintptr_t) RBHEAP_SLAB_SIZE - 1 ) );
    }

    node = _RBTree_Successor( node );
  }

  return NULL;
}

static uint32_t SlabClass( uint32_t live )
{
  return 31 - (uint32_t) __builtin_clz( live );
}

/* Returns true if the blocks from i to i + count - 1 are adjacent and used */
static bool AdjacentUsed(
  Context  *ctx,
  size_t    i,
  size_t    count,
  uintptr_t unit
)
{
  size_t j;

  for ( j = 0; j < count; ++j ) {
    if ( ctx->blocks[ i + j ] == NULL ) {
      return false;
    }

    if (
      j > 0 && (uintptr_t) ctx->blocks[ i + j - 1 ] + unit !=
                 (uintptr_t) ctx->blocks[ i + j ]
    ) {
      return false;
    }
  }

  return true;
}

static void FreeBlock( Context *ctx, size_t i )
{
  T_quiet_true( _RBHeap_Free( &ctx->heap, ctx->blocks[ i ] ) );
  ctx->blocks[ i ] = NULL;
}

/*
 * Returns descriptors to the slab.  Of three adjacent used blocks between
 * used blocks, the heap frees the first, the third and then the middle
 * block.  The middle block merges with both, so the descriptors of the
 * middle and the third block go back to their slabs.
 */
static void ReturnTo( Context *ctx, RBHeap_slab *slab, uintptr_t unit )
{
  RBHeap_control *heap;
  size_t          i;

  heap = &ctx->heap;

  for ( i = 1; i + 4 < MOVE_BLOCKS; ++i ) {
    if (
      AdjacentUsed( ctx, i - 1, 5, unit ) &&
      SlabOfBlock( heap, ctx->blocks[ i + 1 ] ) == slab &&
      SlabOfBlock( heap, ctx->blocks[ i + 2 ] ) == slab
    ) {
      FreeBlock( ctx, i );
      FreeBlock( ctx, i + 2 );
      FreeBlock( ctx, i + 1 );
      return;
    }
  }

  T_quiet_true( false );
}

static void RunSlabMove( Context *ctx )
{
  RBHeap_control *heap;
  RBHeap_slab    *a;
  RBHeap_slab    *b;
  uintptr_t       unit;
  uintptr_t       size;
  uint32_t        full;
  uint32_t        k;
  size_t          i;

  heap = &ctx->heap;
  Init( ctx, 0, AREA_SIZE );

  for ( i = 0; i < MOVE_BLOCKS; ++i ) {
    ctx->blocks[ i ] = _RBHeap_Allocate( heap, RBHEAP_MIN_SIZE );
    T_quiet_not_null( ctx->blocks[ i ] );
  }

  T_quiet_true( _RBHeap_Size_of_block( heap, ctx->blocks[ 0 ], &size ) );
  unit = size + RBHEAP_GUARD_SIZE;

  /* Find two full slabs inside the area besides the home slab */
  full = RBHEAP_SLAB_SLOTS - 1;
  a = NULL;
  b = NULL;

  for ( i = 0; i < MOVE_BLOCKS && b == NULL; ++i ) {
    RBHeap_slab *slab;

    slab = SlabOfBlock( heap, ctx->blocks[ i ] );

    if ( slab != heap->home && slab != a && slab->live == full ) {
      if ( a == NULL ) {
        a = slab;
      } else {
        b = slab;
      }
    }
  }

  T_assert_not_null( b );
  k = SlabClass( full - 1 );
  ReturnTo( ctx, a, unit );
  T_eq_ptr( heap->occupied[ k ], a );
  ReturnTo( ctx, b, unit );
  T_eq_ptr( heap->occupied[ k ], b );
  T_eq_ptr( b->next, a );

  for ( i = 0; i < RBHEAP_SLAB_SLOTS && SlabClass( a->live ) == k; ++i ) {
    ReturnTo( ctx, a, unit );
  }

  T_eq_ptr( heap->occupied[ k ], b );
  T_eq_ptr( heap->occupied[ k - 1 ], a );
  T_true( _RBHeap_Walk( heap ) );
}

static T_fixture ScoreRbheapValRbheapEdges_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &ScoreRbheapValRbheapEdges_Instance
};

/**
 * @brief Allocate blocks with a boundary and with invalid alignments and
 *   boundaries.
 */
static void ScoreRbheapValRbheapEdges_Action_0(
  ScoreRbheapValRbheapEdges_Context *ctx
)
{
  RunBoundary( ctx );
}

/**
 * @brief Allocate blocks with a boundary from free blocks which begin between
 *   two multiples of the boundary.
 */
static void ScoreRbheapValRbheapEdges_Action_1(
  ScoreRbheapValRbheapEdges_Context *ctx
)
{
  RunBoundaryPlace( ctx );
}

/**
 * @brief Allocate a block with a boundary while the free block of the best fit
 *   and its four successors cross a multiple of the boundary and a larger free
 *   block holds the block after the move.
 */
static void ScoreRbheapValRbheapEdges_Action_2(
  ScoreRbheapValRbheapEdges_Context *ctx
)
{
  RunBoundaryPadded( ctx );
}

/**
 * @brief Allocate blocks until the heap carves a slab. Then free and allocate
 *   one block several times.
 */
static void ScoreRbheapValRbheapEdges_Action_3(
  ScoreRbheapValRbheapEdges_Context *ctx
)
{
  RunHysteresis( ctx );
}

/**
 * @brief Extend heaps without a spare descriptor by areas which hold no slab:
 *   right after the areas, right below the areas, at the top of the address
 *   space, and with a slab source.
 */
static void ScoreRbheapValRbheapEdges_Action_4(
  ScoreRbheapValRbheapEdges_Context *ctx
)
{
  RunExtendShort( ctx );
}

/**
 * @brief Let the heap carve two slabs at the begin of a free block, each right
 *   after a used block. Free the blocks until the heap releases one of the
 *   slabs.
 */
static void ScoreRbheapValRbheapEdges_Action_5(
  ScoreRbheapValRbheapEdges_Context *ctx
)
{
  RunSlabNext( ctx );
}

/**
 * @brief Let a heap with its slab outside of its area carve a slab inside the
 *   area. Free all blocks and add descriptors to the heap.
 */
static void ScoreRbheapValRbheapEdges_Action_6(
  ScoreRbheapValRbheapEdges_Context *ctx
)
{
  RunClearAreaSlab( ctx );
}

/**
 * @brief Return descriptors to two full slabs, so that both share a list of
 *   their count of descriptors in use. Return descriptors to the slab which is
 *   not the first of the list until it moves to another list.
 */
static void ScoreRbheapValRbheapEdges_Action_7(
  ScoreRbheapValRbheapEdges_Context *ctx
)
{
  RunSlabMove( ctx );
}

/**
 * @brief Allocate an aligned block while the free block of the best fit does
 *   not hold it at its alignment and a successor of equal size does.
 */
static void ScoreRbheapValRbheapEdges_Action_8(
  ScoreRbheapValRbheapEdges_Context *ctx
)
{
  RunAlignedSuccessor( ctx );
}

/**
 * @brief Allocate, resize and free with huge sizes and invalid blocks. Extend
 *   a heap by an area of a huge size.
 */
static void ScoreRbheapValRbheapEdges_Action_9(
  ScoreRbheapValRbheapEdges_Context *ctx
)
{
  RunHuge( ctx );
}

/**
 * @brief Allocate blocks from a heap whose slab source returns no slab.
 *   Initialize heaps with slabs and invalid memory areas.
 */
static void ScoreRbheapValRbheapEdges_Action_10(
  ScoreRbheapValRbheapEdges_Context *ctx
)
{
  RunSlabSource( ctx );
}

/**
 * @brief Add descriptors to heaps with valid and invalid memory areas.
 */
static void ScoreRbheapValRbheapEdges_Action_11(
  ScoreRbheapValRbheapEdges_Context *ctx
)
{
  RunAddDescriptors( ctx );
}

/**
 * @brief Extend a heap by areas which fill parts of its holes, by areas next
 *   to its areas and by invalid areas.
 */
static void ScoreRbheapValRbheapEdges_Action_12(
  ScoreRbheapValRbheapEdges_Context *ctx
)
{
  RunExtend( ctx );
}

/**
 * @brief Free blocks until the heap releases a slab and allocate blocks while
 *   the heap drains the slab.
 */
static void ScoreRbheapValRbheapEdges_Action_13(
  ScoreRbheapValRbheapEdges_Context *ctx
)
{
  RunDrain( ctx );
}

/**
 * @brief Extend a heap by many small areas with gaps between them until the
 *   heap has no spare descriptor.
 */
static void ScoreRbheapValRbheapEdges_Action_14(
  ScoreRbheapValRbheapEdges_Context *ctx
)
{
  RunExtendMany( ctx );
}

/**
 * @brief Extend and add descriptors to heaps at the ends of their areas, near
 *   the top of the address space, over used blocks and over holes.
 */
static void ScoreRbheapValRbheapEdges_Action_15(
  ScoreRbheapValRbheapEdges_Context *ctx
)
{
  RunAreaEdges( ctx );
}

/**
 * @brief Use up the spare descriptors of a heap with a slab source which
 *   returns no slab. Let the source return slabs. Resize a block, free a
 *   processor block on another processor, and free a processor block for a
 *   full bin.
 */
static void ScoreRbheapValRbheapEdges_Action_16(
  ScoreRbheapValRbheapEdges_Context *ctx
)
{
  RunSourceSpare( ctx );
}

/**
 * @brief Walk more heaps with descriptors which form invalid trees. Get the
 *   information of a heap with holes and extend heaps below their used blocks
 *   and below their added descriptors.
 */
static void ScoreRbheapValRbheapEdges_Action_17(
  ScoreRbheapValRbheapEdges_Context *ctx
)
{
  RunFakeWalkMore( ctx );
  RunHoles( ctx );
}

/**
 * @brief Walk heaps with descriptors which form invalid trees.
 */
static void ScoreRbheapValRbheapEdges_Action_18(
  ScoreRbheapValRbheapEdges_Context *ctx
)
{
  RunFakeWalk( ctx );
}

/**
 * @brief Free blocks until the heap drains a slab with descriptors in use.
 *   Extend the heap by areas with a gap until the heap has less than two spare
 *   descriptors. Grow a block into the free block after it.
 */
static void ScoreRbheapValRbheapEdges_Action_19(
  ScoreRbheapValRbheapEdges_Context *ctx
)
{
  RunExtendDrain( ctx );
}

/**
 * @brief Corrupt the descriptors and the counters of a heap one after the
 *   other and walk the heap.
 */
static void ScoreRbheapValRbheapEdges_Action_20(
  ScoreRbheapValRbheapEdges_Context *ctx
)
{
  RunCorruptWalk( ctx );
}

/**
 * @brief Corrupt the slabs of descriptors, the slab lists and the counters of
 *   the descriptors of a heap with a drain one after the other and walk the
 *   heap.
 */
static void ScoreRbheapValRbheapEdges_Action_21(
  ScoreRbheapValRbheapEdges_Context *ctx
)
{
  RunCorruptWalkSlabs( ctx );
}

/**
 * @fn void T_case_body_ScoreRbheapValRbheapEdges( void )
 */
T_TEST_CASE_FIXTURE(
  ScoreRbheapValRbheapEdges,
  &ScoreRbheapValRbheapEdges_Fixture
)
{
  ScoreRbheapValRbheapEdges_Context *ctx;

  ctx = T_fixture_context();

  ScoreRbheapValRbheapEdges_Action_0( ctx );
  ScoreRbheapValRbheapEdges_Action_1( ctx );
  ScoreRbheapValRbheapEdges_Action_2( ctx );
  ScoreRbheapValRbheapEdges_Action_3( ctx );
  ScoreRbheapValRbheapEdges_Action_4( ctx );
  ScoreRbheapValRbheapEdges_Action_5( ctx );
  ScoreRbheapValRbheapEdges_Action_6( ctx );
  ScoreRbheapValRbheapEdges_Action_7( ctx );
  ScoreRbheapValRbheapEdges_Action_8( ctx );
  ScoreRbheapValRbheapEdges_Action_9( ctx );
  ScoreRbheapValRbheapEdges_Action_10( ctx );
  ScoreRbheapValRbheapEdges_Action_11( ctx );
  ScoreRbheapValRbheapEdges_Action_12( ctx );
  ScoreRbheapValRbheapEdges_Action_13( ctx );
  ScoreRbheapValRbheapEdges_Action_14( ctx );
  ScoreRbheapValRbheapEdges_Action_15( ctx );
  ScoreRbheapValRbheapEdges_Action_16( ctx );
  ScoreRbheapValRbheapEdges_Action_17( ctx );
  ScoreRbheapValRbheapEdges_Action_18( ctx );
  ScoreRbheapValRbheapEdges_Action_19( ctx );
  ScoreRbheapValRbheapEdges_Action_20( ctx );
  ScoreRbheapValRbheapEdges_Action_21( ctx );
}

/** @} */
