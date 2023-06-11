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
//      Co-simulation test transWriteAndRead/transWriteAndReadAsync
//      transaction source
//
//  Developed by:
//        Simon Southwell
//
//  Revision History:
//    Date      Version    Description
//    05/2023   2023.05    Initial revision
//
//  This file is part of OSVVM.
//
//  Copyright (c) 2023 by [OSVVM Authors](../AUTHORS.md)
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

// Import VProc user API
#include "OsvvmCosim.h"

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
    std::string test_name("CoSim_writeandread");
    OsvvmCosim  cosim(node, test_name);

    uint32_t addr  = 0x00010000;
    uint16_t data1 = 0x4e8f;
    uint16_t data2 = 0xe01c;
    uint16_t rdata;

    // Write an initial value to target location
    cosim.transWrite(addr, data1);

    // Use transWriteAndRead to update location and fetch old value
    cosim.transWriteAndRead(addr, data2, &rdata);

    // Check the returned value is correct
    if (rdata != data1)
    {
        VPrint("***ERROR: mismatch on read data from transWriteAndRead. Git 0x%04x. Exp 0x%04x\n", rdata, data1);
        error = true;
    }

    // Check the location was updated
    cosim.transReadCheck(addr, data2);
    
/*
    // New write value
    data1 = 0xb209;
    
    // Do asynchronous write-and-read
    cosim.transWriteAndReadAsync(addr, data1);
    
    // Wait for a read transaction
    cosim.transWaitForReadTransaction();
    
    // Check the returned read data
    cosim.transReadDataCheck(data2);
*/  

    // Flag to the simulation we're finished, after 10 more iterations
    cosim.tick(10, true, error);

    // If ever got this far then sleep forever
    SLEEPFOREVER;
}

