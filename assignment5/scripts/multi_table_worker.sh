#! /usr/bin/env bash

if [[ $# -ne 3 ]]; then
  >&2 echo "Usage: multi_table_worker.sh <port> <num accounts> <num transactions>"
  exit 1
fi

# Random delay to make it less likely that workers are executing in lockstep
sleep $(ruby -e 'puts (rand(10000)/10000.0)*0.5')

port="$1"
num_acct="$2"
num_trans="$3"

tcount=0

# Keep track of number of transactions which succeeded and failed
succeeded=0
failed=0

while [[ $tcount -lt "${num_trans}" ]]; do
  # Attempt one transaction

  # Randomly determine source tables, destination table
  src1=''
  src2=''
  dest=''
  permutation=$(ruby -e 'puts [1, 2, 3].shuffle.join(" ")')
  for tnum in ${permutation}; do
    if [[ -z "$src1" ]]; then
      src1=$tnum
    elif [[ -z "$src2" ]]; then
      src2=$tnum
    else
      dest=$tnum
    fi
  done

  # Determine random account
  acct_num=$(ruby -e "puts rand(${num_acct})")

  # Determine random withdrawal amounts
  debit1=$(ruby -e "puts rand(9) + 1")
  debit2=$(ruby -e "puts rand(9) + 1")

  echo "acct_num=${acct_num}, src1=${src1}, src2=${src2}, dest=${dest}, debit1=${debit1}, debit2=${debit2}"

  # Attempt transaction!
  ./scripts/ref_client.rb -e localhost "${port}" \
	"LOGIN alice" \
	"BEGIN" \
	"GET t${src1} acct${acct_num}" \
	"PUSH ${debit1}" \
	"SUB" \
	"SET t${src1} acct${acct_num}" \
	"GET t${src2} acct${acct_num}" \
	"PUSH ${debit2}" \
	"SUB" \
	"SET t${src2} acct${acct_num}" \
	"GET t${dest} acct${acct_num}" \
	"PUSH ${debit1}" \
	"PUSH ${debit2}" \
	"ADD" \
	"ADD" \
	"SET t${dest} acct${acct_num}" \
	"COMMIT" \
	"BYE" > /dev/null

  if [[ $? -eq 0 ]]; then
    succeeded=$((succeeded + 1))
  else
    failed=$((failed + 1))
    # Add back off delay here?
  fi

  tcount=$((tcount + 1))
  if [[ $((tcount % 10)) -eq 0 ]]; then
    >&2 echo -n "."
  fi
done

# Report results
echo "succeeded ${succeeded}"
echo "failed ${failed}"
