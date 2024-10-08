#ifndef CACHE_H
#define CACHE_H

#include <iostream>
#include <vector>
#include <cmath>

class Cache {
public:
    Cache(int num_sets, int num_blocks, int block_size, bool write_allocate, bool write_back, bool lru);
    void simulate(const std::string& trace_file);
    void print_stats() const;

private:
    int num_sets;
    int num_blocks;
    int block_size;
    bool write_allocate;
    bool write_back;
    bool lru;

    // Stats
    int total_loads;
    int total_stores;
    int load_hits;
    int load_misses;
    int store_hits;
    int store_misses;
    int total_cycles;

    // Cache data structures (e.g., a 2D vector for sets and blocks)
    // Define any necessary structures for managing cache lines, tags, etc.
};

#endif // CACHE_H