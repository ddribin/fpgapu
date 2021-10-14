`default_nettype none

module note_player_tb #(
  localparam DEPTH = 256
) (
  input wire                i_clk,
  input wire                i_rst,

  input wire                i_tick,
  input wire                i_load,
  input wire [5:0]          i_pitch,
  input wire [4:0]          i_duration,
  input wire [3:0]          i_instrument,

  output wire               o_done,
  output wire [31:0]        o_phase_delta,
  output wire [8:0]         o_envelope,

  output wire [7:0]         o_rom_addr,
  output wire [15:0]        i_rom_data,
  input wire  [15:0]        zz_memory[DEPTH]
);

  rom_sync_fake #(
    .WIDTH(16),
    .DEPTH(DEPTH)
  ) rom (
    .clk(i_clk),
    .addr(o_rom_addr),
    .data(i_rom_data),
    .memory(zz_memory)
  );

  note_player player (.*);

endmodule
