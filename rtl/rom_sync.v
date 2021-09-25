`default_nettype none

module rom_sync #(
  parameter   WIDTH=8, 
  parameter   DEPTH=512, 
  parameter   FILE="",
  localparam  ADDRW=$clog2(DEPTH)
) (
  input wire logic clk,
  input wire logic [ADDRW-1:0] addr,
  output     logic [WIDTH-1:0] data
);

  reg [WIDTH-1:0] memory [DEPTH];

  initial begin
    if (FILE != 0) begin
      $display("Creating rom_sync from init file '%s'.", FILE);
      $readmemh(FILE, memory);
    end
  end

  always @(posedge clk) begin
    data <= memory[addr];
  end
endmodule
