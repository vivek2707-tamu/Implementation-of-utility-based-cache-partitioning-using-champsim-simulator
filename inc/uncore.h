#ifndef UNCORE_H
#define UNCORE_H

#include "champsim.h"
#include "cache.h"
#include "dram_controller.h"

class UNCORE {
  public:

    // LLC
    CACHE LLC{"LLC", LLC_SET, LLC_WAY, LLC_SET*LLC_WAY, LLC_WQ_SIZE, LLC_RQ_SIZE, LLC_PQ_SIZE, LLC_MSHR_SIZE};
    int32_t way_allocated[NUM_CPUS]={0}; //Initially all the CPUs are allocated zero ways.
    // DRAM
    MEMORY_CONTROLLER DRAM{"DRAM"}; 

    UNCORE(); 
   void Update_partition();
};

extern UNCORE uncore;

#endif
