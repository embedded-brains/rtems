/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup sparc_leon3
 *
 * @brief This header file provides interfaces used by the BSP implementation.
 */

/*
 * Copyright (C) 2021 embedded brains GmbH (http://www.embedded-brains.de)
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

#ifndef _BSP_LEON3_H
#define _BSP_LEON3_H

#include <rtems.h>
#include <grlib/grlib.h>

struct ambapp_dev;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup sparc_leon3
 *
 * @{
 */

/**
 * @brief This lock serializes the interrupt controller access.
 */
extern rtems_interrupt_lock LEON3_IrqCtrl_Lock;

/**
 * @brief This pointer provides the IRQ(A)MP register block address.
 */
extern volatile struct irqmp_regs *LEON3_IrqCtrl_Regs;

/**
 * @brief This pointer provides the IRQ(A)MP device information block.
 */
extern struct ambapp_dev *LEON3_IrqCtrl_Adev;

/**
 * @brief Initializes the interrupt controller for the boot processor.
 *
 * @param[in, out] regs is the IRQ(A)MP register block address.
 */
void leon3_ext_irq_init( volatile struct irqmp_regs *regs );

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _BSP_LEON3_H */
