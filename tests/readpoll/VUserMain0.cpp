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
//      Co-simulation test transaction source
//
//  Developed by:
//        Simon Southwell
//
//  Revision History:
//    Date      Version    Description
//    09/2022   2022       Initial revision
//
//  This file is part of OSVVM.
//
//  Copyright (c) 2022 by Simon Southwell
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
#include <cmath>
#include <algorithm>
#include <vector>

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
    VPrint("VUserMain0(): node=%d\n", node);

    bool                  error = false;
    std::string test_name("CoSim_readpoll");
    OsvvmCosim  cosim(node, test_name);
    
    uint32_t addr   = 0x00000000;
    uint32_t data   = 0x00000001;
    int      bidx   = 14;
    uint32_t expval = (1 << bidx);

    cosim.transWrite(addr, data);
    
    cosim.transReadPoll(addr, &data, bidx, 1);
    
    if (data != expval)
    {
        VPrint("***ERROR: unexpected data value. Got 0x%08x. Exp 0x%08x\n", data, expval);
        error = true;
    }
    
    // Flag to the simulation we're finished, after 10 more iterations
    cosim.tick(10, true, error);

    // If ever got this far then sleep forever
    SLEEPFOREVER;
}

