// =========================================================================
//
//  File Name:         OsvvmPcieAdpater.cpp
//  Design Unit Name:
//  Revision:          OSVVM MODELS STANDARD VERSION
//
//  Maintainer:        Simon Southwell email:  simon.southwell@gmail.com
//  Contributor(s):
//     Simon Southwell      simon.southwell@gmail.com
//
//
//  Description:
//      Adapter for PCIe model code from VProc API to OSVVM calls
//
//  Revision History:
//    Date      Version    Description
//    07/2025   2025.??    Initial revision
//
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

#include <stdint.h>
#include "OsvvmCosim.h"

#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif

static OsvvmCosim *pcie[VP_MAX_NODES] = {{NULL}};

// -------------------------------------------------------------------------
// VProc style word write function to OSVVM co-sim write transaction call
// -------------------------------------------------------------------------

EXTERN int VWrite (unsigned int addr, unsigned int data, int delta, unsigned int node)
{
    int rdata = 0;

    // Check if an OSVVM co-sim API for this node and create if not
    if (pcie[node] == NULL)
    {
        pcie[node] = new OsvvmCosim(node);
    }

    // Do an asynchronous word write if delta set, else a normal write
    if (delta)
    {
        rdata = pcie[node]->transWriteAsync(addr, (uint32_t)data);
    }
    else
    {
        rdata = pcie[node]->transWrite(addr, (uint32_t)data);
    }

    return rdata;
}

// -------------------------------------------------------------------------
// VProc style word read function to OSVVM co-sim read transaction call
// -------------------------------------------------------------------------

EXTERN int VRead (unsigned int addr, unsigned int *data, int delta, unsigned int node)
{
    // Check if an OSVVM co-sim API for this node and create if not
    if (pcie[node] == NULL)
    {
        pcie[node] = new OsvvmCosim(node);
    }

    // Do a word read
    pcie[node]->transRead(addr, (uint32_t*)data);

    return 0;
}

// -------------------------------------------------------------------------
// VProc style word write function to OSVVM co-sim write transaction call
// for 64-bits
// -------------------------------------------------------------------------

uint64_t VWrite64 (uint64_t addr, uint64_t data, int delta, unsigned int node)
{
    uint64_t rdata = 0;

    // Check if an OSVVM co-sim API for this node and create if not
    if (pcie[node] == NULL)
    {
        pcie[node] = new OsvvmCosim(node);
    }

    // Do an asynchronous word write if delta set, else a normal write
    if (delta)
    {
        rdata = pcie[node]->transWriteAsync(addr, data);
    }
    else
    {
        rdata = pcie[node]->transWrite(addr, data);
    }

    return rdata;
}

// -------------------------------------------------------------------------
// VProc style word read function to OSVVM co-sim read transaction call
// for 64-bits
// -------------------------------------------------------------------------

int VRead64(uint64_t addr, uint64_t *data, int delta, unsigned int node)
{
    // Check if an OSVVM co-sim API for this node and create if not
    if (pcie[node] == NULL)
    {
        pcie[node] = new OsvvmCosim(node);
    }

    // Do a word read
    pcie[node]->transRead(addr, data);

    return 0;
}
