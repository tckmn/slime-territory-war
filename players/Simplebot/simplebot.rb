me, board = ARGV.shift.split(',', 2)
board = board.split(',').map{|row| row.split('') }
bw = board.length
bh = board[0].length

meCoords = board.flat_map.with_index {|row, ri|
  row.map.with_index {|col, ci|
    (col == me) ? [ri, ci] : nil
  }.compact
}

spreadDeltas = [*-1..1].product([*-1..1]) - [0, 0]
jumpDeltas = [*-2..2].product([*-2..2]) - spreadDeltas - [0, 0]
spreadTargets = []
jumpTargets = []
meCoords.shuffle.each {|x, y|
  selBlock = ->dx, dy { (0...bw).include?(x + dx) && (0...bh).include?(y + dy) && board[x + dx][y + dy] == '.' }
  mapBlock = ->dx, dy { [x, y, x + dx, y + dy] }
  spreadTargets += spreadDeltas.select{|dx, dy| selBlock[dx, dy] }.map{|dx, dy| mapBlock[dx, dy] }
  jumpTargets += jumpDeltas.select{|dx, dy| selBlock[dx, dy] }.map{|dx, dy| mapBlock[dx, dy] }
}

if spreadTargets.length > 0
  puts spreadTargets.shuffle[0].join(' ')
elsif jumpTargets.length > 0
  puts jumpTargets.shuffle[0].join(' ')
else
  puts '0 0 0 0'
end
