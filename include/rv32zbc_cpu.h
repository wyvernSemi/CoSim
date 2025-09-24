// =========================================================================
//
//  File Name:         rv32zbc_cpu.h
//  Design Unit Name:
//  Revision:          OSVVM MODELS STANDARD VERSION
//
//  Maintainer:        Simon Southwell email:  simon.southwell@gmail.com
//  Contributor(s):
//    Simon Southwell      simon.southwell@gmail.com
//
//  Description:
//    Contains the definitions for the rv32zbc_cpu derived class
//
//    This file is part of the Zbc extended RISC-V instruction
//    set simulator (rv32zbc_cpu).
//
//  Revision History:
//    Date      Version    Description
//    09/2025   ????       Update model to v1.2.9
//
//  This file is part of OSVVM.
//
//  Copyright (c) 2025 Simon Southwell. 
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
// =========================================================================

#ifndef _RV32ZBC_CPU_H_
#define _RV32ZBC_CPU_H_

#include "rv32_extensions.h"
#include "rv32i_cpu_hdr.h"
#include "rv32csr_cpu.h"
#include RV32ZBC_INCLUDE

class rv32zbc_cpu : public RV32_ZBC_INHERITANCE_CLASS
{
public:

    // ------------------------------------------------
    // Constructors/destructors
    // ------------------------------------------------

             LIBRISCV32_API      rv32zbc_cpu      (FILE* dbgfp = stdout);
    virtual  LIBRISCV32_API      ~rv32zbc_cpu()   { };


private:

    // ------------------------------------------------
    // Private member variables
    // ------------------------------------------------

    // Constant strings for instructions

    const char clmul_str   [DISASSEM_STR_SIZE] = "clmul    ";
    const char clmulh_str  [DISASSEM_STR_SIZE] = "clmulh   ";
    const char clmulr_str  [DISASSEM_STR_SIZE] = "clmulr   ";

    // ------------------------------------------------
    // Private methods
    // ------------------------------------------------

    // Instruction execution method prototypes

    void clmul                      (const p_rv32i_decode_t d);
    void clmulh                     (const p_rv32i_decode_t d);
    void clmulr                     (const p_rv32i_decode_t d);

};

#endif
