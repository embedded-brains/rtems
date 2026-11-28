/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup AcfgValHeapDefaultDescriptors
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

#include <rtems.h>
#include <rtems/score/rbheap.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/wkspacedata.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup AcfgValHeapDefaultDescriptors \
 *   spec:/acfg/val/heap-default-descriptors
 *
 * @ingroup TestsuitesValidationHeap2
 *
 * @brief Tests the default count of statically allocated descriptors of the
 *   RTEMS Workspace.
 *
 * This test case performs the following actions:
 *
 * - Create a task for each block for which the RTEMS Workspace reserves
 *   memory.
 *
 *   - Check that the RTEMS Workspace reserves memory for 49 blocks.
 *
 *   - Check that the default value of CONFIGURE_WORKSPACE_DESCRIPTOR_COUNT
 *     gives two slabs of statically allocated descriptors for 49 blocks.
 *
 *   - Check that the creation of each task was successful.
 *
 *   - Check that the statically allocated descriptors hold the descriptors of
 *     all blocks, so that the RTEMS Workspace has no slab in its storage area.
 *
 *   - Delete the tasks.
 *
 * @{
 */

/**
 * @brief Test context for spec:/acfg/val/heap-default-descriptors test case.
 */
typedef struct {
  /**
   * @brief This member contains the identifiers of the created tasks.
   */
  rtems_id tasks[ 49 ];

  /**
   * @brief This member contains the status of each task creation.
   */
  rtems_status_code status[ 49 ];
} AcfgValHeapDefaultDescriptors_Context;

static AcfgValHeapDefaultDescriptors_Context
  AcfgValHeapDefaultDescriptors_Instance;

static T_fixture AcfgValHeapDefaultDescriptors_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &AcfgValHeapDefaultDescriptors_Instance
};

/**
 * @brief Create a task for each block for which the RTEMS Workspace reserves
 *   memory.
 */
static void AcfgValHeapDefaultDescriptors_Action_0(
  AcfgValHeapDefaultDescriptors_Context *ctx
)
{
  size_t i;

  for ( i = 0; i < RTEMS_ARRAY_SIZE( ctx->tasks ); ++i ) {
    ctx->status[ i ] = rtems_task_create(
      rtems_build_name( 'T', 'A', 'S', 'K' ),
      1,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &ctx->tasks[ i ]
    );
  }

  /*
   * Check that the RTEMS Workspace reserves memory for 49 blocks.
   */
  T_eq_sz( _Workspace_TLS_area_count, RTEMS_ARRAY_SIZE( ctx->tasks ) );

  /*
   * Check that the default value of CONFIGURE_WORKSPACE_DESCRIPTOR_COUNT gives
   * two slabs of statically allocated descriptors for 49 blocks.
   */
  T_eq_uptr( _Workspace_Descriptor_size, 2 * RBHEAP_SLAB_SIZE );

  /*
   * Check that the creation of each task was successful.
   */
  for ( i = 0; i < RTEMS_ARRAY_SIZE( ctx->tasks ); ++i ) {
    T_rsc_success( ctx->status[ i ] );
  }

  T_true( _RBHeap_Walk( &_Workspace_RBHeap ) );

  /*
   * Check that the statically allocated descriptors hold the descriptors of
   * all blocks, so that the RTEMS Workspace has no slab in its storage area.
   */
  T_eq_uptr( _Workspace_RBHeap.area_slab_count, 0 );

  /*
   * Delete the tasks.
   */
  for ( i = 0; i < RTEMS_ARRAY_SIZE( ctx->tasks ); ++i ) {
    if ( ctx->status[ i ] == RTEMS_SUCCESSFUL ) {
      DeleteTask( ctx->tasks[ i ] );
    }
  }
}

/**
 * @fn void T_case_body_AcfgValHeapDefaultDescriptors( void )
 */
T_TEST_CASE_FIXTURE(
  AcfgValHeapDefaultDescriptors,
  &AcfgValHeapDefaultDescriptors_Fixture
)
{
  AcfgValHeapDefaultDescriptors_Context *ctx;

  ctx = T_fixture_context();

  AcfgValHeapDefaultDescriptors_Action_0( ctx );
}

/** @} */
