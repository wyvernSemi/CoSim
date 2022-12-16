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
pSchedState_t ns[VP_MAX_NODES] = { [0 ... VP_MAX_NODES-1] = NULL};

/////////////////////////////////////////////////////////////
// Main routine called whenever $vinit task invoked from
// initial block of VProc module.
//
VPROC_RTN_TYPE VInit (VINIT_PARAMS)
{

    VPrint("VInit()\n");

    // Range check node number
    if (node < 0 || node >= VP_MAX_NODES)
    {
        VPrint("***Error: VInit() got out of range node number (%d)\n", node);
        exit(VP_USER_ERR);
    }

    DebugVPrint("VInit(): node = %d\n", node);

    // Allocate some space for the node state and update pointer
    ns[node] = (pSchedState_t) malloc(sizeof(SchedState_t));

    // Set up semaphores for this node
    DebugVPrint("VInit(): initialising semaphores for node %d\n", node);

    if (sem_init(&(ns[node]->snd), 0, 0) == -1)
    {
        VPrint("***Error: VInit() failed to initialise semaphore\n");
        exit(1);
    }
    if (sem_init(&(ns[node]->rcv), 0, 0) == -1)
    {
        VPrint("***Error: VInit() failed to initialise semaphore\n");
        exit(1);
    }

    DebugVPrint("VInit(): initialising semaphores for node %d---Done\n", node);

    // Issue a new thread to run the user code
    VUser(node);
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
    DebugVPrint("VSched(): setting rcv[%d] semaphore\n", node);
    sem_post(&(ns[node]->rcv));

    // Wait for a message from VUser process with output data
    DebugVPrint("VSched(): waiting for snd[%d] semaphore\n", node);
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

        DebugVPrint("VSched(): VPTicks=%08x\n", VPTicks_int);
    }

    DebugVPrint("VSched(): returning to simulation from node %d\n\n", node);

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
    int VPDataOutHi_int, VPAddrHi_int, VPBurstSize_int, VPDone_int;
    int VPError_int;

    // Sample inputs and update node state
    if (ns[node]->send_buf.type != trans32_rd_burst && ns[node]->send_buf.type != trans32_wr_burst)
    {
        ns[node]->rcv_buf.data_in    = VPDataIn;
        ns[node]->rcv_buf.data_in_hi = VPDataInHi;
    }
    else
    {
        ns[node]->rcv_buf.num_burst_bytes = ns[node]->send_buf.num_burst_bytes;
    }
    ns[node]->rcv_buf.interrupt  = Interrupt;

    // Send message to VUser with VPDataIn value
    DebugVPrint("VTrans(): setting rcv[%d] semaphore\n", node);
    sem_post(&(ns[node]->rcv));

    // Wait for a message from VUser process with output data
    DebugVPrint("VTrans(): waiting for snd[%d] semaphore\n", node);
    sem_wait(&(ns[node]->snd));

    // Update outputs of VTrans procedure
    if (ns[node]->send_buf.ticks >= DELTA_CYCLE)
    {
        VPDataOut_int   = ((uint32_t*)ns[node]->send_buf.data)[0];
        VPDataOutHi_int = ((uint32_t*)ns[node]->send_buf.data)[4];
        VPAddr_int      = (uint32_t)((ns[node]->send_buf.addr)       & 0xffffffffULL);
        VPAddrHi_int    = (uint32_t)((ns[node]->send_buf.addr >> 32) & 0xffffffffULL);
        VPRw_int        = ns[node]->send_buf.rw;
        VPBurstSize_int = ns[node]->send_buf.num_burst_bytes;
        VPTicks_int     = ns[node]->send_buf.ticks;
        VPDone_int      = ns[node]->send_buf.done;
        VPError_int     = ns[node]->send_buf.error;

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
            case trans32_wr_burst:
            case trans32_rd_burst:
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
            case trans64_wr_burst:
            case trans64_rd_burst:
              *VPAddrWidth = 64;
              *VPDataWidth = 64;
              break;
              
            default:
              // Unsupported
              break;
        }

        DebugVPrint("VTrans(): VPTicks=%08x\n", VPTicks_int);
    }

    DebugVPrint("VTrans(): returning to simulation from node %d\n\n", node);
    
    DebugVPrint("===> addr=%08x rnw=%d burst=%d ticks=%d\n", VPAddr_int, VPRw_int, VPBurstSize_int, VPTicks_int);

    // Export outputs over FLI
    *VPDataOut        = VPDataOut_int;
    *VPDataOutHi      = VPDataOutHi_int;
    *VPAddr           = VPAddr_int;
    *VPAddrHi         = VPAddrHi_int;
    *VPRw             = VPRw_int;
    *VPBurstSize      = VPBurstSize_int;
    *VPTicks          = VPTicks_int;
    *VPDone           = VPDone_int;
    *VPError          = VPError_int;

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

// -------------------------------------------------------------------------
// VSetBurstByte()
//
// -------------------------------------------------------------------------

VPROC_RTN_TYPE VSetBurstRdByte(VSETBURSTRDBYTE_PARAMS)
{
    ns[node]->rcv_buf.databuf[idx % DATABUF_SIZE] = data;
}

// -------------------------------------------------------------------------
// VGetBurstWrByte()
//
// -------------------------------------------------------------------------

VPROC_RTN_TYPE VGetBurstWrByte(VGETBURSTWRBYTE_PARAMS)
{
    *data = ns[node]->send_buf.databuf[idx % DATABUF_SIZE];
}

