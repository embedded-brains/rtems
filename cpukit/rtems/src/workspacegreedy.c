/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This source file contains the implementation of
 *   rtems_workspace_greedy_allocate(),
 *   rtems_workspace_greedy_allocate_all_except_largest(), and
 *   rtems_workspace_greedy_free().
 */

/*
 * Copyright (C) 2012, 2026 embedded brains GmbH & Co. KG
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

#include <rtems/rtems/support.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/greedyimpl.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/wkspacedata.h>
#include <rtems/malloc.h>

#define SBRK_ALLOC_SIZE ( 128 * 1024UL * 1024UL )

static const Greedy_Operations _Workspace_Greedy_operations = {
  .allocate = _Workspace_Allocate,
  .free = _Workspace_Free
};

/*
 * With the fallback, _Workspace_Allocate() allocates from the C Program Heap
 * if the storage area is exhausted, so the greedy allocation empties both
 * heaps.
 */
static uintptr_t _Workspace_Greedy_prepare( void )
{
  Heap_Control *const  *fallback_heap;
  rtems_heap_statistics stats;
  uintptr_t             size_bound;

  fallback_heap = _Workspace_Fallback_heap;

  if ( fallback_heap != NULL ) {
    rtems_heap_sbrk_greedy_allocate( *fallback_heap, SBRK_ALLOC_SIZE );
  }

  _RTEMS_Lock_allocator();
  (void) rtems_workspace_get_statistics( &stats );
  size_bound = stats.largest_free_size;

  if ( fallback_heap != NULL ) {
    (void) malloc_get_statistics( &stats );

    if ( stats.largest_free_size > size_bound ) {
      size_bound = stats.largest_free_size;
    }
  }

  return size_bound;
}

void *rtems_workspace_greedy_allocate(
  const uintptr_t *block_sizes,
  size_t           block_count
)
{
  uintptr_t size_bound;
  void     *opaque;

  size_bound = _Workspace_Greedy_prepare();
  opaque = _Greedy_Allocate(
    &_Workspace_Greedy_operations,
    size_bound,
    block_sizes,
    block_count
  );
  _RTEMS_Unlock_allocator();

  return opaque;
}

void *rtems_workspace_greedy_allocate_all_except_largest(
  uintptr_t *allocatable_size
)
{
  uintptr_t size_bound;
  void     *opaque;

  size_bound = _Workspace_Greedy_prepare();
  opaque = _Greedy_Allocate_all_except_largest(
    &_Workspace_Greedy_operations,
    size_bound,
    allocatable_size
  );
  _RTEMS_Unlock_allocator();

  return opaque;
}

void rtems_workspace_greedy_free( void *opaque )
{
  _RTEMS_Lock_allocator();
  _Greedy_Free( &_Workspace_Greedy_operations, opaque );
  _RTEMS_Unlock_allocator();
}
