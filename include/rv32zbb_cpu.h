// =========================================================================
//
//  File Name:         rv32zbb_cpu.h
//  Design Unit Name:
//  Revision:          OSVVM MODELS STANDARD VERSION
//
//  Maintainer:        Simon Southwell email:  simon.southwell@gmail.com
//  Contributor(s):
//    Simon Southwell      simon.southwell@gmail.com
//
//  Description:
//    Contains the definitions for the rv32zbb_cpu derived class
//
//    This file is part of the Zbb extended RISC-V instruction
//    set simulator (rv32zbb_cpu).
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

#ifndef _RV32ZBB_CPU_H_
#define _RV32ZBB_CPU_H_

#include "rv32_extensions.h"
#include "rv32i_cpu_hdr.h"
#include "rv32csr_cpu.h"
#include RV32ZBB_INCLUDE

class rv32zbb_cpu : public RV32_ZBB_INHERITANCE_CLASS
{
public:

    // ------------------------------------------------
    // Constructors/destructors
    // ------------------------------------------------

    LIBRISCV32_API      rv32zbb_cpu      (FILE* dbgfp = stdout);
    virtual  LIBRISCV32_API      ~rv32zbb_cpu()   { };

protected:

    // Quarternary table for clz, ctz and cpop
    rv32i_decode_table_t  cxx_tbl [RV32I_NUM_QUARTERNARY_OPCODES];

private:

    // ------------------------------------------------
    // Private member variables
    // ------------------------------------------------

    // Constant strings for instructions
    const char andn_str     [DISASSEM_STR_SIZE] = "andn     ";
    const char orn_str      [DISASSEM_STR_SIZE] = "orn      ";
    const char xnor_str     [DISASSEM_STR_SIZE] = "xnor     ";
    const char clz_str      [DISASSEM_STR_SIZE] = "clz      ";
    const char ctz_str      [DISASSEM_STR_SIZE] = "ctz      ";
    const char cpop_str     [DISASSEM_STR_SIZE] = "cpop     ";
    const char max_str      [DISASSEM_STR_SIZE] = "max      ";
    const char maxu_str     [DISASSEM_STR_SIZE] = "maxu     ";
    const char min_str      [DISASSEM_STR_SIZE] = "min      ";
    const char minu_str     [DISASSEM_STR_SIZE] = "minu     ";
    const char setxb_str    [DISASSEM_STR_SIZE] = "sext.b   ";
    const char setxh_str    [DISASSEM_STR_SIZE] = "sext.h   ";
    const char zexth_str    [DISASSEM_STR_SIZE] = "zext.h   ";
    const char rol_str      [DISASSEM_STR_SIZE] = "rol      ";
    const char ror_str      [DISASSEM_STR_SIZE] = "ror      ";
    const char rori_str     [DISASSEM_STR_SIZE] = "rori     ";
    const char orcb_str     [DISASSEM_STR_SIZE] = "orc.b    ";
    const char rev8_str     [DISASSEM_STR_SIZE] = "rev8     ";

    // ------------------------------------------------
    // Private methods
    // ------------------------------------------------

    // Instruction execution method prototypes
    void        andn                      (const p_rv32i_decode_t d);
    void        orn                       (const p_rv32i_decode_t d);
    void        xnor                      (const p_rv32i_decode_t d);
    void        clz                       (const p_rv32i_decode_t d);
    void        ctz                       (const p_rv32i_decode_t d);
    void        cpop                      (const p_rv32i_decode_t d);
    void        maxs                      (const p_rv32i_decode_t d);
    void        maxu                      (const p_rv32i_decode_t d);
    void        mins                      (const p_rv32i_decode_t d);
    void        minu                      (const p_rv32i_decode_t d);
    void        sextb                     (const p_rv32i_decode_t d);
    void        sexth                     (const p_rv32i_decode_t d);
    void        zexth                     (const p_rv32i_decode_t d);
    void        rol                       (const p_rv32i_decode_t d);
    void        ror                       (const p_rv32i_decode_t d);
    void        rori                      (const p_rv32i_decode_t d);
    void        orcb                      (const p_rv32i_decode_t d);
    void        rev8                      (const p_rv32i_decode_t d);


    virtual void decode_exception (rv32i_decode_table_t*& p_entry, rv32i_decode_t& d)
    {

#ifndef RV32F_NOT_INCLUDE
        if (d.opcode == RV32I_OPCODE_OP_FP)
        {
            rv32f_cpu::decode_exception(p_entry, d);
        }
#endif
        if (d.opcode != RV32I_OPCODE_OP_FP)
        {
            // Have the possibility of a fourth level decode on rs2 index
            if (p_entry && p_entry->sub_table)
            {
                p_entry = &p_entry->ref.p_entry[d.rs2];
            }
            else
            {
                p_entry = NULL;
            }
        }
    }
};

#endif
