#ifndef CACHE_H
#define CACHE_H

#include <cstdint>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>


// Class representing a block in a set of a cache describing the tag, the access time, and validity of the block
class Block {
    public: 
    int tag = INT32_MAX;
    bool dirty = false;
    int ts = 0; //default zero
};

// Class representing a set in a cache implemented using a vector of 'Block' elements
class Set {
    public:
    std::vector<Block> block_vec;

    /*
    * Constructor from a integer value 
    * 
    * Parameters:
    *     blocks - the specified number of blocks per set 
    */
    Set(int blocks);
};

// Class representing a cache implemented using a vector of `Set` elements along with the necessary counter variables and cache configuration
class Cache {
public:

    /*
    * Constructor from three integer values and three std::strings
    * 
    * Parameters:
    *     sets - int value specifying the number of sets in the cache
    *     blocks - int value specifying the number of blocks per set
    *     blockSize - int value specifying the size of each block in the cache
    *     writeAllocate - std::string value specifying the cache policy for a write/store miss
    *     writePolicy - std::string value specifying the cache policy for a write/store hit
    *     evictionPolicy - std::string value specifying the eviction policy when a block must be evicted from the cache
    */
    Cache(int sets, int blocks, int blockSize, const std::string& writeAllocate, const std::string& writePolicy, const std::string& evictionPolicy);
    
    /*
    * Load a value from the cache
    * Will update the relevant summary information
    * 
    * Parameters:
    *     address - unsigned int of the address to load from the cache
    */
    void load(unsigned int address);

    /*
    * Store a value into the cache
    * Will update the relevant summary information
    * 
    * Parameters:
    *     address - unsigned int of the address to load into the cache
    */
    void store(unsigned int address);

    /*
    * Print the cache summary information
    * 
    */
    void printSummary();

private:
    //summary information
    int totalLoads;
    int totalStores;
    int loadHits;
    int loadMisses;
    int storeHits;
    int storeMisses;
    //standard int size too small for large caches
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


    /*
    * Determine the number of offset bits given a size
    *
    * Parameters:
    *     size - int value corresponding to the size to offset
    *
    * Returns:
    *   an integer value for how many bits to offset
    */
    int get_num_bits (int size);

    /*
    * Determine the index bits from the address
    *
    * Parameters:
    *     address - int value corresponding to the address to parse the index bits from
    *
    * Returns:
    *   an integer value of the index
    */
    int get_index_bits (unsigned int address);

    /*
    * Determine the tag bits from the address
    *
    * Parameters:
    *     address - int value corresponding to the address to parse the tag bits from
    *
    * Returns:
    *   an integer value of the tag
    */
    int get_tag_bits (unsigned int address);

    /*
    * Handle load misses
    * Will update the relevant summary information and block information
    *
    * Parameters:
    *     current - Set object corresponding to the set specified by the index
    *     smallest - int value corresponding to the index of the block to be evicted
    *     tag - int value corresponding to the tag of the new block to load in
    */
    void load_miss(Set *current, int smallest, int tag);

    /*
    * Handle load hits
    * Will update the relevant summary information and block information
    *
    * Parameters:
    *     current_block - Block object corresponding to the found block
    */
    void load_hit(Block *current_block);

    /*
    * Handle store misses
    * Will update the relevant summary information and block information
    *
    * Parameters:
    *     current - Set object corresponding to the set specified by the index
    *     smallest - int value corresponding to the index of the block to be evicted
    *     tag - int value corresponding to the tag of the new block to load in
    */
    void store_miss(Set *current, int smallest, int tag);

    /*
    * Handle store hits
    * Will update the relevant summary information and block information
    *
    * Parameters:
    *     current_block - Block object corresponding to the found block
    */
    void store_hit(Block *current_block);
};

#endif
