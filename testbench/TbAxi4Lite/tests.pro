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
#  Developed for:
#        SynthWorks Design Inc.
#        VHDL Training Classes
#        11898 SW 128th Ave.  Tigard, Or  97223
#        http://www.SynthWorks.com
#
#  Revision History:
#    Date      Version    Description
#     9/2022   2023.01    Initial version
#
#
#  This file is part of OSVVM.
#
#  Copyright (c) 2022 by SynthWorks Design Inc.
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

MkVproc    ${::osvvm::SCRIPT_DIR}/../CoSim tests/usercode_size
TestName   CoSim_usercode_size
simulate   TbAxi4_CoSim [generic TEST_NAME usercode_size]

# MkVproc    ${::osvvm::SCRIPT_DIR}/../CoSim tests/usercode_burst
# TestName   CoSim_usercode_burst
# simulate   TbAxi4_CoSim [generic TEST_NAME usercode_burst]

 
MkVproc    ${::osvvm::SCRIPT_DIR}/../CoSim tests/iss rv32
TestName   CoSim_iss
simulate   TbAxi4_CoSim [generic TEST_NAME iss]

# MkVprocSkt ${::osvvm::SCRIPT_DIR}/../CoSim tests/socket
# simulate   TbAxi4_CoSim

# if {$::osvvm::ToolName eq "GHDL"} {
#
#  MkVprocGhdlMain  $::osvvm::CurrentWorkingDirectory/../../../CoSim tests/ghdl_main
#
#  set ::osvvm::GhdlRunCmd "-r"
#  simulate        TbAxi4_CoSim
#  unset ::osvvm::GhdlRunCmd
#}