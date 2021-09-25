`default_nettype none

module note_sequencer #(
  parameter LENGTH = 15
) (
  input wire          i_clk,
  input wire          i_rst,
  input wire          i_tick_stb,
  input wire          i_note_stb,

  output wire [4:0]   o_rom_addr,
  input wire  [15:0]  i_rom_data


  // input wire          clk,
  // input wire          rst,
  // input wire          advance_note_en,

  // input wire  [7:0]   initial_addr,
  // input wire          initial_addr_valid,

  // output wire [7:0]   rom_rd_addr,
  // input wire  [15:0]  rom_rd_data,

  // output wire [5:0]   note,       // 6 bits
  // output wire [4:0]   note_len,   // 5 bits
  // output wire [1:0]   instrument, // 2 bits
  // output wire         sequence_complete
);
  
  // localparam STATE_IDLE = 0;
  // localparam STATE_OUTPUT_ADDR = 1;
  // localparam STATE_INPUT_DATA = 2;

  reg [4:0] r_duration_count = 0;
  reg [4:0] r_note_index = 0;

  reg [5:0]  r_note = 0;
  reg [4:0]  r_note_len = 0;
  reg        r_note_stb_z = 0;

  always @(posedge i_clk) begin
    if (i_rst) begin
      r_note_index <= 0;
      r_duration_count <= 0;
      r_note_stb_z <= 0;
    end else if (i_note_stb) begin
      if (r_duration_count == r_note_len) begin
        r_duration_count <= 0;
        r_note_index <= r_note_index + 1;
        if (r_note_index == LENGTH) begin
          r_note_index <= 0;
        end
      end else begin
        r_duration_count <= r_duration_count + 1;
      end
    end
    r_note_stb_z <= i_note_stb;
  end
  wire r_new_note = i_note_stb & (r_duration_count == r_note_len);

  assign o_rom_addr = r_note_index;
  always @(*) begin
    r_note = i_rom_data[5:0];
    r_note_len = i_rom_data[10:6];
  end
  
endmodule
