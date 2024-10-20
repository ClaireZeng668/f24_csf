#ifndef CACHE_H
#define CACHE_H

#include <cstdint>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>


class Block {
    public: 
    int tag;
    bool dirty=false;
    int ts=0; //default zero
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
    void output();

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

    int get_num_offset_bits (unsigned int address, int label);
    int get_index_bits (unsigned int address);
    int get_tag_bits (unsigned int address);
    std::vector<Set*> set_vec;
};

#endif