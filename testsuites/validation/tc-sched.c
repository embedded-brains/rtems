/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup NewlibValSched
 */

/*
 * Copyright (C) 2026 OHB System AG
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
#include <sys/lock.h>

#include <rtems/test.h>

/**
 * @defgroup NewlibValSched spec:/newlib/val/sched
 *
 * @ingroup TestsuitesValidationNoClock1
 *
 * @brief Tests the scheduler queries defined by Newlib.
 *
 * This test case performs the following actions:
 *
 * - Call _Sched_Count().
 *
 *   - Check that _Sched_Count() returned the count of scheduler instances
 *     configured by the test suite.
 *
 * @{
 */

#if defined( RTEMS_SMP )
#define SCHEDULER_COUNT 4
#else
#define SCHEDULER_COUNT 1
#endif

/**
 * @brief Call _Sched_Count().
 */
static void NewlibValSched_Action_0( void )
{
  int count;

  count = _Sched_Count();

  /*
   * Check that _Sched_Count() returned the count of scheduler instances
   * configured by the test suite.
   */
  T_eq_int( count, SCHEDULER_COUNT );
}

/**
 * @fn void T_case_body_NewlibValSched( void )
 */
T_TEST_CASE( NewlibValSched )
{
  NewlibValSched_Action_0();
}

/** @} */
