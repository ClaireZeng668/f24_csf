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

Higher block sizes (like 4096 bytes/block and greater) will likely do worse.
Configurations:



| Configuration Type       | Sets | Blocks | Bytes | Write Policy                   | Eviction Policy | Hit Rate (%) | Miss Rate (%) | Total Cycles |
|--------------------------|------|--------|-------|--------------------------------|-----------------|--------------|---------------|--------------|
| Fully Associative        | 1    | 256    | 4096  | Write-allocate, Write-back     | LRU             | 99.942       | 0.05798       | 33078883     |
|                          | 1    | 1024   | 1024  | Write-allocate, Write-through   | LRU             | 99.899       | 0.1012        | 33404653     |
|                          | 1    | 4096   | 256   | No-write-allocate, Write-through| FIFO            | 96.830       | 3.1699        | 25986797     |
|                          | 1    | 16,384 | 64    | No-write-allocate, Write-through| FIFO            | 94.9432      | 5.0568        | 23746797     |
|                          | 1    | 65,536 | 16    | Write-allocate, Write-back     | LRU             | 97.826       | 2.174         | 5000083      |
|                          | 1    | 262,144| 4     | Write-allocate, Write-through   | LRU             | 93.836       | 6.164         | 20537253     |
| Direct Mapped            | 256  | 1      | 4096  | Write-allocate, Write-back     | -               | 99.839       | 0.1606        | 116227683    |
|                          | 1024 | 1      | 1024  | No-write-allocate, Write-through| -               | 97.926       | 2.074         | 35401197     |
|                          | 4096 | 1      | 256   | Write-allocate, Write-back     | -               | 99.753       | 0.247         | 9283683      |
|                          | 16,384| 1     | 64    | Write-allocate, Write-through   | -               | 99.330       | 0.669         | 25301989     |
|                          | 65,536| 1     | 16    | Write-allocate, Write-through   | -               | 97.818       | 2.1823        | 23688683     |
|                          | 262,144| 1    | 4     | No-write-allocate, Write-through| -               | 90.896       | 9.104         | 21013697     |
| Set-Associative          | 256  | 4      | 4096  | Write-allocate, Write-back     | LRU             | 99.943       | 0.0572        | 50263313     |
|                          | 256  | 16     | 1024  | Write-allocate, Write-through   | LRU             | 99.899       | 0.1012        | 33404653     |
|                          | 256  | 64     | 256   | Write-allocate, Write-through   | LRU             | 99.899       | 0.1012        | 33404653     |
|                          | 256  | 256    | 64    | Write-allocate, Write-through   | LRU             |              |               |              |
|                          | 256  | 1024   | 16    | Write-allocate, Write-back      | LRU             |              |               |              |
|                          | 256  | 4096   | 4     | No-write-allocate, Write-through | FIFO            |              |               |              |


Analysis:
- Fully Associative: High hit rates (over 99%) for write-back configurations indicate good cache performance, especially for larger block sizes (4096 bytes). However, the miss rates increase significantly when using FIFO and no-write-allocate policies.
- Direct Mapped: Lower hit rates compared to fully associative caches, especially with larger sets (e.g., 256 sets with a block size of 4096 bytes). The miss rates are higher than fully associative configurations, showing potential downside.
- Set-Associative: Hit rates are lower than fully associative but generally better than direct mapped, especially with larger block sizes. This indicates that set-associative caches are a good in-between.
- Fully associative caches have lower cycle counts than direct mapped, indicating they can service requests faster due to fewer misses. In contrast, the direct-mapped configuration, especially with a high number of sets, has a significantly higher cycle count, indicating inefficiencies with some workloads.
- Set-associative caches also demonstrate lower cycle counts compared to direct mapped configurations, and thus effectively reduce miss penalties.
- Fully associative caches offer the highest hit rates but come with increased complexity and cost in hardware implementation. Set-associative configurations provide a balance, offering lower miss rates than direct-mapped caches without the high complexity of fully associative designs.
- For all configurations, very large block sizes (4096 bytes and above) tend to increase miss rates due to reduced cache utilization.
(?)

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