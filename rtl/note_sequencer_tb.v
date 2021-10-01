`default_nettype none

module note_sequencer_tb #(
  localparam DEPTH = 17
) (
  input wire          i_clk,
  input wire          i_rst,
  input wire          i_note_stb_tb,
  output wire         o_new_note_valid,

  input wire [4:0]    i_new_addr,
  input wire [4:0]    i_new_pattern_len,
  input wire          i_new_addr_valid,

  output wire [4:0]   o_rom_addr,
  output wire  [15:0] i_rom_data,

  input wire  [15:0]  zz_memory[DEPTH]
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
  // assign i_rom_data = zz_memory[o_rom_addr];

  reg r_note_stb_tb;
  always @(posedge i_clk) begin
    r_note_stb_tb <= i_note_stb_tb;
  end

  note_sequencer seq (
    .i_clk(i_clk),
    .i_rst(i_rst),
    .i_note_stb(r_note_stb_tb),
    .o_new_note_valid(o_new_note_valid),

    .i_new_addr(i_new_addr),
    .i_new_pattern_len(i_new_pattern_len),
    .i_new_addr_valid(i_new_addr_valid),

    .o_rom_addr(o_rom_addr),
    .i_rom_data(i_rom_data)
  );

endmodule
