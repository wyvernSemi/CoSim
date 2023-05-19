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
//    05/2023   2023       Initial revision
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
#include <cmath>
#include <algorithm>
#include <vector>

// Import VProc user API
#include "OsvvmCosim.h"

// I am node 0 context
static int node  = 0;

extern int barrier;
static int last_barrier = 0;

uint32_t testvals[1024];
int      wcount = 0;
int      rcount = 0;

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------

static void waitOnBarrier(void)
{
    OsvvmCosim  cosim(node);

    while(barrier <= last_barrier)
    {
        cosim.tick(1);
    }

    last_barrier = barrier;
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

    bool                  error = false;
    std::string test_name("CoSim_responder");
    OsvvmCosim  cosim(node, test_name);

    uint32_t addr;
    uint32_t data32;
    uint16_t data16;
    uint8_t  data8;

    int tidx = 0;

    waitOnBarrier();

    addr   = testvals[tidx++] = 0x70004000;
    data32 = testvals[tidx++] = 0x9a50b000;

    cosim.transWrite(addr, data32); wcount++;

    addr   += 0x1000; testvals[tidx++] = addr;
    data32 += 0x123;  testvals[tidx++] = data32;

    cosim.transWrite(addr, data32); wcount++;

    waitOnBarrier();

    addr   = testvals[tidx++] = 0xa0008000;
    data16 = testvals[tidx++] = 0x1964;

    cosim.transWrite(addr, data16); wcount++;

    addr   += 0x1000; testvals[tidx++] = addr;
    data16 += 0x123;  testvals[tidx++] = data16;

    cosim.transWrite(addr, data16); wcount++;

    waitOnBarrier();

    addr  = testvals[tidx++] = 0x9700ade0;
    data8 = testvals[tidx++] = 0x25;

    cosim.transWrite(addr, data8); wcount++;

    addr   += 0x1000; testvals[tidx++] = addr;
    data8  += 0x12;   testvals[tidx++] = data8;

    cosim.transWrite(addr, data8); wcount++;

    waitOnBarrier();

    addr   = testvals[tidx++] = 0xbeef9000;
    data32 = testvals[tidx++] = 0;

    cosim.transWriteAddressAsync(addr); wcount++;

    waitOnBarrier();

    addr   = testvals[tidx++] = 0;
    data32 = testvals[tidx++] = 0x7800c344;

    cosim.transWriteDataAsync(data32);

    waitOnBarrier();

    addr   = testvals[tidx++] = 0x41987902;
    data32 = testvals[tidx++] = 0;

    cosim.transWriteAddressAsync(addr); wcount++;

    waitOnBarrier();

    addr   = testvals[tidx++] = 0;
    data16 = testvals[tidx++] = 0xffc0;

    cosim.transWriteDataAsync(data16, 2);

    waitOnBarrier();

    addr   = testvals[tidx++] = 0x007c98d0;
    data32 = testvals[tidx++] = 0;

    cosim.transWriteAddressAsync(addr); wcount++;

    waitOnBarrier();

    addr   = testvals[tidx++] = 0;
    data8  = testvals[tidx++] = 0xa6;

    cosim.transWriteDataAsync(data8, 0);

    waitOnBarrier();

    addr   = testvals[tidx++] = 0x20001000;
    testvals[tidx++] = 0;
    testvals[tidx++] = 0;
    data32 = testvals[tidx++] = 0xb508de78;

    cosim.transWriteAddressAsync(addr); wcount++;
    cosim.transWriteDataAsync(data32);

    addr   += 4;       testvals[tidx++] = addr;
    testvals[tidx++] = 0;
    testvals[tidx++] = 0;
    data16  = 0x9250;  testvals[tidx++] = data16;

    cosim.transWriteAddressAsync(addr); wcount++;
    cosim.transWriteDataAsync(data16);

    addr   += 2;   testvals[tidx++] = addr;
    testvals[tidx++] = 0;
    testvals[tidx++] = 0;
    data8  = 0x78; testvals[tidx++] = data8;

    cosim.transWriteAddressAsync(addr); wcount++;
    cosim.transWriteDataAsync(data8);

    waitOnBarrier();

    addr   = testvals[tidx++] = 0xb9008710;
    data32 = testvals[tidx++] = 0x00340043;

    cosim.transReadCheck(addr, data32); rcount++;

    waitOnBarrier();

    addr   = testvals[tidx++] = 0x09005eed;
    data16 = testvals[tidx++] = 0xb13d;

    cosim.transReadCheck(addr, data16); rcount++;

    waitOnBarrier();

    addr   = testvals[tidx++] = 0x3278abba;
    data8  = testvals[tidx++] = 0x13;

    cosim.transReadCheck(addr, data8); rcount++;

    addr   = testvals[tidx++] = 0xA0900400;
    data32 = testvals[tidx++] = 0xb190ef44;

    cosim.transReadCheck(addr, data32); rcount++;

    waitOnBarrier();

    addr   = testvals[tidx++] = 0x60047d08;
    data16 = testvals[tidx++] = 0x106f;

    cosim.transReadCheck(addr, data16); rcount++;

    waitOnBarrier();

    addr   = testvals[tidx++] = 0xfff07904;
    data8  = testvals[tidx++] = 0x77;

    cosim.transReadCheck(addr, data8); rcount++;

    waitOnBarrier();

    addr   = testvals[tidx++] = 0xA0900400;
    data32 = testvals[tidx++] = 0;

    cosim.transReadAddressAsync(addr); rcount++;

    addr   = testvals[tidx++] = 0;
    data32 = testvals[tidx++] = 0xc0089508;

    cosim.transReadDataCheck(data32);

    waitOnBarrier();
    addr   = testvals[tidx++] = 0x777014e8;
    data32 = testvals[tidx++] = 0;

    cosim.transReadAddressAsync(addr); rcount++;

    addr   = testvals[tidx++] = 0;
    data16 = testvals[tidx++] = 0xee61;

    cosim.transReadDataCheck(data16);

    waitOnBarrier();
    addr   = testvals[tidx++] = 0x43211234;
    data32 = testvals[tidx++] = 0;

    cosim.transReadAddressAsync(addr); rcount++;

    addr   = testvals[tidx++] = 0;
    data8 = testvals[tidx++] = 0x71;

    cosim.transReadDataCheck(data8);

    waitOnBarrier();

    addr   = testvals[tidx++] = 0x0099e094;
    data32 = testvals[tidx++] = 0;

    cosim.transReadAddressAsync(addr); rcount++;

    addr   = testvals[tidx++] = 0;
    data32 = testvals[tidx++] = 0x811fe4c0;

    cosim.transReadDataCheck(data32);

    waitOnBarrier();

    addr   = testvals[tidx++] = 0xee0039e0;
    data32 = testvals[tidx++] = 0;

    cosim.transReadAddressAsync(addr); rcount++;

    addr   = testvals[tidx++] = 0;
    data16 = testvals[tidx++] = 0xd0cc;

    cosim.transReadDataCheck(data16);

    waitOnBarrier();

    addr   = testvals[tidx++] = 0x29001cf8;
    data32 = testvals[tidx++] = 0;

    cosim.transReadAddressAsync(addr); rcount++;

    addr   = testvals[tidx++] = 0;
    data8 = testvals[tidx++] = 0xc5;

    cosim.transReadDataCheck(data8);
    
    waitOnBarrier();
    cosim.tick(100);
    
    addr   = testvals[tidx++] = 0x99885614;
    data32 = testvals[tidx++] = 0x5ee71190;
    
    cosim.transWrite(addr, data32); wcount++;
    
    waitOnBarrier();
    cosim.tick(100);
    
    addr   = testvals[tidx++] = 0xb9e14568;
    data32 = testvals[tidx++] = 0x1ce067d2;
    
    cosim.transReadCheck(addr, data32); rcount++;
    
    waitOnBarrier();
    cosim.tick(100);
    
    addr   = testvals[tidx++] = 0x11112244;
    data32 = testvals[tidx++] = 0x3901d6fb;
    
    cosim.transWrite(addr, data32); wcount++;
    
    waitOnBarrier();
    cosim.tick(100);
    
    addr   = testvals[tidx++] = 0x22229b80;
    data32 = testvals[tidx++] = 0xe73aa691;
    
    cosim.transReadCheck(addr, data32); rcount++;

    // Flag to the simulation we're finished, after 10 more iterations
    cosim.tick(10, true, error);

    // If ever got this far then sleep forever
    SLEEPFOREVER;
}

