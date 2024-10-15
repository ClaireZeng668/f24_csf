//cache related logic implementation

#include "cache.h"
#include <cstdint>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <cmath>

//have timestamp for entire program, just find smallest timestamp
//write though - 100 cycles
//default constructor for struct

Cache::Cache(int sets, int blocks, int blockSize, const std::string& writeAllocate, const std::string& writePolicy, const std::string& evictionPolicy)
    : sets(sets), blocks(blocks), blockSize(blockSize), writeAllocate(writeAllocate), writePolicy(writePolicy), evictionPolicy(evictionPolicy),
      totalLoads(0), totalStores(0), loadHits(0), loadMisses(0), storeHits(0), storeMisses(0), totalCycles(0), time(0) {
        std::vector<Set> set_vec;
    }


void Cache::load(unsigned int address) {
    totalLoads++;
    time++;
    int index = get_index_bits(address);
    int tag = get_tag_bits(address);

    //write back, wrte allocate

    //load hit - exists in cache - 1 cycle
    //load miss - doesnt exists, find smallest ts, check validity, 
        //if valid, get rid, pull from memory (100), load to cache (1), update ts, hit, misses, cycles, etc.
        //if not, evict and store to mem if dirty (100), pull memory (100), update data (1)
    
    //todo: implement functionality
}

void Cache::store(unsigned int address) {
    totalStores++;
    time++;
    //store hit - exists in cache, update data (1), make dirty, update stuff
    
    //write through - 100 (eject 4 byte address, data already in mem)

    //store miss - doesnt exists, find smallest ts, eject, check validity
        //if not valid, put into memory (100+100*size), pull from memory (100), update cache (1)
        //if valid, pull from memory (100), update cache (1)

    //no write allocate (must be write through) - update mem (just 100)

    //todo: implement functionality

}

void Cache::printSummary() {
    std::cout << "Total loads: " << totalLoads << std::endl;
    std::cout << "Total stores: " << totalStores << std::endl;
    std::cout << "Load hits: " << loadHits << std::endl;
    std::cout << "Load misses: " << loadMisses << std::endl;
    std::cout << "Store hits: " << storeHits << std::endl;
    std::cout << "Store misses: " << storeMisses << std::endl;
    std::cout << "Total cycles: " << totalCycles << std::endl;
}


int Cache::get_num_offset_bits (unsigned int address, int label) {
    int num_bits = 0;
    int result = label;
    while (result > 1) {
        num_bits++;
        result = result >> 1;
    }
    return num_bits;
}

int Cache::get_index_bits (unsigned int address) {
    int num_offset_bits = get_num_offset_bits(address, blockSize);
    unsigned int index_bits = address >> num_offset_bits;
    index_bits = index_bits & (sets-1);
    return index_bits;
}

int Cache::get_tag_bits (unsigned int address) {
    int num_offset_bits = get_num_offset_bits(address, blockSize);
    int num_index_bits = get_num_offset_bits(address, sets);
    int total_offset = num_offset_bits + num_index_bits;
    return address >> total_offset;
}

//helper functions to be added later