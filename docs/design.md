# Pattern Sequencer Layout

# Note Player Layout

Stored in a 256x16 block RAM (BRAM).

There are 64 note pitches (0-63) and looks up a 32-bit value. So this consumes bytes 0-127 of the BRAM.

There can be up to 16 instruments. Each instrument can contain up to 16 values. Each value is 4 bits (0-15).
