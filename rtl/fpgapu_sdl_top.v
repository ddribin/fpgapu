`default_nettype none

module fpga_sdl_top (
  input wire        i_clk,
  output wire [7:0] o_audio_sample
);

  wire w_tick_stb;
  wire w_beat_stb;
  timing_strobe_generator #(
    .CLOCKS_PER_TICK(800) // 48,000Hz / 60Hz
  ) pulse_generator (
    .i_clk(i_clk),
    .o_tick_stb(w_tick_stb),
    .o_beat_stb(w_beat_stb)
  );
  
  wire [8:0] w_compare_pulse_1;
  wire w_frame_pulse;
  channel_1_pulse pulse_1(
    .i_clk(i_clk),
    .i_tick_stb(w_tick_stb),
    .i_note_stb(w_beat_stb),
    .o_output(w_compare_pulse_1),
    .o_frame_pulse(w_frame_pulse)
  );

  wire [8:0] w_compare_pulse_2;
  channel_2_pulse pulse_2(
    .i_clk(i_clk),
    .i_tick_stb(w_tick_stb),
    .i_note_stb(w_beat_stb),
    .o_output(w_compare_pulse_2),
    .o_frame_pulse()
  );

  wire [8:0] w_triangle_3_output;
  channel_3_triangle triangle_3(
    .i_clk(i_clk),
    .i_tick_stb(w_tick_stb),
    .i_note_stb(w_beat_stb),
    .o_output(w_triangle_3_output),
    .o_frame_pulse()
  );

  // Mixer
  wire [8:0] w_compare =
    (w_compare_pulse_1 + w_compare_pulse_2 + w_triangle_3_output);

  assign o_audio_sample = w_compare[7:0];

endmodule