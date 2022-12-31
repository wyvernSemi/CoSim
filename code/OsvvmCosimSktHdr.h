// =========================================================================
//
//  File Name:         osvvm_cosim_skt.h
//  Design Unit Name:  osvvm_cosim_skt
//  Revision:          OSVVM MODELS STANDARD VERSION
//
//  Maintainer:        Simon Southwell email:  simon.southwell@gmail.com
//  Contributor(s):
//     Simon Southwell      simon.southwell@gmail.com
//
//
//  Description:
//      Defines osvvm_cosim_skt class internal definitions to support
//       compilation on windows and linux
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

#ifndef _OSVVM_COSIM_SKT_HDR_H_
#define _OSVVM_COSIM_SKT_HDR_H_

// -------------------------------------------------------------------------
// INCLUDES
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
// DEFINES
// -------------------------------------------------------------------------

#if defined (_WIN32) || defined (_WIN64)

// -------------------------------------------------------------------------
// DEFINITIONS (windows)
// -------------------------------------------------------------------------

// Define the max supported winsocket spec. major and minor numbers
# define VER_MAJOR           2
# define VER_MINOR           2

#else

// -------------------------------------------------------------------------
// DEFINITIONS (Linux)
// -------------------------------------------------------------------------

// Map some windows function names to the file access Linux equivalents
# define closesocket         close
# define ZeroMemory          bzero

#endif

// -------------------------------------------------------------------------
// MACRO DEFINITIONS
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
// TYPE DEFINITIONS
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
// ENUMERATIONS
// -------------------------------------------------------------------------

#endif
