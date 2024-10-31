Prasi Thapa
Implemented:
cache.cpp
main.cpp
Makefile
Experimentation

Claire Zeng
Implemented:
cache.cpp
main.cpp
Makefile
Experimentation

Same capacity used for all configurations - 1 MB = 1048576 bytes
Direct mapped - n sets of 1 block each
M-way set associative - n sets of m blocks each
Fully associative - 1 set of n blocks

Parameters: 
Number of sets (paritioning of cache)
Number of blocks per set (impacting whether direct-mapped, set-associative, or fully associative)
Block size (impacting spatial locality)
Write Policy
Eviction Policy (relevant when multiple blocks are eligible for eviction)


Experimentation Data;



| Configuration Type       | Sets  | Blocks | Bytes | Write Policy                    | Eviction Policy | Hit Rate (%) | Miss Rate (%) | Total Cycles  |
|--------------------------|-------|--------|-------|---------------------------------|-----------------|--------------|---------------|---------------|
| Fully Associative        | 1     | 256    | 4096  | Write-allocate, Write-back      | LRU             | 99.942       | 0.05798       | 33078883      |
|                          | 1     | 1024   | 1024  | Write-allocate, Write-through   | LRU             | 99.899       | 0.1012        | 33404653      |
|                          | 1     | 4096   | 256   | No-write-allocate, Write-through| FIFO            | 96.830       | 3.1699        | 25986797      |
|                          | 1     | 16,384 | 64    | No-write-allocate, Write-through| FIFO            | 94.9432      | 5.0568        | 23746797      |
|                          | 1     | 65,536 | 16    | Write-allocate, Write-back      | LRU             | 97.826       | 2.174         | 5000083       |
|                          | 1     | 262,144| 4     | Write-allocate, Write-through   | LRU             | 93.836       | 6.164         | 20537253      |

| Direct Mapped            | 256   | 1      | 4096  | Write-allocate, Write-back      | -               | 99.839       | 0.1606        | 116227683     |
|                          | 1024  | 1      | 1024  | No-write-allocate, Write-through| -               | 97.926       | 2.074         | 35401197      |
|                          | 4096  | 1      | 256   | Write-allocate, Write-back      | -               | 99.753       | 0.247         | 9283683       |
|                          | 16384 | 1      | 64    | Write-allocate, Write-through   | -               | 99.330       | 0.669         | 25301989      |
|                          | 65536 | 1      | 16    | Write-allocate, Write-through   | -               | 97.818       | 2.1823        | 23688683      |
|                          | 262144| 1      | 4     | No-write-allocate, Write-through| -               | 90.896       | 9.104         | 21013697      |

| Set-Associative          | 256   | 4      | 1024  | No-write-allocate, Write-through| LRU             | 97.9786      | 2.021         | 31509997      |
|                          | 256   | 16     | 256   | No-write-allocate, Write-through| LRU             | 97.9786      | 2.021         | 25986797      |
|                          | 256   | 64     | 64    | No-write-allocate, Write-through| LRU             | 94.9432      | 5.0567        | 23746797      |
|                          | 256   | 256    | 16    | No-write-allocate, Write-through| LRU             | 92.807       | 7.419         | 22462797      |
|                          | 256   | 1024   | 4     | No-write-allocate, Write-through| LRU             | 90.9006      | 9.099         | 21012197      |

|                          | 256   | 4      | 1024  | Write-allocate, Write-back      | LRU             | 99.8964      | 0.1036        | 14928483      |
|                          | 256   | 16     | 256   | Write-allocate, Write-back      | LRU             | 99.767       | 0.233         | 8298083       |
|                          | 256   | 64     | 64    | Write-allocate, Write-back      | LRU             | 99.347       | 0.6529        | 5902883       |
|                          | 256   | 256    | 16    | Write-allocate, Write-back      | LRU             | 97.82599     | 2.17401       | 5000083       |
|                          | 256   | 1024   | 4     | Write-allocate, Write-back      | LRU             | 93.836       | 6.164         | 3694383       |

|                          | 256   | 4      | 1024  | Write-allocate, Write-through   | LRU             | 99.89645     | 0.10355       | 33711655      |
|                          | 256   | 16     | 256   | Write-allocate, Write-through   | LRU             | 99.7673      | 0.233         | 27675121      |
|                          | 256   | 64     | 64    | Write-allocate, Write-through   | LRU             | 99.347       | 0.6529        | 25218971      |
|                          | 256   | 256    | 16    | Write-allocate, Write-through   | LRU             | 97.82599     | 2.1740        | 23673166      |
|                          | 256   | 1024   | 4     | Write-allocate, Write-through   | LRU             | 93.836       | 6.164         | 20537253      |

|                          | 1024 | 4      | 256   | No-write-allocate, Write-through| LRU             | 96.829       | 3.170         | 25993197     |
|                          | 1024 | 16     | 64    | No-write-allocate, Write-through| LRU             | 94.943       | 5.057         | 23748397     |
|                          | 1024 | 64     | 16    | No-write-allocate, Write-through| LRU             | 92.581       | 7.419         | 22462797     |
|                          | 1024 | 256    | 4     | No-write-allocate, Write-through| LRU             | 90.901       | 9.099         | 21012197     |

|                          | 1024 | 4      | 256   | write-allocate, Write-back      | LRU             | 99.767       | 0.233         | 8394083     |
|                          | 1024 | 16     | 64    | write-allocate, Write-back      | LRU             | 99.346       | 0.653         | 5923683     |
|                          | 1024 | 64     | 16    | write-allocate, Write-back      | LRU             | 97.826       | 2.174         | 5000083     |
|                          | 1024 | 256    | 4     | write-allocate, Write-back      | LRU             | 93.835       | 6.164         | 3694383     |

|                          | 1024 | 4      | 256   | write-allocate, Write-through   | LRU             | 99.766       | 0.233         | 27687723     |
|                          | 1024 | 16     | 64    | write-allocate, Write-through   | LRU             | 99.346       | 0.653         | 25220571     |
|                          | 1024 | 64     | 16    | write-allocate, Write-through   | LRU             | 97.826       | 2.174         | 23673166     |
|                          | 1024 | 256    | 4     | write-allocate, Write-through   | LRU             | 93.835       | 6.164         | 20537253     |


|                          | 4096 | 4      | 64    | write-allocate, Write-back      | LRU             | 99.345       | 0.654         | 5971683      |
|                          | 4096 | 64     | 4     | write-allocate, Write-back      | LRU             | 93.835       | 6.164         | 3694383      |

|                          |16,384| 4      | 16    | write-allocate, Write-back      | LRU             | 97.825       | 2.174         | 5016483      |
|                          |16,384| 16     | 4     | write-allocate, Write-back      | LRU             | 93.835       | 6.164         | 3695083      |

|                          |65,536| 4      | 4     | write-allocate, Write-back      | LRU             | 93.835       | 6.164         | 3697783      |


Observations:
- The highest hit rate (99.942%) was observed with 1 set, 256 blocks, 4096 bytes, write-allocate, write-back, and LRU eviction.
- As the number of blocks increased, hit rates generally remained high, but total cycles also increased slightly.
- Direct mapped caches performed worse compared to fully associative caches, especially with smaller block sizes. For example, with 256 sets, 1 block, 4096 bytes, write-allocate, and write-back, the hit rate was 99.839%, but total cycles were much higher at 116,227,683.
- Direct-mapped caches performed best when block size was larger (4096 bytes for example) but still fell short in total cycles.
- Set-associative caches were generally in between direct-mapped and fully associative caches. 
- Numbers for set associate caches are all fairly similar, so sizes may be somewhat neglible.

Analysis:
- Fully Associative: High hit rates (over 99%) for write-back configurations indicate good cache performance, especially for larger block sizes (4096 bytes). However, the miss rates increase significantly when using FIFO and no-write-allocate policies.
- Direct Mapped: Lower hit rates compared to fully associative caches, especially with larger sets (e.g., 256 sets with a block size of 4096 bytes). The miss rates are higher than fully associative configurations, showing potential downside.
- Set-Associative: Hit rates are lower than fully associative but generally better than direct mapped, especially with larger block sizes. Set-associative caches also show lower cycle counts compared to direct mapped configurations, and thus effectively reduce misses.
- Fully associative caches offer the highest hit rates but come with increased complexity and cost in hardware implementation. 
- For all configurations, very large block sizes (4096 bytes and above) tend to increase miss rates due to reduced cache utilization.

Best cache configuration is:
- 1024 sets
- 16 blocks per set 
- 64 bytes per block 
- Write-allocate, Write-back      
- LRU eviction policy 

- In general, set associative cashes are more efficient than direct mapped and fully associative. We thus focused on set associative configurations.
- Has a relative low cycle count, with a relatively high hit rate.
- Does not take up as much space as 4096 set configurations
- A good in-between for trade off between hit rate and cycle count.


