#  File Name:         makefile
#  Purpose:           make file for compiling RISC-V demonstration code
#  Revision:          OSVVM MODELS STANDARD VERSION
#
#  Maintainer:        Simon Southwell email:  simon@gmail.com
#  Contributor(s):
#     Simon Southwell     email:  simon.southwell@gmail.com
#
#  Description
#    Make file supporting compiling of Co-simuation C/C++ code
#    using make
#
#  Revision History:
#    Date      Version    Description
#    07/2026   ????.??    Initial version
#
#  This file is part of OSVVM.
#
#  Copyright (c) 2026 by [OSVVM Authors](AUTHORS.md)
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      https://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
# --------------------------------------------------------------------------

OUTPREFIX  = test
SRCFILES   = crt0.S ../slzw.c test.c
INCLFILES  = message.h

TOOLPREFIX = riscv64-unknown-elf-
CC         = $(TOOLPREFIX)gcc
C++        = $(TOOLPREFIX)g++
OBJCOPY    = $(TOOLPREFIX)objcopy

RVARCHOPT  = -march=rv32imfdc_zicsr
RVABIOPT   = -mabi=ilp32

CFLAGS     = -nostartfiles -DRV32_ISS -I ../
LDFLAGS    = -Wl,-T rv32.ld

# ---------------------------------------------------------------------
# BUILD RULES
# ---------------------------------------------------------------------

.PHONY: all, clean

all: $(OUTPREFIX).bin

$(OUTPREFIX).exe: $(SRCFILES) $(INCLFILES)
	@$(CC) -g $(RVARCHOPT) $(RVABIOPT) $(CFLAGS) $(SRCFILES) $(LDFLAGS) -o $(OUTPREFIX).exe

$(OUTPREFIX).bin: $(OUTPREFIX).exe
	@$(OBJCOPY) -O binary $< $@

clean:
	@rm -rf $(OUTPREFIX).exe $(OUTPREFIX).bin
