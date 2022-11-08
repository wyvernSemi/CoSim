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
#include "OsvvmCosimSktHdr.h"

#if defined (_WIN32) || defined (_WIN64)

#include <Winsock2.h>

#endif

// -------------------------------------------------------------------------
// CLASS DEFINITION
// -------------------------------------------------------------------------

class osvvm_cosim_skt
{
public:
           static const int  OSVVM_COSIM_OK      = 0;
           static const int  OSVVM_COSIM_ERR     = -1;

private:
           static const int  default_tcp_portnum = 0xc000;
           static const int  ip_buffer_size      = 1024;
           static const int  op_buffer_size      = 1024;
           static const int  str_buffer_size     = 100;

public:
    // Constructor
                             osvvm_cosim_skt (const int  port_number       = default_tcp_portnum,
                                              const bool le                = false) ;

    // User entry point method
           int               process_pkts    (void);

private:

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
           bool              proc_cmd        (const osvvm_cosim_skt_t skt_hdl, const char* cmd, const int cmdlen);
           bool              read_cmd        (const osvvm_cosim_skt_t skt_hdl, char* buf);
           bool              write_cmd       (const osvvm_cosim_skt_t skt_hdl, const char* buf);

           // Methods to do the memory mapped accesses
           int               read_mem        (const char* cmd, const int cmdlen, char *buf, unsigned char &checksum);
           int               write_mem       (const osvvm_cosim_skt_t skt_hdl,   const char* cmd, const int cmdlen, char *buf,
                                              unsigned char &checksum, const bool is_binary);

    // Private member variables
           bool              rcvd_kill;
           char              ip_buf[ip_buffer_size];
           char              op_buf[op_buffer_size];
           osvvm_cosim_skt_t skt_hdl;
    const  int               portnum;
    const  char              ack_char;
           char              sop_char;
           char              eop_char;
           char              hexchars[str_buffer_size];

           bool              little_endian;

};

#endif
