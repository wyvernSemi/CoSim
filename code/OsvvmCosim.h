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
//    11/2022   2023.01    Initial revision
//
//
//  This file is part of OSVVM.
//
//  Copyright (c) 2022 by [OSVVM Authors](../AUTHORS.md)
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

      uint8_t  transWrite      (const uint32_t addr, const uint8_t  data, const int prot = 0) {return VTransWrite(addr, data, prot, node);}
      uint16_t transWrite      (const uint32_t addr, const uint16_t data, const int prot = 0) {return VTransWrite(addr, data, prot, node);}
      uint32_t transWrite      (const uint32_t addr, const uint32_t data, const int prot = 0) {return VTransWrite(addr, data, prot, node);}
      uint8_t  transWrite      (const uint64_t addr, const uint8_t  data, const int prot = 0) {return VTransWrite(addr, data, prot, node);}
      uint16_t transWrite      (const uint64_t addr, const uint16_t data, const int prot = 0) {return VTransWrite(addr, data, prot, node);}
      uint32_t transWrite      (const uint64_t addr, const uint32_t data, const int prot = 0) {return VTransWrite(addr, data, prot, node);}
      uint64_t transWrite      (const uint64_t addr, const uint64_t data, const int prot = 0) {return VTransWrite(addr, data, prot, node);}
                               
      void     transRead       (const uint32_t addr, uint8_t  *data,      const int prot = 0) {VTransRead(addr, data, prot, node);}
      void     transRead       (const uint32_t addr, uint16_t *data,      const int prot = 0) {VTransRead(addr, data, prot, node);}
      void     transRead       (const uint32_t addr, uint32_t *data,      const int prot = 0) {VTransRead(addr, data, prot, node);}
      void     transRead       (const uint64_t addr, uint8_t  *data,      const int prot = 0) {VTransRead(addr, data, prot, node);}
      void     transRead       (const uint64_t addr, uint16_t *data,      const int prot = 0) {VTransRead(addr, data, prot, node);}
      void     transRead       (const uint64_t addr, uint32_t *data,      const int prot = 0) {VTransRead(addr, data, prot, node);}
      void     transRead       (const uint64_t addr, uint64_t *data,      const int prot = 0) {VTransRead(addr, data, prot, node);}

      void     transBurstWrite (const uint32_t addr, uint8_t  *data, const int bytesize, const int prot = 0)  {VTransBurstWrite(addr, data, bytesize, prot, node);}
      void     transBurstWrite (const uint64_t addr, uint8_t  *data, const int bytesize, const int prot = 0)  {VTransBurstWrite(addr, data, bytesize, prot, node);}
      void     transBurstRead  (const uint32_t addr, uint8_t  *data, const int bytesize, const int prot = 0)  {VTransBurstRead (addr, data, bytesize, prot, node);}
      void     transBurstRead  (const uint64_t addr, uint8_t  *data, const int bytesize, const int prot = 0)  {VTransBurstRead (addr, data, bytesize, prot, node);}

      void     regInterruptCB  (pVUserInt_t func)                                             {VRegInterrupt(func, node);}
      
      void     waitForSim      (void)                                                         {VWaitForSim(node);}

      int      getNodeNumber   () {return node;}

private:

      int      node;
};

#endif