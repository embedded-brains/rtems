/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreRBHeap
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSScoreRBHeap.
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RTEMS_SCORE_RBHEAP_H
#define _RTEMS_SCORE_RBHEAP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <rtems/score/cpu.h>
#include <rtems/score/heapinfo.h>
#include <rtems/score/rbtree.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSScoreRBHeap Red-Black Tree Heap Handler
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the red-black tree heap.
 *
 * The heap keeps a descriptor for each block in slabs apart from the blocks.
 * A red-black tree orders the free blocks by the size and then by the begin.
 * A second tree orders the used blocks, the slabs and the holes by the begin.
 *
 * @image html rbheap-structures.svg "Data structures of the heap"
 *
 * The control of a heap holds the roots of the two trees, the slab lists and
 * the counts of the descriptors.  Each node of a tree is a descriptor in a
 * slot of a slab.  The first slot of a slab in an area describes the slab
 * itself.  The slab lists order the slabs by the count of their descriptors in
 * use.
 *
 * @image html rbheap-layout.svg "Blocks and descriptors of two areas"
 *
 * The used tree holds the used blocks, the slabs inside the areas and the
 * holes between the areas.  The free tree holds the free blocks.  Between two
 * entries of the used tree lies at most one free block, so the two trees give
 * the free neighbours of a used block.  An added slab lies outside the blocks
 * of the heap and is in no tree.
 *
 * @image html rbheap-alloc.svg "Allocation"
 *
 * An allocation takes the best fit of the free tree which holds the block at a
 * multiple of the alignment.  It carves the block out of the free block.  The
 * parts before and after the block stay free.
 *
 * @image html rbheap-free.svg "Free"
 *
 * A free finds the block and the gap around it in the used tree.  It merges
 * the block with the free blocks of the gap.  It then drains a slab which the
 * reserve does not need.
 *
 * @image html rbheap-slabs.svg "Life cycle of a slab"
 *
 * The capacity of the slabs holds at least 2E + 3 descriptors for E entries of
 * the used tree.  The heap carves a slab when the reserve needs one.  It
 * drains a slab which the reserve does not need.  A drain moves a few
 * descriptors in each operation, so no operation moves all descriptors of a
 * slab.
 *
 * @{
 */

/**
 * @brief The least value of #RBHEAP_ALIGNMENT.
 */
#define RBHEAP_UNIT 8

/**
 * @brief The least usable size of a used block of the red-black tree heap.
 *
 * A used block holds at least two pointers, so that a user can keep a chain
 * node in it.
 */
#define RBHEAP_MIN_SIZE ( 2 * sizeof( void * ) )

/**
 * @brief The size of a slab of descriptors of the red-black tree heap.
 *
 * A slab is a used block.  It begins at a multiple of its size.
 */
#define RBHEAP_SLAB_SIZE ( 512 * sizeof( uintptr_t ) )

/**
 * @brief The status of a resize of the red-black tree heap.
 */
typedef enum {
  RBHEAP_RESIZE_SUCCESSFUL,
  RBHEAP_RESIZE_UNSATISFIED,
  RBHEAP_RESIZE_INVALID_BLOCK
} RBHeap_resize_status;

/**
 * @brief The alignment of each begin and the unit of each size of a block of
 *   the red-black tree heap.
 *
 * It is the heap alignment of the CPU port, at least #RBHEAP_UNIT.
 */
#ifndef RBHEAP_ALIGNMENT
#if CPU_HEAP_ALIGNMENT > RBHEAP_UNIT
#define RBHEAP_ALIGNMENT CPU_HEAP_ALIGNMENT
#else
#define RBHEAP_ALIGNMENT RBHEAP_UNIT
#endif
#endif

/**
 * @brief The descriptor of a block of the red-black tree heap.
 *
 * The descriptor is the node of the block in the used tree or in the free
 * tree.
 */
typedef struct RBHeap_descriptor {
  /**
   * @brief The node of the descriptor in the used tree or in the free tree.
   */
  RBTree_Node node;

  /**
   * @brief The begin of the block.
   *
   * A descriptor in the list of the returned descriptors of a slab holds the
   * next descriptor of the list.
   */
  uintptr_t begin;

  /**
   * @brief The size of the block.
   */
  uintptr_t size;
} RBHeap_descriptor;

/**
 * @brief The maximum size of the area and of a request.
 */
#define RBHEAP_MAX_SIZE ( UINTPTR_MAX / 4 )

/**
 * @brief The header of a slab of descriptors of the red-black tree heap.
 *
 * The header takes the room of one descriptor.  The first descriptor after
 * the header describes the slab itself.
 */
typedef struct RBHeap_slab {
  /**
   * @brief The next slab of the list of the slab, else NULL.
   */
  struct RBHeap_slab *next;

  /**
   * @brief The previous slab of the list of the slab, else NULL.
   */
  struct RBHeap_slab *prev;

  /**
   * @brief The first descriptor of the list of the returned descriptors.
   */
  RBHeap_descriptor *returned;

  /**
   * @brief The count of descriptors in use besides the one of the slab.
   */
  uint32_t live;

  /**
   * @brief The index of the first descriptor which was never in use.
   */
  uint32_t untouched;
} RBHeap_slab;

/**
 * @brief The count of descriptors in a slab, the descriptor of the slab
 *   included.
 */
#define RBHEAP_SLAB_SLOTS \
  ( RBHEAP_SLAB_SIZE / sizeof( RBHeap_descriptor ) - 1 )

/**
 * @brief The count of the occupancy classes of the slabs in the areas of a
 *   red-black tree heap.
 */
#define RBHEAP_SLAB_CLASSES 8

/**
 * @brief The count of descriptors which one call of _RBHeap_Release_slab()
 *   moves out of the draining slab at most.
 */
#define RBHEAP_RELEASE_MOVES 4

/**
 * @brief The size of the guard word which ends a used block.
 *
 * Without #RBHEAP_GUARD, the size is zero.
 */
#ifdef RBHEAP_GUARD
#define RBHEAP_GUARD_SIZE sizeof( uintptr_t )
#else
#define RBHEAP_GUARD_SIZE 0
#endif

/**
 * @brief An upper bound of the bytes which an allocation takes from the area
 *   in addition to its block.
 *
 * The block holds the size, at least #RBHEAP_MIN_SIZE, plus
 * #RBHEAP_GUARD_SIZE, rounded up to #RBHEAP_ALIGNMENT.  The bound holds the
 * share of the allocation in a slab of descriptors.  It holds also the share
 * in a free part which the alignment of a slab leaves in front of it.  Each
 * slab gives one descriptor to itself and one to that free part.
 */
#define RBHEAP_BLOCK_OVERHEAD                            \
  ( 2 * ( ( RBHEAP_SLAB_SIZE + RBHEAP_SLAB_SLOTS - 3 ) / \
          ( RBHEAP_SLAB_SLOTS - 2 ) ) )

/**
 * @brief An upper bound of the bytes which an area takes beyond the blocks
 *   which it provides.
 *
 * The bound holds the first slab of descriptors, the alignment of the slab,
 * and the alignment of the begin and the end of the area.
 */
#define RBHEAP_AREA_OVERHEAD ( 2 * RBHEAP_SLAB_SIZE + 2 * RBHEAP_ALIGNMENT )

/**
 * @brief The handler which provides a slab of descriptors to a red-black tree
 *   heap.
 *
 * @return Returns the begin of a slab of #RBHEAP_SLAB_SIZE bytes, which begins
 *   at a multiple of its size, else NULL.  The slab lies outside of each area
 *   of the heap and stays with the heap.
 */
typedef void *( *RBHeap_slab_source )( void );

/**
 * @brief The control of a red-black tree heap.
 */
typedef struct RBHeap_control {
  /**
   * @brief The root of the free tree, which orders the free blocks by the size
   *   and then by the begin.
   */
  RBTree_Control free_root;

  /**
   * @brief The root of the used tree, which orders the used blocks, the slabs
   *   and the holes by the begin.
   */
  RBTree_Control used_root;

  /**
   * @brief The begin of the first area.
   */
  uintptr_t area_begin;

  /**
   * @brief The end of the last area.
   */
  uintptr_t area_end;

  /**
   * @brief The begin of the range of the added slabs.
   */
  uintptr_t added_begin;

  /**
   * @brief The end of the range of the added slabs.
   */
  uintptr_t added_end;

  /**
   * @brief The total size of the free blocks.
   */
  uintptr_t free_size;

  /**
   * @brief The home slab and the slabs outside of the areas, each with a free
   *   descriptor.
   */
  RBHeap_slab *partial;

  /**
   * @brief The slabs in the areas with a free descriptor and a descriptor in
   *   use besides their own, by occupancy.
   *
   * Index i holds the slabs with 2^i up to below 2^(i + 1) descriptors in use
   * besides their own.
   */
  RBHeap_slab *occupied[ RBHEAP_SLAB_CLASSES ];

  /**
   * @brief The slabs in the areas without a descriptor in use besides their
   *   own.
   */
  RBHeap_slab *empty;

  /**
   * @brief The slab which _RBHeap_Initialize() places in the area, else NULL.
   *
   * The home slab provides a descriptor before the other slabs in the area and
   * stays with the heap.
   */
  RBHeap_slab *home;

  /**
   * @brief The slab which _RBHeap_Release_slab() empties, else NULL.
   *
   * The draining slab is on no slab list.  The capacity and the spare count
   * leave out its descriptors.
   */
  RBHeap_slab *draining;

  /**
   * @brief The index of the next slot of the draining slab which
   *   _RBHeap_Release_slab() visits.
   */
  uint32_t drain_slot;

  /**
   * @brief The count of slabs in the empty list.
   */
  uintptr_t empty_count;

  /**
   * @brief The source of the slabs of descriptors, else NULL.
   *
   * Without a source, the heap takes each slab from its areas.  With a source,
   * the heap writes nothing into its areas besides the guard words.
   */
  RBHeap_slab_source slab_source;

  /**
   * @brief The count of free descriptors in the slabs.
   */
  uintptr_t spare;

  /**
   * @brief The count of descriptors of all slabs, each without the
   *   descriptor of its slab.
   */
  uintptr_t capacity;

  /**
   * @brief The count of entries of the used tree.
   *
   * The used blocks, the holes, and the slabs inside the areas are the
   * entries.  At most one free block lies between two entries, so the free
   * blocks are at most one more than the entries.  An allocation ensures a
   * capacity of at least two descriptors per entry plus one, so that each
   * split of a free block finds its descriptors.
   */
  uintptr_t entries;

  /**
   * @brief The statistics of the heap.
   *
   * The member free_size of the statistics is unused.  The member free_size
   * of the control holds the free size.
   */
  Heap_Statistics stats;
} RBHeap_control;

/**
 * @brief Initializes the red-black tree heap so that it manages the area.
 *
 * The first slab of descriptors takes a part of the area.
 *
 * @param[out] control is the control of the heap.
 *
 * @param area_begin is the begin of the area.
 *
 * @param area_size is the size of the area.
 *
 * @retval true The heap manages the area.
 *
 * @retval false The area is too small.
 */
bool _RBHeap_Initialize(
  RBHeap_control *control,
  void           *area_begin,
  uintptr_t       area_size
);

/**
 * @brief Initializes the red-black tree heap so that it manages the area with
 *   descriptors outside of the area.
 *
 * The heap takes its first slab of descriptors from the slab and each further
 * slab from the slab source.  It writes nothing into its areas besides the
 * guard words.
 *
 * @param[out] control is the control of the heap.
 *
 * @param area_begin is the begin of the area.
 *
 * @param area_size is the size of the area.
 *
 * @param slab is the begin of the first slab of descriptors.  The slab has
 *   #RBHEAP_SLAB_SIZE bytes, begins at a multiple of its size, and lies outside
 *   of the area.  It stays with the heap.
 *
 * @param slab_source is the source of the further slabs of descriptors.
 *
 * @retval true The heap manages the area.
 *
 * @retval false The area holds no block.
 */
bool _RBHeap_Initialize_with_slab(
  RBHeap_control    *control,
  void              *area_begin,
  uintptr_t          area_size,
  void              *slab,
  RBHeap_slab_source slab_source
);

/**
 * @brief Adds the descriptors of the area to the red-black tree heap.
 *
 * The heap takes each slab of #RBHEAP_SLAB_SIZE at a multiple of its
 * size in the area.  An added slab stays with the heap and never becomes a
 * block.  If the heap holds no used block, the free area moves to the added
 * descriptors and the slab of the initialization becomes free.
 *
 * @param[in, out] control is the control of the heap.
 *
 * @param area_begin is the begin of the area.
 *
 * @param area_size is the size of the area.
 *
 * @retval true The heap uses the descriptors of the area.
 *
 * @retval false The area holds no slab or overlaps an area of the heap.  The
 *   heap is unchanged.
 */
bool _RBHeap_Add_descriptors(
  RBHeap_control *control,
  void           *area_begin,
  uintptr_t       area_size
);

/**
 * @brief Extends the red-black tree heap by the area.
 *
 * The area lies below the areas of the heap, above them, or in a hole between
 * two of them.  A hole between two areas is a used block.  Its descriptor lies
 * in a slab of descriptors, and bit 0 of its begin is set.  A free and a
 * resize of a hole fail.
 *
 * @param[in, out] control is the control of the heap.
 *
 * @param area_begin is the begin of the area.
 *
 * @param area_size is the size of the area.
 *
 * @retval true The heap manages the area.
 *
 * @retval false The area is too small, or it overlaps an area of the heap or
 *   the range of the added slabs.  Else the heap lacks a descriptor for a hole
 *   or for the area.  The heap is unchanged.
 */
bool _RBHeap_Extend(
  RBHeap_control *control,
  void           *area_begin,
  uintptr_t       area_size
);

/**
 * @brief Allocates a block of the size in the red-black tree heap.
 *
 * @param[in, out] control is the control of the heap.
 *
 * @param size is the size of the block.
 *
 * @return Returns the begin of the block, else NULL.  The begin is a multiple
 *   of #RBHEAP_ALIGNMENT.  The size of the block is a multiple of
 *   #RBHEAP_ALIGNMENT.  With #RBHEAP_GUARD, a guard word ends the block.
 */
void *_RBHeap_Allocate( RBHeap_control *control, uintptr_t size );

/**
 * @brief Allocates a block of the size which begins at a multiple of the
 *   alignment in the red-black tree heap.
 *
 * @param[in, out] control is the control of the heap.
 *
 * @param size is the size of the block.
 *
 * @param alignment is the alignment of the block.  It is a power of two.
 *
 * @return Returns the begin of the block, else NULL.  The begin is a
 *   multiple of the alignment and of #RBHEAP_ALIGNMENT.  The size of the
 *   block is a multiple of #RBHEAP_ALIGNMENT.
 */
void *_RBHeap_Allocate_aligned(
  RBHeap_control *control,
  uintptr_t       size,
  uintptr_t       alignment
);

/**
 * @brief Resizes the block of the red-black tree heap in place.
 *
 * @param[in, out] control is the control of the heap.
 *
 * @param block is the begin of the block.
 *
 * @param size is the new size of the block.
 *
 * @param[out] old_size is the size of the block before the resize.  With
 *   #RBHEAP_GUARD, it is the size before the guard word.
 *
 * @retval RBHEAP_RESIZE_SUCCESSFUL The block holds the new size.  A shrink
 *   without a free block after the block and without a spare descriptor leaves
 *   the block unchanged.
 *
 * @retval RBHEAP_RESIZE_UNSATISFIED The neighbours of the block cannot
 *   provide the new size.  The block is unchanged.
 *
 * @retval RBHEAP_RESIZE_INVALID_BLOCK The address is no begin of a used
 *   block, or the guard word of the block is overwritten.
 */
RBHeap_resize_status _RBHeap_Resize(
  RBHeap_control *control,
  void           *block,
  uintptr_t       size,
  uintptr_t      *old_size
);

/**
 * @brief Frees the block of the red-black tree heap.
 *
 * @param[in, out] control is the control of the heap.
 *
 * @param block is the begin of the block, or NULL.
 *
 * @retval true The block is free, or the block is NULL.
 *
 * @retval false The address is no begin of a used block, or the guard word of
 *   the block is overwritten.  The heap is unchanged.
 */
bool _RBHeap_Free( RBHeap_control *control, void *block );

/**
 * @brief Allocates a block of the size which begins at a multiple of the
 *   alignment and which crosses no multiple of the boundary in the red-black
 *   tree heap.
 *
 * A boundary other than zero raises the alignment to the boundary.  The block
 * then begins at a multiple of the boundary, so it crosses no multiple of the
 * boundary if its size is at most the boundary.
 *
 * @param[in, out] control is the control of the heap.
 *
 * @param size is the size of the block.
 *
 * @param alignment is the alignment of the block.  It is zero or a power of
 *   two.
 *
 * @param boundary is the boundary of the block.  It is zero or a power of
 *   two.
 *
 * @return Returns the begin of the block, else NULL.  The function returns
 *   NULL if the alignment or the boundary is not zero and no power of two.  It
 *   returns NULL if the boundary is not zero and is no multiple of the
 *   alignment.  It returns NULL if the size is greater than a boundary which
 *   is not zero.
 */
void *_RBHeap_Allocate_aligned_with_boundary(
  RBHeap_control *control,
  uintptr_t       size,
  uintptr_t       alignment,
  uintptr_t       boundary
);

/**
 * @brief Gets the size of the used block of the red-black tree heap.
 *
 * @param[in] control is the control of the heap.
 *
 * @param block is the begin of the block.
 *
 * @param[out] size is the size of the block, without the guard word of
 *   #RBHEAP_GUARD.
 *
 * @retval true The address is the begin of a used block.
 *
 * @retval false The address is no begin of a used block.  The function does
 *   not set the size.
 */
bool _RBHeap_Size_of_block(
  const RBHeap_control *control,
  const void           *block,
  uintptr_t            *size
);

/**
 * @brief Gets the information of the red-black tree heap.
 *
 * The free information and the used information come from a walk of the
 * trees.  The used information holds the blocks of the users alone.
 *
 * @param[in] control is the control of the heap.
 *
 * @param[out] info is the information.
 */
void _RBHeap_Get_information(
  const RBHeap_control   *control,
  Heap_Information_block *info
);

/**
 * @brief Checks the trees, the blocks and the slabs of descriptors of the
 *   red-black tree heap.
 *
 * @param[in] control is the control of the heap.
 *
 * @retval true The heap is consistent.
 *
 * @retval false The heap is inconsistent.
 */
bool _RBHeap_Walk( const RBHeap_control *control );

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_RBHEAP_H */
