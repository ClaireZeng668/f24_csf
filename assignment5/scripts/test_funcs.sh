#! /usr/bin/env bash

# Start server, report its pid to the "supervise" parent process,
# and record its pid as SERVER_PID.
# Use -n <num fds> option to set a limit on the maximum number
# of file descriptors the server can have open.
start_server() {
  max_fds='0'
  if [[ $# -ge 2 ]] && [[ "$1" = '-n' ]]; then
    shift
    max_fds="$1"
    >&2 echo "Limiting max server file descriptors to ${max_fds}"
    shift
  fi

  local port="$1"

  >&2 echo "Starting server..."
  if [[ "${max_fds}" -gt 0 ]]; then
    (ulimit -n "${max_fds}" && exec ./server ${port}) 2> server_err.log &
  else
    ./server ${port} 2> server_err.log &
  fi
  SERVER_PID=$!
  >&3 echo "pid ${SERVER_PID}"
}

# Run a command, and set $success to "no" if it doesn't exit in the
# expected way. Invoke with -e as first argument if command is expected to fail.
run() {
  local expect_error=no
  if [[ $# -ge 1 ]] && [[ "$1" = "-e" ]]; then
    shift
    expect_error=yes
  fi

  local exe="$1"
  shift

  $exe "$@"

  if [[ "${expect_error}" = "no" ]] && [[ $? -ne 0 ]]; then
    >&2 echo "test command exited with non-zero exit code"
    success=no
  elif [[ "${expect_error}" = "yes" ]] && [[ $? -eq 0 ]]; then
    >&2 echo "test command exited with zero exit code (but was expected to fail)"
    success=no
  fi
}

# Use reference client to check that a specified value exists
check_value_exists() {
  local port="$1"
  local expected="$2"
  local table="$3"
  local key="$4"

  data=$(./scripts/ref_client.rb localhost ${port} "LOGIN bob" "GET ${table} ${key}" "TOP" "BYE" | perl -ne '/^\s*DATA\s+(\S*)\s*$/ && print $1,"\n"')
  if [[ $? -ne 0 ]]; then
    >&2 echo "ref_client.rb exited with non-zero exit code"
    success=no
  elif [[ "${data}" != "${expected}" ]]; then
    >&2 echo "Data value '${data}' for ${table}/${key} doesn't match expected value '${expected}'"
    success=no
  fi
}

# Check to make sure that a test script is being supervised by
# the supervise program
ensure_supervised() {
  >&3 echo "ignore 0" 2> /dev/null
  if [[ $? -ne 0 ]]; then
    >&2 echo "This script must be run using the supervise program:"
    >&2 echo "e.g.,  ./supervise ./name_of_script.sh <arguments...>"
    exit 1
  fi
}

# Compare expected output to actual output, setting success to "no"
# if they aren't identical
diff_output() {
  local expected="$1"
  local actual="$2"

  >&2 echo "Comparing expected output (${expected}) to actual output (${actual})..."
  >&2 diff -u "${expected}" "${actual}"
  if [[ $? -ne 0 ]]; then
    # Actual output didn't match
    >&2 echo "Actual output did not match expected output!"
    success=no
  else
    # Actual output matched
    >&2 echo "Actual output matched expected output, good"
  fi
}

# Send a series of requests read from a file, collect the responses to
# a file, and check whether the responses match the expected responses.
# Also checks whether the client exited with expected exit status.
# Invoke with -e as first argument to indicate that failure is expected.
run_client_check_output() {
  local expect_error=no
  if [[ $# -ge 1 ]] && [[ "$1" == "-e" ]]; then
    shift
    expect_error=yes
  fi

  local port="$1"
  local input="$2"
  local expected="$3"
  local actual="actual/$(basename ${expected})"

  # see how many lines are in input file
  nlines=$(wc -l "${input}" | cut -f 1 -d ' ')

  # run reference client, passing each line of input as one command line argument
  (cat "${input}" | xargs -d '\n' -n "${nlines}" ./scripts/ref_client.rb localhost "${port}") > "${actual}"
  client_exit_code=$?

  # see if client exited with expected exit code
  expected_exit_code_seen=yes
  if [[ "${expect_error}" = "no" ]] && [[ "${client_exit_code}" -ne 0 ]]; then
    expected_exit_code_seen=no
  elif [[ "${expect_error}" = "yes" ]] && [[ "${client_exit_code}" -eq 0 ]]; then
    expected_exit_code_seen=no
  fi

  if [[ "${expected_exit_code_seen}" = "no" ]]; then
    # Didn't see expected exit code
    >&2 echo "ref_client.rb did not exit with expected exit code (it exited with ${client_exit_code})"
    success=no
  else
    # Check expected output against actual output
    diff_output "${expected}" "${actual}"
  fi
}

# Function to fail a test early if setup fails.
# This should be safe as long as the main test script is supervised:
# any background child processes will be terminated by the
# supervisor.
exit_on_failure() {
  if [[ "${success}" = "no" ]]; then
    >&2 echo "Exiting test script early due to setup failure"
    exit 1
  fi
}
