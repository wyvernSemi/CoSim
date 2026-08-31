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


#include "rv32_cosim_utils.h"

// -------------------------------------------------------------------------
// Parse configuration file arguments for rv32 ISS, reading from a
// vusermain.cfg file
// -------------------------------------------------------------------------

int rv32_cosim_utils::parse_args(rv32i_cfg_s &cfg, const int node)
{
    int    error               = 0;
    int    argc                = 0;
    char*  argvBuf[MAXARGS];
    char** argv = NULL;
    int    c;

    char   argstr[STRBUFSIZE];
    size_t len                 = 0;
    char   delim[2];
    char   vusermainname[16];
    FILE*  fp;

    // Open configuration file for this node
    fp = fopen(CFGFILENAME.c_str(), "r");
    if (fp == NULL)
    {
        printf("parse_args: failed to open file %s\n", CFGFILENAME.c_str());
        error = 1;
    }

    // If configuration file opened successfully, then parse it
    if (!error)
    {

        strcpy(delim, " ");
        sprintf(vusermainname, "%s%c", CFGROOTNAME.c_str(), '0' + node);

        while (fgets(argstr, STRBUFSIZE, fp) != NULL)
        {
            char* name = strtok(argstr, delim);

            if (strcmp(name, vusermainname) == 0)
            {
                argvBuf[argc++] = name;
                break;
            }
        }

        fclose(fp);

        while((argvBuf[argc] = strtok(NULL, " ")) != NULL && argc < MAXARGS)
        {
            unsigned lastchar = argvBuf[argc][strlen(argvBuf[argc])-1];

            // If last character is CR or LF, delete it
            if (lastchar == '\r' || lastchar == '\n')
            {
                argvBuf[argc][strlen(argvBuf[argc])-1] = 0;
            }

            argc++;
        }

        argv = argvBuf;

        // Parse the command line arguments and/or configuration file
        // Process the command line options *only* for the INI filename, as we
        // want the command line options to override the INI options
        while ((c = getopt(argc, argv, GETOPT_ARG_STR.c_str())) != EOF)
        {
            switch (c)
            {
            case 't':
                strncpy(execstr, optarg, STRBUFSIZE);
                cfg.exec_fname             = execstr;
                cfg.user_fname             = true;
                break;
            case 'B':
                cfg.load_binary            = true;
                break;
            case 'L':
                cfg.load_bin_addr          = strtol(optarg, NULL, 0);
                break;
            case 'n':
                cfg.num_instr              = atoi(optarg);
                break;
            case 'b':
                cfg.en_brk_on_addr         = true;
                break;
            case 'A':
                cfg.brk_addr               = strtol(optarg, NULL, 0);
                break;
            case 'r':
                cfg.rt_dis                 = true;
                break;
            case 'd':
                cfg.dis_en                 = true;
                break;
            case 'H':
                cfg.hlt_on_inst_err        = true;
                break;
            case 'e':
                cfg.hlt_on_ecall           = true;
                break;
            case 'E':
                cfg.hlt_on_ebreak          = true;
                break;
            case 'D':
                if ((cfg.dbg_fp = fopen(optarg, "wb")) == NULL)
                {
                    fprintf(stderr, "**ERROR: unable to open specified debug file (%s) for writing.\n", optarg);
                    error = 1;
                }
                break;
            case 'g':
                cfg.gdb_mode = true;
                break;
            case 'p':
                cfg.gdb_ip_portnum         = strtol(optarg, NULL, 0);
                break;
            case 'S':
                cfg.update_rst_vec         = true;
                cfg.new_rst_vec            = strtol(optarg, NULL, 0);
                break;
            case 's':
                cfg.update_sp              = true;
                cfg.new_sp                 = strtol(optarg, NULL, 0);
                break;
            case 'C':
                cfg.use_cycles_for_mtime   = true;
                break;
            case 'a':
                cfg.abi_en = true;
                break;
            case 'T':
                cfg.use_external_timer     = true;
                break;
            case 'x':
                cfg.dump_regs              = true;
                break;
            case 'c':
                cfg.dump_csrs              = true;
                break;
            case 'm':
                cfg.num_mem_dump_words     = strtol(optarg, NULL, 0);
                break;
            case 'M':
                cfg.mem_dump_start         = (uint32_t)strtol(optarg, NULL, 0);
                break;
            case 'i':
                sw_irq_addr                = strtol(optarg, NULL, 0);
                break;
            case 'h':
            default:
                fprintf(stderr, "Usage: %s -t <test executable> [-hHeEbdrgcxTaCBRc][-n <num instructions>]\n"
                                "     [-L <load addr>][-S <start addr>][-s <sp addr>[-A <brk addr>]\n"
                                "     [-m <num of mem dump words>][-M <mem dump start addr>]\n"
                                "     [-D <debug o/p filename>][-p <port num>][-s <addr>]\n"
                                "   -t specify test executable (default test.exe)\n"
                                "   -B specify to load a raw binary file (default load ELF executable)\n"
                                "   -L specify address to load binary, if -B specified (default 0x00000000)\n"
                                "   -n specify number of instructions to run (default 0, i.e. run until unimp)\n"
                                "   -d Enable disassemble mode (default off)\n"
                                "   -r Enable run-time disassemble mode (default off. Overridden by -d)\n"
                                "   -C Use cycle count for internal mtime timer (default real-time)\n"
                                "   -a display ABI register names when disassembling (default x names)\n"
                                "   -T Use external memory mapped timer model (default internal)\n"
                                "   -H Halt on unimplemented instructions (default trap)\n"
                                "   -e Halt on ecall instruction (default trap)\n"
                                "   -E Halt on ebreak instruction (default trap)\n"
                                "   -b Halt at a specific address (default off)\n"
                                "   -A Specify halt address if -b active (default 0x00000040)\n"
                                "   -D Specify file for debug output (default stdout)\n"
                                "   -i Specify a software interrupt address (default = 0x%08x)\n"
                                "   -x Dump x0 to x31 on exit (default no dump)\n"
                                "   -c Dump CSR registers on exit (default no dump)\n"
                                "   -m Dump specified number of words from memory (default no dump)\n"
                                "   -M Start byte address of memory dump (default 0x1000)\n"
                                "   -g Enable remote gdb mode (default disabled)\n"
                                "   -p Specify remote GDB port number (default 49152)\n"
                                "   -S Specify start address (default 0)\n"
                                "   -s Specify stack pointer address (default 0)\n"
                                "   -h display this help message\n", argv[0], SWIRQADDR);
                error = (c == 'h') ? 0 : 1;
                break;
            }
        }
    }

    return error;
}

// -------------------------------------------------------------------------
// Dump registers using calls to rv32 object
// -------------------------------------------------------------------------

void rv32_cosim_utils::reg_dump(rv32* pCpu, FILE* dfp, bool abi_en)
{
    fprintf(dfp, "\nRegister state:\n\n  ");

    // Loop through all the registers
    for (int idx = 0; idx < rv32i_cpu::RV32I_NUM_OF_REGISTERS; idx++)
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
// Dump CSRs
// -------------------------------------------------------------------------

void rv32_cosim_utils::csr_dump (rv32* pCpu, FILE* dfp)
{
    fprintf(dfp, "CSR state:\n\n");
    fprintf(dfp, "  mstatus    = 0x%08x\n",     pCpu->csr_val(rv32csr_consts::RV32CSR_ADDR_MSTATUS));
    fprintf(dfp, "  mie        = 0x%08x\n",     pCpu->csr_val(rv32csr_consts::RV32CSR_ADDR_MIE));
    fprintf(dfp, "  mvtec      = 0x%08x\n",     pCpu->csr_val(rv32csr_consts::RV32CSR_ADDR_MTVEC));
    fprintf(dfp, "  mscratch   = 0x%08x\n",     pCpu->csr_val(rv32csr_consts::RV32CSR_ADDR_MSCRATCH));
    fprintf(dfp, "  mepc       = 0x%08x\n",     pCpu->csr_val(rv32csr_consts::RV32CSR_ADDR_MEPC));
    fprintf(dfp, "  mcause     = 0x%08x\n",     pCpu->csr_val(rv32csr_consts::RV32CSR_ADDR_MCAUSE));
    fprintf(dfp, "  mtval      = 0x%08x\n",     pCpu->csr_val(rv32csr_consts::RV32CSR_ADDR_MTVAL));
    fprintf(dfp, "  mip        = 0x%08x\n",     pCpu->csr_val(rv32csr_consts::RV32CSR_ADDR_MIP));
    fprintf(dfp, "  mcycle     = 0x%08x%08x\n", pCpu->csr_val(rv32csr_consts::RV32CSR_ADDR_MCYCLEH),   pCpu->csr_val(rv32csr_consts::RV32CSR_ADDR_MCYCLE));
    fprintf(dfp, "  minstret   = 0x%08x%08x\n", pCpu->csr_val(rv32csr_consts::RV32CSR_ADDR_MINSTRETH), pCpu->csr_val(rv32csr_consts::RV32CSR_ADDR_MINSTRET));

    bool fault;
    uint32_t mtimel = pCpu->read_mem(rv32i_consts::RV32I_RTCLOCK_ADDRESS,   rv32i_consts::RV32I_MEM_RD_ACCESS_WORD, fault);
    uint32_t mtimeh = pCpu->read_mem(rv32i_consts::RV32I_RTCLOCK_ADDRESS+4, rv32i_consts::RV32I_MEM_RD_ACCESS_WORD, fault);
    fprintf(dfp, "  mtime      = 0x%08x%08x\n", mtimeh, mtimel);

    mtimel = pCpu->read_mem(rv32i_consts::RV32I_RTCLOCK_CMP_ADDRESS,   rv32i_consts::RV32I_MEM_RD_ACCESS_WORD, fault);
    mtimeh = pCpu->read_mem(rv32i_consts::RV32I_RTCLOCK_CMP_ADDRESS+4, rv32i_consts::RV32I_MEM_RD_ACCESS_WORD, fault);
    fprintf(dfp, "  mtimecmp   = 0x%08x%08x\n", mtimeh, mtimel);

}

// -------------------------------------------------------------------------
// Dump memory
// -------------------------------------------------------------------------

void rv32_cosim_utils::mem_dump(uint32_t num, uint32_t start, rv32* pCpu, FILE* dfp)
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


// -------------------------------------------------------------------------
// Check for pass/fail valid exit status
// -------------------------------------------------------------------------

bool rv32_cosim_utils::check_exit_status(rv32* pCpu)
{
    return pCpu->regi_val(10) || pCpu->regi_val(17) != 93;
}

