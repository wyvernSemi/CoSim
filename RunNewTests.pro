#  File Name:         RunAllTests.pro
#  Revision:          OSVVM MODELS STANDARD VERSION
#
#  Maintainer:        Simon Southwell      email:  simon.southwell@gmail.com
#  Contributor(s):
#     Simon Southwell      simon.southwell@gmail.com
#
#
#  Description:
#        Script to run Axi4 Lite co-sim API test  
#
#  Revision History:
#    Date      Version   Description
#    01/2023   2023.01   Relocated testbenches to be under CoSim
#    10/2022             Compile Script for OSVVM co-sim
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

if {$::osvvm::ToolName eq "NVC"} {
  set osname [string tolower [exec uname]]
  if {"$osname" ne "linux"} {
    set ::env(NVC_FOREIGN_OBJ) VProc.so
  } else {
    SetExtendedRunOptions --load=./VProc.so
  }
}

# # Analyze Axi4Lite testbench and run tests on it
# include  ./testbench/TbAxi4Lite
# # include  ./tests
# include  ./testbench/TbAxi4Lite/tests.pro
# 
# # Analyze Axi4 testbench and run tests on it
include  ./testbench/TbAxi4
# # include  ./tests
# include  ./testbench/TbAxi4/tests.pro
include  ./testbench/TbAxi4/RunDemoTests.pro
# include  ./testbench/TestCases
# include  ./testbench/TestCases/RunDemoTests.pro
# 
# # Analyze Axi4 testbench and run tests on it
# include ./testbench/TbAxi4_Interrupt
# include ./testbench/TestCases_Interrupt
# include ./testbench/TestCases_Interrupt/RunDemoTests.pro
