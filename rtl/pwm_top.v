`default_nettype none

module pwm_top (
  input wire  i_Clk,

  input wire  i_Switch_1,
  input wire  i_Switch_2,
  input wire  i_Switch_3,

  output wire io_PMOD_1,
  output wire io_PMOD_2,
  output wire io_PMOD_3,
  output wire o_LED_1
);
  wire i_clk = i_Clk;
  
  wire w_Switch_1;
  synchronizer switch_1_sync (
    .i_clk(i_Clk),
    .i_input(i_Switch_1),
    .o_input_sync(w_Switch_1)
  );

  wire w_Switch_2;
  synchronizer switch_2_sync (
    .i_clk(i_Clk),
    .i_input(i_Switch_2),
    .o_input_sync(w_Switch_2)
  );

  wire w_Switch_3;
  synchronizer switch_3_sync (
    .i_clk(i_Clk),
    .i_input(i_Switch_3),
    .o_input_sync(w_Switch_3)
  );

  // wire [7:0]  w_top;
  // wire        w_top_valid;
  // wire [8:0]  w_compare;
  // wire        w_compare_valid;
  // pwm_sequencer pwm_sequencer(
  //   .i_clk(i_Clk),
  //   .o_top(w_top),
  //   .o_top_valid(w_top_valid),
  //   .o_compare(w_compare),
  //   .o_compare_valid(w_compare_valid)
  // );

  wire w_tick_stb;
  wire w_beat_stb;
  timing_strobe_generator pulse_generator (
    .i_clk(i_clk),
    .o_tick_stb(w_tick_stb),
    .o_beat_stb(w_beat_stb)
  );
  
  wire [8:0] w_compare_pulse_1;
  wire w_frame_pulse;
  channel_1_pulse pulse_1(
    .i_clk(i_Clk),
    .i_tick_stb(w_tick_stb),
    .i_note_stb(w_beat_stb),
    .o_output(w_compare_pulse_1),
    .o_frame_pulse(w_frame_pulse)
  );

  wire [8:0] w_compare_pulse_2;
  channel_2_pulse pulse_2(
    .i_clk(i_Clk),
    .i_tick_stb(w_tick_stb),
    .i_note_stb(w_beat_stb),
    .o_output(w_compare_pulse_2),
    .o_frame_pulse()
  );

  wire [8:0] w_triangle_3_output;
  channel_3_triangle triangle_3(
    .i_clk(i_Clk),
    .i_tick_stb(w_tick_stb),
    .i_note_stb(w_beat_stb),
    .o_output(w_triangle_3_output),
    .o_frame_pulse()
  );

  // Mixer
  wire [8:0] w_compare =
    (w_Switch_1? 0 : w_compare_pulse_1 )
    + (w_Switch_2? 0 : w_compare_pulse_2)
    + (w_Switch_3? 0 : w_triangle_3_output)
    ;
  wire w_pwm;
  wire w_cycle_end;
  pwm pwm(
    .i_clk(i_Clk),
    .i_top(8'hff),
    .i_top_valid(1'b1),
    .i_compare(w_compare),
    .i_compare_valid(1'b1),
    .o_pwm(w_pwm),
    .o_cycle_end(w_cycle_end)
  );

  assign io_PMOD_1 = w_pwm;
  assign io_PMOD_2 = w_frame_pulse;
  assign io_PMOD_3 = ~w_pwm;
  // assign o_LED_1 = w_pwm;

endmodule
