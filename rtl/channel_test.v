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
  wire w_beat_stb;
  timing_strobe_generator #(
    .CLOCK_FREQ(CLOCK_FREQ)
  ) pulse_generator (
    .i_clk(i_clk),
    .i_rst(i_rst),
    .o_tick_stb(w_tick_stb),
    .o_beat_stb(w_beat_stb)
  );

endmodule
