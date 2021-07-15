/*
 *  This is a dummy bsp_reset routine.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/bspIo.h>
#include <rtems/score/io.h>
#include <bsp/bootcard.h>

void bsp_reset( void )
{
#if defined(RTEMS_COVERAGE)
  _IO_Dump_gcov_info( rtems_put_char, NULL );
#endif
}
