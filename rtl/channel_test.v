`default_nettype none

module channel_test #(
  parameter CLOCK_FREQ = 0,
  parameter NOTE_TABLE_FILE = "",
  parameter PATTERN_FILE = ""
) (
  input wire          i_clk,
  input wire          i_rst,
  output wire [8:0]   o_sample
);

  wire w_tick_stb;
  wire w_note_stb;
  timing_strobe_generator #(
    .CLOCK_FREQ(CLOCK_FREQ)
  ) pulse_generator (
    .i_clk(i_clk),
    .i_rst(i_rst),
    .o_tick_stb(w_tick_stb),
    .o_beat_stb(w_note_stb)
  );

  wire [7:0] w_note_rom_addr;
  wire [15:0] w_note_rom_data;
  rom_sync #(
    .WIDTH(16),
    .DEPTH(256),
    .FILE(NOTE_TABLE_FILE)
  ) note_rom (
    .clk(i_clk),
    .addr(w_note_rom_addr),
    .data(w_note_rom_data)
  );

  wire [7:0] w_pattern_rom_addr;
  wire [15:0] w_pattern_rom_data;
  rom_sync #(
    .WIDTH(16),
    .DEPTH(256),
    .FILE(PATTERN_FILE)
  ) pattern_rom (
    .clk(i_clk),
    .addr(w_pattern_rom_addr),
    .data(w_pattern_rom_data)
  );

  wire w_pattern_enable;
  wire w_pattern_valid;
  wire w_pitch_lookup_enable;
  wire w_pitch_lookup_valid;
  wire w_duration_enable;
  channel_controller channel_controller (
    .i_clk(i_clk),
    .i_rst(i_rst),
    .i_note_stb(w_note_stb),
    .i_tick_stb(w_tick_stb),
    .o_pattern_enable(w_pattern_enable),
    .i_pattern_valid(w_pattern_valid),
    .o_pitch_lookup_enable(w_pitch_lookup_enable),
    .i_pitch_lookup_valid(w_pitch_lookup_valid),
    .o_duration_enable(w_duration_enable)
  );

  wire [5:0]  w_pitch;
  wire [4:0]  w_duration;
  wire [3:0]  w_instrument;
  pattern_sequencer pattern_sequencer (
    .i_clk(i_clk),
    .i_rst(i_rst),
    .i_note_stb(w_pattern_enable),

    .o_note_pitch(w_pitch),
    .o_note_len(w_duration),
    .o_note_instrument(w_instrument),
    .o_note_valid(w_pattern_valid),

    .o_rom_addr(w_note_rom_addr),
    .i_rom_data(w_note_rom_data)
  );

  pitch_lookup pitch_lookup (
    .i_clk(i_clk),
    .i_rst(i_rst),
    .i_enable(w_pitch_lookup_enable),

    .i_pitch(w_pitch),
    .o_phase_delta(),
    .o_valid(w_pitch_lookup_valid),

    .o_rom_addr(w_pattern_rom_addr),
    .i_rom_data(w_pattern_rom_data)
  );

  duration_counter duration_counter (
    .i_clk(i_clk),
    .i_rst(i_rst),
    .i_enable(w_duration_enable),
    .i_load(0),
    .i_duration(0),
    .o_done()
  );

endmodule
