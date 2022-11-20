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

#ifndef _OSVVM_VPROC_H_
#define _OSVVM_VPROC_H_

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#if defined(WIN32) &&  defined(WIN64)
#include <windows.h>
#define dlopen(x, y)  LoadLibrary(x)
#define dlsym(x,y) GetProcAddress((HMODULE)x, y)
#else

# ifndef __USE_GNU
# define __USE_GNU
# include <dlfcn.h>
# undef __USE_GNU
# else
# include <dlfcn.h>
# endif

#endif

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
#define MAX_INTERRUPT_LEVEL     7

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
  trans64_rd_burst

} trans_type_e;

typedef enum arch_e
{
    arch32,
    arch64,
    arch128
} arch_e;

typedef struct
{
    trans_type_e        type;
    uint32_t            prot;
    uint64_t            addr;
    uint8_t             data[16];
    int                 num_burst_bytes;
    uint8_t             databuf[DATABUF_SIZE];
    uint32_t            rw;
    int                 ticks;
    int                 done;
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
typedef int  (*pVUserInt_t)      (void);
typedef int  (*pVUserCB_t)       (int);

typedef struct
{
    sem_t               snd;
    sem_t               rcv;
    send_buf_t          send_buf;
    rcv_buf_t           rcv_buf;
    pVUserInt_t         VInt_table[MAX_INTERRUPT_LEVEL+1];
    pVUserCB_t          VUserCB;
} SchedState_t, *pSchedState_t;

extern pSchedState_t ns[VP_MAX_NODES];

#endif
