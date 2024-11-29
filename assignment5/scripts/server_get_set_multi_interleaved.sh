#! /usr/bin/env bash

success=yes

. "scripts/test_funcs.sh"

if [[ "$#" -ne "1" ]]; then
  >&2 echo "Usage: ./server_get_set_multi_interleaved.sh <port>"
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

# Create actual output directory if necessary
mkdir -p actual

# Create FIFOs for each client
>&2 echo "Creating FIFOs for reference clients..."
fifo_1="${stem}"_fifo_1.fifo
mkfifo "${fifo_1}"
fifo_2="${stem}"_fifo_2.fifo
mkfifo "${fifo_2}"
fifo_3="${stem}"_fifo_3.fifo
mkfifo "${fifo_3}"

# Start reference clients in the background, reading their commands
# from the FIFO and sending their responses to actual output files
>&2 echo "Starting reference clients..."
./scripts/ref_client.rb localhost "${port}" '--' < "${fifo_1}" > actual/"${stem}"_1.out &
CLIENT_1_PID=$!
>&3 echo "pid ${CLIENT_1_PID}"
./scripts/ref_client.rb localhost "${port}" '--' < "${fifo_2}" > actual/"${stem}"_2.out &
CLIENT_2_PID=$!
>&3 echo "pid ${CLIENT_2_PID}"
./scripts/ref_client.rb localhost "${port}" '--' < "${fifo_3}" > actual/"${stem}"_3.out &
CLIENT_3_PID=$!
>&3 echo "pid ${CLIENT_3_PID}"

# Open the FIFOs for writing
>&2 echo "Opening client FIFOs for writing..."
exec 7> "${fifo_1}"
exec 8> "${fifo_2}"
exec 9> "${fifo_3}"

# Now we can interleave commands among the various clients
>&2 echo "Sending requests..."
>&7 echo "LOGIN alice"
sleep 1
>&8 echo "LOGIN bob"
sleep 1
>&9 echo "LOGIN cornelius"
sleep 1
>&7 echo "CREATE fruit"
sleep 1
>&8 echo "PUSH 12"
sleep 1
>&8 echo "SET fruit apples"
sleep 1
>&9 echo "GET fruit apples"
sleep 1
>&9 echo "TOP"
sleep 1
>&7 echo "BYE"
>&8 echo "BYE"
>&9 echo "BYE"

sleep 1

# Close FIFOs
>&2 echo "Closing FIFOs..."
exec 7>&-
exec 8>&-
exec 9>&-

# Wait for clients to finish
>&2 echo "Waiting for clients to finish..."
wait ${CLIENT_1_PID}
wait ${CLIENT_2_PID}
wait ${CLIENT_3_PID}

# Clean up FIFOs
>&2 echo "Cleaning up FIFOs..."
rm -f "${fifo_1}" "${fifo_2}" "${fifo_3}"

# Shut down server
>&2 echo "Shutting down server..."
kill -TERM ${SERVER_PID}
sleep 1

# Check client outputs against expected outputs
>&2 echo "Comparing expected outputs against actual outputs..."
diff_output "expected/${stem}_1.out" "actual/${stem}_1.out"
diff_output "expected/${stem}_2.out" "actual/${stem}_2.out"
diff_output "expected/${stem}_3.out" "actual/${stem}_3.out"

if [[ "${success}" = "yes" ]]; then
  >&2 echo "Success!"
  exit 0
fi

exit 1
