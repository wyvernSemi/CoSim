#  File Name:         TbAxi4.pro
#  Revision:          STANDARD VERSION
#
#  Maintainer:        Jim Lewis      email:  jim@synthworks.com
#  Contributor(s):
#     Jim Lewis      jim@synthworks.com
#
#
#  Description:
#        Script to analyze Axi4 testbench  
#
#  Developed for:
#        SynthWorks Design Inc.
#        VHDL Training Classes
#        11898 SW 128th Ave.  Tigard, Or  97223
#        http://www.SynthWorks.com
#
#  Revision History:
#    Date      Version    Description
#    05/2023   2023.05    Compile Script for OSVVM
#
#
#  This file is part of OSVVM.
#  
#  Copyright (c) 2023 by SynthWorks Design Inc.  
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
TestSuite  CoSim_TbDpRam
library    osvvm_CoSim_TbDpRam
analyze    TestCtrl_e.vhd

analyze    TbDpRam.vhd 
analyze    ../TestCases_DpRam/TbDpRam_WriteAndRead.vhd

