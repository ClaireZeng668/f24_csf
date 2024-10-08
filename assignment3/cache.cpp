//cache related logic implementation

#include "cache.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <cmath>

Cache::Cache(int sets, int blocks, int blockSize, const std::string& writeAllocate, const std::string& writePolicy, const std::string& evictionPolicy)
    : sets(sets), blocks(blocks), blockSize(blockSize), writeAllocate(writeAllocate), writePolicy(writePolicy), evictionPolicy(evictionPolicy),
      totalLoads(0), totalStores(0), loadHits(0), loadMisses(0), storeHits(0), storeMisses(0), totalCycles(0) {
        
      }

void Cache::load(unsigned int address) {
    totalLoads++;
    //todo
}

void Cache::store(unsigned int address) {
    totalStores++;
    //todo
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