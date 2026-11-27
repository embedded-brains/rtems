/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreWorkspace
 *
 * @brief This source file contains the definition of ::_Workspace_Area and the
 *   implementation of _Workspace_Handler_initialization().
 */

/*
 * Copyright (C) 2020, 2026 embedded brains GmbH & Co. KG
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

#include <rtems/score/wkspace.h>
#include <rtems/score/heapfirstfitimpl.h>
#include <rtems/score/assert.h>
#include <rtems/score/interr.h>
#include <rtems/score/thread.h>
#include <rtems/score/tls.h>
#include <rtems/score/wkspacedata.h>
#include <rtems/sysinit.h>

Heap_Control _Workspace_Area;

void _Workspace_Handler_initialization( void )
{
  uintptr_t available_size;

  available_size = _Heap_Initialize(
    &_Workspace_Area,
    _Workspace_Storage_area,
    _Workspace_Storage_size,
    CPU_HEAP_ALIGNMENT
  );

  _Assert( available_size >= _Workspace_Storage_size - _Heap_Area_overhead(
    CPU_HEAP_ALIGNMENT
  ) );
  (void) available_size;

  if ( _Workspace_TLS_area_count != 0 && _Thread_Maximum_TLS_size == 0 ) {
    /*
     * The storage area reserves THREAD_DEFAULT_MAXIMUM_TLS_SIZE for each
     * thread-local storage area.
     */
    if ( _TLS_Get_allocation_size() > THREAD_DEFAULT_MAXIMUM_TLS_SIZE ) {
      _Internal_error( INTERNAL_ERROR_TOO_LARGE_TLS_SIZE );
    }
  }

  _Heap_Protection_set_delayed_free_fraction( &_Workspace_Area, 1 );
}

RTEMS_SYSINIT_ITEM(
  _Workspace_Handler_initialization,
  RTEMS_SYSINIT_WORKSPACE,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
