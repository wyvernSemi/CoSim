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
//       co-simulation communications via TCP/IP socket
//
//  Developed by:
//        SynthWorks Design Inc.
//        VHDL Training Classes
//        http://www.SynthWorks.com
//
//  Revision History:
//    Date      Version    Description
//    10/2022   2022       Initial revision
//
//
//  This file is part of OSVVM.
//
//  Copyright (c) 2022 by SynthWorks Design Inc.
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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

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

#define GDB_ACK_CHAR         '+'
#define GDB_NAK_CHAR         '-'
#define GDB_SOP_CHAR         '$'
#define GDB_EOP_CHAR         '#'
#define GDB_MEM_DELIM_CHAR   ':'
#define GDB_BIN_ESC          0x7d
#define GDB_BIN_XOR_VAL      0x20
#define HEX_CHAR_MAP         "0123456789abcdef"
#define MAXBACKLOG           5

// -------------------------------------------------------------------------
// MACRO DEFINITIONS
// -------------------------------------------------------------------------

#define HIHEXCHAR(_x) hexchars[((_x) & 0xf0) >> 4]
#define LOHEXCHAR(_x) hexchars[ (_x) & 0x0f]

#define CHAR2NIB(_x) (((_x) >= '0' && (_x) <= '9') ? (_x) - '0': \
                      ((_x) >= 'a' && (_x) <= 'f') ? (10 + (_x) - 'a'): \
                                                     (10 + (_x) - 'A'))

#define BUFBYTE(_b,_i,_v) {     \
    _b[_i]     = HIHEXCHAR(_v); \
    _b[(_i)+1] = LOHEXCHAR(_v); \
    _i        += 2;             \
}
#define BUFWORD(_b,_i,_v) {     \
    BUFBYTE(_b,_i,(_v) >> 24);  \
    BUFBYTE(_b,_i,(_v) >> 16);  \
    BUFBYTE(_b,_i,(_v) >>  8);  \
    BUFBYTE(_b,_i,(_v) >>  0);  \
}

#define BUFWORDLE(_b,_i,_v) {   \
    BUFBYTE(_b,_i,(_v) >>  0);  \
    BUFBYTE(_b,_i,(_v) >>  8);  \
    BUFBYTE(_b,_i,(_v) >> 16);  \
    BUFBYTE(_b,_i,(_v) >> 24);  \
}

#define BUFOK(_b,_i,_c) {_c += _b[_i] = 'O'; _c += _b[_i+1] = 'K'; _i+=2;}

#define BUFERR(_e,_b,_i,_c) {_c += _b[_i] = 'E'; _c += _b[_i+1] = HIHEXCHAR(_e); _c += _b[_i+2] = LOHEXCHAR(_e); _i+=3;}

// -------------------------------------------------------------------------
// TYPE DEFINITIONS
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
// ENUMERATIONS
// -------------------------------------------------------------------------

#endif
