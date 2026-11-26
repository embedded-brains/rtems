/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CValFreeDouble
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

#include "tr-free-double.h"

#include <rtems/test.h>

/**
 * @defgroup CValFreeDouble spec:/c/val/free-double
 *
 * @ingroup TestsuitesFatalFreeDoubleBin
 * @ingroup TestsuitesFatalFreeDoubleBinPop
 * @ingroup TestsuitesFatalFreeDoubleBinRemote
 * @ingroup TestsuitesFatalFreeDoubleKeptOwner
 * @ingroup TestsuitesFatalFreeDoubleKeptRemote
 *
 * @brief Tests a free of a block which the application freed before.
 *
 * This test case performs the following actions:
 *
 * - The test suite frees a block twice.
 *
 *   - Check that the expected fatal source is present.
 *
 *   - Check that the address of the block is the fatal code.
 *
 * @{
 */

/**
 * @brief Test context for spec:/c/val/free-double test case.
 */
typedef struct {
  /**
   * @brief This member contains a copy of the corresponding
   *   CValFreeDouble_Run() parameter.
   */
  rtems_fatal_source source;

  /**
   * @brief This member contains a copy of the corresponding
   *   CValFreeDouble_Run() parameter.
   */
  rtems_fatal_code code;
} CValFreeDouble_Context;

static CValFreeDouble_Context CValFreeDouble_Instance;

static T_fixture CValFreeDouble_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &CValFreeDouble_Instance
};

/**
 * @brief The test suite frees a block twice.
 */
static void CValFreeDouble_Action_0( CValFreeDouble_Context *ctx )
{
  /* Nothing to do */

  /*
   * Check that the expected fatal source is present.
   */
  T_step_eq_int( 0, ctx->source, RTEMS_FATAL_SOURCE_INVALID_HEAP_FREE );

  /*
   * Check that the address of the block is the fatal code.
   */
  T_step_eq_ulong( 1, ctx->code, (rtems_fatal_code) FreeDoubleBlock );
}

void CValFreeDouble_Run( rtems_fatal_source source, rtems_fatal_code code )
{
  CValFreeDouble_Context *ctx;

  ctx = &CValFreeDouble_Instance;
  ctx->source = source;
  ctx->code = code;

  ctx = T_case_begin( "CValFreeDouble", &CValFreeDouble_Fixture );

  T_plan( 2 );

  CValFreeDouble_Action_0( ctx );

  T_case_end();
}

/** @} */
