// =========================================================================
//
//  File Name:         osvvm_cosim_skt.cpp
//  Design Unit Name:  osvvm_cosim_skt
//  Revision:          OSVVM MODELS STANDARD VERSION
//
//  Maintainer:        Simon Southwell email:  simon.southwell@gmail.com
//  Contributor(s):
//     Simon Southwell      simon.southwell@gmail.com
//
//
//  Description:
//      Defines methods for osvvm_cosim_skt class to support co-simulation
//      communications via TCP/IP socket
//
//  Revision History:
//    Date      Version    Description
//    10/2022   2023.01    Initial revision
//
//
//  This file is part of OSVVM.
//
//  Copyright (c) 2022 by [OSVVM Authors](../AUTHORS.md)
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

// -------------------------------------------------------------------------
// INCLUDES
// -------------------------------------------------------------------------

#include <signal.h>
#include <errno.h>
#include <fcntl.h>

// For Windows, need to link with Ws2_32.lib
#if defined (_WIN32) || defined (_WIN64)
// -------------------------------------------------------------------------
// INCLUDES (windows)
// -------------------------------------------------------------------------

# undef   UNICODE
# define  WIN32_LEAN_AND_MEAN

# include <windows.h>
# include <winsock2.h>
# include <ws2tcpip.h>

#else
// -------------------------------------------------------------------------
// INCLUDES (Linux)
// -------------------------------------------------------------------------

# include <string.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <termios.h>
#endif

#include "OsvvmCosim.h"
#include "OsvvmCosimSktHdr.h"
#include "OsvvmCosimSkt.h"

// -------------------------------------------------------------------------
// DEFINES
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
// LOCAL CONSTANTS
// -------------------------------------------------------------------------

const char OsvvmCosimSkt::HEXCHARS[HEX_BUF_SIZE] = "0123456789abcdef";

// -------------------------------------------------------------------------
// STATIC VARIABLES
// -------------------------------------------------------------------------

OsvvmCosimSkt::OsvvmCosimSkt (const int  NodeNum,
                              const int  PortNumber,
                              const bool LittleEndian,
                              const char Eop,
                              const char Sop,
                              const int  SfxBytes) :
    node(NodeNum),
    portnum(PortNumber),
    ack_char(GDB_ACK_CHAR),
    sop_char(Sop),
    eop_char(Eop),
    little_endian(LittleEndian),
    suffix_bytes(SfxBytes)
{

    if (init() < 0)
    {
        VPrint("osvvm_cosim_skt: ***ERROR initialising socket environment. Exiting...\n");
        exit(1);
    }

    // Create a TCP/IP socket
    if ((skt_hdl = connect_skt(portnum)) < 0)
    {
        VPrint("osvvm_cosim_skt: ***ERROR creating a TCP/IP socket. Exiting...\n");
        exit(2);
    }
}


// -------------------------------------------------------------------------
// OsvvmCosimSkt::init()
//
// Does any required socket initialisation, prior to opening a TCP socket.
// Current, only windows requires any handling.
//
// -------------------------------------------------------------------------

int OsvvmCosimSkt::init(void)
{
#if defined (_WIN32) || defined (_WIN64)
    WSADATA wsaData;

    // Initialize Winsock (windows only). Use windows socket spec. verions up to 2.2.
    if (int status = WSAStartup(MAKEWORD(VER_MAJOR, VER_MINOR), &wsaData))
    {
        VPrint("WSAStartup failed with error: %d\n", status);
        return OSVVM_COSIM_ERR;
    }
#endif

    return OSVVM_COSIM_OK;
}

// -------------------------------------------------------------------------
// OsvvmCosimSkt::connect_skt()
//
// Opens a TCP socket connection, suitable for remote debugging, on the
// given port number (portno). It listens for a single connection, before
// returning the connection handle established. If any error occurs,
// OSVVM_COSIM_ERR is returned instead.
//
// -------------------------------------------------------------------------

OsvvmCosimSkt::osvvm_cosim_skt_t OsvvmCosimSkt::connect_skt (const int portno)
{
    int enable = 1;

    // Create an IPv4 socket byte stream
    osvvm_cosim_skt_t svrskt;

    if ((svrskt = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0)
    {
        VPrint("ERROR opening socket\n");
        cleanup();
        return OSVVM_COSIM_ERR;
    }
    
    // Allow a waiting socket to be reused
    if (setsockopt(svrskt, SOL_SOCKET, SO_REUSEADDR, (char*)&enable, sizeof(int)) < 0)
    {
        VPrint("ERROR setting socket option\n");
        cleanup();
        return OSVVM_COSIM_ERR;
    }

    // Create and zero a server address structure
    struct sockaddr_in serv_addr;
    ZeroMemory((char *) &serv_addr, sizeof(serv_addr));

    // Configure the server address structure
    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port        = htons(portno);

    // Bind the socket to the address. Make up to 10 attempts on consecutive
    // different port numbers in case a port is already in use.
    int status, attempts;
    for (attempts = 0; attempts < 10; attempts++)
    {
        serv_addr.sin_port    = htons(portno + attempts);
        status = bind(svrskt, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
        if (status >= 0)
        {
            break;
        }
    }

    // If failed to bind to a port, cleanup and report the error
    if (status < 0)
    {
        VPrint("ERROR on Binding: %d\n", status);
        cleanup();
        return OSVVM_COSIM_ERR;
    }

    // Advertise the port number
    char errmsg[1024];

    sprintf(errmsg, "OSVVM_COSIM_SKT: Using TCP port number: %d\n", portno + attempts);
    VPrint(errmsg, portno);

    // Listen for connections (blocking)
    if (int status = listen(svrskt, MAXBACKLOG) < 0)
    {
        VPrint("ERROR on listening: %d\n", status);
        cleanup();
        return OSVVM_COSIM_ERR;
    }

    // Get a client address structure, and length as has to be passed as a pointer to accept()
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);

    // Accept a connection, and get returned handle
    osvvm_cosim_skt_t skt_hdl;
    if ((skt_hdl = accept(svrskt, (struct sockaddr *) &cli_addr,  &clilen)) < 0)
    {
        VPrint("ERROR on accept\n");
        cleanup();
        return OSVVM_COSIM_ERR;
    }

    // No longer need the server side (listening) socket
    closesocket(svrskt);

    // Return the handle to the connected socket. With this handle can
    // use recv()/send() to read and write (or, Linux only, read()/write()).
    return skt_hdl;
}

// -------------------------------------------------------------------------
// OsvvmCosimSkt::cleanup()
//
// Does any open TCP socket cleanup before exiting the program. Current,
// only windows requires any handling.
//
// -------------------------------------------------------------------------

inline void OsvvmCosimSkt::cleanup(void)
{
#if defined (_WIN32) || defined (_WIN64)
    WSACleanup();
#endif
}

// -------------------------------------------------------------------------
// OsvvmCosimSkt::read_cmd()
//
// Read a byte from the socket and place in the buffer (buf). Return true
// on successful read, else return false. Compile dependent for windows
// (ReadFile) and linux (read)
//
// -------------------------------------------------------------------------

inline bool OsvvmCosimSkt::read_cmd (const osvvm_cosim_skt_t skt_hdl, char* buf)
{
    int status = OSVVM_COSIM_OK;

    // Read from the connection (up to 255 bytes plus null termination).
    if (recv(skt_hdl, buf, 1, 0) < 0)
    {
        VPrint("ERROR reading from socket\n");
        cleanup();
        status = OSVVM_COSIM_ERR;
    }

    return status == OSVVM_COSIM_OK;
}

// -------------------------------------------------------------------------
// OsvvmCosimSkt::write_cmd()
//
// Write a byte to the socket from the buffer (buf). Return true
// on successful read, else return false. Compile dependent for windows
// (WriteFile) and linux (write)
//
// -------------------------------------------------------------------------

inline bool OsvvmCosimSkt::write_cmd (const osvvm_cosim_skt_t skt_hdl, const char* buf)
{
    int status = OSVVM_COSIM_OK;

    if (send(skt_hdl, buf, 1, 0) < 0)
    {
        VPrint("ERROR writing to socket\n");
        status = OSVVM_COSIM_ERR;
    }

    return status == OSVVM_COSIM_OK;
}

// -------------------------------------------------------------------------
// OsvvmCosimSkt::proc_cmd()
//
// Processes a single command, as stored in cmd_rec. The command is
// inspected and the appropriate local functions called. Generated replies
// are added to op_buf, with this function bracketing these with $ and #,
// followed by the two character checksum, returned by the functions (if
// any). An exception to a reply is for the kill (k) command which has
// no reply. Unsupported commands return a default reply of "$#00".
// The function sends the reply to the socket and then return either
// true if a 'detach' command (D) was seen, otherwise false.
//
// -------------------------------------------------------------------------

bool OsvvmCosimSkt::proc_cmd (CmdAttrType &cmd_rec)
{
    OsvvmCosim cosim(node);

    if (cmd_rec.Detach || cmd_rec.Kill)
    {
        return true;
    }
    else
    {
        if (cmd_rec.Rnw)
        {
            switch(cmd_rec.DataWidth)
            {
                uint32_t rdata32;
                uint16_t rdata16;
                uint8_t  rdata8;

                case 32: cosim.transRead((uint32_t)cmd_rec.Addr, (uint32_t*)&rdata32); cmd_rec.Data = rdata32; break;
                case 16: cosim.transRead((uint32_t)cmd_rec.Addr, (uint16_t*)&rdata16); cmd_rec.Data = rdata16; break;
                case  8: cosim.transRead((uint32_t)cmd_rec.Addr,  (uint8_t*)&rdata8);  cmd_rec.Data = rdata8;  break;
                default: cmd_rec.Error = OSVVM_COSIM_ERR; return true;
            }
        }
        else
        {
             switch(cmd_rec.DataWidth)
             {
                 case 32: cosim.transWrite((uint32_t)cmd_rec.Addr, (uint32_t)cmd_rec.Data); break;
                 case 16: cosim.transWrite((uint32_t)cmd_rec.Addr, (uint16_t)cmd_rec.Data); break;
                 case  8: cosim.transWrite((uint32_t)cmd_rec.Addr,  (uint8_t)cmd_rec.Data); break;
                 default: cmd_rec.Error = OSVVM_COSIM_ERR; return true;
             }
        }
    }

    return false;
}

// -------------------------------------------------------------------------
// OsvvmCosimSkt::ParsePkt ()
//
// Parse the packet passed in as cmdstr and contruct a protocol independent
// command record and return it for processing by the co-simulation code.
//
// -------------------------------------------------------------------------

OsvvmCosimSkt::CmdAttrType OsvvmCosimSkt::ParsePkt (const std::string cmdstr)
{
    CmdAttrType cmd_rec;
    int         cdx = 0;
    int         len = 0;
    uint8_t     byte;

    // Initialise the command record
    cmd_rec.Addr       = 0;
    cmd_rec.Data       = 0x0badc0de;
    cmd_rec.Detach     = false;
    cmd_rec.Kill       = false;
    cmd_rec.Error      = OSVVM_COSIM_OK;

    DebugVPrint("ParsePkt(): %s\n", cmdstr.c_str());

    // Skip the SOP
    cdx++;

    // Get command character
    char cmd = cmdstr.at(cdx++);

    if (cmd != 'D' && cmd != 'k')
    {

        // Get address
        while (cdx < cmdstr.length() && cmdstr.at(cdx) != ',')
        {
            if (cmdstr.at(cdx) != ' ')
            {
                cmd_rec.Addr <<= 4;
                cmd_rec.Addr  |= char2nib(cmdstr.at(cdx));
            }
            cdx++;
        }

        // Set address width based on value
        cmd_rec.AddrWidth = (cmd_rec.Addr > 0xffffffffULL) ? 64 : 32;

        // Skip comma and any spaces
        while (cmdstr.at(cdx) == ',' || cmdstr.at(cdx) == ' ')
        {
            cdx++;
        }

        // Get data length
        while (cdx < cmdstr.length() && cmdstr.at(cdx) != GDB_MEM_DELIM_CHAR && cmdstr.at(cdx) != eop_char)
        {
            len <<= 4;
            len  |= char2nib(cmdstr.at(cdx));
            cdx++;
        }

        cmd_rec.DataWidth = len * 8;
    }

    // Select on command character
    switch(cmd)
    {

    // Read memory
    case 'm':
        cmd_rec.Rnw        = true;
        break;

    // Write memory
    case 'M':

        cmd_rec.Rnw        = false;
        cmd_rec.Data       = 0;

        // Skip colon
        cdx++;

        // Get hex characters byte values and put into memory
        for (unsigned int idx = 0; idx < len; idx++)
        {
            cmd_rec.Data <<= 8;

            // Get byte value from hex
            uint8_t byte = 0;

            byte |= char2nib(cmdstr.at(cdx)) << 4; cdx++;
            byte |= char2nib(cmdstr.at(cdx));      cdx++;

            cmd_rec.Data |= byte;
        }
        break;

    case 'D':
        cmd_rec.Detach = true;
        break;

    case 'k':
        cmd_rec.Kill   = true;
        break;
    }

   DebugVPrint("%s: addr=%08llx awidth=%d, data=%08llx dwidth=%d detach=%d kill=%d error=%d\n",
               cmd_rec.Rnw ? "read " : "write:",
               cmd_rec.Addr, cmd_rec.AddrWidth,
               cmd_rec.Data, cmd_rec.DataWidth,
               cmd_rec.Detach, cmd_rec.Kill, cmd_rec.Error);

    return cmd_rec;
}

// -------------------------------------------------------------------------
// OsvvmCosimSkt::GenRespPkt()
//
// Generate a response packet based on the response record settings,
// returning a string with the complete response, including the
// acknowledgement.
//
// -------------------------------------------------------------------------

std::string OsvvmCosimSkt::GenRespPkt (const OsvvmCosimSkt::CmdAttrType Resp,
                                       const char                       SopByte,
                                       const char                       EopByte,
                                       const bool                       LittleEndian)
{
    std::string cmd;
    char nibble;

    unsigned char chksum = 0;

    if (!Resp.Error)
    {
        if (Resp.Detach || Resp.Kill)
        {
            if (Resp.Detach)
            {
                cmd.append("OK");
            }
        }
        if (!Resp.Rnw)
        {
            cmd.append("OK");
        }
        else
        {
            for (int idx = 0; idx < (Resp.DataWidth/8); idx++)
            {
                uint8_t byte;

                if (LittleEndian)
                {
                    byte = (Resp.Data >> (8*idx)) & 0xff; // Little endian
                }
                else
                {
                    byte = (Resp.Data >> (8*((Resp.DataWidth/8)-idx-1))) & 0xff; // Big endian
                }


               nibble = hihexchar(byte); cmd.push_back(nibble);
               nibble = lohexchar(byte); cmd.push_back(nibble);
            }
        }
    }
    else
    {
        // Send an error response
        cmd.append("E01");
    }

    // Calculate checksum
    for (int idx = 0; idx < cmd.length(); idx++)
    {
        chksum += cmd.at(idx);
    }

    // Prepend an acknowledgement and SOP
    char prefix[3];

    prefix[0] = ack_char;
    prefix[1] = SopByte;
    prefix[2] = '\0';
    cmd.insert(0, prefix);

    // Append an EOP
    nibble = EopByte; cmd.push_back(nibble);

    // Append the checksum
    nibble = hihexchar(chksum); cmd.push_back(nibble);
    nibble = lohexchar(chksum); cmd.push_back(nibble);

    DebugVPrint("GenRespPkt(): %s\n", cmd.c_str());

    return cmd;
}

// -------------------------------------------------------------------------
// OsvvmCosimSkt::fetch_next_pkt()
//
// Method to read a packet from the open socket in a generic way, using
// the sop_char and eop_char to delimit the packet, and the read any
// suffix bytes, as defined by suffix_bytes, all set at construction.
//
// -------------------------------------------------------------------------

int OsvvmCosimSkt::fetch_next_pkt(const OsvvmCosimSkt::osvvm_cosim_skt_t skt, std::string &cmdstr)
{
    bool  status;
    char ipbyte;

    cmdstr.clear();

    // Read bytes from socket, discarding bytes, until SOP
    do
    {
        if (!read_cmd(skt, &ipbyte))
        {
            return OSVVM_COSIM_ERR;
        }
    }
    while (ipbyte != sop_char);

    // Add the SOP to the command string
    cmdstr.push_back(ipbyte);

    // Read bytes from socket, adding bytes to string, until EOP
    do
    {
        if (!read_cmd(skt, &ipbyte))
        {
            return OSVVM_COSIM_ERR;
        }
        cmdstr.push_back(ipbyte);
    }
    while (ipbyte != eop_char);

    // Read bytes from socket, adding bytes to string for suffix bytes
    for (int idx = 0; idx < suffix_bytes; idx++)
    {
        if (!read_cmd(skt, &ipbyte))
        {
            return OSVVM_COSIM_ERR;
        }
        cmdstr.push_back(ipbyte);
    }

    return OSVVM_COSIM_OK;
}


// -------------------------------------------------------------------------
// OsvvmCosimSkt::process_pkt()
//
// Top level for the socket interface of OSVVM cosim features.
//
// It calls local functions to create a pseudo/virtual serial port for GDB
// connection, and starts reading characters for this port. It monitors
// for start and end of packets, placing packet contents in ip_buf. Once
// a whole packet is received, it calls osvvm_cosim_proc_cmd() to process
// it. This repeats until osvvm_cosim_proc_cmd() returns true, flagging
// that the GDB session has detached, when the function cleans up and
// returns. It will return OSVVM_COSIM_OK if all is well, else OSVVM_COSIM_ERR
// is returned.
//
// -------------------------------------------------------------------------

int OsvvmCosimSkt::ProcessPkts (void)
{
    int         idx      = 0;
    bool        active   = false;
    bool        detached = false;
    bool        waiting  = true;
    char        ipbyte;

    std::string cmdstr, respstr;
    CmdAttrType cmd_rec;

    while (!detached)
    {
        // If waiting for first communication, flag that attachment has happened.
        if (waiting)
        {
            waiting = false;
            VPrint("OSVVM_COSIM_SKT: host attached.\n");
            fflush(stderr);
        }
        else
        {
            // Fetch a whole packet and place in cmdstr
            int status = fetch_next_pkt (skt_hdl, cmdstr);

            // If an error occured, return with status
            if (status)
            {
                return status;
            }

            // Parse the packet in the command string and return
            // the transaction command record
            cmd_rec  = ParsePkt(cmdstr);

            // Process the command record with co-sim accesses to the OSVVM address bus manager transactor
            detached = proc_cmd(cmd_rec);

            // If not a kill command, send a response
            if (!cmd_rec.Kill)
            {
                // Generate a response from the command record
                respstr = GenRespPkt(cmd_rec, sop_char, eop_char, little_endian);

                DebugVPrint("respstr = %s (%d)\n", respstr.c_str(), respstr.length());

                // Send the response packet
                for (int idx = 0; idx < respstr.length(); idx++)
                {
                    char rchar = respstr.at(idx);
                    if (!write_cmd(skt_hdl, &rchar))
                    {
                        VPrint("OSVVM_COSIM_SKT: ERROR writing to host: terminating.\n");
                        return true;
                    }
                }
            }
        }
    }

    if (detached)
    {
        VPrint("OSVVM_COSIM_SKT: host %s from target: terminating.\n", cmd_rec.Kill ? "received 'kill'" : "detached");
    }
    else
    {
        VPrint("OSVVM_COSIM_SKT: connection lost to host: terminating.\n");
    }

    // Close socket of TCP connection
    closesocket(skt_hdl);

    return OSVVM_COSIM_OK;
}

