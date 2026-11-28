/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup TestsuitesFatalFreeDoubleBinRemote
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

#include <stdlib.h>
#include <rtems/malloc.h>
#include <rtems/score/smpimpl.h>
#include <rtems/score/threaddispatch.h>

#include "tr-free-double.h"

#include <rtems/test.h>

/**
 * @defgroup TestsuitesFatalFreeDoubleBinRemote \
 *   spec:/testsuites/fatal-free-double-bin-remote
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This validation test suite contains a test case which triggers a
 *   fatal error. The initialization task frees a block, so the block goes into
 *   a bin. Another processor frees the block again, so the block goes into the
 *   kept list of its processor. An allocation of the processor finds no used
 *   block for the block of the kept list.
 *
 * @{
 */

const char rtems_test_name[] = "TestsuitesFatalFreeDoubleBinRemote";

void *FreeDoubleBlock;

static void FreeBlock( void *arg )
{
  free( arg );
}

static void Init( rtems_task_argument arg )
{
  Per_CPU_Control *cpu_self;
  uint32_t         other;
  void            *ptr;

  (void) arg;

  /* The block goes into a bin */
  ptr = malloc( 16 );
  RTEMS_OBFUSCATE_VARIABLE( ptr );
  FreeDoubleBlock = ptr;
  free( ptr );

  cpu_self = _Thread_Dispatch_disable();
  other = ( _Per_CPU_Get_index( cpu_self ) + 1 ) %
          rtems_scheduler_get_processor_maximum();
  _SMP_Unicast_action( other, FreeBlock, FreeDoubleBlock );
  _Thread_Dispatch_enable( cpu_self );

  /* The bin of the size class is empty, so the allocation drains the list */
  ptr = rtems_malloc( 3500 );
  RTEMS_OBFUSCATE_VARIABLE( ptr );
  rtems_fatal( RTEMS_FATAL_SOURCE_EXIT, 1 );
}

#define FATAL_SYSINIT_RUN CValFreeDouble_Run

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

/* The storage holds the thread-local storage area of errno */
#define CONFIGURE_IDLE_TASK_STORAGE_SIZE ( 2 * RTEMS_MINIMUM_STACK_SIZE )

#define CONFIGURE_MALLOC_CPU_DESCRIPTORS 64

#define CONFIGURE_WORKSPACE_ALLOCATOR RTEMS_WORKSPACE_ALLOCATOR_RED_BLACK_TREE

#define CONFIGURE_MAXIMUM_PROCESSORS 2

#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_CONSTRUCT_STORAGE_SIZE \
  ( 2 * RTEMS_MINIMUM_STACK_SIZE )

#define CONFIGURE_MAXIMUM_THREAD_LOCAL_STORAGE_SIZE 64

#include "ts-fatal-sysinit.h"

/** @} */
