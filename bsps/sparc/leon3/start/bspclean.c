/**
 * @file
 * @ingroup RTEMSBSPsSPARCLEON3
 * @brief LEON3 BSP fatal extension
 *
 *  Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  COPYRIGHT (c) 2014
 *  Aeroflex Gaisler
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/leon3.h>
#include <rtems/score/cpuimpl.h>
#include <rtems/score/smpimpl.h>

#if defined(RTEMS_SMP)
static void leon3_wait_for_power_down(irqamp *regs)
{
  uint32_t max_wait;
  uint32_t cpu_self;
  uint32_t cpu_count;
  uint32_t halt_mask;
  uint32_t i;

  cpu_count = leon3_get_cpu_count(regs);

  if (cpu_count > rtems_configuration_get_maximum_processors()) {
    cpu_count = rtems_configuration_get_maximum_processors();
  }

  cpu_self = rtems_scheduler_get_processor();
  halt_mask = 0;

  for (i = 0; i < cpu_count; ++i) {
    if (i != cpu_self && _SMP_Should_start_processor(i)) {
      halt_mask |= UINT32_C(1) << i;
    }
  }

  /*
   * Wait some time for secondary processors to halt.
   *
   * The value was chosen to get something in the magnitude of 1ms on a 200MHz
   * processor.
   */

  max_wait = 1234567;
  i = 0;

  while (
    (grlib_load_32(&regs->mpstat) & halt_mask) != halt_mask && i < max_wait
  ) {
    ++i;
  }
}
#endif

void bsp_fatal_extension(
  rtems_fatal_source source,
  bool always_set_to_false,
  rtems_fatal_code code
)
{
  rtems_interrupt_level level;

  rtems_interrupt_local_disable(level);
  (void) level;

#if defined(RTEMS_SMP)
  /*
   * On SMP we must wait for all other CPUs not requesting a fatal halt, they
   * are responding to another CPU's fatal request. These CPUs goes into
   * power-down. The CPU requesting fatal halt waits for the others and then
   * handles the system shutdown via the normal procedure.
   */
  if ((source == RTEMS_FATAL_SOURCE_SMP) &&
      (code == SMP_FATAL_SHUTDOWN_RESPONSE)) {
    leon3_power_down_loop(); /* CPU didn't start shutdown sequence .. */
  } else {
    irqamp *regs;

    _SMP_Request_shutdown();

    regs = LEON3_IrqCtrl_Regs;
#if defined(LEON3_IRQAMP_BASE)
    leon3_wait_for_power_down(regs);
#else
    if (regs != NULL) {
      leon3_wait_for_power_down(regs);
    }
#endif
  }
#endif

#if BSP_PRINT_EXCEPTION_CONTEXT
  if ( source == RTEMS_FATAL_SOURCE_EXCEPTION ) {
    rtems_exception_frame_print( (const rtems_exception_frame *) code );
  }
#endif

#if BSP_RESET_BOARD_AT_EXIT
  /*
   * Stop the system termination right now.  This skips the dynamically
   * installed fatal error extensions and the generics shutdown procedure.
   */
  _CPU_Fatal_halt( source, code );
#endif
}
