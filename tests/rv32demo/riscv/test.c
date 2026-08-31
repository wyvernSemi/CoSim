//
//  File Name:           test.c
//  Design Unit Name:
//  Revision:            OSVVM MODELS STANDARD VERSION
//
//  Maintainer:          Simon Southwell  email: simon.southwell@gmail.com
//  Contributor(s):
//     Simon Southwell  simon.southwell@gmail.com
//
//
//  Description:
//      RISC-V code using data LZW compression
//
//
//  Developed by:
//        SynthWorks Design Inc.
//        VHDL Training Classes
//        http://www.SynthWorks.com
//
//  Revision History:
//    Date      Version    Description
//    07/2026   2026.08    Initial revision
//
//
//  This file is part of OSVVM.
//
//  Copyright (c) 2026 by [OSVVM Authors](../../../AUTHORS.md)
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

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "slzw.h"
#include "message.h"

#define UART_BASE_ADDR 0x80000000
#define BUFSIZE        2048

static char obuf [BUFSIZE];

int main (int argc, char** argv)
{
    uint32_t* uartptr = (uint32_t*)UART_BASE_ADDR;

    // Compress in-built message data and place in obuf, returning
    // length of compressed data
    int comp_size = compress(message, obuf, strlen(message));

    // Send compressed data to UART
    for (int idx = 0; idx < comp_size; idx++)
    {
        *uartptr = obuf[idx];
    }

    return 0;
}
