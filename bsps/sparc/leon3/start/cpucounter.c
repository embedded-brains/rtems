/*
 * Copyright (c) 2014, 2018 embedded brains GmbH.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <leon.h>
#include <grlib/irqamp.h>

#include <rtems/counter.h>
#include <rtems/sysinit.h>
#include <rtems/score/sparcimpl.h>

static uint32_t leon3_counter_frequency = 1000000000;

uint32_t _CPU_Counter_frequency(void)
{
  return leon3_counter_frequency;
}

static void leon3_counter_initialize(void)
{
  irqamp_timestamp *irqmp_ts;
  volatile struct gptimer_regs *gpt;
  SPARC_Counter *counter;

  irqmp_ts = irqamp_get_timestamp_registers(LEON3_IrqCtrl_Regs);
  gpt = LEON3_Timer_Regs;
  counter = &_SPARC_Counter_mutable;

  leon3_up_counter_enable();

  if (leon3_up_counter_is_available()) {
    /* Use the LEON4 up-counter if available */
    counter->read_isr_disabled = _SPARC_Counter_read_asr23;
    counter->read = _SPARC_Counter_read_asr23;

    leon3_counter_frequency = leon3_up_counter_frequency();
  } else if (irqmp_ts != NULL) {
    /* Use the interrupt controller timestamp counter if available */
    counter->read_isr_disabled = _SPARC_Counter_read_up;
    counter->read = _SPARC_Counter_read_up;
    counter->counter_register = &irqmp_ts->itcnt;

    /* Enable interrupt timestamping for an arbitrary interrupt line */
    grlib_store_32(&irqmp_ts->itstmpc, IRQAMP_ITSTMPC_TSISEL(1));

    leon3_counter_frequency = ambapp_freq_get(ambapp_plb(), LEON3_IrqCtrl_Adev);
  } else if (gpt != NULL) {
    /* Fall back to the first GPTIMER if available */
    counter->read_isr_disabled = _SPARC_Counter_read_down;
    counter->read = _SPARC_Counter_read_down;
    counter->counter_register = &gpt->timer[LEON3_COUNTER_GPTIMER_INDEX].value;

    /* Enable timer just in case no clock driver is configured */
    gpt->timer[LEON3_COUNTER_GPTIMER_INDEX].reload = 0xffffffff;
    gpt->timer[LEON3_COUNTER_GPTIMER_INDEX].ctrl |= GPTIMER_TIMER_CTRL_EN |
                                                    GPTIMER_TIMER_CTRL_RS |
                                                    GPTIMER_TIMER_CTRL_LD;

    leon3_counter_frequency = ambapp_freq_get(ambapp_plb(), LEON3_Timer_Adev) /
      (gpt->scaler_reload + 1);
  }
}

RTEMS_SYSINIT_ITEM(
  leon3_counter_initialize,
  RTEMS_SYSINIT_CPU_COUNTER,
  RTEMS_SYSINIT_ORDER_FIRST
);

SPARC_COUNTER_DEFINITION;
