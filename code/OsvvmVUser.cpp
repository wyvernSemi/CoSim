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
//    05/2023   2023.05    Adding support for Async, Check and Try functionality
//    04/2023   2023.04    Adding basic stream support
//    01/2023   2023.01    Initial revision
//
//
//  This file is part of OSVVM.
//
//  Copyright (c) 2023 by [OSVVM Authors](../AUTHORS.md)
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

// -------------------------------------------------------------------------
// INCLUDES
// -------------------------------------------------------------------------

#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <mutex>

extern "C"
{
#include "OsvvmVProc.h"
}
#include "OsvvmVUser.h"

#if defined(ALDEC)

# include <windows.h>

// -------------------------------------------------------------------------
// DEFINES AND MACROS
// -------------------------------------------------------------------------

// Map Linux dynamic laoding calls to Windows equivalents
# define dlsym GetProcAddress
# define dlopen(_dll, _args) {LoadLibrary(_dll)}
# define dlerror() ""
# define dlclose FreeLibrary

// -------------------------------------------------------------------------
// TYPEDEFS
// -------------------------------------------------------------------------

// Aldec seems to doesn't free mutexes unless deleted, so make pointers
typedef HINSTANCE symhdl_t;
static std::mutex *acc_mx[VP_MAX_NODES];
#else
typedef void* symhdl_t;
static std::mutex acc_mx[VP_MAX_NODES];
#endif

#if defined (ACTIVEHDL) || defined(SIEMENS)
static symhdl_t hdlvp;
#endif

// -------------------------------------------------------------------------
// LOCAL STATICS
// -------------------------------------------------------------------------

static int unusedstatus;

// -------------------------------------------------------------------------
// FUNCTION DEFINITIONS
// -------------------------------------------------------------------------

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
    sbuf.addr            = 0;
    sbuf.prot            = 0;
    sbuf.num_burst_bytes = 0;
    sbuf.ticks           = 0;
    sbuf.done            = 0;
    sbuf.error           = 0;
    sbuf.param           = 0;
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
    symhdl_t hdlvu;

    DebugVPrint("VUserInit(%d)\n", node);

    VWaitOnFirstMessage(node);

    // Get function name of user entry routine
    sprintf(funcname, "%s%d",    "VUserMain", node);

#if defined(ALDEC)
    // Create a new mutex for this node in ALDEC
    acc_mx[node] = new std::mutex;
#endif

#if defined(ACTIVEHDL)
    // No separate user DLL under Active-HDL so simply use the VProc.so handle
    hdlvu = hdlvp;
#else
    sprintf(vusersoname, "./VUser.so");
    // Load user shared object to get handle to lookup VUsermain function symbols
    hdlvu = dlopen(vusersoname, RTLD_LAZY | RTLD_GLOBAL);

    if (hdlvu == NULL)
    {
        VPrint("***Error: failed to load VUser.so. %s\n", dlerror());
    }
#endif

    // Get the function pointer for the entry routine
    if ((VUserMain_func = (pVUserMain_t) dlsym(hdlvu, funcname)) == NULL)
    {
        printf("***Error: failed to find user code symbol %s (VUserInit)\n", funcname);
        exit(1);
    }

#if defined(ACTIVEHDL) || defined(SIEMENS)
    // Close the VProc.so handle to decrement the count, incremented with the open
    dlclose(hdlvp);
#endif

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

    DebugVPrint("VUser(): initialised interrupt table node %d\n", node);

#if defined(ACTIVEHDL) || defined (SIEMENS)
    // Load VProc shared object to make symbols global
    hdlvp = dlopen("./VProc.so", RTLD_LAZY | RTLD_GLOBAL);

    if (hdlvp == NULL)
    {
        VPrint("***Error: failed to load VProc.so. %s\n", dlerror());
    }
#endif


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
#if defined(ALDEC)
    acc_mx[node]->lock();
#else
    acc_mx[node].lock();
#endif

    int status;

    // Send message to simulator
    ns[node]->send_buf = *psbuf;
    DebugVPrint("VExch(): setting snd[%d] semaphore\n", node);

    if ((status = sem_post(&(ns[node]->snd))) == -1)
    {
        printf("***Error: bad sem_post status (%d) on node %d (VExch)\n", status, node);
        exit(1);
    }

    // If this is the last message from the user code
    // unlock/destroy the mutex, as GUI runs seem to
    // hold on to the mutex state which hangs a simulation
    // on subsequent runs.
    if (ns[node]->send_buf.done)
    {
#if defined(ALDEC)
        delete acc_mx[node];
#else
        acc_mx[node].unlock();
#endif
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
#if defined(ALDEC)
    acc_mx[node]->unlock();
#else
    acc_mx[node].unlock();
#endif

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
// VTransCommon()
//
// Common 8-bit byte transaction exchange function (32-bit address)
//
// -------------------------------------------------------------------------

static uint8_t VTransCommon (const addr_bus_trans_op_t op, const uint32_t addr, const uint8_t data, int* status, const int prot, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = trans32_byte;
    sbuf.addr            = addr;
    sbuf.prot            = prot;
    sbuf.op              = op;

    *((uint8_t*)sbuf.data) = data & 0xffU;

    VExch(&sbuf, &rbuf, node);

    *status = rbuf.status;

    return rbuf.data_in & 0xffU;
}

// -------------------------------------------------------------------------
// VTransCommon()
//
// Common 16-bit word transaction exchange function (32-bit address)
//
// -------------------------------------------------------------------------
static uint16_t VTransCommon (const addr_bus_trans_op_t op, const uint32_t addr, const uint16_t data,  int* status, int const prot, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = trans32_hword;
    sbuf.addr            = addr;
    sbuf.prot            = prot;
    sbuf.op              = op;

    *((uint16_t*)sbuf.data) = data & 0xffffU;

    VExch(&sbuf, &rbuf, node);

    *status = rbuf.status;

    return rbuf.data_in & 0xffffU;
}

// -------------------------------------------------------------------------
// VTransCommon()
//
// Common 32-bit word transaction exchange function (32-bit address)
//
// -------------------------------------------------------------------------

static uint32_t VTransCommon (const addr_bus_trans_op_t op, const uint32_t addr, const uint32_t data, int* status,  const int prot, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = trans32_word;
    sbuf.addr            = addr;
    sbuf.prot            = prot;
    sbuf.op              = op;

    *((uint32_t*)sbuf.data) = data;

    VExch(&sbuf, &rbuf, node);

    *status = rbuf.status;

    return rbuf.data_in;
}

// -------------------------------------------------------------------------
// VTransCommon()
//
// Common 8-bit word transaction exchange function (64-bit address)
//
// -------------------------------------------------------------------------

static uint8_t VTransCommon (const addr_bus_trans_op_t op, const uint64_t addr, const uint8_t data, int* status, const int prot, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = trans64_byte;
    sbuf.addr            = addr;
    sbuf.prot            = prot;
    sbuf.op              = op;

    *((uint8_t*)sbuf.data) = data & 0xffU;

    VExch(&sbuf, &rbuf, node);

    *status = rbuf.status;

    return rbuf.data_in & 0xffU;
}

// -------------------------------------------------------------------------
// VTransCommon()
//
// Common 16-bit word transaction exchange function (64-bit address)
//
// -------------------------------------------------------------------------

static uint16_t VTransCommon (const addr_bus_trans_op_t op, const uint64_t addr, const uint16_t data, int* status, const int prot, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = trans64_hword;
    sbuf.addr            = addr;
    sbuf.prot            = prot;
    sbuf.op              = op;

    *((uint16_t*)sbuf.data) = data & 0xffffU;

    VExch(&sbuf, &rbuf, node);

    *status = rbuf.status;

    return rbuf.data_in & 0xffffU;
}

// -------------------------------------------------------------------------
// VTransCommon()
//
// Common 32-bit word transaction exchange function (64-bit address)
//
// -------------------------------------------------------------------------

static uint32_t VTransCommon (const addr_bus_trans_op_t op, const uint64_t addr, const uint32_t data, int* status, const int prot, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = trans64_word;
    sbuf.addr            = addr;
    sbuf.prot            = prot;
    sbuf.op              = op;

    *((uint32_t*)sbuf.data) = data;

    VExch(&sbuf, &rbuf, node);

    *status = rbuf.status;

    return rbuf.data_in;
}

// -------------------------------------------------------------------------
// VTransCommon()
//
// Common 64-bit word transaction exchange function (64-bit address)
//
// -------------------------------------------------------------------------

static uint64_t VTransCommon (const addr_bus_trans_op_t op, const uint64_t addr, const uint64_t data, int* status, const int prot, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = trans64_dword;
    sbuf.addr            = addr;
    sbuf.prot            = prot;
    sbuf.op              = op;

    *((uint64_t*)sbuf.data) = data;

    VExch(&sbuf, &rbuf, node);

    *status = rbuf.status;

    return (uint64_t)rbuf.data_in | ((uint64_t)rbuf.data_in_hi << 32);
}

// -------------------------------------------------------------------------
// VTransBurstCommon()
//
// Common burst transaction exchange function (32-bit address)
//
// -------------------------------------------------------------------------

static void VTransBurstCommon (const addr_bus_trans_op_t op, const int param, const uint32_t addr, uint8_t* data, const int bytesize, const int prot, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = trans32_burst;
    sbuf.addr            = addr;
    sbuf.prot            = prot;
    sbuf.op              = op;
    sbuf.param           = param;
    sbuf.num_burst_bytes = bytesize % DATABUF_SIZE;

    // Flag when a FIFO fill (or check) operation
    bool is_fill = param == BURST_INCR || param == BURST_INCR_PUSH || param == BURST_INCR_CHECK ||
                   param == BURST_RAND || param == BURST_RAND_PUSH || param == BURST_RAND_CHECK;

    // The number of write bytes is either 1, when a fill/check operation (with first bytes),
    // or none when a pure burst transaction or the same as the bytesize value.
    int num_of_wr_bytes = is_fill ? 1 : (param == BURST_TRANS) ? 0 : sbuf.num_burst_bytes;

    for (int idx = 0; idx < num_of_wr_bytes; idx++)
    {
        sbuf.databuf[idx] = data[idx];
    }

    VExch(&sbuf, &rbuf, node);

    if (op == READ_BURST && param != BURST_TRANS && !is_fill)
    {
        for (int idx = 0; idx < sbuf.num_burst_bytes; idx++)
        {
            data[idx] = rbuf.databuf[idx];
        }
    }

    return;
}

// -------------------------------------------------------------------------
// VTransBurstCommon()
//
// Common burst transaction exchange function (64-bit address)
//
// -------------------------------------------------------------------------

static void VTransBurstCommon (const addr_bus_trans_op_t op, const int param, const uint64_t addr, uint8_t* data, const int bytesize, const int prot, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = trans64_burst;
    sbuf.addr            = addr;
    sbuf.prot            = prot;
    sbuf.op              = op;
    sbuf.param           = param;
    sbuf.num_burst_bytes = bytesize % DATABUF_SIZE;

    // Flag when a FIFO fill (or check) operation
    bool is_fill = param == BURST_INCR || param == BURST_INCR_PUSH || param == BURST_INCR_CHECK ||
                   param == BURST_RAND || param == BURST_RAND_PUSH || param == BURST_RAND_CHECK;

    // The number of write bytes is either 1, when a fill/check operation (with first bytes),
    // or none when a pure burst transaction or the same as the bytesize value.
    int num_of_wr_bytes = is_fill ? 1  : (param == BURST_TRANS) ? 0 : sbuf.num_burst_bytes;

    for (int idx = 0; idx < num_of_wr_bytes; idx++)
    {
        sbuf.databuf[idx] = data[idx];
    }

    VExch(&sbuf, &rbuf, node);

    if (op == READ_BURST && param != BURST_TRANS)
    {
        for (int idx = 0; idx < sbuf.num_burst_bytes; idx++)
        {
            data[idx] = rbuf.databuf[idx];
        }
    }

    return;
}

// -------------------------------------------------------------------------
// VStreamCommon()
//
// Common 8-bit byte stream send/check transaction exchange function
//
// -------------------------------------------------------------------------

static uint8_t VStreamCommon (const stream_operation_t op, const uint8_t data, const int param, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = stream_snd_byte;
    sbuf.op              = (addr_bus_trans_op_t)op;
    sbuf.param           = param;

    *((uint8_t*)sbuf.data) = data & 0xffU;

    VExch(&sbuf, &rbuf, node);

    return rbuf.data_in & 0xffU;
}

// -------------------------------------------------------------------------
// VStreamGetCommon()
//
// Common 8-bit byte stream get transaction exchange
//
// -------------------------------------------------------------------------

static bool VStreamGetCommon (int op, uint8_t *rdata, int *status, const uint8_t wdata, const int param, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = stream_get_byte;
    sbuf.op              = (addr_bus_trans_op_t)op;
    sbuf.param           = param;

    *((uint8_t*)sbuf.data) = wdata & 0xffU;

    VExch(&sbuf, &rbuf, node);

    if (op != TRY_CHECK)
    {
        *status = rbuf.status;
        *rdata = rbuf.data_in & 0xffU;
    }

    // Return available status (sent back in unused interrupt field)
    return rbuf.interrupt;
}

// -------------------------------------------------------------------------
// VStreamCommon()
//
// Common 16-bit word stream send/check transaction exchange function
//
// -------------------------------------------------------------------------

static uint16_t VStreamCommon (const stream_operation_t op, const uint16_t data, const int param, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = stream_snd_hword;
    sbuf.op              = (addr_bus_trans_op_t)op;
    sbuf.param           = param;

    *((uint16_t*)sbuf.data) = data & 0xffffU;

    VExch(&sbuf, &rbuf, node);

    return rbuf.data_in & 0xffffU;
}

// -------------------------------------------------------------------------
// VStreamGetCommon()
//
// Common 16-bit word stream get transaction get exchange function
//
// -------------------------------------------------------------------------

static bool VStreamGetCommon (int op, uint16_t *rdata, int *status, const uint16_t wdata, const int param, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = stream_get_hword;
    sbuf.op              = (addr_bus_trans_op_t)op;
    sbuf.param           = param;

    *((uint16_t*)sbuf.data) = wdata & 0xffffU;

    VExch(&sbuf, &rbuf, node);

    if (op != TRY_CHECK)
    {
        *status = rbuf.status;
        *rdata = rbuf.data_in & 0xffffU;
    }

    // Return available status (sent back in unused interrupt field)
    return rbuf.interrupt;
}

// -------------------------------------------------------------------------
// VStreamCommon()
//
// Common 32-bit word stream send/check transaction exchange function
//
// -------------------------------------------------------------------------

static uint32_t VStreamCommon (const stream_operation_t op, const uint32_t data, const int param, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = stream_snd_word;
    sbuf.op              = (addr_bus_trans_op_t)op;
    sbuf.param           = param;

    *((uint32_t*)sbuf.data) = data & 0xffffffffU;

    VExch(&sbuf, &rbuf, node);

    return rbuf.data_in & 0xffffffffU;
}


// -------------------------------------------------------------------------
// VStreamGetCommon()
//
// Common 32-bit word stream get transaction exchange function
//
// -------------------------------------------------------------------------

static bool VStreamGetCommon (int op, uint32_t *rdata, int *status, const uint32_t wdata, const int param, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = stream_get_word;
    sbuf.op              = (addr_bus_trans_op_t)op;
    sbuf.param           = param;

    *((uint32_t*)sbuf.data) = wdata & 0xffffffffU;

    VExch(&sbuf, &rbuf, node);

    if (op != TRY_CHECK)
    {
        *status = rbuf.status;
        *rdata = rbuf.data_in & 0xffffffffU;
    }

    // Return available status (sent back in unused interrupt field)
    return rbuf.interrupt;
}

// -------------------------------------------------------------------------
// VStreamCommon()
//
// Common 64-bit word send/check transaction exchange function
//
// -------------------------------------------------------------------------

static uint64_t VStreamCommon (const stream_operation_t op, const uint64_t data, const int param, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = stream_snd_dword;
    sbuf.op              = (addr_bus_trans_op_t)op;
    sbuf.param           = param;

    *((uint64_t*)sbuf.data) = data;

    VExch(&sbuf, &rbuf, node);

    return (uint64_t)rbuf.data_in | ((uint64_t)rbuf.data_in_hi << 32);
}

// -------------------------------------------------------------------------
// VStreamGetCommon()
//
// Common 64-bit word stream get transaction exchange function
//
// -------------------------------------------------------------------------

static bool VStreamGetCommon (int op, uint64_t *rdata, int *status, const uint64_t wdata, const int param,  const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = stream_get_dword;
    sbuf.op              = (addr_bus_trans_op_t)op;

    VExch(&sbuf, &rbuf, node);

    *((uint64_t*)sbuf.data) = wdata;


    if (op != TRY_CHECK)
    {
        *status = rbuf.status;
        *rdata  = (uint64_t)rbuf.data_in | ((uint64_t)rbuf.data_in_hi << 32);
    }

    // Return available status (sent back in unused interrupt field)
    return rbuf.interrupt;
}

// -------------------------------------------------------------------------
// VStreamBurstSendCommon()
//
// Common function for Send/Check related stream transactions
// -------------------------------------------------------------------------

static bool VStreamBurstSendCommon (const stream_operation_t op, const int burst_type, uint8_t* data, const int bytesize, const int param, const uint32_t node)
{
    rcv_buf_t  rbuf;
    send_buf_t sbuf;

    VInitSendBuf(sbuf);

    sbuf.type               = stream_snd_burst;
    sbuf.op                 = (addr_bus_trans_op_t)op;
    sbuf.num_burst_bytes    = bytesize % DATABUF_SIZE;
    sbuf.param              = param;
    *((uint32_t*)sbuf.data) = burst_type; // Re-use data field of send buffer for burst sub-operation

    // The number of write bytes is either 1, when a fill/check operation (with first bytes),
    // or none when a pure burst transaction or the same as the bytesize value.
    int num_of_wr_bytes = (burst_type == BURST_TRANS)                         ? 0 :
                          (op == TRY_CHECK_BURST && burst_type != BURST_NORM) ? 1 :
                                                                                sbuf.num_burst_bytes;

    for (int idx = 0; idx < num_of_wr_bytes; idx++)
    {
        sbuf.databuf[idx] = data[idx];
    }

    VExch(&sbuf, &rbuf, node);


    // Return available status (sent back in unused interrupt field)
    return rbuf.interrupt;
}

// -------------------------------------------------------------------------
// VStreamBurstGetCommon()
//
// Common function for Get related stream transactions
// -------------------------------------------------------------------------

static bool VStreamBurstGetCommon (const int op, const int param, uint8_t* data, const int bytesize, int* status, const uint32_t node)
{
    rcv_buf_t    rbuf;
    send_buf_t   sbuf;

    VInitSendBuf(sbuf);

    sbuf.type            = stream_get_burst;
    sbuf.op              = (addr_bus_trans_op_t)op;
    sbuf.num_burst_bytes = bytesize % DATABUF_SIZE;
    sbuf.param           = param;

    VExch(&sbuf, &rbuf, node);

    *status = rbuf.status;

    // Return data for normal/data transactions, but only if not a try with none available
    if ((param == BURST_NORM || param == BURST_DATA) && !((stream_operation_t)sbuf.op == TRY_GET_BURST && !rbuf.interrupt))
    {
        for (int idx = 0; idx < sbuf.num_burst_bytes; idx++)
        {
            data[idx] = rbuf.databuf[idx];
        }
    }

    // Return available status (sent back in unused interrupt field)
    return rbuf.interrupt;
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

// -------------------------------------------------------------------------
// Wrapper functions for the transaction operations for the various
// overloaded versions, calling common routines with different operation
// codes.
// -------------------------------------------------------------------------

void VTransRead (const uint32_t addr, uint8_t *rdata, const int prot, const uint32_t node)
{
    *rdata = VTransCommon(READ_OP, addr, (uint8_t)0, &unusedstatus, prot, node);
}

void VTransRead (const uint32_t addr, uint16_t *rdata, const int prot, const uint32_t node)
{
    *rdata = VTransCommon(READ_OP, addr, (uint16_t)0, &unusedstatus, prot, node);
}

void VTransRead (const uint32_t addr, uint32_t *rdata, const int prot, const uint32_t node)
{
    *rdata = VTransCommon(READ_OP, addr, (uint32_t)0, &unusedstatus, prot, node);
}

void VTransRead (const uint64_t addr, uint8_t *rdata, const int prot, const uint32_t node)
{
    *rdata = VTransCommon(READ_OP, addr, (uint8_t)0, &unusedstatus, prot, node);
}

void VTransRead (const uint64_t addr, uint16_t *rdata, const int prot, const uint32_t node)
{
    *rdata = VTransCommon(READ_OP, addr, (uint16_t)0, &unusedstatus, prot, node);
}

void VTransRead (const uint64_t addr, uint32_t *rdata, const int prot, const uint32_t node)
{
    *rdata = VTransCommon(READ_OP, addr, (uint32_t)0, &unusedstatus, prot, node);
}

void VTransRead (const uint64_t addr, uint64_t *rdata, const int prot, const uint32_t node)
{
    *rdata = VTransCommon(READ_OP, addr, (uint64_t)0, &unusedstatus, prot, node);
}

uint8_t VTransWrite (const uint32_t addr, const uint8_t data, const int prot, const uint32_t node)
{
    return VTransCommon(WRITE_OP, addr, data, &unusedstatus, prot, node);
}

uint8_t VTransWriteAsync (const uint32_t addr, const uint8_t data, const int prot, const uint32_t node)
{
    return VTransCommon(ASYNC_WRITE, addr, data, &unusedstatus, prot, node);
}

uint16_t VTransWrite (const uint32_t addr, const uint16_t data, const int prot, const uint32_t node)
{
    return VTransCommon(WRITE_OP, addr, data, &unusedstatus, prot, node);
}

uint16_t VTransWriteAsync (const uint32_t addr, const uint16_t data, const int prot, const uint32_t node)
{
    return VTransCommon(ASYNC_WRITE, addr, data, &unusedstatus, prot, node);
}

uint32_t VTransWrite (const uint32_t addr, const uint32_t data, const int prot, const uint32_t node)
{
    return VTransCommon(WRITE_OP, addr, data, &unusedstatus, prot, node);
}

uint32_t VTransWriteAsync (const uint32_t addr, const uint32_t data, const int prot, const uint32_t node)
{
    return VTransCommon(ASYNC_WRITE, addr, data, &unusedstatus, prot, node);
}

uint8_t VTransWrite (const uint64_t addr, const uint8_t data, const int prot, const uint32_t node)
{
    return VTransCommon(WRITE_OP, addr, data, &unusedstatus, prot, node);
}

uint8_t VTransWriteAsync (const uint64_t addr, const uint8_t data, const int prot, const uint32_t node)
{
    return VTransCommon(ASYNC_WRITE, addr, data, &unusedstatus, prot, node);
}

uint16_t VTransWrite (const uint64_t addr, const uint16_t data, const int prot, const uint32_t node)
{
    return VTransCommon(WRITE_OP, addr, data, &unusedstatus, prot, node);
}

uint16_t VTransWriteAsync (const uint64_t addr, const uint16_t data, const int prot, const uint32_t node)
{
    return VTransCommon(ASYNC_WRITE, addr, data, &unusedstatus, prot, node);
}

uint32_t VTransWrite (const uint64_t addr, const uint32_t data, const int prot, const uint32_t node)
{
    return VTransCommon(WRITE_OP, addr, data, &unusedstatus, prot, node);
}

uint32_t VTransWriteAsync (const uint64_t addr, const uint32_t data, const int prot, const uint32_t node)
{
     return VTransCommon(ASYNC_WRITE, addr, data, &unusedstatus, prot, node);
}

uint64_t VTransWrite (const uint64_t addr, const uint64_t data, const int prot, const uint32_t node)
{
    return VTransCommon(WRITE_OP, addr, data, &unusedstatus, prot, node);
}

uint64_t VTransWriteAsync (const uint64_t addr, const uint64_t data, const int prot, const uint32_t node)
{
    return VTransCommon(ASYNC_WRITE, addr, data, &unusedstatus, prot, node);
}

uint8_t VTransWriteAndRead (const uint32_t addr, const uint8_t data, const int prot, const uint32_t node)
{
    return VTransCommon(WRITE_AND_READ, addr, data, &unusedstatus, prot, node);
}

uint8_t VTransWriteAndReadAsync (const uint32_t addr, const uint8_t data, const int prot, const uint32_t node)
{
    return VTransCommon(ASYNC_WRITE_AND_READ, addr, data, &unusedstatus, prot, node);
}

uint16_t VTransWriteAndRead (const uint32_t addr, const uint16_t data, const int prot, const uint32_t node)
{
    return VTransCommon(WRITE_AND_READ, addr, data, &unusedstatus, prot, node);
}

uint16_t VTransWriteAndReadAsync (const uint32_t addr, const uint16_t data, const int prot, const uint32_t node)
{
    return VTransCommon(ASYNC_WRITE_AND_READ, addr, data, &unusedstatus, prot, node);
}

uint32_t VTransWriteAndRead (const uint32_t addr, const uint32_t data, const int prot, const uint32_t node)
{
    return VTransCommon(WRITE_AND_READ, addr, data, &unusedstatus, prot, node);
}

uint32_t VTransWriteAndReadAsync (const uint32_t addr, const uint32_t data, const int prot, const uint32_t node)
{
    return VTransCommon(ASYNC_WRITE_AND_READ, addr, data, &unusedstatus, prot, node);
}

uint8_t VTransWriteAndRead (const uint64_t addr, const uint8_t data, const int prot, const uint32_t node)
{
    return VTransCommon(WRITE_AND_READ, addr, data, &unusedstatus, prot, node);
}

uint8_t VTransWriteAndReadAsync (const uint64_t addr, const uint8_t data, const int prot, const uint32_t node)
{
    return VTransCommon(ASYNC_WRITE_AND_READ, addr, data, &unusedstatus, prot, node);
}

uint16_t VTransWriteAndRead (const uint64_t addr, const uint16_t data, const int prot, const uint32_t node)
{
    return VTransCommon(WRITE_AND_READ, addr, data, &unusedstatus, prot, node);
}

uint16_t VTransWriteAndReadAsync (const uint64_t addr, const uint16_t data, const int prot, const uint32_t node)
{
    return VTransCommon(ASYNC_WRITE_AND_READ, addr, data, &unusedstatus, prot, node);
}

uint32_t VTransWriteAndRead (const uint64_t addr, const uint32_t data, const int prot, const uint32_t node)
{
    return VTransCommon(WRITE_AND_READ, addr, data, &unusedstatus, prot, node);
}

uint32_t VTransWriteAndReadAsync (const uint64_t addr, const uint32_t data, const int prot, const uint32_t node)
{
    return VTransCommon(ASYNC_WRITE_AND_READ, addr, data, &unusedstatus, prot, node);
}

uint64_t VTransWriteAndRead (const uint64_t addr, const uint64_t data, const int prot, const uint32_t node)
{
    return VTransCommon(WRITE_AND_READ, addr, data, &unusedstatus, prot, node);
}

uint64_t VTransWriteAndReadAsync (const uint64_t addr, const uint64_t data, const int prot, const uint32_t node)
{
    return VTransCommon(ASYNC_WRITE_AND_READ, addr, data, &unusedstatus, prot, node);
}

void VTransWriteAddressAsync (const uint32_t addr, const uint32_t node)
{
    VTransCommon(ASYNC_WRITE_ADDRESS, addr, (uint32_t)0, &unusedstatus, 0, node);
}

void VTransWriteAddressAsync (const uint64_t addr, const uint32_t node)
{
    VTransCommon(ASYNC_WRITE_ADDRESS, addr, (uint32_t)0, &unusedstatus, 0, node);
}

void VTransWriteDataAsync (const uint8_t  data, const uint32_t bytelane, const uint32_t node)
{
    VTransCommon(ASYNC_WRITE_DATA, bytelane, data, &unusedstatus, 0, node);
}

void VTransWriteDataAsync (const uint16_t data, const uint32_t bytelane, const uint32_t node)
{
    VTransCommon(ASYNC_WRITE_DATA, bytelane, data, &unusedstatus, 0, node);
}

void VTransWriteDataAsync (const uint32_t data, const uint32_t bytelane, const uint32_t node)
{
    VTransCommon(ASYNC_WRITE_DATA, bytelane, data, &unusedstatus, 0, node);
}

void VTransWriteDataAsync (const uint64_t data, const uint64_t bytelane, const uint32_t node)
{
    VTransCommon(ASYNC_WRITE_DATA, bytelane, data, &unusedstatus, 0, node);
}

void VTransReadAddressAsync  (const uint32_t addr, const uint32_t node)
{
    VTransCommon(ASYNC_READ_ADDRESS, addr, (uint32_t)0, &unusedstatus, 0, node);
}

void VTransReadAddressAsync  (const uint64_t addr, const uint32_t node)
{
    VTransCommon(ASYNC_READ_ADDRESS, addr, (uint32_t)0, &unusedstatus, 0, node);
}

void VTransReadData (uint8_t  *data, const uint32_t node)
{
    *data = VTransCommon(READ_DATA, (uint32_t)0, (uint8_t)0, &unusedstatus, 0, node);
}

void VTransReadData (uint16_t *data, const uint32_t node)
{
    *data = VTransCommon(READ_DATA, (uint32_t)0, (uint16_t)0, &unusedstatus, 0, node);
}

void VTransReadData (uint32_t *data, const uint32_t node)
{
    *data = VTransCommon(READ_DATA, (uint32_t)0, (uint32_t)0, &unusedstatus, 0, node);
}

void VTransReadData (uint64_t  *data, const uint32_t node)
{
    *data = VTransCommon(READ_DATA, (uint64_t)0, (uint64_t)0, &unusedstatus, 0, node);
}

bool VTransTryReadData (uint8_t *data, const uint32_t node)
{
    int status;

    *data = VTransCommon(ASYNC_READ_DATA, (uint32_t)0, (uint8_t)0, &status, 0, node);

    return status;
}

bool VTransTryReadData (uint16_t *data, const uint32_t node)
{
    int status;

    *data = VTransCommon(ASYNC_READ_DATA, (uint32_t)0, (uint16_t)0, &status, 0, node);

    return status;
}

bool VTransTryReadData (uint32_t *data, const uint32_t node)
{
    int status;

    *data = VTransCommon(ASYNC_READ_DATA, (uint32_t)0, (uint32_t)0, &status, 0, node);

    return status;
}

bool VTransTryReadData (uint64_t *data, const uint32_t node)
{
    int status;

    *data = VTransCommon(ASYNC_READ_DATA, (uint64_t)0, (uint64_t)0, &status, 0, node);

    return status;
}

void  VTransReadCheck (const uint32_t addr, const uint8_t data, const int prot, const uint32_t node)
{
    VTransCommon(READ_CHECK, addr, data, &unusedstatus, prot, node);
}

void  VTransReadCheck (const uint32_t addr, const uint16_t data, const int prot, const uint32_t node)
{
    VTransCommon(READ_CHECK, addr, data, &unusedstatus, prot, node);
}

void  VTransReadCheck (const uint32_t addr, const uint32_t data, const int prot, const uint32_t node)
{
    VTransCommon(READ_CHECK, addr, data, &unusedstatus, prot, node);
}

void  VTransReadCheck (const uint64_t addr, const uint8_t data, const int prot, const uint32_t node)
{
    VTransCommon(READ_CHECK, addr, data, &unusedstatus, prot, node);
}

void  VTransReadCheck (const uint64_t addr, const uint16_t data, const int prot, const uint32_t node)
{
    VTransCommon(READ_CHECK, addr, data, &unusedstatus, prot, node);
}

void  VTransReadCheck (const uint64_t addr, const uint32_t data, const int prot, const uint32_t node)
{
    VTransCommon(READ_CHECK, addr, data, &unusedstatus, prot, node);
}

void  VTransReadCheck (const uint64_t addr, const uint64_t data, const int prot, const uint32_t node)
{
    VTransCommon(READ_CHECK, addr, data, &unusedstatus, prot, node);
}

void  VTransReadCheckData ( const uint8_t data, const uint32_t node)
{
    VTransCommon(READ_DATA_CHECK, (uint32_t)0, data, &unusedstatus, (uint32_t)0, node);
}

void  VTransReadCheckData (const uint16_t data, const uint32_t node)
{
    VTransCommon(READ_DATA_CHECK, (uint32_t)0, data, &unusedstatus, (uint32_t)0, node);
}

void  VTransReadCheckData (const uint32_t data, const uint32_t node)
{
    VTransCommon(READ_DATA_CHECK, (uint32_t)0, data, &unusedstatus, (uint32_t)0, node);
}

void  VTransReadCheckData (const uint64_t data, const uint32_t node)
{
    VTransCommon(READ_DATA_CHECK, (uint64_t)0, data, &unusedstatus, (uint32_t)0, node);
}

bool  VTransTryReadCheckData ( const uint8_t data, const uint32_t node)
{
    int status;

    VTransCommon(ASYNC_READ_DATA_CHECK, (uint32_t)0, data, &status, (uint32_t)0, node);

    return status;
}

bool  VTransTryReadCheckData (const uint16_t data, const uint32_t node)
{
    int status;

    VTransCommon(ASYNC_READ_DATA_CHECK, (uint32_t)0, data, &status, (uint32_t)0, node);

    return status;
}

bool  VTransTryReadCheckData (const uint32_t data, const uint32_t node)
{
    int status;

    VTransCommon(ASYNC_READ_DATA_CHECK, (uint32_t)0, data, &status, (uint32_t)0, node);

    return status;
}

bool  VTransTryReadCheckData (const uint64_t data, const uint32_t node)
{
    int status;

    VTransCommon(ASYNC_READ_DATA_CHECK, (uint64_t)0, data, &status, (uint32_t)0, node);

    return status;
}

void VTransBurstWrite (const uint32_t addr, uint8_t* data, const int bytesize, const int prot, const uint32_t node)
{
    VTransBurstCommon(WRITE_BURST, BURST_NORM, addr, data, bytesize, prot, node);
}

void VTransBurstWrite (const uint64_t addr, uint8_t* data, const int bytesize, const int prot, const uint32_t node)
{
    VTransBurstCommon(WRITE_BURST, BURST_NORM, addr, data, bytesize, prot, node);
}

void VTransBurstWrite (const uint32_t addr, const int bytesize, const int prot, const uint32_t node)
{
    VTransBurstCommon(WRITE_BURST, BURST_TRANS, addr, NULL, bytesize, prot, node);
}

void VTransBurstWrite (const uint64_t addr, const int bytesize, const int prot, const uint32_t node)
{
    VTransBurstCommon(WRITE_BURST, BURST_TRANS, addr, NULL, bytesize, prot, node);
}

void VTransBurstWriteAsync (const uint32_t addr, uint8_t* data, const int bytesize, const int prot, const uint32_t node)
{
    VTransBurstCommon(ASYNC_WRITE_BURST, BURST_NORM, addr, data, bytesize, prot, node);
}

void VTransBurstWriteAsync (const uint64_t addr, uint8_t* data, const int bytesize, const int prot, const uint32_t node)
{
    VTransBurstCommon(ASYNC_WRITE_BURST, BURST_NORM, addr, data, bytesize, prot, node);
}

void VTransBurstWriteIncrement (const uint32_t addr, uint8_t* data, const int bytesize, const int prot, const uint32_t node)
{
    VTransBurstCommon(WRITE_BURST, BURST_INCR, addr, data, bytesize, prot, node);
}

void VTransBurstWriteIncrement (const uint64_t addr, uint8_t* data, const int bytesize, const int prot, const uint32_t node)
{
    VTransBurstCommon(WRITE_BURST, BURST_INCR, addr, data, bytesize, prot, node);
}

void VTransBurstWriteIncrementAsync (const uint32_t addr, uint8_t* data, const int bytesize, const int prot, const uint32_t node)
{
    VTransBurstCommon(ASYNC_WRITE_BURST, BURST_INCR, addr, data, bytesize, prot, node);
}

void VTransBurstWriteIncrementAsync (const uint64_t addr, uint8_t* data, const int bytesize, const int prot, const uint32_t node)
{
    VTransBurstCommon(ASYNC_WRITE_BURST, BURST_INCR, addr, data, bytesize, prot, node);
}

void VTransBurstWriteRandom (const uint32_t addr, uint8_t* data, const int bytesize, const int prot, const uint32_t node)
{
    VTransBurstCommon(WRITE_BURST, BURST_RAND, addr, data, bytesize, prot, node);
}

void VTransBurstWriteRandom (const uint64_t addr, uint8_t* data, const int bytesize, const int prot, const uint32_t node)
{
    VTransBurstCommon(WRITE_BURST, BURST_RAND, addr, data, bytesize, prot, node);
}

void VTransBurstWriteRandomAsync (const uint32_t addr, uint8_t* data, const int bytesize, const int prot, const uint32_t node)
{
    VTransBurstCommon(ASYNC_WRITE_BURST, BURST_RAND, addr, data, bytesize, prot, node);
}

void VTransBurstWriteRandomAsync (const uint64_t addr, uint8_t* data, const int bytesize, const int prot, const uint32_t node)
{
    VTransBurstCommon(ASYNC_WRITE_BURST, BURST_RAND, addr, data, bytesize, prot, node);
}

void VTransBurstPushData (uint8_t *data, const int count, const uint32_t node)
{
    VTransBurstCommon(WRITE_BURST, BURST_DATA, (uint32_t)0, data, count, (uint32_t)0, node);
}

void VTransBurstPushIncrement (uint8_t *data, const int count, const uint32_t node)
{
    VTransBurstCommon(WRITE_BURST, BURST_INCR_PUSH, (uint32_t)0, data, count, (uint32_t)0, node);
}

void VTransBurstPushRandom    (uint8_t *data, const int count, const uint32_t node)
{
    VTransBurstCommon(WRITE_BURST, BURST_RAND_PUSH, (uint32_t)0, data, count, (uint32_t)0, node);
}

void VTransCheckBurstReadIncrement (const uint32_t addr, uint8_t *data, const int bytesize, const int prot, const uint32_t node)
{
    VTransBurstCommon(READ_BURST, BURST_INCR, addr, data, bytesize, prot, node);
}

void VTransCheckBurstReadIncrement (const uint64_t addr, uint8_t *data, const int bytesize, const int prot, const uint32_t node)
{
     VTransBurstCommon(READ_BURST, BURST_INCR, addr, data, bytesize, prot, node);
}

void VTransCheckBurstReadRandom    (const uint32_t addr, uint8_t *data, const int bytesize, const int prot, const uint32_t node)
{
     VTransBurstCommon(READ_BURST, BURST_RAND, addr, data, bytesize, prot, node);
}

void VTransCheckBurstReadRandom    (const uint64_t addr, uint8_t *data, const int bytesize, const int prot, const uint32_t node)
{
     VTransBurstCommon(READ_BURST, BURST_RAND, addr, data, bytesize, prot, node);
}

void VTransCheckBurstIncrement (uint8_t *data, const int bytesize, const uint32_t node)
{
    VTransBurstCommon(READ_BURST, BURST_INCR_CHECK, (uint32_t)0, data, bytesize, (uint32_t)0, node);
}

void VTransCheckBurstRandom    (uint8_t *data, const int bytesize, const uint32_t node)
{
    VTransBurstCommon(READ_BURST, BURST_RAND_CHECK, (uint32_t)0, data, bytesize, (uint32_t)0, node);
}

void VTransCheckBurstData    (uint8_t *data, const int bytesize, const uint32_t node)
{
    VTransBurstCommon(READ_BURST, BURST_DATA, (uint32_t)0, data, bytesize, (uint32_t)0, node);
}

void VTransBurstRead  (const uint32_t addr, uint8_t* data, const int bytesize, const int prot, const uint32_t node)
{
    VTransBurstCommon(READ_BURST, BURST_NORM, addr, data, bytesize, prot, node);
}

void VTransBurstRead  (const uint64_t addr, uint8_t* data, const int bytesize, const int prot, const uint32_t node)
{
    VTransBurstCommon(READ_BURST, BURST_NORM, addr, data, bytesize, prot, node);
}

void VTransBurstRead  (const uint32_t addr, const int bytesize, const int prot, const uint32_t node)
{
    VTransBurstCommon(READ_BURST, BURST_TRANS, addr, 0, bytesize, prot, node);
}

void VTransBurstRead  (const uint64_t addr, const int bytesize, const int prot, const uint32_t node)
{
    VTransBurstCommon(READ_BURST, BURST_TRANS, addr, 0, bytesize, prot, node);
}

void VTransBurstPopData (uint8_t *data, const int bytesize, const uint32_t node)
{
    VTransBurstCommon(READ_BURST, BURST_DATA, (uint32_t)0, data, bytesize, 0, node);
}

// --------------------------------------------------------------------------------
// Overloaded stream functions
// --------------------------------------------------------------------------------

void VStreamGet (uint8_t *rdata, int *status, const uint32_t node)
{
    VStreamGetCommon(GET, rdata, status, 0, 0, node);
}

bool VStreamTryGet (uint8_t *rdata, int *status, const uint32_t node)
{
    return VStreamGetCommon(TRY_GET, rdata, status, 0, 0, node);
}

bool VStreamTryCheck (uint8_t data, const int param, const uint32_t node)
{
    int status;

    return VStreamGetCommon(TRY_CHECK, null, &status, data, param, node);
}

void VStreamGet (uint16_t *rdata, int *status, const uint32_t node)
{
    VStreamGetCommon(GET, rdata, status, 0, 0, node);
}

bool VStreamTryGet (uint16_t *rdata, int *status, const uint32_t node)
{
    return VStreamGetCommon(TRY_GET, rdata, status, 0, 0, node);
}

bool VStreamTryCheck (uint16_t data, const int param, const uint32_t node)
{
    int status;

    return VStreamGetCommon(TRY_CHECK, null, &status, data, param, node);
}

void VStreamGet (uint32_t *rdata, int *status, const uint32_t node)
{
    VStreamGetCommon(GET, rdata, status, 0, 0, node);
}

bool VStreamTryGet (uint32_t *rdata, int *status, const uint32_t node)
{
    return VStreamGetCommon(TRY_GET, rdata, status, 0, 0, node);
}

bool VStreamTryCheck (uint32_t data, const int param, const uint32_t node)
{
    int status;
    return VStreamGetCommon(TRY_CHECK, null, &status, data, param, node);
}

void VStreamGet (uint64_t *rdata, int *status, const uint32_t node)
{
    VStreamGetCommon(GET, rdata, status, 0, 0, node);
}

bool VStreamTryGet (uint64_t *rdata, int *status, const uint32_t node)
{
    return VStreamGetCommon(TRY_GET, rdata, status, 0, 0, node);
}

bool VStreamTryCheck (uint64_t data, const int param, const uint32_t node)
{
    int status;
    return VStreamGetCommon(TRY_CHECK, null, &status, data, param, node);
}

uint8_t VStreamSend (const uint8_t data, const int param, const uint32_t node)
{
    return VStreamCommon(SEND, data, param, node);
}

uint16_t VStreamSend (const uint16_t data, const int param, const uint32_t node)
{
    return VStreamCommon(SEND, data, param, node);
}

uint32_t VStreamSend (const uint32_t data, const int param, const uint32_t node)
{
    return VStreamCommon(SEND, data, param, node);
}

uint64_t VStreamSend (const uint64_t data, const int param, const uint32_t node)
{
    return VStreamCommon(SEND, data, param, node);
}

uint8_t VStreamSendAsync (const uint8_t data, const int param, const uint32_t node)
{
    return VStreamCommon(SEND_ASYNC, data, param, node);
}

uint16_t VStreamSendAsync (const uint16_t data, const int param, const uint32_t node)
{
    return VStreamCommon(SEND_ASYNC, data, param, node);
}

uint32_t VStreamSendAsync (const uint32_t data, const int param, const uint32_t node)
{
    return VStreamCommon(SEND_ASYNC, data, param, node);
}

uint64_t VStreamSendAsync (const uint64_t data, const int param, const uint32_t node)
{
    return VStreamCommon(SEND_ASYNC, data, param, node);
}

void VStreamCheck (const uint8_t data, const int param, const uint32_t node)
{
    VStreamCommon(CHECK, data, param, node);
}

void VStreamCheck (const uint16_t data, const int param, const uint32_t node)
{
    VStreamCommon(CHECK, data, param, node);
}

void VStreamCheck (const uint32_t data, const int param, const uint32_t node)
{
    VStreamCommon(CHECK, data, param, node);
}

void VStreamCheck (const uint64_t data, const int param, const uint32_t node)
{
    VStreamCommon(CHECK, data, param, node);
}

void VStreamBurstSend (uint8_t* data, const int bytesize, const int param, const uint32_t node)
{
    VStreamBurstSendCommon(SEND_BURST, BURST_NORM, data, bytesize, param, node);
}

void VStreamBurstSend (const int bytesize, const int param, const uint32_t node)
{
    VStreamBurstSendCommon(SEND_BURST, BURST_TRANS, null, bytesize, param, node);
}

void VStreamBurstSendAsync (uint8_t* data, const int bytesize, const int param, const uint32_t node)
{
    VStreamBurstSendCommon(SEND_BURST_ASYNC, BURST_NORM, data, bytesize, param, node);
}

void VStreamBurstSendAsync (const int bytesize, const int param, const uint32_t node)
{
    VStreamBurstSendCommon(SEND_BURST_ASYNC, BURST_TRANS, null, bytesize, param, node);
}

void VStreamBurstCheck (uint8_t* data, const int bytesize, const int param, const uint32_t node)
{
    VStreamBurstSendCommon(CHECK_BURST, BURST_NORM, data, bytesize, param, node);
}

void VStreamBurstCheck (const int bytesize, const int param, const uint32_t node)
{
    VStreamBurstSendCommon(CHECK_BURST, BURST_TRANS, null, bytesize, param, node);
}

void VStreamBurstSendIncrement (uint8_t  *data, const int  bytesize, const int  param, const uint32_t node)
{
    VStreamBurstSendCommon(SEND_BURST, BURST_INCR, data, bytesize, param, node);
}

void VStreamBurstSendIncrementAsync (uint8_t  *data, const int  bytesize, const int  param, const uint32_t node)
{
    VStreamBurstSendCommon(SEND_BURST_ASYNC, BURST_INCR, data, bytesize, param, node);
}

void VStreamBurstCheckIncrement (uint8_t  *data, const int  bytesize, const int  param, const uint32_t node)
{
    VStreamBurstSendCommon(CHECK_BURST, BURST_INCR_CHECK, data, bytesize, param, node);
}

void VStreamBurstSendRandom (uint8_t  *data, const int  bytesize, const int  param, const uint32_t node)
{
    VStreamBurstSendCommon(CHECK_BURST, BURST_RAND, data, bytesize, param, node);
}

void VStreamBurstSendRandomAsync (uint8_t  *data, const int  bytesize, const int  param, const uint32_t node)
{
    VStreamBurstSendCommon(SEND_BURST_ASYNC, BURST_RAND, data, bytesize, param, node);
}

void VStreamBurstCheckRandom (uint8_t  *data, const int  bytesize, const int  param, const uint32_t node)
{
    VStreamBurstSendCommon(CHECK_BURST, BURST_RAND_CHECK, data, bytesize, param, node);
}

void VStreamBurstPushData(uint8_t* data, const int bytesize, const uint32_t node)
{

    VStreamBurstSendCommon(SEND_BURST, BURST_DATA, data, bytesize, 0, node);
}

void VStreamBurstPushCheckData(uint8_t* data, const int bytesize, const uint32_t node)
{

    VStreamBurstSendCommon(CHECK_BURST, BURST_DATA, data, bytesize, 0, node);
}

void VStreamBurstPushIncrement(uint8_t* data, const int bytesize, const uint32_t node)
{

    VStreamBurstSendCommon(SEND_BURST, BURST_INCR_PUSH, data, bytesize, 0, node);
}

void VStreamBurstPushCheckIncrement(uint8_t* data, const int bytesize, const uint32_t node)
{

    VStreamBurstSendCommon(CHECK_BURST, BURST_INCR_PUSH, data, bytesize, 0, node);
}

void VStreamBurstPushRandom(uint8_t* data, const int bytesize, const uint32_t node)
{

    VStreamBurstSendCommon(SEND_BURST, BURST_RAND_PUSH, data, bytesize, 0, node);
}

void VStreamBurstPushCheckRandom(uint8_t* data, const int bytesize, const uint32_t node)
{

    VStreamBurstSendCommon(CHECK_BURST, BURST_RAND_PUSH, data, bytesize, 0, node);
}

bool VStreamBurstTryCheck (uint8_t *data, const int bytesize, const int param, const uint32_t node)
{
    return VStreamBurstSendCommon(TRY_CHECK_BURST, BURST_NORM, data, bytesize, param, node);
}

bool VStreamBurstTryCheck (const int bytesize, const int param, const uint32_t node)
{
    return VStreamBurstSendCommon(TRY_CHECK_BURST, BURST_TRANS, null, bytesize, param, node);
}

bool VStreamBurstTryCheckIncrement (uint8_t *data, const int bytesize, const int param, const uint32_t node)
{
    return VStreamBurstSendCommon(TRY_CHECK_BURST, BURST_INCR_CHECK, data, bytesize, param, node);
}

bool VStreamBurstTryCheckRandom (uint8_t *data, const int bytesize, const int param, const uint32_t node)
{
    return VStreamBurstSendCommon(TRY_CHECK_BURST, BURST_RAND_CHECK, data, bytesize, param, node);
}

void VStreamBurstGet (uint8_t* data, const int bytesize, int* status, const uint32_t node)
{
    VStreamBurstGetCommon(GET_BURST, BURST_NORM, data, bytesize, status, node);
}

void VStreamBurstGet (const int bytesize, int* status, const uint32_t node)
{
    VStreamBurstGetCommon(GET_BURST, BURST_TRANS, null, bytesize, status, node);
}

void VStreamBurstPopData(uint8_t* data, const int bytesize, const uint32_t node)
{
    int status;

    VStreamBurstGetCommon(GET_BURST, BURST_DATA, data, bytesize, &status, node);
}

bool VStreamBurstTryGet   (uint8_t  *data, const int  bytesize, const int param, const uint32_t node)
{
    int status;

    return VStreamBurstGetCommon(TRY_GET_BURST, BURST_NORM, data, bytesize, &status, node);
}

bool VStreamBurstTryGet (const int bytesize, const int param, const uint32_t node)
{
    int status;

    return VStreamBurstGetCommon(TRY_GET_BURST, BURST_TRANS, null, bytesize, &status, node);
}

