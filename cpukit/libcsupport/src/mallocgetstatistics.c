/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup MallocSupport
 *
 * @brief This source file contains the implementation of
 *   malloc_get_statistics().
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

#include <rtems/malloc.h>
#include <rtems/rtems/supportimpl.h>

#include <errno.h>
#include <string.h>

#include "malloc_p.h"

int malloc_get_statistics( rtems_heap_statistics *stats )
{
  Heap_Information_block info;

  if ( stats == NULL ) {
    return EINVAL;
  }

  if ( _Malloc_System_state() == MALLOC_SYSTEM_STATE_NO_ALLOCATION ) {
    memset( stats, 0, sizeof( *stats ) );
    return EPROTO;
  }

  memset( &info, 0, sizeof( info ) );
  _RBHeap_Get_statistics( RTEMS_Malloc_Heap, &info.Stats );
  info.Used.number = info.Stats.used_blocks;
  info.Free.number = info.Stats.free_blocks;
  info.Free.total = info.Stats.free_size;

  /*
   * The blocks in the processor bins are free for the caller.  The heap frees
   * the slabs which their entries keep, when the bins return them.
   */
#if defined( RTEMS_SMP )
  if ( _RBHeap_Has_processor_blocks( RTEMS_Malloc_Heap ) ) {
    uintptr_t bin_blocks;

    _RBHeap_Get_processor_information( RTEMS_Malloc_Heap, &info );
    bin_blocks = info.Free.number - info.Stats.free_blocks;
    info.Free.total += _RBHeap_Get_releasable_size(
      RTEMS_Malloc_Heap,
      bin_blocks
    );
  }
#endif

  info.Stats.free_size = info.Free.total;
  info.Stats.free_blocks = (uint32_t) info.Free.number;
  info.Stats.used_blocks = (uint32_t) info.Used.number;
  _RTEMS_Convert_heap_statistics( &info.Stats, stats );
  return 0;
}
