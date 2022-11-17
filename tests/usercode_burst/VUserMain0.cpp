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
#include "OsvvmVUser.h"

// I am node 0 context
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
// ------------------------------------------------------------------------------

static void logGdbMsg(FILE *fp, wtrans_t &w, bool rnw)
{
    char msg[256];

    if (rnw)
    {
        sprintf(msg, "m%x,%d\n", w.addr, w.size/8);
    }
    else
    {
        int byteSize = w.size/8;
        switch(w.size)
        {
        case 32: sprintf(msg, "M%x,%d:%08x\n", w.addr, byteSize, w.wdata); break;
        case 16: sprintf(msg, "M%x,%d:%04x\n", w.addr, byteSize, w.wdata & 0xffff); break;
        case  8: sprintf(msg, "M%x,%d:%02x\n", w.addr, byteSize, w.wdata & 0xff); break;
        }
    }

    fprintf(fp, "%s", msg);
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
    VPrint("VUserMain0(): node=%d\n", node);

    std::vector<wtrans_t> vec;
    wtrans_t              wtrans;

    // Use node number, inverted, as the random number generator seed.
    srandom(~node);

    uint8_t wbuf[4096];
    uint8_t rbuf[4096];
    
    for (int idx = 0; idx < 16; idx++)
    {
        wbuf[idx] = idx; //random() & 0xff;
    }
    
    uint32_t address = ((random() ^ (random() << 16)) & 0xfffffffc) + 2;
    
    VTransBurstWrite(address, wbuf, 16);
    
    VTick(10);
    
    VTransBurstRead(address, rbuf, 16);
    
    VPrint("===> ");
    for (int idx =0; idx < 16; idx++)
    {
        VPrint("%02x ", rbuf[idx]);
    }
    VPrint("\n");

    // If ever got this far then sleep forever
    SLEEPFOREVER;
}

