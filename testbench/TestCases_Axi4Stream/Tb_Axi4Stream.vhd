--
--  File Name:         TbStream_SendGet1.vhd
--  Design Unit Name:  Architecture of TestCtrl
--  Revision:          OSVVM MODELS STANDARD VERSION
--
--  Maintainer:        Jim Lewis      email:  jim@synthworks.com
--  Contributor(s):
--     Jim Lewis      jim@synthworks.com
--
--
--  Description:
--      Validates Stream Model Independent Transactions
--      Send, Get, Check,
--      WaitForTransaction, GetTransactionCount
--      GetAlertLogID, GetErrorCount,
--
--
--  Developed by:
--        SynthWorks Design Inc.
--        VHDL Training Classes
--        http://www.SynthWorks.com
--
--  Revision History:
--    Date      Version    Description
--    03/2023   2023.04   Initial revision
--
--
--  This file is part of OSVVM.
--
--  Copyright (c) 2018 - 2020 by SynthWorks Design Inc.
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
architecture Axi4Stream of TestCtrl is
--  constant BURST_MODE     : StreamFifoBurstModeType := STREAM_BURST_WORD_MODE ;   
--  constant BURST_MODE     : StreamFifoBurstModeType := STREAM_BURST_WORD_PARAM_MODE ;   
  constant BURST_MODE     : StreamFifoBurstModeType := STREAM_BURST_BYTE_MODE ;

  signal   TestDone : integer_barrier := 1 ;

begin

  ------------------------------------------------------------
  -- ControlProc
  --   Set up AlertLog and wait for end of test
  ------------------------------------------------------------
  ControlProc : process
  begin
    -- Initialization of test
    SetTestName("CoSim_axi4_streams") ;
    SetLogEnable(PASSED, TRUE) ;    -- Enable PASSED logs
    SetLogEnable(INFO, TRUE) ;    -- Enable INFO logs

    -- Wait for simulation elaboration/initialization
    wait for 0 ns ;  wait for 0 ns ;
    TranscriptOpen("CoSim_axi4_streams.txt") ;
    SetTranscriptMirror(TRUE) ;

    -- Wait for Design Reset
    wait until nReset = '1' ;
    ClearAlerts ;

    -- Wait for test to finish
    WaitForBarrier(TestDone, 35 ms) ;
    AlertIf(now >= 35 ms, "Test finished due to timeout") ;
    --AlertIf(GetAffirmCount < 1, "Test is not Self-Checking");

    TranscriptClose ;
--    AlertIfDiff("./results/CoSim_axi4_streams.txt", "../sim_shared/validated_results/CoSim_axi4_streams.txt", "") ;

    EndOfTestReports(ExternalErrors => (0, 0, 0)) ;
    std.env.stop ;
    wait ;
  end process ControlProc ;

  ------------------------------------------------------------
  -- AxiTransmitterReceiverProc
  --   Generate transactions for AxiTransmitter
  ------------------------------------------------------------
  StreamTxRxProc : process
    variable OpRV           : RandomPType ;
    variable WaitForClockRV : RandomPType ;

    variable NodeNum        : integer := 0 ;
    variable Done           : integer := 0 ;
    variable Error          : integer := 0 ;
  begin
    -- Initialize Randomization Objects
    OpRV.InitSeed(OpRv'instance_name) ;
    WaitForClockRV.InitSeed(WaitForClockRV'instance_name) ;
    
    SetBurstMode(StreamTxRec, BURST_MODE) ;
    SetBurstMode(StreamRxRec, BURST_MODE) ;

    -- Initialise Co-simulation user code
    CoSimInit(NodeNum);

    wait until nReset = '1' ;
    WaitForClock(StreamTxRec, 2) ;

    OperationLoop:  loop
      -- 20 % of the time add a no-op cycle with a delay of 1 to 5 clocks
      if WaitForClockRV.DistInt((8, 2)) = 1 then
        WaitForClock(StreamTxRec, WaitForClockRV.RandInt(1, 5)) ;
      end if ;

      -- Fetch new MAC stream TX operation and receive data from PHY
      CoSimStream(StreamTxRec, StreamRxRec, Done, Error, NodeNum);

      AlertIf(Error /= 0, "StreamTxRxProc CoSimStream flagged an error") ;

      -- Finish when flagged by software
      exit when Done /= 0;
    end loop ;

    -- Wait for outputs to propagate and signal TestDone
    WaitForClock(StreamTxRec, 2) ;
    WaitForBarrier(TestDone) ;
    wait ;
  end process StreamTxRxProc ;


end Axi4Stream ;

Configuration Tb_Axi4Stream of TbStream is
  for TestHarness
    for TestCtrl_1 : TestCtrl
      use entity work.TestCtrl(Axi4Stream) ;
    end for ;
  end for ;
end Tb_Axi4Stream ;