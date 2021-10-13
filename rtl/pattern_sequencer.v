`default_nettype none

module pattern_sequencer #(
) (
  input wire                i_clk,
  input wire                i_rst,

  input wire                i_note_stb,
  output wire               o_note_valid,
  output wire [5:0]         o_note_pitch,
  output wire [4:0]         o_note_len,
  output wire [3:0]         o_note_instrument,

  // ROM interaface
  output reg  [7:0]         o_rom_addr,
  input wire  [15:0]        i_rom_data
);

  localparam STATE_IDLE                   = 0;
  localparam STATE_OUTPUT_ORDER_ADDR      = 1;
  localparam STATE_READ_ORDER_DATA        = 2;
  localparam STATE_OUTPUT_PATTERN_ADDR    = 3;
  localparam STATE_READ_PATTERN_DATA      = 4;
  localparam STATE_OUTPUT_NOTE            = 5;
  localparam STATE_IDLE_IN_PATTERN        = 6;

  localparam STATE_WIDTH = 3;
  reg [STATE_WIDTH-1:0]   state;
  reg [STATE_WIDTH-1:0]   state_nxt;

  reg [7:0]   order_addr;
  reg [7:0]   pattern_addr, pattern_addr_nxt;
  reg [7:0]   pattern_len, pattern_len_nxt;
  reg [7:0]   pattern_count, pattern_count_nxt;

  reg [5:0]   note_pitch, note_pitch_nxt;
  reg [4:0]   note_len, note_len_nxt;
  reg [3:0]   note_instrument, note_instrument_nxt;

  always @(*) begin
    state_nxt = state;

    pattern_addr_nxt = pattern_addr;
    pattern_len_nxt = pattern_len;
    pattern_count_nxt = pattern_count;

    note_pitch_nxt = note_pitch;
    note_len_nxt = note_len;
    note_instrument_nxt = note_instrument;

    case (state)
      STATE_IDLE: begin
        if (i_note_stb) begin
          state_nxt = STATE_OUTPUT_ORDER_ADDR;
        end
      end

      STATE_IDLE_IN_PATTERN: begin
        if (i_note_stb) begin
          state_nxt = STATE_OUTPUT_PATTERN_ADDR;
        end
      end

      STATE_OUTPUT_ORDER_ADDR: begin
        state_nxt = STATE_READ_ORDER_DATA;
      end

      STATE_READ_ORDER_DATA: begin
        pattern_addr_nxt = i_rom_data[7:0];
        pattern_len_nxt = i_rom_data[15:8];
        pattern_count_nxt = 1;

        state_nxt = STATE_OUTPUT_PATTERN_ADDR;
      end

      STATE_OUTPUT_PATTERN_ADDR: begin
        state_nxt = STATE_READ_PATTERN_DATA;
      end

      STATE_READ_PATTERN_DATA: begin
        note_pitch_nxt = i_rom_data[5:0];
        note_len_nxt = i_rom_data[10:6];
        note_instrument_nxt = i_rom_data[14:11];

        state_nxt = STATE_OUTPUT_NOTE;
      end

      STATE_OUTPUT_NOTE: begin
        if (pattern_count < pattern_len) begin
          state_nxt = STATE_IDLE_IN_PATTERN;
        end else begin
          state_nxt = STATE_IDLE;
        end
      end
    endcase
  end

  always @(posedge i_clk) begin
    if (i_rst) begin
      state <= STATE_IDLE;
      order_addr <= '0;
      pattern_addr <= '0;
      pattern_len <= '0;
    end else begin
      state <= state_nxt;

      pattern_addr <= pattern_addr_nxt;
      pattern_len <= pattern_len_nxt;
      pattern_count <= pattern_count_nxt;

      note_pitch <= note_pitch_nxt;
      note_len <= note_len_nxt;
      note_instrument <= note_instrument_nxt;

      case (state)
        STATE_OUTPUT_NOTE: begin
          if (state_nxt == STATE_IDLE_IN_PATTERN) begin
            pattern_addr <= pattern_addr + 1;
            pattern_count <= pattern_count + 1;
          end else begin
            if (order_addr == 8'h01) begin
              order_addr <= 8'h00;
            end else begin
              order_addr <= order_addr + 1;
            end
          end
        end
      endcase
    end
  end

  always @(*) begin
    o_rom_addr = 0;
    if (state == STATE_OUTPUT_ORDER_ADDR) begin
      o_rom_addr = order_addr;
    end else if (state == STATE_OUTPUT_PATTERN_ADDR) begin
      o_rom_addr = pattern_addr;
    end
  end

  assign o_note_valid = (state == STATE_OUTPUT_NOTE);
  assign o_note_pitch = note_pitch;
  assign o_note_len = note_len;
  assign o_note_instrument = note_instrument;
  
endmodule
