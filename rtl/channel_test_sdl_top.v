`default_nettype none

module fpgapu_sdl_top (
  input wire        i_clk,
  input wire        i_rst,
  input wire  [3:0] i_mixer,
  output wire [7:0] o_audio_sample
);

  wire w_power_on_rst;
  reset_generator reset_generator (
    .i_clk(i_clk),
    .o_rst(w_power_on_rst)
  );
  wire w_rst = w_power_on_rst | i_rst;

  wire [8:0] w_audio_sample;
  channel_test #(
    .CLOCK_FREQ(1_000_000),
    .NOTE_TABLE_FILE("resources/note_table_1MHz.txt")
  ) channel_test (
    .i_clk(i_clk),
    .i_rst(w_rst),
    .o_sample(w_audio_sample)
  );

  assign o_audio_sample = w_audio_sample[7:0];

endmodule
