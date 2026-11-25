/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreGreedy
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSScoreGreedy.
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

#ifndef _RTEMS_SCORE_GREEDYIMPL_H
#define _RTEMS_SCORE_GREEDYIMPL_H

#include <rtems/score/basedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSScoreGreedy Greedy Allocation
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the greedy allocation implementation.
 *
 * A greedy allocation uses all memory of an allocator except the blocks which
 * the caller wants to remain allocatable.  It uses only the allocate and free
 * operations of the allocator.  It allocates blocks of a size until an
 * allocation fails, and then halves the size.  An allocation of a size
 * succeeds if and only if the allocator has a free block for this size.  The
 * greedy allocation therefore finds the largest allocatable size by
 * allocations which probe downward from a bound.  The search is short if the
 * bound is close to this size.
 *
 * The first word of each block of a greedy allocation references the next
 * block.
 *
 * @{
 */

/**
 * @brief This structure provides the operations of an allocator.
 */
typedef struct {
  /**
   * @brief This member references the handler which allocates a block of a
   *   size.
   *
   * The handler shall return NULL, if the allocator has no free block of the
   * size.
   */
  void *( *allocate )( size_t size );

  /**
   * @brief This member references the handler which frees a block.
   */
  void ( *free )( void *ptr );
} Greedy_Operations;

/**
 * @brief Allocates all blocks of the allocator except blocks of the sizes.
 *
 * Afterwards, the allocator has at most @a block_count allocatable blocks of
 * the sizes specified by @a block_sizes.  All other blocks are used.
 *
 * The caller shall own the lock which protects the allocator.
 *
 * @param operations is the operations of the allocator.
 *
 * @param size_bound is a size which no allocatable size of a block of the
 *   allocator exceeds.
 *
 * @param block_sizes is the array of block sizes.
 *
 * @param block_count is the count of block sizes.
 *
 * @return Returns the list of blocks of the greedy allocation.
 */
void *_Greedy_Allocate(
  const Greedy_Operations *operations,
  uintptr_t                size_bound,
  const uintptr_t         *block_sizes,
  size_t                   block_count
);

/**
 * @brief Allocates all blocks of the allocator except the largest free block.
 *
 * Afterwards, the allocator has at most one allocatable block.  This block
 * is the largest free block if it exists.
 *
 * The caller shall own the lock which protects the allocator.
 *
 * @param operations is the operations of the allocator.
 *
 * @param size_bound is a size which no allocatable size of a block of the
 *   allocator exceeds.
 *
 * @param[out] allocatable_size is the allocatable size of the remaining
 *   block.  The value is zero, if no block remains.
 *
 * @return Returns the list of blocks of the greedy allocation.
 */
void *_Greedy_Allocate_all_except_largest(
  const Greedy_Operations *operations,
  uintptr_t                size_bound,
  uintptr_t               *allocatable_size
);

/**
 * @brief Frees the blocks of a greedy allocation.
 *
 * The caller shall own the lock which protects the allocator.
 *
 * @param operations is the operations of the allocator.
 *
 * @param blocks is the list of blocks which _Greedy_Allocate() or
 *   _Greedy_Allocate_all_except_largest() returned.
 */
void _Greedy_Free( const Greedy_Operations *operations, void *blocks );

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_GREEDYIMPL_H */
