/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicCache
 *
 * @brief This source file contains the implementation of
 *   rtems_cache_coherent_allocate() and rtems_cache_coherent_free().
 */

/*
 * Copyright (C) 2014, 2026 embedded brains GmbH & Co. KG
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

#include "cachecoherentimpl.h"

#include <rtems.h>
#include <rtems/malloc.h>
#include <rtems/score/atomic.h>
#include <rtems/score/interr.h>
#include <rtems/score/rbheap.h>
#include <rtems/sysinit.h>

static RBHeap_control cache_coherent_heap_instance;

/*
 * The reference is NULL until the first area starts the heap.  The release
 * store which sets it publishes the initialized heap control.
 */
static Atomic_Uintptr cache_coherent_heap;

static RBHeap_control *cache_coherent_get_heap( Atomic_Order order )
{
  uintptr_t heap;

  heap = _Atomic_Load_uintptr( &cache_coherent_heap, order );
  return (RBHeap_control *) heap;
}

/*
 * The descriptors of the heap lie in cached memory.  A BSP may map the area of
 * the heap as device memory, which allows no unaligned access.
 */
static char cache_coherent_slab[ RBHEAP_SLAB_SIZE ]
  RTEMS_ALIGNED( RBHEAP_SLAB_SIZE ) RTEMS_SECTION( ".noinit.rbheap.slabs" );

static void *cache_coherent_slab_source( void )
{
  RBHeap_control *heap;

  heap = RTEMS_Malloc_Heap;

  if ( heap == NULL ) {
    return NULL;
  }

  return _RBHeap_Allocate_aligned( heap, RBHEAP_SLAB_SIZE, RBHEAP_SLAB_SIZE );
}

void *rtems_cache_coherent_allocate(
  size_t    size,
  uintptr_t alignment,
  uintptr_t boundary
)
{
  void         *ptr;
  RBHeap_control *heap;


  heap = cache_coherent_get_heap( ATOMIC_ORDER_ACQUIRE );
  if ( heap == NULL ) {
    heap = RTEMS_Malloc_Heap;
  }

  ptr = _RBHeap_Allocate_aligned_with_boundary(
    heap,
    size,
    alignment,
    boundary
  );


  return ptr;
}

void rtems_cache_coherent_free( void *ptr )
{
  RBHeap_control *heap;


  heap = cache_coherent_get_heap( ATOMIC_ORDER_ACQUIRE );
  if ( heap != NULL ) {
    if ( _RBHeap_Free( heap, ptr ) ) {
      heap = NULL;
    } else {
      heap = RTEMS_Malloc_Heap;
    }
  } else {
    heap = RTEMS_Malloc_Heap;
  }

  if ( heap != NULL ) {
    (void) _RBHeap_Free( heap, ptr );
  }

}

static rtems_status_code add_area( void *area_begin, uintptr_t area_size )
{
  RBHeap_control *heap;

  /* The callers of add_area() are serialized */
  heap = cache_coherent_get_heap( ATOMIC_ORDER_RELAXED );

  if ( heap == NULL ) {
    heap = &cache_coherent_heap_instance;

    if (
      !_RBHeap_Initialize_with_slab(
        heap,
        area_begin,
        area_size,
        cache_coherent_slab,
        cache_coherent_slab_source
      )
    ) {
      return RTEMS_UNSATISFIED;
    }
    _Atomic_Store_uintptr(
      &cache_coherent_heap,
      (uintptr_t) heap,
      ATOMIC_ORDER_RELEASE
    );
  } else {
    if ( !_RBHeap_Extend( heap, area_begin, area_size ) ) {
      return RTEMS_UNSATISFIED;
    }
  }
  return RTEMS_SUCCESSFUL;
}

static void cache_coherent_initialize( void )
{
  size_t i;

  for ( i = 0; i < _Cache_coherent_Pending_count; ++i ) {
    const Cache_coherent_Area *area;

    area = &_Cache_coherent_Pending_areas[ i ];

    if ( add_area( area->begin, area->size ) != RTEMS_SUCCESSFUL ) {
      _Internal_error( INTERNAL_ERROR_NO_MEMORY_FOR_HEAP );
    }
  }

  _Cache_coherent_Add_area_handler = add_area;
}

/*
 * The BSP adds its cache-coherent areas in bsp_start().  The heap starts right
 * after it, before a device driver allocates.
 */
RTEMS_SYSINIT_ITEM(
  cache_coherent_initialize,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_LAST
);
