//cache related logic implementation

#include "cache.h"
#include <stdlib.h>
#include <stdio.h>

Cache *initialize_cache(int num_sets, int blocks_per_set, int block_size) {
    Cache *cache = (Cache *)malloc(sizeof(Cache));
    cache->num_sets = num_sets;
    cache->blocks_per_set = blocks_per_set;
    cache->block_size = block_size;
    cache->sets = (CacheSet *)malloc(num_sets * sizeof(CacheSet));

    for (int i = 0; i < num_sets; i++) {
        cache->sets[i].blocks = (CacheBlock *)malloc(blocks_per_set * sizeof(CacheBlock));
        for (int j = 0; j < blocks_per_set; j++) {
            cache->sets[i].blocks[j].valid = 0;
        }
    }
    return cache;
}

void free_cache(Cache *cache) {
    for (int i = 0; i < cache->num_sets; i++) {
        free(cache->sets[i].blocks);
    }
    free(cache->sets);
    free(cache);
}

int is_valid_cache_params(int sets, int blocks_per_set, int block_size) {
    return is_power_of_two(sets) && is_power_of_two(blocks_per_set) && is_power_of_two(block_size) && block_size >= 4;
}