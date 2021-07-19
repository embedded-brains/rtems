/*
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified for LEON3 BSP.
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef APBUART_TERMIOS_H
#define APBUART_TERMIOS_H

#include <rtems/termiostypes.h>
#include "apbuart.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct apbuart_context {
  rtems_termios_device_context base;
  apbuart *regs;
  unsigned int freq_hz;
  rtems_vector_number irq;
  volatile int sending;
  char *buf;
};

extern const rtems_termios_device_handler apbuart_handler_interrupt;

extern const rtems_termios_device_handler apbuart_handler_polled;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* APBUART_TERMIOS_H */
