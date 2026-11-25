/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreWorkspace
 *
 * @brief This header file provides data structures used by the implementation
 *   and the @ref RTEMSImplApplConfig to define ::_Workspace_Size,
 *   ::_Workspace_Is_unified, ::_Workspace_Storage_area,
 *   ::_Workspace_Storage_size, ::_Workspace_TLS_area_count, and
 *   ::_Workspace_Fallback_heap.
 */

/*
 * Copyright (C) 2020, 2026 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_SCORE_WKSPACEDATA_H
#define _RTEMS_SCORE_WKSPACEDATA_H

#include <rtems/score/basedefs.h>
#include <rtems/score/heapinfo.h>

#ifdef __cplusplus
extern "C" {
#endif

struct RBHeap_control;

/**
 * @defgroup RTEMSScoreWorkspace Workspace Handler
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the Workspace Handler implementation.
 *
 * This handler encapsulates functionality related to the management of the
 * RTEMS Workspace.  It provides mechanisms which can be used to define,
 * initialize and manipulate the RTEMS Workspace.
 *
 * @{
 */

/**
 * @brief The workspace size in bytes.
 *
 * This constant is defined by the application configuration via
 * <rtems/confdefs.h>.
 */
extern const uintptr_t _Workspace_Size;

/**
 * @brief Indicates if the application configuration defines
 *   #CONFIGURE_UNIFIED_WORK_AREAS.
 *
 * This constant is defined by the application configuration via
 * <rtems/confdefs.h> or a default configuration.
 */
extern const bool _Workspace_Is_unified;

/**
 * @brief This array provides the storage area of the RTEMS Workspace.
 *
 * This array is defined by the application configuration via
 * <rtems/confdefs.h>.
 */
extern char _Workspace_Storage_area[];

/**
 * @brief This constant provides the size of ::_Workspace_Storage_area in
 *   bytes.
 *
 * This constant is defined by the application configuration via
 * <rtems/confdefs.h>.
 */
extern const uintptr_t _Workspace_Storage_size;

/**
 * @brief This constant provides the count of thread-local storage areas
 *   which ::_Workspace_Storage_area shall hold.
 *
 * The count is zero if the RTEMS Workspace allocates from the C Program Heap
 * when its storage area has no fitting block.
 *
 * This constant is defined by the application configuration via
 * <rtems/confdefs.h>.
 */
extern const size_t _Workspace_TLS_area_count;

/**
 * @brief This constant references the heap control of the C Program Heap if
 *   the RTEMS Workspace allocates from the C Program Heap when its storage
 *   area is exhausted.
 *
 * If the value is NULL, then the RTEMS Workspace uses only its storage area.
 *
 * This constant is defined by the application configuration via
 * <rtems/confdefs.h> or a default configuration.
 */
extern struct RBHeap_control *const *const _Workspace_Fallback_heap;

/**
 * @brief Allocates a memory block of the specified size from the workspace.
 *
 * @param size The size of the memory block.
 *
 * @retval pointer The pointer to the memory block.  The pointer is at least
 *   aligned by CPU_HEAP_ALIGNMENT.
 * @retval NULL No memory block with the requested size is available in the
 *   workspace.
 */
void *_Workspace_Allocate( size_t size );

/**
 * @brief Frees memory to the workspace.
 *
 * This function frees the specified block of memory.
 *
 * @param block The memory to free.
 *
 * @note If @a block is equal to NULL, then the request is ignored.
 *        This allows the caller to not worry about whether or not
 *        a pointer is NULL.
 */
void _Workspace_Free( void *block );

/**
 * @brief Allocates a memory area from the RTEMS Workspace and, if its storage
 *   area is exhausted, from the C Program Heap.
 *
 * The application configuration uses this function to define
 * _Workspace_Allocate().
 *
 * @param size is the size in bytes of the memory area.
 *
 * @return Returns the begin address of the memory area, otherwise NULL.
 */
void *_Workspace_Allocate_with_fallback( size_t size );

/**
 * @brief Frees a memory area which _Workspace_Allocate_with_fallback()
 *   returned.
 *
 * The application configuration uses this function to define
 * _Workspace_Free().
 *
 * @param ptr is the begin address of the memory area.
 */
void _Workspace_Free_with_fallback( void *ptr );

/**
 * @brief Initializes the C Program Heap separated from the RTEMS Workspace.
 *
 * @return Returns the heap control used for the C Program Heap.
 */
struct RBHeap_control *_Workspace_Malloc_initialize_separate( void );

/**
 * @brief This object is the red-black tree heap of the RTEMS Workspace.
 *
 * The application configuration uses this heap if it selects
 * #RTEMS_WORKSPACE_ALLOCATOR_RED_BLACK_TREE.
 */
extern struct RBHeap_control _Workspace_RBHeap;

/**
 * @brief Initializes ::_Workspace_RBHeap with the storage area of the RTEMS
 *   Workspace.
 */
void _Workspace_RBHeap_initialize( void );

/**
 * @brief Stops the system initialization if the allocation size of the
 *   thread-local storage exceeds the size which the storage area of the RTEMS
 *   Workspace reserves for it.
 */
void _Workspace_Check_TLS_allocation_size( void );

/**
 * @brief Allocates a memory area from the storage area of the RTEMS
 *   Workspace.
 *
 * The application configuration defines this function if it selects
 * #RTEMS_WORKSPACE_ALLOCATOR_RED_BLACK_TREE.  Otherwise, the function of the
 * library allocates from ::_Workspace_Area.
 *
 * @param size is the size in bytes of the memory area.
 *
 * @return Returns the begin address of the memory area, otherwise NULL.
 */
void *_Workspace_Storage_allocate( size_t size );

/**
 * @brief Frees a memory area which _Workspace_Storage_allocate() returned.
 *
 * The application configuration defines this function if it selects
 * #RTEMS_WORKSPACE_ALLOCATOR_RED_BLACK_TREE.
 *
 * @param ptr is the begin address of the memory area.
 */
void _Workspace_Storage_free( void *ptr );

/**
 * @brief Gets the statistics of the storage area of the RTEMS Workspace.
 *
 * The application configuration defines this function if it selects
 * #RTEMS_WORKSPACE_ALLOCATOR_RED_BLACK_TREE.  The caller shall own the
 * allocator mutex.
 *
 * @param[out] stats is the statistics.
 */
void _Workspace_Get_statistics( Heap_Statistics *stats );

/**
 * @brief Gets the information of the storage area of the RTEMS Workspace.
 *
 * The application configuration defines this function if it selects
 * #RTEMS_WORKSPACE_ALLOCATOR_RED_BLACK_TREE.  The caller shall own the
 * allocator mutex.
 *
 * @param[out] info is the information.
 */
void _Workspace_Get_information( Heap_Information_block *info );

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_WKSPACEDATA_H */
