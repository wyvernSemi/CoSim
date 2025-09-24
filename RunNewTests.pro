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
#  Copyright (c) 2022-2023 by [OSVVM Authors](AUTHORS.md)  
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


# Analyze Axi4 testbench and run tests on it
include  ./testbench/TbAxi4
include  ./testbench/TbAxi4/isstest.pro