#  File Name:         TestCases_Ethernet.pro
#  Revision:          STANDARD VERSION
#
#  Maintainer:        Simon Soutghwell email:  simon.southwell@gmail.com
#  Contributor(s):
#     Simon Southwell simon.southwell@gmail.com
#
#
#  Description:
#        Script to run one Ethernet co-simulation stream test  
#
#  Developed for:
#        SynthWorks Design Inc.
#        VHDL Training Classes
#        11898 SW 128th Ave.  Tigard, Or  97223
#        http://www.SynthWorks.com
#
#  Revision History:
#    Date      Version    Description
#     3/2023   2023.04    Initial release
#
#
#  This file is part of OSVVM.
#  
#  Copyright (c) 2023 by [OSVVM Authors](../../AUTHORS.md)  
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

analyze Tb_xMii1.vhd

ChangeWorkingDirectory ../../tests
MkVproc  stream_ethernet

TestName   CoSim_ethernet_streams
simulate Tb_xMii1 [generic MII_INTERFACE GMII] [generic MII_BPS BPS_1G]    [CoSim]

TestName   CoSim_ethernet_streams
simulate Tb_xMii1 [generic MII_INTERFACE RGMII] [generic MII_BPS BPS_1G]   [CoSim]

TestName   CoSim_ethernet_streams
simulate Tb_xMii1 [generic MII_INTERFACE MII]   [generic MII_BPS BPS_100M] [CoSim]

TestName   CoSim_ethernet_streams
simulate Tb_xMii1 [generic MII_INTERFACE MII]   [generic MII_BPS BPS_10M]  [CoSim]

TestName   CoSim_ethernet_streams
simulate Tb_xMii1 [generic MII_INTERFACE RMII]  [generic MII_BPS BPS_100M] [CoSim]

TestName   CoSim_ethernet_streams
simulate Tb_xMii1 [generic MII_INTERFACE RMII]  [generic MII_BPS BPS_10M]  [CoSim]
