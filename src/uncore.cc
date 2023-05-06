#include "uncore.h"
#include "ooo_cpu.h"
// uncore
UNCORE uncore;

// constructor
UNCORE::UNCORE() {
    for (int j=0;j<NUM_CPUS;j++) //Initially total number of ways are equally distributed among the CPUs
    {
        way_allocated[j]=LLC_WAY/NUM_CPUS;
    }
}

void UNCORE:: Update_partition() // Implementation of LookAhead Algorithm
{
    int Balance = LLC_WAY-NUM_CPUS;
    for (int Core =0;Core<NUM_CPUS;Core++)   //Initially each CPU is allocated atleast one block
    {
        way_allocated[Core]=1;
    }
    
    int Way_Required[NUM_CPUS];
    float Max_Utility[NUM_CPUS];
    
    while (Balance>0)
    {
        float Current_Utility=-1;
        int Way=0;
        int Max_Utility_Core;  //Defines the core with the Maximum Utility
        
        for (int Core=0;Core<NUM_CPUS;Core++)
        {
            int Alloc = way_allocated[Core];
            Max_Utility[Core]=-1;
            
            float Sum=0;
            int Cnt=0;
            
            for (int j=Alloc;j<(Alloc+Balance);j++) //Finding the Maximum Marginal Utility by allocating the ways iteratively
            {
                Sum+=ooo_cpu[Core].TagDirectory.umon_global_hit_Counter[j];
                Cnt+=1;
                if (Sum/Cnt>Max_Utility[Core])
                {
                    Max_Utility[Core]=Sum/Cnt;
                    Way_Required[Core]=Cnt;
                }
            }
            
            if (Current_Utility<Max_Utility[Core])
            {
                Current_Utility=Max_Utility[Core];
                Max_Utility_Core=Core;
                Way=Way_Required[Core];
            }
        }
        way_allocated[Max_Utility_Core]+=Way; //Allocating the ways to that core which gets maximum marginal Utility among others
        Balance-=Way; //Updating the remaining balance after each allocation
        
    }
    for (int Core=0;Core<NUM_CPUS;Core++)
    {
        for (int j=0;j<LLC_WAY;j++)   //After each partitioning interval , the hit counters are halved in all UMONs to retain the past information,also giving importance torecent information
        {
            ooo_cpu[Core].TagDirectory.umon_global_hit_Counter[j]/=2;
        }
    }
}
