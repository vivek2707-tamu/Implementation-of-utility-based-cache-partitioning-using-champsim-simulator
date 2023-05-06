
#include "auxiliary_tag_Directory.h"
#include "uncore.h"
#include "ooo_cpu.h"

int updateInterval = 5000000;
int lastRefreshCycle = 0;

void ATD::update_atd(PACKET *packet, int cpu, int write_back_hit) // function to update the Auxiliary Tag Directory for every LLC access.
{
    int set = uncore.LLC.get_set(packet->address); //Retrieving the set for the current packet     
    int current_cycle = 0;
    for (int i = 0; i < NUM_CPUS; i++)
    {
        if (current_core_cycle[i] > current_cycle)
        {
            current_cycle = current_core_cycle[i];
        }
    }
    if ((current_cycle - lastRefreshCycle) >= updateInterval)   //Partitioning is done once every 5 Million cycles
    {
        uncore.Update_partition();
        lastRefreshCycle = current_cycle;
    }
    if (set % SAMPLING_FREQUENCY == 0)
    {
        ATD *myTagDirectory = &ooo_cpu[cpu].TagDirectory;
        int mySet = set /SAMPLING_FREQUENCY;
        bool found = false;
        for (int j = 0; j < LLC_WAY; j++)
        {
            if (myTagDirectory->atd_block[mySet][j].valid == 1 && myTagDirectory->atd_block[mySet][j].tag == packet->address) //If the atd block is valid and the tag matches with the packet's tag.(Hit Case)
            {
                // hit block
                myTagDirectory->umon_global_hit_Counter[myTagDirectory->atd_block[mySet][j].lru]++;  //Updating the UMON hit counter for that way.
                if (write_back_hit == 1)
                {
                    return;
                }
                for (int way = 0; way < LLC_WAY; way++)//Incrementing the LRU of all the blocks whose LRU is less than the HIT block
                {
                    if (myTagDirectory->atd_block[mySet][j].lru > myTagDirectory->atd_block[mySet][way].lru)
                    {
                        myTagDirectory->atd_block[mySet][way].lru++;
                    }
                }
                myTagDirectory->atd_block[mySet][j].lru = 0; //Set the hit block to most recent position
		found = true;
                break;
            }
            else if (!found && myTagDirectory->atd_block[mySet][j].valid == 0) //If the atd block is invalid and there is no block whose tag is matched
            {
                // miss block with at least one valid bit being 0
                myTagDirectory->atd_block[mySet][j].lru = LLC_WAY;
                myTagDirectory->atd_block[mySet][j].tag = packet->address;
                myTagDirectory->atd_block[mySet][j].valid = 1;
                for (int way = 0; way < LLC_WAY; way++)//Incrementing the LRU of all the blocks whose LRU is less than this block
                {
                    if (myTagDirectory->atd_block[mySet][way].lru < myTagDirectory->atd_block[mySet][j].lru)
                    {
                        myTagDirectory->atd_block[mySet][way].lru++;
                    }
                }
                myTagDirectory->atd_block[mySet][j].lru = 0;//set the block to most recent position
                found = true;
                break;
            }
            else if (!found && myTagDirectory->atd_block[mySet][j].lru == LLC_WAY - 1)//If all the blocks are valid and no block matches the packet's tag
            {
                // miss block with all valid bits being 1
                myTagDirectory->atd_block[mySet][j].tag = packet->address;
                for (int way = 0; way < LLC_WAY; way++)
                {
                    if (myTagDirectory->atd_block[mySet][way].lru < myTagDirectory-> atd_block[mySet][j].lru)//Incrementing the LRU of all the blocks whose LRU is less than this block
			{
				myTagDirectory->atd_block[mySet][way].lru++;

			}
		}
			myTagDirectory->atd_block[mySet][j].lru = 0;//set the block to most recent position
                  found=true;
                  break;
             }
	}
    }
}
