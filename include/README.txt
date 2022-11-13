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
            USRCDIR=$OSVVMDIR/CoSim/iss> \
            USRFLAGS="-I $OSVVMDIR/CoSim/include -L $OSVVMDIR/CoSim/lib -lrv32"

