CONTRIBUTIONS

Prasi Thapa:
parsort.c
Experimentation/Analysis

Claire Zeng:
parsort.c
Experimentation/Analysis

REPORT

Outputs:

Test run with threshold 2097152

real    0m0.399s
user    0m0.379s
sys     0m0.017s
Test run with threshold 1048576

real    0m0.235s
user    0m0.386s
sys     0m0.031s
Test run with threshold 524288

real    0m0.175s
user    0m0.432s
sys     0m0.048s
Test run with threshold 262144

real    0m0.149s
user    0m0.460s
sys     0m0.049s
Test run with threshold 131072

real    0m0.142s
user    0m0.461s
sys     0m0.061s
Test run with threshold 65536

real    0m0.134s
user    0m0.479s
sys     0m0.090s
Test run with threshold 32768

real    0m0.139s
user    0m0.502s
sys     0m0.125s
Test run with threshold 16384

real    0m0.151s
user    0m0.561s
sys     0m0.146s

Outputs Trial #2:

Test run with threshold 2097152

real    0m0.384s
user    0m0.372s
sys     0m0.011s
Test run with threshold 1048576

real    0m0.253s
user    0m0.396s
sys     0m0.039s
Test run with threshold 524288

real    0m0.173s
user    0m0.427s
sys     0m0.044s
Test run with threshold 262144

real    0m0.130s
user    0m0.458s
sys     0m0.039s
Test run with threshold 131072

real    0m0.138s
user    0m0.456s
sys     0m0.069s
Test run with threshold 65536

real    0m0.120s
user    0m0.477s
sys     0m0.075s
Test run with threshold 32768

real    0m0.120s
user    0m0.508s
sys     0m0.103s
Test run with threshold 16384

real    0m0.153s
user    0m0.530s
sys     0m0.176s


Outputs Trial #3:

Test run with threshold 2097152

real    0m0.378s
user    0m0.359s
sys     0m0.017s
Test run with threshold 1048576

real    0m0.241s
user    0m0.387s
sys     0m0.036s
Test run with threshold 524288

real    0m0.182s
user    0m0.446s
sys     0m0.038s
Test run with threshold 262144

real    0m0.153s
user    0m0.460s
sys     0m0.057s
Test run with threshold 131072

real    0m0.143s
user    0m0.467s
sys     0m0.054s
Test run with threshold 65536

real    0m0.138s
user    0m0.496s
sys     0m0.074s
Test run with threshold 32768

real    0m0.139s
user    0m0.502s
sys     0m0.117s
Test run with threshold 16384

real    0m0.126s
user    0m0.543s
sys     0m0.141s

Threshold	Real Time (Average)
2097152	0.387
1048576	0.243
524288	0.177
262144	0.144
131072	0.141
65536	0.131
32768	0.133
16384	0.143


Analysis:

Evaluates parallel performance of parsort by sorting 16 mb random data file with varying thresholds. 
The threshold controls size of data segments before they are sorted.
Lower thresholds = more parallelism -> more processes running but may have more overhead

By using the ./run_experiments.sh command:
A 16 MB random data file (data_16M.in) generated using gen_rand_data
Thresholds were set to values from 2097152 down to 16384 to test effects of increased parallelism on performance
Timed using the time command to capture real, user, and sys times, which represent wall clock time, CPU time, and system call time


- Lowering the threshold means that more processes are created, increasing parallelism. 
This improved performance initially, as smaller segments of the array could be sorted simultaneously across multiple CPU cores.
- Once the threshold was lowered to around 65536, the real time no longer improved significantly. Diminishing returns.
Likely due to the overhead associated with processes and process creation, offsetting the benefits of parallelism.
- With too many processes, more costs.
- Since machine has limited number of cores, the program can only benefit from parallelism up to that limit. 
When the number of processes exceeded the number of available cores, some processes had to wait, resulting in minimal performance gains.
In addition, the OS kernel will have to switch processes leading to more overhead.
- With too many processes, more actual costs exceed the theoretical benefits.

The optimal threshold for this experiment is probably around 65536