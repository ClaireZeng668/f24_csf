#!/bin/bash

# Function to compare the output of a command to the expected result
compare_output() {
    local command="$1"
    local expected_output="$2"

    # Run the command with a 5-minute timeout and redirect the output to output.txt
    echo "Running: $command"
    timeout 5m bash -c "$command" > output.txt

    # Check if the command timed out (exit code 124 is returned by timeout)
    if [ $? -eq 124 ]; then
        echo "Test Failed for $command: Timed out after 5 minutes"
        echo "----------------------------------------"
        return
    fi

    # Read through the expected and actual output files line by line
    pass=true
    while IFS= read -r expected_line && IFS= read -r actual_line <&3; do
        if [[ "$expected_line" == *"Total cycles:"* ]]; then
            # Extract the cycle values
            expected_cycles=$(echo "$expected_line" | grep -oP '(?<=Total cycles: )\d+')
            actual_cycles=$(echo "$actual_line" | grep -oP '(?<=Total cycles: )\d+')

            # Check if both expected and actual cycles are extracted
            if [ -n "$expected_cycles" ] && [ -n "$actual_cycles" ]; then
                # Calculate 10% margin
                margin=$(echo "$expected_cycles * 0.1" | bc)
                min_cycles=$(echo "$expected_cycles - $margin" | bc)
                max_cycles=$(echo "$expected_cycles + $margin" | bc)

                # Check if the actual cycles are within the 10% margin
                if (( $(echo "$actual_cycles < $min_cycles" | bc -l) )) || (( $(echo "$actual_cycles > $max_cycles" | bc -l) )); then
                    echo "Test Failed for $command"
                    echo "Expected cycles: $expected_cycles"
                    echo "Actual cycles: $actual_cycles"
                    pass=false
                    break
                fi
            else
                pass=false
                break
            fi
        else
            # Compare non-cycles lines
            if [ "$expected_line" != "$actual_line" ]; then
                pass=false
                break
            fi
        fi
    done 3< output.txt < "$expected_output"

    # If the output matched, pass the test
    if $pass; then
        echo "Test Passed"
    else
        echo "Test Failed for $command"
        echo "Expected output:"
        cat "$expected_output"
        echo ""
        echo "Actual output:"
        cat output.txt
    fi

    echo "----------------------------------------"
}

# List of commands and their corresponding expected output files
# feel free to comment some out if you are trying to run specific tests!
# If you do comment one out, comment out the corresponding output file in expected_outputs!
declare -a commands=(
    "./csim 2 4 128 write-allocate write-through lru < traces/read03.trace"
    "./csim 2 4 128 write-allocate write-back lru < traces/read03.trace"
    "./csim 2 4 128 no-write-allocate write-through lru < traces/read03.trace"

    "./csim 2 4 128 write-allocate write-through lru < traces/write01.trace"
    "./csim 2 4 128 write-allocate write-back lru < traces/write01.trace"
    "./csim 2 4 128 no-write-allocate write-through lru < traces/write01.trace"

    "./csim 1 1 4 write-allocate write-through lru < traces/smallcache.trace"
    "./csim 1 16 16 no-write-allocate write-through lru < traces/smallcache.trace"
    "./csim 1 16 16 write-allocate write-through lru < traces/smallcache.trace"
    #"./csim 1 16 16 write-allocate write-back lru < traces/smallcache.trace"
    # "./csim 16 1 16 no-write-allocate write-through lru < traces/smallcache.trace"
    # "./csim 16 1 16 write-allocate write-through lru < traces/smallcache.trace"
    # "./csim 16 1 16 write-allocate write-back lru < traces/smallcache.trace"
    # "./csim 256 4 128 no-write-allocate write-through lru < traces/smallcache.trace"
    # "./csim 256 4 128 write-allocate write-through lru < traces/smallcache.trace"
    # "./csim 256 4 128 write-allocate write-back lru < traces/smallcache.trace"
)

declare -a expected_outputs=(
    "expected_results/read/2_4_128_wa_wt_lru.txt"
    "expected_results/read/2_4_128_wa_wb_lru.txt"
    "expected_results/read/2_4_128_nwa_wt_lru.txt"

    "expected_results/write/2_4_128_wa_wt_lru.txt"
    "expected_results/write/2_4_128_wa_wb_lru.txt"
    "expected_results/write/2_4_128_nwa_wt_lru.txt"

    "expected_results/smallresults/1_1_4_wa_wt_lru.txt" 
    "expected_results/smallresults/1_16_16_nwa_wt_lru.txt"
    "expected_results/smallresults/1_16_16_wa_wt_lru.txt"
    #"expected_results/smallresults/1_16_16_wa_wb_lru.txt"
    # "expected_results/smallresults/16_1_16_nwa_wt_lru.txt"
    # "expected_results/smallresults/16_1_16_wa_wt_lru.txt"
    # "expected_results/smallresults/16_1_16_wa_wb_lru.txt"
    # "expected_results/smallresults/256_4_128_nwa_wt_lru.txt"
    # "expected_results/smallresults/256_4_128_wa_wt_lru.txt"
    # "expected_results/smallresults/256_4_128_wa_wb_lru.txt"
)

for i in "${!commands[@]}"; do
    compare_output "${commands[$i]}" "${expected_outputs[$i]}"
done
