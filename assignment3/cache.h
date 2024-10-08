#ifndef CACHE_H
#define CACHE_H

#include <iostream>
#include <vector>
#include <cmath>
#include <string>

class Cache {
public:
    Cache(int sets, int blocks, int blockSize, const std::string& writeAllocate, const std::string& writePolicy, const std::string& evictionPolicy);
    
    void load(unsigned int address);
    void store(unsigned int address);
    void printSummary();

private:
    int totalLoads;
    int totalStores;
    int loadHits;
    int loadMisses;
    int storeHits;
    int storeMisses;
    int totalCycles;

    //config param for cache
    int sets;
    int blocks;
    int blockSize;
    std::string writeAllocate;
    std::string writePolicy;
    std::string evictionPolicy;

    //add more later
};

#endif