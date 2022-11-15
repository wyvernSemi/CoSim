// =========================================================================
//
//  File Name:         OsvvmVProc.h
//  Design Unit Name:  
//  Revision:          OSVVM MODELS STANDARD VERSION
//
//  Maintainer:        Simon Southwell email:  simon.southwell@gmail.com
//  Contributor(s):
//     Simon Southwell      simon.southwell@gmail.com
//
//
//  Description:
//      Simulator co-simulation virtual procedure C interface routines for
//      scheduling access to user processes
//
//
//  Developed by:
//        SynthWorks Design Inc.
//        VHDL Training Classes
//        http://www.SynthWorks.com
//
//  Revision History:
//    Date      Version    Description
//    10/2022   2022       Initial revision
//
//
//  This file is part of OSVVM.
//
//  Copyright (c) 2022 by SynthWorks Design Inc.
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
// =========================================================================

#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include "OsvvmVProc.h"
#include "OsvvmVUser.h"
#include "OsvvmVSchedPli.h"

// Pointers to state for each node (up to VP_MAX_NODES)
pSchedState_t ns[VP_MAX_NODES];


/////////////////////////////////////////////////////////////
// Main routine called whenever $vinit task invoked from
// initial block of VProc module.
//
VPROC_RTN_TYPE VInit (VINIT_PARAMS)
{

    io_printf("VInit()\n");

    // Range check node number
    if (node < 0 || node >= VP_MAX_NODES)
    {
        io_printf("***Error: VInit() got out of range node number (%d)\n", node);
        exit(VP_USER_ERR);
    }

    debug_io_printf("VInit(): node = %d\n", node);

    // Allocate some space for the node state and update pointer
    ns[node] = (pSchedState_t) malloc(sizeof(SchedState_t));

    // Set up semaphores for this node
    debug_io_printf("VInit(): initialising semaphores for node %d\n", node);

    if (sem_init(&(ns[node]->snd), 0, 0) == -1)
    {
        io_printf("***Error: VInit() failed to initialise semaphore\n");
        exit(1);
    }
    if (sem_init(&(ns[node]->rcv), 0, 0) == -1)
    {
        io_printf("***Error: VInit() failed to initialise semaphore\n");
        exit(1);
    }

    debug_io_printf("VInit(): initialising semaphores for node %d---Done\n", node);

    // Issue a new thread to run the user code
    VUser(node);
}

// -------------------------------------------------------------------------
// VHalt()
//
// Called for a 'reason'. Holding procedure to catch 'finish'
// in case of any tidying up required.
// -------------------------------------------------------------------------

int VHalt (int data, int reason)
{
    debug_io_printf("VHalt(): data = %d reason = %d\n", data, reason);

    if (reason == reason_endofcompile) {
    } else if (reason == reason_finish) {
    } else if (reason == reason_startofsave) {
    } else if (reason == reason_save) {
    } else if (reason == reason_restart) {
        debug_io_printf("VHalt(): restart\n");
    } else if (reason != reason_finish) {
        debug_io_printf("VHalt(): not called for a halt reason (%d)\n", reason);
        return 0;
    }
}

// -------------------------------------------------------------------------
// VSched()
//
// Main routine called whenever $vsched task invoked, on
// clock edge of scheduled cycle.
// -------------------------------------------------------------------------

VPROC_RTN_TYPE VSched (VSCHED_PARAMS)
{

    int VPDataOut_int, VPAddr_int, VPRw_int, VPTicks_int;

    // Sample inputs and update node state
    ns[node]->rcv_buf.data_in   = VPDataIn;
    ns[node]->rcv_buf.interrupt = Interrupt;

    // Send message to VUser with VPDataIn value
    debug_io_printf("VSched(): setting rcv[%d] semaphore\n", node);
    sem_post(&(ns[node]->rcv));

    // Wait for a message from VUser process with output data
    debug_io_printf("VSched(): waiting for snd[%d] semaphore\n", node);
    sem_wait(&(ns[node]->snd));

    // Update outputs of $vsched task
    if (ns[node]->send_buf.ticks >= DELTA_CYCLE)
    {

        switch(ns[node]->send_buf.type)
        {
            case trans32_wr_word:
            case trans32_rd_word:
              VPDataOut_int = ((uint32_t*)ns[node]->send_buf.data)[0];
              VPAddr_int    = (uint32_t)((ns[node]->send_buf.addr) & 0xffffffffULL);
              VPRw_int      = ns[node]->send_buf.rw;
              VPTicks_int   = ns[node]->send_buf.ticks;
              break;

            default:
              break;
        }

        debug_io_printf("VSched(): VPTicks=%08x\n", VPTicks_int);
    }

    debug_io_printf("VSched(): returning to simulation from node %d\n\n", node);

    // Export outputs over FLI
    *VPDataOut        = VPDataOut_int;
    *VPAddr           = VPAddr_int;
    *VPRw             = VPRw_int;
    *VPTicks          = VPTicks_int;

}

// -------------------------------------------------------------------------
// VTrans
// Main routine called whenever VTrans procedure invoked on
// clock edge of scheduled cycle.
//
// -------------------------------------------------------------------------

VPROC_RTN_TYPE VTrans (VTRANS_PARAMS)
{

    int VPDataOut_int,   VPAddr_int,   VPRw_int, VPTicks_int;
    int VPDataOutHi_int, VPAddrHi_int;

    // Sample inputs and update node state
    ns[node]->rcv_buf.data_in    = VPDataIn;
    ns[node]->rcv_buf.data_in_hi = VPDataInHi;
    ns[node]->rcv_buf.interrupt  = Interrupt;

    // Send message to VUser with VPDataIn value
    debug_io_printf("VTrans(): setting rcv[%d] semaphore\n", node);
    sem_post(&(ns[node]->rcv));

    // Wait for a message from VUser process with output data
    debug_io_printf("VTrans(): waiting for snd[%d] semaphore\n", node);
    sem_wait(&(ns[node]->snd));

    // Update outputs of VTrans procedure
    if (ns[node]->send_buf.ticks >= DELTA_CYCLE)
    {
        VPDataOut_int   = ((uint32_t*)ns[node]->send_buf.data)[0];
        VPDataOutHi_int = ((uint32_t*)ns[node]->send_buf.data)[4];
        VPAddr_int      = (uint32_t)((ns[node]->send_buf.addr)       & 0xffffffffULL);
        VPAddrHi_int    = (uint32_t)((ns[node]->send_buf.addr >> 32) & 0xffffffffULL);
        VPRw_int        = ns[node]->send_buf.rw;
        VPTicks_int     = ns[node]->send_buf.ticks;

        switch(ns[node]->send_buf.type)
        {

            case trans32_wr_byte:
            case trans32_rd_byte:
              *VPAddrWidth = 32;
              *VPDataWidth = 8;
              break;
            case trans32_wr_hword:
            case trans32_rd_hword:
              *VPAddrWidth = 32;
              *VPDataWidth = 16;
              break;
            case trans32_wr_word:
            case trans32_rd_word:
              *VPAddrWidth = 32;
              *VPDataWidth = 32;
              break;

            case trans64_wr_byte:
            case trans64_rd_byte:
              *VPAddrWidth = 64;
              *VPDataWidth = 8;
              break;
            case trans64_wr_hword:
            case trans64_rd_hword:
              *VPAddrWidth = 64;
              *VPDataWidth = 16;
              break;
            case trans64_wr_word:
            case trans64_rd_word:
              *VPAddrWidth = 64;
              *VPDataWidth = 32;
              break;
            case trans64_wr_dword:
            case trans64_rd_dword:
              *VPAddrWidth = 64;
              *VPDataWidth = 64;
              break;
              
            default:
              // Unsupported
              break;
        }

        debug_io_printf("VTrans(): VPTicks=%08x\n", VPTicks_int);
    }

    debug_io_printf("VTrans(): returning to simulation from node %d\n\n", node);

    // Export outputs over FLI
    *VPDataOut        = VPDataOut_int;
    *VPDataOutHi      = VPDataOutHi_int;
    *VPAddr           = VPAddr_int;
    *VPAddrHi         = VPAddrHi_int;
    *VPRw             = VPRw_int;
    *VPTicks          = VPTicks_int;

}

// -------------------------------------------------------------------------
// VUser()
//
// Calls a user registered function (if available) when
// $vprocuser(node) called in verilog
//
// -------------------------------------------------------------------------

VPROC_RTN_TYPE VProcUser(VPROCUSER_PARAMS)
{
    if (ns[node]->VUserCB != NULL)
    {
        (*(ns[node]->VUserCB))(value);
    }
}

