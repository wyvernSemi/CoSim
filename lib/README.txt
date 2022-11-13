This folder conatins a static library (Linux) for the RISC-V RV32 ISS which can
be linked with the user co-simulation code and used within the VUserMain0
environment. The required headers are in CoSim/include. The ISS is an RV32G
specification model. Full documentation is available in the github repository:

    https://github.com/wyvernSemi/riscV/iss.

The library also includes a gdb remote target interface to allow connection to
gdb, and any IDE that can be configured to use gdb, with all the debug
facilities of such a CAD tool. The Eclipse IDE has been tested with the RISC-V
toochain and gdb executable.

To use the ISS, the co-simulation code must include rv32.h and link the library
(-lrv32) in CoSim/lib. If the gdb remote target wrapper interface is to be used
then rv32_cpu_gdb.h must also be included. Then, in place of a call to the
model's run() method---pCpu->run(cfg)---a call to the gdb wrapper function is
made instead:

    rv32gdb_process_gdb(pCpu, portnum, cfg);

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