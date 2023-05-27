// ------------------------------------------------------------------------------
//
//  File Name:           VUserMain1.cpp
//  Design Unit Name:    Co-simulation stream Ethernet VC test program
//  Revision:            OSVVM MODELS STANDARD VERSION
//
//  Maintainer:          Simon Southwell      email:  simon.southwell@gmail.com
//  Contributor(s):
//     Simon Southwell   simon.southwell@gmail.com
//
//  Description:
//      Co-simulation burst treaming test using OSVVM Ethernet VC
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

// Import OSVVM user API
#include "OsvvmCosimStream.h"

#ifdef _WIN32
#define srandom srand
#define random rand
#endif

#define BUF_SIZE 1024

// I am node 1 context
static int node  = 1;

       uint8_t TestData1[BUF_SIZE];
extern uint8_t TestData0[BUF_SIZE];
static uint8_t RxData[BUF_SIZE];

// ------------------------------------------------------------------------------
// Use VUserMain0's checkRdata function (re-entrant)
// ------------------------------------------------------------------------------

extern bool checkRdata(uint8_t got, uint8_t exp, int idx, int node_num);

// ------------------------------------------------------------------------------
// Main entry point for node 1 virtual processor software
//
// VUserMainX has no calling arguments. If runtime configuration required
// then you'll need to read in a configuration file.
//
// ------------------------------------------------------------------------------

extern "C" void VUserMain1()
{
    VPrint("VUserMain%d()\n", node);


    int      bufidx = 0;
    int      ridx = 0;

    bool                  error = false;
    OsvvmCosimStream      txrx(node);

    // Use node number, inverted, as the random number generator seed.
    srandom(~node);

    // Generate some random data
    for (int idx = 0; idx < BUF_SIZE; idx ++)
    {
        TestData1[bufidx++] = random() & 0xff;
    }

    // Reset the buffer index
    bufidx = 0;

    // Send some burst of data over TX stream
    txrx.streamBurstSend(&TestData1[bufidx], 128); bufidx += 128;
    txrx.streamBurstSend(&TestData1[bufidx], 128); bufidx += 128;
    txrx.streamBurstSend(&TestData1[bufidx], 16);  bufidx += 16;
    txrx.streamBurstSend(&TestData1[bufidx], 16);  bufidx += 16;
    txrx.streamBurstSend(&TestData1[bufidx], 32);  bufidx += 32;

    // Get some burst data from RX stream
    txrx.streamBurstGet(&RxData[ridx], 16);        ridx += 16;
    txrx.streamBurstGet(&RxData[ridx], 16);        ridx += 16;
    txrx.streamBurstGet(&RxData[ridx], 256);       ridx += 256;

    // Send some burst of data over TX stream
    txrx.streamBurstSend(&TestData1[bufidx], 64);  bufidx += 64;
    txrx.streamBurstSend(&TestData1[bufidx], 128); bufidx += 128;

    // Get some burst data from RX stream
    txrx.streamBurstGet(&RxData[ridx], 32);        ridx += 32;
    txrx.streamBurstGet(&RxData[ridx], 64);        ridx += 64;
    txrx.streamBurstGet(&RxData[ridx], 128);       ridx += 128;

    // Send some burst of data over TX stream
    txrx.streamBurstSend(&TestData1[bufidx], 256); bufidx += 256;
    txrx.streamBurstSend(&TestData1[bufidx], 256); bufidx += 256;

    // Get some burst data from RX stream
    txrx.streamBurstGet(&RxData[ridx], 256);       ridx += 256;
    txrx.streamBurstGet(&RxData[ridx], 256);       ridx += 256;

    // Check all the received data against that expected
    for (int idx = 0; idx < BUF_SIZE; idx++)
    {
        error |= checkRdata(RxData[idx], TestData0[idx], idx, node);
    }

    // Flag to the simulation we're finished, after 10 more iterations
    txrx.tick(10, true, error);

    // If ever got this far then sleep forever
    SLEEPFOREVER;
}