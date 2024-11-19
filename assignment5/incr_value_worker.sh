#! /usr/bin/env bash

if [[ $# -ne 6 ]]; then
  >&2 echo "Usage: ./incr_value_worker.sh <port> <table> <key> <use transactions (yes/no)> <count output file> <num incr>"
  exit 1
fi

port="$1"
table="$2"
key="$3"
use_transactions="$4"
output_file="$5"
num_incr="$6"

# keep track of number of successful increments
num_successful=0

error_log=$(basename ${output_file} .out)_err.log

count=0
while [[ "${count}" -lt "${num_incr}" ]]; do
  if [[ "${use_transactions}" = "yes" ]]; then
    ./incr_value -t localhost ${port} bob ${table} ${key} 2> ${error_log}
  else
    ./incr_value localhost ${port} bob ${table} ${key} 2> ${error_log}
  fi

  if [[ $? -eq 0 ]]; then
    num_successful=$(expr ${num_successful} + 1)
  fi

  count=$((count+1))
done

echo "${num_successful}" > ${output_file}
exit 0
