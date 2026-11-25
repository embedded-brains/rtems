/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicSupport
 *
 * @brief This source file contains the implementation of
 *   _RTEMS_Get_heap_statistics().
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

#include <rtems/rtems/supportimpl.h>
#include <rtems/score/heapfirstfitimpl.h>

void _RTEMS_Get_heap_statistics(
  Heap_Control          *heap,
  rtems_heap_statistics *stats
)
{
  const Heap_Statistics *heap_stats;
  Heap_Information       free_info;

  _Heap_Protection_free_all_delayed_blocks( heap );
  _Heap_Get_free_information( heap, &free_info );
  heap_stats = &heap->stats;

  stats->lifetime_allocated = heap_stats->lifetime_allocated;
  stats->lifetime_freed = heap_stats->lifetime_freed;
  stats->size = heap_stats->size;
  stats->free_size = heap_stats->free_size;
  stats->largest_free_size = free_info.largest;
  stats->min_free_size = heap_stats->min_free_size;
  stats->free_blocks = heap_stats->free_blocks;
  stats->max_free_blocks = heap_stats->max_free_blocks;
  stats->used_blocks = heap_stats->used_blocks;
  stats->allocs = heap_stats->allocs;
  stats->failed_allocs = heap_stats->failed_allocs;
  stats->frees = heap_stats->frees;
  stats->resizes = heap_stats->resizes;
}
