/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreRBHeap
 *
 * @brief This header file provides the tree access of the
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

/*
 * The trees of the heap are red-black trees of <rtems/score/rbtree.h>.  The
 * node is the first member of a descriptor, so a descriptor and its node have
 * the same address, and NULL maps to NULL.
 */

#ifndef _RTEMS_SCORE_SRC_RBHEAPIMPL_H
#define _RTEMS_SCORE_SRC_RBHEAPIMPL_H

#include <rtems/score/rbheap.h>
#include <rtems/score/rbtreeimpl.h>

#include <stddef.h>

RTEMS_STATIC_ASSERT(
  offsetof( RBHeap_descriptor, node ) == 0,
  RBHeap_descriptor_node
);

static inline RBHeap_descriptor *_RBHeap_Of_node( const RBTree_Node *node )
{
  return (RBHeap_descriptor *) node;
}

static inline RBHeap_descriptor *_RBHeap_Root( const RBTree_Control *tree )
{
  return _RBHeap_Of_node( _RBTree_Root( tree ) );
}

static inline RBHeap_descriptor *_RBHeap_Left( const RBHeap_descriptor *desc )
{
  return _RBHeap_Of_node( _RBTree_Left( &desc->node ) );
}

static inline RBHeap_descriptor *_RBHeap_Right( const RBHeap_descriptor *desc )
{
  return _RBHeap_Of_node( _RBTree_Right( &desc->node ) );
}

static inline RBHeap_descriptor *_RBHeap_Parent(
  const RBHeap_descriptor *desc
)
{
  return _RBHeap_Of_node( _RBTree_Parent( &desc->node ) );
}

static inline bool _RBHeap_Is_red( const RBHeap_descriptor *desc )
{
  return _RBTree_Color( &desc->node ) == RTEMS_RB_RED;
}

/*
 * Inserts the descriptor as the right or the left child of the parent, else as
 * the root.  The parent has no child on that side.
 */
static inline void _RBHeap_Insert(
  RBTree_Control    *tree,
  RBHeap_descriptor *desc,
  RBHeap_descriptor *parent,
  bool               right
)
{
  RBTree_Node **link;

  if ( parent == NULL ) {
    link = _RBTree_Root_reference( tree );
  } else if ( right ) {
    link = _RBTree_Right_reference( &parent->node );
  } else {
    link = _RBTree_Left_reference( &parent->node );
  }

  _RBTree_Initialize_node( &desc->node );
  _RBTree_Insert_with_parent(
    tree,
    &desc->node,
    parent != NULL ? &parent->node : NULL,
    link
  );
}

static inline void _RBHeap_Extract(
  RBTree_Control    *tree,
  RBHeap_descriptor *desc
)
{
  _RBTree_Extract( tree, &desc->node );
}

/* Returns the next descriptor in the order of the tree, else NULL. */
static inline RBHeap_descriptor *_RBHeap_Successor(
  const RBHeap_descriptor *desc
)
{
  return _RBHeap_Of_node( _RBTree_Successor( &desc->node ) );
}

/* Returns the previous descriptor in the order of the tree, else NULL. */
static inline RBHeap_descriptor *_RBHeap_Predecessor(
  const RBHeap_descriptor *desc
)
{
  return _RBHeap_Of_node( _RBTree_Predecessor( &desc->node ) );
}

#endif /* _RTEMS_SCORE_SRC_RBHEAPIMPL_H */
