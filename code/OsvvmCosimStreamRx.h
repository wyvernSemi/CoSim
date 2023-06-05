// =========================================================================
//
//  File Name:         OsvvmCosimStreamRx.h
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
//      side RX only stream code. Virtualises away top level VP user thread routines.
//
//  Revision History:
//    Date      Version    Description
//    06/2023   2023.05    Initial revision
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

#include "OsvvmCosimStream.h"

#ifndef __OSVVM_COSIM_STREAM_RX_H_
#define __OSVVM_COSIM_STREAM_RX_H_

class OsvvmCosimStreamRx : public OsvvmCosimStream
{
public:
                OsvvmCosimStreamRx (int nodeIn = 0, std::string test_name = "") : node(nodeIn) {
                   if (test_name.compare(""))
                   {
                       VSetTestName(test_name.c_str(), test_name.length(), node);
                   }
                };

private:
      // Make the derived TX methods private in this class
      using OsvvmCosimStream::streamSend;
      using OsvvmCosimStream::streamSendAsync;
      using OsvvmCosimStream::streamBurstSend;
      using OsvvmCosimStream::streamBurstSendAsync;
      using OsvvmCosimStream::streamBurstSendIncrement;
      using OsvvmCosimStream::streamBurstSendIncrementAsync;
      using OsvvmCosimStream::streamBurstSendRandom;
      using OsvvmCosimStream::streamBurstSendRandomAsync;
      using OsvvmCosimStream::streamBurstPushData;
      using OsvvmCosimStream::streamBurstPushIncrement;
      using OsvvmCosimStream::streamBurstPushRandom;
      using OsvvmCosimStream::streamGetTxTransactionCount;
      using OsvvmCosimStream::streamWaitForTxTransaction;
      
      int node;
};

#endif