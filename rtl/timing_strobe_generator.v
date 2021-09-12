module timing_strobe_generator #(
  parameter CLOCKS_PER_TICK = 415_667 // 25MHz / 60Hz
) (
  input wire i_clk,
  output wire o_tick_stb,
  output wire o_beat_stb
);
  
  localparam TICK_WIDTH = $clog2(CLOCKS_PER_TICK);
  reg   [TICK_WIDTH-1:0]  r_tick_counter = 0;
  always @(posedge i_clk) begin
    if (r_tick_counter == CLOCKS_PER_TICK-1) begin
      r_tick_counter <= 0;
    end else begin
      r_tick_counter <= r_tick_counter + 1;
    end
  end
  assign o_tick_stb = (r_tick_counter == CLOCKS_PER_TICK-1);
  
  localparam  TICKS_PER_BEAT = 5;
  localparam  BEAT_WIDTH = $clog2(TICKS_PER_BEAT);
  reg   [BEAT_WIDTH-1:0]  r_beat_counter = 0;
  always @(posedge i_clk) begin
    if (o_tick_stb) begin
      if (r_beat_counter == TICKS_PER_BEAT-1) begin
        r_beat_counter <= 0;
      end else begin
        r_beat_counter <= r_beat_counter + 1;
      end
    end
  end
  assign o_beat_stb = o_tick_stb & (r_beat_counter == TICKS_PER_BEAT-1);

endmodule
