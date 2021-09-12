#!/usr/bin/env ruby

require 'pp'
require 'optparse'

NOTES = [
  ["RST", 0],
  [],
  ["A2",  110.0000],
  [],
  ["C3",  130.8128],
  ["D3",  146.8324],
  ["Ds3", 155.5635],
  ["E3",  164.8138],
  ["F3",  174.6141],
  ["Fs3", 184.9972],
  ["G3",  195.9977],
  ["A3",  220.0000],
  ["As3", 233.0819],
  ["B3",  246.9417],
  [],
  ["C4",  261.6256],
  ["Cs4", 277.1826],
  ["D4",  293.6648],
  ["Ds4", 311.1270],
  ["E4",  329.6276],
  ["F4",  349.2282],
  ["Fs4", 369.9944],
  ["Gs4", 415.3047],
  ["A4",  440.0000],
  ["As4", 466.1638],
  ["B4",  493.8833],
  [],
  ["C5",  523.2511],
  ["Cs5", 554.3653],
  ["Fs5", 739.9888],
  ["Gs5", 830.6094],
  ["A5",  880.0000],
  ["As5", 932.3275],
  ["B5",  987.7666],
  [],
  ["C6",  1046.502],
  ["Cs6", 1108.731],
  ["Fs6", 1479.978],
  ["Gs6", 1661.219],
  ["A6",  1760.000],
  ["As6", 1864.655],
  ["B6",  1975.533],
  [],
  ["Cs7", 2217.461 ],
  [],
  ["C8",  4186.009],
]
SAMPLE_HZ = 25_000_000
BPM = 180

COMMAND = File.basename($0)
USAGE = "Usage: #{$COMMAND} [defines | table]"

def print_note_defines
  puts '`ifndef NOTE_TABLE_VH'
  puts '`define NOTE_TABLE_VH'
  puts

  i = 0
  NOTES.each do |note|
    if note.count == 0
      puts
      next
    end
  
    name = note[0]
    freq_hz = note[1]
    printf "`define NOTE_%-3s  6'd%-3d  // %11.5f Hz\n", name, i, freq_hz
    i += 1
  end

  puts
  puts '`endif'
end

def freq_to_count(freq_hz)
  count = (freq_hz * 2**32 / SAMPLE_HZ).round
  return count
end

def print_note_table
  i = 0
  NOTES.each do |note|
    if note.count == 0
      puts
      next
    end

    name = note[0]
    freq_hz = note[1]
    count = freq_to_count(count)
    printf "%08X  // %-3s %11.5f Hz\n", count, name, freq_hz
    i += 1
  end

  separator = "\n"
  while i < 64
    puts "#{separator}00000000"
    separator = ""
    i += 1
  end
end

def print_duration_table
  tick_per_sec = 60
  ticks_per_min = 60*tick_per_sec
  notes_per_beat = 4

  beats_per_tick = BPM.to_f / ticks_per_min
  notes_per_tick = notes_per_beat * beats_per_tick
  ticks_per_note = 1/ notes_per_tick
  sec_per_note = ticks_per_note / tick_per_sec
  clocks_per_note = SAMPLE_HZ * sec_per_note

  (1..32).each_with_index do |note_len, index|
    duration = (note_len * clocks_per_note).round
    printf "      5'd%02d: r_duration = 32'd%d;\n", index, duration
  end
end


# t = fCPU/(16*fpulse) - 1
# fpulse = fCPU/(16*(t+1))
FAMI_CPU_HZ = 1.789773 * 10**6 # 1.789773 MHz
def freq_to_famicount(freq_hz)
  famicount = FAMI_CPU_HZ/(16.0*freq_hz) - 1
  return famicount.round
end

def famicount_to_freq(famicount)
  freq_hz = FAMI_CPU_HZ/(16*(famicount + 1))
end

def notes_by_name
  hash = {}
  NOTES.each do |note|
    next if note.count == 0
    hash[note[0]] = note[1]
  end
  return hash
end

VIBRATO_SPEED = [0, 64, 32, 21, 16, 13, 11, 9, 8, 7, 6, 5, 4]
VIBRATO_DEPTH = [
  0x01, 0x03, 0x05, 0x07, 0x09, 0x0D, 0x13, 0x17,
  0x1B, 0x21, 0x2B, 0x3 , 0x57, 0x7F, 0xBf, 0xFF]

def print_vibrato(speed, depth, note_name)
  puts "note: #{note_name} speed: #{speed} depth: #{depth}"

  notes = notes_by_name
  base_freq = notes[note_name]
  base_count = freq_to_count(base_freq)
  base_famicount = freq_to_famicount(base_freq)
  length = VIBRATO_SPEED[speed]

  printf "base_freq: #{base_freq} base_count: 0x%05X, base_famicount: 0x%03X\n",
    base_count, base_famicount
  puts "length: #{length}"
  (0...length).each do |i|
    value =
      (Math.sin(i* 2.0 * Math::PI / length) *
      (VIBRATO_DEPTH[depth] / 2.0)).round
    famicount = base_famicount + value
    freq = famicount_to_freq(famicount)
    count = base_count - freq_to_count(freq)
    count_sign = "  -"[count <=> 0]
    printf "#{i} = #{value} [0x%03X, %f, %s32'h%04X]\n",
      famicount, freq, count_sign, count.abs
  end
end

if ARGV.length == 0
  $stderr.puts USAGE
  return 1
end

subcommand = ARGV[0]

case subcommand
when "defines"
  print_note_defines
when "table"
  print_note_table
when "duration"
  print_duration_table
when "vibrato"
  print_vibrato(ARGV[1].to_i, ARGV[2].to_i, ARGV[3])
else
  $stderr.puts USAGE
  return 1
end
