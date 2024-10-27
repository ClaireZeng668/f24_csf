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


//Cache constructor
Cache::Cache(int sets, int blocks, int blockSize, const std::string& writeAllocate, const std::string& writePolicy, const std::string& evictionPolicy)
    : sets(sets), blocks(blocks), blockSize(blockSize), writeAllocate(writeAllocate), writePolicy(writePolicy), evictionPolicy(evictionPolicy),
      totalLoads(0), totalStores(0), loadHits(0), loadMisses(0), storeHits(0), storeMisses(0), totalCycles(0), time(0) {
        for (int i = 0; i < sets; i++) {
            Set *empty = new Set(blocks); //create a new blank set
            set_vec.push_back(empty); //add the set to the set vector
        }
    }

//Set constructor from a integer value
Set::Set(int blocks) {
    for (int i = 0; i < blocks; i++) {
        Block empty; //create a blank block
        block_vec.push_back(empty); //add the block to the block vector
    }
}

//handle load operation
void Cache::load(unsigned int address) {
    totalLoads++;
    time++;
    //get the index, tag, and set specified by the index
    int index = get_index_bits(address);
    int tag = get_tag_bits(address);

    Set *current = set_vec.at(index);
    int smallest = 0;

    //loop through the set, check if tag is there
    for (int i = 0; i < blocks; i++) {
        Block &current_block = current->block_vec.at(i);
        //find lru (for ms2)
        if (current_block.ts < current->block_vec.at(smallest).ts) {
            smallest = i;
        }
        if (current_block.tag == tag) {
            load_hit(&current_block);   //tag found
            return;
        }
    }
    //did not find the tag in the set
    load_miss(current, smallest, tag);
}

//handle load hits
void Cache::load_hit(Block *current_block) {
    loadHits++;
    totalCycles++;
    if (evictionPolicy == "lru") {
        current_block->ts = time;
    }
    return;
}

//handle load misses
void Cache::load_miss(Set *current, int smallest, int tag) {
    loadMisses++;
    Block to_replace = current->block_vec.at(smallest);
    int amt = blockSize / 4;
    if (to_replace.dirty) { 
        totalCycles = totalCycles + (amt * 200) + 1;    //is dirty - put into memory and pull from memory
    } else {
        totalCycles = totalCycles + (amt * 100) + 1;    //is not dirty - pull from memory
    }
    //create and replace lru (for ms2) with new block
    Block new_block;
    new_block.tag = tag;
    new_block.ts = time;
    current->block_vec.at(smallest) = new_block;
    return;
}

//handle store operation
void Cache::store(unsigned int address) {
    totalStores++;
    time++;
    //get the index, tag, and set specified by the index
    int index = get_index_bits(address);
    int tag = get_tag_bits(address);

    Set *current = set_vec.at(index);
    int smallest = 0;

    //loop through set, check if tag is there
    for (int i = 0; i < blocks; i++) {
        Block &current_block = current->block_vec.at(i);
        //find lru (for ms2)
        if (current_block.ts < current->block_vec.at(smallest).ts) {
            smallest = i;
        }
        if (current_block.tag == tag) {
            store_hit(&current_block);  //tag found
            return;
        }
    }
    //did not find tag in the set
    store_miss(current, smallest, tag);
}

//handle store hits
void Cache::store_hit(Block *current_block) {
    storeHits++;
    if (evictionPolicy == "lru") {
        current_block->ts = time;
    }
    //handle write back - update cache but not memory
    if (writePolicy == "write-back") {
        current_block->dirty = true;
        totalCycles++;
    } else {
        //handle write through - write directly to memory
        totalCycles+=100;
    }
    return;
}

//handle store misses
void Cache::store_miss(Set *current, int smallest, int tag) {
    storeMisses++;
    int amt = blockSize / 4;
    Block to_replace = current->block_vec.at(smallest);
    //handle store miss for write allocate - load block into cache and update
    if (writeAllocate == "write-allocate") {
        if (to_replace.dirty) { 
            totalCycles = totalCycles + (amt * 200) + 1;    //is dirty - put into memory, pull from memory
        } else { 
            totalCycles = totalCycles + (amt * 100) + 1;    //is not dirty - pull from memory
        }
        //create and replace lru (for ms2) with new block
        Block new_block;
        new_block.tag = tag;
        new_block.ts = time;
        //if write allocate and write back - new block is now dirty
        if (writePolicy == "write-back") {
            new_block.dirty = true;
        }
        current->block_vec.at(smallest) = new_block;
    } else {
        //handle no write allocate - write directly to memory
        totalCycles+=100;
    }
    return;
}

//print cache summary information
void Cache::printSummary() {
    std::cout << "Total loads: " << totalLoads << std::endl;
    std::cout << "Total stores: " << totalStores << std::endl;
    std::cout << "Load hits: " << loadHits << std::endl;
    std::cout << "Load misses: " << loadMisses << std::endl;
    std::cout << "Store hits: " << storeHits << std::endl;
    std::cout << "Store misses: " << storeMisses << std::endl;
    std::cout << "Total cycles: " << totalCycles << std::endl;
}

int Cache::get_num_bits (int size) {
    int num_bits = 0;
    int result = size;
    //divide size by two until 1 to get number of bits for a practiular size
    while (result > 1) {
        num_bits++;
        result = result >> 1;
    }
    return num_bits;
}

int Cache::get_index_bits (unsigned int address) {
    int num_offset_bits = get_num_bits(blockSize);
    int index_bits = address >> num_offset_bits;    //shift address by number of offset bits (blockSize = 2^#offset bits)
    index_bits = index_bits & (sets-1);             //removes tag bits to get index
    return index_bits;
}

int Cache::get_tag_bits (unsigned int address) {
    int num_offset_bits = get_num_bits(blockSize);
    int num_index_bits = get_num_bits(sets);
    int total_offset = num_offset_bits + num_index_bits;
    return address >> total_offset;  //shift address by number of offset and index bits to get tag  (blockSize = 2^#offset bits, sets = 2^#index bits)
}
