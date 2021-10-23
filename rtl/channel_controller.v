`default_nettype none

module channel_controller (
  input wire                i_clk,
  input wire                i_rst,

  input wire                i_tick_stb,
  input wire                i_note_stb,

  output wire               o_pattern_enable,
  input wire                i_pattern_valid,

  output wire               o_pitch_lookup_enable,
  input wire                i_pitch_lookup_valid,
  
  output wire               o_duration_enable,
  output wire               o_duration_load,
  input wire                i_duration_running,

  output wire               o_envelope_enable,
  output wire               o_envelope_load
);

  // To start playing a new note:
  // - Get a new note from pattern sequencer
  // - Lookup the pitch
  // - Load the duration counter
  // - Load the envelope generator

  // To continue playing a note:
  // - If note strobe:
  //    - If duration counter is zero, start new note
  //    - Otherwise decrement duration counter
  // - Advance envelope generator

  // - Output pitch is output of pitch lookup + output of vibrato adjust
  // - Output amplitude is output of envelope generator
  
  localparam STATE_START_NOTE           = 4'd0;
  localparam STATE_ADVANCE_TICK         = 4'd2;
  localparam STATE_ENABLE_PATTERN       = 4'd3;
  localparam STATE_WAIT_PATTERN         = 4'd4;
  localparam STATE_ENABLE_PITCH_LOOKUP  = 4'd5;
  localparam STATE_WAIT_PITCH_LOOKUP    = 4'd6;
  localparam STATE_LOAD_DURATION        = 4'd7;

  localparam STATE_CONTINUE_NOTE        = 4'd1;

  localparam STATE_WIDTH = 4;
  reg [STATE_WIDTH-1:0]   state, state_nxt;

  reg pattern_enable;
  reg pitch_lookup_enable;
  reg duration_enable;
  reg duration_load;
  reg envelope_enable;
  reg envelope_load;
  
  always @(*) begin
    state_nxt = state;

    pattern_enable = 1'b0;
    pitch_lookup_enable = 1'b0;
    duration_enable = 1'b0;
    duration_load = 1'b0;
    envelope_enable = 1'b0;
    envelope_load = 1'b0;

    case (state)
      STATE_START_NOTE: begin
        if (i_tick_stb && i_note_stb) begin
          state_nxt = STATE_ENABLE_PATTERN;
        end
      end

      STATE_CONTINUE_NOTE: begin
        if (i_tick_stb && i_note_stb) begin

        end
      end

      STATE_ADVANCE_TICK: begin

      end

      STATE_ENABLE_PATTERN: begin
        pattern_enable = 1'b1;
        state_nxt = STATE_WAIT_PATTERN;
      end

      STATE_WAIT_PATTERN: begin
        if (i_pattern_valid) begin
          state_nxt = STATE_ENABLE_PITCH_LOOKUP;
        end
      end

      STATE_ENABLE_PITCH_LOOKUP: begin
        pitch_lookup_enable = 1'b1;
        state_nxt = STATE_WAIT_PITCH_LOOKUP;
      end

      STATE_WAIT_PITCH_LOOKUP: begin
        if (i_pitch_lookup_valid) begin
          state_nxt = STATE_LOAD_DURATION;
        end
      end

      STATE_LOAD_DURATION: begin
        duration_enable = 1'b1;
        duration_load = 1'b1;
        state_nxt = STATE_CONTINUE_NOTE;
      end

      default: begin
        state_nxt = STATE_START_NOTE;
      end
    endcase
  end

  always @(posedge i_clk) begin
    if (i_rst) begin
      state <= STATE_START_NOTE;
    end else begin
      state <= state_nxt;
    end
  end

  assign o_pattern_enable = pattern_enable;
  assign o_pitch_lookup_enable = pitch_lookup_enable;
  assign o_duration_enable = duration_enable;
  assign o_duration_load = duration_load;
  assign o_envelope_enable = envelope_enable;
  assign o_envelope_load = envelope_load;

endmodule
