// =========================================================================
//
//  File Name:         OsvvmVUser.h
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
//  Revision History:
//    Date      Version    Description
//    05/2023   2023.05    Adding asynchronous transaction support
//    03/2023   2023.04    Adding basic stream support
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

#ifndef _OSVVM_VUSER_H_
#define _OSVVM_VUSER_H_

// -------------------------------------------------------------------------
// INCLUDES
// -------------------------------------------------------------------------

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
// -------------------------------------------------------------------------
// DEFINES AND MACROS
// -------------------------------------------------------------------------

#define DELTA_CYCLE             -1
#define NO_DELTA_CYCLE          0
#define GO_TO_SLEEP             0x7fffffff

#ifdef DISABLE_VUSERMAIN_THREAD
#define SLEEPFOREVER            { return; }
#else
#define SLEEPFOREVER            { while(1) VTick(GO_TO_SLEEP, node); }
#endif

#define MAX_INT_LEVEL           256
#define MIN_INT_LEVEL           1

#define HUNDRED_MILLISECS       1000000
#define FIVESEC_TIMEOUT         (50*HUNDRED_MILLISECS)

// In windows using the FLI, a \n in the printf format string causes
// two lines to be advanced, so replace new lines with carriage returns
// which seems to work
# ifndef ALDEC
#  ifdef _WIN32

# define VPrint(format, ...) {int len;                                             \
                              char formbuf[256];                                   \
                              strncpy(formbuf, format, 255);                       \
                              len = strlen(formbuf);                               \
                              for(int i = 0; i < len; i++)                         \
                                if (formbuf[i] == '\n')                            \
                                  formbuf[i] = '\r';                               \
                              printf (formbuf, ##__VA_ARGS__);                     \
                              }
#  else
// If compiled under C++, io_printf() uses PLI_BYTE* which can't have const char* cast,
// so use buffers for a format string and single string buffer argument, and sprintf to
// format the string into the msg buffer
//# define VPrint(...) {char __msg[4096], fmt[10]; strcpy(fmt, "%s");sprintf(__msg, __VA_ARGS__); io_printf(fmt, __msg);}
#  define VPrint(...) {printf(__VA_ARGS__);}

#  endif
# else
#  define VPrint(...) {vhpi_printf(__VA_ARGS__);}
# endif

#ifdef DEBUG
#define DebugVPrint VPrint
#else
#define DebugVPrint(...) {}

#endif
// -------------------------------------------------------------------------
// TYPE DEFINITIONS
// -------------------------------------------------------------------------

// Pointer to pthread_create compatible function
typedef void *(*pThreadFunc_t)(void *);

// Pointer to VUserMain function type definition
typedef void (*pVUserMain_t)(void);

// -------------------------------------------------------------------------
// FUNCTION PROTOTYPES
// -------------------------------------------------------------------------

// VUser function prototypes
#ifdef __cplusplus

// Function to advance simulation by a number of clock ticks
extern int      VTick                          (const uint32_t ticks, const bool done = false, const bool error = false, const uint32_t  node = 0);

// GHDL main support function to wait for simultion to be ready to call
extern void     VWaitForSim                    (const uint32_t node = 0);

// OSVVM support function to set the test name
extern void     VSetTestName                   (const char*    data, const int bytesize, const uint32_t node);

// Overloaded write transaction functions for 32 and 64 bit architecture for byte,
// half-word, word and double-word
extern uint8_t  VTransWrite                    (const uint32_t addr, const uint8_t   data, const int prot = 0, const uint32_t node = 0);
extern uint16_t VTransWrite                    (const uint32_t addr, const uint16_t  data, const int prot = 0, const uint32_t node = 0);
extern uint32_t VTransWrite                    (const uint32_t addr, const uint32_t  data, const int prot = 0, const uint32_t node = 0);
extern uint8_t  VTransWrite                    (const uint64_t addr, const uint8_t   data, const int prot = 0, const uint32_t node = 0);
extern uint16_t VTransWrite                    (const uint64_t addr, const uint16_t  data, const int prot = 0, const uint32_t node = 0);
extern uint32_t VTransWrite                    (const uint64_t addr, const uint32_t  data, const int prot = 0, const uint32_t node = 0);
extern uint64_t VTransWrite                    (const uint64_t addr, const uint64_t  data, const int prot = 0, const uint32_t node = 0);

// Overloaded write asynchronous transaction functions for 32 and 64 bit architecture for byte,
// half-word, word and double-word
extern uint8_t  VTransWriteAsync               (const uint32_t addr, const uint8_t   data, const int prot = 0, const uint32_t node = 0);
extern uint16_t VTransWriteAsync               (const uint32_t addr, const uint16_t  data, const int prot = 0, const uint32_t node = 0);
extern uint32_t VTransWriteAsync               (const uint32_t addr, const uint32_t  data, const int prot = 0, const uint32_t node = 0);
extern uint8_t  VTransWriteAsync               (const uint64_t addr, const uint8_t   data, const int prot = 0, const uint32_t node = 0);
extern uint16_t VTransWriteAsync               (const uint64_t addr, const uint16_t  data, const int prot = 0, const uint32_t node = 0);
extern uint32_t VTransWriteAsync               (const uint64_t addr, const uint32_t  data, const int prot = 0, const uint32_t node = 0);
extern uint64_t VTransWriteAsync               (const uint64_t addr, const uint64_t  data, const int prot = 0, const uint32_t node = 0);

// Overloaded read transaction functions for 32 and 64 bit architecture for byte,
// half-word, word, and double-word
extern void     VTransRead                     (const uint32_t addr,       uint8_t  *data, const int prot = 0, const uint32_t node = 0);
extern void     VTransRead                     (const uint32_t addr,       uint16_t *data, const int prot = 0, const uint32_t node = 0);
extern void     VTransRead                     (const uint32_t addr,       uint32_t *data, const int prot = 0, const uint32_t node = 0);
extern void     VTransRead                     (const uint64_t addr,       uint8_t  *data, const int prot = 0, const uint32_t node = 0);
extern void     VTransRead                     (const uint64_t addr,       uint16_t *data, const int prot = 0, const uint32_t node = 0);
extern void     VTransRead                     (const uint64_t addr,       uint32_t *data, const int prot = 0, const uint32_t node = 0);
extern void     VTransRead                     (const uint64_t addr,       uint64_t *data, const int prot = 0, const uint32_t node = 0);

extern void     VTransReadCheck                (const uint32_t addr, const uint8_t   data, const int prot = 0, const uint32_t node = 0);
extern void     VTransReadCheck                (const uint32_t addr, const uint16_t  data, const int prot = 0, const uint32_t node = 0);
extern void     VTransReadCheck                (const uint32_t addr, const uint32_t  data, const int prot = 0, const uint32_t node = 0);
extern void     VTransReadCheck                (const uint64_t addr, const uint8_t   data, const int prot = 0, const uint32_t node = 0);
extern void     VTransReadCheck                (const uint64_t addr, const uint16_t  data, const int prot = 0, const uint32_t node = 0);
extern void     VTransReadCheck                (const uint64_t addr, const uint32_t  data, const int prot = 0, const uint32_t node = 0);
extern void     VTransReadCheck                (const uint64_t addr, const uint64_t  data, const int prot = 0, const uint32_t node = 0);

// Overloaded write-read transaction functions for 32 and 64 bit architecture for byte,
// half-word, word and double-word
uint8_t         VTransWriteAndRead             (const uint32_t addr, const uint8_t   data, const int prot = 0, const uint32_t node = 0);
uint16_t        VTransWriteAndRead             (const uint32_t addr, const uint16_t  data, const int prot = 0, const uint32_t node = 0);
uint32_t        VTransWriteAndRead             (const uint32_t addr, const uint32_t  data, const int prot = 0, const uint32_t node = 0);
uint8_t         VTransWriteAndRead             (const uint64_t addr, const uint8_t   data, const int prot = 0, const uint32_t node = 0);
uint16_t        VTransWriteAndRead             (const uint64_t addr, const uint16_t  data, const int prot = 0, const uint32_t node = 0);
uint32_t        VTransWriteAndRead             (const uint64_t addr, const uint32_t  data, const int prot = 0, const uint32_t node = 0);
uint64_t        VTransWriteAndRead             (const uint64_t addr, const uint64_t  data, const int prot = 0, const uint32_t node = 0);

// Overloaded write-read asynchronous transaction functions for 32 and 64 bit architecture for byte,
// half-word, word and double-word
uint8_t         VTransWriteAndReadAsync        (const uint32_t addr, const uint8_t   data, const int prot = 0, const uint32_t node = 0);
uint16_t        VTransWriteAndReadAsync        (const uint32_t addr, const uint16_t  data, const int prot = 0, const uint32_t node = 0);
uint32_t        VTransWriteAndReadAsync        (const uint32_t addr, const uint32_t  data, const int prot = 0, const uint32_t node = 0);
uint8_t         VTransWriteAndReadAsync        (const uint64_t addr, const uint8_t   data, const int prot = 0, const uint32_t node = 0);
uint16_t        VTransWriteAndReadAsync        (const uint64_t addr, const uint16_t  data, const int prot = 0, const uint32_t node = 0);
uint32_t        VTransWriteAndReadAsync        (const uint64_t addr, const uint32_t  data, const int prot = 0, const uint32_t node = 0);
uint64_t        VTransWriteAndReadAsync        (const uint64_t addr, const uint64_t  data, const int prot = 0, const uint32_t node = 0);

// Overloaded write address asynchronous transaction functions for 32 and 64 bit architecture
void            VTransWriteAddressAsync        (const uint32_t addr, const uint32_t node = 0);
void            VTransWriteAddressAsync        (const uint64_t addr, const uint32_t node = 0);

// Overloaded write data transaction functions for byte, half-word, word and double-word
void            VTransWriteDataAsync           (const uint8_t  data, const uint32_t bytelane = 0, const uint32_t node = 0);
void            VTransWriteDataAsync           (const uint16_t data, const uint32_t bytelane = 0, const uint32_t node = 0);
void            VTransWriteDataAsync           (const uint32_t data, const uint32_t bytelane = 0, const uint32_t node = 0);
void            VTransWriteDataAsync           (const uint64_t data, const uint32_t bytelane = 0, const uint32_t node = 0);

// Overloaded read address asynchronous tr     ansaction functions for 32 and 64 bit architecture
void            VTransReadAddressAsync         (const uint32_t addr, const uint32_t node = 0);
void            VTransReadAddressAsync         (const uint64_t addr, const uint32_t node = 0);

// Overloaded write data transaction functions for byte, half-word, word and double-word
void            VTransReadData                 (     uint8_t  *data, const uint32_t node = 0);
void            VTransReadData                 (     uint16_t *data, const uint32_t node = 0);
void            VTransReadData                 (     uint32_t *data, const uint32_t node = 0);
void            VTransReadData                 (     uint64_t *data, const uint32_t node = 0);

bool            VTransTryReadData              (     uint8_t  *data, const uint32_t node = 0);
bool            VTransTryReadData              (     uint16_t *data, const uint32_t node = 0);
bool            VTransTryReadData              (     uint32_t *data, const uint32_t node = 0);
bool            VTransTryReadData              (     uint64_t *data, const uint32_t node = 0);

// Overloaded write data transaction functions for byte, half-word, word and double-word
void            VTransReadCheckData            (const uint8_t  data, const uint32_t node = 0);
void            VTransReadCheckData            (const uint16_t data, const uint32_t node = 0);
void            VTransReadCheckData            (const uint32_t data, const uint32_t node = 0);
void            VTransReadCheckData            (const uint64_t data, const uint32_t node = 0);

bool            VTransTryReadCheckData         (const uint8_t  data, const uint32_t node = 0);
bool            VTransTryReadCheckData         (const uint16_t data, const uint32_t node = 0);
bool            VTransTryReadCheckData         (const uint32_t data, const uint32_t node = 0);
bool            VTransTryReadCheckData         (const uint64_t data, const uint32_t node = 0);

// Overloaded write burst transaction functions for 32 and 64 bit architecture
extern void     VTransBurstWrite               (const uint32_t  addr, uint8_t *data,      const int bytesize, const int      prot = 0, const uint32_t node = 0);
extern void     VTransBurstWrite               (const uint64_t  addr, uint8_t *data,      const int bytesize, const int      prot = 0, const uint32_t node = 0);
extern void     VTransBurstWrite               (const uint32_t  addr, const int bytesize, const int prot = 0, const uint32_t node = 0);
extern void     VTransBurstWrite               (const uint64_t  addr, const int bytesize, const int prot = 0, const uint32_t node = 0);
extern void     VTransBurstWriteAsync          (const uint32_t  addr, uint8_t *data,      const int bytesize, const int      prot = 0, const uint32_t node = 0);
extern void     VTransBurstWriteAsync          (const uint64_t  addr, uint8_t *data,      const int bytesize, const int      prot = 0, const uint32_t node = 0);
extern void     VTransBurstWriteIncrement      (const uint32_t  addr, uint8_t *data,      const int count,    const int      prot = 0, const uint32_t node = 0);
extern void     VTransBurstWriteIncrement      (const uint64_t  addr, uint8_t *data,      const int count,    const int      prot = 0, const uint32_t node = 0);
extern void     VTransBurstWriteIncrementAsync (const uint32_t  addr, uint8_t *data,      const int count,    const int      prot = 0, const uint32_t node = 0);
extern void     VTransBurstWriteIncrementAsync (const uint64_t  addr, uint8_t *data,      const int count,    const int      prot = 0, const uint32_t node = 0);
extern void     VTransBurstWriteRandom         (const uint32_t  addr, uint8_t *data,      const int count,    const int      prot = 0, const uint32_t node = 0);
extern void     VTransBurstWriteRandom         (const uint64_t  addr, uint8_t *data,      const int count,    const int      prot = 0, const uint32_t node = 0);
extern void     VTransBurstWriteRandomAsync    (const uint32_t  addr, uint8_t *data,      const int count,    const int      prot = 0, const uint32_t node = 0);
extern void     VTransBurstWriteRandomAsync    (const uint64_t  addr, uint8_t *data,      const int count,    const int      prot = 0, const uint32_t node = 0);

extern void     VTransBurstPushData            (       uint8_t *data, const int count,          const uint32_t node = 0);
extern void     VTransBurstPushIncrement       (       uint8_t *data, const int count,          const uint32_t node = 0);
extern void     VTransBurstPushRandom          (       uint8_t *data, const int count,          const uint32_t node = 0);

// Overloaded read burst transaction functions for 32 and 64 bit architecture
extern void     VTransBurstRead                (const uint32_t  addr,    uint8_t   *data,       const int bytesize, const int prot = 0, const uint32_t node = 0);
extern void     VTransBurstRead                (const uint64_t  addr,    uint8_t   *data,       const int bytesize, const int prot = 0, const uint32_t node = 0);
extern void     VTransBurstRead                (const uint32_t  addr,    const int  bytesize,   const int prot = 0, const uint32_t node = 0);
extern void     VTransBurstRead                (const uint64_t  addr,    const int  bytesize,   const int prot = 0, const uint32_t node = 0);
extern void     VTransCheckBurstReadIncrement  (const uint32_t  addr,    uint8_t   *data,       const int bytesize, const int prot = 0, const uint32_t node = 0);
extern void     VTransCheckBurstReadIncrement  (const uint64_t  addr,    uint8_t   *data,       const int bytesize, const int prot = 0, const uint32_t node = 0);
extern void     VTransCheckBurstReadRandom     (const uint32_t  addr,    uint8_t   *data,       const int bytesize, const int prot = 0, const uint32_t node = 0);
extern void     VTransCheckBurstReadRandom     (const uint64_t  addr,    uint8_t   *data,       const int bytesize, const int prot = 0, const uint32_t node = 0);

extern void     VTransBurstPopData             (      uint8_t  *data,    const int  bytesize,   const uint32_t node = 0);
extern void     VTransCheckBurstIncrement      (      uint8_t  *data,    const int  bytesize,   const uint32_t node = 0);
extern void     VTransCheckBurstRandom         (      uint8_t  *data,    const int  bytesize,   const uint32_t node = 0);

// Overloaded stream send transaction functions for byte, half-word, word and double-word
extern uint8_t  VStreamSend                    (const uint8_t   data,    const int  param = 0,  const uint32_t node = 0);
extern uint16_t VStreamSend                    (const uint16_t  data,    const int  param = 0,  const uint32_t node = 0);
extern uint32_t VStreamSend                    (const uint32_t  data,    const int  param = 0,  const uint32_t node = 0);
extern uint64_t VStreamSend                    (const uint64_t  data,    const int  param = 0,  const uint32_t node = 0);
extern uint8_t  VStreamSendAsync               (const uint8_t   data,    const int  param = 0,  const uint32_t node = 0);
extern uint16_t VStreamSendAsync               (const uint16_t  data,    const int  param = 0,  const uint32_t node = 0);
extern uint32_t VStreamSendAsync               (const uint32_t  data,    const int  param = 0,  const uint32_t node = 0);
extern uint64_t VStreamSendAsync               (const uint64_t  data,    const int  param = 0,  const uint32_t node = 0);

// Overloaded stream get transaction functions for byte, half-word, word and double-word
extern void     VStreamGet                     (      uint8_t  *data,          int *status,     const uint32_t node = 0);
extern void     VStreamGet                     (      uint16_t *data,          int *status,     const uint32_t node = 0);
extern void     VStreamGet                     (      uint32_t *data,          int *status,     const uint32_t node = 0);
extern void     VStreamGet                     (      uint64_t *data,          int *status,     const uint32_t node = 0);

extern void     VStreamCheck                   (      uint8_t   data,    const int  param = 0,  const uint32_t node = 0);
extern void     VStreamCheck                   (      uint16_t  data,    const int  param = 0,  const uint32_t node = 0);
extern void     VStreamCheck                   (      uint32_t  data,    const int  param = 0,  const uint32_t node = 0);
extern void     VStreamCheck                   (      uint64_t  data,    const int  param = 0,  const uint32_t node = 0);

// Stream burst send and get transaction functions
extern void     VStreamBurstSend               (      uint8_t *data,     const int  bytesize,   const int      param = 0, const uint32_t node = 0);
extern void     VStreamBurstSend               (const int      bytesize, const int  param=0,    const uint32_t node = 0);
extern void     VStreamBurstSendAsync          (      uint8_t *data,     const int  bytesize,   const int      param = 0, const uint32_t node = 0);
extern void     VStreamBurstSendAsync          (const int      bytesize, const int  param=0,    const uint32_t node = 0);
extern void     VStreamBurstSendIncrement      (      uint8_t *data,     const int  bytesize,   const int      param = 0, const uint32_t node = 0);
extern void     VStreamBurstSendRandom         (      uint8_t *data,     const int  bytesize,   const int      param = 0, const uint32_t node = 0);
extern void     VStreamBurstSendIncrementAsync (      uint8_t *data,     const int  bytesize,   const int      param = 0, const uint32_t node = 0);
extern void     VStreamBurstSendRandomAsync    (      uint8_t *data,     const int  bytesize,   const int      param = 0, const uint32_t node = 0);

extern void     VStreamBurstCheck              (      uint8_t *data,     const int  bytesize,   const int      param = 0, const uint32_t node = 0);
extern void     VStreamBurstCheck              (const int      bytesize, const int  param=0,    const uint32_t node = 0);
extern void     VStreamBurstCheckIncrement     (      uint8_t *data,     const int  bytesize,   const int      param, const uint32_t node);
extern void     VStreamBurstCheckRandom        (      uint8_t *data,     const int  bytesize,   const int      param, const uint32_t node);

extern void     VStreamBurstGet                (      uint8_t *data,     const int  bytesize,         int     *status,    const uint32_t node = 0);
extern void     VStreamBurstGet                (const int      bytesize,       int *status,     const uint32_t node = 0);

extern void     VStreamBurstPopData            (      uint8_t* data,     const int  bytesize,   const uint32_t node = 0);
extern void     VStreamBurstPushData           (      uint8_t* data,     const int  bytesize,   const uint32_t node = 0);
extern void     VStreamBurstPushCheckData      (      uint8_t* data,     const int  bytesize,   const uint32_t node = 0);
extern void     VStreamBurstPushIncrement      (      uint8_t* data,     const int  bytesize,   const uint32_t node = 0);
extern void     VStreamBurstPushCheckIncrement (      uint8_t* data,     const int  bytesize,   const uint32_t node = 0);
extern void     VStreamBurstPushRandom         (      uint8_t* data,     const int  bytesize,   const uint32_t node = 0);
extern void     VStreamBurstPushCheckRandom    (      uint8_t* data,     const int  bytesize,   const uint32_t node = 0);

extern bool     VStreamTryGet                  (     uint8_t  *data,           int *status,     const uint32_t node = 0);
extern bool     VStreamTryGet                  (     uint16_t *data,           int *status,     const uint32_t node = 0);
extern bool     VStreamTryGet                  (     uint32_t *data,           int *status,     const uint32_t node = 0);
extern bool     VStreamTryGet                  (     uint64_t *data,           int *status,     const uint32_t node = 0);

extern bool     VStreamTryCheck                (     uint8_t   data,     const int  param = 0,  const uint32_t node = 0);
extern bool     VStreamTryCheck                (     uint16_t  data,     const int  param = 0,  const uint32_t node = 0);
extern bool     VStreamTryCheck                (     uint32_t  data,     const int  param = 0,  const uint32_t node = 0);
extern bool     VStreamTryCheck                (     uint64_t  data,     const int  param = 0,  const uint32_t node = 0);

extern bool     VStreamBurstTryGet             (      uint8_t *data,     const int  bytesize,   const int      param = 0, const uint32_t node = 0);
extern bool     VStreamBurstTryGet             (const int      bytesize, const int  param = 0,  const uint32_t node = 0);
extern bool     VStreamBurstTryCheck           (      uint8_t *data,     const int  bytesize,   const int      param = 0, const uint32_t node = 0);
extern bool     VStreamBurstTryCheck           (const int      bytesize, const int  param = 0,  const uint32_t node = 0);
extern bool     VStreamBurstTryCheckIncrement  (      uint8_t *data,     const int  bytesize,   const int      param = 0, const uint32_t node = 0);
extern bool     VStreamBurstTryCheckRandom     (      uint8_t *data,     const int  bytesize,   const int      param = 0, const uint32_t node = 0);

#endif

// User function called from VInit to instigate new user thread
extern EXTC int  VUser           (const int node);

// User interrupt callback registering function
extern EXTC void VRegInterrupt   (const pVUserInt_t func, const uint32_t node);

#endif
