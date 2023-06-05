// =========================================================================
//
//  File Name:         OsvvmCosimStreamTx.h
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
//      side TX only stream code. Virtualises away top level VP user thread routines.
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

#ifndef __OSVVM_COSIM_STREAM_TX_H_
#define __OSVVM_COSIM_STREAM_TX_H_

class OsvvmCosimStreamTx : public OsvvmCosimStream
{
public:
                OsvvmCosimStreamTx (int nodeIn = 0, std::string test_name = "") : node(nodeIn) {
                   if (test_name.compare(""))
                   {
                       VSetTestName(test_name.c_str(), test_name.length(), node);
                   }
                };

private:
      // Make the derived RX methods private in this class
      using OsvvmCosimStream::streamGet;
      using OsvvmCosimStream::streamTryGet;
      using OsvvmCosimStream::streamTryCheck;
      using OsvvmCosimStream::streamCheck;
      using OsvvmCosimStream::streamBurstGet;
      using OsvvmCosimStream::streamBurstCheck;
      using OsvvmCosimStream::streamBurstCheckIncrement;
      using OsvvmCosimStream::streamBurstCheckRandom;
      using OsvvmCosimStream::streamBurstPopData;
      using OsvvmCosimStream::streamBurstPushCheckData;
      using OsvvmCosimStream::streamBurstPushCheckIncrement;
      using OsvvmCosimStream::streamBurstPushCheckRandom;
      using OsvvmCosimStream::streamBurstTryGet;
      using OsvvmCosimStream::streamBurstTryCheck;
      using OsvvmCosimStream::streamBurstTryCheckIncrement;
      using OsvvmCosimStream::streamBurstTryCheckRandom;
      using OsvvmCosimStream::streamGetRxTransactionCount;
      using OsvvmCosimStream::streamWaitForRxTransaction;

      int node;
};

#endif