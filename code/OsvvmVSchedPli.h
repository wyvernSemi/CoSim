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
//    10/2022   2023.01    Initial revision
//
//
//  This file is part of OSVVM.
//
//  Copyright (c) 2022 by [OSVVM Authors](../AUTHORS.md)
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

#define VINIT_PARAMS               int  node
#define VSCHED_PARAMS              int  node, int Interrupt, int VPDataIn, int* VPDataOut, int* VPAddr, int* VPOp,int* VPTicks
#define VTRANS_PARAMS              int  node,     int  Interrupt,                                                         \
                                   int  VPDataIn, int  VPDataInHi,  int* VPDataOut,   int* VPDataOutHi, int* VPDataWidth, \
                                   int* VPAddr,   int* VPAddrHi,    int* VPAddrWidth,                                     \
                                   int* VPOp,     int* VPBurstSize, int* VPTicks,      int* VPDone,     int* VPError
#define VPROCUSER_PARAMS           int  node, int value
#define VGETBURSTWRBYTE_PARAMS     int  node, int  idx,  uint8_t* data
#define VSETBURSTRDBYTE_PARAMS     int  node, int  idx,  uint8_t  data
#define VHALT_PARAMS               int, int

#define VPROC_RTN_TYPE     void
                                      
extern VPROC_RTN_TYPE VInit           (VINIT_PARAMS);
extern VPROC_RTN_TYPE VSched          (VSCHED_PARAMS);
extern VPROC_RTN_TYPE VTrans          (VTRANS_PARAMS);
extern VPROC_RTN_TYPE VProcUser       (VPROCUSER_PARAMS);
extern VPROC_RTN_TYPE VSetBurstRdByte (VSETBURSTRDBYTE_PARAMS);
extern VPROC_RTN_TYPE VGetBurstWrByte (VGETBURSTWRBYTE_PARAMS);
extern int            VHalt           (VHALT_PARAMS);

#endif
