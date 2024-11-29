#! /usr/bin/ruby

# Generic client for use by MS2 autograder

require 'socket'

def usage
  STDERR.puts "Usage: ./ref_client.rb [options] <hostname> <port> <command 1> [<command 2> ...]"
  STDERR.puts "       ./ref_client.rb [options] <hostname> <port> --"
  STDERR.puts "(Second form reads commands to send line by line from stdin)"
  STDERR.puts "Options:"
  SRDERR.puts "  -e      Fail immediately if any command results in FAILED or ERROR"
  exit 1
end

# Command reader to read commands line by line from STDIN
class FromSTDIN
  def initialize
    @next = nil
  end

  def at_end()
    begin
      @next = STDIN.readline
      @next.strip!
      return false
    rescue EOFError => e
      return true
    end
  end

  def next
    result = @next
    @next = nil
    return result
  end
end

# Command reader to read commands from ARGV
class FromARGV
  def initialize(arr)
    @index = 0
    @arr = arr
  end

  def at_end()
    return @index >= @arr.length
  end

  def next
    result = @arr[@index]
    @index += 1
    return result
  end
end

$fail_immediately = false
if ARGV.length > 0 && ARGV[0] == '-e'
  $fail_immediately = true
  ARGV.shift
end

usage() if ARGV.length < 3
hostname = ARGV.shift
port = ARGV.shift

# Connect to server
sock = TCPSocket.new(hostname, port.to_i) 

# Assume all requests will be successful
success = true

commands = (ARGV[0] == '--') ? FromSTDIN.new() : FromARGV.new(ARGV)

# Send each command line argument as a request
done = false
while !done && !commands.at_end()
  req = commands.next()

  # Send request
  sock.puts req

  # Read response
  resp = sock.readline

  # Split response into tokens (note: doesn't work for quoted text,
  # but we ignore that, so it doesn't matter)
  resp_tokens = resp.split
  raise IOError.new('server sent blank response') if resp_tokens.length == 0

  # For the MS2 autograder, if the server sends back a FAILED
  # or ERROR response, retain only the first token, since the
  # rest is a quoted text argument whose content we can't
  # predict (and would be mangled by String::split in any case.) 
  if resp_tokens[0] == 'ERROR' || resp_tokens[0] == 'FAILED'
    resp_tokens = [ resp_tokens[0] ]
  end

  # Print the response (reformatted for consistency)
  # to stdout (so that it can be checked against expected
  # responses)
  puts resp_tokens.join(' ')

  # Check the response tag to see whether the response indicated
  # success
  resp_tag = resp_tokens[0]
  if resp_tokens[0] == 'ERROR' || resp_tokens[0] == 'FAILED'
    success = false
    if $fail_immediately
      done = true
    end
  end
end

exit success ? 0 : 1
