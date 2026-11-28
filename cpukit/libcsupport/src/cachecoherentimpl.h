/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicCache
 *
 * @brief This header file provides the interfaces which the cache-coherent
 *   area registration and the cache-coherent heap share.
 */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
 *
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

#ifndef _CACHECOHERENTIMPL_H
#define _CACHECOHERENTIMPL_H

#include <rtems/rtems/status.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This structure describes an area which waits for the
 *   cache-coherent heap.
 */
typedef struct {
  /**
   * @brief This member contains the begin of the area.
   */
  void *begin;

  /**
   * @brief This member contains the size of the area.
   */
  uintptr_t size;
} Cache_coherent_Area;

/**
 * @brief This constant defines the count of areas which wait for the
 *   cache-coherent heap at most.
 */
#define CACHE_COHERENT_PENDING_AREAS 4

/**
 * @brief This array contains the areas which wait for the cache-coherent heap.
 */
extern Cache_coherent_Area
  _Cache_coherent_Pending_areas[ CACHE_COHERENT_PENDING_AREAS ];

/**
 * @brief This object contains the count of areas which wait for the
 *   cache-coherent heap.
 */
extern size_t _Cache_coherent_Pending_count;

/**
 * @brief This object references the handler which adds an area to the
 *   cache-coherent heap.
 *
 * The reference is NULL until the system initialization starts the
 * cache-coherent heap.  The caller of the handler owns the allocator mutex if
 * the system is up.
 */
extern rtems_status_code ( *_Cache_coherent_Add_area_handler )(
  void     *begin,
  uintptr_t size
);

#ifdef __cplusplus
}
#endif

#endif /* _CACHECOHERENTIMPL_H */
