// =========================================================================
//
//  File Name:         OsvvmVUser.cpp
//  Design Unit Name:
//  Revision:          OSVVM MODELS STANDARD VERSION
//
//  Maintainer:        Simon Southwell email:  simon.southwell@gmail.com
//  Contributor(s):
//     Simon Southwell      simon.southwell@gmail.com
//
//
//  Description:
//      Simulator co-simulation virtual procedure C interface routine
//      definitions for user side code. Top level VP user thread routines.
//      Sets up connection to queue and calls relevant user function for
//      node number
//
//  Revision History:
//    Date      Version    Description
//    10/2022   2023.01    Initial revision
//
//
//  This file is part of OSVVM.
//
//  Copyright (c) 2022 by [OSVVM Authors](../AUTHORS.md)
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

#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <mutex>

extern "C"
{
#include "OsvvmVProc.h"
}
#include "OsvvmVUser.h"

static std::mutex acc_mx[VP_MAX_NODES];

#if defined ALDEC
#include <windows.h>

#define dlsym GetProcAddress
#define dlopen(_dll, _args) {LoadLibrary(_dll)}
#define dlerror() ""

typedef HINSTANCE symhdl_t;
#else
typedef void* symhdl_t;
#endif

// -------------------------------------------------------------------------
// VInitSendBuf()
//
// Initialise a send_buf_t record
//
// -------------------------------------------------------------------------
static void VInitSendBuf(send_buf_t &sbuf)
{
    sbuf.op              = NOT_DRIVEN;
    sbuf.type            = trans_idle;
    sbuf.prot            = 0;
    sbuf.num_burst_bytes = 0;
    sbuf.ticks           = 0;
    sbuf.done            = 0;
    sbuf.error           = 0;
}

// -------------------------------------------------------------------------
// VWaitOnFirstMessage()
//
// Wait on receive semaphore to indicate simulator is initialised and ready
// to accept transfers from user code.
//
// -------------------------------------------------------------------------
static void VWaitOnFirstMessage(const uint32_t node)
{
    int status;

    // Wait for first message from simulator
    DebugVPrint("VWaitForSim(): waiting for first message semaphore rcv[%d]\n", node);
    if ((status = sem_wait(&(ns[node]->rcv))) == -1)
    {
        printf("***Error: bad sem_post status (%d) on node %d (VUserInit)\n", status, node);
        exit(1);
    }
}

// -------------------------------------------------------------------------
// VUserInit()
//
// New thread initialisation procedure. Synchronises with
// simulation before calling user procedure.
//
// -------------------------------------------------------------------------

static void VUserInit (const int node)
{
    pVUserMain_t VUserMain_func;
    char funcname[DEFAULT_STR_BUF_SIZE];
    char vusersoname[DEFAULT_STR_BUF_SIZE];
    int status;

    DebugVPrint("VUserInit(%d)\n", node);

    VWaitOnFirstMessage(node);

    // Get function name of user entry routine
    sprintf(funcname, "%s%d",    "VUserMain", node);

#if !defined(ALDEC)
    sprintf(vusersoname, "./VUser.so");
#else
    sprintf(vusersoname, "./VProc.so");
#endif

    // Load user shared object to get handle to lookup VUsermain function symbols
    symhdl_t hdlvu = dlopen(vusersoname, RTLD_LAZY | RTLD_GLOBAL);

    if (hdlvu == NULL)
    {
        VPrint("***Error: failed to load VUser.so. %s\n", dlerror());
    }

    // Get the function pointer for the entry routine
    if ((VUserMain_func = (pVUserMain_t) dlsym(hdlvu, funcname)) == NULL)
    {
        printf("***Error: failed to find user code symbol %s (VUserInit)\n", funcname);
        exit(1);
    }

    DebugVPrint("VUserInit(): got user function (%s) for node %d (%p)\n", funcname, node, VUserMain_func);

    DebugVPrint("VUserInit(): calling user code for node %d\n", node);

    // Call user program
    DebugVPrint("VUserInit(): calling VUserMain%d\n", node);
    VUserMain_func();

    while(true);
}

// -------------------------------------------------------------------------
// VUser()
//
// Entry point for new user process. Creates a new thread
// calling VUserInit().
//
// -------------------------------------------------------------------------

extern "C" int VUser (const int node)
{
    pthread_t thread;
    int status;
    int idx, jdx;

    DebugVPrint("VUser(): node %d\n", node);

    // Interrupt callback initialisation
    ns[node]->VIntVecCB  = NULL;
    ns[node]->last_int   = 0;

    ns[node]->VUserCB = NULL;

    // Load VProc shared object to make symbols global
    symhdl_t hdlvp = dlopen("./VProc.so", RTLD_LAZY | RTLD_GLOBAL);

    if (hdlvp == NULL)
    {
        VPrint("***Error: failed to load VProc.so. %s\n", dlerror());
    }

    DebugVPrint("VUser(): initialised interrupt table node %d\n", node);

#ifndef DISABLE_VUSERMAIN_THREAD
    // Set off the user code thread
    if (status = pthread_create(&thread, NULL, (pThreadFunc_t)VUserInit, (void *)((long long)node)))
    {
        DebugVPrint("VUser(): pthread_create returned %d\n", status);
        return 1;
    }

    DebugVPrint("VUser(): spawned user thread for node %d\n", node);
#endif
    return 0;
}

// -------------------------------------------------------------------------
// VExch()
//
// Message exchange routine. Handles all messages to and from
// simulation process (apart from initialisation). Each sent
// message has a reply. Interrupt messages require that
// the original IO message reply is waited for again.
//
// -------------------------------------------------------------------------

static void VExch (psend_buf_t psbuf, prcv_buf_t prbuf, const uint32_t node)
{
    // Lock mutex as code is critical if accessed from multiple threads
    // for the same node.
    acc_mx[node].lock();

    int status;

    // Send message to simulator
    ns[node]->send_buf = *psbuf;
    DebugVPrint("VExch(): setting snd[%d] semaphore\n", node);

    if ((status = sem_post(&(ns[node]->snd))) == -1)
    {
        printf("***Error: bad sem_post status (%d) on node %d (VExch)\n", status, node);
        exit(1);
    }

    // Wait for response message from simulator
    DebugVPrint("VExch(): waiting for rcv[%d] semaphore\n", node);
    sem_wait(&(ns[node]->rcv));

    // Get the pointer to the receive response buffer
    *prbuf = ns[node]->rcv_buf;

    // Call user registered interrupt vector callback if the interrupt vector changes
    if ((prbuf->interrupt != ns[node]->last_int) && ns[node]->VIntVecCB != NULL)
    {
        psbuf->ticks = (*(ns[node]->VIntVecCB))(prbuf->interrupt);
    }

    ns[node]->last_int = prbuf->interrupt;

    // Unlock mutex
    acc_mx[node].unlock();

    DebugVPrint("VExch(): returning to user code from node %d\n", node);
}

// -------------------------------------------------------------------------
// VWaitForSim()
//
// Wait for the simulator to initialise and have sent the first message
//
// -------------------------------------------------------------------------

void VWaitForSim(const uint32_t node)
{
#ifdef DISABLE_VUSERMAIN_THREAD

    int count = 0;

    // Wait until the node's state is initialised (with a time out)
    while(ns[node] == NULL && count < FIVESEC_TIMEOUT)
    {
        usleep(HUNDRED_MILLISECS);
        count++;
    }

    usleep(HUNDRED_MILLISECS);

    // If timed out, generate an error
    if (count == FIVESEC_TIMEOUT)
    {
        VPrint("***ERROR: timed out waiting for simulation\n");
        exit(1);
    }
    else
    {
        // Wait for the first message from the simulator
        VWaitOnFirstMessage(node);
    }
#else
    // When running VUserMain in a thread is not disabled then do nothing
    return;
#endif

}

// -------------------------------------------------------------------------
// VTransWrite()
//
// Invokes an 8-bit write transaction exchange
//
// -------------------------------------------------------------------------

uint8_t VTransWrite (const uint32_t addr, const uint8_t data, const int prot, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = trans32_wr_byte;
    sbuf.addr            = addr;
    sbuf.prot            = prot;
    sbuf.op              = WRITE_OP;

    *((uint8_t*)sbuf.data) = data & 0xffU;

    VExch(&sbuf, &rbuf, node);

    return rbuf.data_in & 0xffU;
}

// -------------------------------------------------------------------------
// VTransRead()
//
// Invokes an 8-bit read transaction exchange
//
// -------------------------------------------------------------------------

void VTransRead (const uint32_t addr, uint8_t *rdata, const int prot, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = trans32_rd_byte;
    sbuf.addr            = addr;
    sbuf.prot            = prot;
    sbuf.op              = READ_OP;

    VExch(&sbuf, &rbuf, node);

    *rdata = rbuf.data_in & 0xffU;
}

// -------------------------------------------------------------------------
// VTransWrite()
//
// Invokes an 16-bit write transaction exchange
//
// -------------------------------------------------------------------------
uint16_t VTransWrite (const uint32_t addr, const uint16_t data, int const prot, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = trans32_wr_hword;
    sbuf.addr            = addr;
    sbuf.prot            = prot;
    sbuf.op              = WRITE_OP;

    *((uint16_t*)sbuf.data) = data & 0xffffU;

    VExch(&sbuf, &rbuf, node);

    return rbuf.data_in & 0xffffU;
}

// -------------------------------------------------------------------------
// VTransRead()
//
// Invokes an 16-bit write transaction exchange
//
// -------------------------------------------------------------------------

void VTransRead (const uint32_t addr, uint16_t *rdata, const int prot, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = trans32_rd_hword;
    sbuf.addr            = addr;
    sbuf.prot            = prot;
    sbuf.op              = READ_OP;

    VExch(&sbuf, &rbuf, node);

    *rdata = rbuf.data_in & 0xffffU;
}

// -------------------------------------------------------------------------
// VTransWrite()
//
// Invokes an 32-bit write transaction exchange
//
// -------------------------------------------------------------------------

uint32_t VTransWrite (const uint32_t addr, const uint32_t data, const int prot, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = trans32_wr_word;
    sbuf.addr            = addr;
    sbuf.prot            = prot;
    sbuf.op              = WRITE_OP;

    *((uint32_t*)sbuf.data) = data;

    VExch(&sbuf, &rbuf, node);

    return rbuf.data_in;
}

// -------------------------------------------------------------------------
// VTransRead()
//
// Invokes an 32-bit write transaction exchange
//
// -------------------------------------------------------------------------

void VTransRead (const uint32_t addr, uint32_t *rdata, const int prot, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = trans32_rd_word;
    sbuf.addr            = addr;
    sbuf.prot            = prot;
    sbuf.op              = READ_OP;

    VExch(&sbuf, &rbuf, node);

    *rdata = rbuf.data_in;
}

// -------------------------------------------------------------------------
// VTransWrite()
//
// Invokes an 8-bit write transaction exchange (64-bit address)
//
// -------------------------------------------------------------------------

uint8_t VTransWrite (const uint64_t addr, const uint8_t data, const int prot, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = trans64_wr_byte;
    sbuf.addr            = addr;
    sbuf.prot            = prot;
    sbuf.op              = WRITE_OP;

    *((uint8_t*)sbuf.data) = data & 0xffU;

    VExch(&sbuf, &rbuf, node);

    return rbuf.data_in & 0xffU;
}

// -------------------------------------------------------------------------
// VTransRead()
//
// Invokes an 8-bit read transaction exchange (64-bit address)
//
// -------------------------------------------------------------------------

void VTransRead (const uint64_t addr, uint8_t *rdata, const int prot, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = trans64_rd_byte;
    sbuf.addr            = addr;
    sbuf.prot            = prot;
    sbuf.op              = READ_OP;

    VExch(&sbuf, &rbuf, node);

    *rdata = rbuf.data_in & 0xffU;
}

// -------------------------------------------------------------------------
// VTransWrite()
//
// Invokes an 16-bit write transaction exchange (64-bit address)
//
// -------------------------------------------------------------------------

uint16_t VTransWrite (const uint64_t addr, const uint16_t data, const int prot, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = trans64_wr_hword;
    sbuf.addr            = addr;
    sbuf.prot            = prot;
    sbuf.op              = WRITE_OP;

    *((uint16_t*)sbuf.data) = data & 0xffffU;

    VExch(&sbuf, &rbuf, node);

    return rbuf.data_in & 0xffffU;
}

// -------------------------------------------------------------------------
// VTransRead()
//
// Invokes an 16-bit read transaction exchange (64-bit address)
//
// -------------------------------------------------------------------------

void VTransRead (const uint64_t addr, uint16_t *rdata, const int prot, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = trans64_rd_hword;
    sbuf.addr            = addr;
    sbuf.prot            = prot;
    sbuf.op              = READ_OP;

    VExch(&sbuf, &rbuf, node);

    *rdata = rbuf.data_in & 0xffffU;
}

// -------------------------------------------------------------------------
// VTransWrite()
//
// Invokes an 32-bit write transaction exchange (64-bit address)
//
// -------------------------------------------------------------------------

uint32_t VTransWrite (const uint64_t addr, const uint32_t data, const int prot, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = trans64_wr_word;
    sbuf.addr            = addr;
    sbuf.prot            = prot;
    sbuf.op              = WRITE_OP;

    *((uint32_t*)sbuf.data) = data;

    VExch(&sbuf, &rbuf, node);

    return rbuf.data_in;
}

// -------------------------------------------------------------------------
// VTransRead()
//
// Invokes an 32-bit read transaction exchange (64-bit address)
//
// -------------------------------------------------------------------------

void VTransRead (const uint64_t addr, uint32_t *rdata, const int prot, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = trans64_rd_word;
    sbuf.addr            = addr;
    sbuf.prot            = prot;
    sbuf.op              = READ_OP;

    VExch(&sbuf, &rbuf, node);

    *rdata = rbuf.data_in;
}

// -------------------------------------------------------------------------
// VTransWrite()
//
// Invokes an 64-bit write transaction exchange (64-bit address)
//
// -------------------------------------------------------------------------

uint64_t VTransWrite (const uint64_t addr, const uint64_t data, const int prot, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = trans64_wr_dword;
    sbuf.addr            = addr;
    sbuf.prot            = prot;
    sbuf.op              = WRITE_OP;

    *((uint64_t*)sbuf.data) = data;

    VExch(&sbuf, &rbuf, node);

    return (uint64_t)rbuf.data_in | ((uint64_t)rbuf.data_in_hi << 32);
}

// -------------------------------------------------------------------------
// VTransRead()
//
// Invokes an 64-bit read transaction exchange (64-bit address)
//
// -------------------------------------------------------------------------

void VTransRead (const uint64_t addr, uint64_t *rdata, const int prot, const uint32_t node)
{
    rcv_buf_t    rbuf;
    send_buf_t   sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = trans64_rd_dword;
    sbuf.addr            = addr;
    sbuf.prot            = prot;
    sbuf.op              = READ_OP;

    VExch(&sbuf, &rbuf, node);

    *rdata = (uint64_t)rbuf.data_in | ((uint64_t)rbuf.data_in_hi << 32);
}

// -------------------------------------------------------------------------
// VTransBurstWrite()
//
// Invokes a write burst transaction exchange (32-bit address)
// -------------------------------------------------------------------------

void VTransBurstWrite (const uint32_t addr, uint8_t* data, const int bytesize, const int prot, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = trans32_wr_burst;
    sbuf.addr            = addr;
    sbuf.prot            = prot;
    sbuf.op              = WRITE_BURST;
    sbuf.num_burst_bytes = bytesize % DATABUF_SIZE;

    for (int idx = 0; idx < sbuf.num_burst_bytes; idx++)
    {
        sbuf.databuf[idx] = data[idx];
    }

    VExch(&sbuf, &rbuf, node);

    return;
}

// -------------------------------------------------------------------------
// VTransBurstWrite()
//
// Invokes a write burst transaction exchange (64-bit address)
// -------------------------------------------------------------------------

void VTransBurstWrite (const uint64_t addr, uint8_t* data, const int bytesize, const int prot, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = trans64_wr_burst;
    sbuf.addr            = addr;
    sbuf.prot            = prot;
    sbuf.op              = WRITE_BURST;
    sbuf.num_burst_bytes = bytesize % DATABUF_SIZE;

    for (int idx = 0; idx < sbuf.num_burst_bytes; idx++)
    {
        sbuf.databuf[idx] = data[idx];
    }

    VExch(&sbuf, &rbuf, node);

    return;
}

// -------------------------------------------------------------------------
// VTransBurstRead()
//
// Invokes a read burst transaction exchange (32-bit address)
// -------------------------------------------------------------------------

void VTransBurstRead  (const uint32_t addr, uint8_t* data, const int bytesize, const int prot, const uint32_t node)
{
    rcv_buf_t    rbuf;
    send_buf_t   sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = trans32_rd_burst;
    sbuf.addr            = addr;
    sbuf.prot            = prot;
    sbuf.op              = READ_BURST;
    sbuf.num_burst_bytes = bytesize % DATABUF_SIZE;

    VExch(&sbuf, &rbuf, node);

    for (int idx = 0; idx < sbuf.num_burst_bytes; idx++)
    {
        data[idx] = rbuf.databuf[idx];
    }

    return;
}

// -------------------------------------------------------------------------
// VTransBurstRead()
//
// Invokes a read burst transaction exchange (64-bit address)
// -------------------------------------------------------------------------

void VTransBurstRead  (const uint64_t addr, uint8_t* data, const int bytesize, const int prot, const uint32_t node)
{
    rcv_buf_t    rbuf;
    send_buf_t   sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = trans64_rd_burst;
    sbuf.addr            = addr;
    sbuf.prot            = prot;
    sbuf.op              = READ_BURST;
    sbuf.num_burst_bytes = bytesize % DATABUF_SIZE;

    VExch(&sbuf, &rbuf, node);

    for (int idx = 0; idx < rbuf.num_burst_bytes; idx++)
    {
        data[idx] = rbuf.databuf[idx];
    }

    return;
}

// -------------------------------------------------------------------------
// VTick()
//
// Invokes a tick message exchange
//
// -------------------------------------------------------------------------

int VTick (const uint32_t ticks, const bool done, const bool error, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    // Ensure the tick loop executes at least once to allow donr and/or error to be
    // set with a tick argument of 0.
    int loops = ticks ? ticks : 1;

    for (int idx = 0; idx < loops; idx++)
    {
        // Loop, ticking once, to allow for interrupts to be registered while sleeping
        sbuf.ticks       = ticks ? 1 : 0;

        // Only flag the done and error status on the first tick.
        sbuf.done        = (done  && (idx == 0)) ? 1 : 0;
        sbuf.error       = (error && (idx == 0)) ? 1 : 0;

        // Operation is to wait for clock
        sbuf.op          = WAIT_FOR_CLOCK;

        // Exchange the tick command with simulator code
        VExch(&sbuf, &rbuf, node);
    }

    return 0;
}

// -------------------------------------------------------------------------
// VRegInterrupt()
//
// Registers a user function as an interrupt callback
//
// -------------------------------------------------------------------------

void VRegInterrupt (const pVUserInt_t func, const uint32_t node)
{
    DebugVPrint("VRegInterrupt(): at node %d, registering vector interrupt callback\n", node);

    ns[node]->VIntVecCB = func;
}

// -------------------------------------------------------------------------
// VSetTestName()
//
// Set the tests name to the string of characters in data
// -------------------------------------------------------------------------

void VSetTestName (const char* data, const int bytesize, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = trans_idle;
    sbuf.op              = SET_TEST_NAME;
    sbuf.num_burst_bytes = bytesize % DATABUF_SIZE;

    for (int idx = 0; idx < sbuf.num_burst_bytes; idx++)
    {
        sbuf.databuf[idx] = data[idx];
    }

    VExch(&sbuf, &rbuf, node);

    return;
}

