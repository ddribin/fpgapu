`default_nettype none

module audio_processing_unit #(
  parameter CLOCK_FREQ = 0,
  parameter NOTE_TABLE_FILE = ""
) (
  input wire          i_clk,
  input wire  [3:0]   i_mixer,
  output wire [8:0]   o_sample,
  output wire [3:0]   o_frame_pulse
);

  wire w_tick_stb;
  wire w_beat_stb;
  timing_strobe_generator #(
    .CLOCK_FREQ(CLOCK_FREQ)
  ) pulse_generator (
    .i_clk(i_clk),
    .o_tick_stb(w_tick_stb),
    .o_beat_stb(w_beat_stb)
  );
  
  wire [8:0] w_compare_pulse_1;
  wire w_frame_pulse_1;
  channel_1_pulse #(
    .NOTE_TABLE_FILE(NOTE_TABLE_FILE)
  ) pulse_1 (
    .i_clk(i_clk),
    .i_tick_stb(w_tick_stb),
    .i_note_stb(w_beat_stb),
    .o_output(w_compare_pulse_1),
    .o_frame_pulse(w_frame_pulse_1)
  );

  wire [8:0] w_compare_pulse_2;
  wire w_frame_pulse_2;
  channel_2_pulse #(
    .NOTE_TABLE_FILE(NOTE_TABLE_FILE)
  ) pulse_2 (
    .i_clk(i_clk),
    .i_tick_stb(w_tick_stb),
    .i_note_stb(w_beat_stb),
    .o_output(w_compare_pulse_2),
    .o_frame_pulse(w_frame_pulse_2)
  );

  wire [8:0] w_triangle_3_output;
  wire w_frame_pulse_3;
  channel_3_triangle #(
    .NOTE_TABLE_FILE(NOTE_TABLE_FILE)
  ) triangle_3 (
    .i_clk(i_clk),
    .i_tick_stb(w_tick_stb),
    .i_note_stb(w_beat_stb),
    .o_output(w_triangle_3_output),
    .o_frame_pulse(w_frame_pulse_3)
  );

  // Mixer
  assign o_sample =
    (i_mixer[0]? w_compare_pulse_1 : 9'd0) +
    (i_mixer[1]? w_compare_pulse_2 : 9'd0) +
    (i_mixer[2]? w_triangle_3_output : 9'd0);
  
  assign o_frame_pulse = {1'b0, w_frame_pulse_3, w_frame_pulse_2, w_frame_pulse_1};
endmodule
