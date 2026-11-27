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

#include <rtems.h>
#include <rtems/score/wkspacedata.h>

#include <string.h>

#include <tmacros.h>

const char rtems_test_name[] = "SPTLS 5";

/*
 * The thread-local storage exceeds the 64 pointers which the RTEMS Workspace
 * reserves for each area without CONFIGURE_MAXIMUM_THREAD_LOCAL_STORAGE_SIZE.
 */
static _Thread_local unsigned char large[ 256 * sizeof( void * ) ];

static _Thread_local int initialized = 123;

static rtems_id master_id;

static void check_tls( unsigned char pattern )
{
  size_t i;

  rtems_test_assert( initialized == 123 );

  for ( i = 0; i < sizeof( large ); ++i ) {
    rtems_test_assert( large[ i ] == 0 );
  }

  memset( large, pattern, sizeof( large ) );

  for ( i = 0; i < sizeof( large ); ++i ) {
    rtems_test_assert( large[ i ] == pattern );
  }
}

static void worker( rtems_task_argument arg )
{
  rtems_status_code sc;

  (void) arg;

  check_tls( 0xa5 );

  sc = rtems_event_transient_send( master_id );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  (void) rtems_task_suspend( RTEMS_SELF );
}

static void Init( rtems_task_argument arg )
{
  rtems_status_code sc;
  rtems_id          id;

  (void) arg;

  TEST_BEGIN();

  check_tls( 0x5a );

  master_id = rtems_task_self();

  sc = rtems_task_create(
    rtems_build_name( 'W', 'O', 'R', 'K' ),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  sc = rtems_task_start( id, worker, 0 );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  sc = rtems_event_transient_receive( RTEMS_WAIT, RTEMS_NO_TIMEOUT );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  sc = rtems_task_delete( id );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  rtems_test_assert( _Workspace_TLS_area_count == 0 );

  TEST_END();
  rtems_test_exit( 0 );
}

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS 2

#define CONFIGURE_UNIFIED_WORK_AREAS

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
