--
--  File Name:         Tb_xMii1.vhd
--  Design Unit Name:  Architecture of TestCtrl
--  Revision:          OSVVM MODELS STANDARD VERSION
--
--  Maintainer:        Simon Southwell email:  simon.southwell@gmail.com
--  Contributor(s):
--     Simon Southwell simon.southwell@gmail.com
--     Jim Lewis       jim@synthworks.com
--
--
--  Description:
--      Test for OSVVM co-simulation Ethernet streams
--
--
--  Developed by:
--        SynthWorks Design Inc.
--        VHDL Training Classes
--        http://www.SynthWorks.com
--
--  Revision History:
--    Date      Version    Description
--    03/2023   2023.04    Initial Release
--
--
--  This file is part of OSVVM.
--
--  Copyright (c) 2023 by [OSVVM Authors](../../AUTHORS.md)
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
architecture xMii1 of TestCtrl is

  signal   TestDone : integer_barrier := 1 ;

begin

  ------------------------------------------------------------
  -- ControlProc
  --   Set up AlertLog and wait for end of test
  ------------------------------------------------------------
  ControlProc : process
  begin
    -- Initialization of test
    SetLogEnable(PASSED, TRUE) ;    -- Enable PASSED logs
    SetLogEnable(INFO, TRUE) ;    -- Enable INFO logs

    -- Wait for testbench initialization
    wait for 0 ns ;  wait for 0 ns ;
    TranscriptOpen("Tb_xMii1.txt") ;
    SetTranscriptMirror(TRUE) ;

    -- Wait for Design Reset
--    wait until nReset = '1' ;
    ClearAlerts ;

    -- Wait for test to finish
    WaitForBarrier(TestDone, 5 ms) ;
    AlertIf(now >= 5 ms, "Test finished due to timeout") ;
    AlertIf(GetAffirmCount < 1, "Test is not Self-Checking");

    TranscriptClose ;

    EndOfTestReports ;
    std.env.stop ;
    wait ;
  end process ControlProc ;


  ------------------------------------------------------------
  MacProc : process
  ------------------------------------------------------------

    variable OpRV           : RandomPType ;
    variable WaitForClockRV : RandomPType ;

    variable NodeNum        : integer := 0 ;
    variable Done           : integer := 0 ;
    variable Error          : integer := 0 ;
  begin

    -- Initialize Randomization Objects
    OpRV.InitSeed(OpRv'instance_name) ;
    WaitForClockRV.InitSeed(WaitForClockRV'instance_name) ;

    -- Initialise VProc code
    CoSimInit(NodeNum);
    CoSimStream(MacTxRec, MacRxRec, Done, Error, NodeNum);

    WaitForClock(MacTxRec, 2) ;

    -- Main loop to call CoSimStream
    OperationLoop : loop

      -- 20 % of the time add a no-op cycle with a delay of 1 to 5 clocks
      if WaitForClockRV.DistInt((8, 2)) = 1 then
        WaitForClock(MacTxRec, WaitForClockRV.RandInt(1, 5)) ;
      end if ;

      -- Fetch new MAC stream TX operation and receive data from PHY
      CoSimStream(MacTxRec, MacRxRec, Done, Error, NodeNum);

      AlertIf(Error /= 0, "MacTxProc CoSimStream flagged an error") ;

      -- Finish when flagged by software
      exit when Done /= 0;

    end loop OperationLoop ;

    -- Wait for outputs to propagate and signal TestDone
    WaitForClock(MacTxRec, 2) ;
    WaitForBarrier(TestDone) ;
    wait ;
  end process MacProc ;

  ------------------------------------------------------------
  PhyProc : process
  ------------------------------------------------------------

    variable OpRV           : RandomPType ;
    variable WaitForClockRV : RandomPType ;

    variable NodeNum        : integer := 1 ;
    variable Done           : integer := 0 ;
    variable Error          : integer := 0 ;
  begin
    WaitForClock(PhyRxRec, 2) ;

    -- Initialize Randomization Objects
    OpRV.InitSeed(OpRv'instance_name) ;
    WaitForClockRV.InitSeed(WaitForClockRV'instance_name) ;

    -- Initialise VProc code
    CoSimInit(NodeNum);

    WaitForClock(PhyRxRec, 2) ;

    -- Main loop to call CoSimStream
    OperationLoop : loop

      -- 20 % of the time add a no-op cycle with a delay of 1 to 5 clocks
      if WaitForClockRV.DistInt((8, 2)) = 1 then
        WaitForClock(PhyRxRec, WaitForClockRV.RandInt(1, 5)) ;
      end if ;

      -- Fetch new PHY stream TX operation and receive data from MAC
      CoSimStream(PhyRxRec, PhyTxRec, Done, Error, NodeNum);

      AlertIf(Error /= 0, "MacRxProc CoSimStream flagged an error") ;

      -- Finish when flagged by software
      exit when Done /= 0;

    end loop OperationLoop ;

    -- Wait for outputs to propagate and signal TestDone
    WaitForClock(PhyRxRec, 2) ;
    WaitForBarrier(TestDone) ;
    wait ;
  end process PhyProc ;

end xMii1 ;

Configuration Tb_xMii1 of TbStandAlone is
  for TestHarness
    for TestCtrl_1 : TestCtrl
      use entity work.TestCtrl(xMii1) ;
    end for ;
  end for ;
end Tb_xMii1 ;