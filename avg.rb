#!/usr/bin/ruby
scores = {}
File.open('out.txt', 'r') {|f|
  line = nil
  while line = f.gets
    player, score = line.split(' ').map(&:to_i)
    scores[player] = [] if !scores[player]
    scores[player].push score
  end
}
File.open('avg.txt', 'w') {|f|
  f.puts scores.map{|k, v| vl = v.length.to_f; [k, v.reduce(:+) / vl] }.sort_by{|k, v| -v }.map{|k, v| "#{k} #{v.round 3}" }
}
