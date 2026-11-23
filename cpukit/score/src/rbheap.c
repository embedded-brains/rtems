/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreRBHeap
 *
 * @brief This source file contains the implementation of the
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

/*
 * The red-black tree heap keeps the descriptors of its blocks in slabs.  A
 * descriptor holds the links of its tree, the begin and the size of its block.
 * The free tree orders the free blocks by the size and then by the begin.  The
 * used tree orders the used blocks, the slabs and the holes by the begin.
 * Between two used blocks lies at most one free block, so the used tree and
 * the free tree give the free neighbours of a used block.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rbheapimpl.h"

#include <rtems/score/assert.h>

#include <string.h>

_Static_assert(
  sizeof( RBHeap_descriptor ) == 5 * sizeof( uintptr_t ),
  "RBHeap_descriptor"
);

_Static_assert(
  sizeof( RBHeap_slab ) <= sizeof( RBHeap_descriptor ),
  "RBHeap_slab"
);

_Static_assert( RBHEAP_SLAB_SLOTS >= 8, "RBHEAP_SLAB_SLOTS" );

_Static_assert(
  RBHEAP_SLAB_SLOTS - 2 < ( 1U << RBHEAP_SLAB_CLASSES ),
  "RBHEAP_SLAB_CLASSES"
);

_Static_assert(
  RBHEAP_ALIGNMENT >= RBHEAP_UNIT &&
    ( RBHEAP_ALIGNMENT & ( RBHEAP_ALIGNMENT - 1 ) ) == 0,
  "RBHEAP_ALIGNMENT"
);

_Static_assert(
  ( RBHEAP_SLAB_SIZE & ( RBHEAP_SLAB_SIZE - 1 ) ) == 0,
  "RBHEAP_SLAB_SIZE"
);

#ifdef RBHEAP_GUARD
/* The guard is the last word of a used block. */
static inline void _RBHeap_Guard_set( uintptr_t begin, uintptr_t size )
{
  uintptr_t *guard;

  guard = (uintptr_t *) ( begin + size - RBHEAP_GUARD_SIZE );
  *guard = ~(uintptr_t) guard;
}

static inline bool _RBHeap_Guard_is_intact( uintptr_t begin, uintptr_t size )
{
  const uintptr_t *guard;

  guard = (const uintptr_t *) ( begin + size - RBHEAP_GUARD_SIZE );
  return *guard == ~(uintptr_t) guard;
}
#else
static inline void _RBHeap_Guard_set( uintptr_t begin, uintptr_t size )
{
  (void) begin;
  (void) size;
}

static inline bool _RBHeap_Guard_is_intact( uintptr_t begin, uintptr_t size )
{
  (void) begin;
  (void) size;
  return true;
}
#endif

static inline void _RBHeap_Update_min_free_size( RBHeap_control *control )
{
  if ( control->free_size < control->stats.min_free_size ) {
    control->stats.min_free_size = control->free_size;
  }
}

static RBHeap_descriptor *_RBHeap_Slot( RBHeap_slab *slab, uintptr_t index )
{
  return (RBHeap_descriptor *) slab + 1 + index;
}

static const RBHeap_descriptor *_RBHeap_Const_slot(
  const RBHeap_slab *slab,
  uintptr_t          index
)
{
  return (const RBHeap_descriptor *) slab + 1 + index;
}

static RBHeap_slab *_RBHeap_Slab_of( const RBHeap_descriptor *desc )
{
  return (RBHeap_slab *) ( (uintptr_t) desc & ~( RBHEAP_SLAB_SIZE - 1 ) );
}

/*
 * Returns true if the used descriptor describes a slab.  Only the first slot
 * of a slab lies at the begin of a used block plus the size of a descriptor.
 * The block of a slab has the size of a slab.
 */
static bool _RBHeap_Is_slab( const RBHeap_descriptor *desc )
{
  bool is_slab;

  is_slab = (uintptr_t) desc == desc->begin + sizeof( RBHeap_descriptor );
  _Assert( !is_slab || desc->size == RBHEAP_SLAB_SIZE );
  return is_slab;
}

/*
 * Returns true if the used descriptor lies in its own block.  The descriptor
 * then describes a slab inside an area.  The descriptor of a hole lies in a
 * slab, see _RBHeap_Is_hole().
 */
static bool _RBHeap_Is_own( const RBHeap_descriptor *desc )
{
  return (uintptr_t) desc - desc->begin < desc->size;
}

/* Returns true if the slab lies outside of the blocks of the heap. */
static bool _RBHeap_Is_added( const RBHeap_slab *slab )
{
  return _RBHeap_Const_slot( slab, 0 )->begin != (uintptr_t) slab;
}

/*
 * A hole is a used block between two areas.  Its descriptor lies in a slab.
 * Bit 0 of its begin marks it, so that no begin of a block matches it.
 */
static bool _RBHeap_Is_hole( const RBHeap_descriptor *desc )
{
  return ( desc->begin & 1 ) != 0;
}

/* Returns the begin of the used block or the hole. */
static uintptr_t _RBHeap_Begin( const RBHeap_descriptor *desc )
{
  return desc->begin & ~(uintptr_t) 1;
}

/* Returns the end of the used block or the hole. */
static uintptr_t _RBHeap_End( const RBHeap_descriptor *desc )
{
  return _RBHeap_Begin( desc ) + desc->size;
}

static void _RBHeap_Slab_add( RBHeap_slab **head, RBHeap_slab *slab )
{
  slab->prev = NULL;
  slab->next = *head;

  if ( *head != NULL ) {
    ( *head )->prev = slab;
  }

  *head = slab;
}

static void _RBHeap_Slab_remove( RBHeap_slab **head, RBHeap_slab *slab )
{
  if ( slab->prev != NULL ) {
    slab->prev->next = slab->next;
  } else {
    *head = slab->next;
  }

  if ( slab->next != NULL ) {
    slab->next->prev = slab->prev;
  }
}

/*
 * Adds the slab which lies outside of the blocks of the heap.  The slab stays
 * with the heap.
 */
static void _RBHeap_Slab_add_outside(
  RBHeap_control *control,
  RBHeap_slab    *slab
)
{
  _RBHeap_Slot( slab, 0 )->begin = 0;
  slab->returned = NULL;
  slab->live = 0;
  slab->untouched = 1;
  _RBHeap_Slab_add( &control->partial, slab );
  control->spare += RBHEAP_SLAB_SLOTS - 1;
  control->capacity += RBHEAP_SLAB_SLOTS - 1;
}

static void _RBHeap_Slab_initialize(
  RBHeap_control *control,
  RBHeap_slab    *slab
)
{
  _RBHeap_Slot( slab, 0 )->begin = (uintptr_t) slab;
  slab->returned = NULL;
  slab->live = 0;
  slab->untouched = 1;

  if ( slab == control->home ) {
    _RBHeap_Slab_add( &control->partial, slab );
  } else {
    _RBHeap_Slab_add( &control->empty, slab );
    ++control->empty_count;
  }

  control->spare += RBHEAP_SLAB_SLOTS - 1;
  control->capacity += RBHEAP_SLAB_SLOTS - 1;
}

/* Returns the index of the most significant bit of the non-zero value. */
static uint32_t _RBHeap_Msb( uint32_t value )
{
  uint32_t index;

  index = 0;

  while ( value > 1 ) {
    value >>= 1;
    ++index;
  }

  return index;
}

/*
 * The slab classes below RBHEAP_SLAB_CLASSES are the indices of the occupied
 * lists.
 */
#define RBHEAP_CLASS_EMPTY RBHEAP_SLAB_CLASSES

#define RBHEAP_CLASS_PARTIAL ( RBHEAP_SLAB_CLASSES + 1 )

#define RBHEAP_CLASS_FULL ( RBHEAP_SLAB_CLASSES + 2 )

/*
 * Returns the class of the list of a slab with the count of descriptors in use
 * besides its own.
 */
static uint32_t _RBHeap_Slab_class(
  const RBHeap_control *control,
  const RBHeap_slab    *slab,
  uint32_t              live
)
{
  if ( live == RBHEAP_SLAB_SLOTS - 1 ) {
    return RBHEAP_CLASS_FULL;
  }

  if ( _RBHeap_Is_added( slab ) || slab == control->home ) {
    return RBHEAP_CLASS_PARTIAL;
  }

  if ( live == 0 ) {
    return RBHEAP_CLASS_EMPTY;
  }

  return _RBHeap_Msb( live );
}

/*
 * Returns the list of a slab with the count of descriptors in use besides its
 * own, else NULL for a full slab.
 */
static RBHeap_slab **_RBHeap_Slab_list(
  RBHeap_control *control,
  RBHeap_slab    *slab,
  uint32_t        live
)
{
  uint32_t slab_class;

  slab_class = _RBHeap_Slab_class( control, slab, live );

  if ( slab_class == RBHEAP_CLASS_FULL ) {
    return NULL;
  }

  if ( slab_class == RBHEAP_CLASS_PARTIAL ) {
    return &control->partial;
  }

  if ( slab_class == RBHEAP_CLASS_EMPTY ) {
    return &control->empty;
  }

  return &control->occupied[ slab_class ];
}

/* Moves the slab to the list of its new count of descriptors in use. */
static void _RBHeap_Slab_move(
  RBHeap_control *control,
  RBHeap_slab    *slab,
  uint32_t        live
)
{
  RBHeap_slab **from;
  RBHeap_slab **to;

  from = _RBHeap_Slab_list( control, slab, slab->live );
  to = _RBHeap_Slab_list( control, slab, live );
  slab->live = live;

  if ( from == to ) {
    return;
  }

  if ( from != NULL ) {
    _RBHeap_Slab_remove( from, slab );

    if ( from == &control->empty ) {
      --control->empty_count;
    }
  }

  if ( to != NULL ) {
    _RBHeap_Slab_add( to, slab );

    if ( to == &control->empty ) {
      ++control->empty_count;
    }
  }
}

/*
 * Returns a free descriptor.  An added slab provides it first, and then the
 * fullest slab in an area.  The slabs with few descriptors in use thus empty,
 * so that _RBHeap_Release_slab() can free them.
 */
static RBHeap_descriptor *_RBHeap_Get_descriptor( RBHeap_control *control )
{
  RBHeap_slab       *slab;
  RBHeap_descriptor *desc;
  uint32_t           i;

  _Assert( control->spare > 0 );
  slab = control->partial;

  for ( i = RBHEAP_SLAB_CLASSES; slab == NULL && i > 0; --i ) {
    slab = control->occupied[ i - 1 ];
  }

  if ( slab == NULL ) {
    slab = control->empty;
  }

  desc = slab->returned;

  if ( desc != NULL ) {
    slab->returned = (RBHeap_descriptor *) (uintptr_t) desc->begin;
  } else {
    desc = _RBHeap_Slot( slab, slab->untouched );
    ++slab->untouched;
  }

  _RBHeap_Slab_move( control, slab, slab->live + 1 );
  --control->spare;
  return desc;
}

/*
 * Returns the descriptor to its slab.  A size of zero marks the descriptor as
 * free for _RBHeap_Drain().  A descriptor of the draining slab joins no free
 * list.
 */
static void _RBHeap_Put_descriptor(
  RBHeap_control    *control,
  RBHeap_descriptor *desc
)
{
  RBHeap_slab *slab;

  slab = _RBHeap_Slab_of( desc );
  desc->size = 0;

  if ( slab == control->draining ) {
    --slab->live;
    return;
  }

  desc->begin = (uintptr_t) slab->returned;
  slab->returned = desc;
  _RBHeap_Slab_move( control, slab, slab->live - 1 );
  ++control->spare;
}

static void _RBHeap_Used_insert(
  RBHeap_control    *control,
  RBHeap_descriptor *desc
)
{
  RBHeap_descriptor *parent;
  RBHeap_descriptor *node;
  uintptr_t          begin;
  bool               right;

  begin = desc->begin;
  parent = NULL;
  node = _RBHeap_Root( &control->used_root );
  right = false;

  while ( node != NULL ) {
    parent = node;
    right = begin > node->begin;

    if ( right ) {
      node = _RBHeap_Right( node );
    } else {
      node = _RBHeap_Left( node );
    }
  }

  _RBHeap_Insert( &control->used_root, desc, parent, right );
  ++control->entries;
}

/* Extracts the descriptor from the used tree. */
static void _RBHeap_Used_extract(
  RBHeap_control    *control,
  RBHeap_descriptor *desc
)
{
  _RBHeap_Extract( &control->used_root, desc );
  --control->entries;
}

/* Returns the begin of the free gap before the used descriptor. */
static inline uintptr_t _RBHeap_Gap_begin(
  const RBHeap_control    *control,
  const RBHeap_descriptor *desc
)
{
  const RBHeap_descriptor *prev;

  prev = _RBHeap_Predecessor( desc );

  if ( prev == NULL ) {
    return control->area_begin;
  }

  return _RBHeap_End( prev );
}

/* Returns the end of the free gap after the used descriptor. */
static inline uintptr_t _RBHeap_Gap_end(
  const RBHeap_control    *control,
  const RBHeap_descriptor *desc
)
{
  const RBHeap_descriptor *next;

  next = _RBHeap_Successor( desc );

  if ( next == NULL ) {
    return control->area_end;
  }

  return _RBHeap_Begin( next );
}

/*
 * Returns the used descriptor of the begin, else NULL.  On 64-bit targets, the
 * gap begin, if it is not NULL, and the gap end delimit the gap around the
 * block.  The descent records the ancestors on the path from the root, which
 * are the neighbours of a descriptor without a child on that side.  On 32-bit
 * targets, the function sets neither.
 */
static RBHeap_descriptor *_RBHeap_Used_find(
  const RBHeap_control *control,
  uintptr_t             begin,
  uintptr_t            *gap_begin,
  uintptr_t            *gap_end
)
{
#if UINTPTR_MAX == UINT32_MAX
  RBHeap_descriptor *node;

  (void) gap_begin;
  (void) gap_end;
  node = _RBHeap_Root( &control->used_root );

  while ( node != NULL ) {
    if ( begin == node->begin ) {
      return node;
    }

    if ( begin < node->begin ) {
      node = _RBHeap_Left( node );
    } else {
      node = _RBHeap_Right( node );
    }
  }

  return NULL;
#else
  RBHeap_descriptor *node;
  RBHeap_descriptor *prev;
  RBHeap_descriptor *next;

  node = _RBHeap_Root( &control->used_root );
  prev = NULL;
  next = NULL;

  while ( node != NULL ) {
    RBHeap_descriptor *child;

    if ( begin < node->begin ) {
      next = node;
      node = _RBHeap_Left( node );
      continue;
    }

    if ( begin > node->begin ) {
      prev = node;
      node = _RBHeap_Right( node );
      continue;
    }

    child = _RBHeap_Left( node );

    if ( child != NULL ) {
      do {
        prev = child;
        child = _RBHeap_Right( child );
      } while ( child != NULL );
    }

    child = _RBHeap_Right( node );

    if ( child != NULL ) {
      do {
        next = child;
        child = _RBHeap_Left( child );
      } while ( child != NULL );
    }

    if ( gap_begin != NULL ) {
      *gap_begin = prev != NULL ? _RBHeap_End( prev ) : control->area_begin;
    }

    *gap_end = next != NULL ? _RBHeap_Begin( next ) : control->area_end;
    return node;
  }

  return NULL;
#endif
}

/* Returns true if the key of the size and the begin is below the node. */
static bool _RBHeap_Free_less(
  uintptr_t                size,
  uintptr_t                begin,
  const RBHeap_descriptor *node
)
{
  return size < node->size || ( size == node->size && begin < node->begin );
}

static void _RBHeap_Free_insert(
  RBHeap_control    *control,
  RBHeap_descriptor *desc
)
{
  RBHeap_descriptor *parent;
  RBHeap_descriptor *node;
  uintptr_t          size;
  uintptr_t          begin;
  bool               right;

  size = desc->size;
  begin = desc->begin;
  parent = NULL;
  node = _RBHeap_Root( &control->free_root );
  right = false;

  while ( node != NULL ) {
    parent = node;
    right = !_RBHeap_Free_less( size, begin, node );

    if ( right ) {
      node = _RBHeap_Right( node );
    } else {
      node = _RBHeap_Left( node );
    }
  }

  _RBHeap_Insert( &control->free_root, desc, parent, right );
  ++control->stats.free_blocks;

  if ( control->stats.free_blocks > control->stats.max_free_blocks ) {
    control->stats.max_free_blocks = control->stats.free_blocks;
  }
}

static void _RBHeap_Free_remove(
  RBHeap_control    *control,
  RBHeap_descriptor *desc
)
{
  _RBHeap_Extract( &control->free_root, desc );
  --control->stats.free_blocks;
}

/*
 * Gives the free block the begin and the size.  The block keeps its node if
 * the new key stays between the keys of its neighbours in the free tree.
 */
static void _RBHeap_Free_update(
  RBHeap_control    *control,
  RBHeap_descriptor *desc,
  uintptr_t          begin,
  uintptr_t          size
)
{
  const RBHeap_descriptor *neighbour;

  if ( _RBHeap_Free_less( size, begin, desc ) ) {
    neighbour = _RBHeap_Predecessor( desc );

    if ( neighbour == NULL || !_RBHeap_Free_less( size, begin, neighbour ) ) {
      desc->begin = begin;
      desc->size = size;
      return;
    }
  } else {
    neighbour = _RBHeap_Successor( desc );

    if ( neighbour == NULL || _RBHeap_Free_less( size, begin, neighbour ) ) {
      desc->begin = begin;
      desc->size = size;
      return;
    }
  }

  _RBHeap_Free_remove( control, desc );
  desc->begin = begin;
  desc->size = size;
  _RBHeap_Free_insert( control, desc );
}

/*
 * Returns the free block of the smallest size of at least the size, the one
 * with the lowest begin among the blocks of this size, else NULL.
 */
static RBHeap_descriptor *_RBHeap_Free_find(
  const RBHeap_control *control,
  uintptr_t             size,
  uint32_t             *visits
)
{
  RBHeap_descriptor *node;
  RBHeap_descriptor *best;
  uint32_t           count;

  node = _RBHeap_Root( &control->free_root );
  best = NULL;
  count = 0;

  while ( node != NULL ) {
    ++count;

    if ( size <= node->size ) {
      best = node;
      node = _RBHeap_Left( node );
    } else {
      node = _RBHeap_Right( node );
    }
  }

  *visits += count;
  return best;
}

/*
 * Returns the free block of the size and the begin.  Each caller looks up a gap
 * of the heap, and the free tree holds a block for each gap, see
 * _RBHeap_Walk_has_free().
 */
static RBHeap_descriptor *_RBHeap_Free_lookup(
  const RBHeap_control *control,
  uintptr_t             size,
  uintptr_t             begin
)
{
  RBHeap_descriptor *node;

  node = _RBHeap_Root( &control->free_root );

  while ( true ) {
    _Assert( node != NULL );

    if ( size == node->size && begin == node->begin ) {
      return node;
    }

    if ( _RBHeap_Free_less( size, begin, node ) ) {
      node = _RBHeap_Left( node );
    } else {
      node = _RBHeap_Right( node );
    }
  }
}

/*
 * Returns the free block after the used descriptor, else NULL.  The gap end
 * is the end of the gap after the block.
 */
static RBHeap_descriptor *_RBHeap_Next_free(
  const RBHeap_control    *control,
  const RBHeap_descriptor *desc,
  uintptr_t                gap_end
)
{
  uintptr_t end;

  end = desc->begin + desc->size;

  if ( end == gap_end ) {
    return NULL;
  }

  return _RBHeap_Free_lookup( control, gap_end - end, end );
}

/*
 * Returns true if a used block of the size begins in the free block at a
 * multiple of the alignment.
 */
static bool _RBHeap_Fit(
  const RBHeap_descriptor *free_desc,
  uintptr_t                block_size,
  uintptr_t                alignment,
  uintptr_t               *used_begin
)
{
  uintptr_t begin;
  uintptr_t size;
  uintptr_t front;

  begin = free_desc->begin;
  size = free_desc->size;
  front = ( ( begin + alignment - 1 ) & ~( alignment - 1 ) ) - begin;

  if ( front > size || size - front < block_size ) {
    return false;
  }

  *used_begin = begin + front;
  return true;
}

/*
 * Returns the free block of the best fit which holds a used block of the size
 * at a multiple of the alignment, else NULL.  If the block of the best fit
 * does not hold it, the search leaves room for the alignment.
 */
static RBHeap_descriptor *_RBHeap_Find(
  const RBHeap_control *control,
  uintptr_t             block_size,
  uintptr_t             alignment,
  uintptr_t            *used_begin,
  uint32_t             *visits
)
{
  RBHeap_descriptor *desc;
  bool               fits;

  desc = _RBHeap_Free_find( control, block_size, visits );

  if ( desc == NULL ) {
    return NULL;
  }

  if ( _RBHeap_Fit( desc, block_size, alignment, used_begin ) ) {
    return desc;
  }

  desc = _RBHeap_Free_find(
    control,
    block_size + alignment - RBHEAP_ALIGNMENT,
    visits
  );

  if ( desc == NULL ) {
    return NULL;
  }

  fits = _RBHeap_Fit( desc, block_size, alignment, used_begin );
  _Assert( fits );
  (void) fits;
  return desc;
}

/*
 * Turns the part of the free block which begins at the used begin into a used
 * block of the block size.  The parts before and after it stay free.  The used
 * descriptor describes the used block, else the function takes one.  The
 * caller inserts the used descriptor into the used tree.
 */
static RBHeap_descriptor *_RBHeap_Carve(
  RBHeap_control    *control,
  RBHeap_descriptor *free_desc,
  uintptr_t          used_begin,
  uintptr_t          block_size,
  RBHeap_descriptor *used
)
{
  uintptr_t front;
  uintptr_t rest;

  front = used_begin - free_desc->begin;
  rest = free_desc->size - front - block_size;

  if ( front == 0 ) {
    if ( rest == 0 ) {
      _RBHeap_Free_remove( control, free_desc );

      if ( used == NULL ) {
        used = free_desc;
      } else {
        _RBHeap_Put_descriptor( control, free_desc );
      }
    } else {
      if ( used == NULL ) {
        used = _RBHeap_Get_descriptor( control );
      }

      _RBHeap_Free_update( control, free_desc, used_begin + block_size, rest );
    }
  } else {
    if ( used == NULL ) {
      used = _RBHeap_Get_descriptor( control );
    }

    _RBHeap_Free_update( control, free_desc, free_desc->begin, front );

    if ( rest != 0 ) {
      RBHeap_descriptor *tail;

      tail = _RBHeap_Get_descriptor( control );
      tail->begin = used_begin + block_size;
      tail->size = rest;
      _RBHeap_Free_insert( control, tail );
    }
  }

  used->begin = used_begin;
  used->size = block_size;
  control->free_size -= block_size;
  return used;
}

/*
 * Adds a slab of descriptors.  The slab provides the descriptors of its carve.
 * Returns false if the area has no room for the slab.
 */
static bool _RBHeap_New_slab( RBHeap_control *control )
{
  RBHeap_descriptor *free_desc;
  RBHeap_descriptor *desc;
  uintptr_t          used_begin;
  RBHeap_slab       *slab;
  uint32_t           visits;

  if ( control->slab_source != NULL ) {
    slab = ( *control->slab_source )();

    if ( slab == NULL ) {
      return false;
    }

    _RBHeap_Slab_add_outside( control, slab );
    return true;
  }

  visits = 0;
  free_desc = _RBHeap_Find(
    control,
    RBHEAP_SLAB_SIZE,
    RBHEAP_SLAB_SIZE,
    &used_begin,
    &visits
  );

  if ( free_desc == NULL ) {
    return false;
  }

  slab = (RBHeap_slab *) used_begin;
  _RBHeap_Slab_initialize( control, slab );
  desc = _RBHeap_Carve(
    control,
    free_desc,
    used_begin,
    RBHEAP_SLAB_SIZE,
    _RBHeap_Slot( slab, 0 )
  );
  _RBHeap_Used_insert( control, desc );
  return true;
}

/*
 * Returns true if the capacity holds one more entry of the tree of the used
 * blocks.  With E entries, at most E + 1 free blocks exist, so 2 * E + 1
 * descriptors hold each block.  The heap keeps this capacity for one more
 * entry after each operation.  _RBHeap_Release_slab() uses the same bound, so
 * the count of slabs in the areas depends only on the count of entries.
 */
static bool _RBHeap_Holds_entry( uintptr_t capacity, uintptr_t entries )
{
  return capacity >= 2 * entries + 3;
}

/*
 * Returns the draining slab to the list of its count of descriptors in use.
 * The free slots of the slab form its list of returned descriptors again.
 */
static void _RBHeap_Cancel_drain( RBHeap_control *control )
{
  RBHeap_slab  *slab;
  RBHeap_slab **list;
  uint32_t      i;

  slab = control->draining;
  control->draining = NULL;
  slab->returned = NULL;

  for ( i = 1; i < slab->untouched; ++i ) {
    RBHeap_descriptor *desc;

    desc = _RBHeap_Slot( slab, i );

    if ( desc->size == 0 ) {
      desc->begin = (uintptr_t) slab->returned;
      slab->returned = desc;
    }
  }

  list = _RBHeap_Slab_list( control, slab, slab->live );
  _Assert( list != NULL );
  _RBHeap_Slab_add( list, slab );

  if ( list == &control->empty ) {
    ++control->empty_count;
  }

  control->spare += RBHEAP_SLAB_SLOTS - 1 - slab->live;
  control->capacity += RBHEAP_SLAB_SLOTS - 1;
}

/*
 * Ensures the capacity for the entry of an allocation and for one more entry
 * after it.  The draining slab provides the capacity before a new slab.
 * Returns false if the area has no room for a slab which the capacity needs.
 */
static bool _RBHeap_Reserve_entry( RBHeap_control *control )
{
  while ( !_RBHeap_Holds_entry( control->capacity, control->entries + 1 ) ) {
    if ( control->draining != NULL ) {
      _RBHeap_Cancel_drain( control );
    } else if ( !_RBHeap_New_slab( control ) ) {
      return false;
    }
  }

  return true;
}

/*
 * Cancels the drain if the spare descriptors do not hold the count and the
 * descriptors which the drain needs.  The drain needs one descriptor for each
 * descriptor in use of the draining slab besides its own, and one for the free
 * block of the slab.
 */
static void _RBHeap_Keep_drain( RBHeap_control *control, uintptr_t count )
{
  RBHeap_slab *slab;

  slab = control->draining;

  if ( slab != NULL && control->spare < count + slab->live + 1 ) {
    _RBHeap_Cancel_drain( control );
  }
}

/*
 * Ensures a spare descriptor besides the descriptors which the drain needs.
 * Returns false if the area has no room for a slab which the descriptor needs.
 */
static bool _RBHeap_Reserve_descriptor( RBHeap_control *control )
{
  _RBHeap_Keep_drain( control, 1 );
  return control->spare > 0 || _RBHeap_New_slab( control );
}

#if defined( RTEMS_DEBUG ) || defined( RTEMS_STATIC_ANALYSIS )
/* Returns the count of new descriptors which a carve needs. */
static uintptr_t _RBHeap_Carve_descriptors(
  const RBHeap_descriptor *free_desc,
  uintptr_t                used_begin,
  uintptr_t                block_size
)
{
  uintptr_t front;
  uintptr_t rest;

  front = used_begin - free_desc->begin;
  rest = free_desc->size - front - block_size;

  if ( front == 0 ) {
    return rest == 0 ? 0 : 1;
  }

  return rest == 0 ? 1 : 2;
}
#endif

/*
 * Frees the used block of the descriptor and merges it with its free
 * neighbours.  The gap begin and the gap end delimit the gap around the
 * block.  The descriptor of a slab lies in the freed block.
 */
static void _RBHeap_Merge_gap(
  RBHeap_control    *control,
  RBHeap_descriptor *desc,
  uintptr_t          gap_begin,
  uintptr_t          gap_end,
  bool               is_slab
)
{
  uintptr_t          begin;
  uintptr_t          size;
  uintptr_t          end;
  RBHeap_descriptor *prev;
  RBHeap_descriptor *next;

  begin = desc->begin;
  size = desc->size;
  end = begin + size;
  _RBHeap_Used_extract( control, desc );
  control->free_size += size;

  if ( gap_begin != begin ) {
    prev = _RBHeap_Free_lookup( control, begin - gap_begin, gap_begin );
  } else {
    prev = NULL;
  }

  if ( gap_end != end ) {
    next = _RBHeap_Free_lookup( control, gap_end - end, end );
  } else {
    next = NULL;
  }

  if ( prev != NULL ) {
    if ( next != NULL ) {
      _RBHeap_Free_remove( control, next );
      size += next->size;
      _RBHeap_Put_descriptor( control, next );
    }

    if ( !is_slab ) {
      _RBHeap_Put_descriptor( control, desc );
    }

    _RBHeap_Free_update( control, prev, prev->begin, prev->size + size );
  } else if ( next != NULL ) {
    if ( !is_slab ) {
      _RBHeap_Put_descriptor( control, desc );
    }

    _RBHeap_Free_update( control, next, begin, next->size + size );
  } else {
    if ( is_slab ) {
      desc = _RBHeap_Get_descriptor( control );
      desc->begin = begin;
      desc->size = size;
    }

    _RBHeap_Free_insert( control, desc );
  }
}

/*
 * Frees the used block of the descriptor and merges it with its free
 * neighbours.  The descriptor of a slab lies in the freed block.
 */
static void _RBHeap_Merge(
  RBHeap_control    *control,
  RBHeap_descriptor *desc,
  bool               is_slab
)
{
  _RBHeap_Merge_gap(
    control,
    desc,
    _RBHeap_Gap_begin( control, desc ),
    _RBHeap_Gap_end( control, desc ),
    is_slab
  );
}

/* Returns the slab in an area with the fewest descriptors in use, else NULL. */
static RBHeap_slab *_RBHeap_Least_slab( const RBHeap_control *control )
{
  uint32_t i;

  if ( control->empty != NULL ) {
    return control->empty;
  }

  for ( i = 0; i < RBHEAP_SLAB_CLASSES; ++i ) {
    if ( control->occupied[ i ] != NULL ) {
      return control->occupied[ i ];
    }
  }

  return NULL;
}

/*
 * Moves up to RBHEAP_RELEASE_MOVES descriptors in use of the draining slab,
 * besides its own, to other slabs.  Returns true if the draining slab has no
 * descriptor in use besides its own.
 */
static bool _RBHeap_Drain( RBHeap_control *control, RBHeap_slab *slab )
{
  uint32_t i;
  uint32_t moves;

  i = control->drain_slot;
  moves = 0;

  while ( slab->live > 0 && moves < RBHEAP_RELEASE_MOVES ) {
    RBHeap_descriptor *desc;
    RBHeap_descriptor *other;
    RBTree_Control    *tree;

    _Assert( i < slab->untouched );
    desc = _RBHeap_Slot( slab, i );
    ++i;

    if ( desc->size == 0 ) {
      continue;
    }

    other = _RBHeap_Get_descriptor( control );

    /* Only the root of a tree needs the tree, and the root has no parent. */
    if ( _RBHeap_Root( &control->used_root ) == desc ) {
      tree = &control->used_root;
    } else {
      tree = &control->free_root;
    }

    _RBTree_Replace_node( tree, &desc->node, &other->node );
    other->begin = desc->begin;
    other->size = desc->size;
    desc->size = 0;
    --slab->live;
    ++moves;
  }

  control->drain_slot = i;
  return slab->live == 0;
}

/*
 * Empties and frees the slabs in the areas which the capacity for one more
 * entry does not need, see _RBHeap_Holds_entry().  The slab with the fewest
 * descriptors in use goes first.  One call moves up to RBHEAP_RELEASE_MOVES of
 * its descriptors in use to other slabs.  The home slab stays.
 */
static void _RBHeap_Release_slab( RBHeap_control *control )
{
  RBHeap_slab *slab;

  slab = control->draining;

  if ( slab == NULL ) {
    RBHeap_slab **list;

    /* Each slab in an area is an entry, so the capacity holds its slots. */
    if (
      control->entries == 0 || control->capacity < RBHEAP_SLAB_SLOTS - 1 ||
      !_RBHeap_Holds_entry(
        control->capacity - ( RBHEAP_SLAB_SLOTS - 1 ),
        control->entries - 1
      )
    ) {
      return;
    }

    slab = _RBHeap_Least_slab( control );

    if ( slab == NULL ) {
      return;
    }

    list = _RBHeap_Slab_list( control, slab, slab->live );
    _RBHeap_Slab_remove( list, slab );

    if ( list == &control->empty ) {
      --control->empty_count;
    }

    control->spare -= RBHEAP_SLAB_SLOTS - 1 - slab->live;
    control->capacity -= RBHEAP_SLAB_SLOTS - 1;
    control->draining = slab;
    control->drain_slot = 1;
  }

  if ( _RBHeap_Drain( control, slab ) ) {
    control->draining = NULL;
    _RBHeap_Merge( control, _RBHeap_Slot( slab, 0 ), true );
  }
}

/*
 * Returns the used block of the begin, else NULL.  A slab and a hole are no
 * used block.  On 64-bit targets, the gap begin, if it is not NULL, and the
 * gap end delimit the gap around the block.  On 32-bit targets, the function
 * sets neither.
 */
static inline RBHeap_descriptor *_RBHeap_Get_used(
  const RBHeap_control *control,
  void                 *begin,
  uintptr_t            *gap_begin,
  uintptr_t            *gap_end
)
{
  RBHeap_descriptor *desc;

  desc = _RBHeap_Used_find( control, (uintptr_t) begin, gap_begin, gap_end );

  if ( desc == NULL || _RBHeap_Is_own( desc ) ) {
    return NULL;
  }

  return desc;
}

/* Returns the used descriptor of the greatest begin up to the begin. */
static RBHeap_descriptor *_RBHeap_Used_floor(
  const RBHeap_control *control,
  uintptr_t             begin
)
{
  RBHeap_descriptor *node;
  RBHeap_descriptor *floor;

  node = _RBHeap_Root( &control->used_root );
  floor = NULL;

  while ( node != NULL ) {
    if ( _RBHeap_Begin( node ) <= begin ) {
      floor = node;
      node = _RBHeap_Right( node );
    } else {
      node = _RBHeap_Left( node );
    }
  }

  return floor;
}

/* Returns the hole which holds the range, else NULL. */
static RBHeap_descriptor *_RBHeap_Hole_of(
  const RBHeap_control *control,
  uintptr_t             begin,
  uintptr_t             end
)
{
  RBHeap_descriptor *hole;

  hole = _RBHeap_Used_floor( control, begin );

  if (
    hole == NULL || !_RBHeap_Is_hole( hole ) || end > _RBHeap_End( hole )
  ) {
    return NULL;
  }

  return hole;
}

/* Returns true if the heap holds no used block besides at most one slab. */
static bool _RBHeap_Is_unused( const RBHeap_control *control )
{
  const RBHeap_descriptor *root;

  root = _RBHeap_Root( &control->used_root );
  return root == NULL ||
         ( _RBHeap_Left( root ) == NULL && _RBHeap_Right( root ) == NULL &&
           _RBHeap_Is_slab( root ) );
}

/*
 * Empties the trees of a heap without a used block.  Each descriptor of a free
 * block returns to its slab.  The slab of the heap, if one exists, leaves the
 * slab lists.
 */
static void _RBHeap_Clear( RBHeap_control *control )
{
  RBHeap_descriptor *desc;
  uint32_t           visits;

  visits = 0;

  while ( ( desc = _RBHeap_Free_find( control, 1, &visits ) ) != NULL ) {
    _RBHeap_Free_remove( control, desc );
    _RBHeap_Put_descriptor( control, desc );
  }

  desc = _RBHeap_Root( &control->used_root );

  if ( desc != NULL ) {
    RBHeap_slab *slab;

    slab = (RBHeap_slab *) desc->begin;
    _Assert( slab->live == 0 );
    if ( slab == control->draining ) {
      control->draining = NULL;
    } else {
      if ( slab == control->home ) {
        _RBHeap_Slab_remove( &control->partial, slab );
        control->home = NULL;
      } else {
        _RBHeap_Slab_remove( &control->empty, slab );
        --control->empty_count;
      }

      control->spare -= RBHEAP_SLAB_SLOTS - 1;
      control->capacity -= RBHEAP_SLAB_SLOTS - 1;
    }

    _RBTree_Initialize_empty( &control->used_root );
    control->entries = 0;
  }
}

bool _RBHeap_Add_descriptors(
  RBHeap_control *control,
  void           *area_begin,
  uintptr_t       area_size
)
{
  uintptr_t begin;
  uintptr_t end;
  uintptr_t slab_begin;
  bool      unused;

  begin = (uintptr_t) area_begin;
  end = ( begin + area_size ) & ~( RBHEAP_SLAB_SIZE - 1 );
  slab_begin = ( begin + RBHEAP_SLAB_SIZE - 1 ) & ~( RBHEAP_SLAB_SIZE - 1 );

  /*
   * If the end of the area wraps, either the begin of the slab wraps too, or
   * the end lies below it.
   */
  if ( slab_begin < begin || slab_begin >= end ) {
    return false;
  }

  if (
    slab_begin < control->area_end && end > control->area_begin &&
    _RBHeap_Hole_of( control, slab_begin, end ) == NULL
  ) {
    return false;
  }

  unused = _RBHeap_Is_unused( control );

  if ( unused ) {
    _RBHeap_Clear( control );
  }

  if ( control->added_begin == control->added_end ) {
    control->added_begin = slab_begin;
    control->added_end = end;
  } else {
    if ( slab_begin < control->added_begin ) {
      control->added_begin = slab_begin;
    }

    if ( end > control->added_end ) {
      control->added_end = end;
    }
  }

  for ( ; slab_begin < end; slab_begin += RBHEAP_SLAB_SIZE ) {
    RBHeap_slab *slab;

    slab = (RBHeap_slab *) slab_begin;
    _RBHeap_Slab_add_outside( control, slab );
  }

  if ( unused ) {
    RBHeap_descriptor *desc;

    control->free_size = control->area_end - control->area_begin;
    desc = _RBHeap_Get_descriptor( control );
    desc->begin = control->area_begin;
    desc->size = control->free_size;
    _RBHeap_Free_insert( control, desc );
  }

  return true;
}

/* Returns the free block which ends at the end, else NULL. */
static RBHeap_descriptor *_RBHeap_Free_before(
  const RBHeap_control *control,
  uintptr_t             end
)
{
  const RBHeap_descriptor *prev;
  uintptr_t                begin;

  prev = _RBHeap_Used_floor( control, end - 1 );
  begin = prev != NULL ? _RBHeap_End( prev ) : control->area_begin;

  if ( begin == end ) {
    return NULL;
  }

  return _RBHeap_Free_lookup( control, end - begin, begin );
}

/* Returns the free block which begins at the begin, else NULL. */
static RBHeap_descriptor *_RBHeap_Free_after(
  const RBHeap_control *control,
  uintptr_t             begin
)
{
  const RBHeap_descriptor *prev;
  const RBHeap_descriptor *next;
  uintptr_t                end;

  prev = _RBHeap_Used_floor( control, begin );

  if ( prev != NULL ) {
    if ( _RBHeap_Begin( prev ) == begin ) {
      return NULL;
    }

    next = _RBHeap_Successor( prev );
  } else {
    next = _RBHeap_Root( &control->used_root );

    if ( next != NULL ) {
      const RBHeap_descriptor *left;

      while ( ( left = _RBHeap_Left( next ) ) != NULL ) {
        next = left;
      }
    }
  }

  end = next != NULL ? _RBHeap_Begin( next ) : control->area_end;
  return _RBHeap_Free_lookup( control, end - begin, begin );
}

/* Inserts the hole into the used tree.  A spare descriptor exists. */
static void _RBHeap_Hole_insert(
  RBHeap_control *control,
  uintptr_t       begin,
  uintptr_t       end
)
{
  RBHeap_descriptor *hole;

  hole = _RBHeap_Get_descriptor( control );
  hole->begin = begin | 1;
  hole->size = end - begin;
  _RBHeap_Used_insert( control, hole );
}
bool _RBHeap_Extend(
  RBHeap_control *control,
  void           *area_begin,
  uintptr_t       area_size
)
{
  RBHeap_descriptor *hole;
  RBHeap_descriptor *below;
  RBHeap_descriptor *above;
  RBHeap_slab       *slab;
  uintptr_t          begin;
  uintptr_t          end;
  uintptr_t          size;
  uintptr_t          lower_begin;
  uintptr_t          upper_end;
  uintptr_t          need;
  bool               merge_below;
  bool               merge_above;

  begin = (uintptr_t) area_begin;

  if ( area_size > RBHEAP_MAX_SIZE ) {
    area_size = RBHEAP_MAX_SIZE;
  }

  end = ( begin + area_size ) & ~( RBHEAP_ALIGNMENT - 1 );
  begin = ( begin + RBHEAP_ALIGNMENT - 1 ) & ~( RBHEAP_ALIGNMENT - 1 );

  if ( begin < (uintptr_t) area_begin || end <= begin ) {
    return false;
  }

  if ( begin < control->added_end && end > control->added_begin ) {
    return false;
  }

  /*
   * The area leaves a hole from the lower begin to its begin and a hole from
   * its end to the upper end.  Each hole may be empty.
   */
  if ( begin >= control->area_end ) {
    hole = NULL;
    lower_begin = control->area_end;
    upper_end = end;
    merge_below = ( lower_begin == begin );
    merge_above = false;
  } else if ( end <= control->area_begin ) {
    hole = NULL;
    lower_begin = begin;
    upper_end = control->area_begin;
    merge_below = false;
    merge_above = ( upper_end == end );
  } else {
    hole = _RBHeap_Hole_of( control, begin, end );

    if ( hole == NULL ) {
      return false;
    }

    lower_begin = _RBHeap_Begin( hole );
    upper_end = _RBHeap_End( hole );
    merge_below = ( lower_begin == begin );
    merge_above = ( upper_end == end );
  }

  below = merge_below ? _RBHeap_Free_before( control, begin ) : NULL;
  above = merge_above ? _RBHeap_Free_after( control, end ) : NULL;

  /*
   * A new hole needs a descriptor.  A hole of the heap provides the descriptor
   * of one of the holes which the area leaves.  The area needs a descriptor if
   * it merges with no free block.
   */
  need = 0;

  if ( lower_begin != begin ) {
    ++need;
  }

  if ( upper_end != end ) {
    ++need;
  }

  if ( hole != NULL && need > 0 ) {
    --need;
  }

  if ( below == NULL && above == NULL ) {
    ++need;
  }

  slab = NULL;
  _RBHeap_Keep_drain( control, need );

  if ( control->spare < need ) {
    uintptr_t slab_begin;

    slab_begin = ( begin + RBHEAP_SLAB_SIZE - 1 ) & ~( RBHEAP_SLAB_SIZE - 1 );

    if (
      control->slab_source == NULL && slab_begin >= begin &&
      slab_begin <= end && end - slab_begin >= RBHEAP_SLAB_SIZE
    ) {
      slab = (RBHeap_slab *) slab_begin;
      _RBHeap_Slab_initialize( control, slab );
    } else if ( _RBHeap_New_slab( control ) ) {
      /* The slab may come from a free block next to the area. */
      below = merge_below ? _RBHeap_Free_before( control, begin ) : NULL;
      above = merge_above ? _RBHeap_Free_after( control, end ) : NULL;
    } else {
      return false;
    }
  }

  if ( hole != NULL ) {
    if ( lower_begin != begin ) {
      hole->size = begin - lower_begin;

      if ( upper_end != end ) {
        _RBHeap_Hole_insert( control, end, upper_end );
      }
    } else if ( upper_end != end ) {
      /* The hole keeps its place in the used tree. */
      hole->begin = end | 1;
      hole->size = upper_end - end;
    } else {
      _RBHeap_Used_extract( control, hole );
      _RBHeap_Put_descriptor( control, hole );
    }
  } else if ( begin >= control->area_end ) {
    if ( lower_begin != begin ) {
      _RBHeap_Hole_insert( control, lower_begin, begin );
    }

    control->area_end = end;
  } else {
    if ( upper_end != end ) {
      _RBHeap_Hole_insert( control, end, upper_end );
    }

    control->area_begin = begin;
  }

  size = end - begin;
  control->free_size += size;
  control->stats.size += size;
  control->stats.min_free_size += size;

  if ( below != NULL ) {
    if ( above != NULL ) {
      size += above->size;
      _RBHeap_Free_remove( control, above );
      _RBHeap_Put_descriptor( control, above );
    }

    above = below;
    _RBHeap_Free_update( control, below, below->begin, below->size + size );
  } else if ( above != NULL ) {
    _RBHeap_Free_update( control, above, begin, above->size + size );
  } else {
    above = _RBHeap_Get_descriptor( control );
    above->begin = begin;
    above->size = size;
    _RBHeap_Free_insert( control, above );
  }

  if ( slab != NULL ) {
    RBHeap_descriptor *desc;

    desc = _RBHeap_Carve(
      control,
      above,
      (uintptr_t) slab,
      RBHEAP_SLAB_SIZE,
      _RBHeap_Slot( slab, 0 )
    );
    _RBHeap_Used_insert( control, desc );
  }

  _RBHeap_Update_min_free_size( control );
  return true;
}

bool _RBHeap_Initialize(
  RBHeap_control *control,
  void           *area_begin,
  uintptr_t       area_size
)
{
  uintptr_t          begin;
  uintptr_t          end;
  uintptr_t          slab_begin;
  RBHeap_slab       *slab;
  RBHeap_descriptor *desc;

  begin = (uintptr_t) area_begin;

  if ( area_size > RBHEAP_MAX_SIZE ) {
    area_size = RBHEAP_MAX_SIZE;
  }

  end = ( begin + area_size ) & ~( RBHEAP_ALIGNMENT - 1 );
  begin = ( begin + RBHEAP_ALIGNMENT - 1 ) & ~( RBHEAP_ALIGNMENT - 1 );
  slab_begin = ( begin + RBHEAP_SLAB_SIZE - 1 ) & ~( RBHEAP_SLAB_SIZE - 1 );

  if (
    end < begin || slab_begin < begin || slab_begin > end ||
    end - slab_begin < RBHEAP_SLAB_SIZE
  ) {
    return false;
  }

  memset( control, 0, sizeof( *control ) );
  control->area_begin = begin;
  control->area_end = end;
  control->free_size = end - begin;

  slab = (RBHeap_slab *) slab_begin;
  control->home = slab;
  _RBHeap_Slab_initialize( control, slab );
  desc = _RBHeap_Get_descriptor( control );
  desc->begin = begin;
  desc->size = end - begin;
  _RBHeap_Free_insert( control, desc );
  desc = _RBHeap_Carve(
    control,
    desc,
    slab_begin,
    RBHEAP_SLAB_SIZE,
    _RBHeap_Slot( slab, 0 )
  );
  _RBHeap_Used_insert( control, desc );
  control->stats.size = end - begin;
  control->stats.min_free_size = control->free_size;
  return true;
}

bool _RBHeap_Initialize_with_slab(
  RBHeap_control    *control,
  void              *area_begin,
  uintptr_t          area_size,
  void              *slab,
  RBHeap_slab_source slab_source
)
{
  uintptr_t          begin;
  uintptr_t          end;
  RBHeap_descriptor *desc;

  begin = (uintptr_t) area_begin;

  if ( area_size > RBHEAP_MAX_SIZE ) {
    area_size = RBHEAP_MAX_SIZE;
  }

  end = ( begin + area_size ) & ~( RBHEAP_ALIGNMENT - 1 );
  begin = ( begin + RBHEAP_ALIGNMENT - 1 ) & ~( RBHEAP_ALIGNMENT - 1 );

  if ( begin < (uintptr_t) area_begin || end <= begin ) {
    return false;
  }

  memset( control, 0, sizeof( *control ) );
  control->area_begin = begin;
  control->area_end = end;
  control->free_size = end - begin;
  control->slab_source = slab_source;
  _RBHeap_Slab_add_outside( control, slab );
  desc = _RBHeap_Get_descriptor( control );
  desc->begin = begin;
  desc->size = end - begin;
  _RBHeap_Free_insert( control, desc );
  control->stats.size = end - begin;
  control->stats.min_free_size = control->free_size;
  return true;
}

void *_RBHeap_Allocate( RBHeap_control *control, uintptr_t size )
{
  return _RBHeap_Allocate_aligned( control, size, RBHEAP_ALIGNMENT );
}

void *_RBHeap_Allocate_aligned(
  RBHeap_control *control,
  uintptr_t       size,
  uintptr_t       alignment
)
{
  uintptr_t          block_size;
  uintptr_t          used_begin;
  RBHeap_descriptor *desc;
  uint32_t           visits;

  if ( size > RBHEAP_MAX_SIZE || alignment > RBHEAP_MAX_SIZE ) {
    ++control->stats.failed_allocs;
    return NULL;
  }

  if ( size < RBHEAP_MIN_SIZE ) {
    size = RBHEAP_MIN_SIZE;
  }

  size += RBHEAP_GUARD_SIZE;

  if ( alignment < RBHEAP_ALIGNMENT ) {
    alignment = RBHEAP_ALIGNMENT;
  }

  block_size = ( size + RBHEAP_ALIGNMENT - 1 ) & ~( RBHEAP_ALIGNMENT - 1 );

  visits = 0;

  if ( _RBHeap_Reserve_entry( control ) ) {
    desc = _RBHeap_Find( control, block_size, alignment, &used_begin, &visits );
  } else {
    desc = NULL;
  }

  _Assert(
    desc == NULL ||
    control->spare >= _RBHeap_Carve_descriptors( desc, used_begin, block_size )
  );
  ++control->stats.searches;

  if ( visits > control->stats.max_search ) {
    control->stats.max_search = visits;
  }

  if ( desc == NULL ) {
    ++control->stats.failed_allocs;
    return NULL;
  }

  desc = _RBHeap_Carve( control, desc, used_begin, block_size, NULL );
  _RBHeap_Used_insert( control, desc );
  _RBHeap_Guard_set( used_begin, block_size );
  ++control->stats.allocs;
  ++control->stats.used_blocks;
  control->stats.lifetime_allocated += block_size;
  _RBHeap_Update_min_free_size( control );
  return (void *) used_begin;
}

static RBHeap_resize_status _RBHeap_Do_resize(
  RBHeap_control *control,
  void           *begin,
  uintptr_t       size,
  uintptr_t      *old_size
)
{
  RBHeap_descriptor *desc;
  RBHeap_descriptor *next;
  uintptr_t          block_size;
  uintptr_t          current_size;
  uintptr_t          gap_end;

  desc = _RBHeap_Get_used( control, begin, NULL, &gap_end );

  if ( desc == NULL || !_RBHeap_Guard_is_intact( desc->begin, desc->size ) ) {
    *old_size = 0;
    return RBHEAP_RESIZE_INVALID_BLOCK;
  }

  current_size = desc->size;
  *old_size = current_size - RBHEAP_GUARD_SIZE;

  if ( size > RBHEAP_MAX_SIZE ) {
    return RBHEAP_RESIZE_UNSATISFIED;
  }

  if ( size < RBHEAP_MIN_SIZE ) {
    size = RBHEAP_MIN_SIZE;
  }

  size += RBHEAP_GUARD_SIZE;
  block_size = ( size + RBHEAP_ALIGNMENT - 1 ) & ~( RBHEAP_ALIGNMENT - 1 );

  if ( block_size == current_size ) {
    return RBHEAP_RESIZE_SUCCESSFUL;
  }

#if UINTPTR_MAX == UINT32_MAX
  gap_end = _RBHeap_Gap_end( control, desc );
#endif

  next = _RBHeap_Next_free( control, desc, gap_end );

  if ( block_size > current_size ) {
    uintptr_t need;

    need = block_size - current_size;

    if ( next == NULL || next->size < need ) {
      return RBHEAP_RESIZE_UNSATISFIED;
    }

    if ( next->size == need ) {
      _RBHeap_Free_remove( control, next );
      _RBHeap_Put_descriptor( control, next );
    } else {
      _RBHeap_Free_update(
        control,
        next,
        next->begin + need,
        next->size - need
      );
    }

    desc->size = block_size;
    control->free_size -= need;
    control->stats.lifetime_allocated += need;
    _RBHeap_Guard_set( desc->begin, block_size );
    _RBHeap_Release_slab( control );
  } else {
    uintptr_t rest;

    rest = current_size - block_size;

    if ( next != NULL ) {
      _RBHeap_Free_update(
        control,
        next,
        next->begin - rest,
        next->size + rest
      );
    } else if ( _RBHeap_Reserve_descriptor( control ) ) {
      RBHeap_descriptor *tail;

      tail = _RBHeap_Get_descriptor( control );
      tail->begin = desc->begin + block_size;
      tail->size = rest;
      _RBHeap_Free_insert( control, tail );
    } else {
      return RBHEAP_RESIZE_SUCCESSFUL;
    }

    desc->size = block_size;
    control->free_size += rest;
    control->stats.lifetime_freed += rest;
    _RBHeap_Guard_set( desc->begin, block_size );
  }

  return RBHEAP_RESIZE_SUCCESSFUL;
}

RBHeap_resize_status _RBHeap_Resize(
  RBHeap_control *control,
  void           *begin,
  uintptr_t       size,
  uintptr_t      *old_size
)
{
  RBHeap_resize_status status;

  status = _RBHeap_Do_resize( control, begin, size, old_size );

  if ( status == RBHEAP_RESIZE_SUCCESSFUL ) {
    ++control->stats.resizes;
    _RBHeap_Update_min_free_size( control );
  }

  return status;
}

bool _RBHeap_Free( RBHeap_control *control, void *begin )
{
  RBHeap_descriptor *desc;
  uintptr_t          gap_begin;
  uintptr_t          gap_end;

  if ( begin == NULL ) {
    return true;
  }

  desc = _RBHeap_Get_used( control, begin, &gap_begin, &gap_end );

  if ( desc == NULL || !_RBHeap_Guard_is_intact( desc->begin, desc->size ) ) {
    return false;
  }

  ++control->stats.frees;
  --control->stats.used_blocks;
  control->stats.lifetime_freed += desc->size;

#if UINTPTR_MAX == UINT32_MAX
  _RBHeap_Merge( control, desc, false );
#else
  _RBHeap_Merge_gap( control, desc, gap_begin, gap_end, false );
#endif
  _RBHeap_Release_slab( control );
  return true;
}

void *_RBHeap_Allocate_aligned_with_boundary(
  RBHeap_control *control,
  uintptr_t       size,
  uintptr_t       alignment,
  uintptr_t       boundary
)
{
  if (
    ( alignment & ( alignment - 1 ) ) != 0 ||
    ( boundary & ( boundary - 1 ) ) != 0
  ) {
    ++control->stats.failed_allocs;
    return NULL;
  }

  if ( boundary != 0 ) {
    if ( boundary < alignment || size > boundary ) {
      ++control->stats.failed_allocs;
      return NULL;
    }

    alignment = boundary;
  }

  return _RBHeap_Allocate_aligned( control, size, alignment );
}

bool _RBHeap_Size_of_block(
  const RBHeap_control *control,
  const void           *block,
  uintptr_t            *size
)
{
  RBHeap_descriptor *desc;
  uintptr_t          gap_begin;
  uintptr_t          gap_end;

  desc = _RBHeap_Get_used( control, (void *) block, &gap_begin, &gap_end );

  if ( desc == NULL ) {
    return false;
  }

  *size = desc->size - RBHEAP_GUARD_SIZE;
  return true;
}

static RBHeap_descriptor *_RBHeap_First( const RBHeap_descriptor *root )
{
  RBHeap_descriptor *node;
  RBHeap_descriptor *left;

  node = (RBHeap_descriptor *) root;

  if ( node == NULL ) {
    return NULL;
  }

  while ( ( left = _RBHeap_Left( node ) ) != NULL ) {
    node = left;
  }

  return node;
}

static void _RBHeap_Add_information( Heap_Information *info, uintptr_t size )
{
  ++info->number;
  info->total += size;

  if ( size > info->largest ) {
    info->largest = size;
  }
}

void _RBHeap_Get_information(
  const RBHeap_control   *control,
  Heap_Information_block *info
)
{
  const RBHeap_descriptor *node;

  memset( info, 0, sizeof( *info ) );
  info->Stats = control->stats;
  info->Stats.free_size = control->free_size;

  node = _RBHeap_First( _RBHeap_Root( &control->free_root ) );

  while ( node != NULL ) {
    _RBHeap_Add_information( &info->Free, node->size );
    node = _RBHeap_Successor( node );
  }

  node = _RBHeap_First( _RBHeap_Root( &control->used_root ) );

  while ( node != NULL ) {
    if ( !_RBHeap_Is_hole( node ) && !_RBHeap_Is_own( node ) ) {
      _RBHeap_Add_information( &info->Used, node->size );
    }

    node = _RBHeap_Successor( node );
  }
}

/* Returns true if the tree of the root is a valid red-black tree. */
static bool _RBHeap_Walk_tree( const RBHeap_descriptor *root )
{
  const RBHeap_descriptor *node;
  uint32_t                 black_height;

  if ( root == NULL ) {
    return true;
  }

  if ( _RBHeap_Parent( root ) != NULL || _RBHeap_Is_red( root ) ) {
    return false;
  }

  black_height = 0;
  node = _RBHeap_First( root );

  while ( node != NULL ) {
    const RBHeap_descriptor *left;
    const RBHeap_descriptor *right;

    left = _RBHeap_Left( node );
    right = _RBHeap_Right( node );

    /*
     * The walk reaches a node with a left child only through the parent of the
     * left child, see _RBTree_Successor().
     */
    if ( right != NULL && _RBHeap_Parent( right ) != node ) {
      return false;
    }

    if (
      _RBHeap_Is_red( node ) &&
      ( ( left != NULL && _RBHeap_Is_red( left ) ) ||
        ( right != NULL && _RBHeap_Is_red( right ) ) )
    ) {
      return false;
    }

    if ( left == NULL || right == NULL ) {
      const RBHeap_descriptor *up;
      uint32_t                 height;

      height = 0;
      up = node;

      while ( up != NULL ) {
        if ( !_RBHeap_Is_red( up ) ) {
          ++height;
        }

        up = _RBHeap_Parent( up );
      }

      if ( black_height == 0 ) {
        black_height = height;
      } else if ( height != black_height ) {
        return false;
      }
    }

    node = _RBHeap_Successor( node );
  }

  return true;
}

/* Returns true if the free tree holds the free block of the size and begin. */
static bool _RBHeap_Walk_has_free(
  const RBHeap_control *control,
  uintptr_t             size,
  uintptr_t             begin
)
{
  const RBHeap_descriptor *node;

  node = _RBHeap_Root( &control->free_root );

  while ( node != NULL ) {
    if ( size == node->size && begin == node->begin ) {
      return true;
    }

    if ( _RBHeap_Free_less( size, begin, node ) ) {
      node = _RBHeap_Left( node );
    } else {
      node = _RBHeap_Right( node );
    }
  }

  return false;
}

/* The counts of a walk of the slabs. */
typedef struct {
  /* The count of descriptors of the trees in the slabs. */
  uintptr_t descriptors;

  /* The count of descriptors in use of the slabs, besides their own. */
  uintptr_t live;

  /* The count of free descriptors of the slabs on the slab lists. */
  uintptr_t spare;

  /* The count of slabs on the slab lists and of the full slabs. */
  uintptr_t slabs;

  /* The count of slabs inside the areas besides the draining slab. */
  uintptr_t area_slabs;

  /* The count of slabs in the empty list. */
  uintptr_t empty;
} RBHeap_Walk_counts;

/*
 * Returns the index of the slot of the slab at the descriptor.  A descriptor
 * between two slots gives the index of the slot below it.
 */
static uintptr_t _RBHeap_Walk_index(
  const RBHeap_slab       *slab,
  const RBHeap_descriptor *desc
)
{
  return ( (uintptr_t) desc - (uintptr_t) _RBHeap_Const_slot( slab, 0 ) ) /
         sizeof( *desc );
}

/*
 * Returns true if the descriptors in use of the slab match its count.  Each
 * descriptor in use has a size other than zero.  A slab which is not the
 * draining slab lists each free descriptor below its first untouched one.
 */
static bool _RBHeap_Walk_slab(
  const RBHeap_control *control,
  const RBHeap_slab    *slab
)
{
  const RBHeap_descriptor *desc;
  uintptr_t                live;
  uintptr_t                returned;
  uintptr_t                i;

  if ( slab->untouched == 0 || slab->untouched > RBHEAP_SLAB_SLOTS ) {
    return false;
  }

  live = 0;

  for ( i = 1; i < slab->untouched; ++i ) {
    if ( _RBHeap_Const_slot( slab, i )->size != 0 ) {
      ++live;
    }
  }

  if ( live != slab->live ) {
    return false;
  }

  if ( slab == control->draining ) {
    return true;
  }

  returned = 0;
  desc = slab->returned;

  while ( desc != NULL ) {
    uintptr_t index;

    index = _RBHeap_Walk_index( slab, desc );

    if (
      returned == slab->untouched - 1 - live || _RBHeap_Slab_of( desc ) != slab ||
      index == 0 || index >= slab->untouched ||
      _RBHeap_Const_slot( slab, index ) != desc || desc->size != 0
    ) {
      return false;
    }

    ++returned;
    desc = (const RBHeap_descriptor *) desc->begin;
  }

  return returned == slab->untouched - 1 - live;
}

/* Adds the slab to the counts. */
static bool _RBHeap_Walk_count_slab(
  const RBHeap_control *control,
  const RBHeap_slab    *slab,
  RBHeap_Walk_counts   *counts
)
{
  if ( !_RBHeap_Walk_slab( control, slab ) ) {
    return false;
  }

  counts->live += slab->live;
  counts->spare += RBHEAP_SLAB_SLOTS - 1 - slab->live;
  ++counts->slabs;

  if ( !_RBHeap_Is_added( slab ) ) {
    ++counts->area_slabs;
  }

  return true;
}

/*
 * Returns true if the descriptor lies below the first untouched descriptor of
 * its slab.  The first descriptor of a full slab counts the slab.
 */
static bool _RBHeap_Walk_slot(
  const RBHeap_control    *control,
  const RBHeap_descriptor *desc,
  RBHeap_Walk_counts      *counts
)
{
  const RBHeap_slab *slab;
  uintptr_t          index;

  slab = _RBHeap_Slab_of( desc );
  index = _RBHeap_Walk_index( slab, desc );

  if (
    index == 0 || index >= slab->untouched ||
    _RBHeap_Const_slot( slab, index ) != desc
  ) {
    return false;
  }

  ++counts->descriptors;

  if ( index == 1 && slab->live == RBHEAP_SLAB_SLOTS - 1 ) {
    return _RBHeap_Walk_count_slab( control, slab, counts );
  }

  return true;
}

/* Adds the slabs of the list of the class to the counts. */
static bool _RBHeap_Walk_list(
  const RBHeap_control *control,
  const RBHeap_slab    *slab,
  uint32_t              slab_class,
  RBHeap_Walk_counts   *counts
)
{
  const RBHeap_slab *prev;

  prev = NULL;

  while ( slab != NULL ) {
    if (
      slab->prev != prev || slab == control->draining ||
      _RBHeap_Slab_class( control, slab, slab->live ) != slab_class ||
      counts->slabs >= control->capacity / ( RBHEAP_SLAB_SLOTS - 1 ) ||
      !_RBHeap_Walk_count_slab( control, slab, counts )
    ) {
      return false;
    }

    if ( slab_class == RBHEAP_CLASS_EMPTY ) {
      ++counts->empty;
    }

    prev = slab;
    slab = slab->next;
  }

  return true;
}

/*
 * Returns true if the draining slab is valid.  It lies inside an area, no
 * descriptor in use lies below the next slot of the drain, and the spare
 * descriptors hold the drain.
 */
static bool _RBHeap_Walk_draining( const RBHeap_control *control )
{
  const RBHeap_slab *slab;
  uint32_t           i;

  slab = control->draining;

  if ( slab == NULL ) {
    return true;
  }

  if (
    _RBHeap_Is_added( slab ) || slab == control->home ||
    !_RBHeap_Walk_slab( control, slab ) || control->drain_slot == 0 ||
    control->drain_slot > slab->untouched ||
    control->spare < (uintptr_t) slab->live + 1
  ) {
    return false;
  }

  for ( i = 1; i < control->drain_slot; ++i ) {
    if ( _RBHeap_Const_slot( slab, i )->size != 0 ) {
      return false;
    }
  }

  return true;
}

/*
 * Returns true if the slabs hold each descriptor of the trees and if the
 * counts of the control match the slabs.  The function also checks that the
 * spare descriptors hold the drain.
 */
static bool _RBHeap_Walk_slabs(
  const RBHeap_control *control,
  RBHeap_Walk_counts   *counts,
  uintptr_t             own
)
{
  uint32_t  i;
  uintptr_t draining_live;
  uintptr_t draining;

  if ( control->home != NULL && _RBHeap_Is_added( control->home ) ) {
    return false;
  }

  if ( !_RBHeap_Walk_draining( control ) ) {
    return false;
  }

  if ( control->draining != NULL ) {
    draining_live = control->draining->live;
    draining = 1;
  } else {
    draining_live = 0;
    draining = 0;
  }

  if (
    !_RBHeap_Walk_list(
      control,
      control->partial,
      RBHEAP_CLASS_PARTIAL,
      counts
    ) ||
    !_RBHeap_Walk_list( control, control->empty, RBHEAP_CLASS_EMPTY, counts )
  ) {
    return false;
  }

  for ( i = 0; i < RBHEAP_SLAB_CLASSES; ++i ) {
    if ( !_RBHeap_Walk_list( control, control->occupied[ i ], i, counts ) ) {
      return false;
    }
  }

  return counts->descriptors == counts->live + draining_live &&
         counts->spare == control->spare &&
         counts->slabs * ( RBHEAP_SLAB_SLOTS - 1 ) == control->capacity &&
         counts->empty == control->empty_count &&
         counts->area_slabs + draining == own &&
         ( own == 0 || control->slab_source == NULL );
}

bool _RBHeap_Walk( const RBHeap_control *control )
{
  const RBHeap_descriptor *node;
  const RBHeap_descriptor *previous;
  RBHeap_Walk_counts       counts;
  uintptr_t                end;
  uintptr_t                free_size;
  uintptr_t                free_blocks;
  uintptr_t                free_nodes;
  uintptr_t                used_blocks;
  uintptr_t                entries;
  uintptr_t                own;

  if (
    !_RBHeap_Walk_tree( _RBHeap_Root( &control->free_root ) ) ||
    !_RBHeap_Walk_tree( _RBHeap_Root( &control->used_root ) )
  ) {
    return false;
  }

  memset( &counts, 0, sizeof( counts ) );
  free_nodes = 0;
  previous = NULL;
  node = _RBHeap_First( _RBHeap_Root( &control->free_root ) );

  while ( node != NULL ) {
    if (
      node->size == 0 || ( node->begin & ( RBHEAP_ALIGNMENT - 1 ) ) != 0 ||
      ( node->size & ( RBHEAP_ALIGNMENT - 1 ) ) != 0 ||
      ( previous != NULL &&
        _RBHeap_Free_less( node->size, node->begin, previous ) ) ||
      !_RBHeap_Walk_slot( control, node, &counts )
    ) {
      return false;
    }

    ++free_nodes;
    previous = node;
    node = _RBHeap_Successor( node );
  }

  end = control->area_begin;
  free_size = 0;
  free_blocks = 0;
  used_blocks = 0;
  entries = 0;
  own = 0;
  node = _RBHeap_First( _RBHeap_Root( &control->used_root ) );

  while ( node != NULL ) {
    uintptr_t begin;

    begin = _RBHeap_Begin( node );

    if ( begin < end || node->size == 0 ) {
      return false;
    }

    if ( begin > end ) {
      if ( !_RBHeap_Walk_has_free( control, begin - end, end ) ) {
        return false;
      }

      free_size += begin - end;
      ++free_blocks;
    }

    /*
     * The descriptor of a hole may lie in a slab added inside the hole.  The
     * walk of the slabs counts the own descriptors against the slabs inside
     * the areas.
     */
    if ( !_RBHeap_Is_hole( node ) && _RBHeap_Is_own( node ) ) {
      if ( (uintptr_t) node != begin + sizeof( *node ) ) {
        return false;
      }

      ++own;
    } else {
      if ( !_RBHeap_Is_hole( node ) ) {
        ++used_blocks;
      }

      if ( !_RBHeap_Walk_slot( control, node, &counts ) ) {
        return false;
      }
    }

    ++entries;
    end = begin + node->size;
    node = _RBHeap_Successor( node );
  }

  if ( end > control->area_end ) {
    return false;
  }

  if ( end < control->area_end ) {
    if ( !_RBHeap_Walk_has_free( control, control->area_end - end, end ) ) {
      return false;
    }

    free_size += control->area_end - end;
    ++free_blocks;
  }

  return free_size == control->free_size &&
         free_blocks == control->stats.free_blocks &&
         free_nodes == free_blocks && entries == control->entries &&
         used_blocks == control->stats.used_blocks &&
         _RBHeap_Walk_slabs( control, &counts, own );
}
