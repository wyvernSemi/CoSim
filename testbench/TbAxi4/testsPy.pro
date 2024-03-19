library    osvvm_CoSim_TbAxi4

ChangeWorkingDirectory ../../tests

MkVproc    ../PythonApi/src python $CurrentWorkingDirectory/testpy
TestName   CoSim_python
SetDebugMode true
SetLogSignals true
SetSaveWaves true
simulate   TbAb_CoSim [CoSim]