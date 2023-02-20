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

// I am node 1 context
static int node  = 0;

#ifdef _WIN32
#define srandom srand
#define random rand
#endif

typedef struct {
    uint32_t addr;
    uint32_t wdata;
    uint32_t size;
} wtrans_t;


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
    uint32_t              wdata = 0;
    std::string test_name("TbAb_InterruptCoSim4");
    OsvvmCosim  cosim(node, test_name);

    for (int loop = 0; loop < 4; loop++)
    {
        uint32_t addr  = 0x10000000;
        uint32_t rdata;

        for (int idx = 0; idx < 4; idx++)
        {
            cosim.transWrite(addr, wdata + idx);
            addr  += 4;
        }

        addr = 0xa0002000;

        for (int idx = 0; idx < 4; idx++)
        {
            cosim.transRead(addr, &rdata);
            if (rdata != (wdata + idx))
            {
                VPrint("VUserMain0: ***ERROR*** read %08X from address %08X. Expected %08x\n", rdata, addr, wdata + idx);
                error = true;
                break;
            }
            addr += 4;
        }
        
        wdata += 0x10;
    }

    // Flag to the simulation we're finished, after 10 more iterations
    cosim.tick(10, true, error);

    // If ever got this far then sleep forever
    SLEEPFOREVER;
}

