//
//  File Name:           VUserMain.cpp
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
//      with node 0 running the rv32 ISS
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

#include <cstdint>
#include <cstdio>
#include <queue>

// OSVVM co-simulation headers for both address bus and streaming MITs
#include "OsvvmCosim.h"
#include "OsvvmCosimStream.h"

// RV32 ISS headers
#include "rv32.h"
#include "rv32_cpu_gdb.h"

// RV32 co-simulation utilities header
#include "rv32_cosim_utils.h"

// SLZW compression/decompression header
extern "C" {
#include "slzw.h"
}

// -------------------------------------------------------------------------
// CONSTANTS
// -------------------------------------------------------------------------

// Define the node running rv32 for use by memory callback
static const int           RV32NODE                = 0;

// Default data buffer size
static const int           BUFSIZE                 = 2048;

// Define base addresses
static const uint32_t      ROM_BASE_ADDR           = 0x00000000;
static const uint32_t      RAM_BASE_ADDR           = 0x00800000;
static const uint32_t      UART_BASE_ADDR          = 0x80000000;
static const uint32_t      MEM_TOP_ADDR            = 0xffffffff;

static const int           MSG_COMPRESSED_COUNT    = 338;

// UART VC status values
static const int           UARTTB_NO_ERROR         = 0;
static const int           UARTTB_PARITY_ERROR     = 1;
static const int           UARTTB_STOP_ERROR       = 2;
static const int           UARTTB_BREAK_ERROR      = 4;

// Default co-simulation memory block size
static const uint32_t      INTMEMSIZE              = 0x00200000;

// Address where co-simulated memory starts
static const uint32_t      SIM_MEM_BASE_ADDR        = ROM_BASE_ADDR;         // ROM + RAM in Axi4Memory VC
//static const uint32_t      SIM_MEM_BASE_ADDR        = RAM_BASE_ADDR;         // ROM in Axi4Memory VC, RAM in software
//static const uint32_t      SIM_MEM_BASE_ADDR        = MEM_TOP_ADDR;          // ROM + RAM in software

// -------------------------------------------------------------------------
// LOCAL STATIC STATE
// -------------------------------------------------------------------------

// Queue for compressed data to UART TX
static std::queue<uint8_t> uartq;

// Flag to indicated rv32 exited for use by other nodes
static bool                rv32_finished           = false;

// Compressed and decompressed data buffers
static char                cmpdata[BUFSIZE];
static char                dcmpdata[BUFSIZE];

// Internal ROM and RAM buffers
static uint8_t             rammem[INTMEMSIZE];
static uint8_t             rommem[INTMEMSIZE];

// -------------------------------------------------------------------------
// ISS memory access callback function
// -------------------------------------------------------------------------

static int memcb (const uint32_t byte_addr, uint32_t &data, const int type, const rv32i_time_t time)
{
    int        cycle_count = 5;
    uint8_t    rdata8;
    uint16_t   rdata16;
    uint32_t   rdata32;

    OsvvmCosim cosim(RV32NODE);

    // Accessing UART
    if (byte_addr == UART_BASE_ADDR)
    {
        // If a write access, push a byte onto the queue
        if (type == MEM_WR_ACCESS_BYTE || type == MEM_WR_ACCESS_HWORD || type == MEM_WR_ACCESS_WORD)
        {
            uartq.push((uint8_t)data);
        }
    }
    // Only process RAM accesses as co-simulated, leaving ROM to rv32 internal memory
    else if (byte_addr >= SIM_MEM_BASE_ADDR)
    {
        // Select the co-simulation call based on the access type
        switch(type)
        {
            case MEM_WR_ACCESS_BYTE  : cosim.transWrite(byte_addr,  (uint8_t)data)           ; break;
            case MEM_WR_ACCESS_HWORD : cosim.transWrite(byte_addr, (uint16_t)data)           ; break;
            case MEM_WR_ACCESS_WORD  :
            case MEM_WR_ACCESS_INSTR : cosim.transWrite(byte_addr, (uint32_t)data)           ; break;
            case MEM_RD_ACCESS_BYTE  : cosim.transRead(byte_addr,  &rdata8);  data = rdata8  ; break;
            case MEM_RD_ACCESS_HWORD : cosim.transRead(byte_addr,  &rdata16); data = rdata16 ; break;
            case MEM_RD_ACCESS_WORD  :
            case MEM_RD_ACCESS_INSTR : cosim.transRead(byte_addr,  &rdata32); data = rdata32 ; break;
            default: cycle_count = RV32I_EXT_MEM_NOT_PROCESSED; break;
        }
    }
    // Use co-simulated memory for any RAM addresses not already processed
    else if (byte_addr < (RAM_BASE_ADDR+INTMEMSIZE))
    {
        // Calculate offset into co-simulated memory ROM or RAM
        uint32_t offset  = byte_addr - ((byte_addr < RAM_BASE_ADDR) ? ROM_BASE_ADDR : RAM_BASE_ADDR);

        // Point to co-sim co-simulated ROM or RAM
        uint8_t  *pmem   = (byte_addr < RAM_BASE_ADDR) ? rommem : rammem;


        // Select the co-simulated memory access call based on the access type
        switch(type)
        {
            case MEM_WR_ACCESS_BYTE  : pmem[offset]   = data; break;
            case MEM_WR_ACCESS_HWORD : pmem[offset]   = data;
                                       pmem[offset+1] = data >> 8;
                                       break;
            case MEM_WR_ACCESS_WORD  :
            case MEM_WR_ACCESS_INSTR : pmem[offset]   = data;
                                       pmem[offset+1] = data >> 8;
                                       pmem[offset+2] = data >> 16;
                                       pmem[offset+3] = data >> 24;
                                       break;
            case MEM_RD_ACCESS_BYTE  : data = pmem[offset];
                                       break;
            case MEM_RD_ACCESS_HWORD : data = (uint32_t)pmem[offset]         |
                                              (uint32_t)pmem[offset+1] << 8;
                                       break;
            case MEM_RD_ACCESS_WORD  :
            case MEM_RD_ACCESS_INSTR : data = (uint32_t)pmem[offset]         |
                                              (uint32_t)pmem[offset+1] << 8  |
                                              (uint32_t)pmem[offset+2] << 16 |
                                              (uint32_t)pmem[offset+3] << 24;
                                       break;
            default: cycle_count = RV32I_EXT_MEM_NOT_PROCESSED; break;
        }
    }
    else
    {
        cycle_count = RV32I_EXT_MEM_NOT_PROCESSED;
    }

    return cycle_count;
}

// =========================================================================
// Main entry point for co-simulation node 0
//
// Configures and runs the rv32 instruction set simulator, either in normal
// mode or gdb remote target mode
//
// =========================================================================

extern "C" void VUserMain0 (int node)
{
    bool                error = false;
    std::string         test_name("CoSim_Rv32Demo");
    OsvvmCosim          cosim(node, test_name);
    rv32_cosim_utils    rv32util;

    // Create a configuration object
    rv32i_cfg_s cfg;

    // Override the defaults with test specifics
    rv32util.parse_args(cfg, node);

    // Create a new cpu object
    rv32* pCpu               = new rv32();

    // Register external memory access callback with RV32
    pCpu->register_ext_mem_callback(memcb);

    // Load an executable, either binary or ELF, always if not gdb
    // mode, or if a filename  was specified in gdb mode
    if (!cfg.gdb_mode || cfg.user_fname)
    {
        if (cfg.load_binary)
        {
            error = pCpu->read_binary(cfg.exec_fname, cfg.load_bin_addr) != 0;
        }
        else
        {
            error = pCpu->read_elf(cfg.exec_fname) != 0;
        }
    }

    // If GDB mode not configured, simply run the specified program
    if (!cfg.gdb_mode)
    {
        // If program loaded successfully, execute
        if (!error)
        {
            // Run processor until halted
            pCpu->run(cfg);

            // Check exit status
            if (error = rv32util.check_exit_status(pCpu))
            {
                VPrint("*FAIL*: exit code = 0x%08x finish code = 0x%08x running %s\n",
                        pCpu->regi_val(10) >> 1, pCpu->regi_val(17), cfg.exec_fname);
            }
            else
            {
                VPrint("PASS: exit code = 0x%08x running %s\n", pCpu->regi_val(10), cfg.exec_fname);
            }
        }
        else
        {
            VPrint("***ERROR in loading executable file\n");
            error = true;
        }
    }
    // If GDB remote target mode configured, run in debug mode
    else
    {
#ifdef __WIN32__
        // If a windows compilation, initialise Winsock
        WORD versionWanted   = MAKEWORD(1, 1);
        WSADATA wsaData;
        WSAStartup(versionWanted, &wsaData);
#endif

        if (!error)
        {
            // In debug mode, always nee to halt on ebreak for inserted breakpoints
            cfg.hlt_on_ebreak = true;
            
            // Start processing commands from GDB
            if (rv32gdb_process_gdb(pCpu, cfg.gdb_ip_portnum, cfg))
            {
                VPrint("***ERROR in opening debug  socket\n");
                error = true;
            }
        }

#ifdef __WIN32__
        // Terminate windows socket operations
        WSACleanup;
#endif
    }

    // If specified, dump the registers
    if (cfg.dump_regs)
    {
        rv32util.reg_dump(pCpu, cfg.dbg_fp, cfg.abi_en);
    }

    // If specified, dump the CSRs
    if (cfg.dump_csrs)
    {
        rv32util.csr_dump(pCpu, cfg.dbg_fp);
    }

    // If specified, dump the number of DMEM words
    if (cfg.num_mem_dump_words)
    {
        rv32util.mem_dump(cfg.num_mem_dump_words,
                           cfg.mem_dump_start,
                           pCpu,
                           cfg.dbg_fp);
    }

    // Close debug output file (unless stdout)
    if (cfg.dbg_fp != stdout)
    {
        fclose(cfg.dbg_fp);
    }

    // Delete RV32 object
    delete pCpu;

    // Flag to other nodes that we're done
    rv32_finished = true;

    // Flag to the simulation that node 0 has finished and tick to allow flushing
    cosim.tick(10, true, error);

    // Tick forever to allow other nodes to continue to run
    SLEEPFOREVER;
}

// =========================================================================
// Main entry point for co-simulation node 1
// =========================================================================

extern "C" void VUserMain1 (int node)
{
    bool             error = false;
    std::string      test_name("CoSim_Rv32Demo");
    OsvvmCosimStream uarttx(node, test_name);
    uint32_t         byte_count = 0;


    // Loop, processing TX data until RV32 finished
    while (true)
    {
        // Wait for a TX byte
        while (uartq.empty())
        {
            if (!rv32_finished)
            {
                uarttx.tick(1);
            }
            else
            {
                break;
            }
        }

        if (!uartq.empty())
        {
            // Send it out over the UART
            uarttx.streamSend(uartq.front(), UARTTB_NO_ERROR);

            // Pop the byte just sent from the queue
            uartq.pop();

            byte_count++;
        }
        else
        {
            if (rv32_finished)
            {
                break;
            }
        }
    }

    // Flag to the simulation that node 1 has finished
    uarttx.tick(10, true, error);

    SLEEPFOREVER;
}

// =========================================================================
// Main entry point for co-simulation node 2
// =========================================================================

extern "C" void VUserMain2 (int node)
{
    bool             error = false;
    int              idx   = 0;
    uint8_t          rdata;

    std::string test_name("CoSim_Rv32Demo");
    OsvvmCosimStream uartrx(node, test_name);

    while (idx < MSG_COMPRESSED_COUNT)
    {
        uartrx.streamGet(&rdata);
        cmpdata[idx++] = rdata;
    }

    // Decompress the received message
    decompress(cmpdata, dcmpdata, idx);

    VPrint("\n****************************************************************\n");
    VPrint("* VUserMain2: Decompressed %d bytes into a %d byte message: *\n", idx, strlen(dcmpdata));
    VPrint("****************************************************************\n");

    // Print the received message
    VPrint("\n%s\n", dcmpdata);

    // Flag to the simulation that node 2 has finished
    uartrx.tick(1, true, error);

    SLEEPFOREVER;
}

