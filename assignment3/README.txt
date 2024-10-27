Prasi Thapa
Implemented:
cache.cpp
main.cpp
Makefile

Claire Zeng
Implemented:
cache.cpp
main.cpp
Makefile

Same capacity used for all configurations - 1 MB = 1048576 bytes
Direct mapped - n sets of 1 block each
M-way set associative - n sets of m blocks each
Fully associative - 1 set of n blocks

Configs to test: idk is this too many? also anything past 4096 bytes/block probably gonna do worse i think

    Fully Associative:                       write-allocate write-back  write-allocate write through    no-write-allocate write-through
        1 set, 256 blocks, 4096 bytes/block
        1 set, 1024 blocks, 1024 bytes/block
        1 set, 4096 blocks, 256 bytes/block
        1 set, 16384 blocks, 64 bytes/block
        1 set, 65536 blocks, 16 bytes/block
        1 set, 262144 blocks, 4 bytes/block
    
    Direct mapped:                           write-allocate write-back  write-allocate write through    no-write-allocate write-through
        256 sets, 1 block, 4096 bytes/block
        1024 sets, 1 block, 1024 bytes/block
        4096 sets, 1 block, 256 bytes/block
        16384 sets, 1 block, 64 bytes/block
        65536 sets, 1 block, 16 bytes/block
        262144 sets, 1 block, 4 bytes/block

        fifo
        256 sets, 1 block, 4096 bytes/block
        1024 sets, 1 block, 1024 bytes/block
        4096 sets, 1 block, 256 bytes/block
        16384 sets, 1 block, 64 bytes/block
        65536 sets, 1 block, 16 bytes/block
        262144 sets, 1 block, 4 bytes/block

    Set associative
        4 sets, 65536 blocks, 4 bytes/block
        16 sets, 
        64 sets, 
        256 sets, 
        1024 sets, 
        4096 sets, 
        16384 sets, 
        65536 sets, 
    