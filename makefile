#  File Name:         makefile
#  Purpose:           make file for compiling cosimulation shared object
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
#    10/2022   2023.01    Initial version
#
#  This file is part of OSVVM.
#
#  Copyright (c) 2022 by [OSVVM Authors](AUTHORS.md)
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
#
# User overridable variables: make VAR=NEW_VALUE ...
#
#   USRCDIR     : Directory where the test source directory is located
#   OPDIR       : Directory for compilation output
#   USRFLAGS    : Additional user defined compile and link flags
#   SIM         : The target simulator. One of GHDL, NVC, RivieraPRO,
#                 QuestaSim, or ModelSim
#   ALDECDIR    : Location of RivieraPRO installation, when selected by SIM
#
# --------------------------------------------------------------------------

USRCDIR            = usercode
OPDIR              = .
USRFLAGS           =
SIM                = ModelSim


# Get OS type
OSTYPE:=$(shell uname)

# Derived directory locations
SRCDIR             = code
TESTDIR            = $(OPDIR)
VOBJDIR            = $(TESTDIR)/obj
PCIEDIR            = ../PCIe

# Derive correct PCIe co-sim library
ifneq ($(OSTYPE), Linux)
  ifeq ("$(SIM)", "ModelSim")
    PCIELIB=pcie_win32
  else
    PCIELIB=pcie_win64
  endif
else
  ifeq ("$(SIM)", "ModelSim")
    PCIELIB=pcie_lnx32
  else
    PCIELIB=pcie_lnx64
  endif
endif 

# VPROC C source code
VPROC_C            = $(wildcard $(SRCDIR)/*.c*)
VPROC_C_BASE       = $(notdir $(filter %c, $(VPROC_C)))
VPROC_CPP_BASE     = $(notdir $(filter %cpp, $(VPROC_C)))

# Test user code
USER_C             = $(wildcard $(USRCDIR)/*.c*)
EXCLFILE           =
USER_CPP_BASE      = $(patsubst $(EXCLFILE),,$(notdir $(filter %cpp, $(USER_C))))
USER_C_BASE        = $(notdir $(filter %c, $(USER_C)))

SRC_INCL           = $(wildcard $(SRCDIR)/*.h)
USER_INCL          = $(wildcard $(USRCDIR)/*.h)

VOBJS              = $(addprefix $(VOBJDIR)/, $(VPROC_C_BASE:%.c=%.o) $(VPROC_CPP_BASE:%.cpp=%.o))
VUOBJS             = $(addprefix $(VOBJDIR)/, $(USER_C_BASE:%.c=%.o)  $(USER_CPP_BASE:%.cpp=%.o))

VPROCLIBSUFFIX     = so

TOOLFLAGS          = -m64

ifeq ("$(SIM)", "QuestaSim")
  TOOLFLAGS        += -DSIEMENS
else ifeq ("$(SIM)", "RivieraPRO")
  ALDECDIR         =  /c/Aldec/Riviera-PRO-2022.10-x64
  TOOLFLAGS        += -DALDEC -I$(ALDECDIR)/interfaces/include
  ifeq ($(OSTYPE), Linux)
    TOOLFLAGS      += -L$(ALDECDIR)/bin -laldecpli
  else
    TOOLFLAGS      += -L$(ALDECDIR)/interfaces/lib -l:aldecpli.lib
  endif
else ifeq ("$(SIM)", "ModelSim")
  TOOLFLAGS        = -m32 -DSIEMENS
endif

RV32EXE            = test.exe
RV32CMD            = cp $(USRCDIR)/test.exe $(OPDIR)

ifneq ("$(wildcard $(USRCDIR)/test.exe)", "")
  RV32TEST = $(RV32EXE)
else
  RV32TEST = dummy
endif

# Generated  PLI C library
VPROC_PLI          = $(OPDIR)/VProc.$(VPROCLIBSUFFIX)
VLIB               = $(TESTDIR)/libvproc.a

VUSER_PLI          = $(OPDIR)/VUser.so
VULIB              = $(TESTDIR)/libvuser.a

# Set OS specific variables between Linux and Windows (MinGW)
ifeq ($(OSTYPE), Linux)
  CFLAGS_SO        = -shared -lpthread -lrt -rdynamic
  CPPSTD           = -std=c++11
  WLIB             =
else
  CFLAGS_SO        = -shared -Wl,-export-all-symbols
  CPPSTD           =
  WLIB             = -lWs2_32 -l:vproc.$(VPROCLIBSUFFIX)
endif

# Define the maximum number of supported VProcs in the compile pli library
MAX_NUM_VPROC      = 16

CC                 = gcc
C++                = g++
CFLAGS             = -fPIC                                 \
                     $(TOOLFLAGS)                          \
                     -g                                    \
                     -I$(SRCDIR)                           \
                     -I$(USRCDIR)                          \
                     -I$(PCIEDIR)/include                  \
                     -DVP_MAX_NODES=$(MAX_NUM_VPROC)       \
                     -DOSVVM

#------------------------------------------------------
# BUILD RULES
#------------------------------------------------------

all: $(VPROC_PLI) $(VUSER_PLI)

$(VOBJDIR)/%.o: $(SRCDIR)/%.cpp $(SRC_INCL)
	@$(C++) $(CPPSTD) -c $(CFLAGS) $(USRFLAGS) $< -o $@

$(VOBJDIR)/%.o: $(USRCDIR)/%.c $(USER_INCL)
	@$(CC) -Wno-write-strings -c $(CFLAGS) $(USRFLAGS) $< -o $@

$(VOBJDIR)/%.o: $(USRCDIR)/%.cpp $(USER_INCL)
	@$(C++) $(CPPSTD) -Wno-write-strings -c $(CFLAGS) $(USRFLAGS) $< -o $@

$(VLIB) : $(VOBJS) $(VOBJDIR)
	@ar cr $(VLIB) $(VOBJS)

$(VULIB) : $(VUOBJS) $(VOBJDIR)
	@ar cr $(VULIB) $(VUOBJS)

$(VOBJS): | $(VOBJDIR)

$(VUOBJS): | $(VOBJDIR)

$(VOBJDIR):
	@mkdir $(VOBJDIR)

$(OPDIR):
	@mkdir $(OPDIR)

$(RV32EXE):
	@$(RV32CMD)

.PHONY: dummy
dummy:

$(VPROC_PLI): $(VLIB)
	@$(C++) $(CPPSTD)                                   \
            $(CFLAGS_SO)                                \
            -Wl,-whole-archive                          \
            $(CFLAGS)                                   \
            -L$(TESTDIR) -lvproc                        \
            -L$(PCIEDIR)/lib -l$(PCIELIB)               \
            -Wl,-no-whole-archive                       \
            $(WLIB)                                     \
            -ldl                                        \
            -o $@

$(VUSER_PLI): $(VULIB) $(RV32TEST)
	@$(C++) $(CPPSTD)                                   \
            $(CFLAGS_SO)                                \
            -Wl,-whole-archive                          \
            $(CFLAGS)                                   \
            $(USRFLAGS)                                 \
            -L$(TESTDIR) -lvuser                        \
            -Wl,-no-whole-archive                       \
            $(WLIB)                                     \
            -ldl                                        \
            -o $@

#------------------------------------------------------
# CLEANING RULES
#------------------------------------------------------

clean:
	@rm -rf $(VPROC_PLI) $(VUSER_PLI) $(VLIB) $(VULIB) $(VOBJS) $(VOBJDIR) $(RV32EXE)

