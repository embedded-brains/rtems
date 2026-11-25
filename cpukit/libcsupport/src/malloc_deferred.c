/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of
 *   _Malloc_System_state(), rtems_heap_allocate_aligned_with_boundary() and
 *   rtems_malloc().
 */

/*
 *  Process free requests deferred because they were from ISR
 *  or other critical section.
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
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

#ifdef RTEMS_NEWLIB
#include <stdlib.h>
#include <string.h>

#include "malloc_p.h"

#include <rtems/score/sysstate.h>
#include <rtems/score/threaddispatch.h>

Malloc_System_state _Malloc_System_state( void )
{
  System_state_Codes state = _System_state_Get();

  if ( _System_state_Is_up( state ) ) {
    if ( _Thread_Dispatch_is_enabled() ) {
      return MALLOC_SYSTEM_STATE_NORMAL;
    } else {
      return MALLOC_SYSTEM_STATE_NO_ALLOCATION;
    }
  } else if ( _System_state_Is_before_multitasking( state ) ) {
    return MALLOC_SYSTEM_STATE_NORMAL;
  } else {
    return MALLOC_SYSTEM_STATE_NO_PROTECTION;
  }
}

static void *_Malloc_Allocate(
  size_t    size,
  uintptr_t alignment,
  uintptr_t boundary,
  bool      bound
)
{
  RBHeap_control *heap = RTEMS_Malloc_Heap;
  void           *p;

  switch ( _Malloc_System_state() ) {
    case MALLOC_SYSTEM_STATE_NORMAL:
    case MALLOC_SYSTEM_STATE_NO_PROTECTION: {
      size_t heap_size;

      heap_size = size;
      p = NULL;

#if defined( RTEMS_SMP )
      if ( bound && alignment == 0 && boundary == 0 ) {
        p = _RBHeap_Allocate_from_processor( heap, size );
        heap_size = _RBHeap_Round_for_processor( heap, size );
      }
#else
      (void) bound;
#endif

      if ( p == NULL ) {
        p = _RBHeap_Allocate_aligned_with_boundary(
          heap,
          heap_size,
          alignment,
          boundary
        );
      }

      break;
    }
    default:
      /*
       *  Do not attempt to allocate memory if not in correct system state.
       */
      return NULL;
  }

  if ( p == NULL && alignment == 0 && boundary == 0 ) {
    p = ( *rtems_malloc_extend_handler )( heap, size );
  }

  /*
   *  If the user wants us to dirty the allocated memory, then do it.
   */
  if ( p != NULL && rtems_malloc_dirty_helper != NULL ) {
    ( *rtems_malloc_dirty_helper )( p, size );
  }

  return p;
}

void *rtems_heap_allocate_aligned_with_boundary(
  size_t    size,
  uintptr_t alignment,
  uintptr_t boundary
)
{
  return _Malloc_Allocate( size, alignment, boundary, true );
}

void *_Malloc_Allocate_unbound( size_t size )
{
  if ( size == 0 ) {
    return NULL;
  }

  return _Malloc_Allocate( size, 0, 0, false );
}

void *rtems_malloc( size_t size )
{
  if ( size == 0 ) {
    return NULL;
  }

  return rtems_heap_allocate_aligned_with_boundary( size, 0, 0 );
}
#endif
