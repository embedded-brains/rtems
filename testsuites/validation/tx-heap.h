/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestSuitesValidation
 *
 * @brief This header file provides the functions to test the heaps.
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

#ifndef _TX_HEAP_H
#define _TX_HEAP_H

#include <rtems/score/rbheap.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSTestSuitesValidation
 *
 * @{
 */

/**
 * @brief This constant defines the size in bytes of the memory area of the
 *   heap of a test.
 */
#define HEAP_AREA_SIZE ( 4 * RBHEAP_SLAB_SIZE )

/**
 * @brief This structure provides the context of a heap of a test.
 */
typedef struct {
  /**
   * @brief This member contains the heap of the test.
   */
  RBHeap_control heap;

  /**
   * @brief This member contains the C Program Heap which HeapInstall()
   *   replaced.
   */
  RBHeap_control *heap_before;
} HeapContext;

/**
 * @brief This structure contains a snapshot of the blocks of a heap.
 */
typedef struct {
  /**
   * @brief This member contains the count of the used blocks.
   */
  uint32_t used_blocks;

  /**
   * @brief This member contains the count of the free blocks.
   */
  uint32_t free_blocks;

  /**
   * @brief This member contains the total size of the free blocks.
   */
  uintptr_t free_size;
} HeapSnapshot;

/**
 * @brief Initializes the heap of the test on the memory area of the heap
 *   tests and makes it the C Program Heap.
 *
 * @param[out] ctx is the context of the heap.
 */
void HeapInstall( HeapContext *ctx );

/**
 * @brief Initializes the heap of the test again.
 *
 * The heap then holds no used block.
 *
 * @param[out] ctx is the context of the heap.
 */
void HeapReset( HeapContext *ctx );

/**
 * @brief Makes the heap which HeapInstall() replaced the C Program Heap again.
 *
 * @param ctx is the context of the heap.
 */
void HeapRestore( const HeapContext *ctx );

/**
 * @brief Gets the begin of the memory area of the heap tests.
 *
 * @return Returns the begin of the memory area.
 */
void *HeapGetArea( void );

/**
 * @brief Gets the size of the largest free block of the heap.
 *
 * @param heap is the heap.
 *
 * @return Returns the size in bytes of the largest free block.
 */
uintptr_t HeapGetLargestFreeSize( const RBHeap_control *heap );

/**
 * @brief Gets the size of the largest block which an allocation of the heap
 *   can return.
 *
 * @param heap is the heap.
 *
 * @return Returns the size in bytes of the largest block.
 */
uintptr_t HeapGetLargestAllocatableSize( const RBHeap_control *heap );

/**
 * @brief Allocates blocks from the heap until no allocation of the minimum
 *   size succeeds.
 *
 * @param[in, out] heap is the heap.
 */
void HeapExhaust( RBHeap_control *heap );

/**
 * @brief Takes a snapshot of the blocks of the heap.
 *
 * @param heap is the heap.
 *
 * @param[out] snapshot is the snapshot.
 */
void HeapTakeSnapshot( const RBHeap_control *heap, HeapSnapshot *snapshot );

/**
 * @brief Checks that the blocks of the heap match the snapshot.
 *
 * @param heap is the heap.
 *
 * @param snapshot is the snapshot.
 */
void HeapCheckSnapshot(
  const RBHeap_control *heap,
  const HeapSnapshot   *snapshot
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _TX_HEAP_H */
