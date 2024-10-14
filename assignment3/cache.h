#ifndef CACHE_H
#define CACHE_H

#include <cstdint>
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
    int time;

    //config param for cache
    int sets;
    int blocks;
    int blockSize;
    std::string writeAllocate;
    std::string writePolicy;
    std::string evictionPolicy;

    //add more later
    struct Block {
        uint32_t tag;
        bool valid, dirty;
        uint32_t load_ts, access_ts;

        uint32_t get_tag();
        bool get_validity();
        uint32_t get_ts();
    };

    struct Set {
        std::vector<Block> blocks;
    };

    int get_num_offset_bits (unsigned int address, int label);
    int get_index_bits (unsigned int address);
    int get_tag_bits (unsigned int address);
};

#endif