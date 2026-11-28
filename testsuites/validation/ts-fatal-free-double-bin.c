/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup TestsuitesFatalFreeDoubleBin
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
#include <rtems/sysinit.h>

#include "tr-free-double.h"

#include <rtems/test.h>

/**
 * @defgroup TestsuitesFatalFreeDoubleBin \
 *   spec:/testsuites/fatal-free-double-bin
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This validation test suite contains a test case which triggers a
 *   fatal error during system initialization. A system initialization handler
 *   frees a block twice. The second free finds the block in a bin of the
 *   processor blocks.
 *
 * @{
 */

const char rtems_test_name[] = "TestsuitesFatalFreeDoubleBin";

void *FreeDoubleBlock;

static void FreeBlockTwice( void )
{
  void *ptr;
  void *again;

  ptr = malloc( 16 );
  FreeDoubleBlock = ptr;
  again = ptr;
  RTEMS_OBFUSCATE_VARIABLE( again );
  free( ptr );
  free( again );
}

RTEMS_SYSINIT_ITEM(
  FreeBlockTwice,
  RTEMS_SYSINIT_DEVICE_DRIVERS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

#define FATAL_SYSINIT_RUN CValFreeDouble_Run

#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

/* The storage holds the thread-local storage area of errno */
#define CONFIGURE_IDLE_TASK_STORAGE_SIZE ( 2 * RTEMS_MINIMUM_STACK_SIZE )

#define CONFIGURE_MALLOC_CPU_DESCRIPTORS 64

#define CONFIGURE_WORKSPACE_ALLOCATOR RTEMS_WORKSPACE_ALLOCATOR_RED_BLACK_TREE

#include "ts-fatal-sysinit.h"

/** @} */
