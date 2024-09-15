#! /usr/bin/env ruby

# Execute a transformation on a test image using either c_imgproc
# or asm_imgproc and check whether the transformation succeeds,
# and whether the output image matches the expected output image
# exactly.

require 'open3'

def run(cmd)
  stdout, stderr, status = Open3.capture3(*cmd)
  if !status.exited?
    STDERR.puts "Error: #{cmd[0]} did not exit normally"
    STDERR.puts "Error output was:"
    STDERR.print stderr
    exit 1
  end

  if status.exitstatus != 0
    STDERR.puts "Error: #{cmd[0]} exited with a non-zero exit code (#{status.exitstatus})"
    STDERR.puts "Error output was:"
    STDERR.print stderr
    exit 1
  end
end

if ARGV.length < 3
  STDERR.puts "Usage: ./run_test.rb <exe version> <image stem> <transformation> [<transformation arg> ...]"
  STDERR.puts "   <exe version> is 'c' or 'asm'"
  exit 1
end

exe_version = ARGV[0]
image_stem = ARGV[1]
transformation = ARGV[2]

# If there were arguments to the transformation, they get added to the
# filename base, separated by '_'
transformation_args = ARGV.drop(3)
transformation_args_filename_ext = transformation_args.empty? ? '' : "_#{transformation_args.join('_')}"

# For the composite transformation, the overlay image name will look like
# "input/foobar.png", which we will transform to be just "foobar"
transformation_args_filename_ext.gsub!(/\//, '_')
transformation_args_filename_ext.gsub!(/\.png/, '')
transformation_args_filename_ext.gsub!(/_input/, '')

exe = "./#{exe_version}_imgproc"
if !File.executable?(exe)
  STDERR.puts "#{exe} doesn't exist or is not executable (maybe you need to run make?)"
  exit 1
end

input_filename = "input/#{image_stem}.png"
expected_filename = "expected/#{image_stem}_#{transformation}#{transformation_args_filename_ext}.png"
actual_filename = "actual/#{exe_version}_#{image_stem}_#{transformation}#{transformation_args_filename_ext}.png"
diff_filename = "actual/#{exe_version}_#{image_stem}_#{transformation}#{transformation_args_filename_ext}_diff.png"

#puts "input_filename=#{input_filename}"
#puts "expected_filename=#{expected_filename}"
#puts "actual_filename=#{actual_filename}"

system('mkdir -p actual')

cmd = [exe, transformation, input_filename, actual_filename, *transformation_args]
#puts cmd.join(' ')
run(cmd)

# use Imagemagick compare program to compare images
cmd = ['compare', '-metric', 'mse', expected_filename, actual_filename, diff_filename]
run(cmd)

puts "Test passed!"
exit 0
