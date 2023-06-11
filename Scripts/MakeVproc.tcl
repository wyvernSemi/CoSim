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
#  Revision History:
#    Date      Version    Description
#    10/2022   2023.01    Initial version
#
#
#  This file is part of OSVVM.
#
#  Copyright (c) 2022 by [OSVVM Authors](../AUTHORS.md)
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

namespace eval ::osvvm {

# -------------------------------------------------------------------------
# gen_lib_flags
#
# Generates the appropriate flags for a given OS if a library was specified
#
# -------------------------------------------------------------------------

proc gen_lib_flags {libname} {

  # Get the OS that we are running on
  # set osname [string tolower [exec uname]]
  set osname $::osvvm::OperatingSystemName

  # Select the RISC-V ISS library required
  if {$::osvvm::ToolName ne "ModelSim" } {
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

  return ${flags}
}

# -------------------------------------------------------------------------
# mk_vproc_common
#
# Common make operations that executes the make program
#
# -------------------------------------------------------------------------

proc mk_vproc_common {testname libname} {

  # Get the OS that we are running on
  set osname $::osvvm::OperatingSystemName

  # Default of no additional vendor specific flags
  set vendorflags "DUMMY="
  
  # Default to using the normal makefile
  set mkfilearg "makefile"
  
  # When an ALDEC simulator ...
  if {($::osvvm::ToolName eq "ActiveHDL") || ($::osvvm::ToolName eq "RivieraPRO") } {
    # If ActiveHDL, the choose its own makefile
    if {($::osvvm::ToolName eq "ActiveHDL")} {
      set mkfilearg "makefile.avhdl"
    }
    
    # Ensure the correct path to the ALDEC tools
    if {"$osname" eq "linux"} {
      set aldecpath [file normalize [info nameofexecutable]/../../..]
    } else {
      set aldecpath [file normalize [info nameofexecutable]/../..]
    }
    
    set vendorflags "ALDECDIR=${aldecpath}"
  }

  set flags [ gen_lib_flags ${libname} ]

  exec make --no-print-directory -C $::osvvm::OsvvmCoSimDirectory \
            -f $mkfilearg                                         \
            SIM=$::osvvm::ToolName                                \
            USRCDIR=$testname                                     \
            OPDIR=$::osvvm::CurrentSimulationDirectory            \
            USRFLAGS=${flags}                                     \
            $vendorflags

}

# -------------------------------------------------------------------------
# mk_vproc_clean
#
#   Do a make clean on the VProc test directory
#
# -------------------------------------------------------------------------

proc mk_vproc_clean {testname} {
  exec make --no-print-directory -C $::osvvm::OsvvmCoSimDirectory \
            USRCDIR=$testname OPDIR=$::osvvm::CurrentSimulationDirectory  clean
}

# -------------------------------------------------------------------------
# MkVproc
#
#   Do a clean make compile for the specified VProc
#   test directory
#
# -------------------------------------------------------------------------

proc MkVproc {testname {libname ""} } {

  puts "MkVproc $testname $libname"

  LocalMkVproc $testname $libname
}

proc LocalMkVproc {testname {libname ""} } {

  set NormTestPathName  [file normalize [file join ${::osvvm::CurrentWorkingDirectory} ${testname}]]

  mk_vproc_clean  $NormTestPathName
  mk_vproc_common $NormTestPathName $libname
}

# -------------------------------------------------------------------------
# MkVprocNoClean
#
#   Do a make compile for the VProc test directory
#   without a clean
#
# -------------------------------------------------------------------------

proc MkVprocNoClean {testname {libname ""}} {

  puts "MkVprocNoClean $testname $libname"

  set NormTestPathName  [file normalize [file join ${::osvvm::CurrentWorkingDirectory} ${testname}]]
  mk_vproc_common $NormTestPathName $libname
}

# -------------------------------------------------------------------------
# MkVprocSkt
#
# Do a clean make compile for the spceified VProc test directory
# and run the client_gui.py python script in batch mode in the background
#
# -------------------------------------------------------------------------

proc MkVprocSkt {testname {libname ""} } {

  puts "MkVprocSkt $testname $libname"

  LocalMkVproc $testname $libname

  if {$::osvvm::ToolName eq "NVC"} {
    set wait_time 10
  } else {
    set wait_time 2
  }

  puts "Running client_gui.py batch mode"
  set pid [exec python3 $::osvvm::OsvvmCoSimDirectory/Scripts/client_batch.py -w $wait_time -s $testname/sktscript.txt  > skt.log 2>@1 &]

  return
}

# -------------------------------------------------------------------------
# MkVprocGhdlMain
#
# Do a clean make specific to the callable GHDL environment
# using the wrapper makefile.ghdl, which will call makefile.
#
# -------------------------------------------------------------------------

proc MkVprocGhdlMain {testname {libname ""} } {

  puts "MkVprocGhdlMain $testname $libname"

  set ::env(LD_LIBRARY_PATH) ./

  exec make -f $::osvvm::OsvvmCoSimDirectory/makefile.ghdl clean

  set flags [ gen_lib_flags ${libname} ]

  exec make -f $::osvvm::OsvvmCoSimDirectory/makefile.ghdl                   \
            USRFLAGS=${flags}                                                \
            OSVVMDIR=$::osvvm::OsvvmHomeDirectory                            \
            TBLIBRARY=[string tolower $::osvvm::VhdlWorkingLibrary]          \
            VHDLLIB=[file normalize [FindLibraryPathByName $::osvvm::VhdlWorkingLibrary]]     \
            COSIMDIR=${::osvvm::OsvvmCoSimDirectory}                         \
            CTESTDIR=[file normalize [file join $::osvvm::CurrentWorkingDirectory $testname]] \
            TESTBENCH=TbAb_CoSim

  return
}


# use of namespace hides local proc
namespace export MkVproc MkVprocNoClean MkVprocSkt MkVprocGhdlMain

# end namespace ::osvvm
}
