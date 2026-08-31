//
//  File Name:           slzw.c
//  Design Unit Name:
//  Revision:            OSVVM MODELS STANDARD VERSION
//
//  Maintainer:          Simon Southwell  email: simon.southwell@gmail.com
//  Contributor(s):
//     Simon Southwell  simon.southwell@gmail.com
//
//
//  Description:
//      Simple LZW codec software (non-reentrant)
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

#include <stdio.h>

// ------------------------------------------------------------
// DEFINES
// ------------------------------------------------------------

#define TRUE                (1==1)
#define FALSE               (1==0)
#define BITSIZE             1
#define BYTESIZE            8
#define BYTEMASK            ((1<<BYTESIZE) - 1)
#define MINCWLEN            9
#ifndef MAXCWLEN
#define MAXCWLEN            10
#endif
#define DICTFULL            (1 << MAXCWLEN)
#define CODEWORDMASK        (DICTFULL - 1)
#define MAXDICTSIZE         DICTFULL
#define NOMATCH             DICTFULL
#define NULLCW              DICTFULL
#define FIRSTCW             0x100
#define NOERRROR            0
#define DECOMPRESSION_ERROR -1

// ------------------------------------------------------------
// TYPEDEFS
// ------------------------------------------------------------

typedef unsigned int   codeword_type;
typedef unsigned char  byte_type;
typedef unsigned char  boolean;

typedef struct
{
    codeword_type pointer;
    byte_type     byte;
} dictionary_entry_type;

// ------------------------------------------------------------
// LOCAL STATIC STATE
// ------------------------------------------------------------

static dictionary_entry_type dictionary        [DICTFULL];
static codeword_type         indirection_table [DICTFULL][256];
static codeword_type         next_available_codeword;
static unsigned int          codeword_len;

static byte_type             stack[MAXDICTSIZE];
static unsigned int          stack_pointer = 0;

static unsigned long         barrel;
static int                   residue;

// ------------------------------------------------------------
// reset_state
//
// Reset the local state
//
// ------------------------------------------------------------

void reset_state()
{
    next_available_codeword = FIRSTCW;
    codeword_len            = MINCWLEN;
    barrel                  = 0;
    residue                 = 0;
    stack_pointer           = 0;
}

// ------------------------------------------------------------
// entry_match
//
// Look for a match in the dictionary
//
// ------------------------------------------------------------

static codeword_type entry_match (
    codeword_type pointer,
    byte_type     byte)
{
    codeword_type addr;

    // Fetch the address stored in the indirection table
    addr = indirection_table[pointer][byte];

    // If address outside of the current valid dictionary
    // then no match
    if (addr < FIRSTCW || addr >= next_available_codeword)
    {
        return NOMATCH;
    }
    
    // If the dictionary entry at the address matches the pointer
    // and by passed in, the return a match
    if ((dictionary[addr].byte == byte) && (dictionary[addr].pointer == pointer))
    {
        return addr;
    }

    // The default is no match
    return NOMATCH;
}

// ------------------------------------------------------------
// build_entry
//
// Build a new entry in the dictionary
//
// ------------------------------------------------------------

static unsigned int build_entry (
    codeword_type codeword,
    byte_type     byte,
    boolean       compress_mode
)
{
    // If the next available codeword is at the end of the dictionary
    // then reset the dictionary before building a new entry
    if (next_available_codeword == DICTFULL)
    {
        next_available_codeword = FIRSTCW;
        codeword_len            = MINCWLEN;
        return codeword_len;
    }

    // Update the dictionary entry pointed to by the next available
    // codeword with the new values
    dictionary[next_available_codeword].pointer = codeword;
    dictionary[next_available_codeword].byte    = byte;

    // In compression, update the indirection table, dereferenced by
    // the codeword and byte, with the next available code word value
    if (compress_mode)
    {
        indirection_table[codeword][byte] = next_available_codeword;
    }

    // If not already at maximum...
    if (codeword_len < MAXCWLEN)
    {
        // If the next available codeword size is greater then the codeword length
        // bits (less one in decompress), bump up the codeword length
        if (next_available_codeword == (1U << codeword_len) - (!compress_mode ? 1U : 0U))
        {
            codeword_len++;
        }
    }
    // If decompressing, codeword length at maximum and next available codeword
    // is at the last valid dictionary entry, reset the codeword length one
    // entry earlier than for compression
    else if (!compress_mode && next_available_codeword == (DICTFULL-1))
    {
        codeword_len = MINCWLEN;
    }

    // Increment the next available codeword
    ++next_available_codeword;

    return codeword_len;
}

// ------------------------------------------------------------
// pack
//
// Pack fixed with codeword to variable width output
//
// ------------------------------------------------------------

static void pack (
    codeword_type ip_codeword,
    unsigned int  codelen,
    boolean       flush,
    char          *obuf,
    int           *oidx
)
{
    // Add the new codeword above residue bits on the barrel shifter
    barrel  |= ((ip_codeword & CODEWORDMASK) << residue);
    
    // Add the codeword length to the barrel residue bit count
    residue += codelen;

    // Flush out bytes while byte sized bits on the barrel
    while (residue >= (flush ? BITSIZE : BYTESIZE))
    {
        // Add byte from barrel bottom to output buffer
        obuf[(*oidx)++] = barrel & BYTEMASK;
        
        // Remove byte from barrel
        barrel        >>= BYTESIZE;
        
        // Reduce residue count by a byte's worth
        residue        -= BYTESIZE;
    }
}

// ------------------------------------------------------------
// unpack
//
// Unpack variable width data to fixed width codewords
//
// ------------------------------------------------------------

static int unpack (codeword_type *codeword, unsigned int codelen, const char* ibuf, int idx)
{
    // Capture the input buffer index
    int                 bidx    = idx;

    // While codeword length number of bits on the barrel shifter...
    while (residue < codelen)
    {
        // Add byte from input buffer above the residue bits on the barrel
        barrel  |= (ibuf[bidx++] & BYTEMASK) << residue;
        
        // Add a byte size number of bits to the barrel residue count
        residue += BYTESIZE;
    }

    // Export a codeword length size of bits from the bottom of the barrel
    *codeword   = (barrel & ((1 << codelen) - 1));
    
    // Reduce the residue count by a codeword length number of bits
    residue    -= codelen;
    
    // Remove the codeword from the bottom of the barrel
    barrel    >>= codelen;

    // Return the current input buffer index 
    return bidx;
}

// ------------------------------------------------------------
// decompress
//
// Decompress input buffer data to output buffer
//
// ------------------------------------------------------------

int decompress (
    const char* const ibuf,
    char*             obuf,
    const int         len
)
{
    codeword_type ip_codeword   = NULLCW;
    codeword_type prev_codeword = NULLCW;
    unsigned int  code_size     = MINCWLEN;
    byte_type     byte;
    codeword_type pointer;
    int           oidx          = 0;
    int           idx           = 0;

    // Reset the internal state for each new decompression
    reset_state();

    // Loop for input buffer's length
    while (idx < len)
    {
        // Unpack a new codeword from the input buffer and update index
        // with consumed byte count
        idx = unpack(&ip_codeword, code_size, ibuf, idx);

        // If codeword is in dictionary...
        if (ip_codeword <= next_available_codeword)
        {
            // Set a pointer as the input codeword
            pointer = ip_codeword;

            // Walk through the linked list of dictionary entries until NULL
            while (pointer != NULLCW)
            {
                // If a dynamic dictionary entry...
                if (pointer >= FIRSTCW)
                {
                    // If KwK case detected, make pointer the previous codeword
                    if (pointer == next_available_codeword && (prev_codeword != NULLCW))
                    {
                        pointer = prev_codeword;
                    }
                    // For normal case, fetch the byte and pointer from the dictionary
                    // updating a byte value and moving pointer down the linked list.
                    else
                    {
                        byte    = dictionary[pointer].byte;
                        pointer = dictionary[pointer].pointer;
                    }
                }
                // If a fixed dictionary entry (for byte values), the pointer
                // is the bye value, and pointer is set to NULL for the ned of the
                // list
                else
                {
                    byte    = pointer;
                    pointer = NULLCW;
                }
                
                // Add the byte to the stack
                stack[stack_pointer++] = byte;
            }

            // Output the contents of the stack in reverse order
            while (stack_pointer != 0)
            {
                obuf[oidx++] = stack[--stack_pointer];
            }

            // Add a new entry in the dictionary if the previous codeword
            // valid, with the byte value and the previous codeword
            if (prev_codeword != NULLCW)
            {
                code_size = build_entry(prev_codeword, byte, FALSE);
            }
            
            // The previous codeword becomes the codeword just processed
            prev_codeword = ip_codeword;
        }
        // Decoded codeword outside of the valid dictionary
        else
        {
            return DECOMPRESSION_ERROR;
        }
    }

    // Return the number of bytes decompressed
    return oidx;
}

// ------------------------------------------------------------
// compress
//
// Compress input buffer data to output buffer
//
// ------------------------------------------------------------

int compress (
  const char* const ibuf,
  char*             obuf,
  const int         len
)
{
    codeword_type previous_codeword = NULLCW;
    unsigned int  code_size         = MINCWLEN;
    unsigned int  oidx              = 0;

    codeword_type match_addr;
    int           ipbyte;

    // Reset the internal state for each new compression
    reset_state();

    // Loop over all the input bytes
    for (int idx = 0; idx < len; idx++)
    {
        // Fetch the byte from the input buffer
        ipbyte = ibuf[idx];
        
        // If the previous codeword is NULL, make it the input byte value
        if (previous_codeword == NULLCW)
        {
            previous_codeword = ipbyte;
        }
        // When a valid previous codeword...
        else
        {
            // If no match is found in the dictionary for previous codeword and input byte...
            if ((match_addr = entry_match(previous_codeword, ipbyte)) == NOMATCH)
            {
                // Pack the code_size codeword into bytes and end to output buffer
                pack(previous_codeword, code_size, FALSE, obuf, &oidx);

                // Build a new entry in the dictionary with previous codeword and input byte,
                // updating the code size
                code_size         = build_entry(previous_codeword, ipbyte, TRUE);
                
                // Set the previous codeword to be the input byte value
                previous_codeword = ipbyte;
            }
            // If a match was found in the dictionary make the previous codeword
            // the returned match address
            else
            {
                previous_codeword = match_addr;
            }
        }
    }

    // Flush any residue bits left on the pack barrel shifter to the output buffer
    pack (previous_codeword, code_size, TRUE, obuf, &oidx);

    // Return the compressed byte count
    return oidx;
}

