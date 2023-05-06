#ifndef ATD_H
#define ATD_H
#include "champsim.h"
#include "cache.h"
#define SAMPLING_FREQUENCY 32

//ATD BLOCK
 class ATDBlock
 {
 public:
 uint64_t tag;
 uint32_t valid, lru;
 ATDBlock()//Constructor
 {
	 valid=0; tag=0; lru=0;
 };
 };

class ATD
{
public:
ATDBlock **atd_block;
int umon_global_hit_Counter[LLC_WAY]; //Counter to count number of hits in each way of LLC
ATD()
{
int atd_sets=1+LLC_SET/SAMPLING_FREQUENCY;
atd_block=new ATDBlock *[atd_sets];// Assigning memory to each set in the Tag Directory
for(int j=0;j<atd_sets;j++)
{
 atd_block[j]=new ATDBlock[LLC_WAY];//Assigning memory to each block in a set
}

for(int j=0;j<LLC_WAY;j++)
{
umon_global_hit_Counter[j]=0;//Initially all UMON global hit counters are zero
}
};
void update_atd(PACKET *packet, int cpu, int write_back_hit);  //calling the update_atd function for every LLC access
};
#endif
