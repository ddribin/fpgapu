#!/usr/bin/env ruby

(0..63).each_with_index do |i, index|
  radians = i*2*Math::PI/256
  sin_0_to_2_float = Math.sin(radians) + 1.0
  sin_0_to_256_float = 128 * sin_0_to_2_float
  sin_0_to_256_int = sin_0_to_256_float.round
  printf "%03X // %3d\n", sin_0_to_256_int, sin_0_to_256_int
  # reflect = ((sin_0_to_256_int ^ 0xFF) + 1) & 0x1FF
  # printf "%3d: %03X %3d -> %03X %3d\n", index, sin_0_to_256_int, sin_0_to_256_int, reflect, reflect
end
