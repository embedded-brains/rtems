/*
 * Copyright (c) 2014, 2018 embedded brains GmbH.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp/leon3.h>
#include <grlib/irqamp.h>

#include <rtems/counter.h>
#include <rtems/sysinit.h>
#include <rtems/score/sparcimpl.h>

static uint32_t leon3_counter_frequency = 1000000000;

uint32_t _CPU_Counter_frequency(void)
{
  return leon3_counter_frequency;
}

#if defined(LEON3_HAS_ASR_22_23_UP_COUNTER) || \
   defined(LEON3_PROBE_ASR_22_23_UP_COUNTER)
static void leon3_counter_use_up_counter(SPARC_Counter *counter)
{
  counter->read_isr_disabled = _SPARC_Counter_read_asr23;
  counter->read = _SPARC_Counter_read_asr23;

  leon3_counter_frequency = leon3_up_counter_frequency();
}
#endif

#if defined(LEON3_IRQAMP_PROBE_TIMESTAMP)
static void leon3_counter_use_irqamp_timestamp(
  SPARC_Counter *counter,
  irqamp_timestamp *irqmp_ts
)
{
  counter->read_isr_disabled = _SPARC_Counter_read_up;
  counter->read = _SPARC_Counter_read_up;
  counter->counter_register = &irqmp_ts->itcnt;

  /* Enable interrupt timestamping for an arbitrary interrupt line */
  grlib_store_32(&irqmp_ts->itstmpc, IRQAMP_ITSTMPC_TSISEL(1));

#if defined(LEON3_PLB_FREQUENCY_DEFINED_BY_GPTIMER)
  leon3_counter_frequency = leon3_processor_local_bus_frequency();
#else
  leon3_counter_frequency = ambapp_freq_get(ambapp_plb(), LEON3_IrqCtrl_Adev);
#endif
}
#endif

#if !defined(LEON3_HAS_ASR_22_23_UP_COUNTER)
static void leon3_counter_use_gptimer(SPARC_Counter *counter, gptimer *gpt)
{
  gptimer_timer *timer;

  timer = &gpt->timer[LEON3_COUNTER_GPTIMER_INDEX];
  counter->read_isr_disabled = _SPARC_Counter_read_down;
  counter->read = _SPARC_Counter_read_down;
  counter->counter_register = &timer->tcntval;

  /* Make timer free running */
  grlib_store_32(&timer->trldval, 0xffffffff);
  grlib_store_32(&timer->tctrl, GPTIMER_TCTRL_EN | GPTIMER_TCTRL_RS);

#if defined(LEON3_PLB_FREQUENCY_DEFINED_BY_GPTIMER)
  leon3_counter_frequency = LEON3_GPTIMER_0_FREQUENCY_SET_BY_BOOT_LOADER;
#else
  leon3_counter_frequency = ambapp_freq_get(ambapp_plb(), LEON3_Timer_Adev) /
    (grlib_load_32(&gpt->sreload) + 1);
#endif
}
#endif

static void leon3_counter_initialize(void)
{
#if defined(LEON3_IRQAMP_PROBE_TIMESTAMP)
  irqamp_timestamp *irqmp_ts;
#endif
#if !defined(LEON3_HAS_ASR_22_23_UP_COUNTER)
  gptimer *gpt;
#endif
  SPARC_Counter *counter;

  counter = &_SPARC_Counter_mutable;

#if defined(LEON3_HAS_ASR_22_23_UP_COUNTER)
  leon3_up_counter_enable();
  leon3_counter_use_up_counter(counter);
#else /* LEON3_HAS_ASR_22_23_UP_COUNTER */
#if defined(LEON3_PROBE_ASR_22_23_UP_COUNTER)
  leon3_up_counter_enable();

  if (leon3_up_counter_is_available()) {
    /* Use the LEON4 up-counter if available */
    leon3_counter_use_up_counter(counter);
    return;
  }
#endif

#if defined(LEON3_IRQAMP_PROBE_TIMESTAMP)
  irqmp_ts = irqamp_get_timestamp_registers(LEON3_IrqCtrl_Regs);

  if (irqmp_ts != NULL) {
    /* Use the interrupt controller timestamp counter if available */
    leon3_counter_use_irqamp_timestamp(counter, irqmp_ts);
    return;
  }
#endif

  gpt = LEON3_Timer_Regs;

#if defined(LEON3_GPTIMER_BASE)
  leon3_counter_use_gptimer(counter, gpt);
#else
  if (gpt != NULL) {
    /* Fall back to the first GPTIMER if available */
    leon3_counter_use_gptimer(counter, gpt);
  }
#endif
#endif /* LEON3_HAS_ASR_22_23_UP_COUNTER */
}

RTEMS_SYSINIT_ITEM(
  leon3_counter_initialize,
  RTEMS_SYSINIT_CPU_COUNTER,
  RTEMS_SYSINIT_ORDER_FIRST
);

SPARC_COUNTER_DEFINITION;
