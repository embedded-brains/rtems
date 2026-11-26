/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRLIBSPWTDP
 *
 * @brief This header file defines the SPWTDP register block interface.
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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

/* Find related documentation with spec:/dev/grlib/if/spwtdp-header */

#ifndef _GRLIB_SPWTDP_REGS_H
#define _GRLIB_SPWTDP_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Find related documentation with spec:/dev/grlib/if/spwtdp */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDP SPWTDP
 *
 * @ingroup RTEMSDeviceGRLIB
 *
 * @brief This group contains the SPWTDP interfaces.
 *
 * @{
 */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPCONF0 Configuration 0 (CONF0)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_CONF0_JE 0x1000000U

#define SPWTDP_CONF0_ST 0x200000U

#define SPWTDP_CONF0_EP 0x100000U

#define SPWTDP_CONF0_ET 0x80000U

#define SPWTDP_CONF0_SP 0x40000U

#define SPWTDP_CONF0_SE 0x20000U

#define SPWTDP_CONF0_LE 0x10000U

#define SPWTDP_CONF0_AE 0x8000U

#define SPWTDP_CONF0_MAPPING_SHIFT 8
#define SPWTDP_CONF0_MAPPING_MASK  0x1f00U
#define SPWTDP_CONF0_MAPPING_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_CONF0_MAPPING_MASK ) >> SPWTDP_CONF0_MAPPING_SHIFT )
#define SPWTDP_CONF0_MAPPING_SET( _reg, _val )     \
  ( ( ( _reg ) & ~SPWTDP_CONF0_MAPPING_MASK ) |    \
    ( ( ( _val ) << SPWTDP_CONF0_MAPPING_SHIFT ) & \
      SPWTDP_CONF0_MAPPING_MASK ) )
#define SPWTDP_CONF0_MAPPING( _val ) \
  ( ( ( _val ) << SPWTDP_CONF0_MAPPING_SHIFT ) & SPWTDP_CONF0_MAPPING_MASK )

#define SPWTDP_CONF0_TD 0x80U

#define SPWTDP_CONF0_MU 0x40U

#define SPWTDP_CONF0_SEL_SHIFT 4
#define SPWTDP_CONF0_SEL_MASK  0x30U
#define SPWTDP_CONF0_SEL_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_CONF0_SEL_MASK ) >> SPWTDP_CONF0_SEL_SHIFT )
#define SPWTDP_CONF0_SEL_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_CONF0_SEL_MASK ) | \
    ( ( ( _val ) << SPWTDP_CONF0_SEL_SHIFT ) & SPWTDP_CONF0_SEL_MASK ) )
#define SPWTDP_CONF0_SEL( _val ) \
  ( ( ( _val ) << SPWTDP_CONF0_SEL_SHIFT ) & SPWTDP_CONF0_SEL_MASK )

#define SPWTDP_CONF0_ME 0x8U

#define SPWTDP_CONF0_RE 0x4U

#define SPWTDP_CONF0_TE 0x2U

#define SPWTDP_CONF0_RS 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPCONF1 Configuration 1 (CONF1)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_CONF1_FSINC_SHIFT 0
#define SPWTDP_CONF1_FSINC_MASK  0x3fffffffU
#define SPWTDP_CONF1_FSINC_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_CONF1_FSINC_MASK ) >> SPWTDP_CONF1_FSINC_SHIFT )
#define SPWTDP_CONF1_FSINC_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_CONF1_FSINC_MASK ) | \
    ( ( ( _val ) << SPWTDP_CONF1_FSINC_SHIFT ) & SPWTDP_CONF1_FSINC_MASK ) )
#define SPWTDP_CONF1_FSINC( _val ) \
  ( ( ( _val ) << SPWTDP_CONF1_FSINC_SHIFT ) & SPWTDP_CONF1_FSINC_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPCONF2 Configuration 2 (CONF2)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_CONF2_CV_SHIFT 8
#define SPWTDP_CONF2_CV_MASK  0xffffff00U
#define SPWTDP_CONF2_CV_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_CONF2_CV_MASK ) >> SPWTDP_CONF2_CV_SHIFT )
#define SPWTDP_CONF2_CV_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_CONF2_CV_MASK ) | \
    ( ( ( _val ) << SPWTDP_CONF2_CV_SHIFT ) & SPWTDP_CONF2_CV_MASK ) )
#define SPWTDP_CONF2_CV( _val ) \
  ( ( ( _val ) << SPWTDP_CONF2_CV_SHIFT ) & SPWTDP_CONF2_CV_MASK )

#define SPWTDP_CONF2_ETINC_SHIFT 0
#define SPWTDP_CONF2_ETINC_MASK  0xffU
#define SPWTDP_CONF2_ETINC_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_CONF2_ETINC_MASK ) >> SPWTDP_CONF2_ETINC_SHIFT )
#define SPWTDP_CONF2_ETINC_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_CONF2_ETINC_MASK ) | \
    ( ( ( _val ) << SPWTDP_CONF2_ETINC_SHIFT ) & SPWTDP_CONF2_ETINC_MASK ) )
#define SPWTDP_CONF2_ETINC( _val ) \
  ( ( ( _val ) << SPWTDP_CONF2_ETINC_SHIFT ) & SPWTDP_CONF2_ETINC_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPCONF3 Configuration 3 (CONF3)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_CONF3_OUTPORT_SHIFT 28
#define SPWTDP_CONF3_OUTPORT_MASK  0xf0000000U
#define SPWTDP_CONF3_OUTPORT_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_CONF3_OUTPORT_MASK ) >> SPWTDP_CONF3_OUTPORT_SHIFT )
#define SPWTDP_CONF3_OUTPORT_SET( _reg, _val )     \
  ( ( ( _reg ) & ~SPWTDP_CONF3_OUTPORT_MASK ) |    \
    ( ( ( _val ) << SPWTDP_CONF3_OUTPORT_SHIFT ) & \
      SPWTDP_CONF3_OUTPORT_MASK ) )
#define SPWTDP_CONF3_OUTPORT( _val ) \
  ( ( ( _val ) << SPWTDP_CONF3_OUTPORT_SHIFT ) & SPWTDP_CONF3_OUTPORT_MASK )

#define SPWTDP_CONF3_INPORT_SHIFT 24
#define SPWTDP_CONF3_INPORT_MASK  0xf000000U
#define SPWTDP_CONF3_INPORT_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_CONF3_INPORT_MASK ) >> SPWTDP_CONF3_INPORT_SHIFT )
#define SPWTDP_CONF3_INPORT_SET( _reg, _val )     \
  ( ( ( _reg ) & ~SPWTDP_CONF3_INPORT_MASK ) |    \
    ( ( ( _val ) << SPWTDP_CONF3_INPORT_SHIFT ) & \
      SPWTDP_CONF3_INPORT_MASK ) )
#define SPWTDP_CONF3_INPORT( _val ) \
  ( ( ( _val ) << SPWTDP_CONF3_INPORT_SHIFT ) & SPWTDP_CONF3_INPORT_MASK )

#define SPWTDP_CONF3_STM_SHIFT 16
#define SPWTDP_CONF3_STM_MASK  0x3f0000U
#define SPWTDP_CONF3_STM_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_CONF3_STM_MASK ) >> SPWTDP_CONF3_STM_SHIFT )
#define SPWTDP_CONF3_STM_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_CONF3_STM_MASK ) | \
    ( ( ( _val ) << SPWTDP_CONF3_STM_SHIFT ) & SPWTDP_CONF3_STM_MASK ) )
#define SPWTDP_CONF3_STM( _val ) \
  ( ( ( _val ) << SPWTDP_CONF3_STM_SHIFT ) & SPWTDP_CONF3_STM_MASK )

#define SPWTDP_CONF3_DI64R 0x2000U

#define SPWTDP_CONF3_DI64T 0x1000U

#define SPWTDP_CONF3_DI64 0x800U

#define SPWTDP_CONF3_DI 0x400U

#define SPWTDP_CONF3_INRX_SHIFT 5
#define SPWTDP_CONF3_INRX_MASK  0x3e0U
#define SPWTDP_CONF3_INRX_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_CONF3_INRX_MASK ) >> SPWTDP_CONF3_INRX_SHIFT )
#define SPWTDP_CONF3_INRX_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_CONF3_INRX_MASK ) | \
    ( ( ( _val ) << SPWTDP_CONF3_INRX_SHIFT ) & SPWTDP_CONF3_INRX_MASK ) )
#define SPWTDP_CONF3_INRX( _val ) \
  ( ( ( _val ) << SPWTDP_CONF3_INRX_SHIFT ) & SPWTDP_CONF3_INRX_MASK )

#define SPWTDP_CONF3_INTX_SHIFT 0
#define SPWTDP_CONF3_INTX_MASK  0x1fU
#define SPWTDP_CONF3_INTX_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_CONF3_INTX_MASK ) >> SPWTDP_CONF3_INTX_SHIFT )
#define SPWTDP_CONF3_INTX_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_CONF3_INTX_MASK ) | \
    ( ( ( _val ) << SPWTDP_CONF3_INTX_SHIFT ) & SPWTDP_CONF3_INTX_MASK ) )
#define SPWTDP_CONF3_INTX( _val ) \
  ( ( ( _val ) << SPWTDP_CONF3_INTX_SHIFT ) & SPWTDP_CONF3_INTX_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPSTAT0 Status Register 0 (STAT0)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_STAT0_MA 0x80000000U

#define SPWTDP_STAT0_RP_SHIFT 29
#define SPWTDP_STAT0_RP_MASK  0x60000000U
#define SPWTDP_STAT0_RP_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_STAT0_RP_MASK ) >> SPWTDP_STAT0_RP_SHIFT )
#define SPWTDP_STAT0_RP_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_STAT0_RP_MASK ) | \
    ( ( ( _val ) << SPWTDP_STAT0_RP_SHIFT ) & SPWTDP_STAT0_RP_MASK ) )
#define SPWTDP_STAT0_RP( _val ) \
  ( ( ( _val ) << SPWTDP_STAT0_RP_SHIFT ) & SPWTDP_STAT0_RP_MASK )

#define SPWTDP_STAT0_EDS_SHIFT 24
#define SPWTDP_STAT0_EDS_MASK  0xf000000U
#define SPWTDP_STAT0_EDS_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_STAT0_EDS_MASK ) >> SPWTDP_STAT0_EDS_SHIFT )
#define SPWTDP_STAT0_EDS_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_STAT0_EDS_MASK ) | \
    ( ( ( _val ) << SPWTDP_STAT0_EDS_SHIFT ) & SPWTDP_STAT0_EDS_MASK ) )
#define SPWTDP_STAT0_EDS( _val ) \
  ( ( ( _val ) << SPWTDP_STAT0_EDS_SHIFT ) & SPWTDP_STAT0_EDS_MASK )

#define SPWTDP_STAT0_FW_SHIFT 16
#define SPWTDP_STAT0_FW_MASK  0x7f0000U
#define SPWTDP_STAT0_FW_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_STAT0_FW_MASK ) >> SPWTDP_STAT0_FW_SHIFT )
#define SPWTDP_STAT0_FW_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_STAT0_FW_MASK ) | \
    ( ( ( _val ) << SPWTDP_STAT0_FW_SHIFT ) & SPWTDP_STAT0_FW_MASK ) )
#define SPWTDP_STAT0_FW( _val ) \
  ( ( ( _val ) << SPWTDP_STAT0_FW_SHIFT ) & SPWTDP_STAT0_FW_MASK )

#define SPWTDP_STAT0_CW_SHIFT 8
#define SPWTDP_STAT0_CW_MASK  0x3f00U
#define SPWTDP_STAT0_CW_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_STAT0_CW_MASK ) >> SPWTDP_STAT0_CW_SHIFT )
#define SPWTDP_STAT0_CW_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_STAT0_CW_MASK ) | \
    ( ( ( _val ) << SPWTDP_STAT0_CW_SHIFT ) & SPWTDP_STAT0_CW_MASK ) )
#define SPWTDP_STAT0_CW( _val ) \
  ( ( ( _val ) << SPWTDP_STAT0_CW_SHIFT ) & SPWTDP_STAT0_CW_MASK )

#define SPWTDP_STAT0_ARM 0x8U

#define SPWTDP_STAT0_LC 0x4U

#define SPWTDP_STAT0_TCQ 0x2U

#define SPWTDP_STAT0_INSYNC 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPSTAT1 Status Register 1 (STAT1)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_STAT1_IV_SHIFT 0
#define SPWTDP_STAT1_IV_MASK  0x3fffffffU
#define SPWTDP_STAT1_IV_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_STAT1_IV_MASK ) >> SPWTDP_STAT1_IV_SHIFT )
#define SPWTDP_STAT1_IV_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_STAT1_IV_MASK ) | \
    ( ( ( _val ) << SPWTDP_STAT1_IV_SHIFT ) & SPWTDP_STAT1_IV_MASK ) )
#define SPWTDP_STAT1_IV( _val ) \
  ( ( ( _val ) << SPWTDP_STAT1_IV_SHIFT ) & SPWTDP_STAT1_IV_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPCTRL Control (CTRL)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_CTRL_NC 0x80000000U

#define SPWTDP_CTRL_IS 0x40000000U

#define SPWTDP_CTRL_SPWTC_SHIFT 16
#define SPWTDP_CTRL_SPWTC_MASK  0xff0000U
#define SPWTDP_CTRL_SPWTC_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_CTRL_SPWTC_MASK ) >> SPWTDP_CTRL_SPWTC_SHIFT )
#define SPWTDP_CTRL_SPWTC_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_CTRL_SPWTC_MASK ) | \
    ( ( ( _val ) << SPWTDP_CTRL_SPWTC_SHIFT ) & SPWTDP_CTRL_SPWTC_MASK ) )
#define SPWTDP_CTRL_SPWTC( _val ) \
  ( ( ( _val ) << SPWTDP_CTRL_SPWTC_SHIFT ) & SPWTDP_CTRL_SPWTC_MASK )

#define SPWTDP_CTRL_CPF_SHIFT 0
#define SPWTDP_CTRL_CPF_MASK  0xffffU
#define SPWTDP_CTRL_CPF_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_CTRL_CPF_MASK ) >> SPWTDP_CTRL_CPF_SHIFT )
#define SPWTDP_CTRL_CPF_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_CTRL_CPF_MASK ) | \
    ( ( ( _val ) << SPWTDP_CTRL_CPF_SHIFT ) & SPWTDP_CTRL_CPF_MASK ) )
#define SPWTDP_CTRL_CPF( _val ) \
  ( ( ( _val ) << SPWTDP_CTRL_CPF_SHIFT ) & SPWTDP_CTRL_CPF_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPCET0 Command Elapsed Time 0 (CET0)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_CET0_CET0_SHIFT 0
#define SPWTDP_CET0_CET0_MASK  0xffffffffU
#define SPWTDP_CET0_CET0_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_CET0_CET0_MASK ) >> SPWTDP_CET0_CET0_SHIFT )
#define SPWTDP_CET0_CET0_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_CET0_CET0_MASK ) | \
    ( ( ( _val ) << SPWTDP_CET0_CET0_SHIFT ) & SPWTDP_CET0_CET0_MASK ) )
#define SPWTDP_CET0_CET0( _val ) \
  ( ( ( _val ) << SPWTDP_CET0_CET0_SHIFT ) & SPWTDP_CET0_CET0_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPCET1 Command Elapsed Time 1 (CET1)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_CET1_CET1_SHIFT 0
#define SPWTDP_CET1_CET1_MASK  0xffffffffU
#define SPWTDP_CET1_CET1_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_CET1_CET1_MASK ) >> SPWTDP_CET1_CET1_SHIFT )
#define SPWTDP_CET1_CET1_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_CET1_CET1_MASK ) | \
    ( ( ( _val ) << SPWTDP_CET1_CET1_SHIFT ) & SPWTDP_CET1_CET1_MASK ) )
#define SPWTDP_CET1_CET1( _val ) \
  ( ( ( _val ) << SPWTDP_CET1_CET1_SHIFT ) & SPWTDP_CET1_CET1_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPCET2 Command Elapsed Time 2 (CET2)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_CET2_CET2_SHIFT 0
#define SPWTDP_CET2_CET2_MASK  0xffffffffU
#define SPWTDP_CET2_CET2_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_CET2_CET2_MASK ) >> SPWTDP_CET2_CET2_SHIFT )
#define SPWTDP_CET2_CET2_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_CET2_CET2_MASK ) | \
    ( ( ( _val ) << SPWTDP_CET2_CET2_SHIFT ) & SPWTDP_CET2_CET2_MASK ) )
#define SPWTDP_CET2_CET2( _val ) \
  ( ( ( _val ) << SPWTDP_CET2_CET2_SHIFT ) & SPWTDP_CET2_CET2_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPCET3 Command Elapsed Time 3 (CET3)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_CET3_CET3_SHIFT 0
#define SPWTDP_CET3_CET3_MASK  0xffffffffU
#define SPWTDP_CET3_CET3_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_CET3_CET3_MASK ) >> SPWTDP_CET3_CET3_SHIFT )
#define SPWTDP_CET3_CET3_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_CET3_CET3_MASK ) | \
    ( ( ( _val ) << SPWTDP_CET3_CET3_SHIFT ) & SPWTDP_CET3_CET3_MASK ) )
#define SPWTDP_CET3_CET3( _val ) \
  ( ( ( _val ) << SPWTDP_CET3_CET3_SHIFT ) & SPWTDP_CET3_CET3_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPCET4 Command Elapsed Time 4 (CET4)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_CET4_CET4_SHIFT 24
#define SPWTDP_CET4_CET4_MASK  0xff000000U
#define SPWTDP_CET4_CET4_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_CET4_CET4_MASK ) >> SPWTDP_CET4_CET4_SHIFT )
#define SPWTDP_CET4_CET4_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_CET4_CET4_MASK ) | \
    ( ( ( _val ) << SPWTDP_CET4_CET4_SHIFT ) & SPWTDP_CET4_CET4_MASK ) )
#define SPWTDP_CET4_CET4( _val ) \
  ( ( ( _val ) << SPWTDP_CET4_CET4_SHIFT ) & SPWTDP_CET4_CET4_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPDPF Datation Preamble Field (DPF)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_DPF_DPF_SHIFT 0
#define SPWTDP_DPF_DPF_MASK  0xffffU
#define SPWTDP_DPF_DPF_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_DPF_DPF_MASK ) >> SPWTDP_DPF_DPF_SHIFT )
#define SPWTDP_DPF_DPF_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_DPF_DPF_MASK ) | \
    ( ( ( _val ) << SPWTDP_DPF_DPF_SHIFT ) & SPWTDP_DPF_DPF_MASK ) )
#define SPWTDP_DPF_DPF( _val ) \
  ( ( ( _val ) << SPWTDP_DPF_DPF_SHIFT ) & SPWTDP_DPF_DPF_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPDET0 Datation Elapsed Time 0 (DET0)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_DET0_DET0_SHIFT 0
#define SPWTDP_DET0_DET0_MASK  0xffffffffU
#define SPWTDP_DET0_DET0_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_DET0_DET0_MASK ) >> SPWTDP_DET0_DET0_SHIFT )
#define SPWTDP_DET0_DET0_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_DET0_DET0_MASK ) | \
    ( ( ( _val ) << SPWTDP_DET0_DET0_SHIFT ) & SPWTDP_DET0_DET0_MASK ) )
#define SPWTDP_DET0_DET0( _val ) \
  ( ( ( _val ) << SPWTDP_DET0_DET0_SHIFT ) & SPWTDP_DET0_DET0_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPDET1 Datation Elapsed Time 1 (DET1)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_DET1_DET1_SHIFT 0
#define SPWTDP_DET1_DET1_MASK  0xffffffffU
#define SPWTDP_DET1_DET1_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_DET1_DET1_MASK ) >> SPWTDP_DET1_DET1_SHIFT )
#define SPWTDP_DET1_DET1_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_DET1_DET1_MASK ) | \
    ( ( ( _val ) << SPWTDP_DET1_DET1_SHIFT ) & SPWTDP_DET1_DET1_MASK ) )
#define SPWTDP_DET1_DET1( _val ) \
  ( ( ( _val ) << SPWTDP_DET1_DET1_SHIFT ) & SPWTDP_DET1_DET1_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPDET2 Datation Elapsed Time 2 (DET2)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_DET2_DET2_SHIFT 0
#define SPWTDP_DET2_DET2_MASK  0xffffffffU
#define SPWTDP_DET2_DET2_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_DET2_DET2_MASK ) >> SPWTDP_DET2_DET2_SHIFT )
#define SPWTDP_DET2_DET2_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_DET2_DET2_MASK ) | \
    ( ( ( _val ) << SPWTDP_DET2_DET2_SHIFT ) & SPWTDP_DET2_DET2_MASK ) )
#define SPWTDP_DET2_DET2( _val ) \
  ( ( ( _val ) << SPWTDP_DET2_DET2_SHIFT ) & SPWTDP_DET2_DET2_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPDET3 Datation Elapsed Time 3 (DET3)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_DET3_DET3_SHIFT 0
#define SPWTDP_DET3_DET3_MASK  0xffffffffU
#define SPWTDP_DET3_DET3_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_DET3_DET3_MASK ) >> SPWTDP_DET3_DET3_SHIFT )
#define SPWTDP_DET3_DET3_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_DET3_DET3_MASK ) | \
    ( ( ( _val ) << SPWTDP_DET3_DET3_SHIFT ) & SPWTDP_DET3_DET3_MASK ) )
#define SPWTDP_DET3_DET3( _val ) \
  ( ( ( _val ) << SPWTDP_DET3_DET3_SHIFT ) & SPWTDP_DET3_DET3_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPDET4 Datation Elapsed Time 4 (DET4)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_DET4_DET4_SHIFT 24
#define SPWTDP_DET4_DET4_MASK  0xff000000U
#define SPWTDP_DET4_DET4_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_DET4_DET4_MASK ) >> SPWTDP_DET4_DET4_SHIFT )
#define SPWTDP_DET4_DET4_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_DET4_DET4_MASK ) | \
    ( ( ( _val ) << SPWTDP_DET4_DET4_SHIFT ) & SPWTDP_DET4_DET4_MASK ) )
#define SPWTDP_DET4_DET4( _val ) \
  ( ( ( _val ) << SPWTDP_DET4_DET4_SHIFT ) & SPWTDP_DET4_DET4_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPTRPFRX Time-Stamp Preamble Field Rx (TRPFRX)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_TRPFRX_TRPF_SHIFT 0
#define SPWTDP_TRPFRX_TRPF_MASK  0xffffU
#define SPWTDP_TRPFRX_TRPF_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_TRPFRX_TRPF_MASK ) >> SPWTDP_TRPFRX_TRPF_SHIFT )
#define SPWTDP_TRPFRX_TRPF_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_TRPFRX_TRPF_MASK ) | \
    ( ( ( _val ) << SPWTDP_TRPFRX_TRPF_SHIFT ) & SPWTDP_TRPFRX_TRPF_MASK ) )
#define SPWTDP_TRPFRX_TRPF( _val ) \
  ( ( ( _val ) << SPWTDP_TRPFRX_TRPF_SHIFT ) & SPWTDP_TRPFRX_TRPF_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPTR0 Time Stamp Elapsed Time 0 Rx (TR0)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_TR0_TR0_SHIFT 0
#define SPWTDP_TR0_TR0_MASK  0xffffffffU
#define SPWTDP_TR0_TR0_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_TR0_TR0_MASK ) >> SPWTDP_TR0_TR0_SHIFT )
#define SPWTDP_TR0_TR0_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_TR0_TR0_MASK ) | \
    ( ( ( _val ) << SPWTDP_TR0_TR0_SHIFT ) & SPWTDP_TR0_TR0_MASK ) )
#define SPWTDP_TR0_TR0( _val ) \
  ( ( ( _val ) << SPWTDP_TR0_TR0_SHIFT ) & SPWTDP_TR0_TR0_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPTR1 Time Stamp Elapsed Time 1 Rx (TR1)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_TR1_TR1_SHIFT 0
#define SPWTDP_TR1_TR1_MASK  0xffffffffU
#define SPWTDP_TR1_TR1_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_TR1_TR1_MASK ) >> SPWTDP_TR1_TR1_SHIFT )
#define SPWTDP_TR1_TR1_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_TR1_TR1_MASK ) | \
    ( ( ( _val ) << SPWTDP_TR1_TR1_SHIFT ) & SPWTDP_TR1_TR1_MASK ) )
#define SPWTDP_TR1_TR1( _val ) \
  ( ( ( _val ) << SPWTDP_TR1_TR1_SHIFT ) & SPWTDP_TR1_TR1_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPTR2 Time Stamp Elapsed Time 2 Rx (TR2)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_TR2_TR2_SHIFT 0
#define SPWTDP_TR2_TR2_MASK  0xffffffffU
#define SPWTDP_TR2_TR2_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_TR2_TR2_MASK ) >> SPWTDP_TR2_TR2_SHIFT )
#define SPWTDP_TR2_TR2_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_TR2_TR2_MASK ) | \
    ( ( ( _val ) << SPWTDP_TR2_TR2_SHIFT ) & SPWTDP_TR2_TR2_MASK ) )
#define SPWTDP_TR2_TR2( _val ) \
  ( ( ( _val ) << SPWTDP_TR2_TR2_SHIFT ) & SPWTDP_TR2_TR2_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPTR3 Time Stamp Elapsed Time 3 Rx (TR3)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_TR3_TR3_SHIFT 0
#define SPWTDP_TR3_TR3_MASK  0xffffffffU
#define SPWTDP_TR3_TR3_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_TR3_TR3_MASK ) >> SPWTDP_TR3_TR3_SHIFT )
#define SPWTDP_TR3_TR3_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_TR3_TR3_MASK ) | \
    ( ( ( _val ) << SPWTDP_TR3_TR3_SHIFT ) & SPWTDP_TR3_TR3_MASK ) )
#define SPWTDP_TR3_TR3( _val ) \
  ( ( ( _val ) << SPWTDP_TR3_TR3_SHIFT ) & SPWTDP_TR3_TR3_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPTR4 Time Stamp Elapsed Time 4 Rx (TR4)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_TR4_TR4_SHIFT 24
#define SPWTDP_TR4_TR4_MASK  0xff000000U
#define SPWTDP_TR4_TR4_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_TR4_TR4_MASK ) >> SPWTDP_TR4_TR4_SHIFT )
#define SPWTDP_TR4_TR4_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_TR4_TR4_MASK ) | \
    ( ( ( _val ) << SPWTDP_TR4_TR4_SHIFT ) & SPWTDP_TR4_TR4_MASK ) )
#define SPWTDP_TR4_TR4( _val ) \
  ( ( ( _val ) << SPWTDP_TR4_TR4_SHIFT ) & SPWTDP_TR4_TR4_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPTTPFTX \
 *   Time-Stamp SpaceWire Time-Code and Preamble Field Tx (TTPFTX)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_TTPFTX_TSTC_SHIFT 24
#define SPWTDP_TTPFTX_TSTC_MASK  0xff000000U
#define SPWTDP_TTPFTX_TSTC_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_TTPFTX_TSTC_MASK ) >> SPWTDP_TTPFTX_TSTC_SHIFT )
#define SPWTDP_TTPFTX_TSTC_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_TTPFTX_TSTC_MASK ) | \
    ( ( ( _val ) << SPWTDP_TTPFTX_TSTC_SHIFT ) & SPWTDP_TTPFTX_TSTC_MASK ) )
#define SPWTDP_TTPFTX_TSTC( _val ) \
  ( ( ( _val ) << SPWTDP_TTPFTX_TSTC_SHIFT ) & SPWTDP_TTPFTX_TSTC_MASK )

#define SPWTDP_TTPFTX_TTPF_SHIFT 0
#define SPWTDP_TTPFTX_TTPF_MASK  0xffffU
#define SPWTDP_TTPFTX_TTPF_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_TTPFTX_TTPF_MASK ) >> SPWTDP_TTPFTX_TTPF_SHIFT )
#define SPWTDP_TTPFTX_TTPF_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_TTPFTX_TTPF_MASK ) | \
    ( ( ( _val ) << SPWTDP_TTPFTX_TTPF_SHIFT ) & SPWTDP_TTPFTX_TTPF_MASK ) )
#define SPWTDP_TTPFTX_TTPF( _val ) \
  ( ( ( _val ) << SPWTDP_TTPFTX_TTPF_SHIFT ) & SPWTDP_TTPFTX_TTPF_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPTT0 Time Stamp Elapsed Time 0 Tx (TT0)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_TT0_TT0_SHIFT 0
#define SPWTDP_TT0_TT0_MASK  0xffffffffU
#define SPWTDP_TT0_TT0_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_TT0_TT0_MASK ) >> SPWTDP_TT0_TT0_SHIFT )
#define SPWTDP_TT0_TT0_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_TT0_TT0_MASK ) | \
    ( ( ( _val ) << SPWTDP_TT0_TT0_SHIFT ) & SPWTDP_TT0_TT0_MASK ) )
#define SPWTDP_TT0_TT0( _val ) \
  ( ( ( _val ) << SPWTDP_TT0_TT0_SHIFT ) & SPWTDP_TT0_TT0_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPTT1 Time Stamp Elapsed Time 1 Tx (TT1)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_TT1_TT1_SHIFT 0
#define SPWTDP_TT1_TT1_MASK  0xffffffffU
#define SPWTDP_TT1_TT1_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_TT1_TT1_MASK ) >> SPWTDP_TT1_TT1_SHIFT )
#define SPWTDP_TT1_TT1_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_TT1_TT1_MASK ) | \
    ( ( ( _val ) << SPWTDP_TT1_TT1_SHIFT ) & SPWTDP_TT1_TT1_MASK ) )
#define SPWTDP_TT1_TT1( _val ) \
  ( ( ( _val ) << SPWTDP_TT1_TT1_SHIFT ) & SPWTDP_TT1_TT1_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPTT2 Time Stamp Elapsed Time 2 Tx (TT2)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_TT2_TT2_SHIFT 0
#define SPWTDP_TT2_TT2_MASK  0xffffffffU
#define SPWTDP_TT2_TT2_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_TT2_TT2_MASK ) >> SPWTDP_TT2_TT2_SHIFT )
#define SPWTDP_TT2_TT2_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_TT2_TT2_MASK ) | \
    ( ( ( _val ) << SPWTDP_TT2_TT2_SHIFT ) & SPWTDP_TT2_TT2_MASK ) )
#define SPWTDP_TT2_TT2( _val ) \
  ( ( ( _val ) << SPWTDP_TT2_TT2_SHIFT ) & SPWTDP_TT2_TT2_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPTT3 Time Stamp Elapsed Time 3 Tx (TT3)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_TT3_TT3_SHIFT 0
#define SPWTDP_TT3_TT3_MASK  0xffffffffU
#define SPWTDP_TT3_TT3_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_TT3_TT3_MASK ) >> SPWTDP_TT3_TT3_SHIFT )
#define SPWTDP_TT3_TT3_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_TT3_TT3_MASK ) | \
    ( ( ( _val ) << SPWTDP_TT3_TT3_SHIFT ) & SPWTDP_TT3_TT3_MASK ) )
#define SPWTDP_TT3_TT3( _val ) \
  ( ( ( _val ) << SPWTDP_TT3_TT3_SHIFT ) & SPWTDP_TT3_TT3_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPTT4 Time Stamp Elapsed Time 4 Tx (TT4)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_TT4_TT4_SHIFT 24
#define SPWTDP_TT4_TT4_MASK  0xff000000U
#define SPWTDP_TT4_TT4_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_TT4_TT4_MASK ) >> SPWTDP_TT4_TT4_SHIFT )
#define SPWTDP_TT4_TT4_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_TT4_TT4_MASK ) | \
    ( ( ( _val ) << SPWTDP_TT4_TT4_SHIFT ) & SPWTDP_TT4_TT4_MASK ) )
#define SPWTDP_TT4_TT4( _val ) \
  ( ( ( _val ) << SPWTDP_TT4_TT4_SHIFT ) & SPWTDP_TT4_TT4_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPLPF Latency Preamble Field (LPF)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_LPF_LPF_SHIFT 0
#define SPWTDP_LPF_LPF_MASK  0xffffU
#define SPWTDP_LPF_LPF_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_LPF_LPF_MASK ) >> SPWTDP_LPF_LPF_SHIFT )
#define SPWTDP_LPF_LPF_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_LPF_LPF_MASK ) | \
    ( ( ( _val ) << SPWTDP_LPF_LPF_SHIFT ) & SPWTDP_LPF_LPF_MASK ) )
#define SPWTDP_LPF_LPF( _val ) \
  ( ( ( _val ) << SPWTDP_LPF_LPF_SHIFT ) & SPWTDP_LPF_LPF_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPLE0 Latency Elapsed Time 0 (LE0)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_LE0_LE0_SHIFT 0
#define SPWTDP_LE0_LE0_MASK  0xffffffffU
#define SPWTDP_LE0_LE0_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_LE0_LE0_MASK ) >> SPWTDP_LE0_LE0_SHIFT )
#define SPWTDP_LE0_LE0_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_LE0_LE0_MASK ) | \
    ( ( ( _val ) << SPWTDP_LE0_LE0_SHIFT ) & SPWTDP_LE0_LE0_MASK ) )
#define SPWTDP_LE0_LE0( _val ) \
  ( ( ( _val ) << SPWTDP_LE0_LE0_SHIFT ) & SPWTDP_LE0_LE0_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPLE1 Latency Elapsed Time 1 (LE1)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_LE1_LE1_SHIFT 0
#define SPWTDP_LE1_LE1_MASK  0xffffffffU
#define SPWTDP_LE1_LE1_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_LE1_LE1_MASK ) >> SPWTDP_LE1_LE1_SHIFT )
#define SPWTDP_LE1_LE1_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_LE1_LE1_MASK ) | \
    ( ( ( _val ) << SPWTDP_LE1_LE1_SHIFT ) & SPWTDP_LE1_LE1_MASK ) )
#define SPWTDP_LE1_LE1( _val ) \
  ( ( ( _val ) << SPWTDP_LE1_LE1_SHIFT ) & SPWTDP_LE1_LE1_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPLE2 Latency Elapsed Time 2 (LE2)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_LE2_LE2_SHIFT 0
#define SPWTDP_LE2_LE2_MASK  0xffffffffU
#define SPWTDP_LE2_LE2_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_LE2_LE2_MASK ) >> SPWTDP_LE2_LE2_SHIFT )
#define SPWTDP_LE2_LE2_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_LE2_LE2_MASK ) | \
    ( ( ( _val ) << SPWTDP_LE2_LE2_SHIFT ) & SPWTDP_LE2_LE2_MASK ) )
#define SPWTDP_LE2_LE2( _val ) \
  ( ( ( _val ) << SPWTDP_LE2_LE2_SHIFT ) & SPWTDP_LE2_LE2_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPLE3 Latency Elapsed Time 3 (LE3)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_LE3_LE3_SHIFT 0
#define SPWTDP_LE3_LE3_MASK  0xffffffffU
#define SPWTDP_LE3_LE3_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_LE3_LE3_MASK ) >> SPWTDP_LE3_LE3_SHIFT )
#define SPWTDP_LE3_LE3_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_LE3_LE3_MASK ) | \
    ( ( ( _val ) << SPWTDP_LE3_LE3_SHIFT ) & SPWTDP_LE3_LE3_MASK ) )
#define SPWTDP_LE3_LE3( _val ) \
  ( ( ( _val ) << SPWTDP_LE3_LE3_SHIFT ) & SPWTDP_LE3_LE3_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPLE4 Latency Elapsed Time 4 (LE4)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_LE4_LE4_SHIFT 24
#define SPWTDP_LE4_LE4_MASK  0xff000000U
#define SPWTDP_LE4_LE4_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_LE4_LE4_MASK ) >> SPWTDP_LE4_LE4_SHIFT )
#define SPWTDP_LE4_LE4_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_LE4_LE4_MASK ) | \
    ( ( ( _val ) << SPWTDP_LE4_LE4_SHIFT ) & SPWTDP_LE4_LE4_MASK ) )
#define SPWTDP_LE4_LE4( _val ) \
  ( ( ( _val ) << SPWTDP_LE4_LE4_SHIFT ) & SPWTDP_LE4_LE4_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPIE Interrupt Enable (IE)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_IE_NCTCE 0x80000U

#define SPWTDP_IE_PE_SHIFT 11
#define SPWTDP_IE_PE_MASK  0x7f800U
#define SPWTDP_IE_PE_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_IE_PE_MASK ) >> SPWTDP_IE_PE_SHIFT )
#define SPWTDP_IE_PE_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_IE_PE_MASK ) | \
    ( ( ( _val ) << SPWTDP_IE_PE_SHIFT ) & SPWTDP_IE_PE_MASK ) )
#define SPWTDP_IE_PE( _val ) \
  ( ( ( _val ) << SPWTDP_IE_PE_SHIFT ) & SPWTDP_IE_PE_MASK )

#define SPWTDP_IE_SETE 0x400U

#define SPWTDP_IE_EDIE3 0x200U

#define SPWTDP_IE_EDIE2 0x100U

#define SPWTDP_IE_EDIE1 0x80U

#define SPWTDP_IE_EDIE0 0x40U

#define SPWTDP_IE_DITE 0x20U

#define SPWTDP_IE_DIRE 0x10U

#define SPWTDP_IE_TTE 0x8U

#define SPWTDP_IE_TME 0x4U

#define SPWTDP_IE_TRE 0x2U

#define SPWTDP_IE_SE 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPIS Interrupt Status (IS)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_IS_NCTC 0x80000U

#define SPWTDP_IS_P_SHIFT 11
#define SPWTDP_IS_P_MASK  0x7f800U
#define SPWTDP_IS_P_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_IS_P_MASK ) >> SPWTDP_IS_P_SHIFT )
#define SPWTDP_IS_P_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_IS_P_MASK ) | \
    ( ( ( _val ) << SPWTDP_IS_P_SHIFT ) & SPWTDP_IS_P_MASK ) )
#define SPWTDP_IS_P( _val ) \
  ( ( ( _val ) << SPWTDP_IS_P_SHIFT ) & SPWTDP_IS_P_MASK )

#define SPWTDP_IS_SET 0x400U

#define SPWTDP_IS_EDI3 0x200U

#define SPWTDP_IS_EDI2 0x100U

#define SPWTDP_IS_EDI1 0x80U

#define SPWTDP_IS_EDI0 0x40U

#define SPWTDP_IS_DIT 0x20U

#define SPWTDP_IS_DIR 0x10U

#define SPWTDP_IS_TT 0x8U

#define SPWTDP_IS_TM 0x4U

#define SPWTDP_IS_TR 0x2U

#define SPWTDP_IS_S 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPDC Delay Count (DC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_DC_DC_SHIFT 0
#define SPWTDP_DC_DC_MASK  0x7fffU
#define SPWTDP_DC_DC_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_DC_DC_MASK ) >> SPWTDP_DC_DC_SHIFT )
#define SPWTDP_DC_DC_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_DC_DC_MASK ) | \
    ( ( ( _val ) << SPWTDP_DC_DC_SHIFT ) & SPWTDP_DC_DC_MASK ) )
#define SPWTDP_DC_DC( _val ) \
  ( ( ( _val ) << SPWTDP_DC_DC_SHIFT ) & SPWTDP_DC_DC_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPDS Disable Sync (DS)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_DS_EN 0x80000000U

#define SPWTDP_DS_CD_SHIFT 0
#define SPWTDP_DS_CD_MASK  0xffffffU
#define SPWTDP_DS_CD_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_DS_CD_MASK ) >> SPWTDP_DS_CD_SHIFT )
#define SPWTDP_DS_CD_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_DS_CD_MASK ) | \
    ( ( ( _val ) << SPWTDP_DS_CD_SHIFT ) & SPWTDP_DS_CD_MASK ) )
#define SPWTDP_DS_CD( _val ) \
  ( ( ( _val ) << SPWTDP_DS_CD_SHIFT ) & SPWTDP_DS_CD_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPEDM0 External Datation 0 Mask (EDM0)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_EDM0_EDM0_SHIFT 0
#define SPWTDP_EDM0_EDM0_MASK  0xffffffffU
#define SPWTDP_EDM0_EDM0_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_EDM0_EDM0_MASK ) >> SPWTDP_EDM0_EDM0_SHIFT )
#define SPWTDP_EDM0_EDM0_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_EDM0_EDM0_MASK ) | \
    ( ( ( _val ) << SPWTDP_EDM0_EDM0_SHIFT ) & SPWTDP_EDM0_EDM0_MASK ) )
#define SPWTDP_EDM0_EDM0( _val ) \
  ( ( ( _val ) << SPWTDP_EDM0_EDM0_SHIFT ) & SPWTDP_EDM0_EDM0_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPEDM1 External Datation 1 Mask (EDM1)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_EDM1_EDM1_SHIFT 0
#define SPWTDP_EDM1_EDM1_MASK  0xffffffffU
#define SPWTDP_EDM1_EDM1_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_EDM1_EDM1_MASK ) >> SPWTDP_EDM1_EDM1_SHIFT )
#define SPWTDP_EDM1_EDM1_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_EDM1_EDM1_MASK ) | \
    ( ( ( _val ) << SPWTDP_EDM1_EDM1_SHIFT ) & SPWTDP_EDM1_EDM1_MASK ) )
#define SPWTDP_EDM1_EDM1( _val ) \
  ( ( ( _val ) << SPWTDP_EDM1_EDM1_SHIFT ) & SPWTDP_EDM1_EDM1_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPEDM2 External Datation 2 Mask (EDM2)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_EDM2_EDM2_SHIFT 0
#define SPWTDP_EDM2_EDM2_MASK  0xffffffffU
#define SPWTDP_EDM2_EDM2_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_EDM2_EDM2_MASK ) >> SPWTDP_EDM2_EDM2_SHIFT )
#define SPWTDP_EDM2_EDM2_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_EDM2_EDM2_MASK ) | \
    ( ( ( _val ) << SPWTDP_EDM2_EDM2_SHIFT ) & SPWTDP_EDM2_EDM2_MASK ) )
#define SPWTDP_EDM2_EDM2( _val ) \
  ( ( ( _val ) << SPWTDP_EDM2_EDM2_SHIFT ) & SPWTDP_EDM2_EDM2_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPEDM3 External Datation 3 Mask (EDM3)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_EDM3_EDM3_SHIFT 0
#define SPWTDP_EDM3_EDM3_MASK  0xffffffffU
#define SPWTDP_EDM3_EDM3_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_EDM3_EDM3_MASK ) >> SPWTDP_EDM3_EDM3_SHIFT )
#define SPWTDP_EDM3_EDM3_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_EDM3_EDM3_MASK ) | \
    ( ( ( _val ) << SPWTDP_EDM3_EDM3_SHIFT ) & SPWTDP_EDM3_EDM3_MASK ) )
#define SPWTDP_EDM3_EDM3( _val ) \
  ( ( ( _val ) << SPWTDP_EDM3_EDM3_SHIFT ) & SPWTDP_EDM3_EDM3_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPEDPF0 \
 *   External Datation 0 Preamble Field (EDPF0)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_EDPF0_EDPF0_SHIFT 0
#define SPWTDP_EDPF0_EDPF0_MASK  0xffffU
#define SPWTDP_EDPF0_EDPF0_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_EDPF0_EDPF0_MASK ) >> SPWTDP_EDPF0_EDPF0_SHIFT )
#define SPWTDP_EDPF0_EDPF0_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_EDPF0_EDPF0_MASK ) | \
    ( ( ( _val ) << SPWTDP_EDPF0_EDPF0_SHIFT ) & SPWTDP_EDPF0_EDPF0_MASK ) )
#define SPWTDP_EDPF0_EDPF0( _val ) \
  ( ( ( _val ) << SPWTDP_EDPF0_EDPF0_SHIFT ) & SPWTDP_EDPF0_EDPF0_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPED0ET0 \
 *   External Datation 0 Elapsed Time 0 (ED0ET0)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_ED0ET0_ED0ET0_SHIFT 0
#define SPWTDP_ED0ET0_ED0ET0_MASK  0xffffffffU
#define SPWTDP_ED0ET0_ED0ET0_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_ED0ET0_ED0ET0_MASK ) >> SPWTDP_ED0ET0_ED0ET0_SHIFT )
#define SPWTDP_ED0ET0_ED0ET0_SET( _reg, _val )     \
  ( ( ( _reg ) & ~SPWTDP_ED0ET0_ED0ET0_MASK ) |    \
    ( ( ( _val ) << SPWTDP_ED0ET0_ED0ET0_SHIFT ) & \
      SPWTDP_ED0ET0_ED0ET0_MASK ) )
#define SPWTDP_ED0ET0_ED0ET0( _val ) \
  ( ( ( _val ) << SPWTDP_ED0ET0_ED0ET0_SHIFT ) & SPWTDP_ED0ET0_ED0ET0_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPED0ET1 \
 *   External Datation 0 Elapsed Time 1 (ED0ET1)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_ED0ET1_ED0ET1_SHIFT 0
#define SPWTDP_ED0ET1_ED0ET1_MASK  0xffffffffU
#define SPWTDP_ED0ET1_ED0ET1_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_ED0ET1_ED0ET1_MASK ) >> SPWTDP_ED0ET1_ED0ET1_SHIFT )
#define SPWTDP_ED0ET1_ED0ET1_SET( _reg, _val )     \
  ( ( ( _reg ) & ~SPWTDP_ED0ET1_ED0ET1_MASK ) |    \
    ( ( ( _val ) << SPWTDP_ED0ET1_ED0ET1_SHIFT ) & \
      SPWTDP_ED0ET1_ED0ET1_MASK ) )
#define SPWTDP_ED0ET1_ED0ET1( _val ) \
  ( ( ( _val ) << SPWTDP_ED0ET1_ED0ET1_SHIFT ) & SPWTDP_ED0ET1_ED0ET1_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPED0ET2 \
 *   External Datation 0 Elapsed Time 2 (ED0ET2)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_ED0ET2_ED0ET2_SHIFT 0
#define SPWTDP_ED0ET2_ED0ET2_MASK  0xffffffffU
#define SPWTDP_ED0ET2_ED0ET2_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_ED0ET2_ED0ET2_MASK ) >> SPWTDP_ED0ET2_ED0ET2_SHIFT )
#define SPWTDP_ED0ET2_ED0ET2_SET( _reg, _val )     \
  ( ( ( _reg ) & ~SPWTDP_ED0ET2_ED0ET2_MASK ) |    \
    ( ( ( _val ) << SPWTDP_ED0ET2_ED0ET2_SHIFT ) & \
      SPWTDP_ED0ET2_ED0ET2_MASK ) )
#define SPWTDP_ED0ET2_ED0ET2( _val ) \
  ( ( ( _val ) << SPWTDP_ED0ET2_ED0ET2_SHIFT ) & SPWTDP_ED0ET2_ED0ET2_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPED0ET3 \
 *   External Datation 0 Elapsed Time 3 (ED0ET3)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_ED0ET3_ED0ET3_SHIFT 0
#define SPWTDP_ED0ET3_ED0ET3_MASK  0xffffffffU
#define SPWTDP_ED0ET3_ED0ET3_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_ED0ET3_ED0ET3_MASK ) >> SPWTDP_ED0ET3_ED0ET3_SHIFT )
#define SPWTDP_ED0ET3_ED0ET3_SET( _reg, _val )     \
  ( ( ( _reg ) & ~SPWTDP_ED0ET3_ED0ET3_MASK ) |    \
    ( ( ( _val ) << SPWTDP_ED0ET3_ED0ET3_SHIFT ) & \
      SPWTDP_ED0ET3_ED0ET3_MASK ) )
#define SPWTDP_ED0ET3_ED0ET3( _val ) \
  ( ( ( _val ) << SPWTDP_ED0ET3_ED0ET3_SHIFT ) & SPWTDP_ED0ET3_ED0ET3_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPED0ET4 \
 *   External Datation 0 Elapsed Time 4 (ED0ET4)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_ED0ET4_ED0ET4_SHIFT 24
#define SPWTDP_ED0ET4_ED0ET4_MASK  0xff000000U
#define SPWTDP_ED0ET4_ED0ET4_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_ED0ET4_ED0ET4_MASK ) >> SPWTDP_ED0ET4_ED0ET4_SHIFT )
#define SPWTDP_ED0ET4_ED0ET4_SET( _reg, _val )     \
  ( ( ( _reg ) & ~SPWTDP_ED0ET4_ED0ET4_MASK ) |    \
    ( ( ( _val ) << SPWTDP_ED0ET4_ED0ET4_SHIFT ) & \
      SPWTDP_ED0ET4_ED0ET4_MASK ) )
#define SPWTDP_ED0ET4_ED0ET4( _val ) \
  ( ( ( _val ) << SPWTDP_ED0ET4_ED0ET4_SHIFT ) & SPWTDP_ED0ET4_ED0ET4_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPEDPF1 \
 *   External Datation 1 Preamble Field (EDPF1)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_EDPF1_EDPF1_SHIFT 0
#define SPWTDP_EDPF1_EDPF1_MASK  0xffffU
#define SPWTDP_EDPF1_EDPF1_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_EDPF1_EDPF1_MASK ) >> SPWTDP_EDPF1_EDPF1_SHIFT )
#define SPWTDP_EDPF1_EDPF1_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_EDPF1_EDPF1_MASK ) | \
    ( ( ( _val ) << SPWTDP_EDPF1_EDPF1_SHIFT ) & SPWTDP_EDPF1_EDPF1_MASK ) )
#define SPWTDP_EDPF1_EDPF1( _val ) \
  ( ( ( _val ) << SPWTDP_EDPF1_EDPF1_SHIFT ) & SPWTDP_EDPF1_EDPF1_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPED1ET0 \
 *   External Datation 1 Elapsed Time 0 (ED1ET0)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_ED1ET0_ED1ET0_SHIFT 0
#define SPWTDP_ED1ET0_ED1ET0_MASK  0xffffffffU
#define SPWTDP_ED1ET0_ED1ET0_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_ED1ET0_ED1ET0_MASK ) >> SPWTDP_ED1ET0_ED1ET0_SHIFT )
#define SPWTDP_ED1ET0_ED1ET0_SET( _reg, _val )     \
  ( ( ( _reg ) & ~SPWTDP_ED1ET0_ED1ET0_MASK ) |    \
    ( ( ( _val ) << SPWTDP_ED1ET0_ED1ET0_SHIFT ) & \
      SPWTDP_ED1ET0_ED1ET0_MASK ) )
#define SPWTDP_ED1ET0_ED1ET0( _val ) \
  ( ( ( _val ) << SPWTDP_ED1ET0_ED1ET0_SHIFT ) & SPWTDP_ED1ET0_ED1ET0_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPED1ET1 \
 *   External Datation 1 Elapsed Time 1 (ED1ET1)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_ED1ET1_ED1ET1_SHIFT 0
#define SPWTDP_ED1ET1_ED1ET1_MASK  0xffffffffU
#define SPWTDP_ED1ET1_ED1ET1_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_ED1ET1_ED1ET1_MASK ) >> SPWTDP_ED1ET1_ED1ET1_SHIFT )
#define SPWTDP_ED1ET1_ED1ET1_SET( _reg, _val )     \
  ( ( ( _reg ) & ~SPWTDP_ED1ET1_ED1ET1_MASK ) |    \
    ( ( ( _val ) << SPWTDP_ED1ET1_ED1ET1_SHIFT ) & \
      SPWTDP_ED1ET1_ED1ET1_MASK ) )
#define SPWTDP_ED1ET1_ED1ET1( _val ) \
  ( ( ( _val ) << SPWTDP_ED1ET1_ED1ET1_SHIFT ) & SPWTDP_ED1ET1_ED1ET1_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPED1ET2 \
 *   External Datation 1 Elapsed Time 2 (ED1ET2)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_ED1ET2_ED1ET2_SHIFT 0
#define SPWTDP_ED1ET2_ED1ET2_MASK  0xffffffffU
#define SPWTDP_ED1ET2_ED1ET2_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_ED1ET2_ED1ET2_MASK ) >> SPWTDP_ED1ET2_ED1ET2_SHIFT )
#define SPWTDP_ED1ET2_ED1ET2_SET( _reg, _val )     \
  ( ( ( _reg ) & ~SPWTDP_ED1ET2_ED1ET2_MASK ) |    \
    ( ( ( _val ) << SPWTDP_ED1ET2_ED1ET2_SHIFT ) & \
      SPWTDP_ED1ET2_ED1ET2_MASK ) )
#define SPWTDP_ED1ET2_ED1ET2( _val ) \
  ( ( ( _val ) << SPWTDP_ED1ET2_ED1ET2_SHIFT ) & SPWTDP_ED1ET2_ED1ET2_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPED1ET3 \
 *   External Datation 1 Elapsed Time 3 (ED1ET3)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_ED1ET3_ED1ET3_SHIFT 0
#define SPWTDP_ED1ET3_ED1ET3_MASK  0xffffffffU
#define SPWTDP_ED1ET3_ED1ET3_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_ED1ET3_ED1ET3_MASK ) >> SPWTDP_ED1ET3_ED1ET3_SHIFT )
#define SPWTDP_ED1ET3_ED1ET3_SET( _reg, _val )     \
  ( ( ( _reg ) & ~SPWTDP_ED1ET3_ED1ET3_MASK ) |    \
    ( ( ( _val ) << SPWTDP_ED1ET3_ED1ET3_SHIFT ) & \
      SPWTDP_ED1ET3_ED1ET3_MASK ) )
#define SPWTDP_ED1ET3_ED1ET3( _val ) \
  ( ( ( _val ) << SPWTDP_ED1ET3_ED1ET3_SHIFT ) & SPWTDP_ED1ET3_ED1ET3_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPED1ET4 \
 *   External Datation 1 Elapsed Time 4 (ED1ET4)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_ED1ET4_ED1ET4_SHIFT 24
#define SPWTDP_ED1ET4_ED1ET4_MASK  0xff000000U
#define SPWTDP_ED1ET4_ED1ET4_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_ED1ET4_ED1ET4_MASK ) >> SPWTDP_ED1ET4_ED1ET4_SHIFT )
#define SPWTDP_ED1ET4_ED1ET4_SET( _reg, _val )     \
  ( ( ( _reg ) & ~SPWTDP_ED1ET4_ED1ET4_MASK ) |    \
    ( ( ( _val ) << SPWTDP_ED1ET4_ED1ET4_SHIFT ) & \
      SPWTDP_ED1ET4_ED1ET4_MASK ) )
#define SPWTDP_ED1ET4_ED1ET4( _val ) \
  ( ( ( _val ) << SPWTDP_ED1ET4_ED1ET4_SHIFT ) & SPWTDP_ED1ET4_ED1ET4_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPEDPF2 \
 *   External Datation 2 Preamble Field (EDPF2)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_EDPF2_EDPF2_SHIFT 0
#define SPWTDP_EDPF2_EDPF2_MASK  0xffffU
#define SPWTDP_EDPF2_EDPF2_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_EDPF2_EDPF2_MASK ) >> SPWTDP_EDPF2_EDPF2_SHIFT )
#define SPWTDP_EDPF2_EDPF2_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_EDPF2_EDPF2_MASK ) | \
    ( ( ( _val ) << SPWTDP_EDPF2_EDPF2_SHIFT ) & SPWTDP_EDPF2_EDPF2_MASK ) )
#define SPWTDP_EDPF2_EDPF2( _val ) \
  ( ( ( _val ) << SPWTDP_EDPF2_EDPF2_SHIFT ) & SPWTDP_EDPF2_EDPF2_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPED2ET0 \
 *   External Datation 2 Elapsed Time 0 (ED2ET0)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_ED2ET0_ED2ET0_SHIFT 0
#define SPWTDP_ED2ET0_ED2ET0_MASK  0xffffffffU
#define SPWTDP_ED2ET0_ED2ET0_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_ED2ET0_ED2ET0_MASK ) >> SPWTDP_ED2ET0_ED2ET0_SHIFT )
#define SPWTDP_ED2ET0_ED2ET0_SET( _reg, _val )     \
  ( ( ( _reg ) & ~SPWTDP_ED2ET0_ED2ET0_MASK ) |    \
    ( ( ( _val ) << SPWTDP_ED2ET0_ED2ET0_SHIFT ) & \
      SPWTDP_ED2ET0_ED2ET0_MASK ) )
#define SPWTDP_ED2ET0_ED2ET0( _val ) \
  ( ( ( _val ) << SPWTDP_ED2ET0_ED2ET0_SHIFT ) & SPWTDP_ED2ET0_ED2ET0_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPED2ET1 \
 *   External Datation 2 Elapsed Time 1 (ED2ET1)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_ED2ET1_ED2ET1_SHIFT 0
#define SPWTDP_ED2ET1_ED2ET1_MASK  0xffffffffU
#define SPWTDP_ED2ET1_ED2ET1_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_ED2ET1_ED2ET1_MASK ) >> SPWTDP_ED2ET1_ED2ET1_SHIFT )
#define SPWTDP_ED2ET1_ED2ET1_SET( _reg, _val )     \
  ( ( ( _reg ) & ~SPWTDP_ED2ET1_ED2ET1_MASK ) |    \
    ( ( ( _val ) << SPWTDP_ED2ET1_ED2ET1_SHIFT ) & \
      SPWTDP_ED2ET1_ED2ET1_MASK ) )
#define SPWTDP_ED2ET1_ED2ET1( _val ) \
  ( ( ( _val ) << SPWTDP_ED2ET1_ED2ET1_SHIFT ) & SPWTDP_ED2ET1_ED2ET1_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPED2ET2 \
 *   External Datation 2 Elapsed Time 2 (ED2ET2)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_ED2ET2_ED2ET2_SHIFT 0
#define SPWTDP_ED2ET2_ED2ET2_MASK  0xffffffffU
#define SPWTDP_ED2ET2_ED2ET2_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_ED2ET2_ED2ET2_MASK ) >> SPWTDP_ED2ET2_ED2ET2_SHIFT )
#define SPWTDP_ED2ET2_ED2ET2_SET( _reg, _val )     \
  ( ( ( _reg ) & ~SPWTDP_ED2ET2_ED2ET2_MASK ) |    \
    ( ( ( _val ) << SPWTDP_ED2ET2_ED2ET2_SHIFT ) & \
      SPWTDP_ED2ET2_ED2ET2_MASK ) )
#define SPWTDP_ED2ET2_ED2ET2( _val ) \
  ( ( ( _val ) << SPWTDP_ED2ET2_ED2ET2_SHIFT ) & SPWTDP_ED2ET2_ED2ET2_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPED2ET3 \
 *   External Datation 2 Elapsed Time 3 (ED2ET3)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_ED2ET3_ED2ET3_SHIFT 0
#define SPWTDP_ED2ET3_ED2ET3_MASK  0xffffffffU
#define SPWTDP_ED2ET3_ED2ET3_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_ED2ET3_ED2ET3_MASK ) >> SPWTDP_ED2ET3_ED2ET3_SHIFT )
#define SPWTDP_ED2ET3_ED2ET3_SET( _reg, _val )     \
  ( ( ( _reg ) & ~SPWTDP_ED2ET3_ED2ET3_MASK ) |    \
    ( ( ( _val ) << SPWTDP_ED2ET3_ED2ET3_SHIFT ) & \
      SPWTDP_ED2ET3_ED2ET3_MASK ) )
#define SPWTDP_ED2ET3_ED2ET3( _val ) \
  ( ( ( _val ) << SPWTDP_ED2ET3_ED2ET3_SHIFT ) & SPWTDP_ED2ET3_ED2ET3_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPED2ET4 \
 *   External Datation 2 Elapsed Time 4 (ED2ET4)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_ED2ET4_ED2ET4_SHIFT 24
#define SPWTDP_ED2ET4_ED2ET4_MASK  0xff000000U
#define SPWTDP_ED2ET4_ED2ET4_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_ED2ET4_ED2ET4_MASK ) >> SPWTDP_ED2ET4_ED2ET4_SHIFT )
#define SPWTDP_ED2ET4_ED2ET4_SET( _reg, _val )     \
  ( ( ( _reg ) & ~SPWTDP_ED2ET4_ED2ET4_MASK ) |    \
    ( ( ( _val ) << SPWTDP_ED2ET4_ED2ET4_SHIFT ) & \
      SPWTDP_ED2ET4_ED2ET4_MASK ) )
#define SPWTDP_ED2ET4_ED2ET4( _val ) \
  ( ( ( _val ) << SPWTDP_ED2ET4_ED2ET4_SHIFT ) & SPWTDP_ED2ET4_ED2ET4_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPEDPF3 \
 *   External Datation 3 Preamble Field (EDPF3)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_EDPF3_EDPF3_SHIFT 0
#define SPWTDP_EDPF3_EDPF3_MASK  0xffffU
#define SPWTDP_EDPF3_EDPF3_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_EDPF3_EDPF3_MASK ) >> SPWTDP_EDPF3_EDPF3_SHIFT )
#define SPWTDP_EDPF3_EDPF3_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_EDPF3_EDPF3_MASK ) | \
    ( ( ( _val ) << SPWTDP_EDPF3_EDPF3_SHIFT ) & SPWTDP_EDPF3_EDPF3_MASK ) )
#define SPWTDP_EDPF3_EDPF3( _val ) \
  ( ( ( _val ) << SPWTDP_EDPF3_EDPF3_SHIFT ) & SPWTDP_EDPF3_EDPF3_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPED3ET0 \
 *   External Datation 3 Elapsed Time 0 (ED3ET0)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_ED3ET0_ED3ET0_SHIFT 0
#define SPWTDP_ED3ET0_ED3ET0_MASK  0xffffffffU
#define SPWTDP_ED3ET0_ED3ET0_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_ED3ET0_ED3ET0_MASK ) >> SPWTDP_ED3ET0_ED3ET0_SHIFT )
#define SPWTDP_ED3ET0_ED3ET0_SET( _reg, _val )     \
  ( ( ( _reg ) & ~SPWTDP_ED3ET0_ED3ET0_MASK ) |    \
    ( ( ( _val ) << SPWTDP_ED3ET0_ED3ET0_SHIFT ) & \
      SPWTDP_ED3ET0_ED3ET0_MASK ) )
#define SPWTDP_ED3ET0_ED3ET0( _val ) \
  ( ( ( _val ) << SPWTDP_ED3ET0_ED3ET0_SHIFT ) & SPWTDP_ED3ET0_ED3ET0_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPED3ET1 \
 *   External Datation 3 Elapsed Time 1 (ED3ET1)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_ED3ET1_ED3ET1_SHIFT 0
#define SPWTDP_ED3ET1_ED3ET1_MASK  0xffffffffU
#define SPWTDP_ED3ET1_ED3ET1_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_ED3ET1_ED3ET1_MASK ) >> SPWTDP_ED3ET1_ED3ET1_SHIFT )
#define SPWTDP_ED3ET1_ED3ET1_SET( _reg, _val )     \
  ( ( ( _reg ) & ~SPWTDP_ED3ET1_ED3ET1_MASK ) |    \
    ( ( ( _val ) << SPWTDP_ED3ET1_ED3ET1_SHIFT ) & \
      SPWTDP_ED3ET1_ED3ET1_MASK ) )
#define SPWTDP_ED3ET1_ED3ET1( _val ) \
  ( ( ( _val ) << SPWTDP_ED3ET1_ED3ET1_SHIFT ) & SPWTDP_ED3ET1_ED3ET1_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPED3ET2 \
 *   External Datation 3 Elapsed Time 2 (ED3ET2)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_ED3ET2_ED3ET2_SHIFT 0
#define SPWTDP_ED3ET2_ED3ET2_MASK  0xffffffffU
#define SPWTDP_ED3ET2_ED3ET2_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_ED3ET2_ED3ET2_MASK ) >> SPWTDP_ED3ET2_ED3ET2_SHIFT )
#define SPWTDP_ED3ET2_ED3ET2_SET( _reg, _val )     \
  ( ( ( _reg ) & ~SPWTDP_ED3ET2_ED3ET2_MASK ) |    \
    ( ( ( _val ) << SPWTDP_ED3ET2_ED3ET2_SHIFT ) & \
      SPWTDP_ED3ET2_ED3ET2_MASK ) )
#define SPWTDP_ED3ET2_ED3ET2( _val ) \
  ( ( ( _val ) << SPWTDP_ED3ET2_ED3ET2_SHIFT ) & SPWTDP_ED3ET2_ED3ET2_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPED3ET3 \
 *   External Datation 3 Elapsed Time 3 (ED3ET3)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_ED3ET3_ED3ET3_SHIFT 0
#define SPWTDP_ED3ET3_ED3ET3_MASK  0xffffffffU
#define SPWTDP_ED3ET3_ED3ET3_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_ED3ET3_ED3ET3_MASK ) >> SPWTDP_ED3ET3_ED3ET3_SHIFT )
#define SPWTDP_ED3ET3_ED3ET3_SET( _reg, _val )     \
  ( ( ( _reg ) & ~SPWTDP_ED3ET3_ED3ET3_MASK ) |    \
    ( ( ( _val ) << SPWTDP_ED3ET3_ED3ET3_SHIFT ) & \
      SPWTDP_ED3ET3_ED3ET3_MASK ) )
#define SPWTDP_ED3ET3_ED3ET3( _val ) \
  ( ( ( _val ) << SPWTDP_ED3ET3_ED3ET3_SHIFT ) & SPWTDP_ED3ET3_ED3ET3_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPED3ET4 \
 *   External Datation 3 Elapsed Time 4 (ED3ET4)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_ED3ET4_ED3ET4_SHIFT 24
#define SPWTDP_ED3ET4_ED3ET4_MASK  0xff000000U
#define SPWTDP_ED3ET4_ED3ET4_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_ED3ET4_ED3ET4_MASK ) >> SPWTDP_ED3ET4_ED3ET4_SHIFT )
#define SPWTDP_ED3ET4_ED3ET4_SET( _reg, _val )     \
  ( ( ( _reg ) & ~SPWTDP_ED3ET4_ED3ET4_MASK ) |    \
    ( ( ( _val ) << SPWTDP_ED3ET4_ED3ET4_SHIFT ) & \
      SPWTDP_ED3ET4_ED3ET4_MASK ) )
#define SPWTDP_ED3ET4_ED3ET4( _val ) \
  ( ( ( _val ) << SPWTDP_ED3ET4_ED3ET4_SHIFT ) & SPWTDP_ED3ET4_ED3ET4_MASK )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPPDR Pulse Definition Register 0 to 7 (PDR)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_PDR_PP_SHIFT 20
#define SPWTDP_PDR_PP_MASK  0xf00000U
#define SPWTDP_PDR_PP_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_PDR_PP_MASK ) >> SPWTDP_PDR_PP_SHIFT )
#define SPWTDP_PDR_PP_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_PDR_PP_MASK ) | \
    ( ( ( _val ) << SPWTDP_PDR_PP_SHIFT ) & SPWTDP_PDR_PP_MASK ) )
#define SPWTDP_PDR_PP( _val ) \
  ( ( ( _val ) << SPWTDP_PDR_PP_SHIFT ) & SPWTDP_PDR_PP_MASK )

#define SPWTDP_PDR_PW_SHIFT 16
#define SPWTDP_PDR_PW_MASK  0xf0000U
#define SPWTDP_PDR_PW_GET( _reg ) \
  ( ( ( _reg ) & SPWTDP_PDR_PW_MASK ) >> SPWTDP_PDR_PW_SHIFT )
#define SPWTDP_PDR_PW_SET( _reg, _val )  \
  ( ( ( _reg ) & ~SPWTDP_PDR_PW_MASK ) | \
    ( ( ( _val ) << SPWTDP_PDR_PW_SHIFT ) & SPWTDP_PDR_PW_MASK ) )
#define SPWTDP_PDR_PW( _val ) \
  ( ( ( _val ) << SPWTDP_PDR_PW_SHIFT ) & SPWTDP_PDR_PW_MASK )

#define SPWTDP_PDR_PL 0x400U

#define SPWTDP_PDR_PE 0x2U

/** @} */

/**
 * @brief This structure defines the SPWTDP register block memory map.
 */
typedef struct spwtdp {
  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPCONF0.
   */
  uint32_t conf0;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPCONF1.
   */
  uint32_t conf1;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPCONF2.
   */
  uint32_t conf2;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPCONF3.
   */
  uint32_t conf3;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPSTAT0.
   */
  uint32_t stat0;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPSTAT1.
   */
  uint32_t stat1;

  uint32_t reserved_18_20[ 2 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPCTRL.
   */
  uint32_t ctrl;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPCET0.
   */
  uint32_t cet0;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPCET1.
   */
  uint32_t cet1;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPCET2.
   */
  uint32_t cet2;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPCET3.
   */
  uint32_t cet3;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPCET4.
   */
  uint32_t cet4;

  uint32_t reserved_38_40[ 2 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPDPF.
   */
  uint32_t dpf;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPDET0.
   */
  uint32_t det0;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPDET1.
   */
  uint32_t det1;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPDET2.
   */
  uint32_t det2;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPDET3.
   */
  uint32_t det3;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPDET4.
   */
  uint32_t det4;

  uint32_t reserved_58_60[ 2 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPTRPFRX.
   */
  uint32_t trpfrx;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPTR0.
   */
  uint32_t tr0;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPTR1.
   */
  uint32_t tr1;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPTR2.
   */
  uint32_t tr2;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPTR3.
   */
  uint32_t tr3;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPTR4.
   */
  uint32_t tr4;

  uint32_t reserved_78_80[ 2 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPTTPFTX.
   */
  uint32_t ttpftx;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPTT0.
   */
  uint32_t tt0;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPTT1.
   */
  uint32_t tt1;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPTT2.
   */
  uint32_t tt2;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPTT3.
   */
  uint32_t tt3;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPTT4.
   */
  uint32_t tt4;

  uint32_t reserved_98_a0[ 2 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPLPF.
   */
  uint32_t lpf;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPLE0.
   */
  uint32_t le0;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPLE1.
   */
  uint32_t le1;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPLE2.
   */
  uint32_t le2;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPLE3.
   */
  uint32_t le3;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPLE4.
   */
  uint32_t le4;

  uint32_t reserved_b8_c0[ 2 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPIE.
   */
  uint32_t ie;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPIS.
   */
  uint32_t is;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPDC.
   */
  uint32_t dc;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPDS.
   */
  uint32_t ds;

  uint32_t reserved_d0_100[ 12 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPEDM0.
   */
  uint32_t edm0;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPEDM1.
   */
  uint32_t edm1;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPEDM2.
   */
  uint32_t edm2;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPEDM3.
   */
  uint32_t edm3;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPEDPF0.
   */
  uint32_t edpf0;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPED0ET0.
   */
  uint32_t ed0et0;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPED0ET1.
   */
  uint32_t ed0et1;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPED0ET2.
   */
  uint32_t ed0et2;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPED0ET3.
   */
  uint32_t ed0et3;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPED0ET4.
   */
  uint32_t ed0et4;

  uint32_t reserved_128_130[ 2 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPEDPF1.
   */
  uint32_t edpf1;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPED1ET0.
   */
  uint32_t ed1et0;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPED1ET1.
   */
  uint32_t ed1et1;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPED1ET2.
   */
  uint32_t ed1et2;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPED1ET3.
   */
  uint32_t ed1et3;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPED1ET4.
   */
  uint32_t ed1et4;

  uint32_t reserved_148_150[ 2 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPEDPF2.
   */
  uint32_t edpf2;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPED2ET0.
   */
  uint32_t ed2et0;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPED2ET1.
   */
  uint32_t ed2et1;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPED2ET2.
   */
  uint32_t ed2et2;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPED2ET3.
   */
  uint32_t ed2et3;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPED2ET4.
   */
  uint32_t ed2et4;

  uint32_t reserved_168_170[ 2 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPEDPF3.
   */
  uint32_t edpf3;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPED3ET0.
   */
  uint32_t ed3et0;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPED3ET1.
   */
  uint32_t ed3et1;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPED3ET2.
   */
  uint32_t ed3et2;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPED3ET3.
   */
  uint32_t ed3et3;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPED3ET4.
   */
  uint32_t ed3et4;

  uint32_t reserved_188_1a0[ 6 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPPDR.
   */
  uint32_t pdr[ 8 ];
} spwtdp;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_SPWTDP_REGS_H */
