#! /usr/bin/env bash

success=yes

. "test_funcs.sh"

use_transactions=no
if [[ $# -ge 1 ]] && [[ "$1" == "-t" ]]; then
  use_transactions=yes
  shift
fi

if [[ $# -ne 1 ]]; then
  >&2 echo "Usage: ./incr_value_concurrent.sh [-t] <port>"
  >&2 echo "  -t    use transactions"
  exit 1
fi
port="$1"

# Make sure this script is being supervised by the supervise program
ensure_supervised

# Start reference server
start_ref_server ${port}

# Wait for server to start
sleep 2

# Use reference client to create a table and populate it with
# one initial tuple
>&2 echo "Setting up table..."
run ./ref_client.rb localhost ${port} "LOGIN alice" "CREATE fruit" "BYE" > /dev/null
run ./ref_client.rb localhost ${port} "LOGIN alice" "PUSH 0" "SET fruit apples" "BYE" > /dev/null

# Spawn two workers

>&2 echo "Spawning workers..."

total_incr=10000

./incr_value_worker.sh ${port} fruit apples ${use_transactions} worker1.out $(expr ${total_incr} / 2) &
worker1_pid=$!
>&3 echo pid ${worker1_pid}

./incr_value_worker.sh ${port} fruit apples ${use_transactions} worker2.out $(expr ${total_incr} / 2) &
worker2_pid=$!
>&3 echo pid ${worker2_pid}

# Wait for workers to finish
>&2 echo "Waiting for workers to finish..."
wait ${worker1_pid}
wait ${worker2_pid}

# Check final count
>&2 echo "Checking final count..."

final_count=$(./ref_client.rb localhost ${port} "LOGIN bob" "GET fruit apples" "TOP" "BYE" | perl -ne '/^\s*DATA\s+(\S+)\s*/ && print $1,"\n"')
if [[ $? -ne 0 ]]; then
  >&2 echo "ref_client.rb exited with non-zero exit code"
  success=no
else
  >&2 echo "Final count (after ${total_incr} increments) is ${final_count}"

  if [[ "${use_transactions}" == "no" ]]; then
    # No transaction mode:
    # final count should be at least 1/2 of attempted increments
    if [[ "${final_count}" -lt "$(expr ${total_incr} / 2)" ]]; then
      >&2 echo "Final count is too low"
      success=no
    elif [[ "${final_count}" -gt "${total_incr}" ]]; then
      >&2 echo "Final count is too high"
      success=no
    else
      >&2 echo "Final count looks reasonable"
    fi
  else
    # For transaction mode, the final count should be exactly equal
    # to the number of transactions that were successful
    worker1_success_count=$(cat worker1.out)
    worker2_success_count=$(cat worker2.out)

    total_success_count=$((worker1_success_count + worker2_success_count))
    >&2 echo "Total number of successful transactions: ${total_success_count}"

    if [[ "${final_count}" -eq "0" ]]; then
      >&2 echo "None of the transactions succeeded?"
      success=no
    elif [[ "${final_count}" = "${total_success_count}" ]]; then
      >&2 echo "Successful transaction count matches final count, good"
    else
      >&2 echo "Successful transaction count did not match final count"
      success=no
    fi
  fi
fi

# Shut down server
>&2 echo "Shutting down server..."
kill -TERM ${SERVER_PID}
sleep 1

if [[ "${success}" = "yes" ]]; then
  >&2 echo "Success!"
  exit 0
fi

exit 1
