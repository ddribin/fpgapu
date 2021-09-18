`default_nettype none

module channel_4_noise #(
) (
  input wire          i_clk,
  input wire          i_tick_stb,
  input wire          i_note_stb,
  output wire [8:0]   o_output,
  output wire         o_frame_pulse
);
  
  wire w_mode = '0;

  reg [13:0]    r_shift_register = 14'b1;

  wire [7:0]    w_top;
  wire          w_top_valid;
  wire [31:0]   w_phase_delta;
  wire          w_compare_valid = 1;
  wire [8:0]    w_envelope;
  channel_4_note_sequencer #(
  ) sequencer (
    .i_clk(i_clk),
    .i_tick_stb(i_tick_stb),
    .i_note_stb(i_note_stb),
    .o_top(w_top),
    .o_top_valid(w_top_valid),
    .o_phase_delta(w_phase_delta),
    .o_envelope(w_envelope)
  );

  // // wire [31:0]   w_phase_delta = 'h0E517467;
  // // wire [31:0]   w_phase_delta = 'h0134AB02;
  // wire [31:0]   w_phase_delta = 'h001CD5FA;
  // wire [31:0]   w_phase;
  // reg   [31:0]  r_phase_delta = 0;
  // always @(posedge i_clk) begin
  //   if (r_phase_delta == 0) begin
  //     r_phase_delta <= 'h001CD5FA;
  //   end else if (r_phase_delta >= 'h0134AB02) begin
  //     r_phase_delta <= 'h001CD5FA;
  //   end else begin
  //     r_phase_delta <= r_phase_delta + 'h08;
  //   end
  // end

  wire [31:0]   w_phase;
  wire          w_phase_strobe;
  phase_generator phase_generator (
    .i_clk(i_clk),
    .i_phase_delta(w_phase_delta),
    .i_phase_delta_valid(1'b1),
    .o_phase(w_phase),
    .o_phase_strobe(w_phase_strobe)
  );

  always @(posedge i_clk) begin
    if (w_phase_strobe) begin
      r_shift_register <= {r_shift_register[0] ^ r_shift_register[1], r_shift_register[13:1]};
    end
  end

  assign o_output = r_shift_register[0]? 9'h0B : 9'h00;

endmodule