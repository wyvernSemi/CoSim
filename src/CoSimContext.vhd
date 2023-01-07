--
--  File Name:         CoSimContext.vhd
--  Design Unit Name:  CoSimContext
--  Revision:          OSVVM MODELS STANDARD VERSION
--
--  Maintainer:        Simon Southwell email:  simon.southwell@gmail.com
--  Contributor(s):
--     Simon Southwell      simon.southwell@gmail.com
--     Jim Lewis            jim@synthworks.com
--
--  Description:
--      Defines virtual co-simulation procedures
--
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

context CoSimContext is
  library osvvm_cosim ;
  use osvvm_cosim.OsvvmVprocPkg.all ;
  use osvvm_cosim.OsvvmTestCoSimPkg.all;

end context CoSimContext ;