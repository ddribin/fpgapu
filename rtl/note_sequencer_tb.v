`default_nettype none

module note_sequencer_tb #(
  localparam DEPTH = 17
) (
  input wire          i_clk,
  input wire          i_rst,
  input wire          i_note_stb,
  output wire         o_new_note_valid,

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

  reg r_note_stb;
  always @(posedge i_clk) begin
    r_note_stb <= i_note_stb;
  end

  note_sequencer seq (
    .i_clk(i_clk),
    .i_rst(i_rst),
    .i_note_stb(r_note_stb),
    .o_new_note_valid(o_new_note_valid),

    .o_rom_addr(o_rom_addr),
    .i_rom_data(i_rom_data)
  );

endmodule
