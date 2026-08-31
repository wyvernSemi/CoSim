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
//    06/2026   2026.08    Added support for DLLP and PHY traffic processing
//    09/2025   2026.01    Initial Version
//
//  This file is part of OSVVM.
//
//  Copyright (c) 2025 - 2026 by [OSVVM Authors](../../AUTHORS.md)
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
#include <vector>
#include <queue>

#include "pcieModelClass.h"
#include "OsvvmPcieAdapter.h"

extern "C" {
#include "ltssm.h"
}

#ifndef _PCIEVCINTERFACE_H_
#define _PCIEVCINTERFACE_H_

// -------------------------------------------------------------------------
// DEFINES
// -------------------------------------------------------------------------

#ifndef LO_NIBBLE_MASK
#define LO_NIBBLE_MASK                               0x0f
#endif

#ifndef HI_NIBBLE_MASK
#define HI_NIBBLE_MASK                               0xf0
#endif

// -------------------------------------------------------------------------
// Help macros
// -------------------------------------------------------------------------

#define TLP_HAS_ADDR(_tlp_type) (                                    \
    _tlp_type == TL_MWR32    || _tlp_type == TL_MWR64    ||          \
    _tlp_type == TL_MRD32    || _tlp_type == TL_MRD64    ||          \
    _tlp_type == TL_MRDLCK32 || _tlp_type == TL_MRDLCK64 ||          \
    _tlp_type == TL_IOWR     || _tlp_type == TL_IORD                 \
)

#define TLP_IS_MSG(_tlp_type) (                                      \
    (_tlp_type & 0xf8) == TL_MSG  || (_tlp_type & 0xf8) == TL_MSGD   \
)

#define TLP_IS_CFGSPC(_tlp_type) (                                   \
    _tlp_type == TL_CFGRD0 || _tlp_type == TL_CFGWR0 ||              \
    _tlp_type == TL_CFGRD1 || _tlp_type == TL_CFGWR1                 \
)

#define TLP_HAS_DATA(_tlp_type) (                                    \
    _tlp_type == TL_MWR32  || _tlp_type          == TL_MWR64   ||    \
    _tlp_type == TL_CFGWR0 || _tlp_type          == TL_CFGWR1  ||    \
    _tlp_type == TL_IOWR   || (_tlp_type & 0xf8) == TL_MSGD          \
)

// -------------------------------------------------------------------------
// Class definition
// -------------------------------------------------------------------------

class pcieVcInterface
{
public:

    // -------------------------------
    // Class constant definitions
    // -------------------------------

// **** If the below values change, also update ../src/PcieVcInterfacePkg.vhd ****
//  v    v    v    v    v    v    v    v    v    v    v    v    v    v    v    v
    // These following commented out constants are defined by the pcievhost
    // model's code in pcie_vhost_map.h

    // #define             PVH_STOP                    -3
    // #define             PVH_FINISH                  -2
    // #define             PVH_FATAL                   -1
    // #define
    // #define             LINKADDR0                    0
    // #define             LINKADDR1                    1
    // #define             LINKADDR2                    2
    // #define             LINKADDR3                    3
    // #define             LINKADDR4                    4
    // #define             LINKADDR5                    5
    // #define             LINKADDR6                    6
    // #define             LINKADDR7                    7
    // #define             LINKADDR8                    8
    // #define             LINKADDR9                    9
    // #define             LINKADDR10                  10
    // #define             LINKADDR11                  11
    // #define             LINKADDR12                  12
    // #define             LINKADDR13                  13
    // #define             LINKADDR14                  14
    // #define             LINKADDR15                  15
    // #define
    // #define             NODENUMADDR                200
    // #define             LANESADDR                  201
    // #define             PVH_INVERT                 202
    // #define             EP_ADDR                    203
    // #define             CLK_COUNT                  204
    // #define             LINK_STATE                 205
    // #define             RESET_STATE                206
    // #define             DISABLE_SCRAMBLING         207
    // #define             DISABLE_8B10B              208
    // #define             GEN2_CLK                   209

    // Generics address offsets
    static constexpr int   REQID_ADDR            =    300;
    static constexpr int   EN_ECRC_ADDR          =    302;
    static constexpr int   INITPHY_ADDR          =    303;
    static constexpr int   ENABLE_AUTO_ADDR      =    304;

    // Transaction interface options address offsets
    static constexpr int   GETNEXTTRANS          =    400;
    static constexpr int   GETINTTOMODEL         =    401;
    static constexpr int   GETBOOLTOMODEL        =    402;
    static constexpr int   GETTIMETOMODEL        =    403;
    static constexpr int   GETADDRESS            =    404;
    static constexpr int   GETADDRESSWIDTH       =    405;
    static constexpr int   GETDATATOMODEL        =    406;
    static constexpr int   GETDATAWIDTH          =    407;
    static constexpr int   GETPARAMS             =    408;
    static constexpr int   GETOPTIONS            =    409;
    static constexpr int   ACKTRANS              =    410;
    static constexpr int   SETDATAFROMMODEL      =    411;
    static constexpr int   SETBOOLFROMMODEL      =    412;
    static constexpr int   SETINTFROMMODEL       =    413;
    static constexpr int   SETPARAMS             =    414;
    static constexpr int   POPWDATA              =    415;
    static constexpr int   PUSHWDATA             =    416;
    static constexpr int   POPRDATA              =    417;
    static constexpr int   PUSHRDATA             =    418;
    static constexpr int   PUSHRDATA32           =    419;
    static constexpr int   POPRDATA32            =    420;

    static constexpr int   VCOPTIONSTART         =   1000;
    static constexpr int   ENDMODELRUN           =   VCOPTIONSTART;

    // SetModelOptions for internal memory
    static constexpr int   SETCFGSPC             =   1001;
    static constexpr int   SETCFGSPCMASK         =   1002;
    static constexpr int   SETCFGSPCOFFSET       =   1003;
    static constexpr int   SETMEMADDRLO          =   1004;
    static constexpr int   SETMEMADDRHI          =   1005;
    static constexpr int   SETMEMDATA            =   1006;
    static constexpr int   SETMEMENDIANNESS      =   1007;

    // GetModelOptions for internal memory back door access
    static constexpr int   GETMEMDATA            =   2000;

    // EXTEND_OP Options
    static constexpr int   WAIT_FOR_TRANS        =      0;
    static constexpr int   TRY                   =      1;

    // EXTEND_DIRECTIVE_OP Options
    static constexpr int   INITDLL               =      0;
    static constexpr int   INITPHY               =      1;
    static constexpr int   GEN_OS                =      2;
    static constexpr int   GEN_TS                =      3;
    static constexpr int   GET_EVENT             =      4;
    static constexpr int   RST_EVENT             =      5;
    static constexpr int   GET_LANE_TS           =      6;
    static constexpr int   SEND_DLL_ACK          =      7;
    static constexpr int   SEND_DLL_NAK          =      8;
    static constexpr int   SEND_DLL_FC           =      9;
    static constexpr int   SEND_DLL_PM           =     10;
    static constexpr int   SEND_DLL_VEND_NODATA  =     11;
    static constexpr int   SEND_DLL_VEND_DATA    =     12;
    static constexpr int   WAIT_FOR_DLL          =     13;
    static constexpr int   TRY_DLL               =     14;

    // Internal memory endian control definitions
    static constexpr int   LITTLE_END            =      1;
    static constexpr int   BIG_END               =      0;

//  ^    ^    ^    ^    ^    ^    ^    ^    ^    ^    ^    ^    ^    ^    ^    ^
// **** If the above values change, also update ../src/PcieVcInterfacePkg.vhd ****

    // Simulator cycle control
    static constexpr int   DELTACYCLE            =     -1;
    static constexpr int   CLOCKEDCYCLE          =      0;

    // Simulation control definitions
    static constexpr int   FREERUNSIM            =      0;
    static constexpr int   STOPSIM               =      1;
    static constexpr int   FINISHSIM             =      2;

    // Generic inputs definitions
    static constexpr int   PIPE_MODE_ENABLED     =      1;
    static constexpr int   PIPE_MODE_DISABLED    =      0;
    static constexpr int   EP_MODE_ENABLED       =      1;
    static constexpr int   EP_MODE_DISABLED      =      0;
    static constexpr int   DIGEST_MODE_ENABLED   =      1;
    static constexpr int   DIGEST_MODE_DISABLED  =      0;
    static constexpr int   SCRAMBLING_ENABLED    =      0;
    static constexpr int   SCRAMBLING_DISABLED   =      1;

    // Buffer sizes
    static constexpr int   STRBUFSIZE            =    256;
    static constexpr int   DATABUFSIZE           =   4096;

    // Completion definitions
    static constexpr int   CMPL_ADDR_MASK        =   0x7c;

    // Tag generation control definitions
    static constexpr int   TLP_TAG_AUTO          =  0x100;

    // Burst mask definitions
    static constexpr int   BYTE_OFFSET_MASK      =    0x3;

    static constexpr int   MAX_TAG               =    256;
    
    static constexpr int   NO_TRANS_AVAIL        = 0xffffffff;

    // -------------------------------
    // Class type definitions
    // -------------------------------

    // Single data buffer vector type
    typedef std::vector<PktData_t> DataVec_t;

    // Receive data structure type
    typedef  struct {
        int       pkt_status;
        PktData_t cpl_status;
        PktData_t tag;
        PktData_t loaddr;
        DataVec_t rxbuf;
    } CplDataBuf_t;

    // Completion receive data queue type
    typedef std::queue<CplDataBuf_t> CplDataBufQueue_t;

    typedef struct {
        int      status;
        unsigned type;
        unsigned vc;
        unsigned hdrfc;
        unsigned datafc;
        unsigned seqnum;
        unsigned venddata;
    } DllBuf_t;

    typedef std::queue<DllBuf_t> DllBufQueue_t;

    typedef struct {
      uint8_t  func;
      uint8_t  dev;
      uint8_t  bus;
      uint16_t reg;
    } bus_dev_func_t;

    typedef struct{
        uint8_t fbe;
        uint8_t lbe;
    } byte_enable_t;

    typedef union {
        byte_enable_t be;
        uint8_t       msgcode;
    } be_msgcode_t;

    typedef union
    {
        uint64_t       addr;
        bus_dev_func_t bus;
    }  addr_bus_t;

    // Request data structure type
    typedef  struct {
        int          pkt_status;
        uint16_t     length;
        PktData_t    tag;
        uint16_t     rid;
        uint8_t      type;
        addr_bus_t   addr_bus;
        be_msgcode_t be_msg;
        uint32_t     byte_length;
        DataVec_t    wrbuf;
        bool         has_digest;
        bool         poisoned;
        uint8_t      attr;
        uint8_t      AT;
        uint8_t      cfg_func;
        uint8_t      cfg_dev;
        uint8_t      cfg_bus;
        uint8_t      cfg_reg;
    } ReqBuf_t;

    typedef struct {
        int          type;
        int          hdr_credits;
        int          data_credits;
        int          vc;
    } pcie_fc_params_t;

    // Receive request queue type
    typedef std::queue<ReqBuf_t> ReqBufQueue_t;

    // Enumerated type for different TLPs
    typedef enum pcie_trans_mode_e
    {
        MEM_TRANS,
        IO_TRANS,
        CFG_SPC_TRANS,
        MSG_TRANS,
        CPL_TRANS,
        PART_CPL_TRANS
    } pcie_trans_mode_t;

    // MIT generator parameter definitions
    typedef enum pcie_params_e
    {
        PARAM_TRANS_MODE,
        PARAM_RDLCK,
        PARAM_CMPLRID,
        PARAM_CMPLCID,
        PARAM_CMPLRLEN,
        PARAM_CMPLRTAG,
        PARAM_CMPLSTATUS,
        PARAM_REQTAG,
        PARAM_PKT_STATUS,
        PARAM_CMPL_STATUS,
        PARAM_CMPL_RX_TAG
    } pcie_params_t;

    // MIT receive request parameter definitions
    typedef enum pcie_req_params_e
    {
        PARAM_REQ_TYPE,
        PARAM_REQ_TAG,
        PARAM_REQ_RID,
        PARAM_REQ_BE_MSGCODE,
        PARAM_REQ_DIGEST,
        PARAM_REQ_POISONED,
        PARAM_REQ_ATTR,
        PARAM_REQ_AT,
        PARAM_REQ_FBE,
        PARAM_REQ_LBE,
        PARAM_REQ_MSG_CODE,
        PARAM_REQ_CFG_FUNC,
        PARAM_REQ_CFG_DEV,
        PARAM_REQ_CFG_BUS,
        PARAM_REQ_CFG_REG,
        PARAM_REQ_LENGTH,
        PARAM_REQ_BYTE_LEN,
        PARAM_REQ_PKT_STATUS,
        PARAM_REQ_ADDR,
        PARAM_REQ_ADDRHI
    } pcie_req_params_t;

    typedef enum pcie_ts_params_e
    {
        PARAM_TS_TYPE,
        PARAM_LINK,
        PARAM_LANE,
        PARAM_NFTS,
        PARAM_GEN,
        PARAM_CTL,
        PARAM_TS_COUNT
    } pcie_ts_params_t;
    
    typedef enum pcie_os_params_e
    {
        PARAM_OS_TYPE,
        PARAM_OS_COUNT
    } pcie_os_params_t;

    typedef enum pcie_fc_params_idx_e
    {
        PARAM_FC_TYPE,
        PARAM_FC_VC,
        PARAM_FC_HDR_CREDITS,
        PARAM_FC_DATA_CREDITS
    } pcie_fc_params_idx_t;

    typedef enum pcie_rx_dll_param_idx_e
    {
        PARAM_DLLP_TYPE,
        PARAM_DLLP_STATUS,
        PARAM_DLLP_VC,
        PARAM_DLLP_HDR_CREDITS,
        PARAM_DLLP_DATA_CREDITS,
        PARAM_DLLP_SEQ_NUM,
        PARAM_DLLP_VEND_DATA
    } pcie_rx_dll_param_idx__t;

    // -------------------------------
    // Public methods
    // -------------------------------

public:

    // Constructor
    pcieVcInterface (const unsigned nodeIn) : node (nodeIn)
                {
                    // Create a PCIe API object
                    pcie             = new pcieModelClass(nodeIn);

                    // Default the internal state member variables
                    reset_state      = 0;
                    link_width       = 0;
                    rid              = node;
                    pipe_mode        = PIPE_MODE_DISABLED;
                    ep_mode          = EP_MODE_DISABLED;
                    endian_mode      = LITTLE_END;
                    digest_mode      = DIGEST_MODE_DISABLED;
                    no_scramble_mode = SCRAMBLING_ENABLED;

                    cfgspc_offset    = 0;
                    mem_addr         = 0;
                    tag              = 0;

                    txdatabuf        = new PktData_t[DATABUFSIZE];
                };

    // Run main PCIe VC function
    void        run(void);


    // Run automatic EP mode
    void        runAutoEp(void);

   // -------------------------------
   // Private member variable
   // -------------------------------

private:

    uint32_t           node;
    pcieModelClass    *pcie;

    unsigned           tag;
    unsigned           reset_state;
    unsigned           link_width;
    unsigned           rid;
    unsigned           pipe_mode;
    unsigned           ep_mode;
    unsigned           digest_mode;
    unsigned           endian_mode;
    unsigned           no_scramble_mode;
    unsigned           cfgspc_offset;
    uint64_t           mem_addr;
    char               sbuf[STRBUFSIZE];
    pPktData_t         txdatabuf;

    // Queue for completion RX buffers, for use by input callback
    CplDataBufQueue_t  rxbufq;

    // Queue for arriving TLP requests, for use in input callback
    ReqBufQueue_t      reqbufq;

    // Queue for arriving DLLP packets,  for use by input callback
    DllBufQueue_t      dllbufq;

    // -------------------------------
    // Private methods
    // -------------------------------

private:

    // *EXAMPLE* Type 0 configuration setup for automatic EP model
    void        ConfigureType0PcieCfg (void);

    // Input data callback, passing in a packet, error status
    static void VUserInput    (pPkt_t pkt, int status, void* obj_instance);
    void        InputCallback (pPkt_t pkt, int status);

    // Input callback for automatic EP model (static so can be used as a callback).
    static void VUserInputAutoEp      (pPkt_t pkt, int status, void* usrptr);

};

#endif