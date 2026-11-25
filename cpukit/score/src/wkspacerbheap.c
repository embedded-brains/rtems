/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreWorkspace
 *
 * @brief This source file contains the definition of ::_Workspace_RBHeap and
 *   the implementation of _Workspace_RBHeap_initialize().
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

#include <rtems/score/rbheap.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/assert.h>
#include <rtems/score/interr.h>

RBHeap_control _Workspace_RBHeap;

void _Workspace_RBHeap_initialize( void )
{
  bool ok;

  if ( _Workspace_Descriptor_size > 0 ) {
    /* The first slab of the descriptor area is the first slab of the heap. */
    ok = _RBHeap_Initialize_with_slab(
      &_Workspace_RBHeap,
      _Workspace_Storage_area,
      _Workspace_Storage_size,
      _Workspace_Descriptor_begin,
      NULL
    );
    _Assert( ok );
    _Assert( _Workspace_RBHeap.free_size == _Workspace_Storage_size );

    if (
      ok && _Workspace_Descriptor_size > RBHEAP_SLAB_SIZE &&
      !_RBHeap_Add_descriptors(
        &_Workspace_RBHeap,
        (char *) _Workspace_Descriptor_begin + RBHEAP_SLAB_SIZE,
        _Workspace_Descriptor_size - RBHEAP_SLAB_SIZE
      )
    ) {
      _Internal_error( INTERNAL_ERROR_TOO_LITTLE_WORKSPACE );
    }
  } else {
    ok = _RBHeap_Initialize(
      &_Workspace_RBHeap,
      _Workspace_Storage_area,
      _Workspace_Storage_size
    );
    _Assert( ok );
    _Assert(
      _Workspace_RBHeap.free_size >=
      _Workspace_Storage_size - RBHEAP_AREA_OVERHEAD
    );
  }

  (void) ok;
  _Workspace_Check_TLS_allocation_size();
}
