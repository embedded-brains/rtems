/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRLIBMMCTRL
 *
 * @brief This header file defines the MMCTRL register block interface.
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

/* Generated from spec:/dev/grlib/if/mmctrl-header */

#ifndef _GRLIB_MMCTRL_REGS_H
#define _GRLIB_MMCTRL_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/mmctrl */

/**
 * @defgroup RTEMSDeviceGRLIBMMCTRL MMCTRL
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the MMCTRL interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRLIBMMCTRLSDCFG1 \
 *   SDRAM configuration register 1 (SDCFG1)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MMCTRL_SDCFG1_RF 0x80000000U

#define MMCTRL_SDCFG1_TRP 0x40000000U

#define MMCTRL_SDCFG1_TRFC_SHIFT 27
#define MMCTRL_SDCFG1_TRFC_MASK 0x38000000U
#define MMCTRL_SDCFG1_TRFC_GET( _reg ) \
  ( ( ( _reg ) >> 27 ) & 0x7U )
#define MMCTRL_SDCFG1_TRFC( _val ) ( ( _val ) << 27 )

#define MMCTRL_SDCFG1_TC 0x4000000U

#define MMCTRL_SDCFG1_BANKSZ_SHIFT 23
#define MMCTRL_SDCFG1_BANKSZ_MASK 0x3800000U
#define MMCTRL_SDCFG1_BANKSZ_GET( _reg ) \
  ( ( ( _reg ) >> 23 ) & 0x7U )
#define MMCTRL_SDCFG1_BANKSZ( _val ) ( ( _val ) << 23 )

#define MMCTRL_SDCFG1_COLSZ_SHIFT 21
#define MMCTRL_SDCFG1_COLSZ_MASK 0x600000U
#define MMCTRL_SDCFG1_COLSZ_GET( _reg ) \
  ( ( ( _reg ) >> 21 ) & 0x3U )
#define MMCTRL_SDCFG1_COLSZ( _val ) ( ( _val ) << 21 )

#define MMCTRL_SDCFG1_COMMAND_SHIFT 18
#define MMCTRL_SDCFG1_COMMAND_MASK 0x1c0000U
#define MMCTRL_SDCFG1_COMMAND_GET( _reg ) \
  ( ( ( _reg ) >> 18 ) & 0x7U )
#define MMCTRL_SDCFG1_COMMAND( _val ) ( ( _val ) << 18 )

#define MMCTRL_SDCFG1_MS 0x10000U

#define MMCTRL_SDCFG1_64 0x8000U

#define MMCTRL_SDCFG1_RFLOAD_SHIFT 0
#define MMCTRL_SDCFG1_RFLOAD_MASK 0x7fffU
#define MMCTRL_SDCFG1_RFLOAD_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x7fffU )
#define MMCTRL_SDCFG1_RFLOAD( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMMCTRLSDCFG2 \
 *   SDRAM configuration register 2 (SDCFG2)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MMCTRL_SDCFG2_CE 0x40000000U

#define MMCTRL_SDCFG2_EN2T 0x8000U

#define MMCTRL_SDCFG2_DCS 0x4000U

#define MMCTRL_SDCFG2_BPARK 0x2000U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMMCTRLMUXCFG Mux configuration register (MUXCFG)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MMCTRL_MUXCFG_ERRLOC_SHIFT 20
#define MMCTRL_MUXCFG_ERRLOC_MASK 0xfff00000U
#define MMCTRL_MUXCFG_ERRLOC_GET( _reg ) \
  ( ( ( _reg ) >> 20 ) & 0xfffU )
#define MMCTRL_MUXCFG_ERRLOC( _val ) ( ( _val ) << 20 )

#define MMCTRL_MUXCFG_DDERR 0x80000U

#define MMCTRL_MUXCFG_DWIDTH_SHIFT 16
#define MMCTRL_MUXCFG_DWIDTH_MASK 0x70000U
#define MMCTRL_MUXCFG_DWIDTH_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0x7U )
#define MMCTRL_MUXCFG_DWIDTH( _val ) ( ( _val ) << 16 )

#define MMCTRL_MUXCFG_BEID_SHIFT 12
#define MMCTRL_MUXCFG_BEID_MASK 0xf000U
#define MMCTRL_MUXCFG_BEID_GET( _reg ) \
  ( ( ( _reg ) >> 12 ) & 0xfU )
#define MMCTRL_MUXCFG_BEID( _val ) ( ( _val ) << 12 )

#define MMCTRL_MUXCFG_DATAMUX_SHIFT 5
#define MMCTRL_MUXCFG_DATAMUX_MASK 0xe0U
#define MMCTRL_MUXCFG_DATAMUX_GET( _reg ) \
  ( ( ( _reg ) >> 5 ) & 0x7U )
#define MMCTRL_MUXCFG_DATAMUX( _val ) ( ( _val ) << 5 )

#define MMCTRL_MUXCFG_CEN 0x10U

#define MMCTRL_MUXCFG_BAUPD 0x8U

#define MMCTRL_MUXCFG_BAEN 0x4U

#define MMCTRL_MUXCFG_CODE 0x2U

#define MMCTRL_MUXCFG_EDEN 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMMCTRLFTDA FT diagnostic address register (FTDA)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MMCTRL_FTDA_FTDA_SHIFT 2
#define MMCTRL_FTDA_FTDA_MASK 0xfffffffcU
#define MMCTRL_FTDA_FTDA_GET( _reg ) \
  ( ( ( _reg ) >> 2 ) & 0x3fffffffU )
#define MMCTRL_FTDA_FTDA( _val ) ( ( _val ) << 2 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMMCTRLFTDC FT diagnostic checkbits register (FTDC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MMCTRL_FTDC_CBD_SHIFT 24
#define MMCTRL_FTDC_CBD_MASK 0xff000000U
#define MMCTRL_FTDC_CBD_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xffU )
#define MMCTRL_FTDC_CBD( _val ) ( ( _val ) << 24 )

#define MMCTRL_FTDC_CBC_SHIFT 16
#define MMCTRL_FTDC_CBC_MASK 0xff0000U
#define MMCTRL_FTDC_CBC_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xffU )
#define MMCTRL_FTDC_CBC( _val ) ( ( _val ) << 16 )

#define MMCTRL_FTDC_CBB_SHIFT 8
#define MMCTRL_FTDC_CBB_MASK 0xff00U
#define MMCTRL_FTDC_CBB_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0xffU )
#define MMCTRL_FTDC_CBB( _val ) ( ( _val ) << 8 )

#define MMCTRL_FTDC_CBA_SHIFT 0
#define MMCTRL_FTDC_CBA_MASK 0xffU
#define MMCTRL_FTDC_CBA_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffU )
#define MMCTRL_FTDC_CBA( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMMCTRLFTDD FT diagnostic data register (FTDD)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MMCTRL_FTDD_DATA_SHIFT 0
#define MMCTRL_FTDD_DATA_MASK 0xffffffffU
#define MMCTRL_FTDD_DATA_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define MMCTRL_FTDD_DATA( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBMMCTRLFTBND FT boundary address register (FTBND)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define MMCTRL_FTBND_FTBND_31_3_SHIFT 3
#define MMCTRL_FTBND_FTBND_31_3_MASK 0xfffffff8U
#define MMCTRL_FTBND_FTBND_31_3_GET( _reg ) \
  ( ( ( _reg ) >> 3 ) & 0x1fffffffU )
#define MMCTRL_FTBND_FTBND_31_3( _val ) ( ( _val ) << 3 )

/** @} */

/**
 * @brief This structure defines the MMCTRL register block memory map.
 */
typedef struct mmctrl {
  /**
   * @brief See @ref RTEMSDeviceGRLIBMMCTRLSDCFG1.
   */
  uint32_t sdcfg1;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMMCTRLSDCFG2.
   */
  uint32_t sdcfg2;

  uint32_t reserved_8_20[ 6 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBMMCTRLMUXCFG.
   */
  uint32_t muxcfg;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMMCTRLFTDA.
   */
  uint32_t ftda;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMMCTRLFTDC.
   */
  uint32_t ftdc;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMMCTRLFTDD.
   */
  uint32_t ftdd;

  /**
   * @brief See @ref RTEMSDeviceGRLIBMMCTRLFTBND.
   */
  uint32_t ftbnd;
} mmctrl;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_MMCTRL_REGS_H */
