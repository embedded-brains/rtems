/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRSPWROUTER
 *
 * @brief This header file defines the GRSPWROUTER register block interface.
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

/* Generated from spec:/dev/grlib/if/grspwrouter-header */

#ifndef _GRLIB_GRSPWROUTER_REGS_H
#define _GRLIB_GRSPWROUTER_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/grspwrouter */

/**
 * @defgroup RTEMSDeviceGRSPWROUTER GRSPWROUTER
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the GRSPWROUTER interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERAMBACTRL AMBACTRL
 *
 * @brief AMBA port Control
 *
 * @{
 */

#define GRSPWROUTER_AMBACTRL_RA 0x80000000U

#define GRSPWROUTER_AMBACTRL_RX 0x40000000U

#define GRSPWROUTER_AMBACTRL_RC 0x20000000U

#define GRSPWROUTER_AMBACTRL_NCH_SHIFT 27
#define GRSPWROUTER_AMBACTRL_NCH_MASK 0x18000000U
#define GRSPWROUTER_AMBACTRL_NCH_GET( _reg ) \
  ( ( ( _reg ) >> 27 ) & 0x3U )
#define GRSPWROUTER_AMBACTRL_NCH( _val ) ( ( _val ) << 27 )

#define GRSPWROUTER_AMBACTRL_DI 0x1000000U

#define GRSPWROUTER_AMBACTRL_ME 0x800000U

#define GRSPWROUTER_AMBACTRL_RD 0x20000U

#define GRSPWROUTER_AMBACTRL_RE 0x10000U

#define GRSPWROUTER_AMBACTRL_TQ 0x100U

#define GRSPWROUTER_AMBACTRL_RS 0x40U

#define GRSPWROUTER_AMBACTRL_PM 0x20U

#define GRSPWROUTER_AMBACTRL_TI 0x10U

#define GRSPWROUTER_AMBACTRL_IE 0x8U

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERAMBASTS AMBASTS
 *
 * @brief AMBA port Status
 *
 * @{
 */

#define GRSPWROUTER_AMBASTS_NIRQ_SHIFT 28
#define GRSPWROUTER_AMBASTS_NIRQ_MASK 0x70000000U
#define GRSPWROUTER_AMBASTS_NIRQ_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0x7U )
#define GRSPWROUTER_AMBASTS_NIRQ( _val ) ( ( _val ) << 28 )

#define GRSPWROUTER_AMBASTS_NRXD_SHIFT 26
#define GRSPWROUTER_AMBASTS_NRXD_MASK 0xc000000U
#define GRSPWROUTER_AMBASTS_NRXD_GET( _reg ) \
  ( ( ( _reg ) >> 26 ) & 0x3U )
#define GRSPWROUTER_AMBASTS_NRXD( _val ) ( ( _val ) << 26 )

#define GRSPWROUTER_AMBASTS_NTXD_SHIFT 24
#define GRSPWROUTER_AMBASTS_NTXD_MASK 0x3000000U
#define GRSPWROUTER_AMBASTS_NTXD_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0x3U )
#define GRSPWROUTER_AMBASTS_NTXD( _val ) ( ( _val ) << 24 )

#define GRSPWROUTER_AMBASTS_ME 0x1000U

#define GRSPWROUTER_AMBASTS_EE 0x100U

#define GRSPWROUTER_AMBASTS_IA 0x80U

#define GRSPWROUTER_AMBASTS_TO 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERAMBADEFADDR AMBADEFADDR
 *
 * @brief AMBA port Default address
 *
 * @{
 */

#define GRSPWROUTER_AMBADEFADDR_DEFMASK_SHIFT 8
#define GRSPWROUTER_AMBADEFADDR_DEFMASK_MASK 0xff00U
#define GRSPWROUTER_AMBADEFADDR_DEFMASK_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xffU )
#define GRSPWROUTER_AMBADEFADDR_DEFMASK( _val ) ( ( _val ) << 8 )

#define GRSPWROUTER_AMBADEFADDR_DEFADDR_SHIFT 0
#define GRSPWROUTER_AMBADEFADDR_DEFADDR_MASK 0xffU
#define GRSPWROUTER_AMBADEFADDR_DEFADDR_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )
#define GRSPWROUTER_AMBADEFADDR_DEFADDR( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERAMBADKEY AMBADKEY
 *
 * @brief AMBA port Destination key
 *
 * @{
 */

#define GRSPWROUTER_AMBADKEY_DESTKEY_SHIFT 0
#define GRSPWROUTER_AMBADKEY_DESTKEY_MASK 0xffU
#define GRSPWROUTER_AMBADKEY_DESTKEY_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )
#define GRSPWROUTER_AMBADKEY_DESTKEY( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERAMBATC AMBATC
 *
 * @brief AMBA port Time-code
 *
 * @{
 */

#define GRSPWROUTER_AMBATC_TCMSK_SHIFT 24
#define GRSPWROUTER_AMBATC_TCMSK_MASK 0xff000000U
#define GRSPWROUTER_AMBATC_TCMSK_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xffU )
#define GRSPWROUTER_AMBATC_TCMSK( _val ) ( ( _val ) << 24 )

#define GRSPWROUTER_AMBATC_TCVAL_SHIFT 16
#define GRSPWROUTER_AMBATC_TCVAL_MASK 0xff0000U
#define GRSPWROUTER_AMBATC_TCVAL_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xffU )
#define GRSPWROUTER_AMBATC_TCVAL( _val ) ( ( _val ) << 16 )

#define GRSPWROUTER_AMBATC_TCTRL_SHIFT 6
#define GRSPWROUTER_AMBATC_TCTRL_MASK 0xc0U
#define GRSPWROUTER_AMBATC_TCTRL_GET( _reg ) \
  ( ( ( _reg ) >> 6 ) & 0x3U )
#define GRSPWROUTER_AMBATC_TCTRL( _val ) ( ( _val ) << 6 )

#define GRSPWROUTER_AMBATC_TIMECNT_SHIFT 0
#define GRSPWROUTER_AMBATC_TIMECNT_MASK 0x3fU
#define GRSPWROUTER_AMBATC_TIMECNT_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3fU )
#define GRSPWROUTER_AMBATC_TIMECNT( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERAMBADMACTRL AMBADMACTRL
 *
 * @brief AMBA port DMA control/status
 *
 * @{
 */

#define GRSPWROUTER_AMBADMACTRL_INTNUM_SHIFT 26
#define GRSPWROUTER_AMBADMACTRL_INTNUM_MASK 0xfc000000U
#define GRSPWROUTER_AMBADMACTRL_INTNUM_GET( _reg ) \
  ( ( ( _reg ) >> 26 ) & 0x3fU )
#define GRSPWROUTER_AMBADMACTRL_INTNUM( _val ) ( ( _val ) << 26 )

#define GRSPWROUTER_AMBADMACTRL_EP 0x800000U

#define GRSPWROUTER_AMBADMACTRL_TR 0x400000U

#define GRSPWROUTER_AMBADMACTRL_IE 0x200000U

#define GRSPWROUTER_AMBADMACTRL_IT 0x100000U

#define GRSPWROUTER_AMBADMACTRL_RP 0x80000U

#define GRSPWROUTER_AMBADMACTRL_TP 0x40000U

#define GRSPWROUTER_AMBADMACTRL_SP 0x8000U

#define GRSPWROUTER_AMBADMACTRL_SA 0x4000U

#define GRSPWROUTER_AMBADMACTRL_EN 0x2000U

#define GRSPWROUTER_AMBADMACTRL_NS 0x1000U

#define GRSPWROUTER_AMBADMACTRL_RD 0x800U

#define GRSPWROUTER_AMBADMACTRL_RX 0x400U

#define GRSPWROUTER_AMBADMACTRL_AT 0x200U

#define GRSPWROUTER_AMBADMACTRL_RA 0x100U

#define GRSPWROUTER_AMBADMACTRL_TA 0x80U

#define GRSPWROUTER_AMBADMACTRL_PR 0x40U

#define GRSPWROUTER_AMBADMACTRL_PS 0x20U

#define GRSPWROUTER_AMBADMACTRL_AI 0x10U

#define GRSPWROUTER_AMBADMACTRL_RI 0x8U

#define GRSPWROUTER_AMBADMACTRL_TI 0x4U

#define GRSPWROUTER_AMBADMACTRL_RE 0x2U

#define GRSPWROUTER_AMBADMACTRL_TE 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERAMBADMAMAXLEN AMBADMAMAXLEN
 *
 * @brief AMBA port DMA RX maximum length
 *
 * @{
 */

#define GRSPWROUTER_AMBADMAMAXLEN_RXMAXLEN_SHIFT 2
#define GRSPWROUTER_AMBADMAMAXLEN_RXMAXLEN_MASK 0x1fffffcU
#define GRSPWROUTER_AMBADMAMAXLEN_RXMAXLEN_GET( _reg ) \
  ( ( ( _reg ) >> 2 ) & 0x7fffffU )
#define GRSPWROUTER_AMBADMAMAXLEN_RXMAXLEN( _val ) ( ( _val ) << 2 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERAMBADMATXDESC AMBADMATXDESC
 *
 * @brief AMBA port DMA transmit descriptor table address
 *
 * @{
 */

#define GRSPWROUTER_AMBADMATXDESC_DESCBASEADDR_SHIFT 10
#define GRSPWROUTER_AMBADMATXDESC_DESCBASEADDR_MASK 0xfffffc00U
#define GRSPWROUTER_AMBADMATXDESC_DESCBASEADDR_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3fffffU )
#define GRSPWROUTER_AMBADMATXDESC_DESCBASEADDR( _val ) ( ( _val ) << 10 )

#define GRSPWROUTER_AMBADMATXDESC_DESCSEL_SHIFT 4
#define GRSPWROUTER_AMBADMATXDESC_DESCSEL_MASK 0x3f0U
#define GRSPWROUTER_AMBADMATXDESC_DESCSEL_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0x3fU )
#define GRSPWROUTER_AMBADMATXDESC_DESCSEL( _val ) ( ( _val ) << 4 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERAMBADMARXDESC AMBADMARXDESC
 *
 * @brief AMBA port DMA receive descriptor table address
 *
 * @{
 */

#define GRSPWROUTER_AMBADMARXDESC_DESCBASEADDR_SHIFT 10
#define GRSPWROUTER_AMBADMARXDESC_DESCBASEADDR_MASK 0xfffffc00U
#define GRSPWROUTER_AMBADMARXDESC_DESCBASEADDR_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3fffffU )
#define GRSPWROUTER_AMBADMARXDESC_DESCBASEADDR( _val ) ( ( _val ) << 10 )

#define GRSPWROUTER_AMBADMARXDESC_DESCSEL_SHIFT 3
#define GRSPWROUTER_AMBADMARXDESC_DESCSEL_MASK 0x3f8U
#define GRSPWROUTER_AMBADMARXDESC_DESCSEL_GET( _reg ) \
  ( ( ( _reg ) >> 3 ) & 0x7fU )
#define GRSPWROUTER_AMBADMARXDESC_DESCSEL( _val ) ( ( _val ) << 3 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERAMBADMAADDR AMBADMAADDR
 *
 * @brief AMBA port DMA address
 *
 * @{
 */

#define GRSPWROUTER_AMBADMAADDR_MASK_SHIFT 8
#define GRSPWROUTER_AMBADMAADDR_MASK_MASK 0xff00U
#define GRSPWROUTER_AMBADMAADDR_MASK_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xffU )
#define GRSPWROUTER_AMBADMAADDR_MASK( _val ) ( ( _val ) << 8 )

#define GRSPWROUTER_AMBADMAADDR_ADDR_SHIFT 0
#define GRSPWROUTER_AMBADMAADDR_ADDR_MASK 0xffU
#define GRSPWROUTER_AMBADMAADDR_ADDR_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )
#define GRSPWROUTER_AMBADMAADDR_ADDR( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERAMBAINTCTRL AMBAINTCTRL
 *
 * @brief AMBA port Distributed interrupt control
 *
 * @{
 */

#define GRSPWROUTER_AMBAINTCTRL_INTNUM_SHIFT 26
#define GRSPWROUTER_AMBAINTCTRL_INTNUM_MASK 0xfc000000U
#define GRSPWROUTER_AMBAINTCTRL_INTNUM_GET( _reg ) \
  ( ( ( _reg ) >> 26 ) & 0x3fU )
#define GRSPWROUTER_AMBAINTCTRL_INTNUM( _val ) ( ( _val ) << 26 )

#define GRSPWROUTER_AMBAINTCTRL_EE 0x1000000U

#define GRSPWROUTER_AMBAINTCTRL_IA 0x800000U

#define GRSPWROUTER_AMBAINTCTRL_TQ 0x100000U

#define GRSPWROUTER_AMBAINTCTRL_AQ 0x80000U

#define GRSPWROUTER_AMBAINTCTRL_IQ 0x40000U

#define GRSPWROUTER_AMBAINTCTRL_AA 0x8000U

#define GRSPWROUTER_AMBAINTCTRL_AT 0x4000U

#define GRSPWROUTER_AMBAINTCTRL_IT 0x2000U

#define GRSPWROUTER_AMBAINTCTRL_ID 0x80U

#define GRSPWROUTER_AMBAINTCTRL_II 0x40U

#define GRSPWROUTER_AMBAINTCTRL_TXINT_SHIFT 0
#define GRSPWROUTER_AMBAINTCTRL_TXINT_MASK 0x3fU
#define GRSPWROUTER_AMBAINTCTRL_TXINT_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3fU )
#define GRSPWROUTER_AMBAINTCTRL_TXINT( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERAMBAINTRX AMBAINTRX
 *
 * @brief AMBA port Interrupt receive
 *
 * @{
 */

#define GRSPWROUTER_AMBAINTRX_RXIRQ_SHIFT 0
#define GRSPWROUTER_AMBAINTRX_RXIRQ_MASK 0xffffffffU
#define GRSPWROUTER_AMBAINTRX_RXIRQ_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define GRSPWROUTER_AMBAINTRX_RXIRQ( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERAMBAACKRX AMBAACKRX
 *
 * @brief AMBA port Interrupt acknowledgement / extended interrupt receive
 *
 * @{
 */

#define GRSPWROUTER_AMBAACKRX_RXACK_SHIFT 0
#define GRSPWROUTER_AMBAACKRX_RXACK_MASK 0xffffffffU
#define GRSPWROUTER_AMBAACKRX_RXACK_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define GRSPWROUTER_AMBAACKRX_RXACK( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERAMBAINTTO0 AMBAINTTO0
 *
 * @brief AMBA port Interrupt timeout, interrupt 0-31
 *
 * @{
 */

#define GRSPWROUTER_AMBAINTTO0_INTTO_SHIFT 0
#define GRSPWROUTER_AMBAINTTO0_INTTO_MASK 0xffffffffU
#define GRSPWROUTER_AMBAINTTO0_INTTO_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define GRSPWROUTER_AMBAINTTO0_INTTO( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERAMBAINTTO1 AMBAINTTO1
 *
 * @brief AMBA port Interrupt timeout, interrupt 32-63
 *
 * @{
 */

#define GRSPWROUTER_AMBAINTTO1_INTTO_SHIFT 0
#define GRSPWROUTER_AMBAINTTO1_INTTO_MASK 0xffffffffU
#define GRSPWROUTER_AMBAINTTO1_INTTO_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define GRSPWROUTER_AMBAINTTO1_INTTO( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERAMBAINTMSK0 AMBAINTMSK0
 *
 * @brief AMBA port Interrupt mask, interrupt 0-31
 *
 * @{
 */

#define GRSPWROUTER_AMBAINTMSK0_MASK_SHIFT 0
#define GRSPWROUTER_AMBAINTMSK0_MASK_MASK 0xffffffffU
#define GRSPWROUTER_AMBAINTMSK0_MASK_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define GRSPWROUTER_AMBAINTMSK0_MASK( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRSPWROUTERAMBAINTMSK1 AMBAINTMSK1
 *
 * @brief AMBA port Interrupt mask, interrupt 32-63
 *
 * @{
 */

#define GRSPWROUTER_AMBAINTMSK1_MASK_SHIFT 0
#define GRSPWROUTER_AMBAINTMSK1_MASK_MASK 0xffffffffU
#define GRSPWROUTER_AMBAINTMSK1_MASK_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define GRSPWROUTER_AMBAINTMSK1_MASK( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @brief This structure defines the GRSPWROUTER register block memory map.
 */
typedef struct grspwrouter {
  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBACTRL.
   */
  uint32_t ambactrl;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBASTS.
   */
  uint32_t ambasts;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBADEFADDR.
   */
  uint32_t ambadefaddr;

  uint32_t reserved_c_10;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBADKEY.
   */
  uint32_t ambadkey;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBATC.
   */
  uint32_t ambatc;

  uint32_t reserved_18_20[ 2 ];

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBADMACTRL.
   */
  uint32_t ambadmactrl_0;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBADMAMAXLEN.
   */
  uint32_t ambadmamaxlen_0;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBADMATXDESC.
   */
  uint32_t ambadmatxdesc_0;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBADMARXDESC.
   */
  uint32_t ambadmarxdesc_0;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBADMAADDR.
   */
  uint32_t ambadmaaddr_0;

  uint32_t reserved_34_40[ 3 ];

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBADMACTRL.
   */
  uint32_t ambadmactrl_1;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBADMAMAXLEN.
   */
  uint32_t ambadmamaxlen_1;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBADMATXDESC.
   */
  uint32_t ambadmatxdesc_1;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBADMARXDESC.
   */
  uint32_t ambadmarxdesc_1;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBADMAADDR.
   */
  uint32_t ambadmaaddr_1;

  uint32_t reserved_54_60[ 3 ];

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBADMACTRL.
   */
  uint32_t ambadmactrl_2;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBADMAMAXLEN.
   */
  uint32_t ambadmamaxlen_2;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBADMATXDESC.
   */
  uint32_t ambadmatxdesc_2;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBADMARXDESC.
   */
  uint32_t ambadmarxdesc_2;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBADMAADDR.
   */
  uint32_t ambadmaaddr_2;

  uint32_t reserved_74_80[ 3 ];

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBADMACTRL.
   */
  uint32_t ambadmactrl_3;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBADMAMAXLEN.
   */
  uint32_t ambadmamaxlen_3;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBADMATXDESC.
   */
  uint32_t ambadmatxdesc_3;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBADMARXDESC.
   */
  uint32_t ambadmarxdesc_3;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBADMAADDR.
   */
  uint32_t ambadmaaddr_3;

  uint32_t reserved_94_a0[ 3 ];

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBAINTCTRL.
   */
  uint32_t ambaintctrl;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBAINTRX.
   */
  uint32_t ambaintrx;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBAACKRX.
   */
  uint32_t ambaackrx;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBAINTTO0.
   */
  uint32_t ambaintto0;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBAINTTO1.
   */
  uint32_t ambaintto1;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBAINTMSK0.
   */
  uint32_t ambaintmsk0;

  /**
   * @brief See @ref RTEMSDeviceGRSPWROUTERAMBAINTMSK1.
   */
  uint32_t ambaintmsk1;
} grspwrouter;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_GRSPWROUTER_REGS_H */
