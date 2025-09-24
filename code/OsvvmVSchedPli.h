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
//    07/2025   ????.??    Adding VIrqVec CoSim procedure
//    05/2023   2023.05    Refactored VTrans arguments
//    10/2022   2023.01    Initial revision
//
//
//  This file is part of OSVVM.
//
//  Copyright (c) 2022-2025 by [OSVVM Authors](../AUTHORS.md)
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

#ifndef _OSVVM_VSCHED_PLI_H_
#define _OSVVM_VSCHED_PLI_H_

#ifdef __cplusplus
#define LINKAGE "C"
#else
#define LINKAGE
#endif

#ifndef ALDEC

#define VINIT_PARAMS               int  node
#define VIRQVEC_PARAMS             int  node,     int  irq
#define VTRANS_PARAMS              int  node,     int  Interrupt,   int  VPStatus,    int  VPCount,     int  VPCountSec,  \
                                   int* VPData,   int* VPDataHi,    int* VPDataWidth,                                     \
                                   int* VPAddr,   int* VPAddrHi,    int* VPAddrWidth,                                     \
                                   int* VPOp,     int* VPBurstSize, int* VPTicks,     int* VPDone,      int* VPError,     \
                                   int* VPParam
#define VGETBURSTWRBYTE_PARAMS     int  node,     int  idx,         int* data
#define VSETBURSTRDBYTE_PARAMS     int  node,     int  idx,         int  data

#define VPROC_RTN_TYPE             void

#else

#include <vhpi_user.h>
#include <aldecpli.h>

#define VINIT_PARAMS                        const struct vhpiCbDataS* cb
#define VIRQVEC_PARAMS                      const struct vhpiCbDataS* cb
#define VTRANS_PARAMS                       const struct vhpiCbDataS* cb
#define VGETBURSTWRBYTE_PARAMS              const struct vhpiCbDataS* cb
#define VSETBURSTRDBYTE_PARAMS              const struct vhpiCbDataS* cb

#define VINIT_NUM_ARGS                      1
#define VIRQVEC_NUM_ARGS                    2
#define VTRANS_NUM_ARGS                     17
#define VGETBURSTWRBYTE_NUM_ARGS            3
#define VSETBURSTRDBYTE_NUM_ARGS            3
                                            
#define VTRANS_START_OF_OUTPUTS             5
#define VGETBURSTWRBYTE_START_OF_OUTPUTS    2

#define VPROC_RTN_TYPE                      PLI_VOID

#endif

extern LINKAGE VPROC_RTN_TYPE VInit           (VINIT_PARAMS);
extern LINKAGE VPROC_RTN_TYPE VIrqVec         (VIRQVEC_PARAMS);
extern LINKAGE VPROC_RTN_TYPE VTrans          (VTRANS_PARAMS);
extern LINKAGE VPROC_RTN_TYPE VSetBurstRdByte (VSETBURSTRDBYTE_PARAMS);
extern LINKAGE VPROC_RTN_TYPE VGetBurstWrByte (VGETBURSTWRBYTE_PARAMS);

#endif
