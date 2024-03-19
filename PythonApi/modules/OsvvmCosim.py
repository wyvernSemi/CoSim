# =========================================================================
#
#  File Name:         OsvvmCosim.py
#  Design Unit Name:
#  Revision:          OSVVM MODELS STANDARD VERSION
#
#  Maintainer:        Simon Southwell email:  simon.southwell@gmail.com
#  Contributor(s):
#     Simon Southwell      simon.southwell@gmail.com
#
#
#  Description:
#      Python OSVVM co-simulation API class
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

# Load ctypes library to get access to C domain
from ctypes import *
from sys    import getsizeof

class OsvvmCosim :

# ---------------------------------------------------------------
# Function to load Python C API module
# ---------------------------------------------------------------

  node = -1
  api  = None

  # Constructor
  def __init__(self, nodeIn, testname = "", cmodulename = "./VUser.so") :
    self.node = nodeIn
    self.api  = self.__loadPyModule(cmodulename)
    if testname != "" :
      self.setTestName(testname.encode('utf-8'))

  # Method to load Python C module
  def __loadPyModule(self, name = "./VUser.so") :

    module = CDLL(name)
    module.argtypes = [c_int32]
    module.restype  = [c_int32]

    return module

  # API method to write a word
  def transWrite (self, addr, data, delta = 0) :
    self.api.PyTransWrite(addr, data, delta, self.node)

  # API method to read a word as signed value
  def transSRead (self, addr,  delta = 0) :
    return self.api.PyTransRead(addr, delta, self.node)

  # API method to read a word as unsigned value
  def transRead (self, addr,  delta = 0) :
    return c_uint32(self.transSRead(addr, delta)).value

  # API method to tick for specified number of clocks
  def tick (self, ticks, done = False, error = False) :
    self.api.PyTick(ticks, done, error, self.node)

  # API method to do a burst write
  def transBurstWrite(self, addr, data, length) :
    self.api.PyTransBurstWrite(addr, (c_int32 * len(data))(*data), length, self.node)

  # API method to do a burst read
  def transBurstRead(self, addr, length) :
    data = []
    wlen = int(length/4)
    cdata = (c_int32 * wlen)(0)
    self.api.PyTransBurstRead(addr, cdata, length, self.node)
    for i in range(wlen) :
      data.append(c_uint32(cdata[i]).value)
    return data

  # API method to register a vectored interrupt callback
  def regInterruptCB(self, irqCb) :
    cb_ftype = CFUNCTYPE(c_int32, c_int32)
    cb_wrap  = cb_ftype(irqCb)
    self.api.PyRegIrq(cb_wrap, self.node)
    
  def setTestName(self, testname) :
    self.api.PySetTestName(testname, len(testname), self.node)

  def VPrint(self, printstr) :
    bytestring = printstr.encode('utf-8')
    self.api.PyPrint(c_char_p(bytestring))


