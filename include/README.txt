The include files in this folder are an abbreviated set of headers for using
the librv32.a library (Linux) containing the ROSC-V RV32 ISS. The full source
code is available on github at:

    https://github.com/wyvernSemi/riscV/iss.

To use the ISS, the co-simulation code must include rv32.h and link the library
(-lrv32) in CoSim/lib.

If the gdb remote target wrapper interface is to be used then rv32_cpu_gdb.h
must also be included. Then, in place of a call to the model's run() method---
pCpu->run(cfg)---a call to the gdb wrapper function is made:

    rv32gdb_process_gdb(pCpu, cfg.gdb_ip_portnum, cfg);

This will open a TCP/IP socket and listen for the gdb remote target connection.
The makefile in CoSim can be called to link the ISS library as follows (e.g. in
a bash script):

    #!/bin/bash
    OSVVMDIR=<absolute path>/OsvvmLibraries
    SIMDIR=`pwd`
    
    make -C $OSVVMDIR/CoSim              \
            OPDIR=$SIMDIR                \
            USRCDIR=$OSVVMDIR/CoSim/iss  \
            USRFLAGS="-I $OSVVMDIR/CoSim/include -L $OSVVMDIR/CoSim/lib -lrv32"

#### Copyright and License

Copyright (C) 2020 - 2021 by [OSVVM Authors](AUTHORS.md)   

This file is part of OSVVM.

    Licensed under Apache License, Version 2.0 (the "License")
    You may not use this file except in compliance with the License.
    You may obtain a copy of the License at

  [http://www.apache.org/licenses/LICENSE-2.0](http://www.apache.org/licenses/LICENSE-2.0)

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
