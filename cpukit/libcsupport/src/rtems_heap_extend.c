/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup libcsupport
 *
 * @brief This source file contains the implementation of
 *   rtems_heap_extend().
 */

/*
 * Copyright (C) 2011, 2026 embedded brains GmbH & Co. KG
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
#include "malloc_p.h"

#include <rtems/score/apimutex.h>
#include <rtems/score/heap.h>

rtems_status_code rtems_heap_extend( void *area_begin, uintptr_t area_size )
{
  uintptr_t extended_size;

  switch ( _Malloc_System_state() ) {
    case MALLOC_SYSTEM_STATE_NORMAL:
      _RTEMS_Lock_allocator();
      extended_size = _Heap_Extend(
        RTEMS_Malloc_Heap,
        area_begin,
        area_size,
        0
      );
      _RTEMS_Unlock_allocator();
      break;
    case MALLOC_SYSTEM_STATE_NO_PROTECTION:
      extended_size = _Heap_Extend(
        RTEMS_Malloc_Heap,
        area_begin,
        area_size,
        0
      );
      break;
    default:
      return RTEMS_INCORRECT_STATE;
  }

  if ( extended_size != 0 ) {
    return RTEMS_SUCCESSFUL;
  } else {
    return RTEMS_INVALID_ADDRESS;
  }
}
#endif /* RTEMS_NEWLIB */
