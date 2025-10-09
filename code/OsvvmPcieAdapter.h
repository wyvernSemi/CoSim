// =========================================================================
//
//  File Name:         OsvvmPcieAdpater.h
//  Design Unit Name:
//  Revision:          OSVVM MODELS STANDARD VERSION
//
//  Maintainer:        Simon Southwell email:  simon.southwell@gmail.com
//  Contributor(s):
//     Simon Southwell      simon.southwell@gmail.com
//
//
//  Description:
//      Header for aapter for PCIe model code from VProc API to OSVVM calls
//
//  Revision History:
//    Date      Version    Description
//    10/2025   ????.??    Initial revision
//
//
//  This file is part of OSVVM.
//
//  Copyright (c) 2025 by [OSVVM Authors](../../AUTHORS.md)
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

uint64_t VWrite64 (uint64_t addr, uint64_t  data, int delta, unsigned int node);
uint64_t VRead64  (uint64_t addr, uint64_t *data, int delta, unsigned int node);