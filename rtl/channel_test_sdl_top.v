`default_nettype none

module channel_test_sdl_top #(
  parameter FOO = "foo"
) (
  input wire        i_clk,
  input wire        i_rst,

  output wire       o_tick,
  output wire       o_beat,

  output wire         o_pattern_enable,
  output wire [5:0]   o_pitch,
  output wire [4:0]   o_duration,
  output wire [3:0]   o_instrument,

  input wire  [3:0]   i_mixer,
  output wire         o_sample_valid,
  output wire [31:0]  o_phase,
  output wire [7:0]   o_audio_sample
);

  wire w_power_on_rst;
  reset_generator reset_generator (
    .i_clk(i_clk),
    .o_rst(w_power_on_rst)
  );
  wire w_rst = w_power_on_rst | i_rst;

  wire [8:0]  w_audio_sample;
  wire        w_sample_valid;
  channel_test #(
    .CLOCK_FREQ(1_000_000),
    .NOTE_TABLE_FILE("resources/note_table_1MHz_full.txt"),
    .PATTERN_FILE("resources/song_scale_rom.txt")
  ) channel_test (
    .i_clk(i_clk),
    .i_rst(w_rst),
    .o_tick(o_tick),
    .o_beat(o_beat),
    .o_pattern_enable(o_pattern_enable),
    .o_pitch(o_pitch),
    .o_duration(o_duration),
    .o_instrument(o_instrument),
    .o_sample_valid(w_sample_valid),
    .o_phase(o_phase),
    .o_sample(w_audio_sample)
  );

  assign o_audio_sample = w_audio_sample[7:0];
  assign o_sample_valid = w_sample_valid;

endmodule
