--
--  File Name:           TbAb_CoSim.vhd
--  Design Unit Name:    Architecture of TestCtrl
--  Revision:            OSVVM MODELS STANDARD VERSION
--
--  Maintainer:          Simon Southwell  email: simon.southwell@gmail.com
--  Contributor(s):
--     Simon Southwell  simon.southwell@gmail.com
--
--
--  Description:
--      Test transaction source for co-simulation demo
--
--
--  Revision History:
--    Date      Version    Description
--    07/2026   2026.?08    Initial revision
--
--
--  This file is part of OSVVM.
--
--  Copyright (c) 2026 by [OSVVM Authors](../../AUTHORS.md)
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

architecture CoSim_Demo of TestCtrl is

  constant NodeImgRx      : integer         := 0 ;
  constant NodeImgTx      : integer         := 1 ;

  signal   TestDone       : integer_barrier := 1 ;

begin

  ------------------------------------------------------------
  ControlProc : process
  --
  --   Set up AlertLog and wait for end of test
  ------------------------------------------------------------
  begin
    SetTestName("TbAb_CoSim_Demo") ;
    
    -- Initialisation of test
    SetLogEnable(PASSED, FALSE) ;  -- Enable/disable PASSED logs
    SetLogEnable(INFO, TRUE) ;     -- Enable/disable INFO logs

    -- Wait for test bench initialisation
    wait for 0 ns ;  wait for 0 ns ;
    TranscriptOpen ;
    SetTranscriptMirror(TRUE) ;

    -- If generic set, stop now, at time 0 for debug attachment
    if STOP_AT_TIME_ZERO then
      std.env.stop ;
    end if ;

    -- Wait for reset to be de-asserted
    wait until nReset = '1' ;
    ClearAlerts ;

    -- Wait for test processes to finish
    WaitForBarrier(TestDone, 1 ms) ;

    TranscriptClose ;

    EndOfTestReports(TimeOut => (now >= 1 ms)) ;
    std.env.stop ;
    wait ;
  end process ControlProc ;

  ------------------------------------------------------------
  ManagerProc : process
  --
  --   Generate transactions for AxiManager
  ------------------------------------------------------------
    variable OpRV           : RandomPType ;
    variable WaitForClockRV : RandomPType ;

    -- CoSim variables
    variable Done           : integer := 0 ;
    variable Error          : integer := 0 ;
    variable Irq            : integer := 0 ;
    variable NodeNum        : integer := NodeImgRx ;
  begin
    -- Initialise Randomisation Objects
    OpRV.InitSeed(OpRv'instance_name) ;
    WaitForClockRV.InitSeed(WaitForClockRV'instance_name) ;

    -- Initialise VProc code
    CoSimInit(NodeNum);

    -- Find exit of reset
    wait until nReset = '1' ;
    WaitForClock(ManagerRec, 2) ;

    OperationLoop : loop

      -- 20 % of the time add a no-op cycle with a delay of 1 to 5 clocks
      if WaitForClockRV.DistInt((8, 2)) = 1 then
        WaitForClock(ManagerRec, WaitForClockRV.RandInt(1, 5)) ;
      end if ;

      -- Call CoSimTrans procedure to generate an access from the running VProc program
      CoSimTrans (ManagerRec, Done, Error, Irq, NodeNum);

      AlertIf(Error /= 0, "CoSimTrans flagged an error") ;

      -- Finish when Done flagged
      exit when Done /= 0;

    end loop OperationLoop ;

    -- Wait for outputs to propagate and signal TestDone
    WaitForClock(ManagerRec, 2) ;
    WaitForBarrier(TestDone) ;
    wait ;

  end process ManagerProc ;

  ------------------------------------------------------------
  ImageGenProc : process
  --
  --   Generate image data transactions for AxiMemory
  ------------------------------------------------------------
    variable OpRV           : RandomPType ;
    variable WaitForClockRV : RandomPType ;

    -- CoSim variables
    variable Done           : integer := 0 ;
    variable Error          : integer := 0 ;
    variable Irq            : integer := 0 ; -- UNUSED
    variable NodeNum        : integer := NodeImgTx ;
  begin

    -- Initialise VProc code
    CoSimInit(NodeNum) ;

    -- Find exit of reset
    wait until nReset = '1' ;
    WaitForClock(SubordinateRec, 2) ;

    OperationLoop : loop

      -- Call CoSimTrans procedure to generate an access from the running program
      CoSimTrans (SubordinateRec, Done, Error, Irq, NodeNum);

      exit when Done /= 0 ;

    end loop ;

    WaitForBarrier(TestDone) ;
    wait ;

  end process ImageGenProc ;

end architecture CoSim_Demo ;

------------------------------------------------------------
Configuration TbAb_CoSim_Demo of TbCosimDemo is
------------------------------------------------------------
  for TestHarness
    for TestCtrl_1 : TestCtrl
      use entity work.TestCtrl(CoSim_Demo) ;
    end for ;
  end for ;
end TbAb_CoSim_Demo ;