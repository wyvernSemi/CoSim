// =========================================================================
//
//  File Name:         OsvvmCosimResp.h
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
//      side responder code. Virtualises away top level VP user
//      thread routines.
//
//  Revision History:
//    Date      Version    Description
//    05/2023   2023.05    Initial revision
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

#ifndef __OSVVM_COSIM_RESP_H_
#define __OSVVM_COSIM_RESP_H_

class OsvvmCosimResp
{
public:
                const int max_data_buf_size = DATABUF_SIZE;

                OsvvmCosimResp (int nodeIn = 1, std::string test_name = "") : node(nodeIn) {
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
      void      respGetWrite                 (uint32_t *addr, uint8_t*  data)     {*data = VTransUserCommon(WRITE_OP, addr, (uint8_t) 0, &dummyStatus, 0, node);}
      void      respGetWrite                 (uint32_t *addr, uint16_t* data)     {*data = VTransUserCommon(WRITE_OP, addr, (uint16_t)0, &dummyStatus, 0, node);}
      void      respGetWrite                 (uint32_t *addr, uint32_t* data)     {*data = VTransUserCommon(WRITE_OP, addr, (uint32_t)0, &dummyStatus, 0, node);}
      void      respGetWrite                 (uint64_t *addr, uint8_t*  data)     {*data = VTransUserCommon(WRITE_OP, addr, (uint8_t)0,  &dummyStatus, 0, node);}
      void      respGetWrite                 (uint64_t *addr, uint16_t* data)     {*data = VTransUserCommon(WRITE_OP, addr, (uint16_t)0, &dummyStatus, 0, node);}
      void      respGetWrite                 (uint64_t *addr, uint32_t* data)     {*data = VTransUserCommon(WRITE_OP, addr, (uint32_t)0, &dummyStatus, 0, node);}
      void      respGetWrite                 (uint64_t *addr, uint64_t* data)     {*data = VTransUserCommon(WRITE_OP, addr, (uint64_t)0, &dummyStatus, 0, node);}

      bool      respTryGetWrite              (uint32_t *addr, uint8_t*  data)     {int status; *data = VTransUserCommon(ASYNC_WRITE, addr, (uint8_t) 0, &status, 0, node); return status;}
      bool      respTryGetWrite              (uint32_t *addr, uint16_t* data)     {int status; *data = VTransUserCommon(ASYNC_WRITE, addr, (uint16_t)0, &status, 0, node); return status;}
      bool      respTryGetWrite              (uint32_t *addr, uint32_t* data)     {int status; *data = VTransUserCommon(ASYNC_WRITE, addr, (uint32_t)0, &status, 0, node); return status;}
      bool      respTryGetWrite              (uint64_t *addr, uint8_t*  data)     {int status; *data = VTransUserCommon(ASYNC_WRITE, addr, (uint8_t)0,  &status, 0, node); return status;}
      bool      respTryGetWrite              (uint64_t *addr, uint16_t* data)     {int status; *data = VTransUserCommon(ASYNC_WRITE, addr, (uint16_t)0, &status, 0, node); return status;}
      bool      respTryGetWrite              (uint64_t *addr, uint32_t* data)     {int status; *data = VTransUserCommon(ASYNC_WRITE, addr, (uint32_t)0, &status, 0, node); return status;}
      bool      respTryGetWrite              (uint64_t *addr, uint64_t* data)     {int status; *data = VTransUserCommon(ASYNC_WRITE, addr, (uint64_t)0, &status, 0, node); return status;}

      void      respGetWriteAddress          (uint32_t* addr)                     {VTransUserCommon(WRITE_ADDRESS, addr, (uint32_t)0, &dummyStatus, 0, node);}
      void      respGetWriteAddress          (uint64_t* addr)                     {VTransUserCommon(WRITE_ADDRESS, addr, (uint64_t)0, &dummyStatus, 0, node);}

      bool      respTryGetWriteAddress       (uint32_t* addr)                     {int status; VTransUserCommon(ASYNC_WRITE_ADDRESS, addr, (uint32_t)0, &status, 0, node); return status;}
      bool      respTryGetWriteAddress       (uint64_t* addr)                     {int status; VTransUserCommon(ASYNC_WRITE_ADDRESS, addr, (uint64_t)0, &status, 0, node); return status;}

      void      respGetWriteData             (uint8_t*  data)                     {*data = VTransUserCommon(WRITE_DATA, &dummyAddr32, (uint8_t)0,  &dummyStatus, 0, node);}
      void      respGetWriteData             (uint16_t* data)                     {*data = VTransUserCommon(WRITE_DATA, &dummyAddr32, (uint16_t)0, &dummyStatus, 0, node);}
      void      respGetWriteData             (uint32_t* data)                     {*data = VTransUserCommon(WRITE_DATA, &dummyAddr32, (uint32_t)0, &dummyStatus, 0, node);}
      void      respGetWriteData             (uint64_t* data)                     {*data = VTransUserCommon(WRITE_DATA, &dummyAddr64, (uint64_t)0, &dummyStatus, 0, node);}

      bool      respTryGetWriteData          (uint8_t*  data)                     {int status; *data = VTransUserCommon(ASYNC_WRITE_DATA, &dummyAddr32, (uint8_t)0,  &status, 0, node); return status;}
      bool      respTryGetWriteData          (uint16_t* data)                     {int status; *data = VTransUserCommon(ASYNC_WRITE_DATA, &dummyAddr32, (uint16_t)0, &status, 0, node); return status;}
      bool      respTryGetWriteData          (uint32_t* data)                     {int status; *data = VTransUserCommon(ASYNC_WRITE_DATA, &dummyAddr32, (uint32_t)0, &status, 0, node); return status;}
      bool      respTryGetWriteData          (uint64_t* data)                     {int status; *data = VTransUserCommon(ASYNC_WRITE_DATA, &dummyAddr64, (uint64_t)0, &status, 0, node); return status;}


      void      respSendRead                 (uint32_t* addr, uint8_t  data)      {VTransUserCommon(READ_OP, addr, data, &dummyStatus, 0, node);}
      void      respSendRead                 (uint32_t* addr, uint16_t data)      {VTransUserCommon(READ_OP, addr, data, &dummyStatus, 0, node);}
      void      respSendRead                 (uint32_t* addr, uint32_t data)      {VTransUserCommon(READ_OP, addr, data, &dummyStatus, 0, node);}
      void      respSendRead                 (uint64_t* addr, uint8_t  data)      {VTransUserCommon(READ_OP, addr, data, &dummyStatus, 0, node);}
      void      respSendRead                 (uint64_t* addr, uint16_t data)      {VTransUserCommon(READ_OP, addr, data, &dummyStatus, 0, node);}
      void      respSendRead                 (uint64_t* addr, uint32_t data)      {VTransUserCommon(READ_OP, addr, data, &dummyStatus, 0, node);}
      void      respSendRead                 (uint64_t* addr, uint64_t data)      {VTransUserCommon(READ_OP, addr, data, &dummyStatus, 0, node);}

      bool      respTrySendRead              (uint32_t* addr, uint8_t  data)      {int status; VTransUserCommon(ASYNC_READ, addr, data, &status, 0, node); return status;}
      bool      respTrySendRead              (uint32_t* addr, uint16_t data)      {int status; VTransUserCommon(ASYNC_READ, addr, data, &status, 0, node); return status;}
      bool      respTrySendRead              (uint32_t* addr, uint32_t data)      {int status; VTransUserCommon(ASYNC_READ, addr, data, &status, 0, node); return status;}
      bool      respTrySendRead              (uint64_t* addr, uint8_t  data)      {int status; VTransUserCommon(ASYNC_READ, addr, data, &status, 0, node); return status;}
      bool      respTrySendRead              (uint64_t* addr, uint16_t data)      {int status; VTransUserCommon(ASYNC_READ, addr, data, &status, 0, node); return status;}
      bool      respTrySendRead              (uint64_t* addr, uint32_t data)      {int status; VTransUserCommon(ASYNC_READ, addr, data, &status, 0, node); return status;}
      bool      respTrySendRead              (uint64_t* addr, uint64_t data)      {int status; VTransUserCommon(ASYNC_READ, addr, data, &status, 0, node); return status;}

      void      respGetReadAddress           (uint32_t* addr)                     {VTransUserCommon(READ_ADDRESS, addr, (uint32_t)0, &dummyStatus, 0, node);}
      void      respGetReadAddress           (uint64_t* addr)                     {VTransUserCommon(READ_ADDRESS, addr, (uint64_t)0, &dummyStatus, 0, node);}

      bool      respTryGetReadAddress        (uint32_t* addr)                     {int status; VTransUserCommon(ASYNC_READ_ADDRESS, addr, (uint32_t)0, &status, 0, node); return status;}
      bool      respTryGetReadAddress        (uint64_t* addr)                     {int status; VTransUserCommon(ASYNC_READ_ADDRESS, addr, (uint64_t)0, &status, 0, node); return status;}

      void      respSendReadData             (uint8_t  data)                      {VTransUserCommon(READ_DATA, &dummyAddr32, data, &dummyStatus, 0, node);}
      void      respSendReadData             (uint16_t data)                      {VTransUserCommon(READ_DATA, &dummyAddr32, data, &dummyStatus, 0, node);}
      void      respSendReadData             (uint32_t data)                      {VTransUserCommon(READ_DATA, &dummyAddr32, data, &dummyStatus, 0, node);}
      void      respSendReadData             (uint64_t data)                      {VTransUserCommon(READ_DATA, &dummyAddr64, data, &dummyStatus, 0, node);}

      bool      respSendReadDataAsync        (uint8_t  data)                      {int status; VTransUserCommon(ASYNC_READ_DATA, &dummyAddr32, data, &status, 0, node); return status;}
      bool      respSendReadDataAsync        (uint16_t data)                      {int status; VTransUserCommon(ASYNC_READ_DATA, &dummyAddr32, data, &status, 0, node); return status;}
      bool      respSendReadDataAsync        (uint32_t data)                      {int status; VTransUserCommon(ASYNC_READ_DATA, &dummyAddr32, data, &status, 0, node); return status;}
      bool      respSendReadDataAsync        (uint64_t data)                      {int status; VTransUserCommon(ASYNC_READ_DATA, &dummyAddr64, data, &status, 0, node); return status;}

      void      respWaitForTransaction       (void)                               {VTransTransactionWait(WAIT_FOR_TRANSACTION,       node);}
      void      respWaitForWriteTransaction  (void)                               {VTransTransactionWait(WAIT_FOR_WRITE_TRANSACTION, node);}
      void      respWaitForReadTransaction   (void)                               {VTransTransactionWait(WAIT_FOR_READ_TRANSACTION,  node);}

      int       respGetTransactionCount      (void)                               {return VTransGetCount(GET_TRANSACTION_COUNT,       node);}
      int       respGetWriteTransactionCount (void)                               {return VTransGetCount(GET_WRITE_TRANSACTION_COUNT, node);}
      int       respGetReadTransactionCount  (void)                               {return VTransGetCount(GET_READ_TRANSACTION_COUNT,  node);}

      void      waitForSim                   (void)                               {VWaitForSim(node);}

      int       getNodeNumber                (void)                               {return node;}

private:

      int      dummyStatus;
      uint32_t dummyAddr32;
      uint64_t dummyAddr64;
      int      node;
};

#endif