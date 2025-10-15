// =========================================================================
//
//  File Name:         pcieVcInterface.h
//  Design Unit Name:
//  Revision:          OSVVM MODELS STANDARD VERSION
//
//  Maintainer:        Simon Southwell email:  simon.southwell@gmail.com
//  Contributor(s):
//    Simon Southwell      simon.southwell@gmail.com
//
//  Description:
//    PCIe VC model C++ interface code between bus independent model port
//    and PCIe link ports
//
//  Revision History:
//    Date      Version    Description
//    09/2025   ????.??    Initial Version
//
//  This file is part of OSVVM.
//
//  Copyright (c) 2025 by [OSVVM Authors](../../AUTHORS.md)
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

#include "OsvvmCosim.h"
#include "pcieVcInterface.h"

//-------------------------------------------------------------
// VUserInput()
//
// Singleton re-entrant wrapper for pcieVcInterface object's
// input callback function, with object instance passed in
// with obj_instance.
//
//-------------------------------------------------------------

static void VUserInput(pPkt_t pkt, int status, void* obj_instance)
{
    ((pcieVcInterface*)obj_instance)->InputCallback(pkt, status);
}

//-------------------------------------------------------------
// pcieVcInterface::InputCallback()
//
// pcieVcInterface input packet callback method
//
//-------------------------------------------------------------

void pcieVcInterface::InputCallback(pPkt_t pkt, int status)
{
    int idx;

    PktData_t tlp_type = GET_TLP_TYPE(pkt->data);

    if (pkt->seq == DLLP_SEQ_ID)
    {
        DebugVPrint("---> VUserInput_0 received DLLP\n");
        free(pkt->data);
        free(pkt);
    }
    else if (tlp_type == TL_CPL || tlp_type == TL_CPLD || tlp_type == TL_CPLLK || tlp_type == TL_CPLDLK)
    {
        DebugVPrint("---> InputCallback received TLP completion,  sequence %d of %d bytes\n", pkt->seq, pkt->ByteCount);

        // Extract the completion status from the packet.
        cpl_status = GET_CPL_STATUS(pkt->data);

        // Warn if a bad (non-zero) status
        if (cpl_status)
        {
            VPrint("**WARNING: InputCallback() received packet with status %s at node %d. Discarding.\n",
                    (cpl_status == CPL_UNSUPPORTED) ? "UNSUPPORTED" :
                    (cpl_status == CPL_CRS)         ? "CRS"         :
                    (cpl_status == CPL_ABORT)       ? "ABORT"       :
                                                      "UNKNOWN", node);
        }
        // If a successful completion with data, extract the TPL payload data and display
        else if (pkt->ByteCount)
        {
            // Get a pointer to the start of the payload data
            pPktData_t payload = GET_TLP_PAYLOAD_PTR(pkt->data);

            // Fetch data and put in receive buffer
            DebugVPrint("---> ");
            for (idx = 0; idx < pkt->ByteCount; idx++)
            {
                rxdatabuf[idx] = payload[idx];

                DebugVPrint("%02x ", payload[idx]);
                if ((idx % 16) == 15)
                {
                    DebugVPrint("\n---> ");
                }
            }

            if ((idx % 16) != 0)
            {
                DebugVPrint("\n");
            }
        }

        // Once input packet is finished with, the allocated space *must* be freed.
        // All input packets have their own memory space to avoid overwrites with
        // shared buffers.
        DISCARD_PACKET(pkt);
    }
}

//-------------------------------------------------------------
// pcieVcInterface::run()
//
// pcieVcInterface main program.
//
//-------------------------------------------------------------

void pcieVcInterface::run(void)
{
    int        error = 0;
    bool       end   = false;
    int        halt  = 0;

    int        byteidx;
    unsigned   operation;
    unsigned   int_to_model;
    unsigned   option;

    uint64_t   rdata;
    uint64_t   wdata;
    uint64_t   wdatawidth;
    uint64_t   rdatawidth;
    uint64_t   address;
    uint64_t   addrlo;

    // Initialise PCIe VHost, with input callback function and no user pointer.
    pcie->initialisePcie(VUserInput, this);

    pcie->getPcieVersionStr(sbuf, strbufsize);
    VPrint("  %s\n", sbuf);

    DebugVPrint("pcieVcInterface::run: on node %d\n", node);

    // Fetch the model parameters
    VRead(LANESADDR,  &link_width, DELTACYCLE, node);
    VRead(PIPE_ADDR,  &pipe_mode,  DELTACYCLE, node);
    VRead(EP_ADDR,    &ep_mode,    DELTACYCLE, node);
    VRead(REQID_ADDR, &rid,        DELTACYCLE, node);

    if (pipe_mode)
    {
        pcie->configurePcie(CONFIG_DISABLE_SCRAMBLING);
        pcie->configurePcie(CONFIG_DISABLE_8B10B);
    }

    // Make sure the link is out of electrical idle
    VWrite(LINK_STATE, 0, DELTACYCLE, node);

    // Use node number as seed
    pcie->pcieSeed(node);

    // Send out idles until reset de-asserted
    do
    {
        pcie->sendIdle();
        VRead(RESET_STATE, &reset_state, CLOCKEDCYCLE, node);
    } while(reset_state);

    // Loop forever, processing commands and driving the PCIe link
    while (!error && !end)
    {
        // Ack transaction
        VWrite(ACKTRANS, 1, DELTACYCLE, node);

        // Check if there is a new transaction (delta)
        VRead(GETNEXTTRANS, &operation, DELTA_CYCLE, node);

        switch (operation)
        {
            case SET_MODEL_OPTIONS :
               VRead(GETOPTIONS,    &option,       DELTACYCLE, node);
               VRead(GETINTTOMODEL, &int_to_model, DELTACYCLE, node);

               // If a PCIe C model config option, pass straight to model
               if (option < VCOPTIONSTART)
               {
                   pcie->configurePcie(static_cast<config_t>(option), int_to_model);
               }
               else
               {
                   switch(option)
                   {
                   case ENDMODELRUN:
                       end = true;
                       halt = int_to_model;
                       break;

                   // Do PHY layer link training initialisation.
                   case INITPHY:
                       InitLink(link_width, node);
                       break;

                   // Do data link layer flow control initialisation
                   case INITDLL:
                       // Initialise flow control
                       pcie->initFc();
                       break;

                   // Set the transaction layer mode---memory, I/O, config space, completion or message
                   case SETTRANSMODE:
                       trans_mode = (pcie_trans_mode_t)int_to_model;
                       break;

                   default:
                       VPrint("pcieVcInterface::run : ***ERROR. Unrecognised SET_MODEL_OPTIONS option (%d)\n", option);
                       error++;
                   }
               }
               break;

            case WRITE_OP :
                VRead64(GETADDRESS,     &address,    DELTACYCLE, node);
                VRead64(GETDATATOMODEL, &wdata,      DELTACYCLE, node);
                VRead64(GETDATAWIDTH,   &wdatawidth, DELTACYCLE, node);

                // Place data into a PCIe model byte buffer
                for (byteidx = 0; byteidx < wdatawidth; byteidx++)
                {
                    txdatabuf[byteidx] = (wdata >> (byteidx<<3)) & 0xff;
                }

                switch(trans_mode)
                {
                case MEM_TRANS :
                    // Do a posted memory write (no completion to wait for
                    pcie->memWrite(address, txdatabuf, wdatawidth/8, tag++, rid, false, ep_mode);
                    break;
                    
                case CFG_SPC_TRANS :
                    pcie->cfgWrite(address, txdatabuf, wdatawidth/8, tag++, rid, false, ep_mode);
                    
                    // Non-posted transaction, so do a wait for the status completion
                    pcie->waitForCompletion();
                    
                    // Flag any bad status
                    if (cpl_status)
                    {
                        VPrint("pcieVcInterface::run : ***ERROR. Received bad status (%d) on WRITE_OP\n", cpl_status);
                        error++;
                    }
                    break;
                    
                default :
                    VPrint("pcieVcInterface::run : ***ERROR. Unrecognised transaction mode on WRITE_OP (%d)\n", trans_mode);
                    error++;
                    break;
                }

                break;

            case READ_OP :
                VRead64(GETADDRESS,   &address,    DELTACYCLE, node);
                VRead64(GETDATAWIDTH, &rdatawidth, DELTACYCLE, node);

                switch(trans_mode)
                {
                case MEM_TRANS :
                    // Instigate a memory read
                    pcie->memRead(address, rdatawidth/8, tag++, rid, false, ep_mode);
                    break;
                    
                case CFG_SPC_TRANS :
                    // Instigate a configuration space read
                    pcie->cfgRead(address, rdatawidth/8, tag++, rid, false, ep_mode);
                    break;

                default :
                    VPrint("pcieVcInterface::run : ***ERROR. Unrecognised transaction mode on WRITE_OP (%d)\n", trans_mode);
                    error++;
                    break;
                }

                // Blocking read, so do a wait for the completion
                pcie->waitForCompletion();

                // If a successful completion returned, extract data
                if (!cpl_status)
                {
                    // Get data
                    addrlo = address & 0x3ULL;
                    for (rdata = 0, byteidx = 0; byteidx < (rdatawidth/8); byteidx++)
                    {
                        rdata |= (rxdatabuf[byteidx+addrlo] & 0xff) << (8 * byteidx);
                    }
                }
                else
                {
                    rdata = 0;
                    VWrite(SETBOOLFROMMODEL, 1, DELTACYCLE, node);
                }

                // Update transaction record return data
                VWrite64(SETDATAFROMMODEL, rdata, DELTACYCLE, node);
                
                break;

            case WAIT_FOR_CLOCK :
                VRead(GETINTTOMODEL, &int_to_model, DELTACYCLE, node);
                pcie->sendIdle(int_to_model);
                break;

              // Configuration options
                // Update model configuration
              // High level actions
                // Link initialisation
              // Posted Transactions
                // Fetch data from transaction
                // Send transaction
              // Non-posted transactions
                // [fetch data from transaction]
                // Send Transaction [queued]
              // Completion transactions
                // [fetch data from transaction]
                // Send completion
              // Simulation control commands (delta)
                // Write to control address
              //

            case SET_BURST_MODE:
                VPrint("===> SET_BURST_MODE");
                break;

            default :
                VPrint("pcieVcInterface::run : ***ERROR. Unrecognised operation (%d)\n", operation);
                error++;
                break;
        }
    }


    if (error)
    {
        VPrint("***Error: pcieVcInterface::run() had an error\n");

        // Halt the simulation with a fatal error
        VWrite(PVH_FATAL, 0, 0, node);
    }
    else if (end)
    {
        if (halt == FINISHSIM)
        {
            VWrite(PVH_FINISH, 0, 0, node);
        }
        else if (halt == STOPSIM)
        {
            VWrite(PVH_STOP, 0, 0, node);
        }
    }

    // If reached here without stop/finish, send out idles forever
    // to allow simulation to continue
    while (true)
    {
        pcie->sendIdle(10000);
    }
}