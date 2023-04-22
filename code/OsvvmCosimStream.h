// =========================================================================
//
//  File Name:         OsvvmCosimStream.h
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
//      side stream code. Virtualises away top level VP user thread routines.
//
//  Revision History:
//    Date      Version    Description
//    11/2022   2023.02    Initial revision
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

#ifndef __OSVVM_COSIM_STREAM_H_
#define __OSVVM_COSIM_STREAM_H_

class OsvvmCosimStream
{
public:
                OsvvmCosimStream (int nodeIn = 0, std::string test_name = "") : node(nodeIn) {
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

      uint8_t  streamSend                     (const uint8_t  data, const int param=0)                     {return VStreamCommon                 (SEND, data, param, node);}
      uint16_t streamSend                     (const uint16_t data, const int param=0)                     {return VStreamCommon                 (SEND, data, param, node);}
      uint32_t streamSend                     (const uint32_t data, const int param=0)                     {return VStreamCommon                 (SEND, data, param, node);}
      uint64_t streamSend                     (const uint64_t data, const int param=0)                     {return VStreamCommon                 (SEND, data, param, node);}

      uint8_t  streamSendAsync                (const uint8_t  data, const int param=0)                     {return VStreamCommon                 (SEND_ASYNC, data, param, node);}
      uint16_t streamSendAsync                (const uint16_t data, const int param=0)                     {return VStreamCommon                 (SEND_ASYNC, data, param, node);}
      uint32_t streamSendAsync                (const uint32_t data, const int param=0)                     {return VStreamCommon                 (SEND_ASYNC, data, param, node);}
      uint64_t streamSendAsync                (const uint64_t data, const int param=0)                     {return VStreamCommon                 (SEND_ASYNC, data, param, node);}

      void     streamGet                      (uint8_t  *data)                                             {int status; VStreamGetCommon         (GET, data, &status, 0, 0, node);}
      void     streamGet                      (uint16_t *data)                                             {int status; VStreamGetCommon         (GET, data, &status, 0, 0, node);}
      void     streamGet                      (uint32_t *data)                                             {int status; VStreamGetCommon         (GET, data, &status, 0, 0, node);}
      void     streamGet                      (uint64_t *data)                                             {int status; VStreamGetCommon         (GET, data, &status, 0, 0, node);}

      void     streamGet                      (uint8_t  *data,            int *status)                     {VStreamGetCommon                     (GET, data, status, 0, 0, node);}
      void     streamGet                      (uint16_t *data,            int *status)                     {VStreamGetCommon                     (GET, data, status, 0, 0, node);}
      void     streamGet                      (uint32_t *data,            int *status)                     {VStreamGetCommon                     (GET, data, status, 0, 0, node);}
      void     streamGet                      (uint64_t *data,            int *status)                     {VStreamGetCommon                     (GET, data, status, 0, 0, node);}

      bool     streamTryGet                   (uint8_t  *data)                                             {int status; return VStreamGetCommon  (TRY_GET, data, &status, 0, 0, node);}
      bool     streamTryGet                   (uint16_t *data)                                             {int status; return VStreamGetCommon  (TRY_GET, data, &status, 0, 0, node);}
      bool     streamTryGet                   (uint32_t *data)                                             {int status; return VStreamGetCommon  (TRY_GET, data, &status, 0, 0, node);}
      bool     streamTryGet                   (uint64_t *data)                                             {int status; return VStreamGetCommon  (TRY_GET, data, &status, 0, 0, node);}

      bool     streamTryGet                   (uint8_t  *data,            int *status)                     {return VStreamGetCommon              (TRY_GET, data, status, 0, 0, node);}
      bool     streamTryGet                   (uint16_t *data,            int *status)                     {return VStreamGetCommon              (TRY_GET, data, status, 0, 0, node);}
      bool     streamTryGet                   (uint32_t *data,            int *status)                     {return VStreamGetCommon              (TRY_GET, data, status, 0, 0, node);}
      bool     streamTryGet                   (uint64_t *data,            int *status)                     {return VStreamGetCommon              (TRY_GET, data, status, 0, 0, node);}

      bool     streamTryCheck                 (const uint8_t  data, const int param=0)                     {int status; return VStreamGetCommon  (TRY_CHECK, null, &status, data, param, node);}
      bool     streamTryCheck                 (const uint16_t data, const int param=0)                     {int status; return VStreamGetCommon  (TRY_CHECK, null, &status, data, param, node);}
      bool     streamTryCheck                 (const uint32_t data, const int param=0)                     {int status; return VStreamGetCommon  (TRY_CHECK, null, &status, data, param, node);}
      bool     streamTryCheck                 (const uint64_t data, const int param=0)                     {int status; return VStreamGetCommon  (TRY_CHECK, null, &status, data, param, node);}

      void     streamCheck                    (const uint8_t  data, const int param=0)                     {VStreamCommon                        (CHECK, data, param, node);}
      void     streamCheck                    (const uint16_t data, const int param=0)                     {VStreamCommon                        (CHECK, data, param, node);}
      void     streamCheck                    (const uint32_t data, const int param=0)                     {VStreamCommon                        (CHECK, data, param, node);}
      void     streamCheck                    (const uint64_t data, const int param=0)                     {VStreamCommon                        (CHECK, data, param, node);}

      void     streamBurstSend                (uint8_t  *data,      const int bytesize, const int param=1) {VStreamBurstSendCommon               (SEND_BURST, BURST_NORM, data, bytesize, param, node);}
      void     streamBurstSend                (const int bytesize,  const int param=1)                     {VStreamBurstSendCommon               (SEND_BURST, BURST_TRANS, null, bytesize, param, node);}
      void     streamBurstSendAsync           (uint8_t  *data,      const int bytesize, const int param=1) {VStreamBurstSendCommon               (SEND_BURST_ASYNC, BURST_NORM, data, bytesize, param, node);}
      void     streamBurstSendAsync           (const int bytesize,  const int param=1)                     {VStreamBurstSendCommon               (SEND_BURST_ASYNC, BURST_TRANS, null, bytesize, param, node);}

      void     streamBurstGet                 (uint8_t  *data,      const int  bytesize)                   {int status; VStreamBurstGetCommon    (GET_BURST, BURST_NORM,  data, bytesize, &status, node);}
      void     streamBurstGet                 (uint8_t  *data,      const int  bytesize, int *status)      {VStreamBurstGetCommon                (GET_BURST, BURST_NORM,  data, bytesize, status, node);}
      void     streamBurstGet                 (const int bytesize)                                         {int status; VStreamBurstGetCommon    (GET_BURST, BURST_TRANS, null, bytesize, &status, node);}
      void     streamBurstGet                 (const int bytesize,        int *status)                     {VStreamBurstGetCommon                (GET_BURST, BURST_TRANS, null, bytesize, status, node);}

      void     streamBurstCheck               (uint8_t  *data,      const int bytesize, const int param=1) {VStreamBurstSendCommon               (CHECK_BURST, BURST_NORM,  data, bytesize, param, node);}
      void     streamBurstCheck               (const int bytesize,  const int param=1)                     {VStreamBurstSendCommon               (CHECK_BURST, BURST_TRANS, null, bytesize, param, node);}
      void     streamBurstCheckIncrement      (uint8_t   data,      const int bytesize, const int param=1) {VStreamBurstSendCommon               (CHECK_BURST, BURST_INCR_CHECK, &data, bytesize, param, node);}
      void     streamBurstCheckRandom         (uint8_t   data,      const int bytesize, const int param=1) {VStreamBurstSendCommon               (CHECK_BURST, BURST_RAND_CHECK, &data, bytesize, param, node);}

      void     streamBurstSendIncrement       (uint8_t   data,      const int bytesize, const int param=1) {VStreamBurstSendCommon               (SEND_BURST, BURST_INCR, &data, bytesize, param, node);}
      void     streamBurstSendIncrementAsync  (uint8_t   data,      const int bytesize, const int param=1) {VStreamBurstSendCommon               (SEND_BURST_ASYNC, BURST_INCR, &data, bytesize, param, node);}
      void     streamBurstSendRandom          (uint8_t   data,      const int bytesize, const int param=1) {VStreamBurstSendCommon               (SEND_BURST, BURST_RAND, &data, bytesize, param, node);}
      void     streamBurstSendRandomAsync     (uint8_t   data,      const int bytesize, const int param=1) {VStreamBurstSendCommon               (SEND_BURST_ASYNC, BURST_RAND, &data, bytesize, param, node);}

      void     streamBurstPopData             (uint8_t  *data,      const int bytesize)                    {int status; VStreamBurstGetCommon    (GET_BURST,   BURST_DATA,       data, bytesize, &status, node);}
      void     streamBurstPushData            (uint8_t  *data,      const int bytesize)                    {VStreamBurstSendCommon               (SEND_BURST,  BURST_DATA,       data, bytesize, 0, node);}
      void     streamBurstPushCheckData       (uint8_t  *data,      const int bytesize)                    {VStreamBurstSendCommon               (CHECK_BURST, BURST_DATA,       data, bytesize, 0, node);}
      void     streamBurstPushIncrement       (uint8_t   data,      const int bytesize)                    {VStreamBurstSendCommon               (SEND_BURST,  BURST_INCR_PUSH, &data, bytesize, 0, node);}
      void     streamBurstPushCheckIncrement  (uint8_t   data,      const int bytesize)                    {VStreamBurstSendCommon               (CHECK_BURST, BURST_INCR_PUSH, &data, bytesize, 0, node);}
      void     streamBurstPushRandom          (uint8_t   data,      const int bytesize)                    {VStreamBurstSendCommon               (SEND_BURST,  BURST_RAND_PUSH, &data, bytesize, 0, node);}
      void     streamBurstPushCheckRandom     (uint8_t   data,      const int bytesize)                    {VStreamBurstSendCommon               (CHECK_BURST, BURST_RAND_PUSH, &data, bytesize, 0, node);}

      bool     streamBurstTryGet              (const int bytesize,  const int param=1)                     {int status; return VStreamBurstGetCommon(TRY_GET_BURST,   BURST_TRANS,        null, bytesize, &status, node);}
      bool     streamBurstTryGet              (uint8_t  *data,      const int bytesize, const int param=1) {int status; return VStreamBurstGetCommon(TRY_GET_BURST,   BURST_NORM,       data, bytesize, &status, node);}
      bool     streamBurstTryCheck            (const int bytesize,  const int param=1)                     {return VStreamBurstSendCommon           (TRY_CHECK_BURST, BURST_TRANS,        null, bytesize, param,   node);}
      bool     streamBurstTryCheck            (uint8_t  *data,      const int bytesize, const int param=1) {return VStreamBurstSendCommon           (TRY_CHECK_BURST, BURST_NORM,       data, bytesize, param,   node);}
      bool     streamBurstTryCheckIncrement   (uint8_t   data,      const int bytesize, const int param=1) {return VStreamBurstSendCommon           (TRY_CHECK_BURST, BURST_INCR_CHECK, &data, bytesize, param,   node);}
      bool     streamBurstTryCheckRandom      (uint8_t   data,      const int bytesize, const int param=1) {return VStreamBurstSendCommon           (TRY_CHECK_BURST, BURST_RAND_CHECK, &data, bytesize, param,   node);}

      void     waitForSim                     (void)                                                       {VWaitForSim(node);}

      int      getNodeNumber                  (void)                                                       {return node;}

private:

      int      node;
};

#endif