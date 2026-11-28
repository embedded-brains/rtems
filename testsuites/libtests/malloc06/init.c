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

/*
 * This test measures the runtime of the processor part of malloc() and
 * free().  The used tree of processor zero holds nearly all its descriptors,
 * so that each lookup runs at the full depth of the tree.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/malloc.h>
#include <rtems/test.h>
#include <rtems/test-info.h>

#include <stdlib.h>
#include <string.h>

#define DESCRIPTORS 1024

#define MAX_SIZE 4096

#define CLASS_COUNT RTEMS_MALLOC_CPU_CLASS_COUNT( MAX_SIZE )

#define CAP ( DESCRIPTORS / 2 / CLASS_COUNT )

/*
 * The fill leaves out the two smallest classes, since the minimum block size
 * of a debug build lifts the first class into the second.
 */
#define FIRST_CLASS 2

#define BLOCK_COUNT ( ( CLASS_COUNT - FIRST_CLASS ) * CAP )

#define PAIRS ( DESCRIPTORS / 2 )

#define RESERVE_COUNT ( PAIRS - BLOCK_COUNT )

#define BIND_CLASS 9

#define MAX_PROCESSORS 32

#define SAMPLE_COUNT 100

typedef struct {
  T_measure_runtime_context *context;
  T_measure_runtime_request  request;
  rtems_id                   runner;
  rtems_id                   helper;
  void                      *blocks[ BLOCK_COUNT ];
  void                      *reserve[ RESERVE_COUNT ];
  void                      *remote[ 1 ];
  size_t                     remote_count;
  void                      *block;
} test_context;

static test_context test_instance;

/* Returns the size of a request of the class for the given index. */
static size_t class_size( size_t index )
{
  size_t units;
  size_t shift;

  if ( index < 4 ) {
    units = index + 1;
  } else {
    shift = index / 4 + 1;
    units = ( (size_t) 1 << shift ) + ( ( index % 4 + 1 ) << ( shift - 2 ) );
  }

  return units * RBHEAP_ALIGNMENT - RBHEAP_GUARD_SIZE;
}

static void helper_task( rtems_task_argument arg )
{
  test_context *ctx;

  ctx = (test_context *) arg;

  while ( true ) {
    rtems_event_set events;
    size_t          i;

    (void) rtems_event_receive(
      RTEMS_EVENT_0,
      RTEMS_EVENT_ALL | RTEMS_WAIT,
      RTEMS_NO_TIMEOUT,
      &events
    );

    for ( i = 0; i < ctx->remote_count; ++i ) {
      free( ctx->remote[ i ] );
    }

    (void) rtems_event_transient_send( ctx->runner );
  }
}

/* Frees the remote blocks on processor one. */
static void free_remote( test_context *ctx )
{
  rtems_status_code sc;

  sc = rtems_event_send( ctx->helper, RTEMS_EVENT_0 );
  T_quiet_rsc_success( sc );
  sc = rtems_event_transient_receive( RTEMS_WAIT, RTEMS_NO_TIMEOUT );
  T_quiet_rsc_success( sc );
}

static bool discard_tick( uint32_t tic, uint32_t toc, unsigned int retry )
{
  return tic == toc || retry > 10;
}

static void setup_fixture( void *arg )
{
  test_context            *ctx;
  T_measure_runtime_config config;
  rtems_status_code        sc;
  cpu_set_t                set;
  size_t                   i;

  ctx = arg;
  memset( &config, 0, sizeof( config ) );
  config.sample_count = SAMPLE_COUNT;
  ctx->request.arg = ctx;
  ctx->request.flags = T_MEASURE_RUNTIME_REPORT_SAMPLES;
  ctx->context = T_measure_runtime_create( &config );
  T_assert_not_null( ctx->context );
  ctx->runner = rtems_task_self();

  if ( rtems_scheduler_get_processor_maximum() < 2 ) {
    return;
  }

  sc = rtems_task_create(
    rtems_build_name( 'H', 'E', 'L', 'P' ),
    2,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->helper
  );
  T_assert_rsc_success( sc );
  CPU_ZERO( &set );
  CPU_SET( 1, &set );
  sc = rtems_task_set_affinity( ctx->helper, sizeof( set ), &set );
  T_assert_rsc_success( sc );
  sc = rtems_task_start( ctx->helper, helper_task, (rtems_task_argument) ctx );
  T_assert_rsc_success( sc );

  /*
   * The blocks come from the heap.  Their frees bind them in the order of the
   * blocks, so the last blocks take the highest descriptor indices.  The
   * allocations then take them back from the bins.
   */
  for ( i = 0; i < BLOCK_COUNT; ++i ) {
    ctx->blocks[ i ] = malloc( class_size( FIRST_CLASS + i / CAP ) );
    T_quiet_assert_not_null( ctx->blocks[ i ] );
  }

  for ( i = 0; i < BLOCK_COUNT; ++i ) {
    free( ctx->blocks[ i ] );
  }

  for ( i = 0; i < BLOCK_COUNT; ++i ) {
    ctx->blocks[ i ] = malloc( class_size( FIRST_CLASS + i / CAP ) );
    T_quiet_assert_not_null( ctx->blocks[ i ] );
  }

  /*
   * The reserve takes the remaining descriptors into the bins of the first
   * classes.  The spare list is then empty, and the bin of the first class is
   * full.  The last block of the reserve has the highest descriptor index.
   */
  for ( i = 0; i < RESERVE_COUNT; ++i ) {
    ctx->reserve[ i ] = malloc( class_size( FIRST_CLASS + i / CAP ) );
    T_quiet_assert_not_null( ctx->reserve[ i ] );
  }

  for ( i = 0; i < RESERVE_COUNT; ++i ) {
    free( ctx->reserve[ i ] );
  }
}

static void teardown_fixture( void *arg )
{
  test_context     *ctx;
  rtems_status_code sc;

  ctx = arg;

  if ( ctx->helper != 0 ) {
    sc = rtems_task_delete( ctx->helper );
    T_rsc_success( sc );
  }
}

static T_fixture fixture = {
  .setup = setup_fixture,
  .teardown = teardown_fixture,
  .initial_context = &test_instance
};

static void alloc_hit_body( void *arg )
{
  test_context *ctx;

  ctx = arg;
  ctx->block = malloc( class_size( 8 ) );
}

static bool alloc_hit_teardown(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  test_context *ctx;

  (void) delta;
  ctx = arg;
  free( ctx->block );
  return discard_tick( tic, toc, retry );
}

static void free_hit_setup( void *arg )
{
  test_context *ctx;

  ctx = arg;
  ctx->block = malloc( class_size( 8 ) );
}

static void free_hit_body( void *arg )
{
  test_context *ctx;

  ctx = arg;
  free( ctx->block );
}

static bool free_hit_teardown(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  (void) arg;
  (void) delta;
  return discard_tick( tic, toc, retry );
}

static void alloc_miss_body( void *arg )
{
  test_context *ctx;

  ctx = arg;
  ctx->block = malloc( class_size( 0 ) + 1 );
}

/* Returns the block to the heap, so that the bin of the class stays empty. */
static bool alloc_miss_teardown(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  test_context *ctx;

  (void) delta;
  ctx = arg;
  ctx->remote[ 0 ] = ctx->block;
  ctx->remote_count = 1;
  free_remote( ctx );
  return discard_tick( tic, toc, retry );
}

/* Allocates a block of the class from the heap, past the bins. */
static void *heap_allocate( size_t size_class )
{
  void *block;
  int   eno;

  block = NULL;
  eno = posix_memalign( &block, RBHEAP_ALIGNMENT, class_size( size_class ) );
  T_quiet_eq_int( eno, 0 );
  return block;
}

static void free_heap_setup( void *arg )
{
  test_context *ctx;

  ctx = arg;
  ctx->block = heap_allocate( FIRST_CLASS );
}

static void free_bind_setup( void *arg )
{
  test_context *ctx;

  ctx = arg;
  ctx->block = heap_allocate( BIND_CLASS );
}

/*
 * Takes the bound block out of its bin and frees it on processor one.  Its
 * descriptor then waits to be recycled.
 */
static bool free_bind_teardown(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  test_context *ctx;

  (void) delta;
  ctx = arg;
  ctx->remote[ 0 ] = malloc( class_size( BIND_CLASS ) );
  ctx->remote_count = 1;
  free_remote( ctx );
  return discard_tick( tic, toc, retry );
}

T_TEST_CASE_FIXTURE( MallocCPUPart, &fixture )
{
  test_context *ctx;

  ctx = T_fixture_context();

  if ( rtems_scheduler_get_processor_maximum() < 2 ) {
    return;
  }

  /* One block of class eight waits in its bin */
  free( ctx->blocks[ ( 8 - FIRST_CLASS ) * CAP ] );

  ctx->request.name = "MallocCPUAllocHit";
  ctx->request.setup = NULL;
  ctx->request.body = alloc_hit_body;
  ctx->request.teardown = alloc_hit_teardown;
  T_measure_runtime( ctx->context, &ctx->request );

  ctx->request.name = "MallocCPUFreeHit";
  ctx->request.setup = free_hit_setup;
  ctx->request.body = free_hit_body;
  ctx->request.teardown = free_hit_teardown;
  T_measure_runtime( ctx->context, &ctx->request );

  ctx->request.name = "MallocCPUAllocMiss";
  ctx->request.setup = NULL;
  ctx->request.body = alloc_miss_body;
  ctx->request.teardown = alloc_miss_teardown;
  T_measure_runtime( ctx->context, &ctx->request );

  ctx->request.name = "MallocCPUFreeHeap";
  ctx->request.setup = free_heap_setup;
  ctx->request.body = free_hit_body;
  ctx->request.teardown = free_hit_teardown;
  T_measure_runtime( ctx->context, &ctx->request );

  /*
   * The last block of the reserve leaves its bin and goes to the heap on
   * processor one.  Its descriptor has the highest index and waits to be
   * recycled.
   */
  ctx->remote[ 0 ] = malloc(
    class_size( FIRST_CLASS + ( RESERVE_COUNT - 1 ) / CAP )
  );
  T_eq_ptr( ctx->remote[ 0 ], ctx->reserve[ RESERVE_COUNT - 1 ] );
  ctx->remote_count = 1;
  free_remote( ctx );

  ctx->request.name = "MallocCPUFreeBindRecycle";
  ctx->request.setup = free_bind_setup;
  ctx->request.body = free_hit_body;
  ctx->request.teardown = free_bind_teardown;
  T_measure_runtime( ctx->context, &ctx->request );
}

const char rtems_test_name[] = "MALLOC 6";

static void Init( rtems_task_argument argument )
{
  rtems_test_run( argument, TEST_STATE );
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS MAX_PROCESSORS

#define CONFIGURE_MAXIMUM_TASKS ( 2 + MAX_PROCESSORS )

#define CONFIGURE_EXTRA_TASK_STACKS \
  ( ( 1 + MAX_PROCESSORS ) * RTEMS_MINIMUM_STACK_SIZE )

#define CONFIGURE_MALLOC_CPU_DESCRIPTORS DESCRIPTORS

#define CONFIGURE_MALLOC_CPU_MAX_SIZE MAX_SIZE

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
