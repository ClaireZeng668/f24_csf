#!/bin/bash

# Function to compare the output of a command to multiple expected results
compare_output() {
    local command="$1"
    shift
    local expected_outputs=("$@")

    echo "Running: $command"
    timeout 1m bash -c "$command" > output.txt

    if [ $? -eq 124 ]; then
        echo "Test Failed for $command: Timed out after 1 minute"
        echo "----------------------------------------"
        return
    fi

    local pass=false
    for expected_output in "${expected_outputs[@]}"; do
        local match=true
        while IFS= read -r expected_line && IFS= read -r actual_line <&3; do
            if [[ "$expected_line" == *"Total cycles:"* ]]; then
                expected_cycles=$(echo "$expected_line" | grep -oP '(?<=Total cycles: )\d+')
                actual_cycles=$(echo "$actual_line" | grep -oP '(?<=Total cycles: )\d+')
                if [ -n "$expected_cycles" ] && [ -n "$actual_cycles" ]; then
                    margin=$(echo "$expected_cycles * 0.1" | bc)
                    min_cycles=$(echo "$expected_cycles - $margin" | bc)
                    max_cycles=$(echo "$expected_cycles + $margin" | bc)
                    if (( $(echo "$actual_cycles < $min_cycles" | bc -l) )) || (( $(echo "$actual_cycles > $max_cycles" | bc -l) )); then
                        match=false
                        break
                    fi
                else
                    match=false
                    break
                fi
            else
                if [ "$expected_line" != "$actual_line" ]; then
                    match=false
                    break
                fi
            fi
        done 3< output.txt < "$expected_output"

        if $match; then
            pass=true
            break
        fi
    done

    if $pass; then
        echo "Test Passed"
    else
        echo "Test Failed for $command"
        echo "Expected one of the following outputs:"
        for expected_output in "${expected_outputs[@]}"; do
            echo "File: $expected_output"
            cat "$expected_output"
            echo ""
        done
        echo "Actual output:"
        cat output.txt
    fi

    echo "---------------------------------------------------------------"
}

# Define commands as associative arrays with expected output files
declare -A commands
declare -A expected_outputs

# Mapping of commands to expected outputs
commands[cmd1]="./csim 1 1 4 write-allocate write-through lru < traces/misses.trace"
expected_outputs[cmd1]="expected_results/misses/1_1_4_wa_wt_lru.txt expected_results/misses/1_1_4_wa_wt_lru_alt.txt"

commands[cmd2]="./csim 1 1024 128 no-write-allocate write-through lru < traces/misses.trace"
expected_outputs[cmd2]="expected_results/misses/1_1024_128_nwa_wt_lru.txt expected_results/misses/1_1024_128_nwa_wt_lru_alt.txt"

commands[cmd3]="./csim 1 1024 128 write-allocate write-through lru < traces/misses.trace"
expected_outputs[cmd3]="expected_results/misses/1_1024_128_wa_wt_lru.txt expected_results/misses/1_1024_128_wa_wt_lru_alt.txt"

commands[cmd4]="./csim 1 1024 128 write-allocate write-back lru < traces/misses.trace"
expected_outputs[cmd4]="expected_results/misses/1_1024_128_wa_wb_lru.txt expected_results/misses/1_1024_128_wa_wb_lru_alt.txt"

commands[cmd5]="./csim 8192 1 16 no-write-allocate write-through lru < traces/misses.trace"
expected_outputs[cmd5]="expected_results/misses/8192_1_16_nwa_wt_lru.txt expected_results/misses/8192_1_16_nwa_wt_lru_alt.txt"

commands[cmd6]="./csim 8192 1 16 write-allocate write-through lru < traces/misses.trace"
expected_outputs[cmd6]="expected_results/misses/8192_1_16_wa_wt_lru.txt expected_results/misses/8192_1_16_wa_wt_lru_alt.txt"

commands[cmd7]="./csim 8192 1 16 write-allocate write-back lru < traces/misses.trace"
expected_outputs[cmd7]="expected_results/misses/8192_1_16_wa_wb_lru.txt expected_results/misses/8192_1_16_wa_wb_lru_alt.txt"

commands[cmd8]="./csim 256 16 64 no-write-allocate write-through lru < traces/misses.trace"
expected_outputs[cmd8]="expected_results/misses/256_16_64_nwa_wt_lru.txt expected_results/misses/256_16_64_nwa_wt_lru_alt.txt"

commands[cmd9]="./csim 128 32 64 write-allocate write-through lru < traces/misses.trace"
expected_outputs[cmd9]="expected_results/misses/128_32_64_wa_wt_lru.txt expected_results/misses/128_32_64_wa_wt_lru_alt.txt"

commands[cmd10]="./csim 2048 4 16 no-write-allocate write-through lru < traces/misses.trace"
expected_outputs[cmd10]="expected_results/misses/2048_4_16_nwa_wt_lru.txt expected_results/misses/2048_4_16_nwa_wt_lru_alt.txt"

commands[cmd11]="./csim 2048 4 16 write-allocate write-through lru < traces/misses.trace"
expected_outputs[cmd11]="expected_results/misses/2048_4_16_wa_wt_lru.txt expected_results/misses/2048_4_16_wa_wt_lru_alt.txt"

commands[cmd12]="./csim 2048 4 16 write-allocate write-back lru < traces/misses.trace"
expected_outputs[cmd12]="expected_results/misses/2048_4_16_wa_wb_lru.txt expected_results/misses/2048_4_16_wa_wb_lru_alt.txt"

commands[cmd13]="./csim 1024 1 128 write-allocate write-through lru < traces/misses.trace"
expected_outputs[cmd13]="expected_results/misses/1024_1_128_wa_wt_lru.txt expected_results/misses/1024_1_128_wa_wt_lru_alt.txt"

commands[cmd14]="./csim 1024 1 128 write-allocate write-back lru < traces/misses.trace"
expected_outputs[cmd14]="expected_results/misses/1024_1_128_wa_wb_lru.txt expected_results/misses/1024_1_128_wa_wb_lru_alt.txt"

commands[cmd15]="./csim 256 4 128 no-write-allocate write-through lru < traces/misses.trace"
expected_outputs[cmd15]="expected_results/misses/256_4_128_nwa_wt_lru.txt expected_results/misses/256_4_128_nwa_wt_lru_alt.txt"

commands[cmd16]="./csim 256 4 128 write-allocate write-through lru < traces/misses.trace"
expected_outputs[cmd16]="expected_results/misses/256_4_128_wa_wt_lru.txt expected_results/misses/256_4_128_wa_wt_lru_alt.txt"

commands[cmd17]="./csim 256 4 128 write-allocate write-back lru < traces/misses.trace"
expected_outputs[cmd17]="expected_results/misses/256_4_128_wa_wb_lru.txt expected_results/misses/256_4_128_wa_wb_lru_alt.txt"

commands[cmd18]="./csim 1 1 4 write-allocate write-through lru < traces/hits.trace"
expected_outputs[cmd18]="expected_results/hits/1_1_4_wa_wt_lru.txt expected_results/hits/1_1_4_wa_wt_lru_alt.txt"

commands[cmd19]="./csim 1 1024 128 no-write-allocate write-through lru < traces/hits.trace"
expected_outputs[cmd19]="expected_results/hits/1_1024_128_nwa_wt_lru.txt expected_results/hits/1_1024_128_nwa_wt_lru_alt.txt"

commands[cmd20]="./csim 1 1024 128 write-allocate write-through lru < traces/hits.trace"
expected_outputs[cmd20]="expected_results/hits/1_1024_128_wa_wt_lru.txt expected_results/hits/1_1024_128_wa_wt_lru_alt.txt"

commands[cmd21]="./csim 1 1024 128 write-allocate write-back lru < traces/hits.trace"
expected_outputs[cmd21]="expected_results/hits/1_1024_128_wa_wb_lru.txt expected_results/hits/1_1024_128_wa_wb_lru_alt.txt"

commands[cmd22]="./csim 8192 1 16 no-write-allocate write-through lru < traces/hits.trace"
expected_outputs[cmd22]="expected_results/hits/8192_1_16_nwa_wt_lru.txt expected_results/hits/8192_1_16_nwa_wt_lru_alt.txt"

commands[cmd23]="./csim 8192 1 16 write-allocate write-through lru < traces/hits.trace"
expected_outputs[cmd23]="expected_results/hits/8192_1_16_wa_wt_lru.txt expected_results/hits/8192_1_16_wa_wt_lru_alt.txt"

commands[cmd24]="./csim 8192 1 16 write-allocate write-back lru < traces/hits.trace"
expected_outputs[cmd24]="expected_results/hits/8192_1_16_wa_wb_lru.txt expected_results/hits/8192_1_16_wa_wb_lru_alt.txt"

commands[cmd25]="./csim 256 16 64 no-write-allocate write-through lru < traces/hits.trace"
expected_outputs[cmd25]="expected_results/hits/256_16_64_nwa_wt_lru.txt expected_results/hits/256_16_64_nwa_wt_lru_alt.txt"

commands[cmd26]="./csim 128 32 64 write-allocate write-through lru < traces/hits.trace"
expected_outputs[cmd26]="expected_results/hits/128_32_64_wa_wt_lru.txt expected_results/hits/128_32_64_wa_wt_lru_alt.txt"

commands[cmd27]="./csim 2048 4 16 no-write-allocate write-through lru < traces/hits.trace"
expected_outputs[cmd27]="expected_results/hits/2048_4_16_nwa_wt_lru.txt expected_results/hits/2048_4_16_nwa_wt_lru_alt.txt"

commands[cmd28]="./csim 2048 4 16 write-allocate write-through lru < traces/hits.trace"
expected_outputs[cmd28]="expected_results/hits/2048_4_16_wa_wt_lru.txt expected_results/hits/2048_4_16_wa_wt_lru_alt.txt"

commands[cmd29]="./csim 2048 4 16 write-allocate write-back lru < traces/hits.trace"
expected_outputs[cmd29]="expected_results/hits/2048_4_16_wa_wb_lru.txt expected_results/hits/2048_4_16_wa_wb_lru_alt.txt"

commands[cmd30]="./csim 1024 1 128 write-allocate write-through lru < traces/hits.trace"
expected_outputs[cmd30]="expected_results/hits/1024_1_128_wa_wt_lru.txt expected_results/hits/1024_1_128_wa_wt_lru_alt.txt"

commands[cmd31]="./csim 1024 1 128 write-allocate write-back lru < traces/hits.trace"
expected_outputs[cmd31]="expected_results/hits/1024_1_128_wa_wb_lru.txt expected_results/hits/1024_1_128_wa_wb_lru_alt.txt"

commands[cmd32]="./csim 256 4 128 no-write-allocate write-through lru < traces/hits.trace"
expected_outputs[cmd32]="expected_results/hits/256_4_128_nwa_wt_lru.txt expected_results/hits/256_4_128_nwa_wt_lru_alt.txt"

commands[cmd33]="./csim 256 4 128 write-allocate write-through lru < traces/hits.trace"
expected_outputs[cmd33]="expected_results/hits/256_4_128_wa_wt_lru.txt expected_results/hits/256_4_128_wa_wt_lru_alt.txt"

commands[cmd34]="./csim 256 4 128 write-allocate write-back lru < traces/hits.trace"
expected_outputs[cmd34]="expected_results/hits/256_4_128_wa_wb_lru.txt expected_results/hits/256_4_128_wa_wb_lru_alt.txt"

commands[cmd35]="./csim 1 1 4 write-allocate write-through lru < traces/mixed.trace"
expected_outputs[cmd35]="expected_results/mixed/1_1_4_wa_wt_lru.txt expected_results/mixed/1_1_4_wa_wt_lru_alt.txt"

commands[cmd36]="./csim 1 256 128 no-write-allocate write-through lru < traces/mixed.trace"
expected_outputs[cmd36]="expected_results/mixed/1_256_128_nwa_wt_lru.txt expected_results/mixed/1_256_128_nwa_wt_lru_alt.txt"

commands[cmd37]="./csim 256 1 16 write-allocate write-through lru < traces/mixed.trace"
expected_outputs[cmd37]="expected_results/mixed/256_1_16_wa_wt_lru.txt expected_results/mixed/256_1_16_wa_wt_lru_alt.txt"

commands[cmd38]="./csim 256 4 128 no-write-allocate write-through lru < traces/mixed.trace"
expected_outputs[cmd38]="expected_results/mixed/256_4_128_nwa_wt_lru.txt expected_results/mixed/256_4_128_nwa_wt_lru_alt.txt"

commands[cmd39]="./csim 256 4 128 write-allocate write-through lru < traces/mixed.trace"
expected_outputs[cmd39]="expected_results/mixed/256_4_128_wa_wt_lru.txt expected_results/mixed/256_4_128_wa_wt_lru_alt.txt"

commands[cmd40]="./csim 256 4 128 write-allocate write-back lru < traces/mixed.trace"
expected_outputs[cmd40]="expected_results/mixed/256_4_128_wa_wb_lru.txt expected_results/mixed/256_4_128_wa_wb_lru_alt.txt"

commands[cmd41]="./csim 1 1 4 write-allocate write-through fifo < traces/misses.trace"
expected_outputs[cmd41]="expected_results/misses/1_1_4_wa_wt_fifo.txt expected_results/misses/1_1_4_wa_wt_fifo_alt.txt"

commands[cmd42]="./csim 1 1024 128 no-write-allocate write-through fifo < traces/misses.trace"
expected_outputs[cmd42]="expected_results/misses/1_1024_128_nwa_wt_fifo.txt expected_results/misses/1_1024_128_nwa_wt_fifo_alt.txt"

commands[cmd43]="./csim 1 1024 128 write-allocate write-through fifo < traces/misses.trace"
expected_outputs[cmd43]="expected_results/misses/1_1024_128_wa_wt_fifo.txt expected_results/misses/1_1024_128_wa_wt_fifo_alt.txt"

commands[cmd44]="./csim 1 1024 128 write-allocate write-back fifo < traces/misses.trace"
expected_outputs[cmd44]="expected_results/misses/1_1024_128_wa_wb_fifo.txt expected_results/misses/1_1024_128_wa_wb_fifo_alt.txt"

commands[cmd45]="./csim 8192 1 16 no-write-allocate write-through fifo < traces/misses.trace"
expected_outputs[cmd45]="expected_results/misses/8192_1_16_nwa_wt_fifo.txt expected_results/misses/8192_1_16_nwa_wt_fifo_alt.txt"

commands[cmd46]="./csim 8192 1 16 write-allocate write-through fifo < traces/misses.trace"
expected_outputs[cmd46]="expected_results/misses/8192_1_16_wa_wt_fifo.txt expected_results/misses/8192_1_16_wa_wt_fifo_alt.txt"

commands[cmd47]="./csim 8192 1 16 write-allocate write-back fifo < traces/misses.trace"
expected_outputs[cmd47]="expected_results/misses/8192_1_16_wa_wb_fifo.txt expected_results/misses/8192_1_16_wa_wb_fifo_alt.txt"

commands[cmd48]="./csim 256 16 64 no-write-allocate write-through fifo < traces/misses.trace"
expected_outputs[cmd48]="expected_results/misses/256_16_64_nwa_wt_fifo.txt expected_results/misses/256_16_64_nwa_wt_fifo_alt.txt"

commands[cmd49]="./csim 128 32 64 write-allocate write-through fifo < traces/misses.trace"
expected_outputs[cmd49]="expected_results/misses/128_32_64_wa_wt_fifo.txt expected_results/misses/128_32_64_wa_wt_fifo_alt.txt"

commands[cmd50]="./csim 2048 4 16 no-write-allocate write-through fifo < traces/misses.trace"
expected_outputs[cmd50]="expected_results/misses/2048_4_16_nwa_wt_fifo.txt expected_results/misses/2048_4_16_nwa_wt_fifo_alt.txt"

commands[cmd51]="./csim 2048 4 16 write-allocate write-through fifo < traces/misses.trace"
expected_outputs[cmd51]="expected_results/misses/2048_4_16_wa_wt_fifo.txt expected_results/misses/2048_4_16_wa_wt_fifo_alt.txt"

commands[cmd52]="./csim 2048 4 16 write-allocate write-back fifo < traces/misses.trace"
expected_outputs[cmd52]="expected_results/misses/2048_4_16_wa_wb_fifo.txt expected_results/misses/2048_4_16_wa_wb_fifo_alt.txt"

commands[cmd53]="./csim 1024 1 128 write-allocate write-through fifo < traces/misses.trace"
expected_outputs[cmd53]="expected_results/misses/1024_1_128_wa_wt_fifo.txt expected_results/misses/1024_1_128_wa_wt_fifo_alt.txt"

commands[cmd54]="./csim 1024 1 128 write-allocate write-back fifo < traces/misses.trace"
expected_outputs[cmd54]="expected_results/misses/1024_1_128_wa_wb_fifo.txt expected_results/misses/1024_1_128_wa_wb_fifo_alt.txt"

commands[cmd55]="./csim 256 4 128 no-write-allocate write-through fifo < traces/misses.trace"
expected_outputs[cmd55]="expected_results/misses/256_4_128_nwa_wt_fifo.txt expected_results/misses/256_4_128_nwa_wt_fifo_alt.txt"

commands[cmd56]="./csim 256 4 128 write-allocate write-through fifo < traces/misses.trace"
expected_outputs[cmd56]="expected_results/misses/256_4_128_wa_wt_fifo.txt expected_results/misses/256_4_128_wa_wt_fifo_alt.txt"

commands[cmd57]="./csim 256 4 128 write-allocate write-back fifo < traces/misses.trace"
expected_outputs[cmd57]="expected_results/misses/256_4_128_wa_wb_fifo.txt expected_results/misses/256_4_128_wa_wb_fifo_alt.txt"

commands[cmd58]="./csim 1 1 4 write-allocate write-through fifo < traces/hits.trace"
expected_outputs[cmd58]="expected_results/hits/1_1_4_wa_wt_fifo.txt expected_results/hits/1_1_4_wa_wt_fifo_alt.txt"

commands[cmd59]="./csim 1 1024 128 no-write-allocate write-through fifo < traces/hits.trace"
expected_outputs[cmd59]="expected_results/hits/1_1024_128_nwa_wt_fifo.txt expected_results/hits/1_1024_128_nwa_wt_fifo_alt.txt"

commands[cmd60]="./csim 1 1024 128 write-allocate write-through fifo < traces/hits.trace"
expected_outputs[cmd60]="expected_results/hits/1_1024_128_wa_wt_fifo.txt expected_results/hits/1_1024_128_wa_wt_fifo_alt.txt"

commands[cmd61]="./csim 1 1024 128 write-allocate write-back fifo < traces/hits.trace"
expected_outputs[cmd61]="expected_results/hits/1_1024_128_wa_wb_fifo.txt expected_results/hits/1_1024_128_wa_wb_fifo_alt.txt"

commands[cmd62]="./csim 8192 1 16 no-write-allocate write-through fifo < traces/hits.trace"
expected_outputs[cmd62]="expected_results/hits/8192_1_16_nwa_wt_fifo.txt expected_results/hits/8192_1_16_nwa_wt_fifo_alt.txt"

commands[cmd63]="./csim 8192 1 16 write-allocate write-through fifo < traces/hits.trace"
expected_outputs[cmd63]="expected_results/hits/8192_1_16_wa_wt_fifo.txt expected_results/hits/8192_1_16_wa_wt_fifo_alt.txt"

commands[cmd64]="./csim 8192 1 16 write-allocate write-back fifo < traces/hits.trace"
expected_outputs[cmd64]="expected_results/hits/8192_1_16_wa_wb_fifo.txt expected_results/hits/8192_1_16_wa_wb_fifo_alt.txt"

commands[cmd65]="./csim 256 16 64 no-write-allocate write-through fifo < traces/hits.trace"
expected_outputs[cmd65]="expected_results/hits/256_16_64_nwa_wt_fifo.txt expected_results/hits/256_16_64_nwa_wt_fifo_alt.txt"

commands[cmd66]="./csim 128 32 64 write-allocate write-through fifo < traces/hits.trace"
expected_outputs[cmd66]="expected_results/hits/128_32_64_wa_wt_fifo.txt expected_results/hits/128_32_64_wa_wt_fifo_alt.txt"

commands[cmd67]="./csim 2048 4 16 no-write-allocate write-through fifo < traces/hits.trace"
expected_outputs[cmd67]="expected_results/hits/2048_4_16_nwa_wt_fifo.txt expected_results/hits/2048_4_16_nwa_wt_fifo_alt.txt"

commands[cmd68]="./csim 2048 4 16 write-allocate write-through fifo < traces/hits.trace"
expected_outputs[cmd68]="expected_results/hits/2048_4_16_wa_wt_fifo.txt expected_results/hits/2048_4_16_wa_wt_fifo_alt.txt"

commands[cmd69]="./csim 2048 4 16 write-allocate write-back fifo < traces/hits.trace"
expected_outputs[cmd69]="expected_results/hits/2048_4_16_wa_wb_fifo.txt expected_results/hits/2048_4_16_wa_wb_fifo_alt.txt"

commands[cmd70]="./csim 1024 1 128 write-allocate write-through fifo < traces/hits.trace"
expected_outputs[cmd70]="expected_results/hits/1024_1_128_wa_wt_fifo.txt expected_results/hits/1024_1_128_wa_wt_fifo_alt.txt"

commands[cmd71]="./csim 1024 1 128 write-allocate write-back fifo < traces/hits.trace"
expected_outputs[cmd71]="expected_results/hits/1024_1_128_wa_wb_fifo.txt expected_results/hits/1024_1_128_wa_wb_fifo_alt.txt"

commands[cmd72]="./csim 256 4 128 no-write-allocate write-through fifo < traces/hits.trace"
expected_outputs[cmd72]="expected_results/hits/256_4_128_nwa_wt_fifo.txt expected_results/hits/256_4_128_nwa_wt_fifo_alt.txt"

commands[cmd73]="./csim 256 4 128 write-allocate write-through fifo < traces/hits.trace"
expected_outputs[cmd73]="expected_results/hits/256_4_128_wa_wt_fifo.txt expected_results/hits/256_4_128_wa_wt_fifo_alt.txt"

commands[cmd74]="./csim 256 4 128 write-allocate write-back fifo < traces/hits.trace"
expected_outputs[cmd74]="expected_results/hits/256_4_128_wa_wb_fifo.txt expected_results/hits/256_4_128_wa_wb_fifo_alt.txt"

commands[cmd75]="./csim 1 1 4 write-allocate write-through fifo < traces/mixed.trace"
expected_outputs[cmd75]="expected_results/mixed/1_1_4_wa_wt_fifo.txt expected_results/mixed/1_1_4_wa_wt_fifo_alt.txt"

commands[cmd76]="./csim 1 256 128 no-write-allocate write-through fifo < traces/mixed.trace"
expected_outputs[cmd76]="expected_results/mixed/1_256_128_nwa_wt_fifo.txt expected_results/mixed/1_256_128_nwa_wt_fifo_alt.txt"

commands[cmd77]="./csim 256 1 16 write-allocate write-through fifo < traces/mixed.trace"
expected_outputs[cmd77]="expected_results/mixed/256_1_16_wa_wt_fifo.txt expected_results/mixed/256_1_16_wa_wt_fifo_alt.txt"

commands[cmd78]="./csim 256 4 128 no-write-allocate write-through fifo < traces/mixed.trace"
expected_outputs[cmd78]="expected_results/mixed/256_4_128_nwa_wt_fifo.txt expected_results/mixed/256_4_128_nwa_wt_fifo_alt.txt"

commands[cmd79]="./csim 256 4 128 write-allocate write-through fifo < traces/mixed.trace"
expected_outputs[cmd79]="expected_results/mixed/256_4_128_wa_wt_fifo.txt expected_results/mixed/256_4_128_wa_wt_fifo_alt.txt"

commands[cmd80]="./csim 256 4 128 write-allocate write-back fifo < traces/mixed.trace"
expected_outputs[cmd80]="expected_results/mixed/256_4_128_wa_wb_fifo.txt expected_results/mixed/256_4_128_wa_wb_fifo_alt.txt"


# Run tests with each command and its set of expected outputs in sequential order
for i in {1..80}; do
    key="cmd$i"
    IFS=' ' read -r -a expected <<< "${expected_outputs[$key]}"
    compare_output "${commands[$key]}" "${expected[@]}"
done