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
//      Simple LZW codec software header
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

int compress   (const char* const ibuf, char *obuf, const int len);
int decompress (const char* const ibuf, char *obuf, const int len);
