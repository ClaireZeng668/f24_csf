#ifndef CACHE_H
#define CACHE_H

#include <cstdint>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>


class Block {
    public: 
    int tag = INT32_MAX;
    bool dirty = false;
    int ts = 0; //default zero
};

class Set {
    public:
    std::vector<Block> block_vec;

    Set(int blocks) {
        for (int i = 0; i < blocks; i++) {
            Block empty;
            block_vec.push_back(empty);
        }
    }
};

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
    uint64_t totalCycles;
    int time;
    std::vector<Set*> set_vec;

    //config param for cache
    int sets;
    int blocks;
    int blockSize;
    std::string writeAllocate;
    std::string writePolicy;
    std::string evictionPolicy;

    int get_num_offset_bits (int label);
    int get_index_bits (unsigned int address);
    int get_tag_bits (unsigned int address);
    void load_miss(Set *current, int smallest, int tag);
    void load_hit(Block *current_block);
    void store_miss(Set *current, int smallest, int tag);
    void store_hit(Block *current_block);
};

#endif