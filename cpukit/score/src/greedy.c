/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreGreedy
 *
 * @brief This source file contains the implementation of
 *   _Greedy_Allocate(), _Greedy_Allocate_all_except_largest(), and
 *   _Greedy_Free().
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

#include <rtems/score/greedyimpl.h>
#include <rtems/score/assert.h>

/*
 * Each block holds the reference to the next block, so the least size of a
 * block is the size of a pointer.
 */
#define GREEDY_MIN_SIZE sizeof( void * )

/*
 * The search probes downward from the bound with a step which doubles after
 * each failed allocation.  A failed allocation frees no block.  The search
 * then bisects the interval between the first successful probe and the last
 * failed probe.  Only a probe of the bisection frees a block.
 */
static void *_Greedy_Find_largest(
  const Greedy_Operations *operations,
  uintptr_t                upper,
  uintptr_t               *allocatable_size
)
{
  uintptr_t step;
  uintptr_t lower;
  void     *block;

  step = 1;

  while ( true ) {
    if ( upper < GREEDY_MIN_SIZE ) {
      *allocatable_size = 0;
      return NULL;
    }

    if ( step <= upper - GREEDY_MIN_SIZE ) {
      lower = upper - ( step - 1 );
    } else {
      lower = GREEDY_MIN_SIZE;
    }

    block = ( *operations->allocate )( lower );

    if ( block != NULL ) {
      break;
    }

    upper = lower - 1;

    if ( step <= upper / 2 ) {
      step *= 2;
    } else {
      step = upper;
    }
  }

  /*
   * The block has the size lower and no allocation of a size above upper
   * succeeds.
   */
  while ( lower < upper ) {
    uintptr_t middle;
    void     *probe;

    middle = lower + ( upper - lower ) / 2 + 1;
    ( *operations->free )( block );
    probe = ( *operations->allocate )( middle );

    if ( probe != NULL ) {
      block = probe;
      lower = middle;
    } else {
      upper = middle - 1;
      block = ( *operations->allocate )( lower );
      _Assert( block != NULL );
    }
  }

  *allocatable_size = lower;
  return block;
}

/*
 * The fill frees no block.  A free of a large block is expensive under the
 * heap protection, since it writes a pattern into the whole block.
 */
static void **_Greedy_Fill(
  const Greedy_Operations *operations,
  uintptr_t                size
)
{
  void **blocks;

  blocks = NULL;

  while ( size >= GREEDY_MIN_SIZE ) {
    void **block;

    block = ( *operations->allocate )( size );

    if ( block != NULL ) {
      *block = blocks;
      blocks = block;
    } else if ( size > GREEDY_MIN_SIZE ) {
      size /= 2;

      if ( size < GREEDY_MIN_SIZE ) {
        size = GREEDY_MIN_SIZE;
      }
    } else {
      break;
    }
  }

  return blocks;
}

void *_Greedy_Allocate(
  const Greedy_Operations *operations,
  uintptr_t                size_bound,
  const uintptr_t         *block_sizes,
  size_t                   block_count
)
{
  void **reserved;
  void **blocks;
  size_t i;

  reserved = NULL;

  for ( i = 0; i < block_count; ++i ) {
    uintptr_t size;
    void    **block;

    size = block_sizes[ i ];

    if ( size < GREEDY_MIN_SIZE ) {
      size = GREEDY_MIN_SIZE;
    }

    block = ( *operations->allocate )( size );

    if ( block != NULL ) {
      *block = reserved;
      reserved = block;
    }
  }

  blocks = _Greedy_Fill( operations, size_bound );
  _Greedy_Free( operations, reserved );

  return blocks;
}

void *_Greedy_Allocate_all_except_largest(
  const Greedy_Operations *operations,
  uintptr_t                size_bound,
  uintptr_t               *allocatable_size
)
{
  void  *reserved;
  void **blocks;

  reserved = _Greedy_Find_largest( operations, size_bound, allocatable_size );

  if ( reserved == NULL ) {
    return NULL;
  }

  blocks = _Greedy_Fill( operations, *allocatable_size );
  ( *operations->free )( reserved );

  return blocks;
}

void _Greedy_Free( const Greedy_Operations *operations, void *blocks )
{
  void **block;

  block = blocks;

  while ( block != NULL ) {
    void **next;

    next = *block;
    ( *operations->free )( block );
    block = next;
  }
}
