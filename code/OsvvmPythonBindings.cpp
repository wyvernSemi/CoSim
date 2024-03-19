// =========================================================================
//
//  File Name:         OsvvmPythonBindings.h
//  Design Unit Name:
//  Revision:          OSVVM MODELS STANDARD VERSION
//
//  Maintainer:        Simon Southwell email:  simon.southwell@gmail.com
//  Contributor(s):
//     Simon Southwell      simon.southwell@gmail.com
//
//
//  Description:
//      Python API binding function definitions.
//
//  Revision History:
//    Date      Version    Description
//    03/2024   2024.??    Initial revision
//
//
//  This file is part of OSVVM.
//
//  Copyright (c) 2024 by [OSVVM Authors](../../AUTHORS.md)
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

#include <stdint.h>
#include "OsvvmVUser.h"

// These must have C linkage, so no overloading.

extern "C"
{
uint8_t  OsvvmPyTransCommon_8_32        (const int op, uint32_t *addr, const uint8_t   data, int* status, const int prot = 0, const uint32_t node = 0) {return VTransUserCommon(op, addr, data, status, prot, node);};
uint16_t OsvvmPyTransCommon_16_32       (const int op, uint32_t *addr, const uint16_t  data, int* status, const int prot = 0, const uint32_t node = 0) {return VTransUserCommon(op, addr, data, status, prot, node);};
uint32_t OsvvmPyTransCommon_32_32       (const int op, uint32_t *addr, const uint32_t  data, int* status, const int prot = 0, const uint32_t node = 0) {return VTransUserCommon(op, addr, data, status, prot, node);};
uint8_t  OsvvmPyTransCommon_8_64        (const int op, uint64_t *addr, const uint8_t   data, int* status, const int prot = 0, const uint32_t node = 0) {return VTransUserCommon(op, addr, data, status, prot, node);};
uint16_t OsvvmPyTransCommon_16_64       (const int op, uint64_t *addr, const uint16_t  data, int* status, const int prot = 0, const uint32_t node = 0) {return VTransUserCommon(op, addr, data, status, prot, node);};
uint32_t OsvvmPyTransCommon_32_64       (const int op, uint64_t *addr, const uint32_t  data, int* status, const int prot = 0, const uint32_t node = 0) {return VTransUserCommon(op, addr, data, status, prot, node);};
uint64_t OsvvmPyTransCommon_64_64       (const int op, uint64_t *addr, const uint64_t  data, int* status, const int prot = 0, const uint32_t node = 0) {return VTransUserCommon(op, addr, data, status, prot, node);};
void     OsvvmPyTransBurstCommon_32     (const int op, const int param, const uint32_t addr, uint8_t* data, const int bytesize, const int prot = 0, const uint32_t node = 0) {VTransBurstCommon(op, param, addr, data, bytesize, prot, node);};
void     OsvvmPyTransBurstCommon_64     (const int op, const int param, const uint64_t addr, uint8_t* data, const int bytesize, const int prot = 0, const uint32_t node = 0) {VTransBurstCommon(op, param, addr, data, bytesize, prot, node);};

int      OsvvmPyTransGetCount           (const int op, const uint32_t node = 0){return VTransGetCount(op, node);};
void     OsvvmPyTransTransactionWait    (const int op, const uint32_t node = 0){VTransTransactionWait(op, node);};

int      OsvvmPyTick                    (const uint32_t ticks, const bool done = false, const bool error = false, const uint32_t  node = 0){return VTick(ticks, done, error, node);};
void     OsvvmPyRegIrq                  (const pVUserInt_t func, const uint32_t node){VRegInterrupt(func, node);};
void     OsvvmSetTestName               (const uint8_t *testname, const int bytesize, const uint32_t node){VSetTestName((char*)testname, bytesize, node);};
}
