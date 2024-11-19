#! /usr/bin/env bash

# Start reference server, report its pid to the "supervise" parent process,
# and record its pid as SERVER_PID
start_ref_server() {
  local port="$1"

  >&2 echo "Starting server..."
  ./ref_server ${port} 2> server_err.log &
  SERVER_PID=$!
  >&3 echo "pid ${SERVER_PID}"
}

# Run a command, and set $success to "no" if it fails
run() {
  local exe="$1"
  shift
  $exe "$@"
  if [[ $? -ne 0 ]]; then
    >&2 echo "test command exited with non-zero exit code"
    success=no
  fi
}

# Run get_value and check that the correct value is read out.
# Set $success to "no" if the command fails, or if incorrect
# data is read.
check_get_value() {
  local port="$1"
  local expected="$2"
  local table="$3"
  local key="$4"

  actual=$(./get_value localhost ${port} bob ${table} ${key})
  if [[ $? -ne 0 ]]; then
    >&2 echo "get_value exited with non-zero error code"
    success=no
  elif [[ "${actual}" != "${expected}" ]]; then
    >&2 echo "Incorrect value '${actual}' read for ${table}/${key} (expected '${expected}')"
    success=no
  fi
}

# Check that an error is reported correctly by get_value.
check_get_value_err() {
  local port="$1"
  local expected="$2"
  local table="$3"
  local key="$4"

  actual=$(./get_value localhost ${port} bob ${table} ${key} 2>&1)
  if [[ $? -eq 0 ]]; then
    >&2 echo "get_value did not exit with non-zero error code (to report error)"
    success=no
  elif [[ "${actual}" != "${expected}" ]]; then
    >&2 echo "Incorrect error message '${actual}' (expected '${expected}')"
    success=no
  fi
}

# Use reference client to check that a specified value exists
check_value_exists() {
  local port="$1"
  local expected="$2"
  local table="$3"
  local key="$4"

  data=$(./ref_client.rb localhost ${port} "LOGIN bob" "GET ${table} ${key}" "TOP" "BYE" | perl -ne '/^\s*DATA\s+(\S*)\s*$/ && print $1,"\n"')
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

# Check that an error is reported correctly by set_value.
check_set_value_error() {
  local port="$1"
  local expected="$2"
  local table="$3"
  local key="$4"
  local value="$5"

  actual=$(./set_value localhost ${port} alice ${table} ${key} ${value} 2>&1)
  if [[ $? -eq 0 ]]; then
    >&2 echo "set_value did not exit with non-zero error code (to report error)"
    success=no
  elif [[ "${actual}" != "${expected}" ]]; then
    >&2 echo "Incorrect error message '${actual}' (expected '${expected}')"
    success=no
  fi
}

# Check that an error is reported correctly by incr_value.
check_incr_value_error() {
  local port="$1"
  local expected="$2"
  local table="$3"
  local key="$4"

  actual=$(./incr_value localhost ${port} alice ${table} ${key} 2>&1)
  if [[ $? -eq 0 ]]; then
    >&2 echo "incr_value did not exit with non-zero error code (to report error)"
    success=no
  elif [[ "${expected}" = "Error: *" ]]; then
    # Accept any error text (there are multiple ways to implement adding
    # 1 to the value of a tuple, so there isn't consistent error message
    # text we can expect.
    match_count=$(expr "${actual}" : "Error: ")
    if [[ "${match_count}" -ne 7 ]]; then
      >&2 echo "Error message '${actual}' isn't a properly formatted error message"
      success=no
    fi
  elif [[ "${actual}" != "${expected}" ]]; then
    >&2 echo "Incorrect error message '${actual}' (expected '${expected}')"
    success=no
  fi
}
