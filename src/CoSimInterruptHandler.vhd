--
--  File Name:         CoSimInterruptHandler.vhd
--  Design Unit Name:  CoSimInterruptHandler
--  Revision:          OSVVM MODELS STANDARD VERSION
--
--  Maintainer:        Jim Lewis      email:  jim@synthworks.com
--  Contributor(s):
--     Jim Lewis      jim@synthworks.com
--
--
--  Description:
--      Passes interrupts to CoSim environment.   
--      Edge sensitive inputs are held long enough for them to be 
--      passed to the CoSim interface and then they are cleared.
--
--
--  Developed by:
--        SynthWorks Design Inc.
--        VHDL Training Classes
--        http://www.SynthWorks.com
--
--  Revision History:
--    Date      Version    Description
--    01/2023   2023.01    Initial revision
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

library osvvm ;
  context osvvm.OsvvmContext ;
  use osvvm.ScoreboardPkg_slv.all ; 
  
library osvvm_common ;
  use osvvm_common.AddressBusTransactionPkg.all; 
  use osvvm_common.InterruptGlobalSignalPkg.all ;


entity CoSimInterruptHandler is
generic (
  EDGE_LEVEL       : std_logic_vector(gIntReq'range) := (others => INTERRUPT_ON_LEVEL) ;
  POLARITY         : std_logic_vector(gIntReq'range) := (others => '1')
) ;
port (
  -- Interrupt Input
  IntReq          : in   std_logic_vector(gIntReq'range)
) ;
end entity CoSimInterruptHandler ;
architecture Behavioral of CoSimInterruptHandler is
  signal gIntReqDelayed : gIntReq'subtype ; 
begin

  gIntReqDelayed <= gIntReq ; 
  
  GenInterrupts : for i in gIntReq'range generate
    gIntReq(i) <= 
      -- Clear when read by VProc. gIntReq'delayed is what was read by VProc
      '0'  when gVProcReadInterrupts'event and gIntReqDelayed(i) = '1' and EDGE_LEVEL(i) = INTERRUPT_ON_EDGE else

      -- If level, pass IntReq(i) thru qualified by POLARITY
      to_01(IntReq(i)) xnor POLARITY(i)  when EDGE_LEVEL(i) = INTERRUPT_ON_LEVEL else

      -- if Edge, capture IntReq(i) when matches polarity and it just changed
      '1'  when IntReq(i) = POLARITY(i) and IntReq(i)'event ; 
  end generate GenInterrupts ; 
  
end architecture Behavioral ; 