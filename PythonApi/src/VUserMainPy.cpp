// =========================================================================
//
//  File Name:         VUserMainPy.c
//  Design Unit Name:
//  Revision:          OSVVM MODELS STANDARD VERSION
//
//  Maintainer:        Simon Southwell email:  simon.southwell@gmail.com
//  Contributor(s):
//     Simon Southwell      simon.southwell@gmail.com
//
//
//  Description:
//      Python OSVVM co-simulation top level C enrty point
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

#define PYSLEEPFOREVER       {while(1) PyTick(0x7fffffff, false, false, node);}

static void VUserMain(int node)
{
    printf("VUserMain%d\n", node);
    
    int status = RunPython(node);

    if (status)
    {
        printf("***ERROR: RunPython(%d) returned error status %d\n", node, status);
    }

    PYSLEEPFOREVER;
}

extern "C"
{
void VUserMain0()  {VUserMain(0);}
void VUserMain1()  {VUserMain(1);}
void VUserMain2()  {VUserMain(2);}
void VUserMain3()  {VUserMain(3);}
void VUserMain4()  {VUserMain(4);}
void VUserMain5()  {VUserMain(5);}
void VUserMain6()  {VUserMain(6);}
void VUserMain7()  {VUserMain(7);}
void VUserMain8()  {VUserMain(8);}
void VUserMain9()  {VUserMain(9);}
void VUserMain10() {VUserMain(10);}
void VUserMain11() {VUserMain(11);}
void VUserMain12() {VUserMain(12);}
void VUserMain13() {VUserMain(13);}
void VUserMain14() {VUserMain(14);}
void VUserMain15() {VUserMain(15);}
}