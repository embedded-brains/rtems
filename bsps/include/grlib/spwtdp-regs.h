/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSDeviceGRLIBSPWTDP
 *
 * @brief This header file defines the SPWTDP register block interface.
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

/* Generated from spec:/dev/grlib/if/spwtdp-header */

#ifndef _GRLIB_SPWTDP_REGS_H
#define _GRLIB_SPWTDP_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/dev/grlib/if/spwtdp */

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
#define SPWTDP_CONF0_MAPPING_MASK 0x1f00U
#define SPWTDP_CONF0_MAPPING_GET( _reg ) \
  ( ( ( _reg ) >> 8 ) & 0x1fU )
#define SPWTDP_CONF0_MAPPING( _val ) ( ( _val ) << 8 )

#define SPWTDP_CONF0_TD 0x80U

#define SPWTDP_CONF0_MU 0x40U

#define SPWTDP_CONF0_SEL_SHIFT 4
#define SPWTDP_CONF0_SEL_MASK 0x30U
#define SPWTDP_CONF0_SEL_GET( _reg ) \
  ( ( ( _reg ) >> 4 ) & 0x3U )
#define SPWTDP_CONF0_SEL( _val ) ( ( _val ) << 4 )

#define SPWTDP_CONF0_ME 0x8U

#define SPWTDP_CONF0_RE 0x4U

#define SPWTDP_CONF0_TE 0x2U

#define SPWTDP_CONF0_RS 0x1U

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPCONF3 Configuration 3 (CONF3)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_CONF3_STM_SHIFT 16
#define SPWTDP_CONF3_STM_MASK 0x3f0000U
#define SPWTDP_CONF3_STM_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0x3fU )
#define SPWTDP_CONF3_STM( _val ) ( ( _val ) << 16 )

#define SPWTDP_CONF3_DI64R 0x2000U

#define SPWTDP_CONF3_DI64T 0x1000U

#define SPWTDP_CONF3_DI64 0x800U

#define SPWTDP_CONF3_DI 0x400U

#define SPWTDP_CONF3_INRX_SHIFT 5
#define SPWTDP_CONF3_INRX_MASK 0x3e0U
#define SPWTDP_CONF3_INRX_GET( _reg ) \
  ( ( ( _reg ) >> 5 ) & 0x1fU )
#define SPWTDP_CONF3_INRX( _val ) ( ( _val ) << 5 )

#define SPWTDP_CONF3_INTX_SHIFT 0
#define SPWTDP_CONF3_INTX_MASK 0x1fU
#define SPWTDP_CONF3_INTX_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x1fU )
#define SPWTDP_CONF3_INTX( _val ) ( ( _val ) << 0 )

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
#define SPWTDP_CTRL_SPWTC_MASK 0xff0000U
#define SPWTDP_CTRL_SPWTC_GET( _reg ) \
  ( ( ( _reg ) >> 16 ) & 0xffU )
#define SPWTDP_CTRL_SPWTC( _val ) ( ( _val ) << 16 )

#define SPWTDP_CTRL_CPF_SHIFT 0
#define SPWTDP_CTRL_CPF_MASK 0xffffU
#define SPWTDP_CTRL_CPF_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffU )
#define SPWTDP_CTRL_CPF( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPCET0 Command Elapsed Time 0 (CET0)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_CET0_CET0_SHIFT 0
#define SPWTDP_CET0_CET0_MASK 0xffffffffU
#define SPWTDP_CET0_CET0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define SPWTDP_CET0_CET0( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPCET1 Command Elapsed Time 1 (CET1)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_CET1_CET1_SHIFT 0
#define SPWTDP_CET1_CET1_MASK 0xffffffffU
#define SPWTDP_CET1_CET1_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define SPWTDP_CET1_CET1( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPCET2 Command Elapsed Time 2 (CET2)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_CET2_CET2_SHIFT 0
#define SPWTDP_CET2_CET2_MASK 0xffffffffU
#define SPWTDP_CET2_CET2_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define SPWTDP_CET2_CET2( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPCET0 Command Elapsed Time 3 (CET0)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_CET0_CET3_SHIFT 0
#define SPWTDP_CET0_CET3_MASK 0xffffffffU
#define SPWTDP_CET0_CET3_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define SPWTDP_CET0_CET3( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPCET4 Command Elapsed Time 4 (CET4)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_CET4_CET4_SHIFT 24
#define SPWTDP_CET4_CET4_MASK 0xff000000U
#define SPWTDP_CET4_CET4_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xffU )
#define SPWTDP_CET4_CET4( _val ) ( ( _val ) << 24 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPDPF Datation Preamble Field (DPF)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_DPF_DPF_SHIFT 0
#define SPWTDP_DPF_DPF_MASK 0xffffU
#define SPWTDP_DPF_DPF_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffU )
#define SPWTDP_DPF_DPF( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPDET0 Datation Elapsed Time 0 (DET0)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_DET0_DET0_SHIFT 0
#define SPWTDP_DET0_DET0_MASK 0xffffffffU
#define SPWTDP_DET0_DET0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define SPWTDP_DET0_DET0( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPDET1 Datation Elapsed Time 1 (DET1)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_DET1_DET1_SHIFT 0
#define SPWTDP_DET1_DET1_MASK 0xffffffffU
#define SPWTDP_DET1_DET1_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define SPWTDP_DET1_DET1( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPDET2 Datation Elapsed Time 2 (DET2)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_DET2_DET2_SHIFT 0
#define SPWTDP_DET2_DET2_MASK 0xffffffffU
#define SPWTDP_DET2_DET2_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define SPWTDP_DET2_DET2( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPDET3 Datation Elapsed Time 3 (DET3)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_DET3_DET3_SHIFT 0
#define SPWTDP_DET3_DET3_MASK 0xffffffffU
#define SPWTDP_DET3_DET3_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define SPWTDP_DET3_DET3( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPDET4 Datation Elapsed Time 4 (DET4)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_DET4_DET4_SHIFT 24
#define SPWTDP_DET4_DET4_MASK 0xff000000U
#define SPWTDP_DET4_DET4_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xffU )
#define SPWTDP_DET4_DET4( _val ) ( ( _val ) << 24 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPTRPFRX Time-Stamp Preamble Field Rx (TRPFRX)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_TRPFRX_TRPF_SHIFT 0
#define SPWTDP_TRPFRX_TRPF_MASK 0xffffU
#define SPWTDP_TRPFRX_TRPF_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffU )
#define SPWTDP_TRPFRX_TRPF( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPTR0 Time Stamp Elapsed Time 0 Rx (TR0)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_TR0_TR0_SHIFT 0
#define SPWTDP_TR0_TR0_MASK 0xffffffffU
#define SPWTDP_TR0_TR0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define SPWTDP_TR0_TR0( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPTR1 Time Stamp Elapsed Time 1 Rx (TR1)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_TR1_TR1_SHIFT 0
#define SPWTDP_TR1_TR1_MASK 0xffffffffU
#define SPWTDP_TR1_TR1_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define SPWTDP_TR1_TR1( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPTR2 Time Stamp Elapsed Time 2 Rx (TR2)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_TR2_TR2_SHIFT 0
#define SPWTDP_TR2_TR2_MASK 0xffffffffU
#define SPWTDP_TR2_TR2_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define SPWTDP_TR2_TR2( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPTR3 Time Stamp Elapsed Time 3 Rx (TR3)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_TR3_TR3_SHIFT 0
#define SPWTDP_TR3_TR3_MASK 0xffffffffU
#define SPWTDP_TR3_TR3_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define SPWTDP_TR3_TR3( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPTR4 Time Stamp Elapsed Time 4 Rx (TR4)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_TR4_TR4_SHIFT 24
#define SPWTDP_TR4_TR4_MASK 0xff000000U
#define SPWTDP_TR4_TR4_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xffU )
#define SPWTDP_TR4_TR4( _val ) ( ( _val ) << 24 )

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
#define SPWTDP_TTPFTX_TSTC_MASK 0xff000000U
#define SPWTDP_TTPFTX_TSTC_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xffU )
#define SPWTDP_TTPFTX_TSTC( _val ) ( ( _val ) << 24 )

#define SPWTDP_TTPFTX_TTPF_SHIFT 0
#define SPWTDP_TTPFTX_TTPF_MASK 0xffffU
#define SPWTDP_TTPFTX_TTPF_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffU )
#define SPWTDP_TTPFTX_TTPF( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPTT0 Time Stamp Elapsed Time 0 Tx (TT0)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_TT0_TT0_SHIFT 0
#define SPWTDP_TT0_TT0_MASK 0xffffffffU
#define SPWTDP_TT0_TT0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define SPWTDP_TT0_TT0( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPTT1 Time Stamp Elapsed Time 1 Tx (TT1)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_TT1_TT1_SHIFT 0
#define SPWTDP_TT1_TT1_MASK 0xffffffffU
#define SPWTDP_TT1_TT1_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define SPWTDP_TT1_TT1( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPTT2 Time Stamp Elapsed Time 2 Tx (TT2)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_TT2_TT2_SHIFT 0
#define SPWTDP_TT2_TT2_MASK 0xffffffffU
#define SPWTDP_TT2_TT2_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define SPWTDP_TT2_TT2( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPTT3 Time Stamp Elapsed Time 3 Tx (TT3)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_TT3_TT3_SHIFT 0
#define SPWTDP_TT3_TT3_MASK 0xffffffffU
#define SPWTDP_TT3_TT3_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define SPWTDP_TT3_TT3( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPTT4 Time Stamp Elapsed Time 4 Tx (TT4)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_TT4_TT4_SHIFT 24
#define SPWTDP_TT4_TT4_MASK 0xff000000U
#define SPWTDP_TT4_TT4_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xffU )
#define SPWTDP_TT4_TT4( _val ) ( ( _val ) << 24 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPLPF Latency Preamble Field (LPF)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_LPF_LPF_SHIFT 0
#define SPWTDP_LPF_LPF_MASK 0xffffU
#define SPWTDP_LPF_LPF_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffU )
#define SPWTDP_LPF_LPF( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPIE Interrupt Enable (IE)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_IE_NCTCE 0x80000U

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
 * @defgroup RTEMSDeviceGRLIBSPWTDPDC Delay Count (DC)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_DC_DC_SHIFT 0
#define SPWTDP_DC_DC_MASK 0x7fffU
#define SPWTDP_DC_DC_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0x7fffU )
#define SPWTDP_DC_DC( _val ) ( ( _val ) << 0 )

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
#define SPWTDP_DS_CD_MASK 0xffffffU
#define SPWTDP_DS_CD_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffU )
#define SPWTDP_DS_CD( _val ) ( ( _val ) << 0 )

/** @} */

/**
 * @defgroup RTEMSDeviceGRLIBSPWTDPEDM0 External Datation 0 Mask (EDM0)
 *
 * @brief This group contains register bit definitions.
 *
 * @{
 */

#define SPWTDP_EDM0_EDM0_SHIFT 0
#define SPWTDP_EDM0_EDM0_MASK 0xffffffffU
#define SPWTDP_EDM0_EDM0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define SPWTDP_EDM0_EDM0( _val ) ( ( _val ) << 0 )

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
#define SPWTDP_EDPF0_EDPF0_MASK 0xffffU
#define SPWTDP_EDPF0_EDPF0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffU )
#define SPWTDP_EDPF0_EDPF0( _val ) ( ( _val ) << 0 )

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
#define SPWTDP_ED0ET0_ED0ET0_MASK 0xffffffffU
#define SPWTDP_ED0ET0_ED0ET0_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define SPWTDP_ED0ET0_ED0ET0( _val ) ( ( _val ) << 0 )

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
#define SPWTDP_ED0ET1_ED0ET1_MASK 0xffffffffU
#define SPWTDP_ED0ET1_ED0ET1_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define SPWTDP_ED0ET1_ED0ET1( _val ) ( ( _val ) << 0 )

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
#define SPWTDP_ED0ET2_ED0ET2_MASK 0xffffffffU
#define SPWTDP_ED0ET2_ED0ET2_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define SPWTDP_ED0ET2_ED0ET2( _val ) ( ( _val ) << 0 )

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
#define SPWTDP_ED0ET3_ED0ET3_MASK 0xffffffffU
#define SPWTDP_ED0ET3_ED0ET3_GET( _reg ) \
  ( ( ( _reg ) >> 0 ) & 0xffffffffU )
#define SPWTDP_ED0ET3_ED0ET3( _val ) ( ( _val ) << 0 )

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
#define SPWTDP_ED0ET4_ED0ET4_MASK 0xff000000U
#define SPWTDP_ED0ET4_ED0ET4_GET( _reg ) \
  ( ( ( _reg ) >> 24 ) & 0xffU )
#define SPWTDP_ED0ET4_ED0ET4( _val ) ( ( _val ) << 24 )

/** @} */

/**
 * @brief This structure defines the SPWTDP register block memory map.
 */
typedef struct spwtdp {
  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPCONF0.
   */
  uint32_t conf0;

  uint32_t reserved_4_c[ 2 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPCONF3.
   */
  uint32_t conf3;

  uint32_t reserved_10_20[ 4 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPCTRL.
   */
  uint32_t ctrl;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPCET0.
   */
  uint32_t cet0_0;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPCET1.
   */
  uint32_t cet1;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPCET2.
   */
  uint32_t cet2;

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPCET0.
   */
  uint32_t cet0_1;

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

  uint32_t reserved_a4_c0[ 7 ];

  /**
   * @brief See @ref RTEMSDeviceGRLIBSPWTDPIE.
   */
  uint32_t ie;

  uint32_t reserved_c4_c8;

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

  uint32_t reserved_104_110[ 3 ];

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
} spwtdp;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _GRLIB_SPWTDP_REGS_H */
