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
--    07/2025   ????.??    Updated to use CoSimIrq
--    09/2022   2023.01    Initial revision
--
--
--  This file is part of OSVVM.
--
--  Copyright (c) 2022 - 2025 by [OSVVM Authors](../../AUTHORS.md)
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

architecture CoSim of TestCtrl is

--  constant BURST_MODE     : AddressBusFifoBurstModeType := ADDRESS_BUS_BURST_WORD_MODE ;
  constant BURST_MODE     : AddressBusFifoBurstModeType := ADDRESS_BUS_BURST_BYTE_MODE ;
  constant Node           : integer         := 0 ;

  signal   TestDone       : integer_barrier := 1 ;
  signal   TestActive     : boolean         := TRUE ;
  signal   OperationCount : integer         := 0 ;
  signal   Initialised    : boolean         := FALSE;       

begin

  ------------------------------------------------------------
  -- ControlProc
  --   Set up AlertLog and wait for end of test
  ------------------------------------------------------------
  ControlProc : process
  begin
    -- Initialization of test
    --!! NOTE:  SetTestName called by software
--    SetTestName("TbAb_CoSim") ;
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
    WaitForBarrier(TestDone, 1 ms) ;

    TranscriptClose ;
    -- Printing differs in different simulators due to differences in process order execution
    -- AffirmIfTranscriptsMatch(PATH_TO_VALIDATED_RESULTS) ;

    EndOfTestReports(TimeOut => (now >= 1 ms)) ;
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
    variable counts         : integer := 0;

    -- CoSim variables
    variable RnW            : integer ;
    variable Done           : integer := 0 ;
    variable Error          : integer := 0 ;
    variable IntReq         : integer := 0 ;
    variable NodeNum        : integer := Node ;

    variable Count          : integer ;
  begin
    -- Initialize Randomization Objects
    OpRV.InitSeed(OpRv'instance_name) ;
    WaitForClockRV.InitSeed(WaitForClockRV'instance_name) ;

    -- Initialise VProc code
    CoSimInit(NodeNum);
    Initialised <= TRUE;
    -- Fetch the SetTestName
    --CoSimTrans (ManagerRec, Done, Error, IntReq, NodeNum);

    SetBurstMode(ManagerRec, BURST_MODE) ;

    -- Find exit of reset
    wait until nReset = '1' ;
    WaitForClock(ManagerRec, 2) ;

    OperationLoop : loop

      -- 20 % of the time add a no-op cycle with a delay of 1 to 5 clocks
      if WaitForClockRV.DistInt((8, 2)) = 1 then
        WaitForClock(ManagerRec, WaitForClockRV.RandInt(1, 5)) ;
      end if ;

      -- Call CoSimTrans procedure to generate an access from the running VProc program
      CoSimTrans (ManagerRec, Done, Error, IntReq, NodeNum);

      AlertIf(Error /= 0, "CoSimTrans flagged an error") ;

      -- Finish when Done flagged
      exit when Done /= 0;
      
      counts := counts + 1;

    end loop OperationLoop ;

    TestActive <= FALSE ;

    -- Allow Subordinate to catch up before signaling OperationCount (needed when WRITE_OP is last)
    -- wait for 0 ns ;  -- this is enough
    WaitForClock(ManagerRec, 2) ;
    Increment(OperationCount) ;

    -- Wait for outputs to propagate and signal TestDone
    WaitForClock(ManagerRec, 2) ;
    WaitForBarrier(TestDone) ;
    wait ;
  end process ManagerProc ;
  
  ------------------------------------------------------------
  -- InterruptProc
  --   Generate interupts in lieu of a DUT
  ------------------------------------------------------------
  InterruptProc : process
  begin
  
    wait until nReset = '1' ;

    wait for 85 ns ; 
    gIntReq(0) <= force '1' ;
    wait for 50 ns ; 
    gIntReq(0) <= force '0' ;
     wait for 180 ns ; 
    gIntReq(0) <= force '1' ;
    wait for 190 ns ; 
    gIntReq(0) <= force '0' ;
  
    wait ;
  
  end process InterruptProc ;
  
  ------------------------------------------------------------
  -- ProcessIrq
  --   Process interrupts and send to cosimulation code
  ------------------------------------------------------------
  ProcessIrq : process (gIntReq)
    variable Int            : integer := 0 ;
    variable NodeNum        : integer := Node ;
  begin
    if Initialised then
      Int := to_integer(gIntReq(0));
      
      --Log("**** ProcessIrq " & integer'image(Int));
      CoSimIrq(Int, NodeNum);
   end if ;

  end process ProcessIrq ;

end CoSim ;

Configuration TbAb_CoSim of TbAddressBusMemory is
  for TestHarness
    for TestCtrl_1 : TestCtrl
      use entity work.TestCtrl(CoSim) ;
    end for ;
  end for ;
end TbAb_CoSim ;