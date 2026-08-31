// ------------------------------------------------------------------------------
//
//  File Name:           jfif_gtk.h
//  Design Unit Name:    Header for JFIF GTK API
//  Revision:            OSVVM MODELS STANDARD VERSION
//
//  Maintainer:          Simon Southwell      email:  simon.southwell@gmail.com
//  Contributor(s):
//     Simon Southwell   simon.southwell@gmail.com
//
//  Description:
//      Header for JFIF GTK API
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

#include <gtk/gtk.h>

#ifndef _GTK_H_
#define _GTK_H_

#ifdef __cplusplus
#define EXTDECL extern "C" void
#else
#define EXTDECL extern void
#endif

#define MAX_DISPLAY_WIDTH  1200
#define MAX_DISPLAY_HEIGHT  900

#define MIN_DISPLAY_WIDTH   200
#define MIN_DISPLAY_HEIGHT  200

// Default is for no border
#define BORDER_WIDTH          0

#ifndef JPEG_NO_GRAPHICS
EXTDECL jpeg_display_bmp_file (int argc, char *argv[], const unsigned char *ibuf, const int X, const int Y);
EXTDECL jpeg_display_img_data (int argc, char *argv[], const uint8_t       *data, const int X, const int Y);
#endif

#endif
