`default_nettype none

module note_sequencer_tb (
  input wire          i_clk,
  input wire          i_rst,
  input wire          i_tick_stb,
  input wire          i_note_stb,

  input wire  [15:0]  i_memory[17],
  output wire [4:0]   o_rom_addr,
  input wire  [15:0]  i_rom_data
);

  wire [4:0]  w_rom_addr;
  wire [15:0] w_rom_data;
  wire [15:0] w_memory;
  rom_sync_fake #(
    .WIDTH(16),
    .DEPTH(17)
  ) rom (
    .clk(i_clk),
    .addr(w_rom_addr),
    .data(w_rom_data),
    .memory()
  );
  assign o_rom_addr = w_rom_addr;
  assign w_memory = i_rom_data;

  note_sequencer seq (
    .i_clk(i_clk),
    .i_rst(i_rst),
    .i_tick_stb(i_tick_stb),
    .i_note_stb(i_note_stb),

    .o_rom_addr(w_rom_addr),
    .i_rom_data(w_rom_data)
  );

endmodule
