--
--  File Name:         OssvmTestCoSimPkg.vhd
--  Design Unit Name:  OssvmTestCoSimPkg
--  Revision:          OSVVM MODELS STANDARD VERSION
--
--  Maintainer:        Simon Southwell  email: simon.southwell@gmail.com
--  Contributor(s):
--     Jim Lewis            jim@synthworks.com
--     Simon Southwell      simon.southwell@gmail.com
--
--
--  Description:
--      Defines procedures to support co-simulation
--
--  Revision History:
--    Date      Version    Description
--    09/2022   2023.01    Initial revision
--
--
--  This file is part of OSVVM.
--
--  Copyright (c) 2022 by [OSVVM Authors](../AUTHORS.md)
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

library OSVVM ;
  context OSVVM.OsvvmContext ;
  use osvvm.ScoreboardPkg_slv.all ;

library OSVVM_Common ;
  use OSVVM_Common.AddressBusTransactionPkg.all ;

library osvvm_cosim ;
  use osvvm_cosim.OsvvmVprocPkg.all ;

package OsvvmTestCoSimPkg is

  type CoSimOperationType is (SET_TEST_NAME) ; 
  ------------------------------------------------------------
  ------------------------------------------------------------

procedure CoSimInit (
  variable NodeNum          : in     integer := 0
  ) ;

procedure CoSimTrans (
  signal   ManagerRec       : inout  AddressBusRecType ;
  variable Done             : inout  integer ;
  variable Error            : inout  integer ;
  variable IntReq           : in     integer := 0 ;
  variable NodeNum          : in     integer := 0
  ) ;

  ------------------------------------------------------------
  -- Co-simulation procedure to dispatch one transactions
  ------------------------------------------------------------
  procedure CoSimDispatchOneTransaction (
    -- Transaction  interface
    signal   ManagerRec      : inout  AddressBusRecType ;
    constant VpOperation     : in     integer ;
    constant VPAddr          : in     integer ;
    constant VPAddrHi        : in     integer ;
    constant VPAddrWidth     : in     integer ;
    constant VPDataOut       : in     integer ;
    constant VPDataOutHi     : in     integer ;
    constant VPDataWidth     : in     integer ;
    constant VPBurstSize     : in     integer ;
    constant VPTicks         : in     integer ;
    constant NodeNum         : in     integer
  ) ;

end package OsvvmTestCoSimPkg ;

-- /////////////////////////////////////////////////////////////////////////////////////////
-- /////////////////////////////////////////////////////////////////////////////////////////

package body OsvvmTestCoSimPkg is
  constant ADDR_WIDTH_MAX  : integer := 64 ;
  constant DATA_WIDTH_MAX  : integer := 64 ;

  ------------------------------------------------------------
  -- Co-simulation software initialisation procedure for
  -- a specified node. Must be called, once per node, before
  -- any CoSimTrans calls
  ------------------------------------------------------------

  procedure CoSimInit (
  variable NodeNum         : in     integer := 0
  ) is

  begin
    VInit(NodeNum);
  end procedure CoSimInit ;

  ------------------------------------------------------------
  -- Co-simulation wrapper procedure to send read and write
  -- transactions
  --
  -- Note: The ticks parameter is to allow the internally set
  -- state to persist between calls and must be connected to
  -- an integer variable in the process where CoSimTrans is
  -- called. It should not be changed in the external process.
  -- It is used to allow time to advance without instigating
  -- a transaction.
  --
  ------------------------------------------------------------
  procedure CoSimTrans (
    -- Transaction  interface
    signal   ManagerRec      : inout  AddressBusRecType ;
    variable Done            : inout  integer ;
    variable Error           : inout  integer ;
    variable IntReq          : in     integer := 0;
    variable NodeNum         : in     integer := 0
    ) is

    variable RdData          : std_logic_vector (ManagerRec.DataFromModel'range) ;

    variable VPDataIn        : integer ;
    variable VPDataInHi      : integer ;
    variable VPDataOut       : integer ;
    variable VPDataOutHi     : integer ;
    variable VPDataWidth     : integer ;
    variable VPAddr          : integer ;
    variable VPAddrHi        : integer ;
    variable VPAddrWidth     : integer ;
    variable VPOp            : integer ;
    variable VPBurstSize     : integer ;
    variable VPTicks         : integer ;
    variable VPDone          : integer ;
    variable VPError         : integer ;
    variable VPOperation     : integer ;

  begin

    -- RdData won't have persisted from last call, so re-fetch from ManagerRec
    -- which will have persisted (and is not yet updated)
    RdData       := osvvm.TbUtilPkg.MetaTo01(SafeResize(ManagerRec.DataFromModel, RdData'length)) ;

    -- Sample the read data from last access, saved in RdData inout port
    if ManagerRec.DataWidth > 32 then
      VPDataIn   := to_integer(signed(RdData(31 downto  0))) ;
      VPDataInHi := to_integer(signed(RdData(RdData'length-1 downto 32))) ;
    else
      VPDataIn   := to_integer(signed(RdData(31 downto 0))) ;
      VPDataInHi := 0 ;
    end if;

    -- Call VTrans to generate a new access
    VTrans(NodeNum,   IntReq,
           VPDataIn,  VPDataInHi,
           VPDataOut, VPDataOutHi, VPDataWidth,
           VPAddr,    VPAddrHi,    VPAddrWidth,
           VPOp,      VPBurstSize, VPTicks,
           VPDone,    VPError) ;

--    Done  := VPDone  when Done  = 0;  -- Sticky
--    Error := VPError when Error = 0;  -- Sticky
    Done  := VPDone  ;
    Error := VPError ;

    CoSimDispatchOneTransaction(ManagerRec,
                                VPOp,
                                VPAddr,      VPAddrHi,    VPAddrWidth,
                                VPDataOut,   VPDataOutHi, VPDataWidth,
                                VPBurstSize, VPTicks,     NodeNum) ;

  end procedure CoSimTrans ;

  ------------------------------------------------------------
  -- Co-simulation procedure to dispatch one transactions
  ------------------------------------------------------------
  procedure CoSimDispatchOneTransaction (
    -- Transaction  interface
    signal   ManagerRec      : inout  AddressBusRecType ;
    constant VpOperation     : in     integer ;
    constant VPAddr          : in     integer ;
    constant VPAddrHi        : in     integer ;
    constant VPAddrWidth     : in     integer ;
    constant VPDataOut       : in     integer ;
    constant VPDataOutHi     : in     integer ;
    constant VPDataWidth     : in     integer ;
    constant VPBurstSize     : in     integer ;
    constant VPTicks         : in     integer ;
    constant NodeNum         : in     integer
  ) is

    variable RdData          : std_logic_vector (DATA_WIDTH_MAX-1 downto 0) ;
    variable WrData          : std_logic_vector (DATA_WIDTH_MAX-1 downto 0) ;
    variable Address         : std_logic_vector (ADDR_WIDTH_MAX-1 downto 0) ;
    variable WrByteData      : signed (DATA_WIDTH_MAX-1 downto 0) ;
    variable RdDataInt       : integer ;
    variable WrDataInt       : integer ;
    variable TestName        : string(1 to VPBurstSize) ;

  begin

    -- Convert address and write data to std_logic_vectors
    Address(31 downto  0) := std_logic_vector(to_signed(VPAddr,      32)) ;
    Address(63 downto 32) := std_logic_vector(to_signed(VPAddrHi,    32)) ;

    WrData(31 downto 0 )  := std_logic_vector(to_signed(VPDataOut,   32)) ;
    WrData(63 downto 32)  := std_logic_vector(to_signed(VPDataOutHi, 32)) ;

--    if IsAddressBusMitValue(VpOperation) then
    if VpOperation < 1024 then
      case AddressBusOperationType'val(VpOperation) is
        when WAIT_FOR_CLOCK =>
          WaitForClock(ManagerRec, VPTicks) ;

        when READ_OP =>
          Read  (ManagerRec, Address(VPAddrWidth-1 downto 0), RdData(VPDataWidth-1 downto 0)) ;

        when WRITE_OP =>
          Write (ManagerRec, Address(VPAddrWidth-1 downto 0), WrData(VPDataWidth-1 downto 0)) ;

        when READ_BURST =>
          ReadBurst(ManagerRec, Address(VPAddrWidth-1 downto  0), VPBurstSize) ;

          -- encapsulate the following:
          -- Pop the bytes from the read fifo and write them the the co-sim receive buffer
          RdData := (others => '0');
          for bidx in 0 to VPBurstSize-1 loop
            Pop(ManagerRec.ReadBurstFifo, RdData(7 downto 0)) ;
            RdDataInt := to_integer(unsigned(RdData(7 downto 0))) ;
            VSetBurstRdByte(NodeNum, bidx, RdDataInt) ;
          end loop;

        when WRITE_BURST =>
          -- encapsulate the following:
          -- Fetch the bytes from the co-sim send buffer and push to the transaction write fifo
          for bidx in 0 to VPBurstSize-1 loop
            VGetBurstWrByte(NodeNum, bidx, WrDataInt) ;
            WrByteData := to_signed(WrDataInt, WrByteData'length);
            Push(ManagerRec.WriteBurstFifo, std_logic_vector(WrByteData(7 downto 0))) ;
          end loop ;

          WriteBurst(ManagerRec, Address(VPAddrWidth-1 downto  0), VPBurstSize) ;

        when others =>
          Alert("CoSim/src/OsvvmTestCoSimPkg: CoSimDispatchOneTransaction received unimplemented transaction") ;

      end case ;

      -- If VPTicks non-zero for transaction operations do wait for clock after the transaction
      -- executed
      if AddressBusOperationType'val(VpOperation) /= WAIT_FOR_CLOCK and VPTicks /= 0 then
        WaitForClock(ManagerRec, VPTicks) ;
      end if ;
    else
      null ;
--
--!! Note:  Below is a Conceptual model for adding non-transaction calls through the interface
      case CoSimOperationType'val(VpOperation - 1024) is
        when SET_TEST_NAME =>
          for bidx in 0 to VPBurstSize-1 loop
            VGetBurstWrByte(NodeNum, bidx, WrDataInt) ;
            if (WrDataInt < 0 or WrDataInt > 255) then 
              Alert("CoSim/src/OsvvmTestCoSimPkg: CoSimDispatchOneTransaction SetTestName - bad character value") ;
              return ; 
            end if ; 
            TestName(bidx+1) := character'val(WrDataInt); 
          end loop ;
          SetTestName(TestName(1 to VPBurstSize)) ;
        when others =>  
          Alert("CoSim/src/OsvvmTestCoSimPkg: CoSimDispatchOneTransaction received unimplemented transaction") ;
      end case ;

    end if ;

  end procedure CoSimDispatchOneTransaction ;

end package body OsvvmTestCoSimPkg ;
