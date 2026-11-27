/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqHelpingAddPriority
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

#ifndef _TR_TQ_HELPING_ADD_PRIORITY_H
#define _TR_TQ_HELPING_ADD_PRIORITY_H

#include "tx-thread-queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreTqReqHelpingAddPriority
 *
 * @{
 */

typedef enum {
  ScoreTqReqHelpingAddPriority_Pre_NewQueue_Empty,
  ScoreTqReqHelpingAddPriority_Pre_NewQueue_High,
  ScoreTqReqHelpingAddPriority_Pre_NewQueue_Equal,
  ScoreTqReqHelpingAddPriority_Pre_NewQueue_Low,
  ScoreTqReqHelpingAddPriority_Pre_NewQueue_NA
} ScoreTqReqHelpingAddPriority_Pre_NewQueue;

typedef enum {
  ScoreTqReqHelpingAddPriority_Pre_OtherQueue_None,
  ScoreTqReqHelpingAddPriority_Pre_OtherQueue_One,
  ScoreTqReqHelpingAddPriority_Pre_OtherQueue_NA
} ScoreTqReqHelpingAddPriority_Pre_OtherQueue;

typedef enum {
  ScoreTqReqHelpingAddPriority_Post_AddPosition_Initial,
  ScoreTqReqHelpingAddPriority_Post_AddPosition_First,
  ScoreTqReqHelpingAddPriority_Post_AddPosition_Second,
  ScoreTqReqHelpingAddPriority_Post_AddPosition_NA
} ScoreTqReqHelpingAddPriority_Post_AddPosition;

typedef enum {
  ScoreTqReqHelpingAddPriority_Post_AddOrder_Kept,
  ScoreTqReqHelpingAddPriority_Post_AddOrder_NA
} ScoreTqReqHelpingAddPriority_Post_AddOrder;

/**
 * @brief Runs the parameterized test case.
 *
 * @param[in,out] tq_ctx is the thread queue context.
 *
 * @param home_only is true, if the thread queue admits only threads with the
 *   home scheduler of the enqueueing thread as their home scheduler, otherwise
 *   false.
 */
void ScoreTqReqHelpingAddPriority_Run( TQContext *tq_ctx, bool home_only );

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _TR_TQ_HELPING_ADD_PRIORITY_H */
