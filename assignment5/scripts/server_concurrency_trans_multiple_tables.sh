#! /usr/bin/env bash

success=yes

. "scripts/test_funcs.sh"

if [[ "$#" -ne "1" ]]; then
  >&2 echo "Usage: ./server_arith.sh <port>"
  exit 1
fi

port="$1"

# Make sure this script is supervised by the supervise program
ensure_supervised

# Start the server
>&2 echo "Starting server"
start_server ${port}

# Input and expected output files will have names based on the
# name of this script
stem=$(basename "$0" .sh)

# Concept: there are three tables, t1, t2, and t3. Each tuple
# in each table represents an account balance, with the key indicating
# the owner. Transactions are run where two accounts (for the same
# owner) are debited by a small amount, and the third account
# (same owner) credited by the sum. The total amount of money
# over all three tables should be the same at the end if transactions
# are used.

NUM_ACCOUNTS=5

# Create tables and initial data
>&2 echo "Creating tables and initial data..."
run ./scripts/ref_client.rb localhost "${port}" \
	"LOGIN alice" \
	"CREATE t1" \
	"CREATE t2" \
	"CREATE t3" \
	"BYE" > /dev/null

# Each account starts with 1,000,000
INITIAL_BALANCE=1000000
i=0
while [[ $i -lt $NUM_ACCOUNTS ]]; do
  run ./scripts/ref_client.rb localhost "${port}" \
	"LOGIN alice" \
	"PUSH ${INITIAL_BALANCE}" \
	"SET t1 acct${i}" \
	"PUSH ${INITIAL_BALANCE}" \
	"SET t2 acct${i}" \
	"PUSH ${INITIAL_BALANCE}" \
	"SET t3 acct${i}" \
	"BYE" > /dev/null
  i=$((i + 1))
done

# If setup failed, end here
exit_on_failure

# Create actual output directory if necessary
mkdir -p actual

# Number of workers to start
NWORKERS=8

# Number of transactions that each worker should attempt
NTRANS=100

# Use an array to keep track of worker pids (since the
# number of workers isn't hard-coded)
worker_pids=()

# Create workers
>&2 echo "Starting worker tasks..."
i=1
while [[ $i -le $NWORKERS ]]; do
  outfile="actual/${stem}_worker${i}.out"
  ./scripts/multi_table_worker.sh "${port}" "${NUM_ACCOUNTS}" "${NTRANS}" > "${outfile}" &
  worker_pid=$!
  >&3 echo "pid ${worker_pid}"
  worker_pids+=( $worker_pid )

  i=$((i + 1))
done

# Wait for workers to finish
>&2 echo "Waiting for workers to finish..."
for worker_pid in ${worker_pids[@]}; do
  wait $worker_pid
done

>&2 echo
>&2 echo "Workers have finished"

# Check number of transactions that succeeded
>&2 echo "Checking results..."
total_successful_transactions=0
i=1
while [[ $i -le $NWORKERS ]]; do
  outfile="actual/${stem}_worker${i}.out"
  worker_nsuccess=$(cat "${outfile}" | egrep "^succeeded" | cut -f 2 -d ' ')
  >&2 echo "Worker ${i}: ${worker_nsuccess}/${NTRANS} transactions succeeded"
  total_successful_transactions=$((total_successful_transactions + worker_nsuccess))
  i=$((i + 1))
done
>&2 echo "${total_successful_transactions} transactions succeeded overall"

# Make sure that at least one transaction succeeded
if [[ "${total_successful_transactions}" -eq 0 ]]; then
  >&2 echo "No transactions succeeded!"
  success=no
fi

# Find sum of all balances, compare against expected total.
# Also, keep track of number of accounts where the final balance
# changed. (There should be at least one.)
>&2 echo "Retrieving all final account balances..."
i=0
total=0
num_changed=0

# for each account...
while [[ $i -lt $NUM_ACCOUNTS ]]; do
  tnum=1

  # for each table...
  while [[ $tnum -le 3 ]]; do
    # get account balance for this table
    balance=$(./scripts/ref_client.rb localhost "${port}" "LOGIN alice" "GET t${tnum} acct${i}" "TOP" "BYE" | egrep "^DATA" | cut -f 2 -d ' ')
    if [[ $? -ne 0 ]]; then
      >&2 echo "Error retrieving balance in table t${tnum} for account acct${i}"
      success=no
    fi

    # check whether the balanced changed
    if [[ "${balance}" -ne "$INITIAL_BALANCE" ]]; then
      num_changed=$((num_changed + 1))
    fi

    # add to running total
    total=$((total + balance))

    # next table
    tnum=$((tnum + 1))
  done

  # next account
  i=$((i + 1))
done

>&2 echo "Number of accounts which changed: ${num_changed}"
if [[ "${num_changed}" -eq 0 ]]; then
  >&2 echo "No account balanced changed: transactions did not actually modify any tables?"
  success=no
fi

>&2 echo "Comparing final account balances to expected total..."
expected=$(($NUM_ACCOUNTS * 3 * $INITIAL_BALANCE))
if [[ "${expected}" -eq "${total}" ]]; then
  >&2 echo "Expected total ${expected} matches observed total ${total}, good"
else
  >&2 echo "Expected total ${expected} does not match observed total ${total}"
  success=no
fi

# Shut down server
>&2 echo "Shutting down server..."
kill -TERM "${SERVER_PID}"
sleep 1

if [[ "${success}" = "yes" ]]; then
  >&2 echo "Success!"
  exit 0
fi

exit 1
