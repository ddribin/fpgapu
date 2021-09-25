`default_nettype none

module rom_sync_fake #(
  parameter   WIDTH=8, 
  parameter   DEPTH=512, 
  parameter   FILE="",
  localparam  ADDRW=$clog2(DEPTH)
) (
  input wire  clk,
  input wire  [ADDRW-1:0] addr,
  output reg  [WIDTH-1:0] data,

  input reg   [WIDTH-1:0] memory[DEPTH]
);

  always @(posedge clk) begin
    data <= memory[addr];
  end
endmodule
