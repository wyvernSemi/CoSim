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
//    Header for PCIe VC model C++ interface code between bus independent
//    model port and PCIe link ports
//
//  Revision History:
//    Date      Version    Description
//    09/2025   ????       Initial Version
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

#include <cstdio>
#include <cstdlib>

#include "pcieModelClass.h"
#include "OsvvmPcieAdapter.h"

extern "C" {
#include "ltssm.h"
}

#ifndef _PCIEVCINTERFACE_H_
#define _PCIEVCINTERFACE_H_

#ifndef LO_NIBBLE_MASK
#define LO_NIBBLE_MASK               0x0f
#endif

#ifndef HI_NIBBLE_MASK
#define HI_NIBBLE_MASK               0xf0
#endif

// -------------------------------------------------------------------------
// DEFINES
// -------------------------------------------------------------------------

    // **** If the below values change, also update ../src/PcieVcInterfacePkg.vhd ****

    // These following commented out constants are defined by the pcievhost
    // model's code in pcie_vhost_map.h

// #define PVH_STOP              -3
// #define PVH_FINISH            -2
// #define PVH_FATAL             -1
// #define                      
// #define LINKADDR0              0
// #define LINKADDR1              1
// #define LINKADDR2              2
// #define LINKADDR3              3
// #define LINKADDR4              4
// #define LINKADDR5              5
// #define LINKADDR6              6
// #define LINKADDR7              7
// #define LINKADDR8              8
// #define LINKADDR9              9
// #define LINKADDR10            10
// #define LINKADDR11            11
// #define LINKADDR12            12
// #define LINKADDR13            13
// #define LINKADDR14            14
// #define LINKADDR15            15
// #define                      
// #define NODENUMADDR          200
// #define LANESADDR            201
// #define PVH_INVERT           202
// #define EP_ADDR              203
// #define CLK_COUNT            204
// #define LINK_STATE           205
// #define RESET_STATE          206

// Parameter read addresses
#define REQID_ADDR           300
#define PIPE_ADDR            301
#define EN_ECRC_ADDR         302
#define INITPHY_ADDR         303

// Transaction interface options address offsets
#define GETNEXTTRANS         400
#define GETINTTOMODEL        401
#define GETBOOLTOMODEL       402
#define GETTIMETOMODEL       403
#define GETADDRESS           404
#define GETADDRESSWIDTH      405
#define GETDATATOMODEL       406
#define GETDATAWIDTH         407
#define GETPARAMS            408
#define GETOPTIONS           409
#define ACKTRANS             410
#define SETDATAFROMMODEL     411
#define SETBOOLFROMMODEL     412
#define POPDATA              413
#define PUSHDATA             414

// **** If the above values change, also update ../src/PcieVcInterfacePkg.vhd ****

// -------------------------------------------------------------------------
// Class definition
// -------------------------------------------------------------------------

class pcieVcInterface
{

public:

    static constexpr int   DELTACYCLE         =  -1;
    static constexpr int   CLOCKEDCYCLE       =   0;
    static constexpr int   PIPE_MODE_ENABLED  =   1;
    static constexpr int   PIPE_MODE_DISABLED =   0;
    static constexpr int   EP_MODE_ENABLED    =   1;
    static constexpr int   EP_MODE_DISABLED   =   0;
    static constexpr int   strbufsize         = 256;
    static constexpr int   databufsize        = 4096;

    static constexpr int   FREERUNSIM         = 0;
    static constexpr int   STOPSIM            = 1;
    static constexpr int   FINISHSIM          = 2;

    static constexpr int   VCOPTIONSTART      = 1000;
    static constexpr int   ENDMODELRUN        = VCOPTIONSTART;

    static constexpr int   SETTRANSMODE       = 1001;
    static constexpr int   INITDLL            = 1002;
    static constexpr int   INITPHY            = 1003;
    static constexpr int   SETRDLCK           = 1004;

    typedef enum pcie_trans_mode_e
    {
        MEM_TRANS,
        IO_TRANS,
        CFG_SPC_TRANS,
        MSG_TRANS,
        CPL_TRANS
    } pcie_trans_mode_t;

                pcieVcInterface (const unsigned nodeIn) : node (nodeIn)
                {
                    // Create a PCIe API object
                    pcie        = new pcieModelClass(nodeIn);

                    // Default the internal state member variables
                    reset_state = 0;
                    link_width  = 0;
                    rid         = node;
                    pipe_mode   = PIPE_MODE_DISABLED;
                    ep_mode     = EP_MODE_DISABLED;

                    trans_mode  = MEM_TRANS;
                    rd_lck      = false;
                    tag         = 0;
                    
                    txdatabuf   = new PktData_t[databufsize];
                    rxdatabuf   = new PktData_t[databufsize];
                };

    void        run(void);

    // Input data callback, passing in a packet, error status
    void        InputCallback (pPkt_t pkt, int status);

private:

    uint32_t           node;
    pcieModelClass    *pcie;

    unsigned           tag;
    unsigned           reset_state;
    unsigned           link_width;
    unsigned           rid;
    unsigned           pipe_mode;
    unsigned           ep_mode;
    pcie_trans_mode_t  trans_mode;
    bool               rd_lck;

    char               sbuf[strbufsize];
    pPktData_t         txdatabuf;

    // Buffer for use by input callback
    pPktData_t         rxdatabuf;
    
    PktData_t          cpl_status;

};

#endif