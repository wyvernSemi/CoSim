--
--  File Name:         TbAb_ReadPoll.vhd
--  Design Unit Name:  Architecture of TestCtrl
--  Revision:          OSVVM MODELS STANDARD VERSION
--
--  Maintainer:        Simon Southwell  email: simon.southwell@gmail.com
--  Contributor(s):
--     Simon Southwell      simon.southwell@gmail.com
--     Jim Lewis            jim@synthworks.com
--
--
--  Description:
--      Test read poll of CoSim interface
--
--  Revision History:
--    Date      Version    Description
--    05/2023   2023.05    Initial revision
--
--
--  This file is part of OSVVM.
--
--  Copyright (c) 2022 by [OSVVM Authors](../../AUTHORS.md)
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

architecture ReadPoll of TestCtrl is

  signal ManagerSync1, MemorySync1, TestDone : integer_barrier := 1 ;

begin

  ------------------------------------------------------------
  -- ControlProc
  --   Set up AlertLog and wait for end of test
  ------------------------------------------------------------
  ControlProc : process
  begin

    -- Initialization of test
--    SetTestName("TbAb_InterruptCoSim2") ;
    SetLogEnable(PASSED, TRUE) ;    -- Enable PASSED logs
    SetLogEnable(INFO, TRUE) ;    -- Enable INFO logs
    SetLogEnable(GetAlertLogID("Memory_1"), INFO, FALSE) ;

    -- Wait for testbench initialization
    wait for 0 ns ;  wait for 0 ns ;
    TranscriptOpen ;
    SetTranscriptMirror(TRUE) ;

    -- Wait for Design Reset
    wait until nReset = '1' ;
    ClearAlerts ;

    -- Wait for test to finish
    WaitForBarrier(TestDone, 35 ms) ;
    AlertIf(now >= 35 ms, "Test finished due to timeout") ;
    AlertIf(GetAffirmCount < 1, "Test is not Self-Checking");


    TranscriptClose ;
    -- Printing differs in different simulators due to differences in process order execution
    -- AlertIfDiff("./results/TbAb_InterruptCoSim2.txt", "../AXI4/Axi4/testbench/validated_results/TbAb_InterruptCoSim2.txt", "") ;

    EndOfTestReports ;
    std.env.stop ;
    wait ;
  end process ControlProc ;

  ------------------------------------------------------------
  -- ManagerProc
  --   Generate transactions for AxiManager
  ------------------------------------------------------------
  ManagerProc : process
    variable Data        : std_logic_vector(AXI_DATA_WIDTH-1 downto 0) := (others => '0') ;
    variable Done        : integer := 0 ;
    variable Error       : integer := 0 ;
    variable Node        : integer := 0 ;
    variable Int         : integer := 0 ;
    variable WaitForClockRV : RandomPType ;
  begin
    wait until nReset = '1' ;
    WaitForClock(ManagerRec, 2) ;

    -- Initialise VProc code
    CoSimInit(Node);
    -- Fetch the SetTestName
    CoSimTrans(ManagerRec, Done, Error, Int, Node) ;

    OperationLoop : loop
    
      -- 20 % of the time add a no-op cycle with a delay of 1 to 5 clocks
      if WaitForClockRV.DistInt((8, 2)) = 1 then
        WaitForClock(ManagerRec, WaitForClockRV.RandInt(1, 5)) ;
      end if ;
      
      -- Inspect interrupt state and and convert to integer
      Int         := to_integer(signed(gIntReq)) ;
      toggle(gVProcReadInterrupts) ; 

      -- Call co-simulation procedure
      CoSimTrans(ManagerRec, Done, Error, Int, Node) ;

      -- Alter if an error
      AlertIf(Error /= 0, "CoSimTrans flagged an error") ;

      -- Finish when counts == 0
      exit when Done /= 0;

    end loop OperationLoop ;
 
    -- Wait for outputs to propagate and signal TestDone
    WaitForClock(ManagerRec, 2) ;
    WaitForBarrier(TestDone) ;
    wait ;
  end process ManagerProc ;

  ------------------------------------------------------------
  -- SubordinateProc
  --   Generate transactions for AxiSubordinate
  ------------------------------------------------------------
  SubordinateProc : process
    variable Addr : std_logic_vector(AXI_ADDR_WIDTH-1 downto 0) ;
    variable Data : std_logic_vector(AXI_DATA_WIDTH-1 downto 0) ;
  begin
  
    WaitForClock(SubordinateRec, 2) ;
    
    -- Get a write to know program has started
    GetWrite(SubordinateRec, Addr, Data) ;
    
    -- First read send non-matching bits
    SendRead(SubordinateRec, Addr, X"0000_0001") ; 
    SendRead(SubordinateRec, Addr, X"0000_0002") ; 
    SendRead(SubordinateRec, Addr, X"0000_0004") ; 
    SendRead(SubordinateRec, Addr, X"0000_0008") ; 
    SendRead(SubordinateRec, Addr, X"0000_0010") ; 
    SendRead(SubordinateRec, Addr, X"0000_0020") ; 
    SendRead(SubordinateRec, Addr, X"0000_0040") ; 
    SendRead(SubordinateRec, Addr, X"0000_0080") ; 
    SendRead(SubordinateRec, Addr, X"0000_0100") ; 
    SendRead(SubordinateRec, Addr, X"0000_0200") ; 
    SendRead(SubordinateRec, Addr, X"0000_0400") ; 
    SendRead(SubordinateRec, Addr, X"0000_0800") ; 
    SendRead(SubordinateRec, Addr, X"0000_1000") ; 
    SendRead(SubordinateRec, Addr, X"0000_2000") ; 
    SendRead(SubordinateRec, Addr, X"0000_8000") ; 
    
    SendRead(SubordinateRec, Addr, X"0001_0000") ; 
    SendRead(SubordinateRec, Addr, X"0002_0000") ; 
    SendRead(SubordinateRec, Addr, X"0004_0000") ; 
    SendRead(SubordinateRec, Addr, X"0008_0000") ; 
    SendRead(SubordinateRec, Addr, X"0010_0000") ; 
    SendRead(SubordinateRec, Addr, X"0020_0000") ; 
    SendRead(SubordinateRec, Addr, X"0040_0000") ; 
    SendRead(SubordinateRec, Addr, X"0080_0000") ; 
    SendRead(SubordinateRec, Addr, X"0100_0000") ; 
    SendRead(SubordinateRec, Addr, X"0200_0000") ; 
    SendRead(SubordinateRec, Addr, X"0400_0000") ; 
    SendRead(SubordinateRec, Addr, X"0800_0000") ; 
    SendRead(SubordinateRec, Addr, X"1000_0000") ; 
    SendRead(SubordinateRec, Addr, X"2000_0000") ; 
    SendRead(SubordinateRec, Addr, X"4000_0000") ;
    SendRead(SubordinateRec, Addr, X"8000_0000") ;
    
    -- Send matching bit
    SendRead(SubordinateRec, Addr, X"0000_4000") ;

    -- Wait for outputs to propagate and signal TestDone
    WaitForClock(SubordinateRec, 2) ;
    WaitForBarrier(TestDone) ;
    wait ;
  end process SubordinateProc ;


end ReadPoll ;

Configuration TbAb_ReadPoll of TbAxi4 is
  for TestHarness
    for TestCtrl_1 : TestCtrl
      use entity work.TestCtrl(ReadPoll) ;
    end for ;
  end for ;
end TbAb_ReadPoll ;