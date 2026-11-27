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

#ifdef __cplusplus
extern "C" {
#endif

struct Heap_Control;

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
extern struct Heap_Control *const *const _Workspace_Fallback_heap;

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
struct Heap_Control *_Workspace_Malloc_initialize_separate( void );

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_WKSPACEDATA_H */
