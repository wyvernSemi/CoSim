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
//      definitions and prototypes for user side code.
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

#ifndef _OSVVM_VUSER_H_
#define _OSVVM_VUSER_H_

#include <stdint.h>

#ifdef __cplusplus
#define EXTC  "C"
extern "C"
{
#else
#define EXTC
#endif

#include "OsvvmVProc.h"
#include "OsvvmVSchedPli.h"

#ifdef __cplusplus
}
#endif

#define DELTA_CYCLE             -1
#define NO_DELTA_CYCLE          0
#define GO_TO_SLEEP             0x7fffffff

#ifdef DISABLE_VUSERMAIN_THREAD
#define SLEEPFOREVER            { return; }
#else
#define SLEEPFOREVER            { while(1) VTick(GO_TO_SLEEP, node); }
#endif
                                
#define MAX_INT_LEVEL           7
#define MIN_INT_LEVEL           1

#define HUNDRED_MILLISECS       1000000
#define FIVESEC_TIMEOUT         (50*HUNDRED_MILLISECS)

// Pointer to pthread_create compatible function
typedef void *(*pThreadFunc_t)(void *);

// VUser function prototypes
#ifdef __cplusplus
// VProc write and read functions (fixed at 32-bit)
extern int      VWrite           (uint32_t addr,  uint32_t  data, int delta, uint32_t node = 0);
extern int      VRead            (uint32_t addr,  uint32_t *data, int delta, uint32_t node = 0);
extern int      VTick            (uint32_t ticks, bool      done = false,    bool     error = false, uint32_t  node = 0);
extern void     VWaitForSim      (uint32_t node = 0);

// Overloaded write and read transaction functions for 32 bit architecture for byte,
// half-word and, words
extern uint8_t  VTransWrite      (uint32_t addr, uint8_t   data, int prot = 0, uint32_t node = 0);
extern void     VTransRead       (uint32_t addr, uint8_t  *data, int prot = 0, uint32_t node = 0);
extern uint16_t VTransWrite      (uint32_t addr, uint16_t  data, int prot = 0, uint32_t node = 0);
extern void     VTransRead       (uint32_t addr, uint16_t *data, int prot = 0, uint32_t node = 0);
extern uint32_t VTransWrite      (uint32_t addr, uint32_t  data, int prot = 0, uint32_t node = 0);
extern void     VTransRead       (uint32_t addr, uint32_t *data, int prot = 0, uint32_t node = 0);

// Overloaded write and read transaction functions for 64 bit architecture for byte,
// half-word, word, and double-word
extern uint8_t  VTransWrite      (uint64_t addr, uint8_t   data, int prot = 0, uint32_t node = 0);
extern void     VTransRead       (uint64_t addr, uint8_t  *data, int prot = 0, uint32_t node = 0);
extern uint16_t VTransWrite      (uint64_t addr, uint16_t  data, int prot = 0, uint32_t node = 0);
extern void     VTransRead       (uint64_t addr, uint16_t *data, int prot = 0, uint32_t node = 0);
extern uint32_t VTransWrite      (uint64_t addr, uint32_t  data, int prot = 0, uint32_t node = 0);
extern void     VTransRead       (uint64_t addr, uint32_t *data, int prot = 0, uint32_t node = 0);
extern uint64_t VTransWrite      (uint64_t addr, uint64_t  data, int prot = 0, uint32_t node = 0);
extern void     VTransRead       (uint64_t addr, uint64_t *data, int prot = 0, uint32_t node = 0);

extern void     VTransBurstWrite (uint32_t addr, uint8_t  *data, int bytesize, int prot = 0, uint32_t node = 0);
extern void     VTransBurstWrite (uint64_t addr, uint8_t  *data, int bytesize, int prot = 0, uint32_t node = 0);
extern void     VTransBurstRead  (uint32_t addr, uint8_t  *data, int bytesize, int prot = 0, uint32_t node = 0);
extern void     VTransBurstRead  (uint64_t addr, uint8_t  *data, int bytesize, int prot = 0, uint32_t node = 0);

#endif

extern EXTC int  VUser         (int node);
extern EXTC void VRegInterrupt (int level, pVUserInt_t func, uint32_t node);
extern EXTC void VRegUser      (pVUserCB_t func, uint32_t node);

// In windows using the FLI, a \n in the printf format string causes
// two lines to be advanced, so replace new lines with carriage returns
// which seems to work
# ifdef _WIN32

#define VPrint(format, ...) {int len;                                             \
                             char formbuf[256];                                   \
                             strncpy(formbuf, format, 255);                       \
                             len = strlen(formbuf);                               \
                             for(int i = 0; i < len; i++)                         \
                               if (formbuf[i] == '\n')                            \
                                 formbuf[i] = '\r';                               \
                             printf (formbuf, ##__VA_ARGS__);                     \
                             }
# else
// If compiled under C++, io_printf() uses PLI_BYTE* which can't have const char* cast,
// so use buffers for a format string and single string buffer argument, and sprintf to
// format the string into the msg buffer
//# define VPrint(...) {char __msg[4096], fmt[10]; strcpy(fmt, "%s");sprintf(__msg, __VA_ARGS__); io_printf(fmt, __msg);}
# define VPrint(...) {printf(__VA_ARGS__);}

# endif

#ifdef DEBUG
#define DebugVPrint VPrint
#else
#define DebugVPrint(...) {}

#endif

// Pointer to VUserMain function type definition
typedef void (*pVUserMain_t)(void);

#endif
