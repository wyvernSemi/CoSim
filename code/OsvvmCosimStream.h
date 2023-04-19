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

      uint8_t  streamSend               (const uint8_t  data, const int param=0)                {return VStreamSend(data, param, node);}
      uint16_t streamSend               (const uint16_t data, const int param=0)                {return VStreamSend(data, param, node);}
      uint32_t streamSend               (const uint32_t data, const int param=0)                {return VStreamSend(data, param, node);}
      uint64_t streamSend               (const uint64_t data, const int param=0)                {return VStreamSend(data, param, node);}
                                        
      uint8_t  streamSendAsync          (const uint8_t  data, const int param=0)                {return VStreamSendAsync(data, param, node);}
      uint16_t streamSendAsync          (const uint16_t data, const int param=0)                {return VStreamSendAsync(data, param, node);}
      uint32_t streamSendAsync          (const uint32_t data, const int param=0)                {return VStreamSendAsync(data, param, node);}
      uint64_t streamSendAsync          (const uint64_t data, const int param=0)                {return VStreamSendAsync(data, param, node);}
                                        
      void     streamGet                (uint8_t  *data)                                        {int status; VStreamGet(data, &status, node);}
      void     streamGet                (uint16_t *data)                                        {int status; VStreamGet(data, &status, node);}
      void     streamGet                (uint32_t *data)                                        {int status; VStreamGet(data, &status, node);}
      void     streamGet                (uint64_t *data)                                        {int status; VStreamGet(data, &status, node);}
                                        
      void     streamGet                (uint8_t  *data, int *status)                           {VStreamGet(data, status, node);}
      void     streamGet                (uint16_t *data, int *status)                           {VStreamGet(data, status, node);}
      void     streamGet                (uint32_t *data, int *status)                           {VStreamGet(data, status, node);}
      void     streamGet                (uint64_t *data, int *status)                           {VStreamGet(data, status, node);}
                                        
      void     streamCheck              (const uint8_t  data, const int param=0)                {VStreamCheck(data, param, node);}
      void     streamCheck              (const uint16_t data, const int param=0)                {VStreamCheck(data, param, node);}
      void     streamCheck              (const uint32_t data, const int param=0)                {VStreamCheck(data, param, node);}
      void     streamCheck              (const uint64_t data, const int param=0)                {VStreamCheck(data, param, node);}
                                        
      void     streamBurstSend          (uint8_t  *data, const int bytesize, const int param=0) {VStreamBurstSend      (data, bytesize, param, node);}
      void     streamBurstSendAsync     (uint8_t  *data, const int bytesize, const int param=0) {VStreamBurstSendAsync (data, bytesize, param, node);}
      void     streamBurstGet           (uint8_t  *data, const int bytesize)                    {int status; VStreamBurstGet (data, bytesize, &status, node);}
      void     streamBurstGet           (uint8_t  *data, const int bytesize,       int *status) {VStreamBurstGet       (data, bytesize, status, node);}
      void     streamBurstGet           (const int bytesize)                                    {int status; VStreamBurstGet (bytesize, &status, node);}
      void     streamBurstGet           (const int bytesize, int *status)                       {VStreamBurstGet       (bytesize, status, node);}
      void     streamBurstCheck         (uint8_t  *data, const int bytesize, const int param=0) {VStreamBurstCheck     (data, bytesize, param, node);}
      void     streamBurstSendIncrement (uint8_t   data, const int bytesize, const int param=0) {VStreamBurstSendIncrement(&data, bytesize, param, node);}
      void     streamBurstSendRandom    (uint8_t   data, const int bytesize, const int param=0) {VStreamBurstSendRandom(&data, bytesize, param, node);}
      void     streamBurstPopData       (uint8_t  *data, const int bytesize)                    {VStreamBurstPopData   (data, bytesize, node);}

      void     waitForSim              (void)                                                  {VWaitForSim(node);}

      int      getNodeNumber           (void)                                                  {return node;}

private:

      int      node;
};

#endif