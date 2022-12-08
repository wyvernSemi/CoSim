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

proc mk_vproc_common {srcrootdir testname libname} {

# Get the OS that we are running on
set osname [string tolower [exec uname]]

# Select the RISC-V ISS library required
if {$::osvvm::ToolName eq "GHDL" || $::osvvm::ToolName eq "NVC" || $::osvvm::ToolName eq "QuestaSim"} {

  if {"$osname" eq "linux"} {
    set rvlib ${libname}x64
  } else {
    set rvlib ${libname}win64
  }
} else {
  if {"$osname" eq "linux"} {
    set rvlib ${libname}
  } else {
    set rvlib ${libname}win32
  }
}

if {"$libname" eq ""} {
  set flags ""
} else {
  set flags "-I ./include -L ./lib -l${rvlib}"
}

exec make --no-print-directory -C $srcrootdir        \
          SIM=$::osvvm::ToolName                     \
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
  mk_vproc_common $srcrootdir $testname $libname
}

# -------------------------------------------------------------------------
# mk_vproc_noclean
#
#   Do a make compile for the VProc test directory
#   without a clean
#
# -------------------------------------------------------------------------

proc mk_vproc_noclean {srcrootdir testname {libname ""}} {

   mk_vproc_common $srcrootdir $testname $libname
}

# -------------------------------------------------------------------------
# mk_vproc_skt
#
# Do a clean make compile for the spceified VProc test directory
# and run the client_gui.py python script in batch mode in the background
#
# -------------------------------------------------------------------------

proc mk_vproc_skt {srcrootdir testname {libname ""} } {

  mk_vproc $srcrootdir $testname $libname
  
  set pathprefix [string range $srcrootdir 0 1]
  
  # In Windows, replace any leading c: with /c
  if {$pathprefix eq "c:" || $pathprefix eq "C:" } {
    set cosimdir [string replace $srcrootdir 0 1 "/c"]
  } else {
   set cosimdir $srcrootdir
  }
  
  if {$::osvvm::ToolName eq "NVC"} {
    set wait_time 10
  } else {
    set wait_time 2
  }
  
  puts "Running client_gui.py batch mode"
  set pid [exec python3 $srcrootdir/Scripts/client_batch.py -w $wait_time -s $srcrootdir/$testname/sktscript.txt  > skt.log 2>@1 &]
  
  return
}

# -------------------------------------------------------------------------
# analyzeForeignProcs
#
# Analyse the foreign procesure packages based on the running simulator 
#
# -------------------------------------------------------------------------
proc analyzeForeignProcs {} {

  if {$::osvvm::ToolName eq "NVC"} {
    analyze ../../../CoSim/src/OsvvmVprocNvcPkg.vhd
    SetExtendedRunOptions --load=./VProc.so
  } elseif {$::osvvm::ToolName eq "GHDL"} {
    analyze ../../../CoSim/src/OsvvmVprocGhdlPkg.vhd
  } else {
    analyze ../../../CoSim/src/OsvvmVprocPkg.vhd
  }
  
  analyze ../../../CoSim/src/OsvvmTestCoSimPkg.vhd
}

