// =========================================================================
//
//  File Name:         osvvm_cosim_skt.h
//  Design Unit Name:  osvvm_cosim_skt
//  Revision:          OSVVM MODELS STANDARD VERSION
//
//  Maintainer:        Simon Southwell email:  simon.southwell@gmail.com
//  Contributor(s):
//     Simon Southwell      simon.southwell@gmail.com
//
//
//  Description:
//      Defines  osvvm_cosim_skt class to support co-simulation
//      communications via TCP/IP socket
//
//  Developed by:
//        SynthWorks Design Inc.
//        VHDL Training Classes
//        http://www.SynthWorks.com
//
//  Revision History:
//    Date      Version    Description
//    10/2022   2022       Initial revision
//
//
//  This file is part of OSVVM.
//
//  Copyright (c) 2022 by SynthWorks Design Inc.
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

#ifndef _OSVVM_COSIM_SKT_H_
#define _OSVVM_COSIM_SKT_H_

// -------------------------------------------------------------------------
// INCLUDES
// -------------------------------------------------------------------------

#include <string>

#if defined (_WIN32) || defined (_WIN64)

#include <Winsock2.h>

#endif

#include "OsvvmCosimSktHdr.h"

// -------------------------------------------------------------------------
// CLASS DEFINITION
// -------------------------------------------------------------------------

class OsvvmCosimSkt
{
    ////////////////////////////////
    // PUBLIC
    ////////////////////////////////

public:
    // Public constants and type definitions
    // Note: Make external facing types and methods CamelCase to be consistent
    // with OSVVM convention
    
           static const int  OSVVM_COSIM_OK      = 0;
           static const int  OSVVM_COSIM_ERR     = -1;

           // Transaction command attribute record type
           typedef class CmdAttrClass
           {
            public:
               bool     Rnw;
               uint64_t Addr;
               int      AddrWidth;
               uint64_t Data;
               int      DataWidth;
               bool     Detach;
               bool     Kill;
               int      Error;

               CmdAttrClass() :
                   Addr       (0),
                   AddrWidth  (0),
                   Data       (0),
                   DataWidth  (32),
                   Detach     (false),
                   Kill       (false),
                   Error      (OSVVM_COSIM_OK)
               {
               };

           } CmdAttrType;

    // Constructor
                             OsvvmCosimSkt (const int  NodeNum      = 0,
                                            const int  PortNumber   = DEFAULT_TCP_PORTNUM,
                                            const bool LittleEndian = false,
                                            const char Eop          = GDB_EOP_CHAR,
                                            const char Sop          = GDB_SOP_CHAR,
                                            const int  SuffixBytes  = 2
                                            ) ;

    // User entry point method
           int               ProcessPkts   (void);

    ////////////////////////////////
    // PROTECTED
    ////////////////////////////////

protected:
   virtual CmdAttrType       ParsePkt      (const std::string CmdStr) ;
   virtual std::string       GenRespPkt    (const CmdAttrType Resp,
                                            const char        SopByte,
                                            const char        EopByte,
                                            const bool        LittleEndian) ;

    ////////////////////////////////
    // PRIVATE
    ////////////////////////////////

private:
    // Internal constants
           static const int  DEFAULT_TCP_PORTNUM = 0xc000;
           static const int  HEX_BUF_SIZE        = 100;
           static const char GDB_ACK_CHAR        = '+';
           static const char GDB_NAK_CHAR        = '-';
           static const char GDB_SOP_CHAR        = '$';
           static const char GDB_EOP_CHAR        = '#';
           static const char GDB_MEM_DELIM_CHAR  = ':';
           static const int  MAXBACKLOG          = 5;

           // Hexadecimal character LUT
                  const char HEXCHARS[HEX_BUF_SIZE] = "0123456789abcdef";

    // Internal type definition

#if defined (_WIN32) || defined (_WIN64)
           // Map the socket type for windows
           typedef SOCKET    osvvm_cosim_skt_t;
#else
           // Map the socket type for Linux
           typedef long long osvvm_cosim_skt_t;
#endif

    // Private methods

           // Methods for managing the socket connection
           int               init            (void);
           osvvm_cosim_skt_t connect_skt     (const int portno);
           void              cleanup         (void);

           // Methods for processing commands
           bool              proc_cmd        (CmdAttrType &cmd_rec);
           bool              read_cmd        (const osvvm_cosim_skt_t skt_hdl,       char* buf);
           bool              write_cmd       (const osvvm_cosim_skt_t skt_hdl, const char* buf);

           int               fetch_next_pkt  (const osvvm_cosim_skt_t skt, std::string &cmdstr);

           // Utility methods
    inline int               char2nib        (char x)
                             {
                                 return ((x >= '0' && x <= '9') ? x - '0'        :
                                         (x >= 'a' && x <= 'f') ? (10 + x - 'a') :
                                                                  (10 + x - 'A'));
                             }

    inline char              hihexchar       (unsigned x){ return HEXCHARS[(x & 0xf0) >> 4]; }
    inline char              lohexchar       (unsigned x){ return HEXCHARS[x & 0x0f]; }

    // Private member variables

           // TCP/IP connection state
           osvvm_cosim_skt_t skt_hdl;
    const  int               portnum;

           // Configuration state for packet protocol
    const  bool              little_endian;
    const  char              sop_char;
    const  char              eop_char;
    const  char              ack_char;
    const  int               suffix_bytes;
    const  int               node;

};

#endif
