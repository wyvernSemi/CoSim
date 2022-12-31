#  File Name:         makefile
#  Purpose:           make file for compiling cosimulation shared object
#  Revision:          OSVVM MODELS STANDARD VERSION
#
#  Maintainer:        Simon Southwell email:  simon@gmail.com
#  Contributor(s):
#     Simon Southwell     email:  simon.southwell@gmail.com
#
#  Description
#    Tcl procedures supporting compiling of Co-cimuation C/C++ code
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
#   USRCDIR     : Directory, relative to OsvvmLibraries/CoSim, where the test directory is located
#   OPDIR       : Directory for compilation output
#   USRFLAGS    : Additional user defined compile and link flags
#   SIM         : The target simulator. One of GHDL, NVC, QuestaSim, or ModelSim
#
# --------------------------------------------------------------------------

USRCDIR            = usercode
OPDIR              = .
USRFLAGS           =
SIM                = ModelSim

# Derived directory locations
SRCDIR             = code
TESTDIR            = ${OPDIR}
VOBJDIR            = ${TESTDIR}/obj

# VPROC C source code
VPROC_C            = $(wildcard ${SRCDIR}/*.c*)
VPROC_C_BASE       = $(notdir $(filter %c, ${VPROC_C}))
VPROC_CPP_BASE     = $(notdir $(filter %cpp, ${VPROC_C}))


# Test user code
USER_C             = $(wildcard ${USRCDIR}/*.c*)
EXCLFILE           = 
USER_CPP_BASE      = $(patsubst ${EXCLFILE},,$(notdir $(filter %cpp, ${USER_C})))
USER_C_BASE        = $(notdir $(filter %c, ${USER_C}))

SRC_INCL           = $(wildcard ${SRCDIR}/*.h)
USER_INCL          = $(wildcard ${USRCDIR}/*.h)

VOBJS              = $(addprefix ${VOBJDIR}/, ${VPROC_C_BASE:%.c=%.o} ${VPROC_CPP_BASE:%.cpp=%.o})
VUOBJS             = $(addprefix ${VOBJDIR}/, ${USER_C_BASE:%.c=%.o} ${USER_CPP_BASE:%.cpp=%.o})

ifeq ("${SIM}", "GHDL")
  ARCHFLAG         = -m64
else ifeq ("${SIM}", "NVC")
  ARCHFLAG         = -m64
else ifeq ("${SIM}", "QuestaSim")
  ARCHFLAG         = -m64
else
  ARCHFLAG         = -m32
endif

RV32EXE            = test.exe
RV32CMD            = cp ${USRCDIR}/test.exe ${OPDIR}

ifneq ("$(wildcard ${USRCDIR}/test.exe)", "")
  RV32TEST = ${RV32EXE}
else
  RV32TEST = dummy
endif

# Generated  PLI C library
VPROC_PLI          = ${OPDIR}/VProc.so
VLIB               = ${TESTDIR}/libvproc.a

VUSER_PLI          = ${OPDIR}/VUser.so
VULIB              = ${TESTDIR}/libvuser.a

# Get OS type
OSTYPE:=$(shell uname)

# Set OS specific variables between Linux and Windows (MinGW)
ifeq (${OSTYPE}, Linux)
  CFLAGS_SO        = -shared -lpthread -lrt -rdynamic
  CPPSTD           = -std=c++11
  WLIB             =
else
  CFLAGS_SO        = -shared -Wl,-export-all-symbols
  CPPSTD           =
  WLIB             = -lWs2_32 -l:vproc.so
endif

# Define the maximum number of supported VProcs in the compile pli library
MAX_NUM_VPROC      = 16

CC                 = gcc
C++                = g++
CFLAGS             = -fPIC                                 \
                     ${ARCHFLAG}                           \
                     -g                                    \
                     -I${SRCDIR}                           \
                     -I${USRCDIR}                          \
                     -DVP_MAX_NODES=${MAX_NUM_VPROC}       \
                     -DMODELSIM                            \
                     -D_REENTRANT

#------------------------------------------------------
# BUILD RULES
#------------------------------------------------------

all: ${VPROC_PLI} ${VUSER_PLI}

${VOBJDIR}/%.o: ${SRCDIR}/%.c ${SRC_INCL}
	@${CC} -c ${CFLAGS} ${USRFLAGS} $< -o $@

${VOBJDIR}/%.o: ${SRCDIR}/%.cpp ${SRC_INCL}
	@${C++} -c ${CFLAGS} ${USRFLAGS} $< -o $@

${VOBJDIR}/%.o: ${USRCDIR}/%.c ${USER_INCL}
	@${CC} -Wno-write-strings -c ${CFLAGS} ${USRFLAGS} $< -o $@

${VOBJDIR}/%.o: ${USRCDIR}/%.cpp ${USER_INCL}
	@${C++} ${CPPSTD} -Wno-write-strings -c ${CFLAGS} ${USRFLAGS} $< -o $@

${VLIB} : ${VOBJS} ${VOBJDIR}
	@ar cr ${VLIB} ${VOBJS}

${VULIB} : ${VUOBJS} ${VOBJDIR}
	@ar cr ${VULIB} ${VUOBJS}

${VOBJS}: | ${VOBJDIR}

${VUOBJS}: | ${VOBJDIR}

${VOBJDIR}:
	@mkdir ${VOBJDIR}

${OPDIR}:
	@mkdir ${OPDIR}

${RV32EXE}:
	@${RV32CMD}

.PHONY: dummy
dummy:

${VPROC_PLI}: ${VLIB}
	@${C++} ${CPPSTD}                                   \
            ${CFLAGS_SO}                                \
            -Wl,-whole-archive                          \
            ${CFLAGS}                                   \
            -lpthread                                   \
            -L${TESTDIR} -lvproc                        \
            -Wl,-no-whole-archive                       \
            ${WLIB}                                     \
            -ldl                                        \
            -o $@

${VUSER_PLI}: ${VULIB} ${RV32TEST}
	@${C++} ${CPPSTD}                                   \
            ${CFLAGS_SO}                                \
            -Wl,-whole-archive                          \
            ${CFLAGS}                                   \
            -lpthread                                   \
            ${USRFLAGS}                                 \
            -L${TESTDIR} -lvuser                        \
            -Wl,-no-whole-archive                       \
            ${WLIB}                                     \
            -ldl                                        \
            -o $@

#------------------------------------------------------
# CLEANING RULES
#------------------------------------------------------

clean:
	@rm -rf ${VPROC_PLI} ${VUSER_PLI} ${VLIB} ${VULIB} ${VOBJS} ${VOBJDIR} ${RV32EXE}

