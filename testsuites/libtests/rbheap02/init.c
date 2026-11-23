/* SPDX-License-Identifier: BSD-2-Clause */

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
#include <rtems/score/rbheap.h>
#include <rtems/test.h>
#include <rtems/test-info.h>

#include <string.h>

#define UNIT RBHEAP_SLAB_SIZE

#define UNIT_COUNT 16

#define SLAB_COUNT 8

#define PATTERN 0xa5

#define BLOCK_COUNT 1024

static char memory[ UNIT_COUNT * UNIT ] RTEMS_ALIGNED( UNIT );

static char slabs[ SLAB_COUNT ][ UNIT ] RTEMS_ALIGNED( UNIT );

static void *blocks[ BLOCK_COUNT ];

static RBHeap_control heap;

static size_t slab_calls;

static size_t slab_limit;

static void *at( uintptr_t unit )
{
  return &memory[ unit * UNIT ];
}

static void *slab_source( void )
{
  void *slab;

  ++slab_calls;

  if ( slab_calls >= slab_limit ) {
    return NULL;
  }

  slab = slabs[ slab_calls ];
  memset( slab, 0, UNIT );
  return slab;
}

static uintptr_t used_count( void )
{
  Heap_Information_block info;

  _RBHeap_Get_information( &heap, &info );
  return info.Used.number;
}

static uintptr_t free_count( void )
{
  Heap_Information_block info;

  _RBHeap_Get_information( &heap, &info );
  return info.Free.number;
}

static bool is_in( const void *ptr, const void *begin, const void *end )
{
  return (uintptr_t) ptr >= (uintptr_t) begin &&
         (uintptr_t) ptr < (uintptr_t) end;
}

/*
 * Allocates at most the count of blocks until the heap is exhausted and
 * returns the count of allocated blocks.
 */
static size_t allocate_blocks( size_t count, uintptr_t size )
{
  size_t i;

  for ( i = 0; i < count; ++i ) {
    blocks[ i ] = _RBHeap_Allocate( &heap, size );

    if ( blocks[ i ] == NULL ) {
      break;
    }
  }

  return i;
}

static void free_all( size_t count )
{
  size_t i;

  for ( i = 0; i < count; ++i ) {
    T_true( _RBHeap_Free( &heap, blocks[ i ] ) );
  }
}

static bool is_pattern( uintptr_t begin, uintptr_t end )
{
  const unsigned char *p;
  const unsigned char *q;

  p = at( begin );
  q = at( end );

  while ( p != q ) {
    if ( *p != PATTERN ) {
      return false;
    }

    ++p;
  }

  return true;
}

/*
 * The heap begins with the area [4, 6).  The extensions leave holes between
 * the areas and fill them partly and completely.
 */
T_TEST_CASE( RBHeapExtendHoles )
{
  uintptr_t size;
  size_t    count;
  size_t    i;

  T_true( _RBHeap_Initialize( &heap, at( 4 ), 2 * UNIT ) );
  T_true( _RBHeap_Walk( &heap ) );
  size = heap.free_size;

  /* An area above the areas leaves the hole [6, 8) */
  T_true( _RBHeap_Extend( &heap, at( 8 ), UNIT ) );
  T_true( _RBHeap_Walk( &heap ) );
  T_eq_uptr( free_count(), 2 );

  /* An area below the areas leaves the hole [2, 4) */
  T_true( _RBHeap_Extend( &heap, at( 1 ), UNIT ) );
  T_true( _RBHeap_Walk( &heap ) );

  /* An area above the areas leaves the hole [9, 12) */
  T_true( _RBHeap_Extend( &heap, at( 12 ), UNIT ) );
  T_true( _RBHeap_Walk( &heap ) );
  T_eq_uptr( free_count(), 4 );
  T_eq_uptr( used_count(), 0 );

  /* A free of a hole fails */
  T_false( _RBHeap_Free( &heap, at( 2 ) ) );
  T_false( _RBHeap_Free( &heap, at( 6 ) ) );
  T_false( _RBHeap_Free( &heap, at( 9 ) ) );

  /* An area which overlaps an area or the bound of a hole fails */
  T_false( _RBHeap_Extend( &heap, at( 5 ), UNIT ) );
  T_false( _RBHeap_Extend( &heap, at( 7 ), 2 * UNIT ) );
  T_false( _RBHeap_Extend( &heap, at( 0 ), 2 * UNIT ) );
  T_true( _RBHeap_Walk( &heap ) );

  /* The area [10, 12) leaves the hole [9, 10) and merges with the area above */
  T_true( _RBHeap_Extend( &heap, at( 10 ), 2 * UNIT ) );
  T_true( _RBHeap_Walk( &heap ) );
  T_eq_uptr( free_count(), 4 );

  /* The area [6, 7) merges with the area below and leaves the hole [7, 8) */
  T_true( _RBHeap_Extend( &heap, at( 6 ), UNIT ) );
  T_true( _RBHeap_Walk( &heap ) );
  T_eq_uptr( free_count(), 4 );

  /* The area [7, 8) fills the hole and merges with both areas */
  T_true( _RBHeap_Extend( &heap, at( 7 ), UNIT ) );
  T_true( _RBHeap_Walk( &heap ) );
  T_eq_uptr( free_count(), 3 );

  /* The area in the middle of the hole [2, 4) leaves two holes */
  T_true( _RBHeap_Extend( &heap, (char *) at( 2 ) + UNIT / 2, UNIT ) );
  T_true( _RBHeap_Walk( &heap ) );
  T_eq_uptr( free_count(), 4 );
  T_false( _RBHeap_Free( &heap, at( 2 ) ) );
  T_false( _RBHeap_Free( &heap, (char *) at( 3 ) + UNIT / 2 ) );

  /* The hole [9, 10) is exactly filled */
  T_true( _RBHeap_Extend( &heap, at( 9 ), UNIT ) );
  T_true( _RBHeap_Walk( &heap ) );
  T_eq_uptr( free_count(), 3 );
  T_eq_uptr( used_count(), 0 );
  T_eq_uptr( heap.free_size, size + 9 * UNIT );

  /* No block lies in a hole */
  count = allocate_blocks( BLOCK_COUNT, UNIT / 4 );
  T_gt_sz( count, 0 );
  T_lt_sz( count, BLOCK_COUNT );
  T_true( _RBHeap_Walk( &heap ) );

  for ( i = 0; i < count; ++i ) {
    T_false( is_in( blocks[ i ], at( 0 ), at( 1 ) ) );
    T_false( is_in( blocks[ i ], at( 2 ), (char *) at( 2 ) + UNIT / 2 ) );
    T_false( is_in( blocks[ i ], (char *) at( 3 ) + UNIT / 2, at( 4 ) ) );
    T_false( is_in( blocks[ i ], at( 13 ), at( UNIT_COUNT ) ) );
  }

  free_all( count );
  T_true( _RBHeap_Walk( &heap ) );
  T_eq_uptr( used_count(), 0 );
}

/*
 * The heap takes its descriptors from a slab and a slab source outside of its
 * areas.  It writes nothing into its free blocks.
 */
T_TEST_CASE( RBHeapOutsideDescriptors )
{
  size_t    count;
  uintptr_t size;

  memset( memory, PATTERN, sizeof( memory ) );
  slab_calls = 0;
  slab_limit = SLAB_COUNT;

  T_true( _RBHeap_Initialize_with_slab(
    &heap,
    at( 2 ),
    2 * UNIT,
    slabs[ 0 ],
    slab_source
  ) );
  T_true( _RBHeap_Walk( &heap ) );
  T_eq_uptr( heap.free_size, 2 * UNIT );

  /* The areas [6, 10), [1, 2) and in the middle of the hole [4, 6) */
  T_true( _RBHeap_Extend( &heap, at( 6 ), 4 * UNIT ) );
  T_true( _RBHeap_Extend( &heap, at( 1 ), UNIT ) );
  T_true( _RBHeap_Extend( &heap, (char *) at( 4 ) + UNIT / 2, UNIT ) );
  T_true( _RBHeap_Walk( &heap ) );
  T_eq_uptr( heap.free_size, 8 * UNIT );
  T_false( _RBHeap_Free( &heap, at( 4 ) ) );
  T_false( _RBHeap_Free( &heap, (char *) at( 5 ) + UNIT / 2 ) );
  T_true( is_pattern( 0, UNIT_COUNT ) );
  T_eq_sz( slab_calls, 0 );

  /* Small blocks need more descriptors than the first slab holds */
  count = allocate_blocks( 3 * RBHEAP_SLAB_SLOTS, RBHEAP_MIN_SIZE );
  T_eq_sz( count, 3 * RBHEAP_SLAB_SLOTS );
  T_true( _RBHeap_Walk( &heap ) );
  T_gt_sz( slab_calls, 0 );
  free_all( count );
  T_true( _RBHeap_Walk( &heap ) );
  T_eq_uptr( heap.free_size, 8 * UNIT );
  T_eq_uptr( used_count(), 0 );

  /* Without a slab from the source, the descriptors run out */
  slab_limit = 0;
  count = allocate_blocks( BLOCK_COUNT, RBHEAP_MIN_SIZE );
  T_lt_sz( count, BLOCK_COUNT );
  T_true( _RBHeap_Walk( &heap ) );
  size = heap.free_size;

  if ( heap.spare == 0 ) {
    T_gt_uptr( size, 0 );
    T_false( _RBHeap_Extend( &heap, at( 12 ), UNIT ) );
    T_true( _RBHeap_Walk( &heap ) );
    T_eq_uptr( heap.free_size, size );
  }

  free_all( count );
  T_true( _RBHeap_Walk( &heap ) );
  T_eq_uptr( heap.free_size, 8 * UNIT );
  T_eq_uptr( used_count(), 0 );
}

static uintptr_t largest_free( void )
{
  Heap_Information_block info;

  _RBHeap_Get_information( &heap, &info );
  return info.Free.largest;
}

#define DRAIN_BLOCK_COUNT 200

/*
 * The heap carves its slabs out of its area.  A load of small blocks needs
 * slabs besides the home slab.  The frees in order start a drain which needs
 * more than one release call.  An allocation which needs more capacity cancels
 * the drain and carves no new slab.  After all frees, the home slab is the
 * only slab, and the free space equals the start.
 */
T_TEST_CASE( RBHeapDrain )
{
  uintptr_t free_size;
  uintptr_t largest;
  uintptr_t capacity;
  uintptr_t used;
  size_t    count;
  size_t    i;
  size_t    j;

  T_true( _RBHeap_Initialize( &heap, at( 0 ), UNIT_COUNT * UNIT ) );
  T_eq_ptr( heap.home, at( 0 ) );
  free_size = heap.free_size;
  largest = largest_free();
  capacity = heap.capacity;
  used = used_count();
  T_eq_uptr( capacity, RBHEAP_SLAB_SLOTS - 1 );

  count = allocate_blocks( DRAIN_BLOCK_COUNT, RBHEAP_MIN_SIZE );
  T_eq_sz( count, DRAIN_BLOCK_COUNT );
  T_gt_uptr( heap.capacity, capacity );
  T_true( _RBHeap_Walk( &heap ) );

  /* Free the blocks in order until a drain needs another release call */
  for ( i = 0; i < count; ++i ) {
    T_true( _RBHeap_Free( &heap, blocks[ i ] ) );
    blocks[ i ] = NULL;

    if ( heap.draining != NULL ) {
      break;
    }
  }

  T_not_null( heap.draining );
  T_gt_u32( heap.draining->live, 0 );
  T_true( _RBHeap_Walk( &heap ) );

  /* Allocate until the reserve needs more capacity */
  capacity = heap.capacity;

  for ( j = 0; j <= i && heap.capacity == capacity; ++j ) {
    blocks[ j ] = _RBHeap_Allocate( &heap, RBHEAP_MIN_SIZE );
    T_not_null( blocks[ j ] );
  }

  T_null( heap.draining );
  T_eq_uptr( heap.capacity, capacity + RBHEAP_SLAB_SLOTS - 1 );
  T_true( _RBHeap_Walk( &heap ) );

  for ( i = 0; i < count; ++i ) {
    if ( blocks[ i ] != NULL ) {
      T_true( _RBHeap_Free( &heap, blocks[ i ] ) );
    }
  }

  T_true( _RBHeap_Walk( &heap ) );
  T_null( heap.draining );
  T_eq_uptr( heap.capacity, RBHEAP_SLAB_SLOTS - 1 );
  T_eq_uptr( heap.free_size, free_size );
  T_eq_uptr( largest_free(), largest );
  T_eq_uptr( used_count(), used );
}

const char rtems_test_name[] = "RBHEAP 2";

static void Init( rtems_task_argument argument )
{
  rtems_test_run( argument, TEST_STATE );
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
