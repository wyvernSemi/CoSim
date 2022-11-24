--
--  File Name:         OsvvmVprocPkg.vhd
--  Design Unit Name:  OsvvmVprocPkg
--  Revision:          OSVVM MODELS STANDARD VERSION
--
--  Maintainer:        Simon Southwell email:  simon.southwell@gmail.com
--  Contributor(s):
--     Simon Southwell      simon.southwell@gmail.com
--
--
--  Description:
--      Defines virtual co-simulation procedures
--
--
--  Developed by:
--        SynthWorks Design Inc.
--        VHDL Training Classes
--        http://www.SynthWorks.com
--
--  Revision History:
--    Date      Version    Description
--    09/2022   2022       Initial revision
--
--
--  This file is part of OSVVM.
--
--  Copyright (c) 2022 by SynthWorks Design Inc.
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

package OsvvmVprocPkg is

  procedure VInit (
    node : in integer
  ) ;
  attribute foreign of VInit : procedure is "VHPIDIRECT ./VProc.so VInit" ;

  procedure VSched (
    node      : in  integer ;
    Interrupt : in  integer ;
    VPDataIn  : in  integer ;
    VPDataOut : out integer ;
    VPAddr    : out integer ;
    VPRw      : out integer ;
    VPTicks   : out integer
  ) ;
  attribute foreign of VSched : procedure is "VHPIDIRECT ./VProc.so VSched" ;

  procedure VTrans (
    node        : in  integer ;
    Interrupt   : in  integer ;
    VPDataIn    : in  integer ;
    VPDataInHi  : in  integer ;
    VPDataOut   : out integer ;
    VPDataOutHi : out integer ;
    VPDataWidth : out integer ;
    VPAddr      : out integer ;
    VPAddrHi    : out integer ;
    VPAddrWidth : out integer ;
    VPRw        : out integer ;
    VPBurstSize : out integer ;
    VPTicks     : out integer ;
    VPDone      : out integer ;
    VPError     : out integer
  ) ;
  attribute foreign of VTrans : procedure is "VHPIDIRECT ./VProc.so VTrans" ;

  procedure VProcUser (
    node      : in  integer ;
    value     : in  integer
  ) ;
  attribute foreign of VProcUser : procedure is "VHPIDIRECT ./VProc.so VProcUser" ;

  procedure VGetBurstWrByte (
    node      : in  integer ;
    idx       : in  integer ;
    data      : out integer
  ) ;
  attribute foreign of VGetBurstWrByte : procedure is "VHPIDIRECT ./VProc.so VGetBurstWrByte" ;

  procedure VSetBurstRdByte (
    node      : in  integer ;
    idx       : in  integer ;
    data      : in  integer
  ) ;
  attribute foreign of VSetBurstRdByte : procedure is "VHPIDIRECT ./VProc.so VSetBurstRdByte" ;

end ;

package body OsvvmVprocPkg is

  procedure VInit (
    node      : in integer
  ) is
  begin
    report "ERROR: foreign subprogram out_params not called" ;
  end ;

  procedure VSched (
    node      : in  integer ;
    Interrupt : in  integer ;
    VPDataIn  : in  integer ;
    VPDataOut : out integer ;
    VPAddr    : out integer ;
    VPRw      : out integer ;
    VPTicks   : out integer
  ) is
  begin
    report "ERROR: foreign subprogram out_params not called";
  end ;

  procedure VTrans (
    node        : in  integer ;
    Interrupt   : in  integer ;
    VPDataIn    : in  integer ;
    VPDataInHi  : in  integer ;
    VPDataOut   : out integer ;
    VPDataOutHi : out integer ;
    VPDataWidth : out integer ;
    VPAddr      : out integer ;
    VPAddrHi    : out integer ;
    VPAddrWidth : out integer ;
    VPRw        : out integer ;
    VPBurstSize : out integer ;
    VPTicks     : out integer ;
    VPDone      : out integer ;
    VPError     : out integer
  ) is
  begin
    report "ERROR: foreign subprogram out_params not called" ;
  end ;

  procedure VProcUser (
    node      : in  integer ;
    value     : in  integer
  ) is
  begin
    report "ERROR: foreign subprogram out_params not called" ;
  end ;

  procedure VGetBurstWrByte (
    node      : in  integer ;
    idx       : in  integer ;
    data      : out integer
  ) is
  begin
    report "ERROR: foreign subprogram out_params not called" ;
  end ;

  procedure VSetBurstRdByte (
    node      : in  integer ;
    idx       : in  integer ;
    data      : in  integer
  ) is
  begin
    report "ERROR: foreign subprogram out_params not called" ;
  end ;

end;