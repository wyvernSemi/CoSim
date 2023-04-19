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
//    05/2023   2023.05    Initial revision
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
#include "OsvvmCosimInt.h"

// I am node 0 context
static int node  = 0;

#ifdef _WIN32
#define srandom srand
#define random rand
#endif

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
    std::string test_name("CoSim_async_trans");
    OsvvmCosimInt  cosim(node, test_name);

    uint32_t addr,   data32,  wdata32, rdata32, i;
    uint16_t data16, wdata16, rdata16;
    uint8_t  data8,  wdata8,  rdata8;
    uint8_t  wbuf[4096], rbuf[4096];

    // -------------------------------
    // Test asynchronous writes with 32 bit data

    addr  = 0x80001000;
    wdata32 = 0x12ff34dd;

    for (i = 0; i < 3; i++)
    {
        cosim.transWriteAsync(addr + i*4, wdata32+i);
    }

    // Blocking write to ensure all async calls have completed
    cosim.transWrite(addr + i*4, wdata32+i);

    for (i = 0; i < 4; i++)
    {
        cosim.transReadCheck(addr + i*4, (uint32_t)(wdata32 + i));
    }

    // -------------------------------
    // Test asynchronous writes with 16 bit data

    addr  = 0x80002000;
    wdata16 = 0x95b3;

    for (i = 0; i < 3; i++)
    {
        data16 = wdata16+(i*0x1111);
        cosim.transWriteAsync(addr + i*2, data16);
    }

    // Blocking write to ensure all async calls have completed
    data16 = wdata16+(i*0x1111);
    cosim.transWrite(addr + i*2, data16);

    for (i = 0; i < 4; i++)
    {
        cosim.transReadCheck(addr + i*2, (uint16_t)(wdata16+(i*0x1111)));
    }

    // -------------------------------
    // Test asynchronous writes with 8 bit data

    addr    = 0x80003001;
    wdata8  = 0x17;

    for (i = 0; i < 3; i++)
    {
        data8 = wdata8+(i*0x22);
        cosim.transWriteAsync(addr + i, data8);
    }

    // Blocking write to ensure all async calls have completed
    data8 = wdata8+(i*0x22);
    cosim.transWrite(addr + i, data8);

    for (i = 0; i < 4; i++)
    {
        cosim.transReadCheck(addr + i, (uint8_t)(wdata8+(i*0x22)));
    }

    // -------------------------------
    // Test asynchronous burst writes

    addr = 0x80004964;

    for (i = 0; i < 128; i++)
    {
        wbuf[i] = 0x23 + i*3;
    }

    cosim.transBurstWriteAsync(addr,    &wbuf[0],  32);
    cosim.transBurstWriteAsync(addr+32, &wbuf[32], 32);
    cosim.transBurstWriteAsync(addr+64, &wbuf[64], 16);

    // Blocking transaction to ensure the others have completed
    cosim.transBurstWrite     (addr+80, &wbuf[80], 48);

    cosim.transBurstRead      (addr, rbuf, 128);

    for (i = 0; i < 128; i++)
    {
        if (rbuf[i] != wbuf[i])
        {
            VPrint("***ERROR: mismatch for async burst write. Got 0x%02x, exp 0x%02x\n", rbuf[i], wbuf[i]);
            error = true;
        }
    }

    // -------------------------------
    // Test asynchronous write address
    // and data

    addr = 0x80010000;

    cosim.transWriteDataAsync((uint32_t) 0xcafef00d);
    cosim.transWriteDataAsync((uint16_t) 0x0bad);

    cosim.transWriteAddressAsync(addr);
    cosim.transWriteAddressAsync(addr + 4);
    cosim.transWriteAddressAsync(addr + 6);
    cosim.transWriteAddressAsync(addr + 8);
    cosim.transWriteAddressAsync(addr + 9);
    cosim.transWriteAddressAsync(addr + 10);
    cosim.transWriteAddressAsync(addr + 11);

    cosim.transWriteDataAsync((uint16_t) 0x0fab, 2);
    cosim.transWriteDataAsync((uint8_t)  0xaa,   0);
    cosim.transWriteDataAsync((uint8_t)  0x55,   1);
    cosim.transWriteDataAsync((uint8_t)  0xbb,   2);
    cosim.transWriteDataAsync((uint8_t)  0xdd,   3);

    uint32_t expdata32[3] = {0xcafef00d, 0x0fab0bad, 0xddbb55aa};

    for (i = 0; i < 3; i++)
    {
        cosim.transRead(addr + i*4, &rdata32);

        if (rdata32 != expdata32[i])
        {
            VPrint("***ERROR: mismatch for async write address/data. Got 0x%08x, exp 0x%08x\n", rdata32, expdata32[i]);
            error = true;
        }
    }

    // -------------------------------
    // Test asynchronous read address
    // and data

    cosim.transReadAddressAsync(addr);
    cosim.transReadAddressAsync(addr + 1);
    cosim.transReadAddressAsync(addr + 2);
    cosim.transReadAddressAsync(addr + 3);

    uint8_t expdata8[4] = {0x0d, 0xf0, 0xfe, 0xca};

    for(i = 0; i < 4; i++)
    {
        cosim.transReadDataCheck(expdata8[i]);
    }

    cosim.transReadAddressAsync(addr + 4);
    cosim.transReadDataCheck(expdata32[1]);


    cosim.transReadAddressAsync(addr + 8);
    cosim.transReadAddressAsync(addr + 10);

    for (i = 0; i < 2; i++)
    {
        data16 = (expdata32[2] >> (i*16)) & 0xffff;

        cosim.transReadDataCheck(data16);
    }

    // -------------------------------
    // Test increment/random burst functions

    addr   = 0x70091230;
    wdata8 = 0x57;

    cosim.transBurstWriteIncrementAsync(addr, wdata8, 16);
    cosim.transBurstWriteIncrement(addr+16, wdata8+16, 32);
    cosim.transBurstReadCheckIncrement(addr, wdata8, 48);

    addr   = 0x5a9607a8;
    wdata8 = 0xdf;

    cosim.transBurstWriteRandomAsync(addr, wdata8, 64);
    cosim.transBurstWriteRandom(addr+64, wdata8 ^ 0xff, 48);

    cosim.transBurstReadCheckRandom(addr, wdata8, 64);
    cosim.transBurstReadCheckRandom(addr+64, wdata8 ^ 0xff, 48);

    // -------------------------------
    // Test "Try" functions for 8 bit data

    addr   = 0x40007000;
    wdata8 = 0x99;

    cosim.transWrite(addr, wdata8);

    bool avail = cosim.transTryReadData(&rdata8);

    if (avail)
    {
        VPrint("***ERROR: got unexpected data available returned from transTryReadData\n");
        error = true;
    }

    cosim.transReadAddressAsync(addr);

    cosim.tick(20);

    avail = cosim.transTryReadData(&rdata8);

    if (!avail)
    {
        VPrint("***ERROR: got unexpected data unavailable returned from transTryReadData\n");
        error = true;
    }

    if (rdata8 != wdata8)
    {
        VPrint("***ERROR: mismatch for transTryReadData. Got 0x%02x, exp 0x%02x\n", rdata8, wdata8);
        error = true;
    }

    // -------------------------------
    // Test "Try" functions for 16 bit data

    addr   = 0x40008000;
    wdata16 = 0x70da;

    cosim.transWrite(addr, wdata16);

    avail = cosim.transTryReadData(&rdata16);

    if (avail)
    {
        VPrint("***ERROR: got unexpected data available returned from transTryReadData\n");
        error = true;
    }

    cosim.transReadAddressAsync(addr);

    cosim.tick(20);

    avail = cosim.transTryReadData(&rdata16);

    if (!avail)
    {
        VPrint("***ERROR: got unexpected data unavailable returned from transTryReadData\n");
        error = true;
    }

    if (rdata16 != wdata16)
    {
        VPrint("***ERROR: mismatch for transTryReadData. Got 0x%04x, exp 0x%04x\n", rdata16, wdata16);
        error = true;
    }

    // -------------------------------
    // Test "Try" functions for 16 bit data

    addr    = 0x40009000;
    wdata32 = 0x196de310;

    cosim.transWrite(addr, wdata32);

    avail = cosim.transTryReadData(&rdata32);

    if (avail)
    {
        VPrint("***ERROR: got unexpected data available returned from transTryReadData\n");
        error = true;
    }

    cosim.transReadAddressAsync(addr);

    cosim.tick(20);

    avail = cosim.transTryReadData(&rdata32);

    if (!avail)
    {
        VPrint("***ERROR: got unexpected data unavailable returned from transTryReadData\n");
        error = true;
    }

    if (rdata32 != wdata32)
    {
        VPrint("***ERROR: mismatch for transTryReadData. Got 0x%04x, exp 0x%04x\n", rdata32, wdata32);
        error = true;
    }

    // -------------------------------
    // Test "Try and check" functions for 8 bit data

    addr   = 0x4000a000;
    wdata8 = 0x3d;

    cosim.transWrite(addr, wdata8);

    avail = cosim.transTryReadDataCheck(wdata8);

    if (avail)
    {
        VPrint("***ERROR: got unexpected data available returned from transTryReadDataCheck\n");
        error = true;
    }

    cosim.transReadAddressAsync(addr);

    cosim.tick(20);

    avail = cosim.transTryReadDataCheck(wdata8);

    if (!avail)
    {
        VPrint("***ERROR: got unexpected data unavailable returned from transTryReadDataCheck\n");
        error = true;
    }

    // -------------------------------
    // Test "Try" functions for 16 bit data

    addr    = 0x4000b000;
    wdata16 = 0xf31a;

    cosim.transWrite(addr, wdata16);

    avail = cosim.transTryReadDataCheck(wdata16);

    if (avail)
    {
        VPrint("***ERROR: got unexpected data available returned from transTryReadDataCheck\n");
        error = true;
    }

    cosim.transReadAddressAsync(addr);

    cosim.tick(20);

    avail = cosim.transTryReadDataCheck(wdata16);

    if (!avail)
    {
        VPrint("***ERROR: got unexpected data unavailable returned from transTryReadDataCheck\n");
        error = true;
    }

    // -------------------------------
    // Test "Try" functions for 16 bit data

    addr    = 0x4000c000;
    wdata32 = 0x9e23a007;

    cosim.transWrite(addr, wdata32);

    avail = cosim.transTryReadDataCheck(wdata32);

    if (avail)
    {
        VPrint("***ERROR: got unexpected data available returned from transTryReadDataCheck\n");
        error = true;
    }

    cosim.transReadAddressAsync(addr);

    cosim.tick(20);

    avail = cosim.transTryReadDataCheck(wdata32);

    if (!avail)
    {
        VPrint("***ERROR: got unexpected data unavailable returned from transTryReadDataCheck\n");
        error = true;
    }

    // -------------------------------
    // Test Push/pop functions

    addr    = 0x39001500;

    for (int idx = 0; idx < 128; idx++)
    {
        wbuf[idx] = 0x75 + (idx*7);
    }

    cosim.transBurstPushData(wbuf, 128);
    cosim.transBurstWrite(addr, 128);
    cosim.transBurstRead(addr, 128);
    cosim.transBurstPopData(rbuf, 128);

    for (int idx = 0; idx < 128; idx++)
    {
        if (rbuf[idx] != wbuf[idx])
        {
            VPrint("***ERROR: mismatch in Push/Pop burst data. Got 0x%02x, exp. 0x%02x\n", rbuf[idx], wbuf[idx]);
            error = true;
        }
    }

    addr   = 0xa0001940;
    wdata8 = 0xd8;

    cosim.transBurstPushIncrement(wdata8, 64);
    cosim.transBurstWrite(addr, 64);
    cosim.transBurstRead(addr, 64);
    cosim.transBurstCheckIncrement(wdata8, 64);

    addr   = 0xe0002834;
    wdata8 = 0x0a;

    cosim.transBurstPushRandom(wdata8, 64);
    cosim.transBurstWrite(addr, 64);
    cosim.transBurstRead(addr, 64);
    cosim.transBurstCheckRandom(wdata8, 64);

    // -------------------------------
    // Test check data methods

    addr = 0x07804720;

    for (i = 0; i < 64; i++)
    {
        wbuf[i] = 0x48 + i*3;
    }

    cosim.transBurstWrite(addr, wbuf, 64);
    if (cosim.transBurstReadCheckData(addr, wbuf, 64))
    {
        VPrint("***ERROR: data mismatch on transBurstReadCheck\n");
        error = true;
    }

    addr = 0x17804700;

    for (i = 0; i < 64; i++)
    {
        wbuf[i] = 0xd5 + i*3;
    }

    cosim.transBurstWrite(addr, wbuf, 64);
    cosim.transBurstRead(addr, 64);
    if (cosim.transBurstCheckData(wbuf, 64))
    {
        VPrint("***ERROR: data mismatch on transBurstReadCheck\n");
        error = true;
    }

    // -------------------------------
    // Flag to the simulation we're finished, after 10 more iterations
    cosim.tick(10, true, error);

    // If ever got this far then sleep forever
    SLEEPFOREVER;
}

