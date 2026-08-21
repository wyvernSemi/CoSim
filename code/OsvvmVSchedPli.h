// =========================================================================
//
//  File Name:         OsvvmVSchedPli.h
//  Design Unit Name:
//  Revision:          OSVVM MODELS STANDARD VERSION
//
//  Maintainer:        Simon Southwell email:  simon.southwell@gmail.com
//  Contributor(s):
//     Simon Southwell      simon.southwell@gmail.com
//
//
//  Description:
//      Simulator co-simulation virtual procedure C interface routine
//      export definitions for simulator side code
//
//  Revision History:
//    Date      Version    Description
//    07/2025   2026.01    Adding VIrqVec CoSim procedure
//    05/2023   2023.05    Refactored VTrans arguments
//    10/2022   2023.01    Initial revision
//
//
//  This file is part of OSVVM.
//
//  Copyright (c) 2022 - 2025 by [OSVVM Authors](../AUTHORS.md)
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
// =========================================================================

#include <string.h>
#include <stdint.h>

#ifndef _OSVVM_VSCHED_PLI_H_
#define _OSVVM_VSCHED_PLI_H_

#ifdef __cplusplus
#define LINKAGE "C"
#else
#define LINKAGE
#endif

#if defined(ALDEC) || defined (ACTIVEHDL)
#define USE_VHPI
#endif

#if !defined(USE_VHPI)

# ifdef VHDLINTEGER64
# define vint_t int64_t
# else
# define vint_t int32_t
# endif

#define VINIT_PARAMS               vint_t  node
#define VIRQVEC_PARAMS             vint_t  node,     vint_t  irq
#define VUSERVALUE_PARAMS          vint_t  node,     vint_t  type,        vint_t  value
#define VTRANS_PARAMS              vint_t  node,     vint_t  Interrupt,   vint_t  VPStatus,    vint_t  VPCount,     vint_t  VPCountSec,  \
                                   vint_t* VPData,   vint_t* VPDataHi,    vint_t* VPDataWidth,                                     \
                                   vint_t* VPAddr,   vint_t* VPAddrHi,    vint_t* VPAddrWidth,                                     \
                                   vint_t* VPOp,     vint_t* VPBurstSize, vint_t* VPTicks,     vint_t* VPDone,      vint_t* VPError,     \
                                   vint_t* VPParam
#define VGETBURSTWRBYTE_PARAMS     vint_t  node,     vint_t  idx,         vint_t* data
#define VSETBURSTRDBYTE_PARAMS     vint_t  node,     vint_t  idx,         vint_t  data

#define VPROC_RTN_TYPE             void

#else

#include <vhpi_user.h>

#if defined(ALDEC)
#include <aldecpli.h>
#endif

#define VINIT_PARAMS                        const vhpiCbDataT* cb
#define VIRQVEC_PARAMS                      const vhpiCbDataT* cb
#define VUSERVALUE_PARAMS                   const vhpiCbDataT* cb
#define VTRANS_PARAMS                       const vhpiCbDataT* cb
#define VGETBURSTWRBYTE_PARAMS              const vhpiCbDataT* cb
#define VSETBURSTRDBYTE_PARAMS              const vhpiCbDataT* cb

#define VINIT_NUM_ARGS                      1
#define VIRQVEC_NUM_ARGS                    2
#define VUSERVALUE_NUM_ARGS                 3
#define VTRANS_NUM_ARGS                     17
#define VGETBURSTWRBYTE_NUM_ARGS            3
#define VSETBURSTRDBYTE_NUM_ARGS            3

#define VTRANS_START_OF_OUTPUTS             5
#define VGETBURSTWRBYTE_START_OF_OUTPUTS    2

# if defined(ALDEC)
# define VPROC_RTN_TYPE                      PLI_VOID
# else
# define VPROC_RTN_TYPE                      void
# endif

# if defined(NVC)
# define vhpiForeignT                        vhpiForeignKindT
# endif

#endif

extern LINKAGE VPROC_RTN_TYPE VInit           (VINIT_PARAMS);
extern LINKAGE VPROC_RTN_TYPE VIrqVec         (VIRQVEC_PARAMS);
extern LINKAGE VPROC_RTN_TYPE VUserValue      (VUSERVALUE_PARAMS);
extern LINKAGE VPROC_RTN_TYPE VTrans          (VTRANS_PARAMS);
extern LINKAGE VPROC_RTN_TYPE VSetBurstRdByte (VSETBURSTRDBYTE_PARAMS);
extern LINKAGE VPROC_RTN_TYPE VGetBurstWrByte (VGETBURSTWRBYTE_PARAMS);

#endif
