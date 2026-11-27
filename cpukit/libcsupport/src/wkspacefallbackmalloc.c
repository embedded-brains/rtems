/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup MallocSupport
 *
 * @brief This source file contains the implementation of
 *   _Workspace_Allocate_with_fallback() and _Workspace_Free_with_fallback().
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

#include <rtems/score/wkspacedata.h>
#include <rtems/score/assert.h>
#include <rtems/score/heapimpl.h>
#include <rtems/score/wkspace.h>
#include <rtems/malloc.h>

#include <stdlib.h>

void *_Workspace_Allocate_with_fallback( size_t size )
{
  void *ptr;

  ptr = _Heap_Allocate( &_Workspace_Area, size );

  if ( ptr == NULL ) {
    ptr = rtems_malloc( size );
  }

  return ptr;
}

void _Workspace_Free_with_fallback( void *ptr )
{
  if ( _Workspace_Is_in_storage_area( ptr ) ) {
    bool ok;

    ok = _Heap_Free( &_Workspace_Area, ptr );
    _Assert( ok );
    (void) ok;
  } else {
    free( ptr );
  }
}
