--
--  File Name:           TbAb_Rv32Demo.vhd
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
--    07/2026   2026.01    Initial revision
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

architecture Rv32Demo of TestCtrl is

  constant NodeRv32       : integer         := 0 ;
  constant NodeUartTx     : integer         := 1 ;
  constant NodeUartRx     : integer         := 2 ;

  signal   DummyUartRxRec : UartRecType ;
  signal   DummyUartTxRec : UartRecType ;

  signal   TestDone       : integer_barrier := 1 ;

begin

  ------------------------------------------------------------
  -- ControlProc
  --   Set up AlertLog and wait for end of test
  ------------------------------------------------------------
  ControlProc : process
  begin
    -- Initialization of test
    --!! NOTE:  SetTestName called by software
    SetTestName("TbAb_Rv32Demo") ;
    SetLogEnable(PASSED, FALSE) ;    -- Enable PASSED logs
    SetLogEnable(INFO, FALSE) ;    -- Enable INFO logs

    -- Wait for testbench initialization
    wait for 0 ns ;  wait for 0 ns ;
    TranscriptOpen ;
    SetTranscriptMirror(TRUE) ;

    if STOP_AT_TIME_ZERO then
      std.env.stop;
    end if ;

    -- Wait for Design Reset
    wait until nReset = '1' ;
    ClearAlerts ;

    -- Wait for test to finish
    WaitForBarrier(TestDone, 50 ms) ;

    TranscriptClose ;

    EndOfTestReports(TimeOut => (now >= 50 ms)) ;
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

    -- variables;
    variable Done           : integer := 0 ;
    variable Error          : integer := 0 ;
    variable IntReq         : integer := 0 ;
    variable NodeNum        : integer := NodeRv32 ;

  begin
    -- Initialize Randomization Objects
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
      CoSimTrans (ManagerRec, Done, Error, IntReq, NodeNum);

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
  -- UartTxProc
  --   Generate transactions for UART transmitter
  ------------------------------------------------------------
  UartTxProc : process

    -- variables
    variable Done           : integer := 0 ;
    variable Error          : integer := 0 ;
    variable NodeNum        : integer := NodeUartTx ;

  begin

    -- Initialise VProc code
    CoSimInit(NodeNum);

    -- Find exit of reset
    wait until nReset = '1' ;
    WaitForClock(UartTxRec, 2) ;

    OperationLoop : loop

      -- Call CoSimTrans procedure to generate an access from the running VProc program
      CoSimStream (UartTxRec, DummyUartRxRec, Done, Error, NodeNum);

      AlertIf(Error /= 0, "CoSimTrans flagged an error") ;

      -- Finish when Done flagged
      exit when Done /= 0;

    end loop OperationLoop ;

    -- Allow Subordinate to catch up
    WaitForClock(UartTxRec, 2) ;

    -- Wait for outputs to propagate and signal TestDone
    WaitForClock(UartTxRec, 2) ;
    WaitForBarrier(TestDone) ;
    wait ;
  end process UartTxProc ;

  ------------------------------------------------------------
  -- UartRxProc
  --   Generate transactions for UART receiver
  ------------------------------------------------------------
  UartRxProc : process

    --  variables
    variable Done           : integer := 0 ;
    variable Error          : integer := 0 ;
    variable NodeNum        : integer := NodeUartRx ;

  begin

    -- Initialise VProc code
    CoSimInit(NodeNum);

    -- Find exit of reset
    wait until nReset = '1' ;
    WaitForClock(UartRxRec, 2) ;

    OperationLoop : loop

      -- Call CoSimTrans procedure to generate an access from the running VProc program
      CoSimStream (DummyUartTxRec, UartRxRec, Done, Error, NodeNum);

      AlertIf(Error /= 0, "UartRxProc: CoSimTrans flagged an error") ;

      -- Finish when Done flagged
      exit when Done /= 0;

    end loop OperationLoop ;

    -- Wait for outputs to propagate and signal TestDone
    WaitForClock(UartRxRec, 2) ;
    WaitForBarrier(TestDone) ;
    wait ;
  end process UartRxProc ;

  ------------------------------------------------------------
  -- UartTxStub
  --   Process to consume WAIT_FOR_CLOCK transactions from
  --   node2 to allow Done and Error signal propagation
  --   from node2's CoSimStream, which uses TxRec connection
  --   for this operation
  ------------------------------------------------------------

  UartTxStub : process
  variable ModelID : AlertLogIDType ;
  begin

    TransactionDispatcherLoop : loop

      WaitForTransaction(
         Clk      => Clk,
         Rdy      => DummyUartTxRec.Rdy,
         Ack      => DummyUartTxRec.Ack
      ) ;

      case DummyUartTxRec.Operation is

        when WAIT_FOR_CLOCK =>
          WaitForClock(Clk, DummyUartTxRec.IntToModel, std_logic'val(DummyUartTxRec.Options)) ;

        when others =>
          -- Signal multiple Driver Detect or not implemented transactions.
          Alert(ClassifyUnimplementedOperation(DummyUartTxRec), FAILURE) ;

      end case;

    end loop TransactionDispatcherLoop ;

  end process UartTxStub ;

end Rv32Demo ;

Configuration TbAb_Rv32Demo of TbAbRv32Demo is
  for TestHarness
    for TestCtrl_1 : TestCtrl
      use entity work.TestCtrl(Rv32Demo) ;
    end for ;
  end for ;
end TbAb_Rv32Demo ;