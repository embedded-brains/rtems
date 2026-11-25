/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This source file contains the implementation of the functions to
 *   test the heaps.
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

#include "tx-heap.h"

#include <rtems/malloc.h>
#include <rtems/score/heapinfo.h>

#include <rtems/test.h>

static char heap_area[ HEAP_AREA_SIZE ] RTEMS_ALIGNED( RBHEAP_SLAB_SIZE );

void HeapInstall( HeapContext *ctx )
{
  HeapReset( ctx );
  ctx->heap_before = RTEMS_Malloc_Heap;
  RTEMS_Malloc_Heap = &ctx->heap;
}

void HeapReset( HeapContext *ctx )
{
  bool ok;

  ok = _RBHeap_Initialize( &ctx->heap, heap_area, sizeof( heap_area ) );
  T_quiet_true( ok );
}

void HeapRestore( const HeapContext *ctx )
{
  RTEMS_Malloc_Heap = ctx->heap_before;
}

void *HeapGetArea( void )
{
  return heap_area;
}

uintptr_t HeapGetLargestFreeSize( const RBHeap_control *heap )
{
  Heap_Information_block info;

  _RBHeap_Get_information( heap, &info );
  return info.Free.largest;
}

uintptr_t HeapGetLargestAllocatableSize( const RBHeap_control *heap )
{
  uintptr_t size;

  size = HeapGetLargestFreeSize( heap );

  if ( size > RBHEAP_GUARD_SIZE ) {
    return size - RBHEAP_GUARD_SIZE;
  }

  return 0;
}

void HeapExhaust( RBHeap_control *heap )
{
  uintptr_t size;

  while ( ( size = HeapGetLargestAllocatableSize( heap ) ) > 0 ) {
    if ( _RBHeap_Allocate( heap, size ) == NULL ) {
      break;
    }
  }

  while ( _RBHeap_Allocate( heap, 1 ) != NULL ) {
    /* Allocate the remaining memory */
  }
}

void HeapTakeSnapshot( const RBHeap_control *heap, HeapSnapshot *snapshot )
{
  Heap_Statistics stats;

  _RBHeap_Get_statistics( heap, &stats );
  snapshot->used_blocks = stats.used_blocks;
  snapshot->free_blocks = stats.free_blocks;
  snapshot->free_size = stats.free_size;
}

void HeapCheckSnapshot(
  const RBHeap_control *heap,
  const HeapSnapshot   *snapshot
)
{
  HeapSnapshot now;

  HeapTakeSnapshot( heap, &now );
  T_eq_u32( now.used_blocks, snapshot->used_blocks );
  T_eq_u32( now.free_blocks, snapshot->free_blocks );
  T_eq_uptr( now.free_size, snapshot->free_size );
}
