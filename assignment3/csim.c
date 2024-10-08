//main cache simulator implementation

#include <stdio.h>
#include <stdlib.h>
#include "cache.h"
#include "trace.h"

int main(int argc, char *argv[]) {
    if (argc != 7) {
        print_usage();
        return 1;
    }

    //parse command line arguments
    int sets = atoi(argv[1]);
    int blocks_per_set = atoi(argv[2]);
    int block_size = atoi(argv[3]);

    if (!is_valid_cache_params(sets, blocks_per_set, block_size)) {
        fprintf(stderr, "Invalid cache parameters\n");
        return 1;
    }

    Cache *cache = initialize_cache(sets, blocks_per_set, block_size);

    //reade file
    read_trace(cache);

    //free memory
    free_cache(cache);

    return 0;
}