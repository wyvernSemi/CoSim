// ------------------------------------------------------------------------------
//
//  File Name:           VUserMain0.cpp
//  Design Unit Name:    Co-simulation virtual processor test program
//  Revision:            OSVVM MODELS STANDARD VERSION
//
//  Maintainer:          Simon Southwell      email:  simon.southwell@gmail.com
//  Contributor(s):
//     Simon Southwell   simon.southwell@gmail.com
//
//  Description:
//    Entry point for OSVVM co-simulation code for node 0
//    
//    This function creates a socket object which opens a TCP/IP server socket
//    and starts listening. When the process_pkts() method is called it will
//    process gdb remote serial interface commands for memory reads and writes
//    up to 64 bits, calling the co-sim API to instigate bus transactions on
//    OSVVM.
//
//  Developed by:
//        Simon Southwell
//
//  Revision History:
//    Date      Version    Description
//    09/2022   2023.01    Initial revision
//
//  This file is part of OSVVM.
//
//  Copyright (c) 2022 by Simon Southwell
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
// ------------------------------------------------------------------------------

#include <cstdio>
#include <cstdlib>
#include <cstdint>

#include "OsvvmCosim.h"
#include "rv32.h"
#include "rv32_cpu_gdb.h"

static const int node = 0;

// Type definition for write transaction, for use in TCP/IP socket script generation
typedef struct {
    uint32_t addr;
    uint32_t wdata;
    uint32_t size;
} wtrans_t;

// Local file pointer for optionally generating TCP/IP socket script file for each transaction
static FILE *sktfp = NULL;

static bool IntReq = false;

// -------------------------------------------------------------------------
// Dump registers using calls to rv32 object
// -------------------------------------------------------------------------

static void reg_dump(rv32* pCpu, FILE* dfp, bool abi_en)
{
    fprintf(dfp, "\nRegister state:\n\n  ");

    // Loop through all the registers
    for (int idx = 0; idx < RV32I_NUM_OF_REGISTERS; idx++)
    {
        // Get the appropriate mapped register name (ABI or x)
        const char* map_str = abi_en ? pCpu->rmap_str[idx] : pCpu->xmap_str[idx];

        // Get the length of the register name string
        size_t  slen = strlen(map_str);

        // Fetch the value of the register indexed
        uint32_t rval = pCpu->regi_val(idx);

        // Print out the register name (right justified) followed by the value
        fprintf(dfp, "%s%s = 0x%08x ", (slen == 2) ? "  " : (slen == 3) ? " ": "",
                                         map_str,
                                         rval);

        // After every fourth value, output a new line
        if ((idx % 4) == 3)
        {
            fprintf(dfp, "\n  ");
        }
    }

    // Add a final new line
    fprintf(dfp, "\n");
}

// -------------------------------------------------------------------------
// Dump memory
// -------------------------------------------------------------------------

static void mem_dump(uint32_t num, uint32_t start, rv32* pCpu, FILE* dfp)
{
    bool fault;

    fprintf(dfp, "\nMEM state:\n\n");
    for (uint32_t idx = start; idx < ((start & 0xfffffffc) + num*4); idx+=4)
    {
        uint32_t rval = pCpu->read_mem(idx, MEM_RD_ACCESS_WORD, fault);
        fprintf(dfp, "  0x%08x : 0x%08x\n", idx, rval);
    }
    fprintf(dfp, "\n");
}

// ------------------------------------------------------------------------------
// Generate a socket script based on transacion struct and rnw flag.
// ------------------------------------------------------------------------------

static void logGdbMsg(FILE *fp, wtrans_t &w, bool rnw)
{
    char msg[256];

    if (rnw)
    {
        sprintf(msg, "m%08x,%d\n", w.addr, w.size/8);
    }
    else
    {
        int byteSize = w.size/8;
        switch(w.size)
        {
        case 32: sprintf(msg, "M%08x,%d:%08x\n", w.addr, byteSize, w.wdata); break;
        case 16: sprintf(msg, "M%08x,%d:%04x\n", w.addr, byteSize, w.wdata & 0xffff); break;
        case  8: sprintf(msg, "M%08x,%d:%02x\n", w.addr, byteSize, w.wdata & 0xff); break;
        }
    }

    fprintf(fp, "%s", msg);
}

// -------------------------------------------------------------------------
// Checks register status for test code exit pass/fail
// -------------------------------------------------------------------------

static bool check_exit_status(rv32* pCpu)
{
    return pCpu->regi_val(10) || pCpu->regi_val(17) != 93;
}
    
// -------------------------------------------------------------------------
// Co-simulation software interrupt callback
// -------------------------------------------------------------------------

int cosim_int_callback(int int_vec)
{
    DebugVPrint("cosim_int_callback(): interrupt = %d\n", int_vec);
    
    IntReq = int_vec & 1;
    
    return 0;
}

// -------------------------------------------------------------------------
// ISS interrupt callback
// -------------------------------------------------------------------------

uint32_t riscv_interrupt_callback (const rv32i_time_t time, rv32i_time_t *wakeup_time)
{
    *wakeup_time = 0;
    return IntReq ? 1 : 0;
}

// -------------------------------------------------------------------------
// ISS memory access callback function
// -------------------------------------------------------------------------

static int memcosim (const uint32_t byte_addr, uint32_t &data, const int type, const rv32i_time_t time)
{
    int        cycle_count = 5;
    uint8_t    rdata8;
    uint16_t   rdata16;
    uint32_t   rdata32;
    wtrans_t   trans;
    OsvvmCosim cosim(node);

    // Select the co-simulation call based on the access type
    switch(type)
    {
        case MEM_WR_ACCESS_BYTE  : cosim.transWrite(byte_addr,  (uint8_t)data)           ; break;
        case MEM_WR_ACCESS_HWORD : cosim.transWrite(byte_addr, (uint16_t)data)           ; break;
        case MEM_WR_ACCESS_WORD  : cosim.transWrite(byte_addr, (uint32_t)data)           ; break;
        case MEM_WR_ACCESS_INSTR : cosim.transWrite(byte_addr, (uint32_t)data)           ; break;
        case MEM_RD_ACCESS_BYTE  : cosim.transRead(byte_addr,  &rdata8);  data = rdata8  ; break;
        case MEM_RD_ACCESS_HWORD : cosim.transRead(byte_addr,  &rdata16); data = rdata16 ; break;
        case MEM_RD_ACCESS_WORD  : cosim.transRead(byte_addr,  &rdata32); data = rdata32 ; break;
        case MEM_RD_ACCESS_INSTR : cosim.transRead(byte_addr,  &rdata32); data = rdata32 ; break;
        default: cycle_count = RV32I_EXT_MEM_NOT_PROCESSED; break;
    }

    // If the socket script file point is active, save off a TCP/IP transaction packet command
    if (sktfp != NULL)
    {
        bool rnw = (type >= MEM_RD_ACCESS_BYTE);
        
        trans.addr = byte_addr;
        trans.wdata = data;
        trans.size  = (type == MEM_WR_ACCESS_BYTE  || type == MEM_RD_ACCESS_BYTE)  ?  8 :
                      (type == MEM_WR_ACCESS_HWORD || type == MEM_RD_ACCESS_HWORD) ? 16 :
                                                                                     32 ;

        logGdbMsg(sktfp, trans, rnw);
    }

    return cycle_count;
}

// =========================================================================
// Main entry point for co-simulation node 0
// =========================================================================

extern "C" void VUserMain0()
{
    bool        error = false;
    std::string test_name("TbAb_InterruptCoSim3");
    OsvvmCosim  cosim(node, test_name);
    
    // Create a configuration object
    rv32i_cfg_s cfg;

    // Override the defaults with test specifics

    // Configure halt conditions
    cfg.hlt_on_ecall         = true;

    // Configure program to run
    cfg.user_fname           = true;
    cfg.exec_fname           = "test.exe";

    // Configure debug output
    cfg.rt_dis               = true;
    cfg.abi_en               = true;
    cfg.dump_regs            = true;

    // Configure GDB debugging mode
    cfg.gdb_mode             = false;
    cfg.gdb_ip_portnum       = 0xc000;
    
    // Open up a socket script file
    sktfp = fopen("sktscript.txt", "w");

    // Create a new cpu object
    rv32* pCpu               = new rv32();

    // Register a memory access callback with the CPU model
    pCpu->register_ext_mem_callback(memcosim);
    
    // Register an interrupt callback with the CPU model
    pCpu->register_int_callback(riscv_interrupt_callback);
    
    // Register an interrupt callback with the cosim software
    cosim.regInterruptCB(cosim_int_callback);

    // If GDB mode not configured, simply run the specified program
    if (!cfg.gdb_mode)
    {
        // Load an executable
        if (!pCpu->read_elf(cfg.exec_fname))
        {
            // Run processor until halted
            pCpu->run(cfg);

            // Check exit status
            if (error = check_exit_status(pCpu))
            {
                VPrint("*FAIL*: exit code = 0x%08x finish code = 0x%08x running %s\n",
                        pCpu->regi_val(10) >> 1, pCpu->regi_val(17), cfg.exec_fname);
            }
            else
            {
                VPrint("PASS: exit code = 0x%08x running %s\n", 
                        pCpu->regi_val(10), cfg.exec_fname);
            }

            // If enabled, dump the registers
            if (cfg.dump_regs)
            {
                reg_dump(pCpu, stdout, cfg.abi_en);
            }

            // If specified dump the numbr of DMEM words
            if (cfg.num_mem_dump_words)
            {
                mem_dump(cfg.num_mem_dump_words,
                         cfg.mem_dump_start,
                         pCpu,
                         stdout);
            }

        }
        else
        {
            VPrint("***ERROR in loading executable file\n");
            error = true;
        }

    }
    // If GDB mode configured, run with that
    else
    {
#ifdef __WIN32__
        WORD versionWanted   = MAKEWORD(1, 1);
        WSADATA wsaData;
        WSAStartup(versionWanted, &wsaData);
#endif

        // Load an executable if specified in the configuration
        if (cfg.user_fname)
        {
            if (pCpu->read_elf(cfg.exec_fname))
            {
                VPrint("***ERROR in loading executable file\n");
                error = true;
            }
        }

        if (!error)
        {
            // Start procssing commands from GDB
            if (rv32gdb_process_gdb(pCpu, cfg.gdb_ip_portnum, cfg))
            {
                VPrint("***ERROR in opening PTY\n");
                error = true;
            }
        }

#ifdef __WIN32__
        WSACleanup;
#endif
    }

    // Clean up
    if (cfg.dbg_fp != stdout)
    {
        fclose(cfg.dbg_fp);
    }
    delete pCpu;
    
    // Flag to the simulation we're finished, after 10 more iterations
    cosim.tick(10, true, error);

    SLEEPFOREVER;
}

