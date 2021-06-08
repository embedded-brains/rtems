/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRLIBGPTIMER
 *
 * @brief This header file defines the GPTIMER register block interface.
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

/* Generated from spec:/dev/grlib/if/gptimer-header */

#ifndef _GRLIB_GPTIMER_REGS_H
#define _GRLIB_GPTIMER_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/group */

/**
 * @defgroup RTEMSDeviceGRLIB GRLIB
 *
 * @ingroup RTEMSDeviceDrivers
 *
 * @brief This group contains the GRLIB interfaces.
 */

/* Generated from spec:/dev/grlib/if/gptimer-timer */

/**
 * @defgroup RTEMSDeviceGRLIBGPTIMERTimer GPTIMER TIMER
 *
 * @ingroup RTEMSDeviceGRLIBGPTIMER
 *
 * @brief This group contains the GPTIMER TIMER interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRLIBGPTIMERTimerTCNTVAL TCNTVAL
 *
 * @brief Timer n counter value register
 *
 * @{
 */

#define GPTIMER_TCNTVAL_TCVAL_SHIFT 0
#define GPTIMER_TCNTVAL_TCVAL_MASK 0xffffffffU
#define GPTIMER_TCNTVAL_TCVAL_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define GPTIMER_TCNTVAL_TCVAL( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBGPTIMERTimerTRLDVAL TRLDVAL
 *
 * @brief Timer n counter reload value register
 *
 * @{
 */

#define GPTIMER_TRLDVAL_TRLDVAL_SHIFT 0
#define GPTIMER_TRLDVAL_TRLDVAL_MASK 0xffffffffU
#define GPTIMER_TRLDVAL_TRLDVAL_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define GPTIMER_TRLDVAL_TRLDVAL( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBGPTIMERTimerTCTRL TCTRL
 *
 * @brief Timer n control register
 *
 * @{
 */

#define GPTIMER_TCTRL_WS 0x100U

#define GPTIMER_TCTRL_WN 0x80U

#define GPTIMER_TCTRL_DH 0x40U

#define GPTIMER_TCTRL_CH 0x20U

#define GPTIMER_TCTRL_IP 0x10U

#define GPTIMER_TCTRL_IE 0x8U

#define GPTIMER_TCTRL_LD 0x4U

#define GPTIMER_TCTRL_RS 0x2U

#define GPTIMER_TCTRL_EN 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBGPTIMERTimerTLATCH TLATCH
 *
 * @brief Timer n latch register
 *
 * @{
 */

#define GPTIMER_TLATCH_LTCV_SHIFT 0
#define GPTIMER_TLATCH_LTCV_MASK 0xffffffffU
#define GPTIMER_TLATCH_LTCV_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define GPTIMER_TLATCH_LTCV( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @brief This structure defines the GPTIMER TIMER register block memory map.
 */
typedef struct gptimer_timer {
  /**
   * @brief See @ref RTEMSDeviceGRLIBGPTIMERTimerTCNTVAL.
   */
  uint32_t tcntval;

  /**
   * @brief See @ref RTEMSDeviceGRLIBGPTIMERTimerTRLDVAL.
   */
  uint32_t trldval;

  /**
   * @brief See @ref RTEMSDeviceGRLIBGPTIMERTimerTCTRL.
   */
  uint32_t tctrl;

  /**
   * @brief See @ref RTEMSDeviceGRLIBGPTIMERTimerTLATCH.
   */
  uint32_t tlatch;
} gptimer_timer;

/** @} */

/* Generated from spec:/dev/grlib/if/gptimer */

/**
 * @defgroup RTEMSDeviceGRLIBGPTIMER GPTIMER
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the GPTIMER interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRLIBGPTIMERSCALER SCALER
 *
 * @brief Scaler value register
 *
 * @{
 */

#define GPTIMER_SCALER_SCALER_SHIFT 0
#define GPTIMER_SCALER_SCALER_MASK 0xffffU
#define GPTIMER_SCALER_SCALER_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffU )
#define GPTIMER_SCALER_SCALER( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBGPTIMERSRELOAD SRELOAD
 *
 * @brief Scaler reload value register
 *
 * @{
 */

#define GPTIMER_SRELOAD_SRELOAD_SHIFT 0
#define GPTIMER_SRELOAD_SRELOAD_MASK 0xffffU
#define GPTIMER_SRELOAD_SRELOAD_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffU )
#define GPTIMER_SRELOAD_SRELOAD( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBGPTIMERCONFIG CONFIG
 *
 * @brief Configuration register
 *
 * @{
 */

#define GPTIMER_CONFIG_EV 0x2000U

#define GPTIMER_CONFIG_ES 0x1000U

#define GPTIMER_CONFIG_EL 0x800U

#define GPTIMER_CONFIG_EE 0x400U

#define GPTIMER_CONFIG_DF 0x200U

#define GPTIMER_CONFIG_SI 0x100U

#define GPTIMER_CONFIG_IRQ_SHIFT 3
#define GPTIMER_CONFIG_IRQ_MASK 0xf8U
#define GPTIMER_CONFIG_IRQ_GET( _reg ) \
  ( ( ( _reg ) >> 3 ) & 0x1fU )
#define GPTIMER_CONFIG_IRQ( _val ) ( ( _val ) << 3 )

#define GPTIMER_CONFIG_TIMERS_SHIFT 0
#define GPTIMER_CONFIG_TIMERS_MASK 0x7U
#define GPTIMER_CONFIG_TIMERS_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x7U )
#define GPTIMER_CONFIG_TIMERS( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBGPTIMERLATCHCFG LATCHCFG
 *
 * @brief Timer latch configuration register
 *
 * @{
 */

#define GPTIMER_LATCHCFG_LATCHSEL_SHIFT 0
#define GPTIMER_LATCHCFG_LATCHSEL_MASK 0xffffffffU
#define GPTIMER_LATCHCFG_LATCHSEL_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define GPTIMER_LATCHCFG_LATCHSEL( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @brief This structure defines the GPTIMER register block memory map.
 */
typedef struct gptimer {
  /**
   * @brief See @ref RTEMSDeviceGRLIBGPTIMERSCALER.
   */
  uint32_t scaler;

  /**
   * @brief See @ref RTEMSDeviceGRLIBGPTIMERSRELOAD.
   */
  uint32_t sreload;

  /**
   * @brief See @ref RTEMSDeviceGRLIBGPTIMERCONFIG.
   */
  uint32_t config;

  /**
   * @brief See @ref RTEMSDeviceGRLIBGPTIMERLATCHCFG.
   */
  uint32_t latchcfg;

  /**
   * @brief See @ref RTEMSDeviceGRLIBGPTIMERTimer.
   */
  gptimer_timer timer[ 15 ];
} gptimer;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_GPTIMER_REGS_H */
