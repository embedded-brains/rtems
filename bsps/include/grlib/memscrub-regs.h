/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRLIBMEMSCRUB
 *
 * @brief This header file defines the MEMSCRUB register block interface.
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

/* Generated from spec:/dev/grlib/if/memscrub-header */

#ifndef _GRLIB_MEMSCRUB_REGS_H
#define _GRLIB_MEMSCRUB_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/memscrub */

/**
 * @defgroup RTEMSDeviceGRLIBMEMSCRUB MEMSCRUB
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the MEMSCRUB interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRLIBMEMSCRUBAHBS AHB Status register (AHBS)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MEMSCRUB_AHBS_CECNT_SHIFT 22
#define MEMSCRUB_AHBS_CECNT_MASK 0xffc00000U
#define MEMSCRUB_AHBS_CECNT_GET( _reg ) \
  ( ( ( _reg ) >> 22 ) & 0x3ffU )
#define MEMSCRUB_AHBS_CECNT( _val ) ( ( _val ) << 22 )

#define MEMSCRUB_AHBS_UECNT_SHIFT 14
#define MEMSCRUB_AHBS_UECNT_MASK 0x3fc000U
#define MEMSCRUB_AHBS_UECNT_GET( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0xffU )
#define MEMSCRUB_AHBS_UECNT( _val ) ( ( _val ) << 14 )

#define MEMSCRUB_AHBS_DONE 0x2000U

#define MEMSCRUB_AHBS_SEC 0x800U

#define MEMSCRUB_AHBS_SBC 0x400U

#define MEMSCRUB_AHBS_CE 0x200U

#define MEMSCRUB_AHBS_NE 0x100U

#define MEMSCRUB_AHBS_HWRITE 0x80U

#define MEMSCRUB_AHBS_HMASTER_SHIFT 3
#define MEMSCRUB_AHBS_HMASTER_MASK 0x78U
#define MEMSCRUB_AHBS_HMASTER_GET( _reg ) \
  ( ( ( _reg ) >> 3 ) & 0xfU )
#define MEMSCRUB_AHBS_HMASTER( _val ) ( ( _val ) << 3 )

#define MEMSCRUB_AHBS_HSIZE_SHIFT 0
#define MEMSCRUB_AHBS_HSIZE_MASK 0x7U
#define MEMSCRUB_AHBS_HSIZE_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x7U )
#define MEMSCRUB_AHBS_HSIZE( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMEMSCRUBAHBFAR \
 *   AHB Failing Address Register (AHBFAR)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MEMSCRUB_AHBFAR_AHB_FAILING_ADDRESS_SHIFT 0
#define MEMSCRUB_AHBFAR_AHB_FAILING_ADDRESS_MASK 0xffffffffU
#define MEMSCRUB_AHBFAR_AHB_FAILING_ADDRESS_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define MEMSCRUB_AHBFAR_AHB_FAILING_ADDRESS( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMEMSCRUBAHBERC \
 *   AHB Error configuration register (AHBERC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MEMSCRUB_AHBERC_CECNTT_SHIFT 22
#define MEMSCRUB_AHBERC_CECNTT_MASK 0xffc00000U
#define MEMSCRUB_AHBERC_CECNTT_GET( _reg ) \
  ( ( ( _reg ) >> 22 ) & 0x3ffU )
#define MEMSCRUB_AHBERC_CECNTT( _val ) ( ( _val ) << 22 )

#define MEMSCRUB_AHBERC_UECNTT_SHIFT 14
#define MEMSCRUB_AHBERC_UECNTT_MASK 0x3fc000U
#define MEMSCRUB_AHBERC_UECNTT_GET( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0xffU )
#define MEMSCRUB_AHBERC_UECNTT( _val ) ( ( _val ) << 14 )

#define MEMSCRUB_AHBERC_CECTE 0x2U

#define MEMSCRUB_AHBERC_UECTE 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMEMSCRUBSTAT Status register (STAT)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MEMSCRUB_STAT_RUNCOUNT_SHIFT 22
#define MEMSCRUB_STAT_RUNCOUNT_MASK 0xffc00000U
#define MEMSCRUB_STAT_RUNCOUNT_GET( _reg ) \
  ( ( ( _reg ) >> 22 ) & 0x3ffU )
#define MEMSCRUB_STAT_RUNCOUNT( _val ) ( ( _val ) << 22 )

#define MEMSCRUB_STAT_BLKCOUNT_SHIFT 14
#define MEMSCRUB_STAT_BLKCOUNT_MASK 0x3fc000U
#define MEMSCRUB_STAT_BLKCOUNT_GET( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0xffU )
#define MEMSCRUB_STAT_BLKCOUNT( _val ) ( ( _val ) << 14 )

#define MEMSCRUB_STAT_DONE 0x2000U

#define MEMSCRUB_STAT_BURSTLEN_SHIFT 1
#define MEMSCRUB_STAT_BURSTLEN_MASK 0x1eU
#define MEMSCRUB_STAT_BURSTLEN_GET( _reg ) \
  ( ( ( _reg ) >> 1 ) & 0xfU )
#define MEMSCRUB_STAT_BURSTLEN( _val ) ( ( _val ) << 1 )

#define MEMSCRUB_STAT_ACTIVE 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMEMSCRUBCONFIG Configuration register (CONFIG)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MEMSCRUB_CONFIG_DELAY_SHIFT 8
#define MEMSCRUB_CONFIG_DELAY_MASK 0xff00U
#define MEMSCRUB_CONFIG_DELAY_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xffU )
#define MEMSCRUB_CONFIG_DELAY( _val ) ( ( _val ) << 8 )

#define MEMSCRUB_CONFIG_IRQD 0x80U

#define MEMSCRUB_CONFIG_SERA 0x20U

#define MEMSCRUB_CONFIG_LOOP 0x10U

#define MEMSCRUB_CONFIG_MODE_SHIFT 2
#define MEMSCRUB_CONFIG_MODE_MASK 0xcU
#define MEMSCRUB_CONFIG_MODE_GET( _reg ) \
  ( ( ( _reg ) >> 2 ) & 0x3U )
#define MEMSCRUB_CONFIG_MODE( _val ) ( ( _val ) << 2 )

#define MEMSCRUB_CONFIG_ES 0x2U

#define MEMSCRUB_CONFIG_SCEN 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMEMSCRUBRANGEL Range low address register (RANGEL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MEMSCRUB_RANGEL_RLADDR_SHIFT 0
#define MEMSCRUB_RANGEL_RLADDR_MASK 0xffffffffU
#define MEMSCRUB_RANGEL_RLADDR_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define MEMSCRUB_RANGEL_RLADDR( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMEMSCRUBRANGEH \
 *   Range high address register (RANGEH)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MEMSCRUB_RANGEH_RHADDR_SHIFT 0
#define MEMSCRUB_RANGEH_RHADDR_MASK 0xffffffffU
#define MEMSCRUB_RANGEH_RHADDR_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define MEMSCRUB_RANGEH_RHADDR( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMEMSCRUBPOS Position register (POS)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MEMSCRUB_POS_POSITION_SHIFT 0
#define MEMSCRUB_POS_POSITION_MASK 0xffffffffU
#define MEMSCRUB_POS_POSITION_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define MEMSCRUB_POS_POSITION( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMEMSCRUBETHRES Error threshold register (ETHRES)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MEMSCRUB_ETHRES_RECT_SHIFT 22
#define MEMSCRUB_ETHRES_RECT_MASK 0xffc00000U
#define MEMSCRUB_ETHRES_RECT_GET( _reg ) \
  ( ( ( _reg ) >> 22 ) & 0x3ffU )
#define MEMSCRUB_ETHRES_RECT( _val ) ( ( _val ) << 22 )

#define MEMSCRUB_ETHRES_BECT_SHIFT 14
#define MEMSCRUB_ETHRES_BECT_MASK 0x3fc000U
#define MEMSCRUB_ETHRES_BECT_GET( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0xffU )
#define MEMSCRUB_ETHRES_BECT( _val ) ( ( _val ) << 14 )

#define MEMSCRUB_ETHRES_RECTE 0x2U

#define MEMSCRUB_ETHRES_BECTE 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMEMSCRUBINIT Initialisation data register (INIT)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MEMSCRUB_INIT_DATA_SHIFT 0
#define MEMSCRUB_INIT_DATA_MASK 0xffffffffU
#define MEMSCRUB_INIT_DATA_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define MEMSCRUB_INIT_DATA( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMEMSCRUBRANGEL2 \
 *   Second range low address register (RANGEL2)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MEMSCRUB_RANGEL2_RLADDR_SHIFT 0
#define MEMSCRUB_RANGEL2_RLADDR_MASK 0xffffffffU
#define MEMSCRUB_RANGEL2_RLADDR_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define MEMSCRUB_RANGEL2_RLADDR( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMEMSCRUBRANGEH2 \
 *   Second range high address register (RANGEH2)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MEMSCRUB_RANGEH2_RHADDR_SHIFT 0
#define MEMSCRUB_RANGEH2_RHADDR_MASK 0xffffffffU
#define MEMSCRUB_RANGEH2_RHADDR_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define MEMSCRUB_RANGEH2_RHADDR( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @brief This structure defines the MEMSCRUB register block memory map.
 */
typedef struct memscrub {
  /**
   * @brief See @ref RTEMSDeviceGRLIBMEMSCRUBAHBS.
   */
  uint32_t ahbs;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMEMSCRUBAHBFAR.
   */
  uint32_t ahbfar;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMEMSCRUBAHBERC.
   */
  uint32_t ahberc;

  uint32_t reserved_c_10;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMEMSCRUBSTAT.
   */
  uint32_t stat;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMEMSCRUBCONFIG.
   */
  uint32_t config;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMEMSCRUBRANGEL.
   */
  uint32_t rangel;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMEMSCRUBRANGEH.
   */
  uint32_t rangeh;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMEMSCRUBPOS.
   */
  uint32_t pos;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMEMSCRUBETHRES.
   */
  uint32_t ethres;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMEMSCRUBINIT.
   */
  uint32_t init;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMEMSCRUBRANGEL2.
   */
  uint32_t rangel2;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMEMSCRUBRANGEH2.
   */
  uint32_t rangeh2;
} memscrub;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_MEMSCRUB_REGS_H */
