#  File Name:               cosim.pro
#  Revision:                OSVVM MODELS STANDARD VERSION
#
#  Maintainer:              Simon Southwell      simon.southwell@gmail.com
#  Contributor(s):
#     Simon Southwell       simon.southwell@gmail.com
#     Jim Lewis             jim@synthworks.com
#
#
#  Description:
#        Script to run Axi4 Lite CoSim tests
#
#  Revision History:
#    Date      Version    Description
#     9/2022   2023.01    Initial version
#
#
#  This file is part of OSVVM.
#
#  Copyright (c) 2022 by [OSVVM Authors](AUTHORS.md)
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

library    osvvm_cosim

if {$::osvvm::ToolName eq "ActiveHDL"} {
  analyze OsvvmVprocAldecPkg.vhd
} elseif {$::osvvm::ToolName eq "NVC"} {
  analyze OsvvmVprocNvcPkg.vhd
  # exec uname does not work in Windows - it does work in MSYS2
  set osname [string tolower [exec uname]]
  if {"$osname" ne "linux"} {
    set ::env(NVC_FOREIGN_OBJ) VProc.so
  } else {
    SetExtendedRunOptions --load=./VProc.so
  }
} elseif {$::osvvm::ToolName eq "GHDL"} {
  analyze OsvvmVprocGhdlPkg.vhd
} else {
  analyze OsvvmVprocPkg.vhd
}

analyze OsvvmTestCoSimPkg.vhd
analyze CoSimInterruptHandler.vhd
analyze CoSimInterruptHandlerComponentPkg.vhd

analyze CoSimContext.vhd
