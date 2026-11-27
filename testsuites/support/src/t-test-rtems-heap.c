/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestFrameworkImpl
 *
 * @brief This source file contains the implementation of
 *   T_check_heap().
 */

/*
 * Copyright (C) 2018, 2026 embedded brains GmbH & Co. KG
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

#include <rtems/test.h>

#include <rtems.h>
#include <rtems/libcsupport.h>
#include <rtems/malloc.h>

#include <string.h>

typedef struct {
  uint32_t used_blocks;
  size_t   used_size;
} T_heap_usage;

typedef struct {
  T_heap_usage heap_usage;
  T_heap_usage workspace_usage;
} T_resource_heap_context;

static T_resource_heap_context T_resource_heap_instance;

/*
 * Only the used blocks show a leak.  The free blocks depend on the heap
 * implementation.
 */
static void T_get_heap_usage(
  T_heap_usage                *usage,
  const rtems_heap_statistics *stats
)
{
  memset( usage, 0, sizeof( *usage ) );
  usage->used_blocks = stats->used_blocks;
  usage->used_size = stats->size - stats->free_size;
}

static void T_get_workspace_usage( T_heap_usage *usage )
{
  rtems_heap_statistics stats;

  (void) rtems_workspace_get_statistics( &stats );
  T_get_heap_usage( usage, &stats );
}

static void T_get_malloc_usage( T_heap_usage *usage )
{
  rtems_heap_statistics stats;

  (void) malloc_get_statistics( &stats );
  T_get_heap_usage( usage, &stats );
}

static void T_heap_run_initialize( void )
{
  T_resource_heap_context *ctx;

  ctx = &T_resource_heap_instance;
  T_get_workspace_usage( &ctx->workspace_usage );
  T_get_malloc_usage( &ctx->heap_usage );
}

static void T_heap_case_end( void )
{
  T_resource_heap_context *ctx;
  T_heap_usage             usage;
  bool                     ok;

  ctx = &T_resource_heap_instance;

  T_get_workspace_usage( &usage );
  ok = memcmp( &usage, &ctx->workspace_usage, sizeof( usage ) ) == 0;

  if ( !ok ) {
    T_check( &T_special, ok, "memory leak in workspace" );
    memcpy( &ctx->workspace_usage, &usage, sizeof( usage ) );
  }

  T_get_malloc_usage( &usage );
  ok = memcmp( &usage, &ctx->heap_usage, sizeof( usage ) ) == 0;

  if ( !ok ) {
    T_check( &T_special, ok, "memory leak in heap" );
    memcpy( &ctx->heap_usage, &usage, sizeof( usage ) );
  }
}

void T_check_heap( T_event event, const char *name )
{
  (void) name;

  switch ( event ) {
    case T_EVENT_RUN_INITIALIZE_EARLY:
      T_heap_run_initialize();
      break;
    case T_EVENT_CASE_END:
      T_heap_case_end();
      break;
    default:
      break;
  };
}
