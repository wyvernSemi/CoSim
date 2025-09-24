// =========================================================================
//
//  File Name:         rv32zbs_cpu.h
//  Design Unit Name:
//  Revision:          OSVVM MODELS STANDARD VERSION
//
//  Maintainer:        Simon Southwell email:  simon.southwell@gmail.com
//  Contributor(s):
//    Simon Southwell      simon.southwell@gmail.com
//
//  Description:
//    Contains the definitions for the rv32zbs_cpu derived class
//
//    This file is part of the Zbs extended RISC-V instruction
//    set simulator (rv32zbs_cpu).
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

#ifndef _RV32ZBS_CPU_H_
#define _RV32ZBS_CPU_H_

#include "rv32_extensions.h"
#include "rv32i_cpu_hdr.h"
#include "rv32csr_cpu.h"
#include RV32ZBS_INCLUDE

class rv32zbs_cpu : public RV32_ZBS_INHERITANCE_CLASS
{
public:

    // ------------------------------------------------
    // Constructors/destructors
    // ------------------------------------------------

             LIBRISCV32_API      rv32zbs_cpu      (FILE* dbgfp = stdout);
    virtual  LIBRISCV32_API      ~rv32zbs_cpu()   { };

protected:
    rv32i_decode_table_t  cxx_tbl        [RV32I_NUM_TERTIARY_OPCODES];
private:

    // ------------------------------------------------
    // Private member variables
    // ------------------------------------------------

    // Constant strings for instructions

    const char bclr_str   [DISASSEM_STR_SIZE] = "bclr     ";
    const char bclri_str  [DISASSEM_STR_SIZE] = "bclri    ";
    const char bext_str   [DISASSEM_STR_SIZE] = "bext     ";
    const char bexti_str  [DISASSEM_STR_SIZE] = "bexti    ";
    const char binv_str   [DISASSEM_STR_SIZE] = "binv     ";
    const char binvi_str  [DISASSEM_STR_SIZE] = "binvi    ";
    const char bset_str   [DISASSEM_STR_SIZE] = "bset     ";
    const char bseti_str  [DISASSEM_STR_SIZE] = "bseti    ";

    // ------------------------------------------------
    // Private methods
    // ------------------------------------------------

    // Instruction execution method prototypes

    void        bclr                       (const p_rv32i_decode_t d);
    void        bclri                      (const p_rv32i_decode_t d);
    void        bext                       (const p_rv32i_decode_t d);
    void        bexti                      (const p_rv32i_decode_t d);
    void        binv                       (const p_rv32i_decode_t d);
    void        binvi                      (const p_rv32i_decode_t d);
    void        bset                       (const p_rv32i_decode_t d);
    void        bseti                      (const p_rv32i_decode_t d);

};

#endif
