// =========================================================================
//
//  File Name:         OsvvmCosim.h
//  Design Unit Name:
//  Revision:          OSVVM MODELS STANDARD VERSION
//
//  Maintainer:        Simon Southwell email:  simon.southwell@gmail.com
//  Contributor(s):
//     Simon Southwell      simon.southwell@gmail.com
//
//
//  Description:
//      Simulator co-simulation virtual procedure C++ class for user
//      side code. Virtualises away top level VP user thread routines.
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

#include <stdint.h>
#include <string>
#include "OsvvmVUser.h"

#ifndef __OSVVM_COSIM_H_
#define __OSVVM_COSIM_H_

class OsvvmCosim
{
public:
                OsvvmCosim      (int nodeIn = 0, std::string test_name = "") : node(nodeIn) {
                   if (test_name.compare(""))
                   {
                       VSetTestName(test_name.c_str(), test_name.length(), node);
                   }
                };

      void     tick            (const int ticks, const bool done = false, const bool error = false)
      {
#ifndef DISABLE_VUSERMAIN_THREAD
          VTick(ticks, done, error, node);
#else
          VTick(ticks, false, error, node);
#endif
      }

      uint8_t  transWrite             (const uint32_t addr, const uint8_t  data, const int prot = 0)                       {return VTransWrite(addr, data, prot, node);}
      uint16_t transWrite             (const uint32_t addr, const uint16_t data, const int prot = 0)                       {return VTransWrite(addr, data, prot, node);}
      uint32_t transWrite             (const uint32_t addr, const uint32_t data, const int prot = 0)                       {return VTransWrite(addr, data, prot, node);}
      uint8_t  transWrite             (const uint64_t addr, const uint8_t  data, const int prot = 0)                       {return VTransWrite(addr, data, prot, node);}
      uint16_t transWrite             (const uint64_t addr, const uint16_t data, const int prot = 0)                       {return VTransWrite(addr, data, prot, node);}
      uint32_t transWrite             (const uint64_t addr, const uint32_t data, const int prot = 0)                       {return VTransWrite(addr, data, prot, node);}
      uint64_t transWrite             (const uint64_t addr, const uint64_t data, const int prot = 0)                       {return VTransWrite(addr, data, prot, node);}

      uint8_t  transWriteAsync        (const uint32_t addr, const uint8_t  data, const int prot = 0)                       {return VTransWriteAsync(addr, data, prot, node);}
      uint16_t transWriteAsync        (const uint32_t addr, const uint16_t data, const int prot = 0)                       {return VTransWriteAsync(addr, data, prot, node);}
      uint32_t transWriteAsync        (const uint32_t addr, const uint32_t data, const int prot = 0)                       {return VTransWriteAsync(addr, data, prot, node);}
      uint8_t  transWriteAsync        (const uint64_t addr, const uint8_t  data, const int prot = 0)                       {return VTransWriteAsync(addr, data, prot, node);}
      uint16_t transWriteAsync        (const uint64_t addr, const uint16_t data, const int prot = 0)                       {return VTransWriteAsync(addr, data, prot, node);}
      uint32_t transWriteAsync        (const uint64_t addr, const uint32_t data, const int prot = 0)                       {return VTransWriteAsync(addr, data, prot, node);}
      uint64_t transWriteAsync        (const uint64_t addr, const uint64_t data, const int prot = 0)                       {return VTransWriteAsync(addr, data, prot, node);}

      void     transWriteAndRead      (const uint32_t addr, const uint8_t  wdata, uint8_t  *rdata, const int prot = 0)     {*rdata = VTransWriteAndRead(addr, wdata, prot, node);}
      void     transWriteAndRead      (const uint32_t addr, const uint16_t wdata, uint16_t *rdata, const int prot = 0)     {*rdata = VTransWriteAndRead(addr, wdata, prot, node);}
      void     transWriteAndRead      (const uint32_t addr, const uint32_t wdata, uint32_t *rdata, const int prot = 0)     {*rdata = VTransWriteAndRead(addr, wdata, prot, node);}
      void     transWriteAndRead      (const uint64_t addr, const uint8_t  wdata, uint8_t  *rdata, const int prot = 0)     {*rdata = VTransWriteAndRead(addr, wdata, prot, node);}
      void     transWriteAndRead      (const uint64_t addr, const uint16_t wdata, uint16_t *rdata, const int prot = 0)     {*rdata = VTransWriteAndRead(addr, wdata, prot, node);}
      void     transWriteAndRead      (const uint64_t addr, const uint32_t wdata, uint32_t *rdata, const int prot = 0)     {*rdata = VTransWriteAndRead(addr, wdata, prot, node);}
      void     transWriteAndRead      (const uint64_t addr, const uint64_t wdata, uint64_t *rdata, const int prot = 0)     {*rdata = VTransWriteAndRead(addr, wdata, prot, node);}

      void     transWriteAndReadAsync (const uint32_t addr, const uint8_t  wdata, uint8_t  *rdata, const int prot = 0)     {*rdata = VTransWriteAndReadAsync(addr, wdata, prot, node);}
      void     transWriteAndReadAsync (const uint32_t addr, const uint16_t wdata, uint16_t *rdata, const int prot = 0)     {*rdata = VTransWriteAndReadAsync(addr, wdata, prot, node);}
      void     transWriteAndReadAsync (const uint32_t addr, const uint32_t wdata, uint32_t *rdata, const int prot = 0)     {*rdata = VTransWriteAndReadAsync(addr, wdata, prot, node);}
      void     transWriteAndReadAsync (const uint64_t addr, const uint8_t  wdata, uint8_t  *rdata, const int prot = 0)     {*rdata = VTransWriteAndReadAsync(addr, wdata, prot, node);}
      void     transWriteAndReadAsync (const uint64_t addr, const uint16_t wdata, uint16_t *rdata, const int prot = 0)     {*rdata = VTransWriteAndReadAsync(addr, wdata, prot, node);}
      void     transWriteAndReadAsync (const uint64_t addr, const uint32_t wdata, uint32_t *rdata, const int prot = 0)     {*rdata = VTransWriteAndReadAsync(addr, wdata, prot, node);}
      void     transWriteAndReadAsync (const uint64_t addr, const uint64_t wdata, uint64_t *rdata, const int prot = 0)     {*rdata = VTransWriteAndReadAsync(addr, wdata, prot, node);}

      void     transWriteAddressAsync        (const uint32_t addr)                                                         {VTransWriteAddressAsync(addr, node);}
      void     transWriteAddressAsync        (const uint64_t addr)                                                         {VTransWriteAddressAsync(addr, node);}

      void     transWriteDataAsync           (const uint8_t  data, uint32_t bytelane = 0)                                  {VTransWriteDataAsync(data, bytelane, node);}
      void     transWriteDataAsync           (const uint16_t data, uint32_t bytelane = 0)                                  {VTransWriteDataAsync(data, bytelane, node);}
      void     transWriteDataAsync           (const uint32_t data, uint32_t bytelane = 0)                                  {VTransWriteDataAsync(data, bytelane, node);}
      void     transWriteDataAsync           (const uint64_t data, uint32_t bytelane = 0)                                  {VTransWriteDataAsync(data, bytelane, node);}

      void     transReadAddressAsync         (const uint32_t addr)                                                         {VTransReadAddressAsync(addr, node);}
      void     transReadAddressAsync         (const uint64_t addr)                                                         {VTransReadAddressAsync(addr, node);}

      void     transReadData                 (uint8_t       *data)                                                         {VTransReadData(data, node);}
      void     transReadData                 (uint16_t      *data)                                                         {VTransReadData(data, node);}
      void     transReadData                 (uint32_t      *data)                                                         {VTransReadData(data, node);}
      void     transReadData                 (uint64_t      *data)                                                         {VTransReadData(data, node);}

      bool     transTryReadData              (uint8_t       *data)                                                         {return VTransTryReadData(data, node);}
      bool     transTryReadData              (uint16_t      *data)                                                         {return VTransTryReadData(data, node);}
      bool     transTryReadData              (uint32_t      *data)                                                         {return VTransTryReadData(data, node);}
      bool     transTryReadData              (uint64_t      *data)                                                         {return VTransTryReadData(data, node);}

      void     transReadDataCheck            (uint8_t        data)                                                         {VTransReadCheckData(data, node);}
      void     transReadDataCheck            (uint16_t       data)                                                         {VTransReadCheckData(data, node);}
      void     transReadDataCheck            (uint32_t       data)                                                         {VTransReadCheckData(data, node);}
      void     transReadDataCheck            (uint64_t       data)                                                         {VTransReadCheckData(data, node);}

      bool     transTryReadDataCheck         (uint8_t        data)                                                         {return VTransTryReadCheckData(data, node);}
      bool     transTryReadDataCheck         (uint16_t       data)                                                         {return VTransTryReadCheckData(data, node);}
      bool     transTryReadDataCheck         (uint32_t       data)                                                         {return VTransTryReadCheckData(data, node);}
      bool     transTryReadDataCheck         (uint64_t       data)                                                         {return VTransTryReadCheckData(data, node);}

      void     transRead                     (const uint32_t addr, uint8_t  *data, const int prot = 0)                     {VTransRead(addr, data, prot, node);}
      void     transRead                     (const uint32_t addr, uint16_t *data, const int prot = 0)                     {VTransRead(addr, data, prot, node);}
      void     transRead                     (const uint32_t addr, uint32_t *data, const int prot = 0)                     {VTransRead(addr, data, prot, node);}
      void     transRead                     (const uint64_t addr, uint8_t  *data, const int prot = 0)                     {VTransRead(addr, data, prot, node);}
      void     transRead                     (const uint64_t addr, uint16_t *data, const int prot = 0)                     {VTransRead(addr, data, prot, node);}
      void     transRead                     (const uint64_t addr, uint32_t *data, const int prot = 0)                     {VTransRead(addr, data, prot, node);}
      void     transRead                     (const uint64_t addr, uint64_t *data, const int prot = 0)                     {VTransRead(addr, data, prot, node);}

      void     transReadPoll                 (const uint32_t addr, uint8_t  *data, const int idx, const int bitval, const int waittime = 10, const int prot = 0)
                                                 {do {tick(waittime); transRead(addr, data, prot);} while((*data & (1 << idx)) != ((bitval & 1) << idx));}

      void     transReadPoll                 (const uint32_t addr, uint16_t *data, const int idx, const int bitval, const int waittime = 10, const int prot = 0)
                                                 {do {tick(waittime); transRead(addr, data, prot);} while((*data & (1 << idx)) != ((bitval & 1) << idx));}

      void     transReadPoll                 (const uint32_t addr, uint32_t *data, const int idx, const int bitval, const int waittime = 10, const int prot = 0)
                                                 {do {tick(waittime); transRead(addr, data, prot);} while((*data & (1 << idx)) != ((bitval & 1) << idx));}

      void     transReadPoll                 (const uint64_t addr, uint8_t  *data, const int idx, const int bitval, const int waittime = 10, const int prot = 0)
                                                 {do {tick(waittime); transRead(addr, data, prot);} while((*data & (1 << idx)) != ((bitval & 1) << idx));}

      void     transReadPoll                 (const uint64_t addr, uint16_t *data, const int idx, const int bitval, const int waittime = 10, const int prot = 0)
                                                 {do {tick(waittime); transRead(addr, data, prot);} while((*data & (1 << idx)) != ((bitval & 1) << idx));}

      void     transReadPoll                 (const uint64_t addr, uint32_t *data, const int idx, const int bitval, const int waittime = 10, const int prot = 0)
                                                 {do {tick(waittime); transRead(addr, data, prot);} while((*data & (1 << idx)) != ((bitval & 1) << idx));}

      void     transReadPoll                 (const uint64_t addr, uint64_t *data, const int idx, const int bitval, const int waittime = 10, const int prot = 0)
                                                 {do {tick(waittime); transRead(addr, data, prot);} while((*data & (1 << idx)) != ((bitval & 1) << idx));}

      void     transReadCheck                (const uint32_t addr, uint8_t   data, const int prot = 0)                     {VTransReadCheck(addr, data, prot, node);}
      void     transReadCheck                (const uint32_t addr, uint16_t  data, const int prot = 0)                     {VTransReadCheck(addr, data, prot, node);}
      void     transReadCheck                (const uint32_t addr, uint32_t  data, const int prot = 0)                     {VTransReadCheck(addr, data, prot, node);}
      void     transReadCheck                (const uint64_t addr, uint8_t   data, const int prot = 0)                     {VTransReadCheck(addr, data, prot, node);}
      void     transReadCheck                (const uint64_t addr, uint16_t  data, const int prot = 0)                     {VTransReadCheck(addr, data, prot, node);}
      void     transReadCheck                (const uint64_t addr, uint32_t  data, const int prot = 0)                     {VTransReadCheck(addr, data, prot, node);}
      void     transReadCheck                (const uint64_t addr, uint64_t  data, const int prot = 0)                     {VTransReadCheck(addr, data, prot, node);}

      void     transBurstWrite               (const uint32_t addr, uint8_t  *data, const int bytesize, const int prot = 0) {VTransBurstWrite(addr, data, bytesize, prot, node);}
      void     transBurstWrite               (const uint64_t addr, uint8_t  *data, const int bytesize, const int prot = 0) {VTransBurstWrite(addr, data, bytesize, prot, node);}
      void     transBurstWrite               (const uint32_t addr, const int bytesize, const int prot = 0)                 {VTransBurstWrite(addr, bytesize, prot, node);}
      void     transBurstWrite               (const uint64_t addr, const int bytesize, const int prot = 0)                 {VTransBurstWrite(addr, bytesize, prot, node);}
      void     transBurstWriteAsync          (const uint32_t addr, uint8_t  *data, const int bytesize, const int prot = 0) {VTransBurstWriteAsync(addr, data, bytesize, prot, node);}
      void     transBurstWriteAsync          (const uint64_t addr, uint8_t  *data, const int bytesize, const int prot = 0) {VTransBurstWriteAsync(addr, data, bytesize, prot, node);}

      void     transBurstWriteIncrement      (const uint32_t addr, uint8_t   data, const int bytesize, const int prot = 0) {VTransBurstWriteIncrement(addr, &data, bytesize, prot, node);}
      void     transBurstWriteIncrement      (const uint64_t addr, uint8_t   data, const int bytesize, const int prot = 0) {VTransBurstWriteIncrement(addr, &data, bytesize, prot, node);}
      void     transBurstWriteIncrementAsync (const uint32_t addr, uint8_t   data, const int bytesize, const int prot = 0) {VTransBurstWriteIncrementAsync(addr, &data, bytesize, prot, node);}
      void     transBurstWriteIncrementAsync (const uint64_t addr, uint8_t   data, const int bytesize, const int prot = 0) {VTransBurstWriteIncrementAsync(addr, &data, bytesize, prot, node);}
      void     transBurstWriteRandom         (const uint32_t addr, uint8_t   data, const int bytesize, const int prot = 0) {VTransBurstWriteRandom(addr, &data, bytesize, prot, node);}
      void     transBurstWriteRandom         (const uint64_t addr, uint8_t   data, const int bytesize, const int prot = 0) {VTransBurstWriteRandom(addr, &data, bytesize, prot, node);}
      void     transBurstWriteRandomAsync    (const uint32_t addr, uint8_t   data, const int bytesize, const int prot = 0) {VTransBurstWriteRandomAsync(addr, &data, bytesize, prot, node);}
      void     transBurstWriteRandomAsync    (const uint64_t addr, uint8_t   data, const int bytesize, const int prot = 0) {VTransBurstWriteRandomAsync(addr, &data, bytesize, prot, node);}

      void     transBurstPushData            (      uint8_t *data, const int bytesize)                                     {VTransBurstPushData(data, bytesize, node);}
      void     transBurstPushIncrement       (      uint8_t  data, const int bytesize)                                     {VTransBurstPushIncrement(&data, bytesize, node);}
      void     transBurstPushRandom          (      uint8_t  data, const int bytesize)                                     {VTransBurstPushRandom(&data, bytesize, node);}

      void     transBurstRead                (const uint32_t addr, uint8_t  *data, const int bytesize, const int prot = 0) {VTransBurstRead (addr, data, bytesize, prot, node);}
      void     transBurstRead                (const uint64_t addr, uint8_t  *data, const int bytesize, const int prot = 0) {VTransBurstRead (addr, data, bytesize, prot, node);}
      void     transBurstRead                (const uint32_t addr, const int bytesize, const int prot = 0)                 {VTransBurstRead (addr, bytesize, prot, node);}
      void     transBurstRead                (const uint64_t addr, const int bytesize, const int prot = 0)                 {VTransBurstRead (addr, bytesize, prot, node);}
      void     transBurstReadCheckIncrement  (const uint32_t addr, uint8_t   data, const int bytesize, const int prot = 0) {VTransCheckBurstReadIncrement(addr, &data, bytesize, prot, node);}
      void     transBurstReadCheckIncrement  (const uint64_t addr, uint8_t   data, const int bytesize, const int prot = 0) {VTransCheckBurstReadIncrement(addr, &data, bytesize, prot, node);}
      void     transBurstReadCheckRandom     (const uint32_t addr, uint8_t   data, const int bytesize, const int prot = 0) {VTransCheckBurstReadRandom(addr, &data, bytesize, prot, node);}
      void     transBurstReadCheckRandom     (const uint64_t addr, uint8_t   data, const int bytesize, const int prot = 0) {VTransCheckBurstReadRandom(addr, &data, bytesize, prot, node);}
      void     transBurstPopData             (uint8_t  *data, const int bytesize)                                          {VTransBurstPopData(data, bytesize, node);}

      void     transBurstCheckIncrement      (      uint8_t  data, const int bytesize)                                     {VTransCheckBurstIncrement(&data, bytesize, node);}
      void     transBurstCheckRandom         (      uint8_t  data, const int bytesize)                                     {VTransCheckBurstRandom   (&data, bytesize, node);}

      void     regInterruptCB                (pVUserInt_t func)                                                            {VRegInterrupt(func, node);}

      void     waitForSim                    (void)                                                                        {VWaitForSim(node);}

      int      getNodeNumber                 (void)                                                                        {return node;}

private:

      int      node;
};

#endif