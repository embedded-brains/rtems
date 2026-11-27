/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqReqEnqueuePriorityInherit
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

#ifndef _TR_TQ_ENQUEUE_PRIORITY_INHERIT_H
#define _TR_TQ_ENQUEUE_PRIORITY_INHERIT_H

#include "tx-thread-queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreTqReqEnqueuePriorityInherit
 *
 * @{
 */

typedef enum {
  ScoreTqReqEnqueuePriorityInherit_Pre_EligibleScheduler_Home,
  ScoreTqReqEnqueuePriorityInherit_Pre_EligibleScheduler_Helping,
  ScoreTqReqEnqueuePriorityInherit_Pre_EligibleScheduler_NA
} ScoreTqReqEnqueuePriorityInherit_Pre_EligibleScheduler;

typedef enum {
  ScoreTqReqEnqueuePriorityInherit_Pre_PriorityForOwner_Vital,
  ScoreTqReqEnqueuePriorityInherit_Pre_PriorityForOwner_Dispensable,
  ScoreTqReqEnqueuePriorityInherit_Pre_PriorityForOwner_NA
} ScoreTqReqEnqueuePriorityInherit_Pre_PriorityForOwner;

typedef enum {
  ScoreTqReqEnqueuePriorityInherit_Pre_SchedulerForOwner_Vital,
  ScoreTqReqEnqueuePriorityInherit_Pre_SchedulerForOwner_Dispensable,
  ScoreTqReqEnqueuePriorityInherit_Pre_SchedulerForOwner_NA
} ScoreTqReqEnqueuePriorityInherit_Pre_SchedulerForOwner;

typedef enum {
  ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState_NotEnqueued,
  ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState_FIFO,
  ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState_Priority,
  ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState_PriorityInherit,
  ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState_NA
} ScoreTqReqEnqueuePriorityInherit_Pre_OwnerState;

typedef enum {
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Raise,
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_Nop,
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority_NA
} ScoreTqReqEnqueuePriorityInherit_Post_OwnerPriority;

typedef enum {
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NewHelper,
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_Nop,
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler_NA
} ScoreTqReqEnqueuePriorityInherit_Post_OwnerScheduler;

typedef enum {
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Raise,
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_Nop,
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority_NA
} ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerPriority;

typedef enum {
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NewHelper,
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_Nop,
  ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler_NA
} ScoreTqReqEnqueuePriorityInherit_Post_OwnerOwnerScheduler;

/**
 * @brief Runs the parameterized test case.
 *
 * @param[in,out] tq_ctx is the thread queue context.
 */
void ScoreTqReqEnqueuePriorityInherit_Run( TQContext *tq_ctx );

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _TR_TQ_ENQUEUE_PRIORITY_INHERIT_H */
