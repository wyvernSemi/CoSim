#  File Name:               cosim.pro
#  Revision:                OSVVM MODELS STANDARD VERSION
#
#  Maintainer:              Simon Southwell      simon.southwell@gmail.com
#  Contributor(s):
#     Simon Southwell       simon.southwell@gmail.com
#
#
#  Description:
#        Script to run Axi4 Lite CoSim tests
#
#  Revision History:
#    Date      Version    Description
#    12/2022   2023.01    Refactored to source scripts in Scripts/StartUpShared.tcl and 
#                         analyze CoSim by calling CoSim/CoSim.pro in OsvvmLibraries/OsvvmLibraries.pro
#     9/2022   --         Initial version
#
#
#  This file is part of OSVVM.
#
#  Copyright (c) 2022 by [OSVVM Authors](../../AUTHORS.md)
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

MkVproc    $::osvvm::OsvvmCoSimDirectory/tests/usercode_size
TestName   CoSim_usercode_size
simulate   TbAb_CoSim [CoSim]

# MkVproc    $::osvvm::OsvvmCoSimDirectory/tests/usercode_burst
# TestName   CoSim_usercode_burst
# simulate   TbAb_CoSim [CoSim]

 
MkVproc    $::osvvm::OsvvmCoSimDirectory/tests/iss rv32
TestName   CoSim_iss
simulate   TbAb_CoSim [CoSim]

# MkVprocSkt $::osvvm::OsvvmCoSimDirectory/tests/socket
# simulate   TbAb_CoSim  [CoSim]
# 
# EndSimulation
# after 1000

# if {$::osvvm::ToolName eq "GHDL"} {
#
#   MkVprocGhdlMain  $::osvvm::OsvvmCoSimDirectory/tests/ghdl_main
#
#   set ::osvvm::GhdlRunCmd "-r"
#   simulate  TbAb_CoSim  [CoSim]
#   unset ::osvvm::GhdlRunCmd
#}