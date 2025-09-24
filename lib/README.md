# The rv32 RISC-V ISS

This folder conatins a static library (Linux) for the RISC-V RV32 ISS which can
be linked with the user co-simulation code and used within the VUserMain0
environment. The required headers are in CoSim/include. The ISS is an RV32
specification model, with GB_Zbc standard extensions. Full [documentation](https://github.com/wyvernSemi/riscV/tree/main/iss/doc/iss_manual.pdf)
is available in the github repository:

[https://github.com/wyvernSemi/riscV/tree/main/iss](https://github.com/wyvernSemi/riscV/tree/main/iss)

The library also includes a gdb remote target interface to allow connection to
gdb, and any IDE that can be configured to use gdb, with all the debug
facilities of such a CAD tool. The Eclipse IDE has been tested with the RISC-V
toochain and gdb executable.

To use the ISS, the co-simulation code must include rv32.h and link the library
`(-lrv32`) in `CoSim/lib`. If the gdb remote target wrapper interface is to be used
then `rv32_cpu_gdb.h` must also be included. Then, in place of a call to the
model's `run()` method&mdash;`pCpu->run(cfg)`&mdash;a call to the gdb wrapper function is
made instead:

```
    rv32gdb_process_gdb(pCpu, portnum, cfg);
```

This will open a TCP/IP socket and listen for the gdb remote target connection.
The `makefile` in the `CoSim` directory can be called to link the ISS library as follows (e.g. in
a bash script):

```
#!/bin/bash
OSVVMDIR=<absolute path>/OsvvmLibraries
SIMDIR=`pwd`
    
make -C $OSVVMDIR/CoSim              \
        OPDIR=$SIMDIR                \
        USRCDIR=$OSVVMDIR/CoSim/iss  \
        USRFLAGS="-I $OSVVMDIR/CoSim/include -L $OSVVMDIR/CoSim/lib -lrv32"
```

## Copyright and License

Copyright &copy; 2020 - 2025 by [OSVVM Authors](AUTHORS.md)   

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
            