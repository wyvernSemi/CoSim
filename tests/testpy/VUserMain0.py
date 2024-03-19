# =========================================================================
#
#  File Name:         VUserMain0.py
#  Design Unit Name:
#  Revision:          OSVVM MODELS STANDARD VERSION
#
#  Maintainer:        Simon Southwell email:  simon.southwell@gmail.com
#  Contributor(s):
#     Simon Southwell      simon.southwell@gmail.com
#
#
#  Description:
#      OSVVM python test
#
#  Revision History:
#    Date      Version    Description
#    03/2024   2024.??    Initial revision
#
#
#  This file is part of OSVVM.
#
#  Copyright (c) 2024 by [OSVVM Authors](../../AUTHORS.md)
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      https://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
# =========================================================================

from ctypes import *
import OsvvmCosim as cosim

# Define some local constants
__LONGTIME    = 0x7fffffff

# Define global variable to flag when reset deassertion interrupt
# event has occurred
seenreset = 0

# Define a global vproc API object handle so that the IRQ callback 
# can use it.
osvvm     = None

# ---------------------------------------------------------------
# Interrupt callback function
# ---------------------------------------------------------------

def irqCb (irq) :

    # Get access to global variable
    global seenreset, osvvm

    osvvm.VPrint("Interrupt = " + hex(irq))

    # If irq[0] is 1, flag that seen a reset deassertion
    if irq & 0x1 :
      osvvm.VPrint("  Seen reset deasserted!\n")
      seenreset = 1

    return 0

# ---------------------------------------------------------------
# Function to wait for reset deassertion event
# ---------------------------------------------------------------

def waitForResetDeassert (osvvm, polltime = 10) :

  while seenreset == 0 :
    osvvm.tick(polltime)

# ---------------------------------------------------------------
# Main entry point for node 0
# ---------------------------------------------------------------

def VUserMain0() :

  # Get access to global variable
  global seenreset, osvvm

  # This is node 0
  node  = 0
  
  # Error status
  error = False
  
  # Create an API object for node 0 and set the testname
  osvvm = cosim.OsvvmCosim(node, "CoSim_python")
  
  # Register IRQ callback
  osvvm.regInterruptCB(irqCb)

  # Wait until reset is deasserted
  #waitForResetDeassert(osvvm)

  # Do some transfers with delays...
  osvvm.tick(11)
  
  # Construct some test data (addresses are byte addresses)
  addr  = [0xa0001000, 0xa0001004]
  wdata = [0x12345678, 0x87654321]

  # Write test
  osvvm.VPrint("Writing " + hex(wdata[0]) + " to   addr " + hex(addr[0]))
  osvvm.transWrite(addr[0], wdata[0])

  osvvm.tick(1)

  osvvm.VPrint("Writing " + hex(wdata[1]) + " to   addr " + hex(addr[1]))
  osvvm.transWrite(addr[1], wdata[1])

  osvvm.tick(5)

  # Read Test
  rdata = osvvm.transRead(addr[0])  # Unsigned value
  
  if rdata == wdata[0] :
    osvvm.VPrint("Read    " + hex(rdata) + " from addr " + hex(addr[0]))
  else :
    osvvm.VPrint("***ERROR: Read    " + hex(rdata) + " from addr " + hex(addr[0]) + ", expected " + hex(wdata[0]))
    error = True
  
  osvvm.tick(3)

  rdata = osvvm.transRead(addr[1])  # Unsigned value
  
  if rdata == wdata[1] :
    osvvm.VPrint("Read    " + hex(rdata) + " from addr " + hex(addr[1]))
  else :
    osvvm.VPrint("***ERROR: Read    " + hex(rdata) + " from addr " + hex(addr[1]) + ", expected " + hex(wdata[1]))
    error = True
    
  osvvm.tick(1)
  
  # Burst write test
  burstWrData = [0xcafebabe, 0xdeadbeef, 0x900ddeed, 0x0badf00d]
  burstAddr   = 0xa0002000
  osvvm.VPrint("Writing burst " + str(list(map(hex, burstWrData))) + " to   addr " + hex(burstAddr))
  osvvm.transBurstWrite(burstAddr, burstWrData, len(burstWrData)*4)
  
  # Burst read test
  burstRdData = osvvm.transBurstRead(burstAddr, 16);
  osvvm.VPrint("Read burst    " + str(list(map(hex, burstRdData))) + " from addr " + hex(burstAddr));
  
  if burstRdData != burstWrData :
    osvvm.VPrint("***ERROR: mismatch in burst read data");
    error = True

  osvvm.VPrint("\nTests complete, stopping simulation\n")

  # Tell simulator to stop/finish
  osvvm.tick(20, True, error);

  # Should not get here
  while True :
    osvvm.tick(__LONGTIME)
