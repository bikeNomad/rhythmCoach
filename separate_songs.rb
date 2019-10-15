#!/user/bin/env ruby

$ranges = [[0, 215], [288, 524], [546, 731], [755, 995], [1054, 1272]]

def trim(filename)
  bn = File.basename(filename, ".wav")
  $ranges.each_with_index do |range, i|
    ofilename = "#{bn}-#{i+1}.wav"
    puts "#{filename} => #{ofilename} (length #{(range[1]-range[0])/60.0}"
    system("sox #{filename} #{ofilename} trim #{range[0]} =#{range[1]}")
  end
end

ARGV.each do |fn|
  trim(fn)
end
