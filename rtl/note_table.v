`default_nettype none

module note_table (
  input wire  [5:0]   i_note,
  output wire [31:0]  o_compare
);
  
  // phase delta = (FREQ_HZ / SAMPLE_HZ) * 2^32;
  reg [31:0] note_table[63:0];
  initial begin
    $readmemh("note_table.txt", note_table);
  end
  assign o_compare = note_table[i_note];
endmodule
