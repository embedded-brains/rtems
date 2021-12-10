/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestCaseBspSparcLeon3ValFatalShutdownResponse
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
#include <rtems/score/smpimpl.h>

#include "tr-fatal-bsp-sparc-leon3-shutdown-response.h"

#include <rtems/test.h>

/**
 * @defgroup RTEMSTestCaseBspSparcLeon3ValFatalShutdownResponse \
 *   spec:/bsp/sparc/leon3/val/fatal-shutdown-response
 *
 * @ingroup RTEMSTestSuiteTestsuitesFatalBspSparcLeon3ShutdownResponse
 *
 * @brief Tests a fatal error.
 *
 * This test case performs the following actions:
 *
 * - Request an SMP shutdown on the second processor and wait for a power down
 *   of this processor.
 *
 *   - Check that the expected fatal source is present.
 *
 *   - Check that the expected fatal code is present.
 *
 *   - Check that the second processor was not powered down before the shutdown
 *     request.
 *
 *   - Wait until the second processor is powered down.
 *
 * @{
 */

/**
 * @brief Test context for spec:/bsp/sparc/leon3/val/fatal-shutdown-response
 *   test case.
 */
typedef struct {
  /**
   * @brief This member contains a copy of the corresponding
   *   BspSparcLeon3ValFatalShutdownResponse_Run() parameter.
   */
  rtems_fatal_source source;

  /**
   * @brief This member contains a copy of the corresponding
   *   BspSparcLeon3ValFatalShutdownResponse_Run() parameter.
   */
  rtems_fatal_code code;
} BspSparcLeon3ValFatalShutdownResponse_Context;

static BspSparcLeon3ValFatalShutdownResponse_Context
  BspSparcLeon3ValFatalShutdownResponse_Instance;

static void TriggerTestCase( void )
{
  rtems_fatal( RTEMS_FATAL_SOURCE_APPLICATION, 123456 );
}

RTEMS_SYSINIT_ITEM(
  TriggerTestCase,
  RTEMS_SYSINIT_DEVICE_DRIVERS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

static T_fixture BspSparcLeon3ValFatalShutdownResponse_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &BspSparcLeon3ValFatalShutdownResponse_Instance
};

/**
 * @brief Request an SMP shutdown on the second processor and wait for a power
 *   down of this processor.
 */
static void BspSparcLeon3ValFatalShutdownResponse_Action_0(
  BspSparcLeon3ValFatalShutdownResponse_Context *ctx
)
{
  irqamp  *regs;
  uint32_t mpstat;

  regs = LEON3_IrqCtrl_Regs;
  mpstat = grlib_load_32( &regs->mpstat );
  _SMP_Request_shutdown();

  /*
   * Check that the expected fatal source is present.
   */
  T_step_eq_int( 0, ctx->source, RTEMS_FATAL_SOURCE_APPLICATION );

  /*
   * Check that the expected fatal code is present.
   */
  T_step_eq_ulong( 1, ctx->code, 123456 );

  /*
   * Check that the second processor was not powered down before the shutdown
   * request.
   */
  T_step_eq_u32( 2, mpstat & 0x2U, 0 );

  /*
   * Wait until the second processor is powered down.
   */
  while ( ( grlib_load_32( &regs->mpstat ) & 0x2U ) != 0x2U ) {
    /* Wait */
  }
}

void BspSparcLeon3ValFatalShutdownResponse_Run(
  rtems_fatal_source source,
  rtems_fatal_code   code
)
{
  BspSparcLeon3ValFatalShutdownResponse_Context *ctx;

  ctx = &BspSparcLeon3ValFatalShutdownResponse_Instance;
  ctx->source = source;
  ctx->code = code;

  ctx = T_case_begin(
    "BspSparcLeon3ValFatalShutdownResponse",
    &BspSparcLeon3ValFatalShutdownResponse_Fixture
  );

  T_plan( 3 );

  BspSparcLeon3ValFatalShutdownResponse_Action_0( ctx );

  T_case_end();
}

/** @} */
