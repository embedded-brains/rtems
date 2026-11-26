/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRETHGBIT
 *
 * @brief This header file defines the GRETH_GBIT register block interface.
 */

/*
 * Copyright (C) 2021, 2026 embedded brains GmbH & Co. KG
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

/* Find related documentation with spec:/dev/grlib/if/grethgbit-header */

#ifndef _GRLIB_GRETHGBIT_REGS_H
#define _GRLIB_GRETHGBIT_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Find related documentation with spec:/dev/grlib/if/grethgbit */

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
 * @defgroup RTEMSDeviceGRETHGBITCR control register (CR)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRETHGBIT_CR_EA 0x80000000U

#define GRETHGBIT_CR_BS_SHIFT 28
#define GRETHGBIT_CR_BS_MASK  0x70000000U
#define GRETHGBIT_CR_BS_GET( _reg ) \
  ( ( ( _reg ) & GRETHGBIT_CR_BS_MASK ) >> GRETHGBIT_CR_BS_SHIFT )
#define GRETHGBIT_CR_BS_SET( _reg, _val )  \
  ( ( ( _reg ) & ~GRETHGBIT_CR_BS_MASK ) | \
    ( ( ( _val ) << GRETHGBIT_CR_BS_SHIFT ) & GRETHGBIT_CR_BS_MASK ) )
#define GRETHGBIT_CR_BS( _val ) \
  ( ( ( _val ) << GRETHGBIT_CR_BS_SHIFT ) & GRETHGBIT_CR_BS_MASK )

#define GRETHGBIT_CR_GA 0x8000000U

#define GRETHGBIT_CR_MA 0x4000000U

#define GRETHGBIT_CR_MC 0x2000000U

#define GRETHGBIT_CR_TC 0x800000U

#define GRETHGBIT_CR_TS 0x8000U

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
 * @defgroup RTEMSDeviceGRETHGBITSR status register. (SR)
 *
 * @brief This group contains register bit definitions.
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
 * @defgroup RTEMSDeviceGRETHGBITMACMSB MAC address MSB. (MACMSB)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRETHGBIT_MACMSB_MSB_SHIFT 0
#define GRETHGBIT_MACMSB_MSB_MASK  0xffffU
#define GRETHGBIT_MACMSB_MSB_GET( _reg ) \
  ( ( ( _reg ) & GRETHGBIT_MACMSB_MSB_MASK ) >> GRETHGBIT_MACMSB_MSB_SHIFT )
#define GRETHGBIT_MACMSB_MSB_SET( _reg, _val )     \
  ( ( ( _reg ) & ~GRETHGBIT_MACMSB_MSB_MASK ) |    \
    ( ( ( _val ) << GRETHGBIT_MACMSB_MSB_SHIFT ) & \
      GRETHGBIT_MACMSB_MSB_MASK ) )
#define GRETHGBIT_MACMSB_MSB( _val ) \
  ( ( ( _val ) << GRETHGBIT_MACMSB_MSB_SHIFT ) & GRETHGBIT_MACMSB_MSB_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITMACLSB MAC address LSB. (MACLSB)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRETHGBIT_MACLSB_LSB_SHIFT 0
#define GRETHGBIT_MACLSB_LSB_MASK  0xffffffffU
#define GRETHGBIT_MACLSB_LSB_GET( _reg ) \
  ( ( ( _reg ) & GRETHGBIT_MACLSB_LSB_MASK ) >> GRETHGBIT_MACLSB_LSB_SHIFT )
#define GRETHGBIT_MACLSB_LSB_SET( _reg, _val )     \
  ( ( ( _reg ) & ~GRETHGBIT_MACLSB_LSB_MASK ) |    \
    ( ( ( _val ) << GRETHGBIT_MACLSB_LSB_SHIFT ) & \
      GRETHGBIT_MACLSB_LSB_MASK ) )
#define GRETHGBIT_MACLSB_LSB( _val ) \
  ( ( ( _val ) << GRETHGBIT_MACLSB_LSB_SHIFT ) & GRETHGBIT_MACLSB_LSB_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITMDIO MDIO control/status register. (MDIO)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRETHGBIT_MDIO_DATA_SHIFT 16
#define GRETHGBIT_MDIO_DATA_MASK  0xffff0000U
#define GRETHGBIT_MDIO_DATA_GET( _reg ) \
  ( ( ( _reg ) & GRETHGBIT_MDIO_DATA_MASK ) >> GRETHGBIT_MDIO_DATA_SHIFT )
#define GRETHGBIT_MDIO_DATA_SET( _reg, _val )     \
  ( ( ( _reg ) & ~GRETHGBIT_MDIO_DATA_MASK ) |    \
    ( ( ( _val ) << GRETHGBIT_MDIO_DATA_SHIFT ) & \
      GRETHGBIT_MDIO_DATA_MASK ) )
#define GRETHGBIT_MDIO_DATA( _val ) \
  ( ( ( _val ) << GRETHGBIT_MDIO_DATA_SHIFT ) & GRETHGBIT_MDIO_DATA_MASK )

#define GRETHGBIT_MDIO_PHYADDR_SHIFT 11
#define GRETHGBIT_MDIO_PHYADDR_MASK  0xf800U
#define GRETHGBIT_MDIO_PHYADDR_GET( _reg )        \
  ( ( ( _reg ) & GRETHGBIT_MDIO_PHYADDR_MASK ) >> \
    GRETHGBIT_MDIO_PHYADDR_SHIFT )
#define GRETHGBIT_MDIO_PHYADDR_SET( _reg, _val )     \
  ( ( ( _reg ) & ~GRETHGBIT_MDIO_PHYADDR_MASK ) |    \
    ( ( ( _val ) << GRETHGBIT_MDIO_PHYADDR_SHIFT ) & \
      GRETHGBIT_MDIO_PHYADDR_MASK ) )
#define GRETHGBIT_MDIO_PHYADDR( _val )             \
  ( ( ( _val ) << GRETHGBIT_MDIO_PHYADDR_SHIFT ) & \
    GRETHGBIT_MDIO_PHYADDR_MASK )

#define GRETHGBIT_MDIO_REGADDR_SHIFT 6
#define GRETHGBIT_MDIO_REGADDR_MASK  0x7c0U
#define GRETHGBIT_MDIO_REGADDR_GET( _reg )        \
  ( ( ( _reg ) & GRETHGBIT_MDIO_REGADDR_MASK ) >> \
    GRETHGBIT_MDIO_REGADDR_SHIFT )
#define GRETHGBIT_MDIO_REGADDR_SET( _reg, _val )     \
  ( ( ( _reg ) & ~GRETHGBIT_MDIO_REGADDR_MASK ) |    \
    ( ( ( _val ) << GRETHGBIT_MDIO_REGADDR_SHIFT ) & \
      GRETHGBIT_MDIO_REGADDR_MASK ) )
#define GRETHGBIT_MDIO_REGADDR( _val )             \
  ( ( ( _val ) << GRETHGBIT_MDIO_REGADDR_SHIFT ) & \
    GRETHGBIT_MDIO_REGADDR_MASK )

#define GRETHGBIT_MDIO_BU 0x8U

#define GRETHGBIT_MDIO_LF 0x4U

#define GRETHGBIT_MDIO_RD 0x2U

#define GRETHGBIT_MDIO_WR 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITTDTBA \
 *   transmitter descriptor table base address register. (TDTBA)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRETHGBIT_TDTBA_BASEADDR_SHIFT 10
#define GRETHGBIT_TDTBA_BASEADDR_MASK  0xfffffc00U
#define GRETHGBIT_TDTBA_BASEADDR_GET( _reg )        \
  ( ( ( _reg ) & GRETHGBIT_TDTBA_BASEADDR_MASK ) >> \
    GRETHGBIT_TDTBA_BASEADDR_SHIFT )
#define GRETHGBIT_TDTBA_BASEADDR_SET( _reg, _val )     \
  ( ( ( _reg ) & ~GRETHGBIT_TDTBA_BASEADDR_MASK ) |    \
    ( ( ( _val ) << GRETHGBIT_TDTBA_BASEADDR_SHIFT ) & \
      GRETHGBIT_TDTBA_BASEADDR_MASK ) )
#define GRETHGBIT_TDTBA_BASEADDR( _val )             \
  ( ( ( _val ) << GRETHGBIT_TDTBA_BASEADDR_SHIFT ) & \
    GRETHGBIT_TDTBA_BASEADDR_MASK )

#define GRETHGBIT_TDTBA_DESCPNT_SHIFT 3
#define GRETHGBIT_TDTBA_DESCPNT_MASK  0x3f8U
#define GRETHGBIT_TDTBA_DESCPNT_GET( _reg )        \
  ( ( ( _reg ) & GRETHGBIT_TDTBA_DESCPNT_MASK ) >> \
    GRETHGBIT_TDTBA_DESCPNT_SHIFT )
#define GRETHGBIT_TDTBA_DESCPNT_SET( _reg, _val )     \
  ( ( ( _reg ) & ~GRETHGBIT_TDTBA_DESCPNT_MASK ) |    \
    ( ( ( _val ) << GRETHGBIT_TDTBA_DESCPNT_SHIFT ) & \
      GRETHGBIT_TDTBA_DESCPNT_MASK ) )
#define GRETHGBIT_TDTBA_DESCPNT( _val )             \
  ( ( ( _val ) << GRETHGBIT_TDTBA_DESCPNT_SHIFT ) & \
    GRETHGBIT_TDTBA_DESCPNT_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITRDTBA \
 *   receiver descriptor table base address register. (RDTBA)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRETHGBIT_RDTBA_BASEADDR_SHIFT 10
#define GRETHGBIT_RDTBA_BASEADDR_MASK  0xfffffc00U
#define GRETHGBIT_RDTBA_BASEADDR_GET( _reg )        \
  ( ( ( _reg ) & GRETHGBIT_RDTBA_BASEADDR_MASK ) >> \
    GRETHGBIT_RDTBA_BASEADDR_SHIFT )
#define GRETHGBIT_RDTBA_BASEADDR_SET( _reg, _val )     \
  ( ( ( _reg ) & ~GRETHGBIT_RDTBA_BASEADDR_MASK ) |    \
    ( ( ( _val ) << GRETHGBIT_RDTBA_BASEADDR_SHIFT ) & \
      GRETHGBIT_RDTBA_BASEADDR_MASK ) )
#define GRETHGBIT_RDTBA_BASEADDR( _val )             \
  ( ( ( _val ) << GRETHGBIT_RDTBA_BASEADDR_SHIFT ) & \
    GRETHGBIT_RDTBA_BASEADDR_MASK )

#define GRETHGBIT_RDTBA_DESCPNT_SHIFT 3
#define GRETHGBIT_RDTBA_DESCPNT_MASK  0x3f8U
#define GRETHGBIT_RDTBA_DESCPNT_GET( _reg )        \
  ( ( ( _reg ) & GRETHGBIT_RDTBA_DESCPNT_MASK ) >> \
    GRETHGBIT_RDTBA_DESCPNT_SHIFT )
#define GRETHGBIT_RDTBA_DESCPNT_SET( _reg, _val )     \
  ( ( ( _reg ) & ~GRETHGBIT_RDTBA_DESCPNT_MASK ) |    \
    ( ( ( _val ) << GRETHGBIT_RDTBA_DESCPNT_SHIFT ) & \
      GRETHGBIT_RDTBA_DESCPNT_MASK ) )
#define GRETHGBIT_RDTBA_DESCPNT( _val )             \
  ( ( ( _val ) << GRETHGBIT_RDTBA_DESCPNT_SHIFT ) & \
    GRETHGBIT_RDTBA_DESCPNT_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITEDCLIP GRETH_GBIT EDCL IP register (EDCLIP)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRETHGBIT_EDCLIP_EDCL_IP_ADDRESS_SHIFT 0
#define GRETHGBIT_EDCLIP_EDCL_IP_ADDRESS_MASK  0xffffffffU
#define GRETHGBIT_EDCLIP_EDCL_IP_ADDRESS_GET( _reg )        \
  ( ( ( _reg ) & GRETHGBIT_EDCLIP_EDCL_IP_ADDRESS_MASK ) >> \
    GRETHGBIT_EDCLIP_EDCL_IP_ADDRESS_SHIFT )
#define GRETHGBIT_EDCLIP_EDCL_IP_ADDRESS_SET( _reg, _val )     \
  ( ( ( _reg ) & ~GRETHGBIT_EDCLIP_EDCL_IP_ADDRESS_MASK ) |    \
    ( ( ( _val ) << GRETHGBIT_EDCLIP_EDCL_IP_ADDRESS_SHIFT ) & \
      GRETHGBIT_EDCLIP_EDCL_IP_ADDRESS_MASK ) )
#define GRETHGBIT_EDCLIP_EDCL_IP_ADDRESS( _val )             \
  ( ( ( _val ) << GRETHGBIT_EDCLIP_EDCL_IP_ADDRESS_SHIFT ) & \
    GRETHGBIT_EDCLIP_EDCL_IP_ADDRESS_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITHMSB GRETH Hash table MSB register (HMSB)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRETHGBIT_HMSB_HASH_TABLE_MSB_SHIFT 0
#define GRETHGBIT_HMSB_HASH_TABLE_MSB_MASK  0xffffffffU
#define GRETHGBIT_HMSB_HASH_TABLE_MSB_GET( _reg )        \
  ( ( ( _reg ) & GRETHGBIT_HMSB_HASH_TABLE_MSB_MASK ) >> \
    GRETHGBIT_HMSB_HASH_TABLE_MSB_SHIFT )
#define GRETHGBIT_HMSB_HASH_TABLE_MSB_SET( _reg, _val )     \
  ( ( ( _reg ) & ~GRETHGBIT_HMSB_HASH_TABLE_MSB_MASK ) |    \
    ( ( ( _val ) << GRETHGBIT_HMSB_HASH_TABLE_MSB_SHIFT ) & \
      GRETHGBIT_HMSB_HASH_TABLE_MSB_MASK ) )
#define GRETHGBIT_HMSB_HASH_TABLE_MSB( _val )             \
  ( ( ( _val ) << GRETHGBIT_HMSB_HASH_TABLE_MSB_SHIFT ) & \
    GRETHGBIT_HMSB_HASH_TABLE_MSB_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITHLSB GRETH Hash table LSB register (HLSB)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRETHGBIT_HLSB_HASH_TABLE_LSB_SHIFT 0
#define GRETHGBIT_HLSB_HASH_TABLE_LSB_MASK  0xffffffffU
#define GRETHGBIT_HLSB_HASH_TABLE_LSB_GET( _reg )        \
  ( ( ( _reg ) & GRETHGBIT_HLSB_HASH_TABLE_LSB_MASK ) >> \
    GRETHGBIT_HLSB_HASH_TABLE_LSB_SHIFT )
#define GRETHGBIT_HLSB_HASH_TABLE_LSB_SET( _reg, _val )     \
  ( ( ( _reg ) & ~GRETHGBIT_HLSB_HASH_TABLE_LSB_MASK ) |    \
    ( ( ( _val ) << GRETHGBIT_HLSB_HASH_TABLE_LSB_SHIFT ) & \
      GRETHGBIT_HLSB_HASH_TABLE_LSB_MASK ) )
#define GRETHGBIT_HLSB_HASH_TABLE_LSB( _val )             \
  ( ( ( _val ) << GRETHGBIT_HLSB_HASH_TABLE_LSB_SHIFT ) & \
    GRETHGBIT_HLSB_HASH_TABLE_LSB_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITEDCLMACMSB EDCL MAC address MSB. (EDCLMACMSB)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRETHGBIT_EDCLMACMSB_MSB_SHIFT 0
#define GRETHGBIT_EDCLMACMSB_MSB_MASK  0xffffU
#define GRETHGBIT_EDCLMACMSB_MSB_GET( _reg )        \
  ( ( ( _reg ) & GRETHGBIT_EDCLMACMSB_MSB_MASK ) >> \
    GRETHGBIT_EDCLMACMSB_MSB_SHIFT )
#define GRETHGBIT_EDCLMACMSB_MSB_SET( _reg, _val )     \
  ( ( ( _reg ) & ~GRETHGBIT_EDCLMACMSB_MSB_MASK ) |    \
    ( ( ( _val ) << GRETHGBIT_EDCLMACMSB_MSB_SHIFT ) & \
      GRETHGBIT_EDCLMACMSB_MSB_MASK ) )
#define GRETHGBIT_EDCLMACMSB_MSB( _val )             \
  ( ( ( _val ) << GRETHGBIT_EDCLMACMSB_MSB_SHIFT ) & \
    GRETHGBIT_EDCLMACMSB_MSB_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITEDCLMACLSB EDCL MAC address LSB. (EDCLMACLSB)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRETHGBIT_EDCLMACLSB_LSB_SHIFT 0
#define GRETHGBIT_EDCLMACLSB_LSB_MASK  0xffffffffU
#define GRETHGBIT_EDCLMACLSB_LSB_GET( _reg )        \
  ( ( ( _reg ) & GRETHGBIT_EDCLMACLSB_LSB_MASK ) >> \
    GRETHGBIT_EDCLMACLSB_LSB_SHIFT )
#define GRETHGBIT_EDCLMACLSB_LSB_SET( _reg, _val )     \
  ( ( ( _reg ) & ~GRETHGBIT_EDCLMACLSB_LSB_MASK ) |    \
    ( ( ( _val ) << GRETHGBIT_EDCLMACLSB_LSB_SHIFT ) & \
      GRETHGBIT_EDCLMACLSB_LSB_MASK ) )
#define GRETHGBIT_EDCLMACLSB_LSB( _val )             \
  ( ( ( _val ) << GRETHGBIT_EDCLMACLSB_LSB_SHIFT ) & \
    GRETHGBIT_EDCLMACLSB_LSB_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITTXRAM Transmitter RAM debug area (TXRAM)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRETHGBIT_TXRAM_TXRAM_SHIFT 0
#define GRETHGBIT_TXRAM_TXRAM_MASK  0xffffffffU
#define GRETHGBIT_TXRAM_TXRAM_GET( _reg ) \
  ( ( ( _reg ) & GRETHGBIT_TXRAM_TXRAM_MASK ) >> GRETHGBIT_TXRAM_TXRAM_SHIFT )
#define GRETHGBIT_TXRAM_TXRAM_SET( _reg, _val )     \
  ( ( ( _reg ) & ~GRETHGBIT_TXRAM_TXRAM_MASK ) |    \
    ( ( ( _val ) << GRETHGBIT_TXRAM_TXRAM_SHIFT ) & \
      GRETHGBIT_TXRAM_TXRAM_MASK ) )
#define GRETHGBIT_TXRAM_TXRAM( _val ) \
  ( ( ( _val ) << GRETHGBIT_TXRAM_TXRAM_SHIFT ) & GRETHGBIT_TXRAM_TXRAM_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITRXRAM Receiver RAM debug area (RXRAM)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRETHGBIT_RXRAM_RXRAM_SHIFT 0
#define GRETHGBIT_RXRAM_RXRAM_MASK  0xffffffffU
#define GRETHGBIT_RXRAM_RXRAM_GET( _reg ) \
  ( ( ( _reg ) & GRETHGBIT_RXRAM_RXRAM_MASK ) >> GRETHGBIT_RXRAM_RXRAM_SHIFT )
#define GRETHGBIT_RXRAM_RXRAM_SET( _reg, _val )     \
  ( ( ( _reg ) & ~GRETHGBIT_RXRAM_RXRAM_MASK ) |    \
    ( ( ( _val ) << GRETHGBIT_RXRAM_RXRAM_SHIFT ) & \
      GRETHGBIT_RXRAM_RXRAM_MASK ) )
#define GRETHGBIT_RXRAM_RXRAM( _val ) \
  ( ( ( _val ) << GRETHGBIT_RXRAM_RXRAM_SHIFT ) & GRETHGBIT_RXRAM_RXRAM_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITEDCLBUF EDCL buffer RAM debug area (EDCLBUF)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRETHGBIT_EDCLBUF_EDCLBUF_SHIFT 0
#define GRETHGBIT_EDCLBUF_EDCLBUF_MASK  0xffffffffU
#define GRETHGBIT_EDCLBUF_EDCLBUF_GET( _reg )        \
  ( ( ( _reg ) & GRETHGBIT_EDCLBUF_EDCLBUF_MASK ) >> \
    GRETHGBIT_EDCLBUF_EDCLBUF_SHIFT )
#define GRETHGBIT_EDCLBUF_EDCLBUF_SET( _reg, _val )     \
  ( ( ( _reg ) & ~GRETHGBIT_EDCLBUF_EDCLBUF_MASK ) |    \
    ( ( ( _val ) << GRETHGBIT_EDCLBUF_EDCLBUF_SHIFT ) & \
      GRETHGBIT_EDCLBUF_EDCLBUF_MASK ) )
#define GRETHGBIT_EDCLBUF_EDCLBUF( _val )             \
  ( ( ( _val ) << GRETHGBIT_EDCLBUF_EDCLBUF_SHIFT ) & \
    GRETHGBIT_EDCLBUF_EDCLBUF_MASK )

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

  /**
   * @brief See @ref RTEMSDeviceGRETHGBITEDCLIP.
   */
  uint32_t edclip;

  /**
   * @brief See @ref RTEMSDeviceGRETHGBITHMSB.
   */
  uint32_t hmsb;

  /**
   * @brief See @ref RTEMSDeviceGRETHGBITHLSB.
   */
  uint32_t hlsb;

  /**
   * @brief See @ref RTEMSDeviceGRETHGBITEDCLMACMSB.
   */
  uint32_t edclmacmsb;

  /**
   * @brief See @ref RTEMSDeviceGRETHGBITEDCLMACLSB.
   */
  uint32_t edclmaclsb;

  uint32_t reserved_30_10000[ 16372 ];

  /**
   * @brief See @ref RTEMSDeviceGRETHGBITTXRAM.
   */
  uint32_t txram[ 512 ];

  uint32_t reserved_10800_20000[ 15872 ];

  /**
   * @brief See @ref RTEMSDeviceGRETHGBITRXRAM.
   */
  uint32_t rxram[ 512 ];

  uint32_t reserved_20800_30000[ 15872 ];

  /**
   * @brief See @ref RTEMSDeviceGRETHGBITEDCLBUF.
   */
  uint32_t edclbuf[ 16384 ];
} grethgbit;

/** @} */

/* Find related documentation with spec:/dev/grlib/if/grethgbit-desc-alignment */

/**
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This constant defines the required alignment in bytes of the
 *   GRETH_GBIT DMA descriptor table base address.
 */
#define GRETHGBIT_DMA_DESC_ALIGNMENT 1024

/* Find related documentation with spec:/dev/grlib/if/grethgbit-desc-max-count */

/**
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This constant defines the maximum count of GRETH_GBIT DMA descriptors
 *   in a descriptor table.
 */
#define GRETHGBIT_DMA_DESC_MAX_COUNT 128

/* Find related documentation with spec:/dev/grlib/if/grethgbit-rxdesc */

/**
 * @defgroup RTEMSDeviceGRETHGBITReceiveDescriptor GRETH_GBIT RX descriptor
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the GRETH_GBIT RX descriptor interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRETHGBITReceiveDescriptorCTRL \
 *   Receive descriptor word 0 (CTRL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRETHGBIT_RX_CTRL_LENGTH_SHIFT 0
#define GRETHGBIT_RX_CTRL_LENGTH_MASK  0x7ffU
#define GRETHGBIT_RX_CTRL_LENGTH_GET( _reg )        \
  ( ( ( _reg ) & GRETHGBIT_RX_CTRL_LENGTH_MASK ) >> \
    GRETHGBIT_RX_CTRL_LENGTH_SHIFT )
#define GRETHGBIT_RX_CTRL_LENGTH_SET( _reg, _val )     \
  ( ( ( _reg ) & ~GRETHGBIT_RX_CTRL_LENGTH_MASK ) |    \
    ( ( ( _val ) << GRETHGBIT_RX_CTRL_LENGTH_SHIFT ) & \
      GRETHGBIT_RX_CTRL_LENGTH_MASK ) )
#define GRETHGBIT_RX_CTRL_LENGTH( _val )             \
  ( ( ( _val ) << GRETHGBIT_RX_CTRL_LENGTH_SHIFT ) & \
    GRETHGBIT_RX_CTRL_LENGTH_MASK )

#define GRETHGBIT_RX_CTRL_EN 0x800U

#define GRETHGBIT_RX_CTRL_WR 0x1000U

#define GRETHGBIT_RX_CTRL_IE 0x2000U

#define GRETHGBIT_RX_CTRL_AE 0x4000U

#define GRETHGBIT_RX_CTRL_FT 0x8000U

#define GRETHGBIT_RX_CTRL_CE 0x10000U

#define GRETHGBIT_RX_CTRL_OE 0x20000U

#define GRETHGBIT_RX_CTRL_LE 0x40000U

#define GRETHGBIT_RX_CTRL_ID 0x80000U

#define GRETHGBIT_RX_CTRL_IR 0x100000U

#define GRETHGBIT_RX_CTRL_UD 0x200000U

#define GRETHGBIT_RX_CTRL_UR 0x400000U

#define GRETHGBIT_RX_CTRL_TD 0x800000U

#define GRETHGBIT_RX_CTRL_TR 0x1000000U

#define GRETHGBIT_RX_CTRL_IF 0x2000000U

#define GRETHGBIT_RX_CTRL_MC 0x4000000U

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITReceiveDescriptorADDR \
 *   Receive descriptor word 1 (ADDR)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRETHGBIT_RX_ADDR_ADDRESS_SHIFT 0
#define GRETHGBIT_RX_ADDR_ADDRESS_MASK  0xffffffffU
#define GRETHGBIT_RX_ADDR_ADDRESS_GET( _reg )        \
  ( ( ( _reg ) & GRETHGBIT_RX_ADDR_ADDRESS_MASK ) >> \
    GRETHGBIT_RX_ADDR_ADDRESS_SHIFT )
#define GRETHGBIT_RX_ADDR_ADDRESS_SET( _reg, _val )     \
  ( ( ( _reg ) & ~GRETHGBIT_RX_ADDR_ADDRESS_MASK ) |    \
    ( ( ( _val ) << GRETHGBIT_RX_ADDR_ADDRESS_SHIFT ) & \
      GRETHGBIT_RX_ADDR_ADDRESS_MASK ) )
#define GRETHGBIT_RX_ADDR_ADDRESS( _val )             \
  ( ( ( _val ) << GRETHGBIT_RX_ADDR_ADDRESS_SHIFT ) & \
    GRETHGBIT_RX_ADDR_ADDRESS_MASK )

/** @} */

/**
 * @brief This structure defines the GRETH_GBIT RX descriptor memory map.
 */
typedef struct grethgbit_rx_desc {
  /**
   * @brief See @ref RTEMSDeviceGRETHGBITReceiveDescriptorCTRL.
   */
  uint32_t ctrl;

  /**
   * @brief See @ref RTEMSDeviceGRETHGBITReceiveDescriptorADDR.
   */
  uint32_t addr;
} grethgbit_rx_desc;

/** @} */

/* Find related documentation with spec:/dev/grlib/if/grethgbit-rxdesc-ts */

/**
 * @defgroup RTEMSDeviceGRETHGBITReceiveTimestampDescriptor \
 *   GRETH_GBIT RX timestamp descriptor
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the GRETH_GBIT RX timestamp descriptor
 *   interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRETHGBITReceiveTimestampDescriptorTIMESTAMPMSB \
 *   Receive descriptor word 2 (TIMESTAMP_MSB)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRETHGBIT_RX_TS_TIMESTAMP_MSB_MSB_SHIFT 0
#define GRETHGBIT_RX_TS_TIMESTAMP_MSB_MSB_MASK  0xffffffffU
#define GRETHGBIT_RX_TS_TIMESTAMP_MSB_MSB_GET( _reg )        \
  ( ( ( _reg ) & GRETHGBIT_RX_TS_TIMESTAMP_MSB_MSB_MASK ) >> \
    GRETHGBIT_RX_TS_TIMESTAMP_MSB_MSB_SHIFT )
#define GRETHGBIT_RX_TS_TIMESTAMP_MSB_MSB_SET( _reg, _val )     \
  ( ( ( _reg ) & ~GRETHGBIT_RX_TS_TIMESTAMP_MSB_MSB_MASK ) |    \
    ( ( ( _val ) << GRETHGBIT_RX_TS_TIMESTAMP_MSB_MSB_SHIFT ) & \
      GRETHGBIT_RX_TS_TIMESTAMP_MSB_MSB_MASK ) )
#define GRETHGBIT_RX_TS_TIMESTAMP_MSB_MSB( _val )             \
  ( ( ( _val ) << GRETHGBIT_RX_TS_TIMESTAMP_MSB_MSB_SHIFT ) & \
    GRETHGBIT_RX_TS_TIMESTAMP_MSB_MSB_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITReceiveTimestampDescriptorTIMESTAMPLSB \
 *   Receive descriptor word 3 (TIMESTAMP_LSB)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRETHGBIT_RX_TS_TIMESTAMP_LSB_LSB_SHIFT 0
#define GRETHGBIT_RX_TS_TIMESTAMP_LSB_LSB_MASK  0xffffffffU
#define GRETHGBIT_RX_TS_TIMESTAMP_LSB_LSB_GET( _reg )        \
  ( ( ( _reg ) & GRETHGBIT_RX_TS_TIMESTAMP_LSB_LSB_MASK ) >> \
    GRETHGBIT_RX_TS_TIMESTAMP_LSB_LSB_SHIFT )
#define GRETHGBIT_RX_TS_TIMESTAMP_LSB_LSB_SET( _reg, _val )     \
  ( ( ( _reg ) & ~GRETHGBIT_RX_TS_TIMESTAMP_LSB_LSB_MASK ) |    \
    ( ( ( _val ) << GRETHGBIT_RX_TS_TIMESTAMP_LSB_LSB_SHIFT ) & \
      GRETHGBIT_RX_TS_TIMESTAMP_LSB_LSB_MASK ) )
#define GRETHGBIT_RX_TS_TIMESTAMP_LSB_LSB( _val )             \
  ( ( ( _val ) << GRETHGBIT_RX_TS_TIMESTAMP_LSB_LSB_SHIFT ) & \
    GRETHGBIT_RX_TS_TIMESTAMP_LSB_LSB_MASK )

/** @} */

/**
 * @brief This structure defines the GRETH_GBIT RX timestamp descriptor memory
 *   map.
 *
 * The descriptor is expanded by the two timestamp words if the core was built
 * with timestamping support and the feature is enabled by the timestamp enable
 * bit of the control register. The count of descriptors in a table is halved
 * to #GRETHGBIT_DMA_TS_DESC_MAX_COUNT since the descriptor size is doubled.
 */
typedef struct grethgbit_rx_ts_desc {
  /**
   * @brief See @ref RTEMSDeviceGRETHGBITReceiveDescriptor.
   */
  grethgbit_rx_desc desc;

  /**
   * @brief See @ref RTEMSDeviceGRETHGBITReceiveTimestampDescriptorTIMESTAMPMSB.
   */
  uint32_t timestamp_msb;

  /**
   * @brief See @ref RTEMSDeviceGRETHGBITReceiveTimestampDescriptorTIMESTAMPLSB.
   */
  uint32_t timestamp_lsb;
} grethgbit_rx_ts_desc;

/** @} */

/* Find related documentation with spec:/dev/grlib/if/grethgbit-ts-desc-max-count */

/**
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This constant defines the maximum count of GRETH_GBIT DMA descriptors
 *   in a descriptor table if timestamping is enabled.
 */
#define GRETHGBIT_DMA_TS_DESC_MAX_COUNT 64

/* Find related documentation with spec:/dev/grlib/if/grethgbit-txdesc */

/**
 * @defgroup RTEMSDeviceGRETHGBITTransmitDescriptor GRETH_GBIT TX descriptor
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the GRETH_GBIT TX descriptor interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRETHGBITTransmitDescriptorCTRL \
 *   Transmit descriptor word 0 (CTRL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRETHGBIT_TX_CTRL_LENGTH_SHIFT 0
#define GRETHGBIT_TX_CTRL_LENGTH_MASK  0x7ffU
#define GRETHGBIT_TX_CTRL_LENGTH_GET( _reg )        \
  ( ( ( _reg ) & GRETHGBIT_TX_CTRL_LENGTH_MASK ) >> \
    GRETHGBIT_TX_CTRL_LENGTH_SHIFT )
#define GRETHGBIT_TX_CTRL_LENGTH_SET( _reg, _val )     \
  ( ( ( _reg ) & ~GRETHGBIT_TX_CTRL_LENGTH_MASK ) |    \
    ( ( ( _val ) << GRETHGBIT_TX_CTRL_LENGTH_SHIFT ) & \
      GRETHGBIT_TX_CTRL_LENGTH_MASK ) )
#define GRETHGBIT_TX_CTRL_LENGTH( _val )             \
  ( ( ( _val ) << GRETHGBIT_TX_CTRL_LENGTH_SHIFT ) & \
    GRETHGBIT_TX_CTRL_LENGTH_MASK )

#define GRETHGBIT_TX_CTRL_EN 0x800U

#define GRETHGBIT_TX_CTRL_WR 0x1000U

#define GRETHGBIT_TX_CTRL_IE 0x2000U

#define GRETHGBIT_TX_CTRL_UE 0x4000U

#define GRETHGBIT_TX_CTRL_AL 0x8000U

#define GRETHGBIT_TX_CTRL_LC 0x10000U

#define GRETHGBIT_TX_CTRL_MO 0x20000U

#define GRETHGBIT_TX_CTRL_IC 0x40000U

#define GRETHGBIT_TX_CTRL_TC 0x80000U

#define GRETHGBIT_TX_CTRL_UC 0x100000U

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITTransmitDescriptorADDR \
 *   Transmit descriptor word 1 (ADDR)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRETHGBIT_TX_ADDR_ADDRESS_SHIFT 0
#define GRETHGBIT_TX_ADDR_ADDRESS_MASK  0xffffffffU
#define GRETHGBIT_TX_ADDR_ADDRESS_GET( _reg )        \
  ( ( ( _reg ) & GRETHGBIT_TX_ADDR_ADDRESS_MASK ) >> \
    GRETHGBIT_TX_ADDR_ADDRESS_SHIFT )
#define GRETHGBIT_TX_ADDR_ADDRESS_SET( _reg, _val )     \
  ( ( ( _reg ) & ~GRETHGBIT_TX_ADDR_ADDRESS_MASK ) |    \
    ( ( ( _val ) << GRETHGBIT_TX_ADDR_ADDRESS_SHIFT ) & \
      GRETHGBIT_TX_ADDR_ADDRESS_MASK ) )
#define GRETHGBIT_TX_ADDR_ADDRESS( _val )             \
  ( ( ( _val ) << GRETHGBIT_TX_ADDR_ADDRESS_SHIFT ) & \
    GRETHGBIT_TX_ADDR_ADDRESS_MASK )

/** @} */

/**
 * @brief This structure defines the GRETH_GBIT TX descriptor memory map.
 */
typedef struct grethgbit_tx_desc {
  /**
   * @brief See @ref RTEMSDeviceGRETHGBITTransmitDescriptorCTRL.
   */
  uint32_t ctrl;

  /**
   * @brief See @ref RTEMSDeviceGRETHGBITTransmitDescriptorADDR.
   */
  uint32_t addr;
} grethgbit_tx_desc;

/** @} */

/* Find related documentation with spec:/dev/grlib/if/grethgbit-txdesc-ts */

/**
 * @defgroup RTEMSDeviceGRETHGBITTransmitTimestampDescriptor \
 *   GRETH_GBIT TX timestamp descriptor
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the GRETH_GBIT TX timestamp descriptor
 *   interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRETHGBITTransmitTimestampDescriptorTIMESTAMPMSB \
 *   Transmit descriptor word 2 (TIMESTAMP_MSB)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRETHGBIT_TX_TS_TIMESTAMP_MSB_MSB_SHIFT 0
#define GRETHGBIT_TX_TS_TIMESTAMP_MSB_MSB_MASK  0xffffffffU
#define GRETHGBIT_TX_TS_TIMESTAMP_MSB_MSB_GET( _reg )        \
  ( ( ( _reg ) & GRETHGBIT_TX_TS_TIMESTAMP_MSB_MSB_MASK ) >> \
    GRETHGBIT_TX_TS_TIMESTAMP_MSB_MSB_SHIFT )
#define GRETHGBIT_TX_TS_TIMESTAMP_MSB_MSB_SET( _reg, _val )     \
  ( ( ( _reg ) & ~GRETHGBIT_TX_TS_TIMESTAMP_MSB_MSB_MASK ) |    \
    ( ( ( _val ) << GRETHGBIT_TX_TS_TIMESTAMP_MSB_MSB_SHIFT ) & \
      GRETHGBIT_TX_TS_TIMESTAMP_MSB_MSB_MASK ) )
#define GRETHGBIT_TX_TS_TIMESTAMP_MSB_MSB( _val )             \
  ( ( ( _val ) << GRETHGBIT_TX_TS_TIMESTAMP_MSB_MSB_SHIFT ) & \
    GRETHGBIT_TX_TS_TIMESTAMP_MSB_MSB_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRETHGBITTransmitTimestampDescriptorTIMESTAMPLSB \
 *   Transmit descriptor word 3 (TIMESTAMP_LSB)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define GRETHGBIT_TX_TS_TIMESTAMP_LSB_LSB_SHIFT 0
#define GRETHGBIT_TX_TS_TIMESTAMP_LSB_LSB_MASK  0xffffffffU
#define GRETHGBIT_TX_TS_TIMESTAMP_LSB_LSB_GET( _reg )        \
  ( ( ( _reg ) & GRETHGBIT_TX_TS_TIMESTAMP_LSB_LSB_MASK ) >> \
    GRETHGBIT_TX_TS_TIMESTAMP_LSB_LSB_SHIFT )
#define GRETHGBIT_TX_TS_TIMESTAMP_LSB_LSB_SET( _reg, _val )     \
  ( ( ( _reg ) & ~GRETHGBIT_TX_TS_TIMESTAMP_LSB_LSB_MASK ) |    \
    ( ( ( _val ) << GRETHGBIT_TX_TS_TIMESTAMP_LSB_LSB_SHIFT ) & \
      GRETHGBIT_TX_TS_TIMESTAMP_LSB_LSB_MASK ) )
#define GRETHGBIT_TX_TS_TIMESTAMP_LSB_LSB( _val )             \
  ( ( ( _val ) << GRETHGBIT_TX_TS_TIMESTAMP_LSB_LSB_SHIFT ) & \
    GRETHGBIT_TX_TS_TIMESTAMP_LSB_LSB_MASK )

/** @} */

/**
 * @brief This structure defines the GRETH_GBIT TX timestamp descriptor memory
 *   map.
 *
 * The descriptor is expanded by the two timestamp words if the core was built
 * with timestamping support and the feature is enabled by the timestamp enable
 * bit of the control register. The count of descriptors in a table is halved
 * to #GRETHGBIT_DMA_TS_DESC_MAX_COUNT since the descriptor size is doubled.
 */
typedef struct grethgbit_tx_ts_desc {
  /**
   * @brief See @ref RTEMSDeviceGRETHGBITTransmitDescriptor.
   */
  grethgbit_tx_desc desc;

  /**
   * @brief See @ref RTEMSDeviceGRETHGBITTransmitTimestampDescriptorTIMESTAMPMSB.
   */
  uint32_t timestamp_msb;

  /**
   * @brief See @ref RTEMSDeviceGRETHGBITTransmitTimestampDescriptorTIMESTAMPLSB.
   */
  uint32_t timestamp_lsb;
} grethgbit_tx_ts_desc;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_GRETHGBIT_REGS_H */
