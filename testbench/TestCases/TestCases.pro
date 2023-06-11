#  File Name:         TestCases.pro
#  Revision:          STANDARD VERSION
#
#  Maintainer:        Simon Southwell email:  simon.southwell@gmail.com
#  Contributor(s):
#     Simon Southwell simon.southwell@gmail.com
#     Jim Lewis       jim@synthworks.com
#
#
#  Description:
#        Script to run Co-simulation Interrupt tests
#
#  Revision History:
#    Date      Version    Description
#    12/2022   2023.01    Updated after refactoring and moving of testbenches to CoSim directory
#    11/2022              Initial
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

# library and TestSuite set by higher level scripts

# Uses OSVVM Interrupt Handling
# analyze TbAb_InterruptCosim1.vhd
# MkVproc $::osvvm::OsvvmCoSimDirectory/tests/interrupt
# simulate TbAb_InterruptCosim1

# Use Interrupt Handling in Vproc
analyze TbAb_InterruptCoSim2.vhd
MkVproc $::osvvm::OsvvmCoSimDirectory/tests/interruptCB
simulate TbAb_InterruptCoSim2 [CoSim]

# Use Interrupt Handling in Vproc
analyze TbAb_InterruptCoSim3.vhd
MkVproc $::osvvm::OsvvmCoSimDirectory/tests/interruptIss rv32
simulate TbAb_InterruptCoSim3 [CoSim]

# Use Interrupt Handling in Vproc
analyze TbAb_InterruptCoSim5.vhd
MkVproc $::osvvm::OsvvmCoSimDirectory/tests/interruptClass
simulate TbAb_InterruptCoSim5 [CoSim]

# Use Interrupt Handling in Vproc
analyze TbAb_InterruptCoSim2.vhd
MkVproc $::osvvm::OsvvmCoSimDirectory/tests/interruptCB
simulate TbAb_InterruptCoSim2 [generic INT_POLARITY '0'] [CoSim]

# Use Interrupt Handling in Vproc
analyze TbAb_InterruptCoSim4.vhd
MkVproc $::osvvm::OsvvmCoSimDirectory/tests/interruptCB
simulate TbAb_InterruptCoSim4 [generic INT_EDGE_LEVEL '1'] [CoSim]


