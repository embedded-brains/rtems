/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicCache
 *
 * @brief This source file contains the implementation of
 *   rtems_cache_coherent_allocate(), rtems_cache_coherent_free() and
 *   rtems_cache_coherent_add_area().
 */

/*
 * Copyright (c) 2014 embedded brains GmbH & Co. KG
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
#include <rtems/score/apimutex.h>
#include <rtems/score/rbheap.h>
#include <rtems/score/sysstate.h>

static RBHeap_control cache_coherent_heap_instance;

static RBHeap_control *cache_coherent_heap;

/*
 * The descriptors of the heap lie in cached memory.  A BSP may map the area of
 * the heap as device memory, which allows no unaligned access.
 */
static char cache_coherent_slab[ RBHEAP_SLAB_SIZE ]
  RTEMS_ALIGNED( RBHEAP_SLAB_SIZE );

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


  heap = cache_coherent_heap;
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


  heap = cache_coherent_heap;
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
  RBHeap_control *heap = cache_coherent_heap;

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
    cache_coherent_heap = heap;
  } else {
    if ( !_RBHeap_Extend( heap, area_begin, area_size ) ) {
      return RTEMS_UNSATISFIED;
    }
  }
  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_cache_coherent_add_area(
  void     *area_begin,
  uintptr_t area_size
)
{
  rtems_status_code sc;
  bool              needs_locking = _System_state_Is_up( _System_state_Get() );

  if ( needs_locking ) {
    _RTEMS_Lock_allocator();
  }

  sc = add_area( area_begin, area_size );

  if ( needs_locking ) {
    _RTEMS_Unlock_allocator();
  }

  return sc;
}
