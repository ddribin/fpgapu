`default_nettype none

module fpga_sdl_top (
  input wire        i_clk,
  output wire [7:0] o_audio_sample
);

assign o_audio_sample = 8'd0;
  
endmodule