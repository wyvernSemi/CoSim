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

#ifdef _WIN32
#define srandom srand
#define random rand
#endif

static const int max_burst_size     = 4096;
static const int max_rd_wr_distance = 5;
static const int size_order_log2    = 8;

typedef struct {
    uint32_t addr;
    uint8_t  wdata[max_burst_size];
    int      size;
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

    std::vector<wtrans_t> vec;
    wtrans_t              wtrans;
    std::string test_name("CoSim_usercode_burst");
    OsvvmCosim  cosim(node, test_name);

    uint8_t rbuf[max_burst_size];
    bool    error = false;
    int     rnw;
    int     count = 400; 

    // Use node number, inverted, as the random number generator seed.
    srandom(~node);

    while(!error and count != 0)
    {
        count--;
        
        // Decide whether a read or a write
        rnw  = vec.empty()                      ? 0 : // Force a write when none outstanding
               vec.size() == max_rd_wr_distance ? 1 : // Force a read if number of writes reaches maximum
               random() & 1;                          // Else do a random read/write

        // If writing....
        if (!rnw)
        {
            // Generate an unaligned random address
            wtrans.addr = random() ^ (random() << 16);
            
            // Calculate the magnitude of the transaction size
            // (Powers of 2 between 1 and 256)
            int magnitude = 1 << (random() % size_order_log2);
            
            // Set the transaction size to be the magnitude plus a random offset
            // which is between 0 and the size of the magnitude
            wtrans.size = magnitude + random()%magnitude;

            // Put random bytes in the write data buffer
            for (int idx = 0; idx < wtrans.size; idx++)
            {
                wtrans.wdata[idx] = random() & 0xff;
            }
            
            // Save the write transaction information
            vec.push_back(wtrans);

            // Generate a write burst transaction
            cosim.transBurstWrite(wtrans.addr, wtrans.wdata, wtrans.size);

        }
        // If reading...
        else
        {
            // Fetch the write transaction's information and clear from vector
            wtrans = vec.front();
            vec.erase(vec.begin()); 
 
            // Read back fro the write address the transaction data stored
            cosim.transBurstRead(wtrans.addr, rbuf, wtrans.size);

            // Compare the data read from memory with that in the write transaction buffer.
            // If a failure, report and end the generation of transactions.
            for (int idx = 0; idx < wtrans.size; idx++)
            {
                if (rbuf[idx] != wtrans.wdata[idx])
                {
                    VPrint("**ERROR: data mismatch on read transaction starting at index %d. Exp 0x%02x Got 0x%02x\n",
                            idx, rbuf[idx], wtrans.wdata[idx]);
                    error = true;
                    break;
                }
            }
        }
    }
    
    // Flag to the simulation we've finished after a small delay
    cosim.tick(10, true, error);

    // If ever got this far then sleep forever
    SLEEPFOREVER;
}

