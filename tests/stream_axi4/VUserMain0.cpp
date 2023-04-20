// ------------------------------------------------------------------------------
//
//  File Name:           VUserMain0.cpp
//  Design Unit Name:    Co-simulation AXI4 stream VC test program
//  Revision:            OSVVM MODELS STANDARD VERSION
//
//  Maintainer:          Simon Southwell      email:  simon.southwell@gmail.com
//  Contributor(s):
//     Simon Southwell   simon.southwell@gmail.com
//
//  Description:
//      Co-simulation test AXI4 stream source
//
//  Developed by:
//        Simon Southwell
//
//  Revision History:
//    Date      Version    Description
//    03/2024   2023.05    Extended tests to non-blocking methods
//    03/2023   2023.04    Initial revision
//
//  This file is part of OSVVM.
//
//  Copyright (c) 2023 by [OSVVM Authors](../../AUTHORS.md)
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

#ifdef _WIN32
#define srandom srand
#define random rand
#endif

#define BUF_SIZE 1024

// I am node 0 context
static int node  = 0;

// ------------------------------------------------------------------------------
// Function to construct parameter from AXI stream signal values
// ------------------------------------------------------------------------------

int makeAxiStreamParam(const int TID, const int TDEST, const int TUSER, const int TLAST)
{
    return ((TID & 0xff) << 9) | ((TDEST & 0xf) << 5) | ((TUSER & 0xf) << 1) | (TLAST & 0x1);
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

    const int TID   = 0xB;
    const int TDEST = 0xA;
    const int TUSER = 0xD;

    const int DATASIZE = 5;

    bool                  error = false;
    std::string           test_name("CoSim_axi4_streams");
    OsvvmCosimStream      axistream(node, test_name);

    uint32_t wdata  = 0x80001000;
    uint32_t rdata;
    uint8_t  TestData0[BUF_SIZE], RxData[BUF_SIZE];
    int      bufidx = 0;
    int      param  = makeAxiStreamParam(TID, TDEST, TUSER, 0);

    // Use node number, inverted, as the random number generator seed.
    srandom(~node);

    // =============================================================

    // Send a series of words over Axi stream interface using TID, TDEST and TUSER,
    // flagging last one by setting TLAST
    for (uint32_t idx = 0; idx < DATASIZE; idx++)
    {
        if (idx == DATASIZE-1)
        {
            param = makeAxiStreamParam(TID, TDEST, TUSER, 1);
        }
        axistream.streamSend(wdata + idx, param);
    }

    // Get received word data and check data and status values
    for (int idx = 0; idx < DATASIZE; idx++)
    {
        int status, expstatus;

        expstatus = makeAxiStreamParam(TID, TDEST, TUSER, (idx == DATASIZE-1));

        axistream.streamGet(&rdata, &status);

        if (rdata != (wdata + idx))
        {
            VPrint("VuserMain%d: ***ERROR mismatch in received data. Got 0x%08x, expected 0x%08x\n", node, rdata, wdata+idx);
            error = true;
        }
        else if (status != expstatus)
        {
            VPrint("VuserMain%d: ***ERROR mismatch in received status. Got 0x%03x, expected 0x%03x\n", node, status, expstatus);
            error = true;
        }
        else
        {
            VPrint("VuserMain%d: received byte 0x%08x\n", node, rdata);
        }
    }

    // =============================================================

    wdata = 0x7650ad34;

    // Send a series of words over Axi stream interface using TID, TDEST and TUSER,
    // flagging last one by setting TLAST
    for (uint32_t idx = 0; idx < DATASIZE; idx++)
    {
        if (idx == DATASIZE-1)
        {
            param = makeAxiStreamParam(TID, TDEST, TUSER, 1);
        }
        axistream.streamSend(wdata + idx, param);
    }

    // Get received word data and check data and status values
    for (int idx = 0; idx < DATASIZE; idx++)
    {
        axistream.streamCheck((uint32_t)(wdata + idx), param);
    }

    // =============================================================

    // Fill test buffer with random numbers
    for (int idx = 0; idx < BUF_SIZE; idx ++)
    {
        TestData0[idx++] = random() & 0xff;
    }

    bufidx = 0;

    // Send a couple of bursts
    axistream.streamBurstSend(&TestData0[bufidx], 16);  bufidx += 16;
    axistream.streamBurstSend(&TestData0[bufidx], 256); bufidx += 256;

    bufidx = 0;

    // Retrieve received data
    axistream.streamBurstGet(&RxData[bufidx], 16);  bufidx += 16;
    axistream.streamBurstGet(&RxData[bufidx], 256); bufidx += 256;

    // Check data validity
    for (int idx = 0; idx < bufidx; idx++)
    {
        if (RxData[idx] != TestData0[idx])
        {
            VPrint("VuserMain%d: ***ERROR mismatch in received byte. Got0x%02x, expected 0x%02x\n", node, RxData[idx], TestData0[idx]);
        }
    }

    // =============================================================
    // Repeat test with asynchronous writes

    VPrint("VUserMain%d: ===== STARTING ASYNC TESTS =====\n", node);

    bufidx = 0;
    wdata  = 0x19640825;
    param  = makeAxiStreamParam(TID, TDEST, TUSER, 0);

    // Send a series of words over Axi stream interface using TID, TDEST and TUSER,
    // flagging last one by setting TLAST
    for (uint32_t idx = 0; idx < DATASIZE; idx++)
    {
        if (idx == DATASIZE-1)
        {
            param = makeAxiStreamParam(TID, TDEST, TUSER, 1);
        }
        axistream.streamSendAsync(wdata + idx, param);
    }


    // Get received word data and check data and status values
    for (int idx = 0; idx < DATASIZE; idx++)
    {
        int status, expstatus;

        expstatus = makeAxiStreamParam(TID, TDEST, TUSER, (idx == DATASIZE-1));

        axistream.streamGet(&rdata, &status);

        if (rdata != (wdata + idx))
        {
            VPrint("VuserMain%d: ***ERROR mismatch in received data. Got 0x%08x, expected 0x%08x\n", node, rdata, wdata+idx);
            error = true;
        }
        else if (status != expstatus)
        {
            VPrint("VuserMain%d: ***ERROR mismatch in received status. Got 0x%03x, expected 0x%03x\n", node, status, expstatus);
            error = true;
        }
        else
        {
            VPrint("VuserMain%d: received byte 0x%08x\n", node, rdata);
        }
    }

    // =============================================================

    // Fill test buffer with random numbers
    for (int idx = 0; idx < BUF_SIZE; idx ++)
    {
        TestData0[idx++] = random() & 0xff;
    }

    bufidx = 0;

    // Send a couple of bursts
    axistream.streamBurstSendAsync(&TestData0[bufidx], 16);  bufidx += 16;
    axistream.streamBurstSendAsync(&TestData0[bufidx], 256); bufidx += 256;

    bufidx = 0;

    // Retrieve received data
    axistream.streamBurstGet(16);
    axistream.streamBurstGet(256);

    axistream.streamBurstPopData(RxData, 272);

    // Check data validity
    for (int idx = 0; idx < bufidx; idx++)
    {
        if (RxData[idx] != TestData0[idx])
        {
            VPrint("VuserMain%d: ***ERROR mismatch in received byte. Got 0x%02x, expected 0x%02x\n", node, RxData[idx], TestData0[idx]);
        }
    }

    // =============================================================

    // Fill test buffer with random numbers
    for (int idx = 0; idx < BUF_SIZE; idx ++)
    {
        TestData0[idx] = random() & 0xff;
    }

    bufidx = 0;

    // Send a couple of bursts
    axistream.streamBurstSendAsync(&TestData0[bufidx], 16);  bufidx += 16;
    axistream.streamBurstSendAsync(&TestData0[bufidx], 256); bufidx += 256;

    bufidx = 0;

    // Check received data
    axistream.streamBurstCheck(&TestData0[bufidx], 16);   bufidx += 16;
    axistream.streamBurstCheck(&TestData0[bufidx], 256);  bufidx += 256;

    // =============================================================

    // Fill test buffer with random numbers
    for (int idx = 0; idx < BUF_SIZE; idx ++)
    {
        TestData0[idx++] = random() & 0xff;
    }

    bufidx = 0;

    // Send a couple of bursts
    axistream.streamBurstPushData(&TestData0[bufidx], 16);
    axistream.streamBurstSendAsync(16);  bufidx += 16;

    axistream.streamBurstPushData(&TestData0[bufidx], 256);
    axistream.streamBurstSend(256); bufidx += 256;

    bufidx = 0;

    // Check received data
    axistream.streamBurstCheck(&TestData0[bufidx], 16);   bufidx += 16;

    axistream.streamBurstPushCheckData(&TestData0[bufidx], 256);
    axistream.streamBurstCheck(256);  bufidx += 256;

    // =============================================================

    bufidx = 0;

    axistream.streamBurstSendIncrementAsync(0x57, 32);
    axistream.streamBurstSendIncrement(0xe6, 128);

    axistream.streamBurstCheckIncrement(0x57, 32);
    axistream.streamBurstCheckIncrement(0xe6, 128);

    axistream.streamBurstSendRandomAsync(0x9b, 64);
    axistream.streamBurstSendRandom(0x0f, 64);

    axistream.streamBurstCheckRandom(0x9b, 64);
    axistream.streamBurstCheckRandom(0x0f, 64);

    axistream.streamBurstPushIncrement(0xa2, 32);
    axistream.streamBurstSend(32);
    axistream.streamBurstCheckIncrement(0xa2, 32);

    axistream.streamBurstPushRandom(0x55, 32);
    axistream.streamBurstSend(32);
    axistream.streamBurstCheckRandom(0x55, 32);

    axistream.streamBurstSendIncrement(0x6e, 48);
    axistream.streamBurstPushCheckIncrement(0x6e, 48);
    axistream.streamBurstCheck(48);

    axistream.streamBurstSendRandom(0x39, 48);
    axistream.streamBurstPushCheckRandom(0x39, 48);
    axistream.streamBurstCheck(48);

    // -----------------------------------------------------------

    // Flag to the simulation we're finished, after 10 more ticks
    axistream.tick(10, true, error);

    // If ever got this far then sleep forever
    SLEEPFOREVER;
}

