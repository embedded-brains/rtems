/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsMallocValContext
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
#include <rtems/score/isrlevel.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/threaddispatch.h>

#include "tx-heap.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsMallocValContext spec:/rtems/malloc/val/context
 *
 * @ingroup TestsuitesValidation0
 *
 * @brief Tests the memory allocation functions of the C Program Heap in the
 *   calling contexts.
 *
 * This test case performs the following actions:
 *
 * - Call the memory allocation functions within an interrupt service routine.
 *
 *   - Check that no function allocated a memory area.
 *
 * - Call the memory allocation functions in task context with thread
 *   dispatching disabled.
 *
 *   - Check that no function allocated a memory area.
 *
 * - Call the memory allocation functions while the system is before
 *   multitasking.
 *
 *   - Check that each function allocated a memory area.
 *
 * - Call the memory allocation functions while the system is terminated.
 *
 *   - Check that each function allocated a memory area.
 *
 * @{
 */

#define FUNCTION_COUNT 7

/**
 * @brief Test context for spec:/rtems/malloc/val/context test case.
 */
typedef struct {
  /**
   * @brief This member contains the heap of the test.
   */
  HeapContext heap;

  /**
   * @brief This member contains the block which the realloc() call resizes.
   */
  void *old;

  /**
   * @brief This member contains the begin addresses which the functions
   *   returned.
   */
  void *ptr[ FUNCTION_COUNT ];
} RtemsMallocValContext_Context;

static RtemsMallocValContext_Context RtemsMallocValContext_Instance;

typedef RtemsMallocValContext_Context Context;

static void Allocate( void *arg )
{
  Context *ctx;
  int      eno;

  ctx = arg;
  HeapReset( &ctx->heap );
  ctx->old = _RBHeap_Allocate( &ctx->heap.heap, 1 );
  T_quiet_not_null( ctx->old );
  ctx->ptr[ 0 ] = malloc( 1 );
  ctx->ptr[ 1 ] = calloc( 1, 1 );
  ctx->ptr[ 2 ] = realloc( ctx->old, 2 * HEAP_AREA_SIZE / 3 );
  ctx->ptr[ 3 ] = aligned_alloc( 64, 1 );
  eno = posix_memalign( &ctx->ptr[ 4 ], 64, 1 );
  (void) eno;
  ctx->ptr[ 5 ] = rtems_malloc( 1 );
  ctx->ptr[ 6 ] = rtems_calloc( 1, 1 );
}

static void AllocateInState( Context *ctx, System_state_Codes state )
{
  ISR_Level level;

  _ISR_Local_disable( level );
  _System_state_Set( state );
  Allocate( ctx );
  _System_state_Set( SYSTEM_STATE_UP );
  _ISR_Local_enable( level );
}

static void CheckNone( const Context *ctx )
{
  size_t i;

  for ( i = 0; i < FUNCTION_COUNT; ++i ) {
    T_null( ctx->ptr[ i ] );
  }
}

static void CheckAll( const Context *ctx )
{
  size_t i;

  for ( i = 0; i < FUNCTION_COUNT; ++i ) {
    T_not_null( ctx->ptr[ i ] );
  }
}

static void RtemsMallocValContext_Setup( RtemsMallocValContext_Context *ctx )
{
  HeapInstall( &ctx->heap );
}

static void RtemsMallocValContext_Setup_Wrap( void *arg )
{
  RtemsMallocValContext_Context *ctx;

  ctx = arg;
  RtemsMallocValContext_Setup( ctx );
}

static void RtemsMallocValContext_Teardown(
  RtemsMallocValContext_Context *ctx
)
{
  HeapRestore( &ctx->heap );
}

static void RtemsMallocValContext_Teardown_Wrap( void *arg )
{
  RtemsMallocValContext_Context *ctx;

  ctx = arg;
  RtemsMallocValContext_Teardown( ctx );
}

static T_fixture RtemsMallocValContext_Fixture = {
  .setup = RtemsMallocValContext_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsMallocValContext_Teardown_Wrap,
  .scope = NULL,
  .initial_context = &RtemsMallocValContext_Instance
};

/**
 * @brief Call the memory allocation functions within an interrupt service
 *   routine.
 */
static void RtemsMallocValContext_Action_0(
  RtemsMallocValContext_Context *ctx
)
{
  CallWithinISR( Allocate, ctx );

  /*
   * Check that no function allocated a memory area.
   */
  CheckNone( ctx );
}

/**
 * @brief Call the memory allocation functions in task context with thread
 *   dispatching disabled.
 */
static void RtemsMallocValContext_Action_1(
  RtemsMallocValContext_Context *ctx
)
{
  Per_CPU_Control *cpu_self;

  cpu_self = _Thread_Dispatch_disable();
  Allocate( ctx );
  _Thread_Dispatch_enable( cpu_self );

  /*
   * Check that no function allocated a memory area.
   */
  CheckNone( ctx );
}

/**
 * @brief Call the memory allocation functions while the system is before
 *   multitasking.
 */
static void RtemsMallocValContext_Action_2(
  RtemsMallocValContext_Context *ctx
)
{
  AllocateInState( ctx, SYSTEM_STATE_BEFORE_MULTITASKING );

  /*
   * Check that each function allocated a memory area.
   */
  CheckAll( ctx );
}

/**
 * @brief Call the memory allocation functions while the system is terminated.
 */
static void RtemsMallocValContext_Action_3(
  RtemsMallocValContext_Context *ctx
)
{
  AllocateInState( ctx, SYSTEM_STATE_TERMINATED );

  /*
   * Check that each function allocated a memory area.
   */
  CheckAll( ctx );
}

/**
 * @fn void T_case_body_RtemsMallocValContext( void )
 */
T_TEST_CASE_FIXTURE( RtemsMallocValContext, &RtemsMallocValContext_Fixture )
{
  RtemsMallocValContext_Context *ctx;

  ctx = T_fixture_context();

  RtemsMallocValContext_Action_0( ctx );
  RtemsMallocValContext_Action_1( ctx );
  RtemsMallocValContext_Action_2( ctx );
  RtemsMallocValContext_Action_3( ctx );
}

/** @} */
