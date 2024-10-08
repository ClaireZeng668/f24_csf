#ifndef CACHE_H
#define CACHE_H

typedef struct {
    int valid;
    unsigned int tag;
    int dirty;
    int lru_counter;
} CacheBlock;

typedef struct {
    CacheBlock *blocks;
} CacheSet;

typedef struct {
    CacheSet *sets;
    int num_sets;
    int blocks_per_set;
    int block_size;
} Cache;

Cache *initialize_cache(int num_sets, int blocks_per_set, int block_size);
void free_cache(Cache *cache);
int is_valid_cache_params(int sets, int blocks_per_set, int block_size);
int is_power_of_two(int x);

#endif