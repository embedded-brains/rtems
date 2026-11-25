/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2011, 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (C) 2009, 2026 embedded brains GmbH & Co. KG
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

#define CONFIGURE_INIT
#include "system.h"

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>
#include <rtems/malloc.h>
#include <rtems/sysinit.h>
#include <malloc.h>

/* The test checks the deprecated malloc_info() */
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

const char rtems_test_name[] = "MALLOCTEST";

static bool is_filled( const void *area, int value, size_t size )
{
  const unsigned char *bytes;
  size_t               i;

  bytes = area;

  for ( i = 0; i < size; ++i ) {
    if ( bytes[ i ] != (unsigned char) value ) {
      return false;
    }
  }

  return true;
}

/*
 *  A simple test of realloc
 */
static void test_realloc( void )
{
  void  *p1, *p2, *p3, *p4;
  size_t i;
  int    sc;
  bool   malloc_walk_ok;

  /* Test growing reallocation */
  p1 = malloc( 1 );
  rtems_test_assert( p1 != NULL );
  memset( p1, 0xa5, 1 );
  for ( i = 2; i < 2048; i++ ) {
    p2 = realloc( p1, i );
    rtems_test_assert( p2 != NULL );
    rtems_test_assert( is_filled( p2, 0xa5, i - 1 ) );
    memset( p2, 0xa5, i );
    p1 = p2;
  }
  free( p1 );

  /* Test shrinking reallocation */
  p1 = malloc( 2048 );
  rtems_test_assert( p1 != NULL );
  memset( p1, 0x5a, 2048 );
  for ( i = 2047; i >= 1; i-- ) {
    p2 = realloc( p1, i );
    rtems_test_assert( p2 != NULL );
    rtems_test_assert( is_filled( p2, 0x5a, i ) );
    p1 = p2;
  }
  free( p1 );

  /* Test realloc that should fail "in place", i.e.,
   * fallback to free()-- malloc()
   */
  p1 = malloc( 32 );
  p2 = malloc( 32 );
  p3 = realloc( p1, 64 );
  if ( p3 == p1 || p3 == NULL )
/*
 * This was added to address the following warning.
 * warning: pointer may be used after 'realloc'
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuse-after-free"
    printf(
      "realloc - failed non-in place: realloc(%p,%d) = %p\n",
      p1,
      64,
      p3
    );
#pragma GCC diagnostic pop
  free( p3 );
  free( p2 );

  /*
   *  Yet another case
   */
  p1 = malloc( 8 );
  p2 = malloc( 8 );
  free( p1 );
  sc = posix_memalign( &p1, 16, 32 );
  if ( !sc ) {
    free( p1 );
  }

  /*
   *  Allocate with default alignment coverage
   */
  sc = rtems_memalign( &p4, 0, 8 );
  if ( !sc && p4 ) {
    free( p4 );
  }

  /*
   * Walk the C Program Heap
   */
  puts( "malloc_walk - normal path" );
  malloc_walk_ok = malloc_walk( 1234, false );
  rtems_test_assert( malloc_walk_ok );

  puts( "malloc_walk - in critical section path" );
  _Thread_Dispatch_disable();
  malloc_walk_ok = malloc_walk( 1234, false );
  rtems_test_assert( malloc_walk_ok );
  _Thread_Dispatch_enable( _Per_CPU_Get() );

  /*
   *  Realloc with a bad pointer to force a point
   */
/*
 * This was added to address the following warning.
 * warning: 'realloc' called on unallocated object
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfree-nonheap-object"
  p4 = realloc( test_realloc, 32 );
#pragma GCC diagnostic pop

  p4 = _realloc_r( NULL, NULL, 1 );
}

static void free_all_delayed_blocks( void )
{
  rtems_resource_snapshot unused;

  rtems_resource_snapshot_take( &unused );
}

static void do_free( void *p )
{
  free( p );
  free_all_delayed_blocks();
}

static void test_heap_info( void )
{
  size_t                 s1, s2;
  void                  *p1;
  int                    sc;
  Heap_Information_block the_info;

  free_all_delayed_blocks();

  s1 = malloc_free_space();
  p1 = malloc( 512 );
  s2 = malloc_free_space();
  puts( "malloc_free_space - check malloc space drops after malloc" );
  rtems_test_assert( s1 );
  rtems_test_assert( s2 );
  rtems_test_assert( s2 <= s1 );
  do_free( p1 );

  puts( "malloc_free_space - verify free space returns to previous value" );
  s2 = malloc_free_space();
  rtems_test_assert( s1 == s2 );

  puts( "malloc_info - called with NULL\n" );
  sc = malloc_info( NULL );
  rtems_test_assert( sc == -1 );

  puts( "malloc_info - check free space drops after malloc" );
  sc = malloc_info( &the_info );
  rtems_test_assert( sc == 0 );
  s1 = the_info.Free.largest;

  p1 = malloc( 512 );

  sc = malloc_info( &the_info );
  rtems_test_assert( sc == 0 );
  s2 = the_info.Free.largest;

  rtems_test_assert( s1 );
  rtems_test_assert( s2 );
  rtems_test_assert( s2 <= s1 );
  do_free( p1 );

  puts( "malloc_info - verify free space returns to previous value" );
  sc = malloc_info( &the_info );
  rtems_test_assert( sc == 0 );
  rtems_test_assert( s1 == the_info.Free.largest );
}

static void test_malloc_get_statistics( void )
{
  rtems_heap_statistics before;
  rtems_heap_statistics during;
  rtems_heap_statistics after;
  void                 *p;
  int                   rv;

  puts( "malloc_get_statistics - called with NULL" );
  rv = malloc_get_statistics( NULL );
  rtems_test_assert( rv == EINVAL );

  puts( "malloc_get_statistics - called with thread dispatching disabled" );
  memset( &after, 0xff, sizeof( after ) );
  _Thread_Dispatch_disable();
  rv = malloc_get_statistics( &after );
  _Thread_Dispatch_enable( _Per_CPU_Get() );
  rtems_test_assert( rv == EPROTO );
  rtems_test_assert( after.size == 0 );
  rtems_test_assert( after.free_size == 0 );

  puts( "malloc_get_statistics - check the counters of malloc() and free()" );
  rv = malloc_get_statistics( &before );
  rtems_test_assert( rv == 0 );
  rtems_test_assert( before.size > 0 );
  rtems_test_assert( before.free_size <= before.size );
  rtems_test_assert( before.largest_free_size > 0 );
  rtems_test_assert( before.largest_free_size <= before.free_size );
  rtems_test_assert( before.min_free_size <= before.free_size );
  rtems_test_assert( before.free_blocks <= before.max_free_blocks );

  p = malloc( 512 );
  rtems_test_assert( p != NULL );

  rv = malloc_get_statistics( &during );
  rtems_test_assert( rv == 0 );
  rtems_test_assert( during.size == before.size );
  rtems_test_assert( during.free_size + 512 <= before.free_size );
  rtems_test_assert( during.min_free_size <= during.free_size );
  rtems_test_assert( during.used_blocks == before.used_blocks + 1 );
  rtems_test_assert( during.allocs == before.allocs + 1 );
  rtems_test_assert( during.failed_allocs == before.failed_allocs );
  rtems_test_assert( during.frees == before.frees );
  rtems_test_assert(
    during.lifetime_allocated - before.lifetime_allocated ==
    before.free_size - during.free_size
  );
  rtems_test_assert( during.lifetime_freed == before.lifetime_freed );

  free( p );

  rv = malloc_get_statistics( &after );
  rtems_test_assert( rv == 0 );
  rtems_test_assert( after.size == before.size );
  rtems_test_assert( after.free_size == before.free_size );
  rtems_test_assert( after.largest_free_size == before.largest_free_size );
  rtems_test_assert( after.free_blocks == before.free_blocks );
  rtems_test_assert( after.used_blocks == before.used_blocks );
  rtems_test_assert( after.allocs == before.allocs + 1 );
  rtems_test_assert( after.frees == before.frees + 1 );
  rtems_test_assert(
    after.lifetime_freed - before.lifetime_freed ==
    during.lifetime_allocated - before.lifetime_allocated
  );

  puts( "malloc_get_statistics - check the counter of failed allocations" );
  p = malloc( before.size );
  rtems_test_assert( p == NULL );

  rv = malloc_get_statistics( &after );
  rtems_test_assert( rv == 0 );
  rtems_test_assert( after.failed_allocs == before.failed_allocs + 1 );
}

static void test_rtems_heap_greedy_allocate( void )
{
  uintptr_t size;
  uintptr_t block_size;
  void     *opaque;
  void     *p;

  puts( "rtems_heap_greedy_allocate_all_except_largest - OK" );
  opaque = rtems_heap_greedy_allocate_all_except_largest( &size );
  rtems_test_assert( size > 0 );

  p = rtems_malloc( size + 1 );
  rtems_test_assert( p == NULL );

  p = rtems_malloc( size );
  rtems_test_assert( p != NULL );
  rtems_test_assert( rtems_malloc( 1 ) == NULL );
  free( p );

  rtems_heap_greedy_free( opaque );

  puts( "rtems_heap_greedy_allocate - OK" );
  block_size = 42;
  opaque = rtems_heap_greedy_allocate( &block_size, 1 );

  p = rtems_malloc( block_size );
  rtems_test_assert( p != NULL );
  rtems_test_assert( rtems_malloc( 1 ) == NULL );
  free( p );

  rtems_heap_greedy_free( opaque );
}

static void test_rtems_heap_allocate_aligned_with_boundary( void )
{
  void *p = NULL;

  p = rtems_heap_allocate_aligned_with_boundary( 1, 1, 1 );
  rtems_test_assert( p != NULL );
  free( p );

  _Thread_Dispatch_disable();
  p = rtems_heap_allocate_aligned_with_boundary( 1, 1, 1 );
  _Thread_Dispatch_enable( _Per_CPU_Get() );
  rtems_test_assert( p == NULL );
}

static void test_rtems_malloc( void )
{
  void *p;

  p = rtems_malloc( 0 );
  rtems_test_assert( p == NULL );

  errno = 0;
  p = rtems_malloc( SIZE_MAX / 2 );
  rtems_test_assert( p == NULL );
  rtems_test_assert( errno == 0 );

  p = rtems_malloc( 1 );
  rtems_test_assert( p != NULL );

  RTEMS_OBFUSCATE_VARIABLE( p );
  free( p );
}

static void test_rtems_calloc( void )
{
  void *p;
  int  *i;

  p = rtems_calloc( 0, 0 );
  rtems_test_assert( p == NULL );

  p = rtems_calloc( 0, 1 );
  rtems_test_assert( p == NULL );

  p = rtems_calloc( 1, 0 );
  rtems_test_assert( p == NULL );

  errno = 0;
  p = rtems_calloc( 1, SIZE_MAX / 2 );
  rtems_test_assert( p == NULL );
  rtems_test_assert( errno == 0 );

  errno = 0;
  p = rtems_calloc( SIZE_MAX / 2, 1 );
  rtems_test_assert( p == NULL );
  rtems_test_assert( errno == 0 );

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Walloc-size-larger-than=N"
  errno = 0;
  p = rtems_calloc( SIZE_MAX, SIZE_MAX );
  rtems_test_assert( p == NULL );
  rtems_test_assert( errno == 0 );
#pragma GCC diagnostic pop

  i = rtems_calloc( 1, sizeof( *i ) );
  rtems_test_assert( i != NULL );
  rtems_test_assert( *i == 0 );

  RTEMS_OBFUSCATE_VARIABLE( p );
  free( i );
}

/*
 *  A simple test of posix_memalign
 */
static void test_posix_memalign( void )
{
  void  *p1;
  void **p2;
  size_t i;
  int    sc;
  size_t maximumShift;

  /*
   * posix_memalign() is declared as never having a NULL first parameter.
   * We need to hide the NULL value from the compiler.
   */
  puts( "posix_memalign - NULL return pointer -- EINVAL" );
  p2 = NULL;
  RTEMS_OBFUSCATE_VARIABLE( p2 );
  sc = posix_memalign( p2, 32, 8 );
  fatal_posix_service_status( sc, EINVAL, "posix_memalign NULL pointer" );

  puts( "posix_memalign - alignment of 0 -- EINVAL" );
  sc = posix_memalign( &p1, 0, 8 );
  fatal_posix_service_status( sc, EINVAL, "posix_memalign alignment of 0" );

  puts( "posix_memalign - alignment  of 2-- EINVAL" );
  sc = posix_memalign( &p1, 2, 8 );
  fatal_posix_service_status( sc, EINVAL, "posix_memalign alignment of 2" );

  maximumShift = ( sizeof( size_t ) * CHAR_BIT ) - 1;
  for ( i = sizeof( void * ); i < maximumShift; i++ ) {
    size_t alignment = 1;

    alignment <<= i;

    p1 = NULL; /* Initialize p1 to aovid used uninitialized */

    printf( "posix_memalign - alignment of %zu -- OK\n", alignment );
    sc = posix_memalign( &p1, alignment, 8 );
    if ( sc == ENOMEM ) {
      printf( "posix_memalign - ran out of memory trying %zu\n", alignment );
      break;
    }
    posix_service_failed( sc, "posix_memalign alignment OK" );

    free( p1 );
  }
  for ( ; i < maximumShift; i++ ) {
    size_t alignment = 1 << i;
    printf( "posix_memalign - alignment of %zd -- SKIPPED\n", alignment );
  }
}

static void test_alloc_zero_size( void )
{
  size_t size;
  void  *p;
  int    eno;

  size = 0;
  errno = -1;

  RTEMS_OBFUSCATE_VARIABLE( size );
  p = malloc( size );
  rtems_test_assert( p == NULL );
  rtems_test_assert( errno == -1 );

  RTEMS_OBFUSCATE_VARIABLE( size );
  p = calloc( size, 1 );
  rtems_test_assert( p == NULL );
  rtems_test_assert( errno == -1 );

  RTEMS_OBFUSCATE_VARIABLE( size );
  p = rtems_malloc( size );
  rtems_test_assert( p == NULL );
  rtems_test_assert( errno == -1 );

  RTEMS_OBFUSCATE_VARIABLE( size );
  p = rtems_calloc( 1, size );
  rtems_test_assert( p == NULL );
  rtems_test_assert( errno == -1 );

  RTEMS_OBFUSCATE_VARIABLE( size );
  p = (void *) (uintptr_t) 1;
  eno = posix_memalign( &p, 32, size );
  rtems_test_assert( eno == 0 );
  rtems_test_assert( p == NULL );
  rtems_test_assert( errno == -1 );

  RTEMS_OBFUSCATE_VARIABLE( size );
  p = (void *) (uintptr_t) 1;
  eno = rtems_memalign( &p, 32, size );
  rtems_test_assert( eno == 0 );
  rtems_test_assert( p == NULL );
  rtems_test_assert( errno == -1 );

  RTEMS_OBFUSCATE_VARIABLE( size );
  p = aligned_alloc( 32, size );
  rtems_test_assert( p == NULL );
  rtems_test_assert( errno == -1 );

  RTEMS_OBFUSCATE_VARIABLE( size );
  p = realloc( NULL, size );
  rtems_test_assert( p == NULL );
  rtems_test_assert( errno == -1 );

  RTEMS_OBFUSCATE_VARIABLE( size );
  p = reallocarray( NULL, 1, size );
  rtems_test_assert( p == NULL );
  rtems_test_assert( errno == -1 );
}

static void test_malloc_usable_size( void )
{
  void  *p1, *p2;
  size_t sz1, sz2;

  p1 = malloc( 120 );
  rtems_test_assert( p1 != NULL );
  rtems_test_assert( errno == -1 );

  sz1 = malloc_usable_size( p1 );
  rtems_test_assert( sz1 != 0 );
  rtems_test_assert( sz1 >= 120 );

  p2 = malloc( 500 );
  rtems_test_assert( p2 != NULL );
  rtems_test_assert( errno == -1 );

  sz2 = malloc_usable_size( p2 );
  rtems_test_assert( sz2 != 0 );
  rtems_test_assert( sz2 >= 500 );

  p1 = realloc( p1, 500 );
  rtems_test_assert( p1 != NULL );
  rtems_test_assert( errno == -1 );

  sz1 = malloc_usable_size( p1 );
  rtems_test_assert( sz1 != 0 );
  rtems_test_assert( sz1 >= 500 );

  free( p1 );
  free( p2 );
}

rtems_task Init( rtems_task_argument argument )
{
  (void) argument;

  void             *p1;
  rtems_time_of_day time;
  rtems_status_code status;

  TEST_BEGIN();

  build_time( &time, 12, 31, 1988, 9, 0, 0, 0 );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );

  /*
   * Verify case where block is too large to calloc.
   */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Walloc-size-larger-than=N"
  p1 = calloc( 1, SIZE_MAX );
  rtems_test_assert( p1 == NULL );

  p1 = calloc( SIZE_MAX, SIZE_MAX );
  rtems_test_assert( p1 == NULL );
#pragma GCC diagnostic pop

  test_realloc();
  test_heap_info();
  test_malloc_get_statistics();
  test_rtems_heap_greedy_allocate();
  test_rtems_heap_allocate_aligned_with_boundary();
  test_rtems_malloc();
  test_rtems_calloc();
  test_alloc_zero_size();
  test_malloc_usable_size();

  test_posix_memalign();

  Task_name[ 1 ] = rtems_build_name( 'T', 'A', '1', ' ' );
  Task_name[ 2 ] = rtems_build_name( 'T', 'A', '2', ' ' );
  Task_name[ 3 ] = rtems_build_name( 'T', 'A', '3', ' ' );
  Task_name[ 4 ] = rtems_build_name( 'T', 'A', '4', ' ' );
  Task_name[ 5 ] = rtems_build_name( 'T', 'A', '5', ' ' );

  status = rtems_task_create(
    Task_name[ 1 ],
    1,
    TASK_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES | RTEMS_FLOATING_POINT,
    &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create of TA1" );

  status = rtems_task_create(
    Task_name[ 2 ],
    1,
    TASK_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES | RTEMS_FLOATING_POINT,
    &Task_id[ 2 ]
  );
  directive_failed( status, "rtems_task_create of TA2" );

  status = rtems_task_create(
    Task_name[ 3 ],
    1,
    TASK_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES | RTEMS_FLOATING_POINT,
    &Task_id[ 3 ]
  );
  directive_failed( status, "rtems_task_create of TA3" );

  status = rtems_task_create(
    Task_name[ 4 ],
    1,
    TASK_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES | RTEMS_FLOATING_POINT,
    &Task_id[ 4 ]
  );
  directive_failed( status, "rtems_task_create of TA4" );

  status = rtems_task_create(
    Task_name[ 5 ],
    1,
    TASK_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES | RTEMS_FLOATING_POINT,
    &Task_id[ 5 ]
  );
  directive_failed( status, "rtems_task_create of TA5" );

  status = rtems_task_start( Task_id[ 1 ], Task_1_through_5, 0 );
  directive_failed( status, "rtems_task_start of TA1" );

  status = rtems_task_start( Task_id[ 2 ], Task_1_through_5, 0 );
  directive_failed( status, "rtems_task_start of TA2" );

  status = rtems_task_start( Task_id[ 3 ], Task_1_through_5, 0 );
  directive_failed( status, "rtems_task_start of TA3" );

  status = rtems_task_start( Task_id[ 4 ], Task_1_through_5, 0 );
  directive_failed( status, "rtems_task_start of TA4" );

  status = rtems_task_start( Task_id[ 5 ], Task_1_through_5, 0 );
  directive_failed( status, "rtems_task_start of TA5" );

  rtems_task_exit();
}

static void test_early_malloc( void )
{
  void *p;
  char *q;
  void *r;
  void *s;
  void *t;
  void *u;

  p = malloc( 1 );
  rtems_test_assert( p != NULL );

  free( p );

  q = calloc( 1, 1 );
  rtems_test_assert( q != NULL );
  rtems_test_assert( q[ 0 ] == 0 );

  free( q );

  r = realloc( NULL, 128 );
  rtems_test_assert( r != NULL );
  memset( r, 0xa5, 128 );

  s = malloc( 1 );
  rtems_test_assert( s != NULL );

  free( s );

  t = realloc( r, 256 );
  rtems_test_assert( t != NULL );
  rtems_test_assert( is_filled( t, 0xa5, 128 ) );

  u = realloc( t, 384 );
  rtems_test_assert( u != NULL );
  rtems_test_assert( is_filled( u, 0xa5, 128 ) );

  free( u );
}

RTEMS_SYSINIT_ITEM(
  test_early_malloc,
  RTEMS_SYSINIT_INITIAL_EXTENSIONS,
  RTEMS_SYSINIT_ORDER_FIRST
);
