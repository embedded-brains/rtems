/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup AcfgValHeap
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

#include <rtems/malloc.h>
#include <rtems/score/rbheap.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/wkspacedata.h>

#include "ts-config.h"

#include <rtems/test.h>

/**
 * @defgroup AcfgValHeap spec:/acfg/val/heap
 *
 * @ingroup TestsuitesValidationHeap0
 *
 * @brief Tests the heap configuration options of the test suite.
 *
 * This test case performs the following actions:
 *
 * - Check the effect of the heap configuration options of the test suite.
 *
 *   - Check that the RTEMS Workspace is a red-black tree heap.
 *
 *   - Check the statically allocated descriptors of the RTEMS Workspace.
 *
 *   - Check the statically allocated descriptors of the C Program Heap.
 *
 *   - Check the configuration of the processor blocks in SMP configurations.
 *
 * @{
 */

/**
 * @brief Check the effect of the heap configuration options of the test suite.
 */
static void AcfgValHeap_Action_0( void )
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
   * Check the statically allocated descriptors of the RTEMS Workspace.
   */
  T_eq_uptr(
    _Workspace_Descriptor_size,
    ( ( TEST_WORKSPACE_DESCRIPTOR_COUNT + RBHEAP_SLAB_SLOTS - 2 ) /
      ( RBHEAP_SLAB_SLOTS - 1 ) ) *
      RBHEAP_SLAB_SIZE
  );

  /*
   * Check the statically allocated descriptors of the C Program Heap.
   */
  T_eq_uptr(
    _Malloc_Descriptor_size,
    ( ( TEST_MALLOC_DESCRIPTOR_COUNT + RBHEAP_SLAB_SLOTS - 2 ) /
      ( RBHEAP_SLAB_SLOTS - 1 ) ) *
      RBHEAP_SLAB_SIZE
  );
  T_eq_uptr(
    RTEMS_Malloc_Heap->added_begin,
    (uintptr_t) _Malloc_Descriptor_begin
  );

  /*
   * Check the configuration of the processor blocks in SMP configurations.
   */
  #if defined( RTEMS_SMP )
  T_true( _RBHeap_Has_processor_blocks( RTEMS_Malloc_Heap ) );
  T_eq_uptr(
    RTEMS_Malloc_Heap->processor_pairs,
    TEST_MALLOC_CPU_DESCRIPTORS / 2
  );
  T_eq_uptr( RTEMS_Malloc_Heap->processor_max_size, 4096 );
  #endif
}

/**
 * @fn void T_case_body_AcfgValHeap( void )
 */
T_TEST_CASE( AcfgValHeap )
{
  AcfgValHeap_Action_0();
}

/** @} */
