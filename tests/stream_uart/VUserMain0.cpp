// ------------------------------------------------------------------------------
//
//  File Name:           VUserMain0.cpp
//  Design Unit Name:    Co-simulation virtual processor test program
//  Revision:            OSVVM MODELS STANDARD VERSION
//
//  Maintainer:          Simon Southwell      email:  simon.southwell@gmail.com
//  Contributor(s):
//     Simon Southwell   simon.southwell@gmail.com
//
//  Description:
//      Co-simulation test UART stream source
//
//  Developed by:
//        Simon Southwell
//
//  Revision History:
//    Date      Version    Description
//    03/2023   2023       Initial revision
//
//  This file is part of OSVVM.
//
//  Copyright (c) 2023 by Simon Southwell
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

#include <cstdio>
#include <cstdlib>
#include <cstdint>

// Import VProc user API for streams
#include "OsvvmCosimStream.h"

// I am node 0 context
static int node  = 0;

// ------------------------------------------------------------------------------
// Main entry point for node 0 virtual processor software
//
// VUserMainX has no calling arguments. If runtime configuration required
// then you'll need to read in a configuration file.
//
// ------------------------------------------------------------------------------

extern "C" void VUserMain0()
{
    VPrint("VUserMain%d()\n", node);

    bool                  error = false;
    std::string           test_name("CoSim_uart_streams");
    OsvvmCosimStream      uart(node, test_name);

    const int UARTTB_NO_ERROR     = 0;
    const int UARTTB_PARITY_ERROR = 1;
    const int UARTTB_STOP_ERROR   = 2;
    const int UARTTB_BREAK_ERROR  = 4;

    uint8_t  wdata[5] = {0x60, 0x61, 0x62, 0x63, 0x12};
    uint8_t  rdata;
    int      param    = 0;
    int      status;

    // Send out some data
    for (uint8_t idx = 0; idx < 5; idx++)
    {
        switch(idx)
        {
        case 1:  param = UARTTB_PARITY_ERROR;                     break;
        case 2:  param = UARTTB_STOP_ERROR;                       break;
        case 3:  param = UARTTB_PARITY_ERROR | UARTTB_STOP_ERROR; break;
        case 4:  param = UARTTB_BREAK_ERROR;                      break;
        default: param = UARTTB_NO_ERROR;                         break;
        }
        
        uart.streamSend(wdata[idx], param);
    }

    // Get the received data and check
    for (int idx = 0; idx < 5; idx++)
    {
        switch(idx)
        {
        case 1:  param = UARTTB_PARITY_ERROR;                     break;
        case 2:  param = UARTTB_STOP_ERROR;                       break;
        case 3:  param = UARTTB_PARITY_ERROR | UARTTB_STOP_ERROR; break;
        case 4:  param = UARTTB_BREAK_ERROR;                      break;
        default: param = UARTTB_NO_ERROR;                         break;
        }
        
        uart.streamGet(&rdata, &status);
        
        if (param != UARTTB_BREAK_ERROR)
        {
            if (rdata != (wdata[0] + idx) || param != status)
            {
                VPrint("VuserMain%d: ***Error mismatch on RX data. Got 0x%02x (%d), exp 0x%02x (%d)\n", 
                        node, rdata, status, wdata[0] + idx, param);
                error = true;
            }
            else
            {
                VPrint("VuserMain%d: received byte 0x%02x with status %x\n", node, rdata, status);
            }
        }
        else
        {
            if (param != (status & UARTTB_BREAK_ERROR))
            {
                VPrint("VuserMain%d: ***Error failed to detect break. %d, exp %d\n", 
                        node, status, param);
                error = true;
            }
            else
            {
                VPrint("VuserMain%d: received byte 0x%02x with status %x\n", node, rdata, status);
            }
        }
    }

    // Flag to the simulation we're finished, after 10 more iterations
    uart.tick(10, true, error);

    // If ever got this far then sleep forever
    SLEEPFOREVER;
}

