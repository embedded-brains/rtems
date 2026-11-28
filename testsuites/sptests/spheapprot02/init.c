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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <bsp.h>

#include <rtems/test-info.h>
#include <rtems/score/rbheap.h>

#include <tmacros.h>

const char rtems_test_name[] = "SPHEAPPROT 2";

static char area[ 2 * RBHEAP_SLAB_SIZE ] RTEMS_ALIGNED( RBHEAP_SLAB_SIZE );

static void test_heap_initialize( RBHeap_control *heap )
{
  bool ok;

  ok = _RBHeap_Initialize( heap, area, sizeof( area ) );
  rtems_test_assert( ok );
}

static void test_invalid_free( void )
{
  RBHeap_control heap;
  char          *p;
  bool           ok;

  test_heap_initialize( &heap );

  p = _RBHeap_Allocate( &heap, 4 * RBHEAP_ALIGNMENT );
  rtems_test_assert( p != NULL );

  /* Interior free */
  ok = _RBHeap_Free( &heap, p + RBHEAP_ALIGNMENT );
  rtems_test_assert( !ok );

  /* Outside free */
  ok = _RBHeap_Free( &heap, &heap );
  rtems_test_assert( !ok );

  ok = _RBHeap_Free( &heap, p );
  rtems_test_assert( ok );

  /* Double free */
  ok = _RBHeap_Free( &heap, p );
  rtems_test_assert( !ok );

  rtems_test_assert( heap.stats.used_blocks == 0 );
  rtems_test_assert( _RBHeap_Walk( &heap ) );
}

#ifdef RBHEAP_GUARD
static void test_guard( void )
{
  RBHeap_control       heap;
  char                *p;
  uintptr_t           *guard;
  uintptr_t            guard_value;
  uintptr_t            size;
  uintptr_t            old_size;
  RBHeap_resize_status status;
  bool                 ok;

  test_heap_initialize( &heap );

  p = _RBHeap_Allocate( &heap, 1 );
  rtems_test_assert( p != NULL );

  ok = _RBHeap_Size_of_block( &heap, p, &size );
  rtems_test_assert( ok );
  rtems_test_assert( size >= 1 );

  /* The guard word follows the size of the block */
  guard = (uintptr_t *) ( p + size );
  guard_value = *guard;
  *guard = 0;

  ok = _RBHeap_Free( &heap, p );
  rtems_test_assert( !ok );

  status = _RBHeap_Resize( &heap, p, size, &old_size );
  rtems_test_assert( status == RBHEAP_RESIZE_INVALID_BLOCK );

  rtems_test_assert( heap.stats.used_blocks == 1 );

  *guard = guard_value;

  ok = _RBHeap_Free( &heap, p );
  rtems_test_assert( ok );

  rtems_test_assert( heap.stats.used_blocks == 0 );
  rtems_test_assert( _RBHeap_Walk( &heap ) );
}
#else
#define test_guard() ( (void) 0 )
#endif

static rtems_task Init( rtems_task_argument argument )
{
  (void) argument;

  TEST_BEGIN();

  test_invalid_free();
  test_guard();

  TEST_END();

  exit( 0 );
}

#define CONFIGURE_INIT

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>
