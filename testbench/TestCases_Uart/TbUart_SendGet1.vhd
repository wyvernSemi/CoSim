--
--  File Name:         TbUart_SendGet1.vhd
--  Design Unit Name:  TbUart_SendGet1
--  OSVVM Release:     OSVVM MODELS STANDARD VERSION
--
--  Maintainer:        Jim Lewis      email:  jim@synthworks.com
--  Contributor(s):
--     Jim Lewis      jim@synthworks.com
--
--
--  Description:
--    Validate send, get, check transactions with errors in PE, SE, and BE
--
--
--  Developed by:
--        SynthWorks Design Inc.
--        VHDL Training Classes
--        http://www.SynthWorks.com
--
--  Revision History:
--    Date      Version    Description
--    1999      1999.00    Developed for SynthWorks' Advanced VHDL Testbenches and Verification Class
--    05/2019   2019.05    Updated for OSVVM public release
--    01/2020   2020.01    Updated license notice
--
--
--  This file is part of OSVVM.
--
--  Copyright (c) 1999 - 2020 by SynthWorks Design Inc.
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

architecture SendGet1 of TestCtrl is

  signal CheckErrors : boolean ;
  signal TestActive  : boolean := TRUE ;

  signal TestDone    : integer_barrier := 1 ;

  use osvvm_uart.ScoreboardPkg_Uart.all ;
  --signal UartScoreboard : osvvm_uart.ScoreboardPkg_Uart.ScoreboardIdType ;

begin

  ------------------------------------------------------------
  -- ControlProc
  --   Set up AlertLog and wait for end of test
  ------------------------------------------------------------
  ControlProc : process
  begin
    -- Initialization of test
    SetTestName("Cosim_uart_streams") ;
    SetLogEnable(PASSED, TRUE) ;    -- Enable PASSED logs
    --UartScoreboard <= NewID("UART_SB1") ;

    wait for 0 ns ;
    SetAlertLogOptions(WriteTimeLast => FALSE) ;
    SetAlertLogOptions(TimeJustifyAmount => 16) ;
    SetAlertLogJustify ;

    -- Wait for testbench initialization
    wait for 0 ns ;  wait for 0 ns ;
    TranscriptOpen(OSVVM_RESULTS_DIR & "TbUart_SendGet1.txt") ;
    SetTranscriptMirror(TRUE) ;

    -- Wait for Design Reset
    wait until nReset = '1' ;
    ClearAlerts ;

    -- Wait for test to finish
    WaitForBarrier(TestDone, 10 ms) ;
    AlertIf(now >= 10 ms, "Test finished due to timeout") ;
    --AlertIf(GetAffirmCount < 1, "Test is not Self-Checking");

    TranscriptClose ;
--    AlertIfDiff("./results/TbUart_SendGet1.txt", "../Uart/testbench/validated_results/TbUart_SendGet1.txt", "") ;

    EndOfTestReports(ReportAll => FALSE) ;
    std.env.stop ;
    wait ;
  end process ControlProc ;

  ------------------------------------------------------------
  -- UartTbTxRxProc
  --   Sends and receives UART transactions
  ------------------------------------------------------------
  UartTbTxRxProc : process
    variable UartTxID : AlertLogIDType ;
    variable TransactionCount, ErrorCount : integer ;

    variable Done     : integer := 0;
    variable Error    : integer := 0;
    variable NodeNum  : integer := 0;
  begin

    CoSimInit(NodeNum);

    GetAlertLogID(UartTxRec, UartTxID) ;
    SetLogEnable(UartTxID, INFO, TRUE) ;
    WaitForClock(UartTxRec, 2) ;

    OperationLoop: loop
      CoSimStream (UartTxRec, UartRxRec, Done, Error, NodeNum);

      AlertIf(Error /= 0, "CoSimStream flagged an error") ;

      -- Finish when Done non-zero
      exit when Done /= 0;

    end loop OperationLoop ;

    TestActive <= FALSE ;  -- last one

    ------------------------------------------------------------
    -- End of test.  Wait for outputs to propagate and signal TestDone
    wait for 4 * UART_BAUD_PERIOD_115200 ;
    WaitForBarrier(TestDone) ;
    wait ;
  end process UartTbTxRxProc ;

end SendGet1 ;

Configuration TbUart_SendGet1 of TbUart is
  for TestHarness
    for TestCtrl_1 : TestCtrl
      use entity work.TestCtrl(SendGet1) ;
    end for ;
  end for ;
end TbUart_SendGet1 ;