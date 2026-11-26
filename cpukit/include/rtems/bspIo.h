/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSAPIKernelCharIO
 *
 * @brief This header file provides the kernel character input/output support
 *   API.
 */

/*
 * Copyright (C) 2020, 2026 embedded brains GmbH & Co. KG
 * Copyright (C) 2015 On-Line Applications Research Corporation (OAR)
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

/*
 * The spec:/... comments allow you to identify content which is
 * duplicated in the rtems and rtems-docs repositories.
 */

/* Find related documentation with spec:/rtems/io/if/header-3 */

#ifndef _RTEMS_BSPIO_H
#define _RTEMS_BSPIO_H

#include <stdarg.h>
#include <stdbool.h>
#include <rtems/score/basedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Find related documentation with spec:/rtems/io/if/group-3 */

/**
 * @defgroup RTEMSAPIKernelCharIO Kernel Character I/O Support
 *
 * @ingroup RTEMSAPIClassicIO
 *
 * @brief The kernel character input/output support is an extension of the @ref
 *   RTEMSAPIClassicIO to output characters to the kernel character output
 *   device and receive characters from the kernel character input device using
 *   a polled and non-blocking implementation.
 *
 * The directives may be used to print debug and test information. The kernel
 * character input/output support should work even if no Console Driver is
 * configured, see @ref CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER. The kernel
 * character input and output device is provided by the BSP. Applications may
 * change the device.
 */

/* Find related documentation with spec:/rtems/io/if/bsp-output-char-function-type */

/**
 * @ingroup RTEMSAPIKernelCharIO
 *
 * @brief Polled character output functions shall have this type.
 */
typedef int ( *BSP_output_char_function_type )( int );

/* Find related documentation with spec:/rtems/io/if/output-char-mask */

/**
 * @ingroup RTEMSAPIKernelCharIO
 *
 * @brief This mask selects the bits of the parameter of #BSP_output_char which
 *   specify the character to output.
 */
#define RTEMS_OUTPUT_CHAR_MASK 0x000000ff

/* Find related documentation with spec:/rtems/io/if/output-char-drain */

/**
 * @ingroup RTEMSAPIKernelCharIO
 *
 * @brief This flag requests #BSP_output_char to drain the kernel character
 *   output device.
 *
 * The kernel character output device is drained when it transmitted each
 * character which it accepted.
 */
#define RTEMS_OUTPUT_CHAR_DRAIN 0x00000100

/* Find related documentation with spec:/rtems/io/if/output-char-try */

/**
 * @ingroup RTEMSAPIKernelCharIO
 *
 * @brief This flag requests #BSP_output_char to perform the request only if it
 *   does not need to wait for the kernel character output device.
 *
 * The referenced function may wait if another output to the device runs
 * concurrently with the request.
 */
#define RTEMS_OUTPUT_CHAR_TRY 0x00000200

/* Find related documentation with spec:/rtems/io/if/output-char-is-drain */

/**
 * @ingroup RTEMSAPIKernelCharIO
 *
 * @brief Checks if the parameter of #BSP_output_char sets
 *   #RTEMS_OUTPUT_CHAR_DRAIN.
 *
 * @param c is the parameter of #BSP_output_char.
 *
 * @retval true The parameter is non-negative and sets
 *   #RTEMS_OUTPUT_CHAR_DRAIN.
 *
 * @retval false Otherwise.
 */
static inline bool rtems_output_char_is_drain( int c )
{
  return c >= 0 && ( c & RTEMS_OUTPUT_CHAR_DRAIN ) != 0;
}

/* Find related documentation with spec:/rtems/io/if/output-char-is-try */

/**
 * @ingroup RTEMSAPIKernelCharIO
 *
 * @brief Checks if the parameter of #BSP_output_char sets
 *   #RTEMS_OUTPUT_CHAR_TRY.
 *
 * @param c is the parameter of #BSP_output_char.
 *
 * @retval true The parameter is non-negative and sets #RTEMS_OUTPUT_CHAR_TRY.
 *
 * @retval false Otherwise.
 */
static inline bool rtems_output_char_is_try( int c )
{
  return c >= 0 && ( c & RTEMS_OUTPUT_CHAR_TRY ) != 0;
}

/* Find related documentation with spec:/rtems/io/if/bsp-output-char */

/**
 * @ingroup RTEMSAPIKernelCharIO
 *
 * @brief This function pointer references the kernel character output
 *   implementation.
 *
 * This function pointer shall never be NULL. It shall be provided by the BSP
 * and statically initialized.
 *
 * The parameter specifies the character to output in the bits of
 * #RTEMS_OUTPUT_CHAR_MASK. The other bits of the parameter specify the flags
 * #RTEMS_OUTPUT_CHAR_DRAIN and #RTEMS_OUTPUT_CHAR_TRY. All other bits of a
 * non-negative parameter shall be zero. A negative parameter is a
 * sign-extended character. It specifies the character to output in the bits of
 * #RTEMS_OUTPUT_CHAR_MASK and sets no flag.
 *
 * For a request without #RTEMS_OUTPUT_CHAR_DRAIN, the referenced function
 * shall output exactly the character specified by the parameter. In
 * particular, it shall not perform character translations, for example `NL` to
 * `CR` followed by `NR`. For a request with #RTEMS_OUTPUT_CHAR_DRAIN, the
 * referenced function shall ignore the bits of #RTEMS_OUTPUT_CHAR_MASK. The
 * referenced function shall not block the calling thread.
 *
 * The referenced function returns zero if it performed the request. It returns
 * minus one if it performed nothing. A BSP without a kernel character output
 * device returns zero for each request.
 *
 * A BSP which does not support #RTEMS_OUTPUT_CHAR_TRY outputs the character as
 * if the flag was cleared. A BSP which does not support
 * #RTEMS_OUTPUT_CHAR_DRAIN returns minus one for each call with this flag set.
 */
extern BSP_output_char_function_type BSP_output_char;

/* Find related documentation with spec:/rtems/io/if/putc */

/**
 * @ingroup RTEMSAPIKernelCharIO
 *
 * @brief Outputs the character to the kernel character output device.
 *
 * @param c is the character to output.
 *
 * The directive outputs the character specified by `c` to the kernel character
 * output device using the polled character output implementation provided by
 * #BSP_output_char. The directive performs a character translation from `NL`
 * to `CR` followed by `NR`.
 *
 * If the kernel character output device is concurrently accessed, then
 * interleaved output may occur.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive may be called from within any runtime context.
 *
 * - The directive will not cause the calling task to be preempted.
 * @endparblock
 */
void rtems_putc( char c );

/* Find related documentation with spec:/rtems/io/if/put-char */

/**
 * @ingroup RTEMSAPIKernelCharIO
 *
 * @brief Puts the character using rtems_putc()
 *
 * @param c is the character to output.
 *
 * @param unused is an unused argument.
 *
 * @par Notes
 * The directive is provided to support the RTEMS Testing Framework.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive may be called from within any runtime context.
 *
 * - The directive will not cause the calling task to be preempted.
 * @endparblock
 */
void rtems_put_char( int c, void *unused );

/* Find related documentation with spec:/rtems/io/if/putk */

/**
 * @ingroup RTEMSAPIKernelCharIO
 *
 * @brief Outputs the characters of the string and a newline character to the
 *   kernel character output device.
 *
 * @param s is the string to output.
 *
 * @return Returns the number of characters output to the kernel character
 *   output device.
 *
 * @par Notes
 * @parblock
 * The directive may be used to print debug and test information. It uses
 * rtems_putc() to output the characters. This directive performs a character
 * translation from `NL` to `CR` followed by `NR`.
 *
 * If the kernel character output device is concurrently accessed, then
 * interleaved output may occur.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive is not included in the pre-qualified feature set of RTEMS.
 *   Applications which are restricted to only use interfaces of the
 *   pre-qualified feature set of RTEMS shall not use the directive.
 *
 * - The directive may be called from within any runtime context.
 *
 * - The directive will not cause the calling task to be preempted.
 * @endparblock
 */
int putk( const char *s );

/* Find related documentation with spec:/rtems/io/if/printk */

/**
 * @ingroup RTEMSAPIKernelCharIO
 *
 * @brief Outputs the characters defined by the format string and the arguments
 *   to the kernel character output device.
 *
 * @param fmt is a printf()-style format string.
 *
 * @param ... is a list of optional parameters required by the format string.
 *
 * @return Returns the number of characters output to the kernel character
 *   output device.
 *
 * @par Notes
 * @parblock
 * The directive may be used to print debug and test information. It uses
 * rtems_putc() to output the characters. This directive performs a character
 * translation from `NL` to `CR` followed by `NR`.
 *
 * If the kernel character output device is concurrently accessed, then
 * interleaved output may occur.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive may be called from within any runtime context.
 *
 * - The directive will not cause the calling task to be preempted.
 *
 * - Formatting of floating point numbers is not supported.
 * @endparblock
 */
RTEMS_PRINTFLIKE( 1, 2 ) int printk( const char *fmt, ... );

/* Find related documentation with spec:/rtems/io/if/vprintk */

/**
 * @ingroup RTEMSAPIKernelCharIO
 *
 * @brief Outputs the characters defined by the format string and the variable
 *   argument list to the kernel character output device.
 *
 * @param fmt is a printf()-style format string.
 *
 * @param ap is the variable argument list required by the format string.
 *
 * @return Returns the number of characters output to the kernel character
 *   output device.
 *
 * @par Notes
 * @parblock
 * The directive may be used to print debug and test information. It uses
 * rtems_putc() to output the characters. This directive performs a character
 * translation from `NL` to `CR` followed by `NR`.
 *
 * If the kernel character output device is concurrently accessed, then
 * interleaved output may occur.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive may be called from within any runtime context.
 *
 * - The directive will not cause the calling task to be preempted.
 *
 * - Formatting of floating point numbers is not supported.
 * @endparblock
 */
int vprintk( const char *fmt, va_list ap );

/* Find related documentation with spec:/rtems/io/if/printk-printer */

/**
 * @ingroup RTEMSAPIKernelCharIO
 *
 * @brief Outputs the characters defined by the format string and the variable
 *   argument list to the kernel character output device.
 *
 * @param unused is an unused argument.
 *
 * @param fmt is a printf()-style format string.
 *
 * @param ap is the variable argument list required by the format string.
 *
 * @return Returns the number of characters output to the kernel character
 *   output device.
 *
 * @par Notes
 * @parblock
 * The directive may be used to print debug and test information. It uses
 * rtems_putc() to output the characters. This directive performs a character
 * translation from `NL` to `CR` followed by `NR`.
 *
 * If the kernel character output device is concurrently accessed, then
 * interleaved output may occur.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive may be called from within any runtime context.
 *
 * - The directive will not cause the calling task to be preempted.
 *
 * - Formatting of floating point numbers is not supported.
 * @endparblock
 */
int rtems_printk_printer( void *unused, const char *fmt, va_list ap );

/* Find related documentation with spec:/rtems/io/if/bsp-polling-getchar-function-type */

/**
 * @ingroup RTEMSAPIKernelCharIO
 *
 * @brief Polled character input functions shall have this type.
 */
typedef int ( *BSP_polling_getchar_function_type )( void );

/* Find related documentation with spec:/rtems/io/if/bsp-poll-char */

/**
 * @ingroup RTEMSAPIKernelCharIO
 *
 * @brief This function pointer may reference the kernel character input
 *   implementation.
 *
 * This function pointer may be NULL. It may reference a function provided by
 * the BSP. Referenced functions shall dequeue the least recently received
 * character from the device and return it as an unsigned character. If no
 * character is enqueued on the device, then the function shall immediately
 * return the value minus one.
 */
extern BSP_polling_getchar_function_type BSP_poll_char;

/* Find related documentation with spec:/rtems/io/if/getchark */

/**
 * @ingroup RTEMSAPIKernelCharIO
 *
 * @brief Tries to dequeue a character from the kernel character input device.
 *
 * The directive tries to dequeue a character from the kernel character input
 * device using the polled character input implementation referenced by
 * #BSP_poll_char if it is available.
 *
 * @retval -1 The #BSP_poll_char pointer was equal to NULL.
 *
 * @retval -1 There was no character enqueued on the kernel character input
 *   device.
 *
 * @return Returns the character least recently enqueued on the kernel
 *   character input device as an unsigned character value.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * - The directive may be called from within any runtime context.
 *
 * - The directive will not cause the calling task to be preempted.
 * @endparblock
 */
int getchark( void );

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_BSPIO_H */
