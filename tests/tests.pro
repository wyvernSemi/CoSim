#  File Name:               tests.pro
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
#  Copyright (c) 2022 by [OSVVM Authors](../AUTHORS.md)
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

MkVproc    $::osvvm::OsvvmCoSimDirectory  tests/usercode_size
simulate   TbAb_CoSim  [CoSim]

MkVproc    $::osvvm::OsvvmCoSimDirectory  tests/usercode_burst
simulate   TbAb_CoSim  [CoSim]

MkVproc    $::osvvm::OsvvmCoSimDirectory  tests/iss rv32
simulate   TbAb_CoSim  [CoSim]

MkVprocSkt $::osvvm::OsvvmCoSimDirectory  tests/socket
simulate   TbAb_CoSim  [CoSim]

#if {$::osvvm::ToolName eq "GHDL"} {
#
#  MkVprocGhdlMain  $::osvvm::CurrentWorkingDirectory/../../../CoSim tests/ghdl_main
#
#  set ::osvvm::GhdlRunCmd "-r"
#  simulate        TbAb_CoSim  [CoSim]
#  unset ::osvvm::GhdlRunCmd
#}