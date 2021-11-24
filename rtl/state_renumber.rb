#!/usr/bin/env ruby

states = []
max_len = 0;

ARGF.each do |line|
  if (line =~ /localparam\s+(\S+)\s+/)
    state = $1
    if (state != "STATE_WIDTH")
      states.append(state)
      len = state.length
      max_len = len if (len > max_len)
    end
  end
end


width = Math.log2(states.length).ceil

states.each_with_index do |state, i|
  printf("  localparam %-*s = %d'd%d;\n", max_len, state, width, i)
end

puts
printf("  localparam %-*s = %d;\n", max_len, "STATE_WIDTH", width);
