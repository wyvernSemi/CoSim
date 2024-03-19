// =========================================================================
//
//  File Name:         OsvvmPython.h
//  Design Unit Name:
//  Revision:          OSVVM MODELS STANDARD VERSION
//
//  Maintainer:        Simon Southwell email:  simon.southwell@gmail.com
//  Contributor(s):
//     Simon Southwell      simon.southwell@gmail.com
//
//
//  Description:
//      Python OSVVM co-simulation C API function definitions
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
#include <Python.h>
#include <dlfcn.h>
#include "OsvvmVUser.h"

#ifndef _OSVVMPYTHON_H_
#define _OSVVMPYTHON_H_

#define DEFAULTSTRBUFSIZE      256

extern "C"
{

int      RunPython           (const int      node);
uint32_t PyTick              (const uint32_t ticks, const bool done, const bool error, const uint32_t node);
uint32_t PyTransWrite        (const uint32_t addr,  const uint32_t data, const uint32_t node);
uint32_t PyTransRead         (const uint32_t addr,  const uint32_t node);
void     PyTransBurstWrite   (const uint32_t addr,  void *data, const uint32_t bytesize, const uint32_t node);
void     PyTransBurstRead    (const uint32_t addr,  void *data, const uint32_t bytesize, const uint32_t node);
uint32_t PyRegIrq            (const pVUserInt_t func, const uint32_t node);
void     PySetTestName       (const uint8_t *data, const uint32_t bytesize, const uint32_t node);
}

#endif
