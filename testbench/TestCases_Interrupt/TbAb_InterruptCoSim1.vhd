--
--  File Name:         TbAb_InterruptCoSim1.vhd
--  Design Unit Name:  Architecture of TestCtrl
--  Revision:          OSVVM MODELS STANDARD VERSION
--
--  Maintainer:        Simon Southwell  email: simon.southwell@gmail.com
--  Contributor(s):
--     Simon Southwell      simon.southwell@gmail.com
--
--
--  Description:
--      Test interrupt handling done in OSVVM Interrupt Handler.
--      CoSim interface drives ManagerProc and InterruptProc
--
--  Revision History:
--    Date      Version    Description
--    10/2022   2023.01    Initial revision
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

architecture InterruptCoSim1 of TestCtrl is

  signal ManagerSync1, MemorySync1, TestDone, GenerateIntSync : integer_barrier := 1 ;

begin

  ------------------------------------------------------------
  -- ControlProc
  --   Set up AlertLog and wait for end of test
  ------------------------------------------------------------
  ControlProc : process
  begin

    -- Initialization of test
--    SetTestName("TbAb_InterruptCoSim1") ;
    SetLogEnable(PASSED, TRUE) ;    -- Enable PASSED logs
    SetLogEnable(INFO, TRUE) ;      -- Enable INFO logs
    SetLogEnable(GetAlertLogID("Memory_1"), INFO, FALSE) ;

    -- Wait for testbench initialization
    wait for 0 ns ;  wait for 0 ns ;
    TranscriptOpen(OSVVM_OUTPUT_DIRECTORY & "TbAb_InterruptCoSim1.txt") ;
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
    -- AlertIfDiff("./results/TbAb_InterruptCoSim1.txt", "../AXI4/Axi4/testbench/validated_results/TbAb_InterruptCoSim1.txt", "") ;

    EndOfTestReports ;
    std.env.stop ;
    wait ;
  end process ControlProc ;

  ------------------------------------------------------------
  -- ManagerProc
  --   Generate transactions for AxiManager
  ------------------------------------------------------------
  ManagerProc : process
    variable Data   : std_logic_vector(AXI_DATA_WIDTH-1 downto 0) := (others => '0') ;
    variable Done   : integer := 0 ;
    variable Error  : integer := 0 ;
    variable Node   : integer := 0 ;
    variable Int    : integer := 0 ;
  begin
    wait until nReset = '1' ;
    WaitForClock(ManagerRec, 2) ;

    -- Initialise VProc code
    Node := 1;
    CoSimInit(Node);
    Node := 0;
    CoSimInit(Node);
    -- Fetch the SetTestName
    CoSimTrans(ManagerRec, Done, Error, Int, Node) ;

    for i in 0 to 3 loop
      blankline(2) ;
      log("Main Starting Writes.  Loop #" & to_string(i)) ;

      for j in 0 to 3 loop
        CoSimTrans(ManagerRec, Done, Error, Int, Node) ;
      end loop ;

      WaitForBarrier(GenerateIntSync) ; 
      wait for 9 ns ;
      WaitForClock(ManagerRec, 1) ;
      log("WaitForClock #1 finished") ;
      WaitForClock(ManagerRec, 1) ;
      log("WaitForClock #2 finished") ;
      WaitForClock(ManagerRec, 1) ;
      log("WaitForClock #3 finished") ;
      WaitForClock(ManagerRec, 1) ;
      log("WaitForClock #4 finished") ;

      blankline(2) ;
      log("Main Starting Reads.  Loop #" & to_string(i)) ;

      for j in 0 to 3 loop
        CoSimTrans(ManagerRec, Done, Error, Int, Node) ;
        AlertIf(Error /= 0, "CoSimTrans node 0 flagged an error") ;
      end loop ;

    end loop ;
 
    -- Wait for outputs to propagate and signal TestDone
    WaitForClock(ManagerRec, 2) ;
    WaitForBarrier(TestDone) ;
    wait ;
  end process ManagerProc ;


  ------------------------------------------------------------
  -- InterruptProc
  --   Generate transactions for AxiSubordinate
  ------------------------------------------------------------
  InterruptProc : process
    variable Done   : integer := 0;
    variable Error  : integer := 0;
    variable Node   : integer := 1;
    variable Int    : integer := 0 ;
  begin
    WaitForClock(InterruptRec, 1) ;
    blankline(2) ;
    log("Interrupt Handler Started") ;

    for i in 0 to 7 loop
      CoSimTrans (InterruptRec, Done, Error, Int, Node) ;
      AlertIf(Error /= 0, "CoSimTrans node 1 flagged an error") ;
    end loop ;

    log("Interrupt Handler Done") ;
    blankline(2) ;
    InterruptReturn(InterruptRec) ;
    wait for 0 ns ;
  end process InterruptProc ;


  ------------------------------------------------------------
  -- InterruptGeneratorProc
  --   Generate transactions for AxiSubordinate
  ------------------------------------------------------------
  GenInterruptProc : process
    variable IterationCount : integer := 0 ; 
  begin
    WaitForBarrier(GenerateIntSync) ; 
    -- IntReq <= '1' after IterationCount * 10 ns + 5 ns, '0' after IterationCount * 10 ns + 50 ns ;
    wait for IterationCount * 10 ns + 5 ns ;
    Send(IntGenBit0Rec, "1") ; 
    wait for 45 ns ;
    Send(IntGenBit0Rec, "0") ; 
    
    IterationCount := IterationCount + 1 ; 
  end process GenInterruptProc ;


  ------------------------------------------------------------
  -- SubordinateProc
  --   Generate transactions for AxiSubordinate
  ------------------------------------------------------------
  SubordinateProc : process
    variable Addr : std_logic_vector(AXI_ADDR_WIDTH-1 downto 0) ;
    variable Data : std_logic_vector(AXI_DATA_WIDTH-1 downto 0) ;
  begin

    -- Wait for outputs to propagate and signal TestDone
    WaitForClock(SubordinateRec, 2) ;
    WaitForBarrier(TestDone) ;
    wait ;
  end process SubordinateProc ;


end InterruptCoSim1 ;

Configuration TbAb_InterruptCoSim1 of TbAddressBusMemory is
  for TestHarness
    for TestCtrl_1 : TestCtrl
      use entity work.TestCtrl(InterruptCoSim1) ;
    end for ;
--!!    for Subordinate_1 : Axi4Subordinate
--!!      use entity OSVVM_AXI4.Axi4Memory ;
--!!    end for ;
  end for ;
end TbAb_InterruptCoSim1 ;