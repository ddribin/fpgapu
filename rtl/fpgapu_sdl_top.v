`default_nettype none

module fpgapu_sdl_top (
  input wire        i_clk,
  output wire [7:0] o_audio_sample
);

  wire [8:0] w_audio_sample;
  audio_processing_unit #(
    .CLOCK_FREQ(48_000),
    .NOTE_TABLE_FILE("resources/note_table_48kHz.txt")
  ) apu (
    .i_clk(i_clk),
    .i_mixer(4'b1111),
    .o_sample(w_audio_sample),
    .o_frame_pulse()
  );

  assign o_audio_sample = w_audio_sample[7:0];

endmodule