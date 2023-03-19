// =========================================================================
//
//  File Name:         rv32csr_cpu.h
//  Design Unit Name:
//  Revision:          OSVVM MODELS STANDARD VERSION
//
//  Maintainer:        Simon Southwell email:  simon.southwell@gmail.com
//  Contributor(s):
//    Simon Southwell      simon.southwell@gmail.com
//
//  Description:
//    Contains the definitions for the rv32csr_cpu derived class
//
//    This file is part of the Zicsr extended RISC-V instruction
//    set simulator (rv32csr_cpu).
//
//  Revision History:
//    Date      Version    Description
//    01/2023   2023.01    Released with OSVVM CoSim
//    12th July 2021       Earlier version
//
//  This file is part of OSVVM.
//
//  Copyright (c) 2021 Simon Southwell. 
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

#ifndef _RV32CSR_CPU_H_
#define _RV32CSR_CPU_H_

#include "rv32_extensions.h"
#include "rv32csr_cpu_hdr.h"
#include RV32CSR_INCLUDE

class rv32csr_cpu : public RV32_ZICSR_INHERITANCE_CLASS
{
public:
             LIBRISCV32_API      rv32csr_cpu      (FILE* dbgfp = stdout);
    virtual  LIBRISCV32_API      ~rv32csr_cpu()   { };

    LIBRISCV32_API void          register_int_callback          (p_rv32i_intcallback_t callback_func) { p_int_callback = callback_func; };

private:
    // ------------------------------------------------
    // Private member variables
    // ------------------------------------------------

    // Pointer to interrupt callback function
    p_rv32i_intcallback_t p_int_callback;

    rv32i_time_t          interrupt_wakeup_time;

    // Strings for instruiction disassembly
    static const char mret_str    [DISASSEM_STR_SIZE] ;
    static const char csrrw_str   [DISASSEM_STR_SIZE] ;
    static const char csrrs_str   [DISASSEM_STR_SIZE] ;
    static const char csrrc_str   [DISASSEM_STR_SIZE] ;
    static const char csrrwi_str  [DISASSEM_STR_SIZE] ;
    static const char csrrsi_str  [DISASSEM_STR_SIZE] ;
    static const char csrrci_str  [DISASSEM_STR_SIZE] ;

    // ------------------------------------------------
    // Private member functions
    // ------------------------------------------------

    // Overloaded reset function
    void reset                           (void);

    // Overload processing of traps
    void process_trap(int trap_type);

    // Process interrupts
    int  process_interrupts();

    // Return from trap instruction
    void mret                            (const p_rv32i_decode_t);

    // Zicsr instructions
    void csrrw                           (const p_rv32i_decode_t);
    void csrrs                           (const p_rv32i_decode_t);
    void csrrc                           (const p_rv32i_decode_t);
    void csrrwi                          (const p_rv32i_decode_t);
    void csrrsi                          (const p_rv32i_decode_t);
    void csrrci                          (const p_rv32i_decode_t);

protected:
    // CSR access method
    virtual uint32_t access_csr          (const unsigned funct3, const uint32_t addr, const uint32_t rd, const uint32_t value);

    // Return write mask (bit set equals writable) for given CSR, with unimplemented status flag
    virtual uint32_t csr_wr_mask         (const uint32_t addr, bool& unimp);

};

#endif
