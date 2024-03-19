// =========================================================================
//
//  File Name:         OsvvmPython.cpp
//  Design Unit Name:
//  Revision:          OSVVM MODELS STANDARD VERSION
//
//  Maintainer:        Simon Southwell email:  simon.southwell@gmail.com
//  Contributor(s):
//     Simon Southwell      simon.southwell@gmail.com
//
//
//  Description:
//      Python OSVVM co-simulation C API functions
//
//  Revision History:
//    Date      Version    Description
//    03/2024   2024.??    Initial revision
//
//
//  This file is part of OSVVM.
//
//  Copyright (c) 2024 by [OSVVM Authors](../../AUTHORS.md)
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

#include "OsvvmPython.h"

// Pointer types for external API functions. Must match prototypes in VUser.h
typedef uint8_t  (*vtranscommon_8_32_t)  (const int, uint32_t *, const uint8_t,  int*, const int, const uint32_t);
typedef uint16_t (*vtranscommon_16_32_t) (const int, uint32_t *, const uint16_t, int*, const int, const uint32_t);
typedef uint32_t (*vtranscommon_32_32_t) (const int, uint32_t *, const uint32_t, int*, const int, const uint32_t);
typedef uint8_t  (*vtranscommon_8_64_t)  (const int, uint64_t *, const uint8_t,  int*, const int, const uint32_t);
typedef uint16_t (*vtranscommon_16_64_t) (const int, uint64_t *, const uint16_t, int*, const int, const uint32_t);
typedef uint32_t (*vtranscommon_32_64_t) (const int, uint64_t *, const uint32_t, int*, const int, const uint32_t);
typedef uint64_t (*vtranscommon_64_64_t) (const int, uint64_t *, const uint64_t, int*, const int, const uint32_t);

typedef void (*setnamefunc_t) (const uint8_t*, const uint32_t, const uint32_t);
typedef void (*regirqfunc_t)  (const pVUserInt_t, const unsigned);
typedef int  (*tkfunc_t)      (const uint32_t, const bool, const bool, const uint32_t);

// Define pointers to the external API functions
static vtranscommon_8_32_t  VTransCommon_8_32;
static vtranscommon_16_32_t VTransCommon_16_32;
static vtranscommon_32_32_t VTransCommon_32_32;
static vtranscommon_8_64_t  VTransCommon_8_64;
static vtranscommon_16_64_t VTransCommon_16_64;
static vtranscommon_32_64_t VTransCommon_32_64;
static vtranscommon_64_64_t VTransCommon_64_64;

static setnamefunc_t VsetTestName;
static tkfunc_t      Vtick;
static regirqfunc_t  VregIrq;

// ------------------------------------------------------------
// Function to load VProc shared object and create bindings to
// the API functions
//
// Returns 0 on success, else 1
//
// ------------------------------------------------------------

static int BindToApiFuncs(void)
{
    void*   hdl  = dlopen("VProc.so", RTLD_NOW | RTLD_GLOBAL);

    if (hdl == NULL)
    {
        fprintf(stderr, "***ERROR: failed to load shared object VProc.so\n");
        return 1;
    }

    if ((VTransCommon_8_32 = (vtranscommon_8_32_t)dlsym(hdl, "OsvvmPyTransCommon_8_32")) == NULL)
    {
        fprintf(stderr, "***ERROR: failed to find symbol OsvvmPyTransCommon_8_32\n");
        return 1;
    }

    if ((VTransCommon_16_32 = (vtranscommon_16_32_t)dlsym(hdl, "OsvvmPyTransCommon_16_32")) == NULL)
    {
        fprintf(stderr, "***ERROR: failed to find symbol OsvvmPyTransCommon_16_32\n");
        return 1;
    }

    if ((VTransCommon_32_32 = (vtranscommon_32_32_t)dlsym(hdl, "OsvvmPyTransCommon_32_32")) == NULL)
    {
        fprintf(stderr, "***ERROR: failed to find symbol OsvvmPyTransCommon_32_32\n");
        return 1;
    }

    if ((VTransCommon_8_64 = (vtranscommon_8_64_t)dlsym(hdl, "OsvvmPyTransCommon_8_64")) == NULL)
    {
        fprintf(stderr, "***ERROR: failed to find symbol OsvvmPyTransCommon_8_64\n");
        return 1;
    }

    if ((VTransCommon_16_64 = (vtranscommon_16_64_t)dlsym(hdl, "OsvvmPyTransCommon_16_64")) == NULL)
    {
        fprintf(stderr, "***ERROR: failed to find symbol OsvvmPyTransCommon_16_64\n");
        return 1;
    }

    if ((VTransCommon_32_64 = (vtranscommon_32_64_t)dlsym(hdl, "OsvvmPyTransCommon_32_64")) == NULL)
    {
        fprintf(stderr, "***ERROR: failed to find symbol OsvvmPyTransCommon_32_64\n");
        return 1;
    }

    if ((VTransCommon_64_64 = (vtranscommon_64_64_t)dlsym(hdl, "OsvvmPyTransCommon_64_64")) == NULL)
    {
        fprintf(stderr, "***ERROR: failed to find symbol OsvvmPyTransCommon_64_64\n");
        return 1;
    }


    if ((Vtick = (tkfunc_t)dlsym(hdl, "OsvvmPyTick")) == NULL)
    {
        fprintf(stderr, "***ERROR: failed to find symbol OsvvmPyTick\n");
        return 1;
    }

    if ((VregIrq = (regirqfunc_t)dlsym(hdl, "OsvvmPyRegIrq")) == NULL)
    {
        fprintf(stderr, "***ERROR: failed to find symbol OsvvmPyRegIrq\n");
        return 1;
    }
    
    if ((VsetTestName = (setnamefunc_t)dlsym(hdl, "OsvvmSetTestName")) == NULL)
    {
        fprintf(stderr, "***ERROR: failed to find symbol OsvvmSetTestName\n");
        return 1;
    }

    return 0;
}

// ------------------------------------------------------------
// Function to load and run a user supplied VUserMainX python
// function to match the node number (e.g. VUserMain0)
//
// Returns 0 on success
//
// ------------------------------------------------------------

extern "C" int RunPython(const int node)
{
    PyObject *pName, *pModule, *pFunc;
    PyObject *pValue;
    char strbuf[DEFAULTSTRBUFSIZE];
    int status;
    int rtnval = 0;

    if (status = BindToApiFuncs())
    {
        return status;
    }

    Py_Initialize();

    void*   hdl  = dlopen("VUser.so", RTLD_NOW | RTLD_GLOBAL);

    if (hdl == NULL)
    {
        fprintf(stderr, "***ERROR: failed to load shared object VUser.so\n");
        return 1;
    }

    sprintf(strbuf, "VUserMain%d", node);

    pName = PyUnicode_DecodeFSDefault(strbuf);

    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule != NULL)
    {
        pFunc = PyObject_GetAttrString(pModule, strbuf);

        if (pFunc && PyCallable_Check(pFunc))
        {
            pValue = PyObject_CallObject(pFunc, NULL);

            if (pValue != NULL)
            {
                rtnval =  PyLong_AsLong(pValue);
                Py_DECREF(pValue);
            }
            else
            {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                fprintf(stderr,"***Error: RunPython() : Call failed\n");
                return 1;
            }
        }
        else
        {
            if (PyErr_Occurred())
            {
                PyErr_Print();
            }

            fprintf(stderr, "***Error: RunPython() : Cannot find function \"%s\"\n", strbuf);
        }

        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    }
    else
    {
        PyErr_Print();
        fprintf(stderr, "***Error: RunPython() : Failed to load \"%s\"\n", strbuf);
        return 1;
    }

    if (Py_FinalizeEx() < 0)
    {
        return 120;
    }

    return rtnval;
}

// Functions callable from Python

// ------------------------------------------------------------
// External print function alternative for when Python print
// output not displayed on the console (E.g. Questa on Linux).
// ------------------------------------------------------------

extern "C" uint32_t PyPrint(const char* str)
{
    printf("%s\n", str);
    return 0;
}

// ------------------------------------------------------------
// VWrite wrapper function for Python
// ------------------------------------------------------------

extern "C" uint32_t PyTransWrite (const uint32_t addr, const uint32_t data, const uint32_t node)
{
    int dummystatus;
    uint32_t localaddr = addr;

    return VTransCommon_32_32(WRITE_OP, &localaddr, data, &dummystatus, 0, node);
}

// ------------------------------------------------------------
// VRead wrapper function for Python
// ------------------------------------------------------------

extern "C" uint32_t PyTransRead (const uint32_t addr, const uint32_t node)
{
    uint32_t rdata;
    int      dummystatus;
    uint32_t localaddr = addr;

    rdata = VTransCommon_32_32(READ_OP, &localaddr, 0, &dummystatus, 0, node);

    return /*(uint64_t)*/rdata;
}


// ------------------------------------------------------------
// VBurstWrite wrapper function for Python
// ------------------------------------------------------------

extern "C" void PyTransBurstWrite (const uint32_t addr, void *data, const uint32_t bytesize, const uint32_t node)
{
    VTransBurstCommon(WRITE_BURST, BURST_NORM, addr, (uint8_t*)data, bytesize, 0, node);
}

// ------------------------------------------------------------
// VBurstRead wrapper function for Python
// ------------------------------------------------------------

extern "C" void PyTransBurstRead (const uint32_t addr, void *data, const uint32_t bytesize, const uint32_t node)
{
    VTransBurstCommon(READ_BURST, BURST_NORM, addr, (uint8_t*)data, bytesize, 0, node);
}


// ------------------------------------------------------------
// VTick wrapper function for Python
// ------------------------------------------------------------

extern "C" uint32_t PyTick (const uint32_t ticks, const bool done, const bool error, const uint32_t node)
{
    //printf("PyTick: ticks=%d node=%d\n", ticks, node);

    return Vtick(ticks, done, error, node);
}

// ------------------------------------------------------------
// VRegIrq wrapper function for Python
// ------------------------------------------------------------

extern "C" uint32_t PyRegIrq (const pVUserInt_t func, const uint32_t node)
{
    //printf("PyRegIrq %p %d\n", func, node);

    VregIrq(func, node);

    return 0;
}

// ------------------------------------------------------------
// VSetTestName wrapper function for python
// ------------------------------------------------------------
void PySetTestName (const uint8_t *data, const uint32_t bytesize, const uint32_t node)
{
    VsetTestName(data, bytesize, node);
}