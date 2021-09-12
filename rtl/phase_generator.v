`default_nettype none

module phase_generator #(
  parameter FREQ_HZ = 440,
  parameter SAMPLE_HZ = 25_000_000
) (
  input wire          i_clk,
  input wire [31:0]   i_phase_delta,
  input wire          i_phase_delta_valid,
  output wire [31:0]  o_phase
);
  
  reg [31:0]    r_phase = 0;
  reg [31:0]    r_phase_delta = 0;
  always @(posedge i_clk) begin
    if (i_phase_delta_valid) begin
      r_phase_delta <= i_phase_delta;
    end

    r_phase <= r_phase + r_phase_delta;
  end
  assign o_phase = r_phase;
endmodule
