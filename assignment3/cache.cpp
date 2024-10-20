//cache related logic implementation

#include "cache.h"
#include <climits>
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
        for (int i = 0; i < sets; i++) {
            Set *empty = new Set(blocks);
            set_vec.push_back(empty);
        }
    }

void Cache::output() {
    for (int i = 0; i < blocks; i++) {
        std::cout << i << " " << set_vec.at(164)->block_vec.at(i).tag << " ";
    }
    std::cout << std::endl;
}

void Cache::load(unsigned int address) {
    totalLoads++;
    time++;
    int index = get_index_bits(address);
    int tag = get_tag_bits(address);
    
    //std::cout << "index " << index << std::endl;
    Set *current = set_vec.at(index);
    //std::cout << current->block_vec.at(0).tag << std::endl;
    int smallest = 0;
    //loop through set, check if tag is there
    for (int i = 0; i < blocks; i++) {
        Block current_block = current->block_vec.at(i);
        int current_tag = current_block.tag;
        int access = current_block.ts;
        //std::cout << "current tag " << current->block_vec.at(i).tag << std::endl;

        //find lru or fifo
        if (access < current->block_vec.at(smallest).ts) {
            smallest = i;
        }
        if (current_tag == tag) {
            loadHits++;
            return;
        }
    }
    loadMisses++;
    Block to_replace = current->block_vec.at(smallest);
    Block new_block;
    new_block.tag = tag;
    new_block.ts = time;
    //std::cout << "new tag " << new_block.tag << " smallest " << smallest << std::endl;
    current->block_vec.at(smallest) = new_block;
    if (to_replace.dirty) { //is dirty
        totalCycles+=201;
    } else {
        totalCycles+=101;
    }

    // for (int i = 0; i < blocks; i++) {
    //     std::cout << i << " " << current->block_vec.at(i).tag << " ";
    // }
    // std::cout << index << std::endl;
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