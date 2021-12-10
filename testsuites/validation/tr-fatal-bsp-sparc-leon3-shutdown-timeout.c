/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestCaseBspSparcLeon3ValFatalShutdownTimeout
 */

/*
 * Copyright (C) 2021 embedded brains GmbH (http://www.embedded-brains.de)
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <bsp/leon3.h>
#include <rtems/sysinit.h>
#include <rtems/score/percpu.h>

#include "tr-fatal-bsp-sparc-leon3-shutdown-timeout.h"

#include <rtems/test.h>

/**
 * @defgroup RTEMSTestCaseBspSparcLeon3ValFatalShutdownTimeout \
 *   spec:/bsp/sparc/leon3/val/fatal-shutdown-timeout
 *
 * @ingroup RTEMSTestSuiteTestsuitesFatalBspSparcLeon3ShutdownTimeout
 *
 * @brief Tests a fatal error.
 *
 * This test case performs the following actions:
 *
 * - Request the second processor to wait in an infinite loop with maskable
 *   interrupts disabled.  The test suite will only finish execution if
 *   bsp_fatal_extension() halts the processor after a shutdown request
 *   timeout.
 *
 *   - Check that the expected fatal source is present.
 *
 *   - Check that the expected fatal code is present.
 *
 * @{
 */

/**
 * @brief Test context for spec:/bsp/sparc/leon3/val/fatal-shutdown-timeout
 *   test case.
 */
typedef struct {
  /**
   * @brief This member contains a copy of the corresponding
   *   BspSparcLeon3ValFatalShutdownTimeout_Run() parameter.
   */
  rtems_fatal_source source;

  /**
   * @brief This member contains a copy of the corresponding
   *   BspSparcLeon3ValFatalShutdownTimeout_Run() parameter.
   */
  rtems_fatal_code code;
} BspSparcLeon3ValFatalShutdownTimeout_Context;

static BspSparcLeon3ValFatalShutdownTimeout_Context
  BspSparcLeon3ValFatalShutdownTimeout_Instance;

static void IdleHandler( void *arg )
{
  (void) arg;
  (void) _CPU_Thread_Idle_body( 0 );
}

static const Per_CPU_Job_context idle_context = {
  .handler = IdleHandler
};

static Per_CPU_Job idle_job = {
  .context = &idle_context
};

static void TriggerTestCase( void )
{
  rtems_fatal( RTEMS_FATAL_SOURCE_APPLICATION, 123456 );
}

RTEMS_SYSINIT_ITEM(
  TriggerTestCase,
  RTEMS_SYSINIT_DEVICE_DRIVERS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

static T_fixture BspSparcLeon3ValFatalShutdownTimeout_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &BspSparcLeon3ValFatalShutdownTimeout_Instance
};

/**
 * @brief Request the second processor to wait in an infinite loop with
 *   maskable interrupts disabled.  The test suite will only finish execution
 *   if bsp_fatal_extension() halts the processor after a shutdown request
 *   timeout.
 */
static void BspSparcLeon3ValFatalShutdownTimeout_Action_0(
  BspSparcLeon3ValFatalShutdownTimeout_Context *ctx
)
{
  _Per_CPU_Submit_job( _Per_CPU_Get_by_index( 1 ), &idle_job );

  /*
   * Check that the expected fatal source is present.
   */
  T_step_eq_int( 0, ctx->source, RTEMS_FATAL_SOURCE_APPLICATION );

  /*
   * Check that the expected fatal code is present.
   */
  T_step_eq_ulong( 1, ctx->code, 123456 );
}

void BspSparcLeon3ValFatalShutdownTimeout_Run(
  rtems_fatal_source source,
  rtems_fatal_code   code
)
{
  BspSparcLeon3ValFatalShutdownTimeout_Context *ctx;

  ctx = &BspSparcLeon3ValFatalShutdownTimeout_Instance;
  ctx->source = source;
  ctx->code = code;

  ctx = T_case_begin(
    "BspSparcLeon3ValFatalShutdownTimeout",
    &BspSparcLeon3ValFatalShutdownTimeout_Fixture
  );

  T_plan( 2 );

  BspSparcLeon3ValFatalShutdownTimeout_Action_0( ctx );

  T_case_end();
}

/** @} */
