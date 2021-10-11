`default_nettype none

module pattern_sequencer #(
  parameter   DEPTH=16, 
  localparam  ADDRW=$clog2(DEPTH)
) (
  input wire                i_clk,
  input wire                i_rst,

  input wire                i_note_stb,
  output wire               o_note_valid,
  output wire [5:0]         o_note,
  output wire [4:0]         o_note_len,
  output wire [3:0]         o_instrument,

  // ROM interaface
  output wire [ADDRW-1:0]   o_rom_addr,
  input wire  [15:0]        i_rom_data
);
  
endmodule
