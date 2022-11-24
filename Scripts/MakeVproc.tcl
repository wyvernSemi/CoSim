#  File Name:         MakeVproc.tcl
#  Purpose:           Scripts for compiling cosimulation shared object
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
#    010/2022   2022.10    Initial version
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

# -------------------------------------------------------------------------
# -------------------------------------------------------------------------

proc mk_vproc_common {srcrootdir testname libname simname} {

if {"$libname" eq ""} {
  set flags ""
} else {
  set flags "-I ./include -L ./lib -l${libname}"
}

exec make --no-print-directory -C $srcrootdir        \
          SIM=$simname                               \
          USRCDIR=$testname                          \
          OPDIR=$::osvvm::CurrentSimulationDirectory \
          USRFLAGS=${flags}

}

# -------------------------------------------------------------------------
# mk_vproc_clean
#
#   Do a make clean on the VProc test directory
#
# -------------------------------------------------------------------------

proc mk_vproc_clean {srcrootdir testname} {
  exec make --no-print-directory -C $srcrootdir \
            USRCDIR=$testname OPDIR=$::osvvm::CurrentSimulationDirectory  clean
}

# -------------------------------------------------------------------------
# mk_vproc
#
#   Do a clean make compile for the specified VProc
#   test directory
#
# -------------------------------------------------------------------------

proc mk_vproc {srcrootdir testname {libname ""} } {

  mk_vproc_clean  $srcrootdir $testname
  mk_vproc_common $srcrootdir $testname $libname $::osvvm::ScriptBaseName
}

# -------------------------------------------------------------------------
# mk_vproc_noclean
#
#   Do a make compile for the VProc test directory
#   without a clean
#
# -------------------------------------------------------------------------

proc mk_vproc_noclean {srcrootdir testname {libname ""} {simname "MODELSIM"}} {

   mk_vproc_common $srcrootdir $testname $libname $::osvvm::ScriptBaseName
}

# -------------------------------------------------------------------------
# -------------------------------------------------------------------------

proc mk_vproc_skt {srcrootdir testname {libname ""} {simname "MODELSIM"} } {

  mk_vproc $srcrootdir $testname $libname
  
  set rc [ exec python3 $srcrootdir/Scripts/client_gui.py -b -w 2 -s $srcrootdir/$testname/sktscript.txt  > skt.log 2>@1 & ]
  puts "Running client_gui.py batch mode"
}


