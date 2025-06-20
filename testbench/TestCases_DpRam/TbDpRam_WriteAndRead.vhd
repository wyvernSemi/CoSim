--
--  File Name:         TbDpRam_BasicReadWrite.vhd
--  Design Unit Name:  Architecture of TestCtrl
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
--    05/2023   2023.05    Initial revision
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

architecture WriteAndRead of TestCtrl is

  constant Node           : integer         := 0 ;

  signal   TestDone       : integer_barrier := 1 ;

begin

  ------------------------------------------------------------
  -- ControlProc
  --   Set up AlertLog and wait for end of test
  ------------------------------------------------------------
  ControlProc : process
  begin
    -- Initialization of test
    --SetTestName("TbDpRam_BasicReadWrite") ;
    SetLogEnable(PASSED, TRUE) ;    -- Enable PASSED logs
    SetLogEnable(INFO, TRUE) ;    -- Enable INFO logs

    -- Wait for testbench initialization
    wait for 0 ns ;  wait for 0 ns ;
    TranscriptOpen ;  -- SetTestName done in SW
    SetTranscriptMirror(TRUE) ;

    -- Wait for Design Reset
    wait until nReset = '1' ;
    ClearAlerts ;

    -- Wait for test to finish
    WaitForBarrier(TestDone, 35 ms) ;

    TranscriptClose ;
    -- Printing differs in different simulators due to differences in process order execution
    -- AffirmIfTranscriptsMatch(PATH_TO_VALIDATED_RESULTS) ;

    EndOfTestReports(TimeOut => (now >= 35 ms)) ;
    std.env.stop ;
    wait ;
  end process ControlProc ;

  ------------------------------------------------------------
  -- ManagerProc
  --   Generate transactions for AxiManager
  ------------------------------------------------------------
  ManagerProc : process
    variable OpRV           : RandomPType ;
    variable WaitForClockRV : RandomPType ;
    
    -- CoSim variables
    variable Done           : integer := 0 ;
    variable Error          : integer := 0 ;
    variable IntReq         : integer := 0 ;
    variable NodeNum        : integer := Node ;
    
  begin

    -- Initialize Randomization Objects
    OpRV.InitSeed(OpRv'instance_name) ;
    WaitForClockRV.InitSeed(WaitForClockRV'instance_name) ;

    -- Initialise VProc code
    CoSimInit(NodeNum) ;

    wait until nReset = '1' ;
    -- First Alignment to clock
    WaitForClock(Manager1Rec, 1) ;

    OperationLoop : loop

      -- 20 % of the time add a no-op cycle with a delay of 1 to 5 clocks
      if WaitForClockRV.DistInt((8, 2)) = 1 then
        WaitForClock(Manager1Rec, WaitForClockRV.RandInt(1, 5)) ;
      end if ;

      -- Call CoSimTrans procedure to generate an access from the running VProc program
      CoSimTrans (Manager1Rec, Done, Error, IntReq, NodeNum);

      AlertIf(Error /= 0, "CoSimTrans flagged an error") ;

      -- Finish when counts == 0
      exit when Done /= 0;

    end loop OperationLoop ;

    WaitForBarrier(TestDone) ;
    wait ;
    
  end process ManagerProc ;


end WriteAndRead ;

library OSVVM_AXI4 ;

Configuration TbDpRam_WriteAndRead of TbDpRam is
  for TestHarness
    for TestCtrl_1 : TestCtrl
      use entity work.TestCtrl(WriteAndRead) ;
    end for ;
  end for ;
end TbDpRam_WriteAndRead ;