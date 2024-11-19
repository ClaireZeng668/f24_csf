#! /usr/bin/ruby

# Generic client for use by MS1 autograder

require 'socket'

raise "Usage: ./ref_client.rb <hostname> <port> <command 1> [<command 2> ...]" if ARGV.length < 3
hostname = ARGV.shift
port = ARGV.shift

# Connect to server
sock = TCPSocket.new(hostname, port.to_i) 

# Assume all requests will be successful
success = true

# Send each command line argument as a request
ARGV.each do |req|
  # Send request
  sock.puts req

  # Read response
  resp = sock.readline

  # Split response into tokens (note: doesn't work for quoted text,
  # but we ignore that, so it doesn't matter)
  resp_tokens = resp.split
  raise IOError.new('server sent blank response') if resp_tokens.length == 0

  # Print the response
  puts resp

  # Check the response tag to see whether the response indicated
  # success
  resp_tag = resp_tokens[0]
  if resp_tokens[0] == 'ERROR' || resp_tokens[0] == 'FAILED'
    success = false
  end
end

exit success ? 0 : 1
