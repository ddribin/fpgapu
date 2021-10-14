`default_nettype none

module note_player (
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

  // ROM interaface
  output reg  [7:0]         o_rom_addr,
  input wire  [15:0]        i_rom_data
);

  localparam  NOTE_VALUE_BASE             = 8'h00;
  localparam  INSTRUMENT_LENGTHS_BASE     = 8'h80;
  localparam  INSTRUMENT_VALUES_BASE      = 8'h84;
  
  localparam STATE_IDLE                     = 4'd0;
  localparam STATE_YIELD                    = 4'd1;
  localparam STATE_OUTPUT_PITCH_LOW_ADDR    = 4'd2;
  localparam STATE_READ_PITCH_LOW_DATA      = 4'd3;
  localparam STATE_OUTPUT_PITCH_HIGH_ADDR   = 4'd4;
  localparam STATE_READ_PITCH_HIGH_DATA     = 4'd5;
  localparam STATE_DONE                     = 4'd6;


  localparam STATE_WIDTH = 4;
  reg [STATE_WIDTH-1:0]   state, state_nxt;

  reg [5:0]   pitch, pitch_nxt;
  reg [4:0]   duration, duration_nxt;
  reg [3:0]   instrument, instrument_nxt;

  reg [7:0]   pitch_addr, pitch_addr_nxt;
  reg [7:0]   instrument_len_addr, instrument_len_addr_nxt;
  reg [7:0]   instrument_value_addr, instrument_value_addr_nxt;
  reg [3:0]   instrument_value, instrument_value_nxt;
  reg [3:0]   instrument_len, instrument_len_nxt;
  reg [3:0]   instrument_count, instrument_count_nxt;

  reg         done, done_nxt;
  reg [31:0]  phase_delta, phase_delta_nxt;
  reg [8:0]   envelope, envelope_nxt;

  reg [7:0]   rom_addr;

  always @(*) begin
    state_nxt = state;

    rom_addr = 0;

    pitch_nxt = pitch;
    duration_nxt = duration;
    instrument_nxt = instrument;

    done_nxt = done;
    phase_delta_nxt  = phase_delta;
    envelope_nxt = envelope;

    pitch_addr_nxt = pitch_addr;
    instrument_len_addr_nxt = instrument_len_addr;
    instrument_value_addr_nxt = instrument_value_addr;

    case (state)
      STATE_IDLE: begin
        if (i_tick) begin
          pitch_nxt = i_pitch;
          duration_nxt = i_duration;
          instrument_nxt = i_instrument;

          pitch_addr_nxt = {1'b0, i_pitch, 1'b0};
          instrument_len_addr_nxt = INSTRUMENT_LENGTHS_BASE + {6'h00,  i_instrument[3:2]};
          instrument_value_addr_nxt = INSTRUMENT_VALUES_BASE + {2'b00, i_instrument, 1'b0};

          state_nxt = STATE_OUTPUT_PITCH_LOW_ADDR;
        end
      end

      STATE_OUTPUT_PITCH_LOW_ADDR: begin
        rom_addr = pitch_addr;
        pitch_addr_nxt = pitch_addr_nxt + 1;
        
        state_nxt = STATE_READ_PITCH_LOW_DATA;
      end

      STATE_READ_PITCH_LOW_DATA: begin
        phase_delta_nxt[15:0] = i_rom_data;
        rom_addr = pitch_addr;

        state_nxt = STATE_READ_PITCH_HIGH_DATA;
      end

      STATE_OUTPUT_PITCH_HIGH_ADDR: begin
        rom_addr = pitch_addr;
        
        state_nxt = STATE_READ_PITCH_HIGH_DATA;
      end

      STATE_READ_PITCH_HIGH_DATA: begin
        phase_delta_nxt[31:16] = i_rom_data;
        done_nxt = 1;

        state_nxt = STATE_DONE;
      end

      STATE_DONE: begin
        done_nxt = 0;

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
      duration <= duration_nxt;
      instrument <= instrument_nxt;

      done <= done_nxt;
      phase_delta <= phase_delta_nxt;
      instrument <= instrument_nxt;

      pitch_addr <= pitch_addr_nxt;
      instrument_len_addr <= instrument_len_addr_nxt;
      instrument_value_addr <= instrument_value_addr_nxt;
    end
  end

  assign o_done = done;
  assign o_phase_delta = phase_delta;
  assign o_envelope = envelope;
  assign o_rom_addr = rom_addr;
endmodule

