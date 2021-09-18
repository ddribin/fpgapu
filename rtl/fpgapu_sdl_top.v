`default_nettype none

module fpgapu_sdl_top (
  input wire        i_clk,
  output wire [7:0] o_audio_sample
);

  wire [8:0] w_audio_sample;
  audio_processing_unit #(
    .CLOCK_FREQ(1_000_000),
    .NOTE_TABLE_FILE("resources/note_table_1MHz.txt"),
    .VIBRATO_TABLE_FILE("resources/vibrato_table_1MHz.txt")
  ) apu (
    .i_clk(i_clk),
    .i_mixer(4'b1000),
    .o_sample(w_audio_sample),
    .o_frame_pulse()
  );

  assign o_audio_sample = w_audio_sample[7:0];

endmodule