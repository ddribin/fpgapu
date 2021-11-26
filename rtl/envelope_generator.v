`default_nettype none

module envelope_generator #(
  parameter BASE_ADDRESS = 8'h0
) (
  input wire                i_clk,
  input wire                i_rst,

  input wire                i_load_instrument,
  input wire [3:0]          i_instrument,

  input wire                i_strobe,

  output wire               o_valid,
  output wire [3:0]         o_amplitude,

  // ROM interaface
  output reg  [7:0]         o_rom_addr,
  input wire  [15:0]        i_rom_data
);

  localparam LENGTHS_BASE_ADDRESS = BASE_ADDRESS;
  localparam ENVELOPES_BASE_ADDRESS = BASE_ADDRESS + 8'h4;

  reg [3:0]   r_instrument = 0;
  always @(posedge i_clk) begin
    if (i_rst) begin
      r_instrument <= 0;
    end else if (i_load_instrument) begin
      r_instrument <= i_instrument;
    end
  end

  localparam STATE_START                = 3'd0;
  localparam STATE_OUTPUT_LENGTH_ADDR   = 3'd1;
  localparam STATE_READ_LENGTH_DATA     = 3'd2;
  localparam STATE_OUTPUT_ENVELOPE_ADDR = 3'd3;
  localparam STATE_READ_ENVELOPE_DATA   = 3'd4;
  localparam STATE_OUTPUT_VALID         = 3'd5;

  localparam STATE_WIDTH                = 3;

  reg [STATE_WIDTH-1:0]   state, state_nxt;

  reg [7:0]   rom_addr;

  wire [3:0]   rom_data_fields[0:3];
  assign rom_data_fields[0] = i_rom_data[3:0];
  assign rom_data_fields[1] = i_rom_data[7:4];
  assign rom_data_fields[2] = i_rom_data[11:8];
  assign rom_data_fields[3] = i_rom_data[15:12];

  reg [3:0]   instrument, instrument_nxt;
  reg [3:0]   instrument_length, instrument_length_nxt;

  reg [3:0]   envelope_index, envelope_index_nxt;
  reg [3:0]   amplitude, amplitude_nxt;

  reg         valid, valid_nxt;

  always @(*) begin
    state_nxt = state;

    rom_addr = 8'd0;

    instrument_nxt = instrument;
    instrument_length_nxt = instrument_length;

    envelope_index_nxt = envelope_index;
    
    valid_nxt = valid;
    amplitude_nxt = amplitude;

    case (state)
      STATE_START: begin
        if (i_strobe) begin
          state_nxt = STATE_OUTPUT_LENGTH_ADDR;
          instrument_nxt = r_instrument;
        end
      end

      STATE_OUTPUT_LENGTH_ADDR: begin
        rom_addr = BASE_ADDRESS + {6'b0, instrument[3:2]};
        state_nxt = STATE_READ_LENGTH_DATA;
      end

      STATE_READ_LENGTH_DATA: begin
        instrument_length_nxt = rom_data_fields[instrument[1:0]];
        state_nxt = STATE_OUTPUT_ENVELOPE_ADDR;
      end

      STATE_OUTPUT_ENVELOPE_ADDR: begin
        rom_addr = ENVELOPES_BASE_ADDRESS + {6'b0, instrument[3:2]};
        state_nxt = STATE_READ_ENVELOPE_DATA;
      end

      STATE_READ_ENVELOPE_DATA: begin
        amplitude_nxt = rom_data_fields[instrument[1:0]];
        valid_nxt = 1'b1;
        state_nxt = STATE_OUTPUT_VALID;
      end

      STATE_OUTPUT_VALID: begin
        valid_nxt = 1'b0;
        state_nxt = STATE_START;
      end

      default: begin
        state_nxt = STATE_START;
      end
    endcase
  end

  always @(posedge i_clk) begin
    if (i_rst) begin
      state <= STATE_START;
      instrument <= 0;
      instrument_length <= 0;
      amplitude <= 0;
      valid <= 0;
    end else begin
      state <= state_nxt;

      instrument <= instrument_nxt;
      instrument_length <= instrument_length_nxt;
      amplitude <= amplitude_nxt;
      envelope_index <= envelope_index_nxt;
      valid <= valid_nxt;
    end
  end

  assign o_rom_addr = rom_addr;
  assign o_valid = valid;
  assign o_amplitude = amplitude;
  
endmodule
