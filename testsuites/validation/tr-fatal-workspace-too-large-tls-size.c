/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup AcfgValFatalWorkspaceTooLargeTlsSize
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
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/wkspacedata.h>

#include "tr-fatal-workspace-too-large-tls-size.h"

#include <rtems/test.h>

/**
 * @defgroup AcfgValFatalWorkspaceTooLargeTlsSize \
 *   spec:/acfg/val/fatal-workspace-too-large-tls-size
 *
 * @ingroup TestsuitesFatalWorkspaceTooLargeTlsSize
 *
 * @brief Tests a fatal error.
 *
 * This test case performs the following actions:
 *
 * - The test action is carried out by providing a thread-local storage object
 *   of sufficient size with respect to the application configuration of the
 *   test suite.
 *
 *   - Check that the expected fatal source is present.
 *
 *   - Check that the expected fatal code is present.
 *
 *   - Check that the CONFIGURE_MAXIMUM_THREAD_LOCAL_STORAGE_SIZE application
 *     configuration option resulted in the expected system setting.
 *
 *   - Check that the RTEMS Workspace reserves a thread-local storage area.
 *
 *   - Check that the RTEMS Workspace is a red-black tree heap without
 *     statically allocated descriptors.
 *
 * @{
 */

/**
 * @brief Test context for spec:/acfg/val/fatal-workspace-too-large-tls-size
 *   test case.
 */
typedef struct {
  /**
   * @brief This member contains a copy of the corresponding
   *   AcfgValFatalWorkspaceTooLargeTlsSize_Run() parameter.
   */
  rtems_fatal_source source;

  /**
   * @brief This member contains a copy of the corresponding
   *   AcfgValFatalWorkspaceTooLargeTlsSize_Run() parameter.
   */
  rtems_fatal_code code;
} AcfgValFatalWorkspaceTooLargeTlsSize_Context;

static AcfgValFatalWorkspaceTooLargeTlsSize_Context
  AcfgValFatalWorkspaceTooLargeTlsSize_Instance;

static _Thread_local volatile uint8_t
  large_tls_object[ 64 * sizeof( void * ) + 1 ];

static volatile bool shall_not_load_the_value;

static T_fixture AcfgValFatalWorkspaceTooLargeTlsSize_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &AcfgValFatalWorkspaceTooLargeTlsSize_Instance
};

/**
 * @brief The test action is carried out by providing a thread-local storage
 *   object of sufficient size with respect to the application configuration of
 *   the test suite.
 */
static void AcfgValFatalWorkspaceTooLargeTlsSize_Action_0(
  AcfgValFatalWorkspaceTooLargeTlsSize_Context *ctx
)
{
  if ( shall_not_load_the_value ) {
    uint8_t value;

    value = large_tls_object[ 0 ];
    RTEMS_OBFUSCATE_VARIABLE( value );
  }

  /*
   * Check that the expected fatal source is present.
   */
  T_step_eq_int( 0, ctx->source, INTERNAL_ERROR_CORE );

  /*
   * Check that the expected fatal code is present.
   */
  T_step_eq_ulong( 1, ctx->code, INTERNAL_ERROR_TOO_LARGE_TLS_SIZE );

  /*
   * Check that the CONFIGURE_MAXIMUM_THREAD_LOCAL_STORAGE_SIZE application
   * configuration option resulted in the expected system setting.
   */
  T_step_eq_sz( 2, _Thread_Maximum_TLS_size, 0 );

  /*
   * Check that the RTEMS Workspace reserves a thread-local storage area.
   */
  T_step_ne_sz( 3, _Workspace_TLS_area_count, 0 );

  /*
   * Check that the RTEMS Workspace is a red-black tree heap without statically
   * allocated descriptors.
   */
  T_step_eq_uptr( 4, _Workspace_Descriptor_size, 0 );
  T_step_eq_uptr(
    5,
    _Workspace_RBHeap.area_begin,
    (uintptr_t) _Workspace_Storage_area
  );
}

void AcfgValFatalWorkspaceTooLargeTlsSize_Run(
  rtems_fatal_source source,
  rtems_fatal_code   code
)
{
  AcfgValFatalWorkspaceTooLargeTlsSize_Context *ctx;

  ctx = &AcfgValFatalWorkspaceTooLargeTlsSize_Instance;
  ctx->source = source;
  ctx->code = code;

  ctx = T_case_begin(
    "AcfgValFatalWorkspaceTooLargeTlsSize",
    &AcfgValFatalWorkspaceTooLargeTlsSize_Fixture
  );

  T_plan( 6 );

  AcfgValFatalWorkspaceTooLargeTlsSize_Action_0( ctx );

  T_case_end();
}

/** @} */
