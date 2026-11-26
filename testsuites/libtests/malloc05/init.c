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
#include <rtems/malloc.h>
#include <rtems/test.h>
#include <rtems/test-info.h>

#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#define DESCRIPTORS ( 4 * RTEMS_MALLOC_CPU_CLASS_COUNT( MAX_SIZE ) )

#define MAX_SIZE 4096

#define PROCESSOR_COUNT 4

#define CAP ( DESCRIPTORS / 2 / RTEMS_MALLOC_CPU_CLASS_COUNT( MAX_SIZE ) )

static void *blocks[ CAP + 1 ];

static void set_processor( uint32_t cpu_index )
{
#if defined( RTEMS_SMP )
  rtems_status_code sc;
  cpu_set_t         set;

  CPU_ZERO( &set );
  CPU_SET( (int) cpu_index, &set );
  sc = rtems_task_set_affinity( RTEMS_SELF, sizeof( set ), &set );
  T_rsc_success( sc );
#else
  (void) cpu_index;
#endif
}

/* Returns the count of used blocks of the heap, bound blocks included. */
static uintptr_t heap_used_blocks( void )
{
  Heap_Statistics stats;

  _RBHeap_Get_statistics( RTEMS_Malloc_Heap, &stats );
  return stats.used_blocks;
}

/* A processor binds a block which it frees and serves the class from it. */
T_TEST_CASE( MallocCPUReuse )
{
  uintptr_t address;
  void     *p;
  void     *q;
  uintptr_t used;

  set_processor( 0 );
  used = heap_used_blocks();
  p = malloc( 96 );
  T_not_null( p );
  address = (uintptr_t) p;
  T_eq_uptr( heap_used_blocks(), used + 1 );
  free( p );
  T_eq_uptr( heap_used_blocks(), used + 1 );
  q = malloc( 96 );
  T_eq_uptr( (uintptr_t) q, address );
  T_ge_sz( malloc_usable_size( q ), 96 );
  T_eq_uptr( heap_used_blocks(), used + 1 );
  free( q );
}

/* The statistics count a block in a bin as free. */
T_TEST_CASE( MallocCPUStatistics )
{
  rtems_heap_statistics before;
  rtems_heap_statistics after;
  uintptr_t             used;
  void                 *p;
  int                   rv;

  set_processor( 0 );
  used = heap_used_blocks();
  rv = malloc_get_statistics( &before );
  T_eq_int( rv, 0 );
  p = malloc( 160 );
  T_not_null( p );
  free( p );
  rv = malloc_get_statistics( &after );
  T_eq_int( rv, 0 );
  T_eq_u32( after.used_blocks, before.used_blocks );
  T_eq_uptr( after.free_size, before.free_size );
  T_eq_uptr( heap_used_blocks(), used + 1 );
}

/* A block above the maximum size returns to the heap. */
T_TEST_CASE( MallocCPULarge )
{
  void     *p;
  uintptr_t used;

  set_processor( 0 );
  used = heap_used_blocks();
  p = malloc( MAX_SIZE + 1 );
  T_not_null( p );
  free( p );
  T_eq_uptr( heap_used_blocks(), used );
}

/* An aligned request takes the heap. */
T_TEST_CASE( MallocCPUAligned )
{
  void *p;
  void *q;
  int   eno;

  set_processor( 0 );
  p = malloc( 320 );
  T_not_null( p );
  free( p );
  q = NULL;
  eno = posix_memalign( &q, 512, 320 );
  T_eq_int( eno, 0 );
  T_eq_uptr( (uintptr_t) q % 512, 0 );
  free( q );
}

/*
 * A full bin sends the block to the heap.  The allocations take the blocks
 * which earlier test cases left in the bin, so the bin is empty before the
 * frees.
 */
T_TEST_CASE( MallocCPUFullBin )
{
  uintptr_t used;
  size_t    i;

  set_processor( 0 );

  for ( i = 0; i < RTEMS_ARRAY_SIZE( blocks ); ++i ) {
    blocks[ i ] = malloc( 2048 );
    T_quiet_not_null( blocks[ i ] );
  }

  used = heap_used_blocks();

  for ( i = 0; i < RTEMS_ARRAY_SIZE( blocks ); ++i ) {
    free( blocks[ i ] );
  }

  T_eq_uptr( heap_used_blocks(), used - 1 );

  for ( i = 0; i < CAP; ++i ) {
    blocks[ i ] = malloc( 2048 );
    T_quiet_not_null( blocks[ i ] );
  }

  T_eq_uptr( heap_used_blocks(), used - 1 );

  for ( i = 0; i < CAP; ++i ) {
    free( blocks[ i ] );
  }
}

/* A block from the heap takes the bound of its size class. */
T_TEST_CASE( MallocCPURound )
{
  void     *p;
  void     *q;
  uintptr_t address;

  set_processor( 0 );
  p = malloc( 72 );
  T_not_null( p );
  address = (uintptr_t) p;
  T_eq_sz( malloc_usable_size( p ), 80 - RBHEAP_GUARD_SIZE );
  free( p );
  q = malloc( 66 );
  T_eq_uptr( (uintptr_t) q, address );
  free( q );

  p = malloc( 260 );
  T_not_null( p );
  T_eq_sz( malloc_usable_size( p ), 320 - RBHEAP_GUARD_SIZE );
  free( p );
}

/* A shrink within the class keeps a bound block in place. */
T_TEST_CASE( MallocCPURealloc )
{
  uintptr_t address;
  size_t    size;
  char     *p;
  char     *q;

  set_processor( 0 );
  p = malloc( 512 );
  T_not_null( p );
  address = (uintptr_t) p;
  free( p );
  q = malloc( 512 );
  T_eq_uptr( (uintptr_t) q, address );
  size = malloc_usable_size( q );
  T_ge_sz( size, 512 );
  memset( q, 0x5a, size );
  q = realloc( q, size - 1 );
  T_eq_uptr( (uintptr_t) q, address );
  q = realloc( q, 16 );
  T_not_null( q );
  T_ne_uptr( (uintptr_t) q, address );
  T_eq_char( q[ 15 ], 0x5a );
  q = realloc( q, 3000 );
  T_not_null( q );
  T_eq_char( q[ 0 ], 0x5a );
  T_eq_char( q[ 15 ], 0x5a );
  free( q );
}

/*
 * A free on another processor puts the block into the kept list of its owner.
 * The statistics count the kept block as free.  The kept list keeps no data in
 * the block, so a write into the freed block does not disturb the drain.  The
 * owner serves the block again after a miss of its bin.
 */
T_TEST_CASE( MallocCPURemote )
{
  uint32_t cpu;
  uint32_t count;

  count = rtems_scheduler_get_processor_maximum();

  if ( count > PROCESSOR_COUNT ) {
    count = PROCESSOR_COUNT;
  }

  for ( cpu = 0; cpu < count; ++cpu ) {
    void     *p;
    void     *q;
    void     *r;
    void                 *s;
    void                 *t;
    uintptr_t             address;
    uintptr_t             used;
    rtems_heap_statistics before;
    rtems_heap_statistics after;

    set_processor( cpu );
    used = heap_used_blocks();
    T_eq_int( malloc_get_statistics( &before ), 0 );
    p = malloc( 1024 );
    T_not_null( p );
    address = (uintptr_t) p;
    free( p );
    T_eq_uptr( heap_used_blocks(), used + 1 );
    q = malloc( 1024 );
    T_eq_uptr( (uintptr_t) q, address );

    set_processor( ( cpu + 1 ) % count );
    free( q );

    if ( count < 2 ) {
      /* The free runs on the owner, so the owner binds the block again */
      T_eq_uptr( heap_used_blocks(), used + 1 );
      continue;
    }

    T_eq_uptr( heap_used_blocks(), used + 1 );
    T_eq_int( malloc_get_statistics( &after ), 0 );
    T_eq_u32( after.used_blocks, before.used_blocks );
    T_eq_sz( after.free_size, before.free_size );

    /* Deliberately write into the freed block */
    RTEMS_OBFUSCATE_VARIABLE( address );
    memset( (void *) address, 0xa5, sizeof( void * ) );

    set_processor( cpu );
    r = malloc( 1024 );
    T_eq_uptr( (uintptr_t) r, address );
    free( r );
    T_eq_uptr( heap_used_blocks(), used + 1 );
    s = malloc( 1024 );
    T_eq_uptr( (uintptr_t) s, address );

    /* This bin miss drains the kept list once more */
    t = malloc( 1024 );
    T_not_null( t );
    free( t );
    free( s );
  }

  set_processor( 0 );
}

const char rtems_test_name[] = "MALLOC 5";

static void Init( rtems_task_argument argument )
{
  rtems_test_run( argument, TEST_STATE );
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_MAXIMUM_PROCESSORS PROCESSOR_COUNT

#define CONFIGURE_MALLOC_CPU_DESCRIPTORS DESCRIPTORS

#define CONFIGURE_MALLOC_CPU_MAX_SIZE MAX_SIZE

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
