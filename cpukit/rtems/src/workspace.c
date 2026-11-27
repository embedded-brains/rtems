/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This source file contains the implementation of
 *   rtems_workspace_get_information(), rtems_workspace_allocate(), and
 *   rtems_workspace_free().
 */

/*
 *  COPYRIGHT (c) 1989-2009.
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

#include <rtems/score/apimutex.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/protectedheap.h>
#include <rtems/rtems/support.h>

bool rtems_workspace_get_information( Heap_Information_block *the_info )
{
  if ( !the_info ) {
    return false;
  }

  return _Protected_heap_Get_information( &_Workspace_Area, the_info );
}

bool rtems_workspace_allocate( size_t bytes, void **pointer )
{
  void *ptr;

  /*
   * check the arguments
   */
  if ( !pointer ) {
    return false;
  }

  if ( !bytes ) {
    return false;
  }

  /*
   * With the fallback, _Workspace_Allocate() allocates from the C Program Heap
   * if the storage area is exhausted.
   */
  _RTEMS_Lock_allocator();
  ptr = _Workspace_Allocate( bytes );
  _RTEMS_Unlock_allocator();

  if ( !ptr ) {
    return false;
  }

  *pointer = ptr;
  return true;
}

bool rtems_workspace_free( void *pointer )
{
  _RTEMS_Lock_allocator();
  _Workspace_Free( pointer );
  _RTEMS_Unlock_allocator();

  return true;
}
