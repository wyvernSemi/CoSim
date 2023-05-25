--
--  File Name:         TestCtrl_e.vhd
--  Design Unit Name:  TestCtrl
--  Revision:          OSVVM MODELS STANDARD VERSION
--
--  Maintainer:        Jim Lewis      email:  jim@synthworks.com
--  Contributor(s):
--     Jim Lewis      jim@synthworks.com
--
--
--  Description:
--      Test transaction source
--
--
--  Developed by:
--        SynthWorks Design Inc.
--        VHDL Training Classes
--        http://www.SynthWorks.com
--
--  Revision History:
--    Date      Version    Description
--    05/2023   2017.09    Initial revision
--
--
--  This file is part of OSVVM.
--  
--  Copyright (c) 2023 by SynthWorks Design Inc.  
--  
--  Licensed under the Apache License, Version 2.0 (the "License");
--  you may not use this file except in compliance with the License.
--  You may obtain a copy of the License at
--  
--      https://www.apache.org/licenses/LICENSE-2.0
--  
--  Unless required by applicable law or agreed to in writing, software
--  distributed under the License is distributed on an "AS IS" BASIS,
--  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
--  See the License for the specific language governing permissions and
--  limitations under the License.
--  

library ieee ;
  use ieee.std_logic_1164.all ;
  use ieee.numeric_std.all ;
  use ieee.numeric_std_unsigned.all ;
  use ieee.math_real.all ;
  
library OSVVM ; 
  context OSVVM.OsvvmContext ; 
  use osvvm.ScoreboardPkg_slv.all ;

library OSVVM_DPRAM ;
  context OSVVM_DpRam.DPRamContext ; 

library osvvm_cosim ;
  context osvvm_cosim.CoSimContext ;

-- use work.OsvvmTestCommonPkg.all ;

entity TestCtrl is
  port (
    -- Global Signal Interface
    nReset         : In    std_logic ;

    -- Transaction Interfaces
    Manager1Rec      : inout AddressBusRecType ;
    Manager2Rec      : inout AddressBusRecType 
  ) ;
  
  constant ADDR_WIDTH : integer := Manager1Rec.Address'length ; 
  constant DATA_WIDTH : integer := Manager1Rec.DataToModel'length ;  
end entity TestCtrl ;
