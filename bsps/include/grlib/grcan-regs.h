/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRCAN
 *
 * @brief This header file defines the GRCAN register block interface.
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

/* Generated from spec:/dev/grlib/if/grcan-header */

#ifndef _GRLIB_GRCAN_REGS_H
#define _GRLIB_GRCAN_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/grcan */

/**
 * @defgroup RTEMSDeviceGRCAN GRCAN
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the GRCAN interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRCANCanCONF CanCONF
 *
 * @brief Configuration Register
 *
 * @{
 */

#define GRCAN_CANCONF_SCALER_SHIFT 24
#define GRCAN_CANCONF_SCALER_MASK 0xff000000U
#define GRCAN_CANCONF_SCALER_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xffU )
#define GRCAN_CANCONF_SCALER( _val ) ( ( _val ) << 24 )

#define GRCAN_CANCONF_PS1_SHIFT 20
#define GRCAN_CANCONF_PS1_MASK 0xf00000U
#define GRCAN_CANCONF_PS1_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfU )
#define GRCAN_CANCONF_PS1( _val ) ( ( _val ) << 20 )

#define GRCAN_CANCONF_PS2_SHIFT 16
#define GRCAN_CANCONF_PS2_MASK 0xf0000U
#define GRCAN_CANCONF_PS2_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xfU )
#define GRCAN_CANCONF_PS2( _val ) ( ( _val ) << 16 )

#define GRCAN_CANCONF_RSJ_SHIFT 12
#define GRCAN_CANCONF_RSJ_MASK 0x7000U
#define GRCAN_CANCONF_RSJ_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0x7U )
#define GRCAN_CANCONF_RSJ( _val ) ( ( _val ) << 12 )

#define GRCAN_CANCONF_BPR_SHIFT 8
#define GRCAN_CANCONF_BPR_MASK 0x300U
#define GRCAN_CANCONF_BPR_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0x3U )
#define GRCAN_CANCONF_BPR( _val ) ( ( _val ) << 8 )

#define GRCAN_CANCONF_SAM 0x20U

#define GRCAN_CANCONF_SILNT 0x10U

#define GRCAN_CANCONF_SELECT 0x8U

#define GRCAN_CANCONF_ENABLE1 0x4U

#define GRCAN_CANCONF_ENABLE0 0x2U

#define GRCAN_CANCONF_ABORT 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanSTAT CanSTAT
 *
 * @brief Status Register
 *
 * @{
 */

#define GRCAN_CANSTAT_TXCHANNELS_SHIFT 28
#define GRCAN_CANSTAT_TXCHANNELS_MASK 0xf0000000U
#define GRCAN_CANSTAT_TXCHANNELS_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0xfU )
#define GRCAN_CANSTAT_TXCHANNELS( _val ) ( ( _val ) << 28 )

#define GRCAN_CANSTAT_RXCHANNELS_SHIFT 24
#define GRCAN_CANSTAT_RXCHANNELS_MASK 0xf000000U
#define GRCAN_CANSTAT_RXCHANNELS_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xfU )
#define GRCAN_CANSTAT_RXCHANNELS( _val ) ( ( _val ) << 24 )

#define GRCAN_CANSTAT_TXERRCNT_SHIFT 16
#define GRCAN_CANSTAT_TXERRCNT_MASK 0xff0000U
#define GRCAN_CANSTAT_TXERRCNT_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xffU )
#define GRCAN_CANSTAT_TXERRCNT( _val ) ( ( _val ) << 16 )

#define GRCAN_CANSTAT_RXERRCNT_SHIFT 8
#define GRCAN_CANSTAT_RXERRCNT_MASK 0xff00U
#define GRCAN_CANSTAT_RXERRCNT_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xffU )
#define GRCAN_CANSTAT_RXERRCNT( _val ) ( ( _val ) << 8 )

#define GRCAN_CANSTAT_ACTIVE 0x10U

#define GRCAN_CANSTAT_AHBERR 0x8U

#define GRCAN_CANSTAT_OR 0x4U

#define GRCAN_CANSTAT_OFF 0x2U

#define GRCAN_CANSTAT_PASS 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanCTRL CanCTRL
 *
 * @brief Control Register
 *
 * @{
 */

#define GRCAN_CANCTRL_RESET 0x2U

#define GRCAN_CANCTRL_ENABLE 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanMASK CanMASK
 *
 * @brief SYNC Mask Filter Register
 *
 * @{
 */

#define GRCAN_CANMASK_MASK_SHIFT 0
#define GRCAN_CANMASK_MASK_MASK 0x1fffffffU
#define GRCAN_CANMASK_MASK_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x1fffffffU )
#define GRCAN_CANMASK_MASK( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanCODE CanCODE
 *
 * @brief SYNC Code Filter Register
 *
 * @{
 */

#define GRCAN_CANCODE_SYNC_SHIFT 0
#define GRCAN_CANCODE_SYNC_MASK 0x1fffffffU
#define GRCAN_CANCODE_SYNC_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x1fffffffU )
#define GRCAN_CANCODE_SYNC( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanTxCTRL CanTxCTRL
 *
 * @brief Transmit Channel Control Register
 *
 * @{
 */

#define GRCAN_CANTXCTRL_SINGLE 0x4U

#define GRCAN_CANTXCTRL_ONGOING 0x2U

#define GRCAN_CANTXCTRL_ENABLE 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanTxADDR CanTxADDR
 *
 * @brief Transmit Channel Address Register
 *
 * @{
 */

#define GRCAN_CANTXADDR_ADDR_SHIFT 10
#define GRCAN_CANTXADDR_ADDR_MASK 0xfffffc00U
#define GRCAN_CANTXADDR_ADDR_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3fffffU )
#define GRCAN_CANTXADDR_ADDR( _val ) ( ( _val ) << 10 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanTxSIZE CanTxSIZE
 *
 * @brief Transmit Channel Size Register
 *
 * @{
 */

#define GRCAN_CANTXSIZE_SIZE_SHIFT 6
#define GRCAN_CANTXSIZE_SIZE_MASK 0x1fffc0U
#define GRCAN_CANTXSIZE_SIZE_GET( _reg ) \
  ( ( ( _reg ) >> 6 ) & 0x7fffU )
#define GRCAN_CANTXSIZE_SIZE( _val ) ( ( _val ) << 6 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanTxWR CanTxWR
 *
 * @brief Transmit Channel Write Register
 *
 * @{
 */

#define GRCAN_CANTXWR_WRITE_SHIFT 4
#define GRCAN_CANTXWR_WRITE_MASK 0xffff0U
#define GRCAN_CANTXWR_WRITE_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xffffU )
#define GRCAN_CANTXWR_WRITE( _val ) ( ( _val ) << 4 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanTxRD CanTxRD
 *
 * @brief Transmit Channel Read Register
 *
 * @{
 */

#define GRCAN_CANTXRD_READ_SHIFT 4
#define GRCAN_CANTXRD_READ_MASK 0xffff0U
#define GRCAN_CANTXRD_READ_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xffffU )
#define GRCAN_CANTXRD_READ( _val ) ( ( _val ) << 4 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanTxRD CanTxRD
 *
 * @brief Transmit Channel Read Register
 *
 * @{
 */

#define GRCAN_CANTXRD_IRQ_SHIFT 4
#define GRCAN_CANTXRD_IRQ_MASK 0xffff0U
#define GRCAN_CANTXRD_IRQ_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xffffU )
#define GRCAN_CANTXRD_IRQ( _val ) ( ( _val ) << 4 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanRxCTRL CanRxCTRL
 *
 * @brief Receive Channel Control Register
 *
 * @{
 */

#define GRCAN_CANRXCTRL_ONGOING 0x2U

#define GRCAN_CANRXCTRL_ENABLE 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanRxADDR CanRxADDR
 *
 * @brief Receive Channel Address Register
 *
 * @{
 */

#define GRCAN_CANRXADDR_ADDR_SHIFT 10
#define GRCAN_CANRXADDR_ADDR_MASK 0xfffffc00U
#define GRCAN_CANRXADDR_ADDR_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3fffffU )
#define GRCAN_CANRXADDR_ADDR( _val ) ( ( _val ) << 10 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanRxSIZE CanRxSIZE
 *
 * @brief Receive Channel Size Register
 *
 * @{
 */

#define GRCAN_CANRXSIZE_SIZE_SHIFT 6
#define GRCAN_CANRXSIZE_SIZE_MASK 0x1fffc0U
#define GRCAN_CANRXSIZE_SIZE_GET( _reg ) \
  ( ( ( _reg ) >> 6 ) & 0x7fffU )
#define GRCAN_CANRXSIZE_SIZE( _val ) ( ( _val ) << 6 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanRxWR CanRxWR
 *
 * @brief Receive Channel Write Register
 *
 * @{
 */

#define GRCAN_CANRXWR_WRITE_SHIFT 4
#define GRCAN_CANRXWR_WRITE_MASK 0xffff0U
#define GRCAN_CANRXWR_WRITE_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xffffU )
#define GRCAN_CANRXWR_WRITE( _val ) ( ( _val ) << 4 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanRxRD CanRxRD
 *
 * @brief Receive Channel Read Register
 *
 * @{
 */

#define GRCAN_CANRXRD_READ_SHIFT 4
#define GRCAN_CANRXRD_READ_MASK 0xffff0U
#define GRCAN_CANRXRD_READ_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xffffU )
#define GRCAN_CANRXRD_READ( _val ) ( ( _val ) << 4 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanRxIRQ CanRxIRQ
 *
 * @brief Receive Channel Interrupt Register
 *
 * @{
 */

#define GRCAN_CANRXIRQ_IRQ_SHIFT 4
#define GRCAN_CANRXIRQ_IRQ_MASK 0xffff0U
#define GRCAN_CANRXIRQ_IRQ_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0xffffU )
#define GRCAN_CANRXIRQ_IRQ( _val ) ( ( _val ) << 4 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanRxMASK CanRxMASK
 *
 * @brief Receive Channel Mask Register
 *
 * @{
 */

#define GRCAN_CANRXMASK_AM_SHIFT 0
#define GRCAN_CANRXMASK_AM_MASK 0x1fffffffU
#define GRCAN_CANRXMASK_AM_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x1fffffffU )
#define GRCAN_CANRXMASK_AM( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRCANCanRxCODE CanRxCODE
 *
 * @brief Receive Channel Code Register
 *
 * @{
 */

#define GRCAN_CANRXCODE_AC_SHIFT 0
#define GRCAN_CANRXCODE_AC_MASK 0x1fffffffU
#define GRCAN_CANRXCODE_AC_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x1fffffffU )
#define GRCAN_CANRXCODE_AC( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @brief This structure defines the GRCAN register block memory map.
 */
typedef struct grcan {
  /**
   * @brief See @ref RTEMSDeviceGRCANCanCONF.
   */
  uint32_t canconf;

  /**
   * @brief See @ref RTEMSDeviceGRCANCanSTAT.
   */
  uint32_t canstat;

  /**
   * @brief See @ref RTEMSDeviceGRCANCanCTRL.
   */
  uint32_t canctrl;

  uint32_t reserved_c_18[ 3 ];

  /**
   * @brief See @ref RTEMSDeviceGRCANCanMASK.
   */
  uint32_t canmask;

  /**
   * @brief See @ref RTEMSDeviceGRCANCanCODE.
   */
  uint32_t cancode;

  uint32_t reserved_20_200[ 120 ];

  /**
   * @brief See @ref RTEMSDeviceGRCANCanTxCTRL.
   */
  uint32_t cantxctrl;

  /**
   * @brief See @ref RTEMSDeviceGRCANCanTxADDR.
   */
  uint32_t cantxaddr;

  /**
   * @brief See @ref RTEMSDeviceGRCANCanTxSIZE.
   */
  uint32_t cantxsize;

  /**
   * @brief See @ref RTEMSDeviceGRCANCanTxWR.
   */
  uint32_t cantxwr;

  /**
   * @brief See @ref RTEMSDeviceGRCANCanTxRD.
   */
  uint32_t cantxrd_0;

  /**
   * @brief See @ref RTEMSDeviceGRCANCanTxRD.
   */
  uint32_t cantxrd_1;

  uint32_t reserved_218_300[ 58 ];

  /**
   * @brief See @ref RTEMSDeviceGRCANCanRxCTRL.
   */
  uint32_t canrxctrl;

  /**
   * @brief See @ref RTEMSDeviceGRCANCanRxADDR.
   */
  uint32_t canrxaddr;

  /**
   * @brief See @ref RTEMSDeviceGRCANCanRxSIZE.
   */
  uint32_t canrxsize;

  /**
   * @brief See @ref RTEMSDeviceGRCANCanRxWR.
   */
  uint32_t canrxwr;

  /**
   * @brief See @ref RTEMSDeviceGRCANCanRxRD.
   */
  uint32_t canrxrd;

  /**
   * @brief See @ref RTEMSDeviceGRCANCanRxIRQ.
   */
  uint32_t canrxirq;

  /**
   * @brief See @ref RTEMSDeviceGRCANCanRxMASK.
   */
  uint32_t canrxmask;

  /**
   * @brief See @ref RTEMSDeviceGRCANCanRxCODE.
   */
  uint32_t canrxcode;
} grcan;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_GRCAN_REGS_H */
