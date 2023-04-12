// =========================================================================
//
//  File Name:         OsvvmCosimInt.h
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
//      side code, with interrupt support. Virtualises away top level
//      VP user thread routines.
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
//
// This is a base class for supporting interruptable user code when writing
// co-simulation tests vectors (i.e. not using a more that has interrupt
// modelling built in). The methods are the same as for OsvvmCosim with
// additional methods for interrupt handling. A set of virtual ISR methods
// should be overridden in the child class with the desired user code, which
// can make calls to the transaction methods themselves.
//
// The class allows up to 32 interuppts, with highest priority at isr0, and
// low priority ISRs can be interrupted by higher priority. A master
// interrrupt enable, along with individual enables are provided.
//
// Interrrupt request input is made via updateIntReq(IntReq) method. This
// would normally be called from the function that was registered for
// interrupt callback using regInterruptCB(intCbFunc) to pass in the
// interrupt request state.
//
// The interrupts granularity is at the transaction level, with interrupts
// being processed before each transaction generating method call.
//
// =========================================================================

#include <stdint.h>
#include "OsvvmCosim.h"

#ifndef __OSVVM_COSIMINT_H_
#define __OSVVM_COSIMINT_H_

class OsvvmCosimInt : public OsvvmCosim
{
public:
      static const int max_interrupts = 32;

               OsvvmCosimInt (int nodeIn = 0, std::string test_name = "") : OsvvmCosim(nodeIn, test_name)
               {
                   int_active  = 0;
                   int_enabled = 0;
                   int_master_enable = false;

                   for (int idx = 0; idx < max_interrupts; idx++)
                   {
                       isr[idx] = NULL;
                   }
               };

      // Override OsvvmCosim transaction methods to insert processINt methods before call to the parent class's methods
      uint8_t  transWrite             (const uint32_t addr, const uint8_t  data, const int prot = 0) {processInt(); return OsvvmCosim::transWrite(addr, data, prot);}
      uint16_t transWrite             (const uint32_t addr, const uint16_t data, const int prot = 0) {processInt(); return OsvvmCosim::transWrite(addr, data, prot);}
      uint32_t transWrite             (const uint32_t addr, const uint32_t data, const int prot = 0) {processInt(); return OsvvmCosim::transWrite(addr, data, prot);}
      uint8_t  transWrite             (const uint64_t addr, const uint8_t  data, const int prot = 0) {processInt(); return OsvvmCosim::transWrite(addr, data, prot);}
      uint16_t transWrite             (const uint64_t addr, const uint16_t data, const int prot = 0) {processInt(); return OsvvmCosim::transWrite(addr, data, prot);}
      uint32_t transWrite             (const uint64_t addr, const uint32_t data, const int prot = 0) {processInt(); return OsvvmCosim::transWrite(addr, data, prot);}
      uint64_t transWrite             (const uint64_t addr, const uint64_t data, const int prot = 0) {processInt(); return OsvvmCosim::transWrite(addr, data, prot);}
      uint8_t  transWriteAsync        (const uint32_t addr, const uint8_t  data, const int prot = 0) {processInt(); return OsvvmCosim::transWriteAsync(addr, data, prot);}
      uint16_t transWriteAsync        (const uint32_t addr, const uint16_t data, const int prot = 0) {processInt(); return OsvvmCosim::transWriteAsync(addr, data, prot);}
      uint32_t transWriteAsync        (const uint32_t addr, const uint32_t data, const int prot = 0) {processInt(); return OsvvmCosim::transWriteAsync(addr, data, prot);}
      uint8_t  transWriteAsync        (const uint64_t addr, const uint8_t  data, const int prot = 0) {processInt(); return OsvvmCosim::transWriteAsync(addr, data, prot);}
      uint16_t transWriteAsync        (const uint64_t addr, const uint16_t data, const int prot = 0) {processInt(); return OsvvmCosim::transWriteAsync(addr, data, prot);}
      uint32_t transWriteAsync        (const uint64_t addr, const uint32_t data, const int prot = 0) {processInt(); return OsvvmCosim::transWriteAsync(addr, data, prot);}
      uint64_t transWriteAsync        (const uint64_t addr, const uint64_t data, const int prot = 0) {processInt(); return OsvvmCosim::transWriteAsync(addr, data, prot);}

      void     transWriteAndRead      (const uint32_t addr, const uint8_t  wdata, uint8_t  *rdata, const int prot = 0) {processInt(); OsvvmCosim::transWriteAndRead(addr, wdata, rdata, prot);}
      void     transWriteAndRead      (const uint32_t addr, const uint16_t wdata, uint16_t *rdata, const int prot = 0) {processInt(); OsvvmCosim::transWriteAndRead(addr, wdata, rdata, prot);}
      void     transWriteAndRead      (const uint32_t addr, const uint32_t wdata, uint32_t *rdata, const int prot = 0) {processInt(); OsvvmCosim::transWriteAndRead(addr, wdata, rdata, prot);}
      void     transWriteAndRead      (const uint64_t addr, const uint8_t  wdata, uint8_t  *rdata, const int prot = 0) {processInt(); OsvvmCosim::transWriteAndRead(addr, wdata, rdata, prot);}
      void     transWriteAndRead      (const uint64_t addr, const uint16_t wdata, uint16_t *rdata, const int prot = 0) {processInt(); OsvvmCosim::transWriteAndRead(addr, wdata, rdata, prot);}
      void     transWriteAndRead      (const uint64_t addr, const uint32_t wdata, uint32_t *rdata, const int prot = 0) {processInt(); OsvvmCosim::transWriteAndRead(addr, wdata, rdata, prot);}
      void     transWriteAndRead      (const uint64_t addr, const uint64_t wdata, uint64_t *rdata, const int prot = 0) {processInt(); OsvvmCosim::transWriteAndRead(addr, wdata, rdata, prot);}
      void     transWriteAndReadAsync (const uint32_t addr, const uint8_t  wdata, uint8_t  *rdata, const int prot = 0) {processInt(); OsvvmCosim::transWriteAndReadAsync(addr, wdata, rdata, prot);}
      void     transWriteAndReadAsync (const uint32_t addr, const uint16_t wdata, uint16_t *rdata, const int prot = 0) {processInt(); OsvvmCosim::transWriteAndReadAsync(addr, wdata, rdata, prot);}
      void     transWriteAndReadAsync (const uint32_t addr, const uint32_t wdata, uint32_t *rdata, const int prot = 0) {processInt(); OsvvmCosim::transWriteAndReadAsync(addr, wdata, rdata, prot);}
      void     transWriteAndReadAsync (const uint64_t addr, const uint8_t  wdata, uint8_t  *rdata, const int prot = 0) {processInt(); OsvvmCosim::transWriteAndReadAsync(addr, wdata, rdata, prot);}
      void     transWriteAndReadAsync (const uint64_t addr, const uint16_t wdata, uint16_t *rdata, const int prot = 0) {processInt(); OsvvmCosim::transWriteAndReadAsync(addr, wdata, rdata, prot);}
      void     transWriteAndReadAsync (const uint64_t addr, const uint32_t wdata, uint32_t *rdata, const int prot = 0) {processInt(); OsvvmCosim::transWriteAndReadAsync(addr, wdata, rdata, prot);}
      void     transWriteAndReadAsync (const uint64_t addr, const uint64_t wdata, uint64_t *rdata, const int prot = 0) {processInt(); OsvvmCosim::transWriteAndReadAsync(addr, wdata, rdata, prot);}

      void     transWriteAddressAsync (const uint32_t addr)                                     {processInt(); OsvvmCosim::transWriteAddressAsync(addr);}
      void     transWriteAddressAsync (const uint64_t addr)                                     {processInt(); OsvvmCosim::transWriteAddressAsync(addr);}
      void     transWriteDataAsync    (const uint8_t  data, uint32_t bytelane = 0)              {processInt(); OsvvmCosim::transWriteDataAsync(data, bytelane);}
      void     transWriteDataAsync    (const uint16_t data, uint32_t bytelane = 0)              {processInt(); OsvvmCosim::transWriteDataAsync(data, bytelane);}
      void     transWriteDataAsync    (const uint32_t data, uint32_t bytelane = 0)              {processInt(); OsvvmCosim::transWriteDataAsync(data, bytelane);}
      void     transWriteDataAsync    (const uint64_t data, uint32_t bytelane = 0)              {processInt(); OsvvmCosim::transWriteDataAsync(data, bytelane);}

      void     transReadAddressAsync  (const uint32_t addr)                                     {processInt(); OsvvmCosim::transReadAddressAsync(addr);}
      void     transReadAddressAsync  (const uint64_t addr)                                     {processInt(); OsvvmCosim::transReadAddressAsync(addr);}
      void     transReadData          (uint8_t       *data, const int prot = 0)                 {processInt(); OsvvmCosim::transReadData(data, prot);}
      void     transReadData          (uint16_t      *data, const int prot = 0)                 {processInt(); OsvvmCosim::transReadData(data, prot);}
      void     transReadData          (uint32_t      *data, const int prot = 0)                 {processInt(); OsvvmCosim::transReadData(data, prot);}
      void     transReadData          (uint64_t      *data, const int prot = 0)                 {processInt(); OsvvmCosim::transReadData(data, prot);}

      void     transRead              (const uint32_t addr, uint8_t  *data, const int prot = 0) {processInt(); OsvvmCosim::transRead(addr, data, prot);}
      void     transRead              (const uint32_t addr, uint16_t *data, const int prot = 0) {processInt(); OsvvmCosim::transRead(addr, data, prot);}
      void     transRead              (const uint32_t addr, uint32_t *data, const int prot = 0) {processInt(); OsvvmCosim::transRead(addr, data, prot);}
      void     transRead              (const uint64_t addr, uint8_t  *data, const int prot = 0) {processInt(); OsvvmCosim::transRead(addr, data, prot);}
      void     transRead              (const uint64_t addr, uint16_t *data, const int prot = 0) {processInt(); OsvvmCosim::transRead(addr, data, prot);}
      void     transRead              (const uint64_t addr, uint32_t *data, const int prot = 0) {processInt(); OsvvmCosim::transRead(addr, data, prot);}
      void     transRead              (const uint64_t addr, uint64_t *data, const int prot = 0) {processInt(); OsvvmCosim::transRead(addr, data, prot);}

      void     transBurstWrite        (const uint32_t addr, uint8_t  *data, const int bytesize, const int prot = 0)  {processInt(); OsvvmCosim::transBurstWrite(addr, data, bytesize, prot);}
      void     transBurstWrite        (const uint64_t addr, uint8_t  *data, const int bytesize, const int prot = 0)  {processInt(); OsvvmCosim::transBurstWrite(addr, data, bytesize, prot);}
      void     transBurstWriteAsync   (const uint32_t addr, uint8_t  *data, const int bytesize, const int prot = 0)  {processInt(); OsvvmCosim::transBurstWriteAsync(addr, data, bytesize, prot);}
      void     transBurstWriteAsync   (const uint64_t addr, uint8_t  *data, const int bytesize, const int prot = 0)  {processInt(); OsvvmCosim::transBurstWriteAsync(addr, data, bytesize, prot);}
      void     transBurstRead         (const uint32_t addr, uint8_t  *data, const int bytesize, const int prot = 0)  {processInt(); OsvvmCosim::transBurstRead (addr, data, bytesize, prot);}
      void     transBurstRead         (const uint64_t addr, uint8_t  *data, const int bytesize, const int prot = 0)  {processInt(); OsvvmCosim::transBurstRead (addr, data, bytesize, prot);}

      void     tick                   (const int ticks, const bool done = false, const bool error = false) {processInt(); OsvvmCosim::tick(ticks, done, error);}

      // Enable/disable master interrupt
      void enableMasterInterrupt      (void) {int_master_enable = true;}
      void disableMasterInterrupt     (void) {int_master_enable = false;}

      // Enable/disable individual interrupts
      void enableIsr                  (const int int_num) {if (int_num < max_interrupts && isr[int_num] != NULL) {int_enabled |=  (1 << (int_num & (max_interrupts-1)));}}
      void disableIsr                 (const int int_num) {int_enabled &= ~(1 << (int_num & (max_interrupts-1)));}

      // Interrupt input. Call from external registered callback function
      int  updateIntReq               (const uint32_t intReq) {int_req = intReq; return 0;}

      void registerIsr                (const pVUserInt_t isrFunc, const unsigned level) {if (level < max_interrupts) isr[level] = isrFunc;}

private:
      // Process any outstanding interrupts. Will process in priority
      // order, with 0 being the highest. The ISRs can be interrupted
      // by higher priority interrupts.
      void processInt()
      {
          // Only process if the master interuppt enable is true
          if (int_master_enable)
          {
              // Set bits for any pending interrupts that are enabled and not active
              // and that has the interrupt request input
              uint32_t int_new_int = int_enabled & ~int_active & int_req;

              // Priority inspect the interrupts for servicing  (0 = highest)
              for (int isr_idx = 0; isr_idx < max_interrupts; isr_idx++)
              {
                  bool higher_active = int_active & (0xffffffffULL >> (max_interrupts-isr_idx));

                  // Map the index to a unary value
                  uint32_t int_unary = 1 << isr_idx;

                  // If IRQ low for indexed bit when active clear active state
                  if(int_active & ~int_req & int_unary)
                  {
                      // Clear the indexed ISR active bit
                      int_active &= ~int_unary;
                  }

                  // If a new interrupt at index and no active higher priority interrupt, process it
                  if ((int_new_int & int_unary) && !higher_active)
                  {
                      // Set the active bit for the interrupt
                      int_active |= int_unary;

                      // Select the ISR and call it.
                      if (isr[isr_idx] != NULL)
                      {
                          (*(isr[isr_idx]))(int_req);
                      }
                  }
              }
          }
      }


      // Function pointers for ISRs
      pVUserInt_t isr[max_interrupts];

      // Interrupt status vector
      uint32_t   int_active;

      // Interrupt enable vector
      uint32_t   int_enabled;

      // Interrupt master enable
      bool       int_master_enable;

      // Interrupts request input state
      uint32_t   int_req;
};

#endif