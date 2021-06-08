/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRETHGBIT
 *
 * @brief This header file defines the GRETH_GBIT register block interface.
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

/* Generated from spec:/dev/grlib/if/grethgbit-header */

#ifndef _GRLIB_GRETHGBIT_REGS_H
#define _GRLIB_GRETHGBIT_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/grethgbit */

/**
 * @defgroup RTEMSDeviceGRETHGBIT GRETH_GBIT
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the GRETH_GBIT interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRETHGBITCR CR
 *
 * @brief control register
 *
 * @{
 */

#define GRETHGBIT_CR_EA 0x80000000U

#define GRETHGBIT_CR_BS_SHIFT 28
#define GRETHGBIT_CR_BS_MASK 0x70000000U
#define GRETHGBIT_CR_BS_GET( _reg ) \
  ( ( ( _reg ) >> 28 ) & 0x7U )
#define GRETHGBIT_CR_BS( _val ) ( ( _val ) << 28 )

#define GRETHGBIT_CR_GA 0x8000000U

#define GRETHGBIT_CR_MA 0x4000000U

#define GRETHGBIT_CR_MC 0x2000000U

#define GRETHGBIT_CR_ED 0x4000U

#define GRETHGBIT_CR_RD 0x2000U

#define GRETHGBIT_CR_DD 0x1000U

#define GRETHGBIT_CR_ME 0x800U

#define GRETHGBIT_CR_PI 0x400U

#define GRETHGBIT_CR_BM 0x200U

#define GRETHGBIT_CR_GB 0x100U

#define GRETHGBIT_CR_SP 0x80U

#define GRETHGBIT_CR_RS 0x40U

#define GRETHGBIT_CR_PM 0x20U

#define GRETHGBIT_CR_FD 0x10U

#define GRETHGBIT_CR_RI 0x8U

#define GRETHGBIT_CR_TI 0x4U

#define GRETHGBIT_CR_RE 0x2U

#define GRETHGBIT_CR_TE 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITSR SR
 *
 * @brief status register.
 *
 * @{
 */

#define GRETHGBIT_SR_PS 0x100U

#define GRETHGBIT_SR_IA 0x80U

#define GRETHGBIT_SR_TS 0x40U

#define GRETHGBIT_SR_TA 0x20U

#define GRETHGBIT_SR_RA 0x10U

#define GRETHGBIT_SR_TI 0x8U

#define GRETHGBIT_SR_RI 0x4U

#define GRETHGBIT_SR_TE 0x2U

#define GRETHGBIT_SR_RE 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITMACMSB MACMSB
 *
 * @brief MAC address MSB.
 *
 * @{
 */

#define GRETHGBIT_MACMSB_MSB_SHIFT 0
#define GRETHGBIT_MACMSB_MSB_MASK 0xffffU
#define GRETHGBIT_MACMSB_MSB_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffU )
#define GRETHGBIT_MACMSB_MSB( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITMACLSB MACLSB
 *
 * @brief MAC address LSB.
 *
 * @{
 */

#define GRETHGBIT_MACLSB_LSB_SHIFT 0
#define GRETHGBIT_MACLSB_LSB_MASK 0xffffffffU
#define GRETHGBIT_MACLSB_LSB_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define GRETHGBIT_MACLSB_LSB( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITMDIO MDIO
 *
 * @brief MDIO control/status register.
 *
 * @{
 */

#define GRETHGBIT_MDIO_DATA_SHIFT 16
#define GRETHGBIT_MDIO_DATA_MASK 0xffff0000U
#define GRETHGBIT_MDIO_DATA_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xffffU )
#define GRETHGBIT_MDIO_DATA( _val ) ( ( _val ) << 16 )

#define GRETHGBIT_MDIO_PHYADDR_SHIFT 11
#define GRETHGBIT_MDIO_PHYADDR_MASK 0xf800U
#define GRETHGBIT_MDIO_PHYADDR_GET( _reg ) \
  ( ( ( _reg ) >> 11 ) & 0x1fU )
#define GRETHGBIT_MDIO_PHYADDR( _val ) ( ( _val ) << 11 )

#define GRETHGBIT_MDIO_REGADDR_SHIFT 6
#define GRETHGBIT_MDIO_REGADDR_MASK 0x7c0U
#define GRETHGBIT_MDIO_REGADDR_GET( _reg ) \
  ( ( ( _reg ) >> 6 ) & 0x1fU )
#define GRETHGBIT_MDIO_REGADDR( _val ) ( ( _val ) << 6 )

#define GRETHGBIT_MDIO_BU 0x8U

#define GRETHGBIT_MDIO_LF 0x4U

#define GRETHGBIT_MDIO_RD 0x2U

#define GRETHGBIT_MDIO_WR 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITTDTBA TDTBA
 *
 * @brief transmitter descriptor table base address register.
 *
 * @{
 */

#define GRETHGBIT_TDTBA_BASEADDR_SHIFT 10
#define GRETHGBIT_TDTBA_BASEADDR_MASK 0xfffffc00U
#define GRETHGBIT_TDTBA_BASEADDR_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3fffffU )
#define GRETHGBIT_TDTBA_BASEADDR( _val ) ( ( _val ) << 10 )

#define GRETHGBIT_TDTBA_DESCPNT_SHIFT 3
#define GRETHGBIT_TDTBA_DESCPNT_MASK 0x3f8U
#define GRETHGBIT_TDTBA_DESCPNT_GET( _reg ) \
  ( ( ( _reg ) >> 3 ) & 0x7fU )
#define GRETHGBIT_TDTBA_DESCPNT( _val ) ( ( _val ) << 3 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITRDTBA RDTBA
 *
 * @brief receiver descriptor table base address register.
 *
 * @{
 */

#define GRETHGBIT_RDTBA_BASEADDR_SHIFT 10
#define GRETHGBIT_RDTBA_BASEADDR_MASK 0xfffffc00U
#define GRETHGBIT_RDTBA_BASEADDR_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3fffffU )
#define GRETHGBIT_RDTBA_BASEADDR( _val ) ( ( _val ) << 10 )

#define GRETHGBIT_RDTBA_DESCPNT_SHIFT 3
#define GRETHGBIT_RDTBA_DESCPNT_MASK 0x3f8U
#define GRETHGBIT_RDTBA_DESCPNT_GET( _reg ) \
  ( ( ( _reg ) >> 3 ) & 0x7fU )
#define GRETHGBIT_RDTBA_DESCPNT( _val ) ( ( _val ) << 3 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITEDCLMACMSB EDCLMACMSB
 *
 * @brief EDCL MAC address MSB.
 *
 * @{
 */

#define GRETHGBIT_EDCLMACMSB_MSB_SHIFT 0
#define GRETHGBIT_EDCLMACMSB_MSB_MASK 0xffffU
#define GRETHGBIT_EDCLMACMSB_MSB_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffU )
#define GRETHGBIT_EDCLMACMSB_MSB( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITEDCLMACLSB EDCLMACLSB
 *
 * @brief EDCL MAC address LSB.
 *
 * @{
 */

#define GRETHGBIT_EDCLMACLSB_LSB_SHIFT 0
#define GRETHGBIT_EDCLMACLSB_LSB_MASK 0xffffffffU
#define GRETHGBIT_EDCLMACLSB_LSB_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define GRETHGBIT_EDCLMACLSB_LSB( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @brief This structure defines the GRETH_GBIT register block memory map.
 */
typedef struct grethgbit {
  /**
   * @brief See @ref RTEMSDeviceGRETHGBITCR.
   */
  uint32_t cr;

  /**
   * @brief See @ref RTEMSDeviceGRETHGBITSR.
   */
  uint32_t sr;

  /**
   * @brief See @ref RTEMSDeviceGRETHGBITMACMSB.
   */
  uint32_t macmsb;

  /**
   * @brief See @ref RTEMSDeviceGRETHGBITMACLSB.
   */
  uint32_t maclsb;

  /**
   * @brief See @ref RTEMSDeviceGRETHGBITMDIO.
   */
  uint32_t mdio;

  /**
   * @brief See @ref RTEMSDeviceGRETHGBITTDTBA.
   */
  uint32_t tdtba;

  /**
   * @brief See @ref RTEMSDeviceGRETHGBITRDTBA.
   */
  uint32_t rdtba;

  uint32_t reserved_1c_28[ 3 ];

  /**
   * @brief See @ref RTEMSDeviceGRETHGBITEDCLMACMSB.
   */
  uint32_t edclmacmsb;

  /**
   * @brief See @ref RTEMSDeviceGRETHGBITEDCLMACLSB.
   */
  uint32_t edclmaclsb;
} grethgbit;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_GRETHGBIT_REGS_H */
