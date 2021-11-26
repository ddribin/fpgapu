`default_nettype none

module envelope_generator_tb #(
  localparam DEPTH = 256
) (
  input wire                i_clk,
  input wire                i_rst,

  input wire                i_load_instrument,
  input wire [3:0]          i_instrument,

  input wire                i_strobe,

  output wire               o_valid,
  output wire [3:0]         o_amplitude,

  // ROM interaface
  output reg  [7:0]         o_rom_addr,
  output wire  [15:0]       i_rom_data,
  input wire  [15:0]        zz_memory[DEPTH]
);
  
  reg [7:0] CLK = 0;
  always @(posedge i_clk) begin
    CLK <= CLK + 1;
  end

  rom_sync_fake #(
    .WIDTH(16),
    .DEPTH(DEPTH)
  ) rom (
    .clk(i_clk),
    .addr(o_rom_addr),
    .data(i_rom_data),
    .memory(zz_memory)
  );

  envelope_generator uut (.*);

endmodule
