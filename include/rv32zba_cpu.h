// =========================================================================
//
//  File Name:         rv32zba_cpu.h
//  Design Unit Name:
//  Revision:          OSVVM MODELS STANDARD VERSION
//
//  Maintainer:        Simon Southwell email:  simon.southwell@gmail.com
//  Contributor(s):
//    Simon Southwell      simon.southwell@gmail.com
//
//  Description:
//    Contains the definitions for the rv32zba_cpu derived class
//
//    This file is part of the Zba extended RISC-V instruction
//    set simulator (rv32zba_cpu).
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

#ifndef _RV32ZBA_CPU_H_
#define _RV32ZBA_CPU_H_

#include "rv32_extensions.h"
#include "rv32i_cpu_hdr.h"
#include "rv32csr_cpu.h"
#include RV32ZBA_INCLUDE

class rv32zba_cpu : public RV32_ZBA_INHERITANCE_CLASS
{
public:

    // ------------------------------------------------
    // Constructors/destructors
    // ------------------------------------------------

             LIBRISCV32_API      rv32zba_cpu      (FILE* dbgfp = stdout);
    virtual  LIBRISCV32_API      ~rv32zba_cpu()   { };


private:

    // ------------------------------------------------
    // Private member variables
    // ------------------------------------------------

    // Constant strings for instructions

    const char sh1add_str   [DISASSEM_STR_SIZE] = "sh1add   ";
    const char sh2add_str   [DISASSEM_STR_SIZE] = "sh2add   ";
    const char sh3add_str   [DISASSEM_STR_SIZE] = "sh3add   ";

    // ------------------------------------------------
    // Private methods
    // ------------------------------------------------

    // Instruction execution method prototypes

    void        shxadd                     (const p_rv32i_decode_t d, const uint32_t shamt);
    inline void sh1add                     (const p_rv32i_decode_t d) {shxadd(d, 1);};
    inline void sh2add                     (const p_rv32i_decode_t d) {shxadd(d, 2);};
    inline void sh3add                     (const p_rv32i_decode_t d) {shxadd(d, 3);};

};

#endif
