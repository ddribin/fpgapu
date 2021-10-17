`default_nettype none

module pitch_lookup (
  input wire                i_clk,
  input wire                i_rst,
  
  input wire                i_enable,
  input wire [5:0]          i_pitch,

  output wire               o_valid,
  output wire [31:0]        o_phase_delta,

  // ROM interaface
  output reg  [7:0]         o_rom_addr,
  input wire  [15:0]        i_rom_data
);
  
  localparam STATE_IDLE                     = 3'd0;
  localparam STATE_OUTPUT_PITCH_LOW_ADDR    = 3'd1;
  localparam STATE_READ_PITCH_LOW_DATA      = 3'd2;
  localparam STATE_READ_PITCH_HIGH_DATA     = 3'd3;
  localparam STATE_VALID                    = 3'd4;

  localparam STATE_WIDTH = 3;
  reg [STATE_WIDTH-1:0]   state, state_nxt;

  reg [5:0]   pitch, pitch_nxt;
  reg [7:0]   pitch_addr, pitch_addr_nxt;
  reg [31:0]  phase_delta, phase_delta_nxt;
  reg         valid, valid_nxt;

  reg [7:0]   rom_addr;

  always @(*) begin
    state_nxt = state;

    rom_addr = 0;

    pitch_nxt = pitch;
    pitch_addr_nxt = pitch_addr;
    phase_delta_nxt  = phase_delta;
    valid_nxt = valid;

    case (state)
      STATE_IDLE: begin
        if (i_enable) begin
          pitch_nxt = i_pitch;

          pitch_addr_nxt = {1'b0, i_pitch, 1'b0};

          state_nxt = STATE_OUTPUT_PITCH_LOW_ADDR;
        end
      end

      STATE_OUTPUT_PITCH_LOW_ADDR: begin
        rom_addr = pitch_addr;
        pitch_addr_nxt = pitch_addr + 1;
        
        state_nxt = STATE_READ_PITCH_LOW_DATA;
      end

      STATE_READ_PITCH_LOW_DATA: begin
        phase_delta_nxt[15:0] = i_rom_data;
        rom_addr = pitch_addr;

        state_nxt = STATE_READ_PITCH_HIGH_DATA;
      end

      STATE_READ_PITCH_HIGH_DATA: begin
        phase_delta_nxt[31:16] = i_rom_data;
        valid_nxt = 1;

        state_nxt = STATE_VALID;
      end

      STATE_VALID: begin
        valid_nxt = 0;

        state_nxt = STATE_IDLE;
      end

      default: begin
        state_nxt = STATE_IDLE;
      end
    endcase
  end

  always @(posedge i_clk) begin
    if (i_rst) begin
      state <= STATE_IDLE;
    end else begin
      state <= state_nxt;

      pitch <= pitch_nxt;
      pitch_addr <= pitch_addr_nxt;
      phase_delta <= phase_delta_nxt;
      valid <= valid_nxt;
    end
  end

  assign o_valid = valid;
  assign o_phase_delta = phase_delta;
  assign o_rom_addr = rom_addr;
endmodule
