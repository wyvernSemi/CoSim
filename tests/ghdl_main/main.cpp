#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>

#include "OsvvmCosim.h"

extern "C" int ghdl_main (int argc, char **argv);
extern "C" int VUserMain0 ();

static int    node = 0;
static int    largc;
static char** largv;

// -----------------------------------------------------
// -----------------------------------------------------
static int run_sim (int dummy)
{
    int status = ghdl_main(largc, largv);

    printf("ghdl_main returned\n");

    return status;
}

// -----------------------------------------------------
// -----------------------------------------------------
static int stop_sim_thread (int dummy)
{
  OsvvmCosim cosim(node);

  // This call will hang because the simulation will not return
  // a respoinse since it was stopped.
  cosim.tick(1, true, false);

  return 0;
}

// -----------------------------------------------------
// -----------------------------------------------------
static void stop_sim(void)
{
    pthread_t stop_thread;
    pthread_create(&stop_thread, NULL, (pThreadFunc_t)stop_sim_thread, (void *)((long long)node));
}

// =====================================================
// ===================== M A I N =======================
// =====================================================

int main (int argc, char **argv)
{
    OsvvmCosim cosim(node);
    pthread_t sim_thread;

    printf("Calling GHDL\n");

    // Export arguments for use by thread code
    largc = argc;
    largv = argv;

    // Create a thread for calling GHDL simulator
    pthread_create(&sim_thread, NULL, (pThreadFunc_t)run_sim, (void *)((long long)node));

    // Allow the simulation's co-simulation code to initialise and be ready
    // for the first API call.
    cosim.waitForSim();

    // Do some tests, calling the VUserMain0 code directly.
    VUserMain0();

    printf("Simulation completed\n");

    // Post a done status to the simulator.
    stop_sim();

    // Wait for the simulator to exit and the thread to complete.
    pthread_join(sim_thread, NULL);

    printf("Thread completed\n");

    return 0;
}
