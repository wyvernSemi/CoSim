// ------------------------------------------------------------------------------
//
//  File Name:           VUserMain0.cpp
//  Design Unit Name:    Co-simulation UART VC test program
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
//    03/2023   2023.04    Initial revision
//
//  This file is part of OSVVM.
//
//  Copyright (c) 2023 by [OSVVM Authors](AUTHORS.md) 
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

// Import OSVVM user API for streams
#include "OsvvmCosimStream.h"

// I am node 0 context
static int node  = 0;

// Define UART paramater values
// (would like to use enum, but need to OR values)
static const int UARTTB_NO_ERROR     = 0;
static const int UARTTB_PARITY_ERROR = 1;
static const int UARTTB_STOP_ERROR   = 2;
static const int UARTTB_BREAK_ERROR  = 4;

// ------------------------------------------------------------------------------
// Select UART VC paramater based on index
// ------------------------------------------------------------------------------

static int GenParam(const int idx)
{
    int param;
    
    switch(idx)
    {
    case 1:  param = UARTTB_PARITY_ERROR;                     break;
    case 2:  param = UARTTB_STOP_ERROR;                       break;
    case 3:  param = UARTTB_PARITY_ERROR | UARTTB_STOP_ERROR; break;
    case 4:  param = UARTTB_BREAK_ERROR;                      break;
    default: param = UARTTB_NO_ERROR;                         break;
    }
    
    return param;
}

// ------------------------------------------------------------------------------
// Check received data
// ------------------------------------------------------------------------------

static bool CheckResult(const uint8_t rdata, const uint8_t wdata[], const int status, const int param, const uint8_t idx)
{
    bool error = false;
    
    if (param != UARTTB_BREAK_ERROR)
    {
        if (rdata != (wdata[0] + idx) || param != status)
        {
            VPrint("CheckResult (node %d): ***Error mismatch on RX data. Got 0x%02x (%d), exp 0x%02x (%d)\n", 
                    node, rdata, status, wdata[0] + idx, param);
            error = true;
        }
        else
        {
            VPrint("CheckResult%d: received byte 0x%02x with status %x\n", node, rdata, status);
        }
    }
    else
    {
        if (param != (status & UARTTB_BREAK_ERROR))
        {
            VPrint("CheckResult (node %d): ***Error failed to detect break. %d, exp %d\n", 
                    node, status, param);
            error = true;
        }
        else
        {
            VPrint("CheckResult (node %d): received byte 0x%02x with status %x\n", node, rdata, status);
        }
    }
    
    return error;
}

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
    
    const int             DATASIZE = 5;
    const int             NUMTESTS = 6;

    bool                  error = false;
    std::string           test_name("CoSim_uart_streams");
    OsvvmCosimStream      uart(node, test_name);

    uint8_t               wdata[DATASIZE] = {0x10, 0x11, 0x12, 0x13, 0x14};
    uint8_t               rdata;
    int                   param    = 0;
    int                   status;

    //  Run test for a number of iterations
    for (int testnum = 0; testnum < NUMTESTS; testnum++)
    {
        // Send out some data
        for (uint8_t idx = 0; idx < DATASIZE; idx++)
        {
            param = GenParam(idx);
            
            uart.streamSend(wdata[idx], param);
        }
        
        // Get the received data and check
        for (int idx = 0; idx < DATASIZE; idx++)
        {
            param = GenParam(idx);
            
            uart.streamGet(&rdata, &status);
            
            error = CheckResult(rdata, wdata, status, param, idx);
        }
        
        // Change the write data pattern for next iteration.
        for (int idx = 0; idx < DATASIZE; idx++)
        {
            wdata[idx] += 0x10;
        }
    }

    // Flag to the simulation we're finished, after 10 more ticks
    uart.tick(10, true, error);

    // If ever got this far then sleep forever
    SLEEPFOREVER;
}

