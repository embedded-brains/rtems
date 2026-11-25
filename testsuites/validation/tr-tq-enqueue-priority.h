/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqEnqueuePriority
 */

/*
 * Copyright (C) 2021, 2026 embedded brains GmbH & Co. KG
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

#ifndef _TR_TQ_ENQUEUE_PRIORITY_H
#define _TR_TQ_ENQUEUE_PRIORITY_H

#include "tx-thread-queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreTqReqEnqueuePriority
 *
 * @{
 */

typedef enum {
  ScoreTqReqEnqueuePriority_Pre_EligibleScheduler_Home,
  ScoreTqReqEnqueuePriority_Pre_EligibleScheduler_One,
  ScoreTqReqEnqueuePriority_Pre_EligibleScheduler_Two,
  ScoreTqReqEnqueuePriority_Pre_EligibleScheduler_More,
  ScoreTqReqEnqueuePriority_Pre_EligibleScheduler_NA
} ScoreTqReqEnqueuePriority_Pre_EligibleScheduler;

typedef enum {
  ScoreTqReqEnqueuePriority_Pre_QueueHome_Empty,
  ScoreTqReqEnqueuePriority_Pre_QueueHome_High,
  ScoreTqReqEnqueuePriority_Pre_QueueHome_Equal,
  ScoreTqReqEnqueuePriority_Pre_QueueHome_Low,
  ScoreTqReqEnqueuePriority_Pre_QueueHome_NA
} ScoreTqReqEnqueuePriority_Pre_QueueHome;

typedef enum {
  ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_Empty,
  ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_High,
  ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_Equal,
  ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_Low,
  ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping_NA
} ScoreTqReqEnqueuePriority_Pre_QueueFirstHelping;

typedef enum {
  ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_Empty,
  ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_High,
  ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_Equal,
  ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_Low,
  ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping_NA
} ScoreTqReqEnqueuePriority_Pre_QueueSecondHelping;

typedef enum {
  ScoreTqReqEnqueuePriority_Pre_QueueIneligible_None,
  ScoreTqReqEnqueuePriority_Pre_QueueIneligible_One,
  ScoreTqReqEnqueuePriority_Pre_QueueIneligible_More,
  ScoreTqReqEnqueuePriority_Pre_QueueIneligible_NA
} ScoreTqReqEnqueuePriority_Pre_QueueIneligible;

typedef enum {
  ScoreTqReqEnqueuePriority_Pre_QueueOrder_HomeFirstSecondIneligible,
  ScoreTqReqEnqueuePriority_Pre_QueueOrder_HomeFirstIneligibleSecond,
  ScoreTqReqEnqueuePriority_Pre_QueueOrder_HomeSecondFirstIneligible,
  ScoreTqReqEnqueuePriority_Pre_QueueOrder_HomeSecondIneligibleFirst,
  ScoreTqReqEnqueuePriority_Pre_QueueOrder_HomeIneligibleFirstSecond,
  ScoreTqReqEnqueuePriority_Pre_QueueOrder_HomeIneligibleSecondFirst,
  ScoreTqReqEnqueuePriority_Pre_QueueOrder_FirstHomeSecondIneligible,
  ScoreTqReqEnqueuePriority_Pre_QueueOrder_FirstHomeIneligibleSecond,
  ScoreTqReqEnqueuePriority_Pre_QueueOrder_FirstSecondHomeIneligible,
  ScoreTqReqEnqueuePriority_Pre_QueueOrder_FirstSecondIneligibleHome,
  ScoreTqReqEnqueuePriority_Pre_QueueOrder_FirstIneligibleHomeSecond,
  ScoreTqReqEnqueuePriority_Pre_QueueOrder_FirstIneligibleSecondHome,
  ScoreTqReqEnqueuePriority_Pre_QueueOrder_SecondHomeFirstIneligible,
  ScoreTqReqEnqueuePriority_Pre_QueueOrder_SecondHomeIneligibleFirst,
  ScoreTqReqEnqueuePriority_Pre_QueueOrder_SecondFirstHomeIneligible,
  ScoreTqReqEnqueuePriority_Pre_QueueOrder_SecondFirstIneligibleHome,
  ScoreTqReqEnqueuePriority_Pre_QueueOrder_SecondIneligibleHomeFirst,
  ScoreTqReqEnqueuePriority_Pre_QueueOrder_SecondIneligibleFirstHome,
  ScoreTqReqEnqueuePriority_Pre_QueueOrder_IneligibleHomeFirstSecond,
  ScoreTqReqEnqueuePriority_Pre_QueueOrder_IneligibleHomeSecondFirst,
  ScoreTqReqEnqueuePriority_Pre_QueueOrder_IneligibleFirstHomeSecond,
  ScoreTqReqEnqueuePriority_Pre_QueueOrder_IneligibleFirstSecondHome,
  ScoreTqReqEnqueuePriority_Pre_QueueOrder_IneligibleSecondHomeFirst,
  ScoreTqReqEnqueuePriority_Pre_QueueOrder_IneligibleSecondFirstHome,
  ScoreTqReqEnqueuePriority_Pre_QueueOrder_NA
} ScoreTqReqEnqueuePriority_Pre_QueueOrder;

typedef enum {
  ScoreTqReqEnqueuePriority_Post_PositionHome_Initial,
  ScoreTqReqEnqueuePriority_Post_PositionHome_First,
  ScoreTqReqEnqueuePriority_Post_PositionHome_Second,
  ScoreTqReqEnqueuePriority_Post_PositionHome_NA
} ScoreTqReqEnqueuePriority_Post_PositionHome;

typedef enum {
  ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Absent,
  ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Initial,
  ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_First,
  ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_Second,
  ScoreTqReqEnqueuePriority_Post_PositionFirstHelping_NA
} ScoreTqReqEnqueuePriority_Post_PositionFirstHelping;

typedef enum {
  ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Absent,
  ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Initial,
  ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_First,
  ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_Second,
  ScoreTqReqEnqueuePriority_Post_PositionSecondHelping_NA
} ScoreTqReqEnqueuePriority_Post_PositionSecondHelping;

typedef enum {
  ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Absent,
  ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_Initial,
  ScoreTqReqEnqueuePriority_Post_PositionOtherHelping_NA
} ScoreTqReqEnqueuePriority_Post_PositionOtherHelping;

typedef enum {
  ScoreTqReqEnqueuePriority_Post_QueueOrder_Kept,
  ScoreTqReqEnqueuePriority_Post_QueueOrder_NA
} ScoreTqReqEnqueuePriority_Post_QueueOrder;

/**
 * @brief Runs the parameterized test case.
 *
 * @param[in,out] tq_ctx is the thread queue context.
 *
 * @param home_only is true, if the thread queue admits only threads with the
 *   home scheduler of the enqueueing thread as their home scheduler, otherwise
 *   false.
 */
void ScoreTqReqEnqueuePriority_Run( TQContext *tq_ctx, bool home_only );

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _TR_TQ_ENQUEUE_PRIORITY_H */
