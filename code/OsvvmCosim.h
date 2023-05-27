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
                const int max_data_buf_size = DATABUF_SIZE;

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

      uint8_t  transWrite             (uint32_t addr, const uint8_t  data, const int prot = 0)                   {return VTransUserCommon(WRITE_OP, &addr, data, &dummyStatus, prot, node);}
      uint16_t transWrite             (uint32_t addr, const uint16_t data, const int prot = 0)                   {return VTransUserCommon(WRITE_OP, &addr, data, &dummyStatus, prot, node);}
      uint32_t transWrite             (uint32_t addr, const uint32_t data, const int prot = 0)                   {return VTransUserCommon(WRITE_OP, &addr, data, &dummyStatus, prot, node);}
      uint8_t  transWrite             (uint64_t addr, const uint8_t  data, const int prot = 0)                   {return VTransUserCommon(WRITE_OP, &addr, data, &dummyStatus, prot, node);}
      uint16_t transWrite             (uint64_t addr, const uint16_t data, const int prot = 0)                   {return VTransUserCommon(WRITE_OP, &addr, data, &dummyStatus, prot, node);}
      uint32_t transWrite             (uint64_t addr, const uint32_t data, const int prot = 0)                   {return VTransUserCommon(WRITE_OP, &addr, data, &dummyStatus, prot, node);}
      uint64_t transWrite             (uint64_t addr, const uint64_t data, const int prot = 0)                   {return VTransUserCommon(WRITE_OP, &addr, data, &dummyStatus, prot, node);}

      uint8_t  transWriteAsync        (uint32_t addr, const uint8_t  data, const int prot = 0)                   {return VTransUserCommon(ASYNC_WRITE, &addr, data, &dummyStatus, prot, node);}
      uint16_t transWriteAsync        (uint32_t addr, const uint16_t data, const int prot = 0)                   {return VTransUserCommon(ASYNC_WRITE, &addr, data, &dummyStatus, prot, node);}
      uint32_t transWriteAsync        (uint32_t addr, const uint32_t data, const int prot = 0)                   {return VTransUserCommon(ASYNC_WRITE, &addr, data, &dummyStatus, prot, node);}
      uint8_t  transWriteAsync        (uint64_t addr, const uint8_t  data, const int prot = 0)                   {return VTransUserCommon(ASYNC_WRITE, &addr, data, &dummyStatus, prot, node);}
      uint16_t transWriteAsync        (uint64_t addr, const uint16_t data, const int prot = 0)                   {return VTransUserCommon(ASYNC_WRITE, &addr, data, &dummyStatus, prot, node);}
      uint32_t transWriteAsync        (uint64_t addr, const uint32_t data, const int prot = 0)                   {return VTransUserCommon(ASYNC_WRITE, &addr, data, &dummyStatus, prot, node);}
      uint64_t transWriteAsync        (uint64_t addr, const uint64_t data, const int prot = 0)                   {return VTransUserCommon(ASYNC_WRITE, &addr, data, &dummyStatus, prot, node);}

      void     transWriteAndRead      (uint32_t addr, const uint8_t  wdata, uint8_t  *rdata, const int prot = 0) {*rdata = VTransUserCommon(WRITE_AND_READ, &addr, wdata, &dummyStatus, prot, node);}
      void     transWriteAndRead      (uint32_t addr, const uint16_t wdata, uint16_t *rdata, const int prot = 0) {*rdata = VTransUserCommon(WRITE_AND_READ, &addr, wdata, &dummyStatus, prot, node);}
      void     transWriteAndRead      (uint32_t addr, const uint32_t wdata, uint32_t *rdata, const int prot = 0) {*rdata = VTransUserCommon(WRITE_AND_READ, &addr, wdata, &dummyStatus, prot, node);}
      void     transWriteAndRead      (uint64_t addr, const uint8_t  wdata, uint8_t  *rdata, const int prot = 0) {*rdata = VTransUserCommon(WRITE_AND_READ, &addr, wdata, &dummyStatus, prot, node);}
      void     transWriteAndRead      (uint64_t addr, const uint16_t wdata, uint16_t *rdata, const int prot = 0) {*rdata = VTransUserCommon(WRITE_AND_READ, &addr, wdata, &dummyStatus, prot, node);}
      void     transWriteAndRead      (uint64_t addr, const uint32_t wdata, uint32_t *rdata, const int prot = 0) {*rdata = VTransUserCommon(WRITE_AND_READ, &addr, wdata, &dummyStatus, prot, node);}
      void     transWriteAndRead      (uint64_t addr, const uint64_t wdata, uint64_t *rdata, const int prot = 0) {*rdata = VTransUserCommon(WRITE_AND_READ, &addr, wdata, &dummyStatus, prot, node);}

      void     transWriteAndReadAsync (uint32_t addr, const uint8_t  wdata, const int prot = 0)                  {VTransUserCommon(ASYNC_WRITE_AND_READ, &addr, wdata, &dummyStatus, prot, node);}
      void     transWriteAndReadAsync (uint32_t addr, const uint16_t wdata, const int prot = 0)                  {VTransUserCommon(ASYNC_WRITE_AND_READ, &addr, wdata, &dummyStatus, prot, node);}
      void     transWriteAndReadAsync (uint32_t addr, const uint32_t wdata, const int prot = 0)                  {VTransUserCommon(ASYNC_WRITE_AND_READ, &addr, wdata, &dummyStatus, prot, node);}
      void     transWriteAndReadAsync (uint64_t addr, const uint8_t  wdata, const int prot = 0)                  {VTransUserCommon(ASYNC_WRITE_AND_READ, &addr, wdata, &dummyStatus, prot, node);}
      void     transWriteAndReadAsync (uint64_t addr, const uint16_t wdata, const int prot = 0)                  {VTransUserCommon(ASYNC_WRITE_AND_READ, &addr, wdata, &dummyStatus, prot, node);}
      void     transWriteAndReadAsync (uint64_t addr, const uint32_t wdata, const int prot = 0)                  {VTransUserCommon(ASYNC_WRITE_AND_READ, &addr, wdata, &dummyStatus, prot, node);}
      void     transWriteAndReadAsync (uint64_t addr, const uint64_t wdata, const int prot = 0)                  {VTransUserCommon(ASYNC_WRITE_AND_READ, &addr, wdata, &dummyStatus, prot, node);}

      void     transWriteAddressAsync        (uint32_t addr, const int prot = 0)                                 {VTransUserCommon(ASYNC_WRITE_ADDRESS, &addr, (uint32_t)0, &dummyStatus, prot, node);}
      void     transWriteAddressAsync        (uint64_t addr, const int prot = 0)                                 {VTransUserCommon(ASYNC_WRITE_ADDRESS, &addr, (uint32_t)0, &dummyStatus, prot, node);}

      void     transWriteDataAsync           (const uint8_t  data, uint32_t bytelane = 0)                        {VTransUserCommon(ASYNC_WRITE_DATA, &bytelane, data, &dummyStatus, 0, node);}
      void     transWriteDataAsync           (const uint16_t data, uint32_t bytelane = 0)                        {VTransUserCommon(ASYNC_WRITE_DATA, &bytelane, data, &dummyStatus, 0, node);}
      void     transWriteDataAsync           (const uint32_t data, uint32_t bytelane = 0)                        {VTransUserCommon(ASYNC_WRITE_DATA, &bytelane, data, &dummyStatus, 0, node);}
      void     transWriteDataAsync           (const uint64_t data, uint64_t bytelane = 0)                        {VTransUserCommon(ASYNC_WRITE_DATA, &bytelane, data, &dummyStatus, 0, node);}

      void     transReadAddressAsync         (uint32_t addr, const int prot = 0)                                 {VTransUserCommon(ASYNC_READ_ADDRESS, &addr, (uint32_t)0, &dummyStatus, prot, node);}
      void     transReadAddressAsync         (uint64_t addr, const int prot = 0)                                 {VTransUserCommon(ASYNC_READ_ADDRESS, &addr, (uint32_t)0, &dummyStatus, prot, node);}

      void     transReadData                 (uint8_t       *data)                                               {*data = VTransUserCommon(READ_DATA, &dummyAddr32, (uint8_t)0, &dummyStatus, 0, node);}
      void     transReadData                 (uint16_t      *data)                                               {*data = VTransUserCommon(READ_DATA, &dummyAddr32, (uint8_t)0, &dummyStatus, 0, node);}
      void     transReadData                 (uint32_t      *data)                                               {*data = VTransUserCommon(READ_DATA, &dummyAddr32, (uint8_t)0, &dummyStatus, 0, node);}
      void     transReadData                 (uint64_t      *data)                                               {*data = VTransUserCommon(READ_DATA, &dummyAddr64, (uint8_t)0, &dummyStatus, 0, node);}

      bool     transTryReadData              (uint8_t       *data)                                               {int status; *data = VTransUserCommon(ASYNC_READ_DATA, &dummyAddr32, (uint8_t) 0, &status, 0, node); return status;}
      bool     transTryReadData              (uint16_t      *data)                                               {int status; *data = VTransUserCommon(ASYNC_READ_DATA, &dummyAddr32, (uint16_t)0, &status, 0, node); return status;}
      bool     transTryReadData              (uint32_t      *data)                                               {int status; *data = VTransUserCommon(ASYNC_READ_DATA, &dummyAddr32, (uint32_t)0, &status, 0, node); return status;}
      bool     transTryReadData              (uint64_t      *data)                                               {int status; *data = VTransUserCommon(ASYNC_READ_DATA, &dummyAddr64, (uint64_t)0, &status, 0, node); return status;}

      void     transReadDataCheck            (uint8_t        data)                                               {VTransUserCommon(READ_DATA_CHECK, &dummyAddr32, data, &dummyStatus, (uint32_t)0, node);}
      void     transReadDataCheck            (uint16_t       data)                                               {VTransUserCommon(READ_DATA_CHECK, &dummyAddr32, data, &dummyStatus, (uint32_t)0, node);}
      void     transReadDataCheck            (uint32_t       data)                                               {VTransUserCommon(READ_DATA_CHECK, &dummyAddr32, data, &dummyStatus, (uint32_t)0, node);}
      void     transReadDataCheck            (uint64_t       data)                                               {VTransUserCommon(READ_DATA_CHECK, &dummyAddr64, data, &dummyStatus, (uint32_t)0, node);}

      bool     transTryReadDataCheck         (uint8_t        data)                                               {int status; VTransUserCommon(ASYNC_READ_DATA_CHECK, &dummyAddr32, data, &status, (uint32_t)0, node); return status;}
      bool     transTryReadDataCheck         (uint16_t       data)                                               {int status; VTransUserCommon(ASYNC_READ_DATA_CHECK, &dummyAddr32, data, &status, (uint32_t)0, node); return status;}
      bool     transTryReadDataCheck         (uint32_t       data)                                               {int status; VTransUserCommon(ASYNC_READ_DATA_CHECK, &dummyAddr32, data, &status, (uint32_t)0, node); return status;}
      bool     transTryReadDataCheck         (uint64_t       data)                                               {int status; VTransUserCommon(ASYNC_READ_DATA_CHECK, &dummyAddr64, data, &status, (uint32_t)0, node); return status;}

      void     transRead                     (uint32_t addr, uint8_t  *data, const int prot = 0)                 {*data = VTransUserCommon(READ_OP, &addr, (uint8_t) 0, &dummyStatus, prot, node);}
      void     transRead                     (uint32_t addr, uint16_t *data, const int prot = 0)                 {*data = VTransUserCommon(READ_OP, &addr, (uint16_t)0, &dummyStatus, prot, node);}
      void     transRead                     (uint32_t addr, uint32_t *data, const int prot = 0)                 {*data = VTransUserCommon(READ_OP, &addr, (uint32_t)0, &dummyStatus, prot, node);}
      void     transRead                     (uint64_t addr, uint8_t  *data, const int prot = 0)                 {*data = VTransUserCommon(READ_OP, &addr, (uint8_t) 0, &dummyStatus, prot, node);}
      void     transRead                     (uint64_t addr, uint16_t *data, const int prot = 0)                 {*data = VTransUserCommon(READ_OP, &addr, (uint16_t)0, &dummyStatus, prot, node);}
      void     transRead                     (uint64_t addr, uint32_t *data, const int prot = 0)                 {*data = VTransUserCommon(READ_OP, &addr, (uint32_t)0, &dummyStatus, prot, node);}
      void     transRead                     (uint64_t addr, uint64_t *data, const int prot = 0)                 {*data = VTransUserCommon(READ_OP, &addr, (uint64_t)0, &dummyStatus, prot, node);}


      void     transReadCheck                (uint32_t addr, uint8_t   data, const int prot = 0)                 {VTransUserCommon(READ_CHECK, &addr, data, &dummyStatus, prot, node);}
      void     transReadCheck                (uint32_t addr, uint16_t  data, const int prot = 0)                 {VTransUserCommon(READ_CHECK, &addr, data, &dummyStatus, prot, node);}
      void     transReadCheck                (uint32_t addr, uint32_t  data, const int prot = 0)                 {VTransUserCommon(READ_CHECK, &addr, data, &dummyStatus, prot, node);}
      void     transReadCheck                (uint64_t addr, uint8_t   data, const int prot = 0)                 {VTransUserCommon(READ_CHECK, &addr, data, &dummyStatus, prot, node);}
      void     transReadCheck                (uint64_t addr, uint16_t  data, const int prot = 0)                 {VTransUserCommon(READ_CHECK, &addr, data, &dummyStatus, prot, node);}
      void     transReadCheck                (uint64_t addr, uint32_t  data, const int prot = 0)                 {VTransUserCommon(READ_CHECK, &addr, data, &dummyStatus, prot, node);}
      void     transReadCheck                (uint64_t addr, uint64_t  data, const int prot = 0)                 {VTransUserCommon(READ_CHECK, &addr, data, &dummyStatus, prot, node);}


      void     transReadPoll                 (uint32_t addr, uint8_t  *data, const int idx, const int bitval, const int waittime = 10, const int prot = 0)
                                                 {do {tick(waittime); transRead(addr, data, prot);} while((*data & (1 << idx)) != ((bitval & 1) << idx));}

      void     transReadPoll                 (uint32_t addr, uint16_t *data, const int idx, const int bitval, const int waittime = 10, const int prot = 0)
                                                 {do {tick(waittime); transRead(addr, data, prot);} while((*data & (1 << idx)) != ((bitval & 1) << idx));}

      void     transReadPoll                 (uint32_t addr, uint32_t *data, const int idx, const int bitval, const int waittime = 10, const int prot = 0)
                                                 {do {tick(waittime); transRead(addr, data, prot);} while((*data & (1 << idx)) != ((bitval & 1) << idx));}

      void     transReadPoll                 (uint64_t addr, uint8_t  *data, const int idx, const int bitval, const int waittime = 10, const int prot = 0)
                                                 {do {tick(waittime); transRead(addr, data, prot);} while((*data & (1 << idx)) != ((bitval & 1) << idx));}

      void     transReadPoll                 (uint64_t addr, uint16_t *data, const int idx, const int bitval, const int waittime = 10, const int prot = 0)
                                                 {do {tick(waittime); transRead(addr, data, prot);} while((*data & (1 << idx)) != ((bitval & 1) << idx));}

      void     transReadPoll                 (uint64_t addr, uint32_t *data, const int idx, const int bitval, const int waittime = 10, const int prot = 0)
                                                 {do {tick(waittime); transRead(addr, data, prot);} while((*data & (1 << idx)) != ((bitval & 1) << idx));}

      void     transReadPoll                 (uint64_t addr, uint64_t *data, const int idx, const int bitval, const int waittime = 10, const int prot = 0)
                                                 {do {tick(waittime); transRead(addr, data, prot);} while((*data & (1 << idx)) != ((bitval & 1) << idx));}

      void     transBurstWrite               (const uint32_t addr, uint8_t  *data, const int bytesize, const int prot = 0) {VTransBurstCommon(WRITE_BURST, BURST_NORM, addr, data, bytesize, prot, node);}
      void     transBurstWrite               (const uint64_t addr, uint8_t  *data, const int bytesize, const int prot = 0) {VTransBurstCommon(WRITE_BURST, BURST_NORM, addr, data, bytesize, prot, node);}
      void     transBurstWrite               (const uint32_t addr, const int bytesize, const int prot = 0)                 {VTransBurstCommon(WRITE_BURST, BURST_TRANS, addr, NULL, bytesize, prot, node);}
      void     transBurstWrite               (const uint64_t addr, const int bytesize, const int prot = 0)                 {VTransBurstCommon(WRITE_BURST, BURST_TRANS, addr, NULL, bytesize, prot, node);}
      void     transBurstWriteAsync          (const uint32_t addr, uint8_t  *data, const int bytesize, const int prot = 0) {VTransBurstCommon(ASYNC_WRITE_BURST, BURST_NORM, addr, data, bytesize, prot, node);}
      void     transBurstWriteAsync          (const uint64_t addr, uint8_t  *data, const int bytesize, const int prot = 0) {VTransBurstCommon(ASYNC_WRITE_BURST, BURST_NORM, addr, data, bytesize, prot, node);}

      void     transBurstWriteIncrement      (const uint32_t addr, uint8_t   data, const int bytesize, const int prot = 0) {VTransBurstCommon(WRITE_BURST, BURST_INCR, addr, &data, bytesize, prot, node);}
      void     transBurstWriteIncrement      (const uint64_t addr, uint8_t   data, const int bytesize, const int prot = 0) {VTransBurstCommon(WRITE_BURST, BURST_INCR, addr, &data, bytesize, prot, node);}
      void     transBurstWriteIncrementAsync (const uint32_t addr, uint8_t   data, const int bytesize, const int prot = 0) {VTransBurstCommon(ASYNC_WRITE_BURST, BURST_INCR, addr, &data, bytesize, prot, node);}
      void     transBurstWriteIncrementAsync (const uint64_t addr, uint8_t   data, const int bytesize, const int prot = 0) {VTransBurstCommon(ASYNC_WRITE_BURST, BURST_INCR, addr, &data, bytesize, prot, node);}
      void     transBurstWriteRandom         (const uint32_t addr, uint8_t   data, const int bytesize, const int prot = 0) {VTransBurstCommon(WRITE_BURST, BURST_RAND, addr, &data, bytesize, prot, node);}
      void     transBurstWriteRandom         (const uint64_t addr, uint8_t   data, const int bytesize, const int prot = 0) {VTransBurstCommon(WRITE_BURST, BURST_RAND, addr, &data, bytesize, prot, node);}
      void     transBurstWriteRandomAsync    (const uint32_t addr, uint8_t   data, const int bytesize, const int prot = 0) {VTransBurstCommon(ASYNC_WRITE_BURST, BURST_RAND, addr, &data, bytesize, prot, node);}
      void     transBurstWriteRandomAsync    (const uint64_t addr, uint8_t   data, const int bytesize, const int prot = 0) {VTransBurstCommon(ASYNC_WRITE_BURST, BURST_RAND, addr, &data, bytesize, prot, node);}

      void     transBurstPushData            (      uint8_t *data, const int bytesize)                                     {VTransBurstCommon(WRITE_BURST, BURST_DATA,      (uint32_t)0,  data, bytesize, (uint32_t)0, node);}
      void     transBurstPushIncrement       (      uint8_t  data, const int bytesize)                                     {VTransBurstCommon(WRITE_BURST, BURST_INCR_PUSH, (uint32_t)0, &data, bytesize, (uint32_t)0, node);}
      void     transBurstPushRandom          (      uint8_t  data, const int bytesize)                                     {VTransBurstCommon(WRITE_BURST, BURST_RAND_PUSH, (uint32_t)0, &data, bytesize, (uint32_t)0, node);}

      void     transBurstRead                (const uint32_t addr, uint8_t  *data, const int bytesize, const int prot = 0) {VTransBurstCommon(READ_BURST, BURST_NORM,  addr, data, bytesize, prot, node);}
      void     transBurstRead                (const uint64_t addr, uint8_t  *data, const int bytesize, const int prot = 0) {VTransBurstCommon(READ_BURST, BURST_NORM,  addr, data, bytesize, prot, node);}
      void     transBurstRead                (const uint32_t addr, const int bytesize, const int prot = 0)                 {VTransBurstCommon(READ_BURST, BURST_TRANS, addr, NULL, bytesize, prot, node);}
      void     transBurstRead                (const uint64_t addr, const int bytesize, const int prot = 0)                 {VTransBurstCommon(READ_BURST, BURST_TRANS, addr, NULL, bytesize, prot, node);}

      void     transBurstReadCheckIncrement  (const uint32_t addr, uint8_t   data, const int bytesize, const int prot = 0) {VTransBurstCommon(READ_BURST, BURST_INCR, addr, &data, bytesize, prot, node);}
      void     transBurstReadCheckIncrement  (const uint64_t addr, uint8_t   data, const int bytesize, const int prot = 0) {VTransBurstCommon(READ_BURST, BURST_INCR, addr, &data, bytesize, prot, node);}
      void     transBurstReadCheckRandom     (const uint32_t addr, uint8_t   data, const int bytesize, const int prot = 0) {VTransBurstCommon(READ_BURST, BURST_RAND, addr, &data, bytesize, prot, node);}
      void     transBurstReadCheckRandom     (const uint64_t addr, uint8_t   data, const int bytesize, const int prot = 0) {VTransBurstCommon(READ_BURST, BURST_RAND, addr, &data, bytesize, prot, node);}

      void     transBurstPopData             (uint8_t  *data, const int bytesize)                                          {VTransBurstCommon(READ_BURST, BURST_DATA, (uint32_t)0, data, bytesize, 0, node);}

      void     transBurstCheckIncrement      (      uint8_t  data, const int bytesize)                                     {VTransBurstCommon(READ_BURST, BURST_INCR_CHECK, (uint32_t)0, &data, bytesize, (uint32_t)0, node);}
      void     transBurstCheckRandom         (      uint8_t  data, const int bytesize)                                     {VTransBurstCommon(READ_BURST, BURST_RAND_CHECK, (uint32_t)0, &data, bytesize, (uint32_t)0, node);}

      bool     transBurstCheckData           (      uint8_t *expdata, const int bytesize)
                                             {uint8_t buf[max_data_buf_size]; transBurstPopData(buf, bytesize); return cmpBuffers(buf, expdata, bytesize);}

      bool     transBurstReadCheckData       (const uint32_t addr, uint8_t *expdata, const int bytesize, const int prot = 0)
                                             {uint8_t buf[max_data_buf_size]; transBurstRead(addr, buf, bytesize, prot); return cmpBuffers(buf, expdata, bytesize);}

      bool     transBurstReadCheckData       (const uint64_t addr, uint8_t *expdata, const int bytesize, const int prot = 0)
                                             {uint8_t buf[max_data_buf_size]; transBurstRead(addr, buf, bytesize, prot); return cmpBuffers(buf, expdata, bytesize);}

      void     transWaitForTransaction       (void)                                                                        {VTransTransactionWait(WAIT_FOR_TRANSACTION, node);}
      void     transWaitForWriteTransaction  (void)                                                                        {VTransTransactionWait(WAIT_FOR_WRITE_TRANSACTION, node);}
      void     transWaitForReadTransaction   (void)                                                                        {VTransTransactionWait(WAIT_FOR_READ_TRANSACTION, node);}
      int      transGetTransactionCount      (void)                                                                        {return VTransGetCount(GET_TRANSACTION_COUNT, node);}
      int      transGetWriteTransactionCount (void)                                                                        {return VTransGetCount(GET_WRITE_TRANSACTION_COUNT, node);}
      int      transGetReadTransactionCount  (void)                                                                        {return VTransGetCount(GET_READ_TRANSACTION_COUNT, node);}

      void     regInterruptCB                (pVUserInt_t func)                                                            {VRegInterrupt(func, node);}

      void     waitForSim                    (void)                                                                        {VWaitForSim(node);}

      int      getNodeNumber                 (void)                                                                        {return node;}

private:

      bool     cmpBuffers                    (const uint8_t *got, const uint8_t *exp, const int bytesize) {for (int i=0; i < bytesize;i++) if (got[i] != exp[i]) return true; return false;}

      int      dummyStatus;
      uint32_t dummyAddr32;
      uint64_t dummyAddr64;

      int      node;
};

#endif