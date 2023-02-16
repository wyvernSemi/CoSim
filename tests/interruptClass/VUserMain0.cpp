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

// Import VProc user API
#include "OsvvmCosimInt.h"

// I am node 0 context
static int node  = 0;

#define INT0 0x00000001

static const uint32_t        sw_int_addr = 0xaffffffc;

// Pointer to the interrupt version of the co-sim API for use in main program and ISRs
static OsvvmCosimInt*        cosim;

// Error flag made available to main program and ISRs
static bool                  error = false;

// ISR calling counts
static uint32_t              int_count[2] = {0, 0};


// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------

static void write_read_test(
    const int num_iterations           = 40,
    const uint32_t base_addr           = 0x10000000,
    const uint32_t wdata_start_pattern = 0,
    int            isr_count           = 15)
{

    uint32_t wdata = 0;
    uint32_t addr  = base_addr;

    for (int loop = 0; loop < num_iterations; loop++)
    {
        uint32_t rdata;
        uint32_t start_addr = addr;

        // At about a third the way through the test write to the s/w interrupt register
        // at initiate an interrupt.
        if (loop == isr_count)
        {
            // Interrupt at level 0
            cosim->transWrite(sw_int_addr, (uint32_t)INT0);
        }

        // Do a series of writes to memory
        for (int idx = 0; idx < 4; idx++)
        {
            cosim->transWrite(addr, wdata + idx);
            addr  += 4;
        }

        // Reset the address to the start of the block
        addr = start_addr;

        // Read back the block of writes and check
        for (int idx = 0; idx < 4; idx++)
        {
            cosim->transRead(addr, &rdata);
            if (rdata != (wdata + idx))
            {
                VPrint("VUserMain0: ***ERROR*** read %08X from address %08X. Expected %08x\n", rdata, addr, wdata + idx);
                error = true;
                break;
            }
            addr += 4;
        }

        // Move the write data on from the value at the start of the block
        wdata += 0x10;
    }
}
// ------------------------------------------------------------------------------
// Interrupt callback from co-simulation layer
// ------------------------------------------------------------------------------

int interruptCB(int int_vec)
{
    VPrint("interruptCB() called with 0x%08x\n", int_vec);

    cosim->updateIntReq(int_vec);

    return 0;
}

// ------------------------------------------------------------------------------
// Interrupt service routine for level 0 (highest priority)
// ------------------------------------------------------------------------------

int isr0(int arg)
{
    VPrint("Entered isr0\n");
    int_count[0]++;

    cosim->disableIsr(1);

    //uint32_t addr  = base_addr;
    const uint32_t base_addr   = 0x20000000;
    const uint32_t wdata_start = 0x10000;
    
    write_read_test(15, base_addr, wdata_start, -1);

    // Clear interrupt level 0
    cosim->transWrite(sw_int_addr, (uint32_t)0);

    // As software runs infinitely fast in simulation time, ensure the
    // clearing of the interrupt propogated before re-enabling it.
    cosim->tick(1);

    cosim->enableIsr(0);
    VPrint("Exiting isr0\n");
    return 0;
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

    const uint32_t        base_addr           = 0x10000000;
    const uint32_t        wdata_start_pattern = 0;
    const uint32_t        num_iterations      = 40;
    std::string           test_name("TbAb_InterruptCoSim5");

    cosim = new OsvvmCosimInt(node, test_name);

    cosim->regInterruptCB(interruptCB);
    cosim->registerIsr(isr0, 0);

    cosim->enableIsr(0);
    cosim->enableMasterInterrupt();

    // Do some read and write tests and write a s/w interrupt part way through
    write_read_test(num_iterations, base_addr, wdata_start_pattern, num_iterations/3);

    // When the write/read test has finished, check the ISR was called once, and only once
    if (int_count[0] != 1)
    {
        VPrint("VUserMain0: ***ERROR*** got interrupt count of %d. Expected 1\n", int_count[0]);
    }

    // Flag to the simulation we're finished, after 10 more iterations
    cosim->tick(10, true, error);

    // If ever got this far then sleep forever
    SLEEPFOREVER;
}

