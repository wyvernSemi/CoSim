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
//      Internal header for co-simulation virtual procedure definitions
//      and data types
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

#ifndef _OSVVM_VPROC_H_
#define _OSVVM_VPROC_H_

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

# if !defined(ALDEC)
#  ifndef __USE_GNU
#  define __USE_GNU
#  include <dlfcn.h>
#  undef __USE_GNU
#  else
#  include <dlfcn.h>
#  endif
# endif

#include <pthread.h>
#include <sched.h>

#include <semaphore.h>

// For file IO
#include <fcntl.h>

// For inode manipulation
#include <unistd.h>

#ifndef VP_MAX_NODES
#define VP_MAX_NODES            64
#endif


#define V_IDLE                  0
#define V_WRITE                 1
#define V_READ                  2
#define V_HALT                  4
#define V_SWAP                  8

#define VP_EXIT_OK              0
#define VP_QUEUE_ERR            1
#define VP_KEY_ERR              2
#define VP_USER_ERR             3
#define VP_SYSCALL_ERR          4

#define UNDEF                   -1

#define DEFAULT_STR_BUF_SIZE    32

#define MIN_INTERRUPT_LEVEL     1
#define MAX_INTERRUPT_LEVEL     255

#define DATABUF_SIZE            4096


typedef enum trans_type_e
{
  trans32_wr_byte  = 0,
  trans32_wr_hword,
  trans32_wr_word,
  trans32_wr_dword,
  trans32_wr_qword,
  trans32_wr_burst,
  trans32_rd_byte,
  trans32_rd_hword,
  trans32_rd_word ,
  trans32_rd_dword,
  trans32_rd_qword,
  trans32_rd_burst,
  trans64_wr_byte,
  trans64_wr_hword,
  trans64_wr_word,
  trans64_wr_dword,
  trans64_wr_qword,
  trans64_wr_burst,
  trans64_rd_byte,
  trans64_rd_hword,
  trans64_rd_word,
  trans64_rd_dword,
  trans64_rd_qword,
  trans64_rd_burst,
  trans_idle

} trans_type_e;

typedef enum addr_bus_trans_op_e
{
    NOT_DRIVEN = 0,
    WAIT_FOR_CLOCK,
    WAIT_FOR_TRANSACTION,
    WAIT_FOR_WRITE_TRANSACTION,
    WAIT_FOR_READ_TRANSACTION,
    GET_TRANSACTION_COUNT,
    GET_WRITE_TRANSACTION_COUNT,
    GET_READ_TRANSACTION_COUNT,
    GET_ALERTLOG_ID,
    SET_BURST_MODE,
    GET_BURST_MODE,
    SET_MODEL_OPTIONS,
    GET_MODEL_OPTIONS,
    INTERRUPT_RETURN,
    WRITE_OP,
    WRITE_ADDRESS,
    WRITE_DATA,
    ASYNC_WRITE,
    ASYNC_WRITE_ADDRESS,
    ASYNC_WRITE_DATA,
    READ_OP,
    READ_ADDRESS,
    READ_DATA,
    READ_CHECK,
    READ_DATA_CHECK,
    ASYNC_READ,
    ASYNC_READ_ADDRESS,
    ASYNC_READ_DATA,
    ASYNC_READ_DATA_CHECK,
    WRITE_AND_READ,
    ASYNC_WRITE_AND_READ,
    WRITE_BURST,
    ASYNC_WRITE_BURST,
    READ_BURST,
    MULTIPLE_DRIVER_DETECT,
    
    SET_TEST_NAME = 1024
} addr_bus_trans_op_t;

typedef enum arch_e
{
    arch32,
    arch64,
    arch128
} arch_e;

typedef struct
{
    addr_bus_trans_op_t op;
    trans_type_e        type;
    uint32_t            prot;
    uint64_t            addr;
    uint8_t             data[16];
    int                 num_burst_bytes;
    uint8_t             databuf[DATABUF_SIZE];
    int                 ticks;
    int                 done;
    int                 error;
} send_buf_t, *psend_buf_t;

typedef struct
{
    unsigned int        data_in;
    unsigned int        data_in_hi;
    int                 num_burst_bytes;
    uint8_t             databuf[DATABUF_SIZE];
    unsigned int        interrupt;
} rcv_buf_t, *prcv_buf_t;


// Shared object handle type
typedef void * handle_t;

// Interrupt function pointer type
typedef int  (*pVUserInt_t)      (int);

typedef struct
{
    sem_t               snd;
    sem_t               rcv;
    send_buf_t          send_buf;
    rcv_buf_t           rcv_buf;
    pVUserInt_t         VIntVecCB;
    unsigned int        last_int;
} SchedState_t, *pSchedState_t;

extern pSchedState_t ns[VP_MAX_NODES];

#endif
