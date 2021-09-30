`default_nettype none

module note_sequencer #(
  parameter LENGTH = 15
) (
  input wire          i_clk,
  input wire          i_rst,
  input wire          i_note_stb,
  output wire         o_new_note_valid,

  // input wire [4:0]    i_new_addr,
  // input wire          i_new_addr_valid,

  output wire [4:0]   o_rom_addr,
  input wire  [15:0]  i_rom_data
);
  
  reg [4:0] r_duration_count = 0;
  reg [4:0] r_note_index = 0;

  reg [5:0]  r_new_note = '0;
  reg [4:0]  r_new_note_len = '0;
  reg [3:0]  r_new_instrument = '0;
  reg        i_note_stb_q1 = 0;
  reg        r_new_note_valid = '0;
  reg [4:0]  r_note_len = '0;

  always @(posedge i_clk) begin
    if (i_rst) begin
      r_note_index <= 0;
      r_duration_count <= 0;
      i_note_stb_q1 <= 0;
      r_new_note_valid <= 0;
    end else if (i_note_stb) begin
      if (r_duration_count == r_new_note_len) begin
        r_new_note_valid <= 1;
        r_duration_count <= 0;

        r_new_note <= i_rom_data[5:0];
        r_new_note_len <= i_rom_data[10:6];
        r_new_instrument <= i_rom_data[14:11];

        if (r_note_index == LENGTH) begin
          r_note_index <= 0;
        end else begin
          r_note_index <= r_note_index + 1;
        end
      end else begin
        r_new_note_valid <= 0;
        r_duration_count <= r_duration_count + 1;
      end
    end
    i_note_stb_q1 <= i_note_stb;
  end
  assign o_new_note_valid = i_note_stb_q1 & r_new_note_valid;

  assign o_rom_addr = r_note_index;
  
endmodule
