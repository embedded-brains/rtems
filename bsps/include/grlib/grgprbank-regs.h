/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRGPRBANK
 *
 * @brief This header file defines the GRGPRBANK register block interface.
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

/* Generated from spec:/dev/grlib/if/grgprbank-header */

#ifndef _GRLIB_GRGPRBANK_REGS_H
#define _GRLIB_GRGPRBANK_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/grgprbank */

/**
 * @defgroup RTEMSDeviceGRGPRBANK GPRBANK
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the GPRBANK interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRGPRBANKFTMFUNC FTMFUNC
 *
 * @brief FTMCTRL function enable register
 *
 * @{
 */

#define GRGPRBANK_FTMFUNC_FTMEN_SHIFT 0
#define GRGPRBANK_FTMFUNC_FTMEN_MASK 0x3fffffU
#define GRGPRBANK_FTMFUNC_FTMEN_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3fffffU )
#define GRGPRBANK_FTMFUNC_FTMEN( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRGPRBANKALTFUNC ALTFUNC
 *
 * @brief Alternative function enable register
 *
 * @{
 */

#define GRGPRBANK_ALTFUNC_ALTEN_SHIFT 0
#define GRGPRBANK_ALTFUNC_ALTEN_MASK 0x3fffffU
#define GRGPRBANK_ALTFUNC_ALTEN_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3fffffU )
#define GRGPRBANK_ALTFUNC_ALTEN( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRGPRBANKLVDSMCLK LVDSMCLK
 *
 * @brief LVDS and memory clock pad enable register
 *
 * @{
 */

#define GRGPRBANK_LVDSMCLK_SMEM 0x20000U

#define GRGPRBANK_LVDSMCLK_DMEM 0x10000U

#define GRGPRBANK_LVDSMCLK_SPWOE_SHIFT 0
#define GRGPRBANK_LVDSMCLK_SPWOE_MASK 0xffU
#define GRGPRBANK_LVDSMCLK_SPWOE_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )
#define GRGPRBANK_LVDSMCLK_SPWOE( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRGPRBANKPLLNEWCFG PLLNEWCFG
 *
 * @brief PLL new configuration register
 *
 * @{
 */

#define GRGPRBANK_PLLNEWCFG_SWTAG_SHIFT 27
#define GRGPRBANK_PLLNEWCFG_SWTAG_MASK 0x18000000U
#define GRGPRBANK_PLLNEWCFG_SWTAG_GET( _reg ) \
  ( ( ( _reg ) >> 27 ) & 0x3U )
#define GRGPRBANK_PLLNEWCFG_SWTAG( _val ) ( ( _val ) << 27 )

#define GRGPRBANK_PLLNEWCFG_SPWPLLCFG_SHIFT 18
#define GRGPRBANK_PLLNEWCFG_SPWPLLCFG_MASK 0x7fc0000U
#define GRGPRBANK_PLLNEWCFG_SPWPLLCFG_GET( _reg ) \
  ( ( ( _reg ) >> 18 ) & 0x1ffU )
#define GRGPRBANK_PLLNEWCFG_SPWPLLCFG( _val ) ( ( _val ) << 18 )

#define GRGPRBANK_PLLNEWCFG_MEMPLLCFG_SHIFT 9
#define GRGPRBANK_PLLNEWCFG_MEMPLLCFG_MASK 0x3fe00U
#define GRGPRBANK_PLLNEWCFG_MEMPLLCFG_GET( _reg ) \
  ( ( ( _reg ) >> 9 ) & 0x1ffU )
#define GRGPRBANK_PLLNEWCFG_MEMPLLCFG( _val ) ( ( _val ) << 9 )

#define GRGPRBANK_PLLNEWCFG_SYSPLLCFG_SHIFT 0
#define GRGPRBANK_PLLNEWCFG_SYSPLLCFG_MASK 0x1ffU
#define GRGPRBANK_PLLNEWCFG_SYSPLLCFG_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x1ffU )
#define GRGPRBANK_PLLNEWCFG_SYSPLLCFG( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRGPRBANKPLLRECFG PLLRECFG
 *
 * @brief PLL reconfigure command register
 *
 * @{
 */

#define GRGPRBANK_PLLRECFG_RECONF_SHIFT 0
#define GRGPRBANK_PLLRECFG_RECONF_MASK 0x7U
#define GRGPRBANK_PLLRECFG_RECONF_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x7U )
#define GRGPRBANK_PLLRECFG_RECONF( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRGPRBANKPLLCURCFG PLLCURCFG
 *
 * @brief PLL current configuration register
 *
 * @{
 */

#define GRGPRBANK_PLLCURCFG_SWTAG_SHIFT 27
#define GRGPRBANK_PLLCURCFG_SWTAG_MASK 0x18000000U
#define GRGPRBANK_PLLCURCFG_SWTAG_GET( _reg ) \
  ( ( ( _reg ) >> 27 ) & 0x3U )
#define GRGPRBANK_PLLCURCFG_SWTAG( _val ) ( ( _val ) << 27 )

#define GRGPRBANK_PLLCURCFG_SPWPLLCFG_SHIFT 18
#define GRGPRBANK_PLLCURCFG_SPWPLLCFG_MASK 0x7fc0000U
#define GRGPRBANK_PLLCURCFG_SPWPLLCFG_GET( _reg ) \
  ( ( ( _reg ) >> 18 ) & 0x1ffU )
#define GRGPRBANK_PLLCURCFG_SPWPLLCFG( _val ) ( ( _val ) << 18 )

#define GRGPRBANK_PLLCURCFG_MEMPLLCFG_SHIFT 9
#define GRGPRBANK_PLLCURCFG_MEMPLLCFG_MASK 0x3fe00U
#define GRGPRBANK_PLLCURCFG_MEMPLLCFG_GET( _reg ) \
  ( ( ( _reg ) >> 9 ) & 0x1ffU )
#define GRGPRBANK_PLLCURCFG_MEMPLLCFG( _val ) ( ( _val ) << 9 )

#define GRGPRBANK_PLLCURCFG_SYSPLLCFG_SHIFT 0
#define GRGPRBANK_PLLCURCFG_SYSPLLCFG_MASK 0x1ffU
#define GRGPRBANK_PLLCURCFG_SYSPLLCFG_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x1ffU )
#define GRGPRBANK_PLLCURCFG_SYSPLLCFG( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRGPRBANKDRVSTR1 DRVSTR1
 *
 * @brief Drive strength configuration register 1
 *
 * @{
 */

#define GRGPRBANK_DRVSTR1_S9_SHIFT 18
#define GRGPRBANK_DRVSTR1_S9_MASK 0xc0000U
#define GRGPRBANK_DRVSTR1_S9_GET( _reg ) \
  ( ( ( _reg ) >> 18 ) & 0x3U )
#define GRGPRBANK_DRVSTR1_S9( _val ) ( ( _val ) << 18 )

#define GRGPRBANK_DRVSTR1_S8_SHIFT 16
#define GRGPRBANK_DRVSTR1_S8_MASK 0x30000U
#define GRGPRBANK_DRVSTR1_S8_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0x3U )
#define GRGPRBANK_DRVSTR1_S8( _val ) ( ( _val ) << 16 )

#define GRGPRBANK_DRVSTR1_S7_SHIFT 14
#define GRGPRBANK_DRVSTR1_S7_MASK 0xc000U
#define GRGPRBANK_DRVSTR1_S7_GET( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0x3U )
#define GRGPRBANK_DRVSTR1_S7( _val ) ( ( _val ) << 14 )

#define GRGPRBANK_DRVSTR1_S6_SHIFT 12
#define GRGPRBANK_DRVSTR1_S6_MASK 0x3000U
#define GRGPRBANK_DRVSTR1_S6_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0x3U )
#define GRGPRBANK_DRVSTR1_S6( _val ) ( ( _val ) << 12 )

#define GRGPRBANK_DRVSTR1_S5_SHIFT 10
#define GRGPRBANK_DRVSTR1_S5_MASK 0xc00U
#define GRGPRBANK_DRVSTR1_S5_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3U )
#define GRGPRBANK_DRVSTR1_S5( _val ) ( ( _val ) << 10 )

#define GRGPRBANK_DRVSTR1_S4_SHIFT 8
#define GRGPRBANK_DRVSTR1_S4_MASK 0x300U
#define GRGPRBANK_DRVSTR1_S4_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0x3U )
#define GRGPRBANK_DRVSTR1_S4( _val ) ( ( _val ) << 8 )

#define GRGPRBANK_DRVSTR1_S3_SHIFT 6
#define GRGPRBANK_DRVSTR1_S3_MASK 0xc0U
#define GRGPRBANK_DRVSTR1_S3_GET( _reg ) \
  ( ( ( _reg ) >> 6 ) & 0x3U )
#define GRGPRBANK_DRVSTR1_S3( _val ) ( ( _val ) << 6 )

#define GRGPRBANK_DRVSTR1_S2_SHIFT 4
#define GRGPRBANK_DRVSTR1_S2_MASK 0x30U
#define GRGPRBANK_DRVSTR1_S2_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0x3U )
#define GRGPRBANK_DRVSTR1_S2( _val ) ( ( _val ) << 4 )

#define GRGPRBANK_DRVSTR1_S1_SHIFT 2
#define GRGPRBANK_DRVSTR1_S1_MASK 0xcU
#define GRGPRBANK_DRVSTR1_S1_GET( _reg ) \
  ( ( ( _reg ) >> 2 ) & 0x3U )
#define GRGPRBANK_DRVSTR1_S1( _val ) ( ( _val ) << 2 )

#define GRGPRBANK_DRVSTR1_S0_SHIFT 0
#define GRGPRBANK_DRVSTR1_S0_MASK 0x3U
#define GRGPRBANK_DRVSTR1_S0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3U )
#define GRGPRBANK_DRVSTR1_S0( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRGPRBANKDRVSTR2 DRVSTR2
 *
 * @brief Drive strength configuration register 2
 *
 * @{
 */

#define GRGPRBANK_DRVSTR2_S19_SHIFT 18
#define GRGPRBANK_DRVSTR2_S19_MASK 0xc0000U
#define GRGPRBANK_DRVSTR2_S19_GET( _reg ) \
  ( ( ( _reg ) >> 18 ) & 0x3U )
#define GRGPRBANK_DRVSTR2_S19( _val ) ( ( _val ) << 18 )

#define GRGPRBANK_DRVSTR2_S18_SHIFT 16
#define GRGPRBANK_DRVSTR2_S18_MASK 0x30000U
#define GRGPRBANK_DRVSTR2_S18_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0x3U )
#define GRGPRBANK_DRVSTR2_S18( _val ) ( ( _val ) << 16 )

#define GRGPRBANK_DRVSTR2_S17_SHIFT 14
#define GRGPRBANK_DRVSTR2_S17_MASK 0xc000U
#define GRGPRBANK_DRVSTR2_S17_GET( _reg ) \
  ( ( ( _reg ) >> 14 ) & 0x3U )
#define GRGPRBANK_DRVSTR2_S17( _val ) ( ( _val ) << 14 )

#define GRGPRBANK_DRVSTR2_S16_SHIFT 12
#define GRGPRBANK_DRVSTR2_S16_MASK 0x3000U
#define GRGPRBANK_DRVSTR2_S16_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0x3U )
#define GRGPRBANK_DRVSTR2_S16( _val ) ( ( _val ) << 12 )

#define GRGPRBANK_DRVSTR2_S15_SHIFT 10
#define GRGPRBANK_DRVSTR2_S15_MASK 0xc00U
#define GRGPRBANK_DRVSTR2_S15_GET( _reg ) \
  ( ( ( _reg ) >> 10 ) & 0x3U )
#define GRGPRBANK_DRVSTR2_S15( _val ) ( ( _val ) << 10 )

#define GRGPRBANK_DRVSTR2_S14_SHIFT 8
#define GRGPRBANK_DRVSTR2_S14_MASK 0x300U
#define GRGPRBANK_DRVSTR2_S14_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0x3U )
#define GRGPRBANK_DRVSTR2_S14( _val ) ( ( _val ) << 8 )

#define GRGPRBANK_DRVSTR2_S13_SHIFT 6
#define GRGPRBANK_DRVSTR2_S13_MASK 0xc0U
#define GRGPRBANK_DRVSTR2_S13_GET( _reg ) \
  ( ( ( _reg ) >> 6 ) & 0x3U )
#define GRGPRBANK_DRVSTR2_S13( _val ) ( ( _val ) << 6 )

#define GRGPRBANK_DRVSTR2_S12_SHIFT 4
#define GRGPRBANK_DRVSTR2_S12_MASK 0x30U
#define GRGPRBANK_DRVSTR2_S12_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0x3U )
#define GRGPRBANK_DRVSTR2_S12( _val ) ( ( _val ) << 4 )

#define GRGPRBANK_DRVSTR2_S11_SHIFT 2
#define GRGPRBANK_DRVSTR2_S11_MASK 0xcU
#define GRGPRBANK_DRVSTR2_S11_GET( _reg ) \
  ( ( ( _reg ) >> 2 ) & 0x3U )
#define GRGPRBANK_DRVSTR2_S11( _val ) ( ( _val ) << 2 )

#define GRGPRBANK_DRVSTR2_S10_SHIFT 0
#define GRGPRBANK_DRVSTR2_S10_MASK 0x3U
#define GRGPRBANK_DRVSTR2_S10_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x3U )
#define GRGPRBANK_DRVSTR2_S10( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRGPRBANKLOCKDOWN LOCKDOWN
 *
 * @brief Configuration lockdown register
 *
 * @{
 */

#define GRGPRBANK_LOCKDOWN_PERMANENT_SHIFT 16
#define GRGPRBANK_LOCKDOWN_PERMANENT_MASK 0xff0000U
#define GRGPRBANK_LOCKDOWN_PERMANENT_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xffU )
#define GRGPRBANK_LOCKDOWN_PERMANENT( _val ) ( ( _val ) << 16 )

#define GRGPRBANK_LOCKDOWN_REVOCABLE_SHIFT 0
#define GRGPRBANK_LOCKDOWN_REVOCABLE_MASK 0xffU
#define GRGPRBANK_LOCKDOWN_REVOCABLE_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )
#define GRGPRBANK_LOCKDOWN_REVOCABLE( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @brief This structure defines the GPRBANK register block memory map.
 */
typedef struct grgprbank {
  /**
   * @brief See @ref RTEMSDeviceGRGPRBANKFTMFUNC.
   */
  uint32_t ftmfunc;

  /**
   * @brief See @ref RTEMSDeviceGRGPRBANKALTFUNC.
   */
  uint32_t altfunc;

  /**
   * @brief See @ref RTEMSDeviceGRGPRBANKLVDSMCLK.
   */
  uint32_t lvdsmclk;

  /**
   * @brief See @ref RTEMSDeviceGRGPRBANKPLLNEWCFG.
   */
  uint32_t pllnewcfg;

  /**
   * @brief See @ref RTEMSDeviceGRGPRBANKPLLRECFG.
   */
  uint32_t pllrecfg;

  /**
   * @brief See @ref RTEMSDeviceGRGPRBANKPLLCURCFG.
   */
  uint32_t pllcurcfg;

  /**
   * @brief See @ref RTEMSDeviceGRGPRBANKDRVSTR1.
   */
  uint32_t drvstr1;

  /**
   * @brief See @ref RTEMSDeviceGRGPRBANKDRVSTR2.
   */
  uint32_t drvstr2;

  /**
   * @brief See @ref RTEMSDeviceGRGPRBANKLOCKDOWN.
   */
  uint32_t lockdown;
} grgprbank;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_GRGPRBANK_REGS_H */
