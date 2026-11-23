/* SPDX-License-Identifier: BSD-2-Clause */

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

#include "../spfatal_support/spfatal.h"

#include <rtems/sysinit.h>

#define FATAL_ERROR_TEST_NAME "37"

#define FATAL_ERROR_DESCRIPTION "cache-coherent area too small for a heap"

#define FATAL_ERROR_EXPECTED_SOURCE INTERNAL_ERROR_CORE

#define FATAL_ERROR_EXPECTED_ERROR INTERNAL_ERROR_NO_MEMORY_FOR_HEAP

static void force_error( void )
{
  (void) rtems_cache_coherent_allocate( 1, 0, 0 );
  rtems_test_assert( 0 );
}

static char small_area[ 1 ];

/*
 * The cache-coherent heap starts on the areas which wait for it at the end of
 * this initialization step.
 */
static void add_small_area( void )
{
  rtems_status_code sc;

  sc = rtems_cache_coherent_add_area( small_area, sizeof( small_area ) );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
}

RTEMS_SYSINIT_ITEM(
  add_small_area,
  RTEMS_SYSINIT_BSP_START,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

#include "../spfatal_support/spfatalimpl.h"
