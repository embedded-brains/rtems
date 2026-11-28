/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicCache
 *
 * @brief This source file contains the implementation of
 *   rtems_cache_coherent_add_area().
 */

/*
 * Copyright (C) 2014, 2026 embedded brains GmbH & Co. KG
 *
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
#include <rtems/score/apimutex.h>
#include <rtems/score/sysstate.h>

Cache_coherent_Area
  _Cache_coherent_Pending_areas[ CACHE_COHERENT_PENDING_AREAS ];

size_t _Cache_coherent_Pending_count;

rtems_status_code ( *_Cache_coherent_Add_area_handler )(
  void     *begin,
  uintptr_t size
);

rtems_status_code rtems_cache_coherent_add_area(
  void     *area_begin,
  uintptr_t area_size
)
{
  rtems_status_code sc;
  bool              needs_locking;

  needs_locking = _System_state_Is_up( _System_state_Get() );

  if ( needs_locking ) {
    _RTEMS_Lock_allocator();
  }

  if ( _Cache_coherent_Add_area_handler != NULL ) {
    sc = ( *_Cache_coherent_Add_area_handler )( area_begin, area_size );
  } else if ( _Cache_coherent_Pending_count < CACHE_COHERENT_PENDING_AREAS ) {
    Cache_coherent_Area *area;

    area = &_Cache_coherent_Pending_areas[ _Cache_coherent_Pending_count ];
    area->begin = area_begin;
    area->size = area_size;
    ++_Cache_coherent_Pending_count;
    sc = RTEMS_SUCCESSFUL;
  } else {
    sc = RTEMS_TOO_MANY;
  }

  if ( needs_locking ) {
    _RTEMS_Unlock_allocator();
  }

  return sc;
}
