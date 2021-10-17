`default_nettype none

module pitch_lookup_tv #(
  localparam DEPTH = 256
) (
  input wire                i_clk,
  input wire                i_rst,

  input wire                i_enable,
  input wire [5:0]          i_pitch,

  output wire               o_valid,
  output wire [31:0]        o_phase_delta,

  // ROM interaface
  output wire [7:0]         o_rom_addr,
  output wire [15:0]        i_rom_data,
  input wire  [15:0]        memory[DEPTH]
);

  rom_sync_fake #(
    .WIDTH(16),
    .DEPTH(DEPTH)
  ) rom (
    .clk(i_clk),
    .addr(o_rom_addr),
    .data(i_rom_data),
    .memory(memory)
  );

  pitch_lookup pitch_lookup (.*);

endmodule
