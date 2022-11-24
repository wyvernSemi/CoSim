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
#  Developed by:
#        SynthWorks Design Inc.
#        VHDL Training Classes
#        OSVVM Methodology and Model Library
#        11898 SW 128th Ave.  Tigard, Or  97223
#        http://www.SynthWorks.com
#
#  Revision History:
#    Date      Version    Description
#    10/2022   2022.10    Initial version
#
#
#  This file is part of OSVVM.
#
#  Copyright (c) 2022 by SynthWorks Design Inc.
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

# $MODEL_TECH environment variable must be set

# Define the maximum number of supported VProcs in the compile pli library
MAX_NUM_VPROC      = 64

SRCDIR             = code
USRCDIR            = usercode
OPDIR              = .
TESTDIR            = ${OPDIR}
VOBJDIR            = ${TESTDIR}/obj

MEMMODELDIR        = .

# VPROC C source code
VPROC_C            = $(wildcard ${SRCDIR}/*.c*)
VPROC_C_BASE       = $(notdir $(filter %c, ${VPROC_C}))
VPROC_CPP_BASE     = $(notdir $(filter %cpp, ${VPROC_C}))

# Memory model C code
MEM_C              =

# Test user code
USER_C             = $(wildcard ${USRCDIR}/*.c*)

USER_CPP_BASE      = $(notdir $(filter %cpp, ${USER_C}))
USER_C_BASE        = $(notdir $(filter %c, ${USER_C}))

SRC_INCL           = $(wildcard ${SRCDIR}/*.h)
USER_INCL          = $(wildcard ${USRCDIR}/*.h)

VOBJS              = $(addprefix ${VOBJDIR}/, ${VPROC_C_BASE:%.c=%.o} ${VPROC_CPP_BASE:%.cpp=%.o})
VUOBJS             = $(addprefix ${VOBJDIR}/, ${USER_C_BASE:%.c=%.o} ${USER_CPP_BASE:%.cpp=%.o})

USRFLAGS           =

SIM                = MODELSIM

ifeq ("${SIM}", "GHDL")
  ARCHFLAG           = -m64
  PLILIBARGS         = 
else
  ARCHFLAG           = -m32
  PLILIBARGS         = -L${MODEL_TECH} -lmtipli
endif

RV32EXE            = test.exe

ifeq ("${USRCDIR}", "tests/iss")
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
  RV32CMD          = cp tests/iss/test.exe ${OPDIR}
else
  CFLAGS_SO        = -shared -Wl,-export-all-symbols
  CPPSTD           =
  WLIB             = -lWs2_32
  RV32CMD          = copy tests/iss/test.exe ${OPDIR}
endif

CC                 = gcc
C++                = g++
CFLAGS             = -fPIC                                 \
                     ${ARCHFLAG}                           \
                     -g                                    \
                     -I${SRCDIR}                           \
                     -I${USRCDIR}                          \
                     -I${MODEL_TECH}/../include            \
                     -DVP_MAX_NODES=${MAX_NUM_VPROC}       \
                     -DMODELSIM                            \
                     -D_REENTRANT

#------------------------------------------------------
# BUILD RULES
#------------------------------------------------------

all: ${VPROC_PLI} ${VUSER_PLI}

${VOBJDIR}/%.o: ${SRCDIR}/%.c ${SRC_INCL}
	@${CC} -c ${CFLAGS} $< -o $@

${VOBJDIR}/%.o: ${SRCDIR}/%.cpp ${SRC_INCL}
	@${C++} -c ${CFLAGS} $< -o $@

${VOBJDIR}/%.o: ${USRCDIR}/%.c ${USER_INCL}
	@${CC} -Wno-write-strings -c ${CFLAGS} ${USRFLAGS}$< -o $@

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
            ${PLILIBARGS}                               \
            -L${TESTDIR} -lvproc                        \
            -Wl,-no-whole-archive                       \
            ${WLIB}                                     \
            -o $@

${VUSER_PLI}: ${VULIB} ${RV32TEST}
	@${C++} ${CPPSTD}                                   \
            ${CFLAGS_SO}                                \
            -Wl,-whole-archive                          \
            ${CFLAGS}                                   \
            -lpthread                                   \
            ${PLILIBARGS}                               \
            ${USRFLAGS}                                 \
            -L${TESTDIR} -lvuser                        \
            -Wl,-no-whole-archive                       \
            ${WLIB}                                     \
            -o $@

#------------------------------------------------------
# CLEANING RULES
#------------------------------------------------------

clean:
	@rm -rf ${VPROC_PLI} ${VUSER_PLI} ${VLIB} ${VULIB} ${VOBJS} ${VOBJDIR} ${RV32EXE}

