/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqPriorityChangePriority
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

#ifndef _TR_TQ_PRIORITY_CHANGE_PRIORITY_H
#define _TR_TQ_PRIORITY_CHANGE_PRIORITY_H

#include "tx-thread-queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreTqReqPriorityChangePriority
 *
 * @{
 */

typedef enum {
  ScoreTqReqPriorityChangePriority_Pre_TargetPosition_First,
  ScoreTqReqPriorityChangePriority_Pre_TargetPosition_Second,
  ScoreTqReqPriorityChangePriority_Pre_TargetPosition_NA
} ScoreTqReqPriorityChangePriority_Pre_TargetPosition;

typedef enum {
  ScoreTqReqPriorityChangePriority_Pre_NewPriority_High,
  ScoreTqReqPriorityChangePriority_Pre_NewPriority_Equal,
  ScoreTqReqPriorityChangePriority_Pre_NewPriority_Low,
  ScoreTqReqPriorityChangePriority_Pre_NewPriority_NA
} ScoreTqReqPriorityChangePriority_Pre_NewPriority;

typedef enum {
  ScoreTqReqPriorityChangePriority_Pre_OtherQueue_None,
  ScoreTqReqPriorityChangePriority_Pre_OtherQueue_Before,
  ScoreTqReqPriorityChangePriority_Pre_OtherQueue_After,
  ScoreTqReqPriorityChangePriority_Pre_OtherQueue_NA
} ScoreTqReqPriorityChangePriority_Pre_OtherQueue;

typedef enum {
  ScoreTqReqPriorityChangePriority_Post_Position_First,
  ScoreTqReqPriorityChangePriority_Post_Position_Second,
  ScoreTqReqPriorityChangePriority_Post_Position_NA
} ScoreTqReqPriorityChangePriority_Post_Position;

typedef enum {
  ScoreTqReqPriorityChangePriority_Post_QueueOrder_Kept,
  ScoreTqReqPriorityChangePriority_Post_QueueOrder_NA
} ScoreTqReqPriorityChangePriority_Post_QueueOrder;

/**
 * @brief Runs the parameterized test case.
 *
 * @param[in,out] tq_ctx is the thread queue context.
 *
 * @param home_only is true, if the thread queue admits only threads with the
 *   home scheduler of the enqueueing thread as their home scheduler, otherwise
 *   false.
 */
void ScoreTqReqPriorityChangePriority_Run( TQContext *tq_ctx, bool home_only );

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _TR_TQ_PRIORITY_CHANGE_PRIORITY_H */
