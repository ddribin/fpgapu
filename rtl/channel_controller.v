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
  
  output wire               o_duration_enable
);
  
  localparam STATE_START_NOTE           = 4'd0;
  localparam STATE_ADVANCE_NOTE         = 4'd1;
  localparam STATE_ADVANCE_TICK         = 4'd2;
  localparam STATE_STROBE_PATTERN       = 4'd3;
  localparam STATE_WAIT_PATTERN         = 4'd4;
  localparam STATE_STROBE_PITCH_LOOKUP  = 4'd5;
  localparam STATE_WAIT_PITCH_LOOKUP    = 4'd6;
  localparam STATE_STROBE_DURATION      = 4'd7;

  localparam STATE_WIDTH = 4;
  reg [STATE_WIDTH-1:0]   state, state_nxt;
  
  always @(*) begin
    state_nxt = state;

    case (state)
      STATE_START_NOTE: begin
        if (i_tick_stb && i_note_stb) begin
          state_nxt = STATE_STROBE_PATTERN;
        end
      end

      STATE_ADVANCE_NOTE: begin
        
      end

      STATE_ADVANCE_TICK: begin

      end

      STATE_STROBE_PATTERN: begin
        state_nxt = STATE_WAIT_PATTERN;
      end

      STATE_WAIT_PATTERN: begin
        state_nxt = STATE_STROBE_PITCH_LOOKUP;
      end

      STATE_STROBE_PITCH_LOOKUP: begin
        state_nxt = STATE_WAIT_PITCH_LOOKUP;
      end

      STATE_WAIT_PITCH_LOOKUP: begin
        state_nxt = STATE_STROBE_DURATION;
      end

      STATE_STROBE_DURATION: begin
        state_nxt = STATE_START_NOTE;
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

endmodule
