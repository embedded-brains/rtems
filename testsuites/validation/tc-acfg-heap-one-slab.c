/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup AcfgValHeapOneSlab
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
#include <rtems/score/thread.h>
#include <rtems/score/tls.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/wkspacedata.h>

#include <rtems/test.h>

/**
 * @defgroup AcfgValHeapOneSlab spec:/acfg/val/heap-one-slab
 *
 * @ingroup TestsuitesValidationHeap1
 *
 * @brief Tests the RTEMS Workspace configuration options of the test suite.
 *
 * This test case performs the following actions:
 *
 * - Check the effect of the RTEMS Workspace configuration options of the test
 *   suite.
 *
 *   - Check that the RTEMS Workspace is a red-black tree heap.
 *
 *   - Check that the statically allocated descriptors of the RTEMS Workspace
 *     fit into one slab.
 *
 *   - Check that the RTEMS Workspace reserves a thread-local storage area
 *     which holds the thread-local storage of the application.
 *
 * @{
 */

/**
 * @brief Check the effect of the RTEMS Workspace configuration options of the
 *   test suite.
 */
static void AcfgValHeapOneSlab_Action_0( void )
{
  /* Nothing to do */

  /*
   * Check that the RTEMS Workspace is a red-black tree heap.
   */
  T_true( _RBHeap_Walk( &_Workspace_RBHeap ) );
  T_eq_uptr(
    _Workspace_RBHeap.area_begin,
    (uintptr_t) _Workspace_Storage_area
  );

  /*
   * Check that the statically allocated descriptors of the RTEMS Workspace fit
   * into one slab.
   */
  T_eq_uptr( _Workspace_Descriptor_size, RBHEAP_SLAB_SIZE );

  /*
   * Check that the RTEMS Workspace reserves a thread-local storage area which
   * holds the thread-local storage of the application.
   */
  T_eq_sz( _Thread_Maximum_TLS_size, 0 );
  T_ne_sz( _Workspace_TLS_area_count, 0 );
  T_le_uptr( _TLS_Get_allocation_size(), 64 * sizeof( void * ) );
}

/**
 * @fn void T_case_body_AcfgValHeapOneSlab( void )
 */
T_TEST_CASE( AcfgValHeapOneSlab )
{
  AcfgValHeapOneSlab_Action_0();
}

/** @} */
