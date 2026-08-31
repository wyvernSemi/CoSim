//
//  File Name:           rv32_cosim_utils.cpp
//  Design Unit Name:
//  Revision:            OSVVM MODELS STANDARD VERSION
//
//  Maintainer:          Simon Southwell  email: simon.southwell@gmail.com
//  Contributor(s):
//     Simon Southwell  simon.southwell@gmail.com
//
//
//  Description:
//      Co-simulation program top levels for nodes 0, 1 and 2
//      with node 0 running rv32 ISS
//
//
//  Developed by:
//        SynthWorks Design Inc.
//        VHDL Training Classes
//        http://www.SynthWorks.com
//
//  Revision History:
//    Date      Version    Description
//    07/2026   2026.08    Initial revision
//
//
//  This file is part of OSVVM.
//
//  Copyright (c) 2026 by [OSVVM Authors](../../../AUTHORS.md)
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//

#include <unistd.h>
#include <cstdio>
#include <cstdint>
#include <string>

#include "rv32.h"

// -------------------------------------------------------------------------
// Class definition
// -------------------------------------------------------------------------
class rv32_cosim_utils
{
private:
    // Internal constants
    static constexpr int       STRBUFSIZE      = 256;
    static constexpr int       MAXARGS         = 25;
    static constexpr uint32_t  SWIRQADDR       = 0xaffffff8;

    const std::string          GETOPT_ARG_STR  = "hHgdbeEraCTiBxct:n:D:A:p:S:s:i:m:M:u:L:";
    const std::string          CFGROOTNAME     = "vusermain";
    const std::string          CFGFILENAME     = CFGROOTNAME + ".cfg";

public:

    // Exported methods
             rv32_cosim_utils  () : sw_irq_addr(SWIRQADDR) {};

    int      parse_args        (rv32i_cfg_s &cfg, const int node);
    void     reg_dump          (rv32* pCpu, FILE* dfp, bool abi_en);
    void     csr_dump          (rv32* pCpu, FILE* dfp);
    void     mem_dump          (uint32_t num, uint32_t start, rv32* pCpu, FILE* dfp);
    bool     check_exit_status (rv32* pCpu);

    uint32_t get_sw_irq_addr   () {return sw_irq_addr;}

private:

    // Internal state
    char     execstr[STRBUFSIZE];
    uint32_t sw_irq_addr;

};


