// ------------------------------------------------------------------------------
//
//  File Name:           jfif.h
//  Design Unit Name:    Header for JFIF API
//  Revision:            OSVVM MODELS STANDARD VERSION
//
//  Maintainer:          Simon Southwell      email:  simon.southwell@gmail.com
//  Contributor(s):
//     Simon Southwell   simon.southwell@gmail.com
//
//  Description:
//      Header for JFIF API
//
//  Developed by:
//        Simon Southwell
//
//  Revision History:
//    Date      Version    Description
//    06/2026   2026.08   Initial revision
//
//  This file is part of OSVVM.
//
//  Copyright (c) 2026 by Simon Southwell
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
// ------------------------------------------------------------------------------
//
//
// The code implements decoding of JFIF/JPEG data based on the
// following standards (links supplied)
//
// JPEG Standard (JPEG ISO/IEC 10918-1 ITU-T Recommendation T.81):
//     http://www.w3.org/Graphics/JPEG/itu-t81.pdf
//
// JPEG File Interchange Format version 1.02
//     http://www.w3.org/Graphics/JPEG/jfif3.pdf
//
// This file provides the definitions for external code to link
// to the main JFIF/JPEG decode routine jpeg_process_jfif().
//
// ------------------------------------------------------------------------------

#include <stdint.h>

#ifndef _JFIF_H_
#define _JFIF_H_

//-------------------------------------------------------------
// General definitions

#define JPEG_NO_ERROR                0
#define JPEG_FILE_ERROR              1
#define JPEG_USER_INPUT_ERROR        2
#define JPEG_FORMAT_ERROR            3
#define JPEG_MEMORY_ERROR            4
#define JPEG_UNSUPPORTED_ERROR       5

#ifndef __cplusplus
#define true                         (1==1)
#define false                        (1==0)
#define bool                         int
#endif

//-------------------------------------------------------------
// Exported function prototype(s) (see function main comments
// for detailed description)

// Takes a byte buffer (ibuf) containing a JFIF/JPEG image, and
// updates a pointer (obuf) to point to a 24 bit window bitmap.
// Return value is one of the six values defined above. If other
// than JPEG_NO_ERROR, the obuf pointer is undefined.

#ifdef __cplusplus
extern "C" int jpeg_process_jfif_c (uint8_t *ibuf, uint8_t **obuf, uint8_t **rawbuf, int debug_enable);
#else
extern     int jpeg_process_jfif_c (uint8_t *ibuf, uint8_t **obuf, uint8_t **rawbuf, int debug_enable);
#endif

#endif
