`default_nettype none

module duration_counter (
  input wire          i_clk,
  input wire          i_rst,
  input wire          i_enable,

  input wire          i_load,
  input wire [4:0]    i_duration,

  output wire         o_done,
  output wire         o_running
);

  localparam STATE_STOPPED    = 0;
  localparam STATE_RUNNING    = 1;

  reg         state, state_nxt;
  reg [4:0]   duration, duration_nxt = 0;
  reg         done;
  reg         running;

  always @(*) begin
    state_nxt = state;
    duration_nxt = duration;
    done = 1'b0;
    running = 1'b0;

    case (state)
      STATE_STOPPED: begin
        running = 1'b0;
        if (i_enable && i_load) begin
          duration_nxt = i_duration;
          state_nxt = STATE_RUNNING;
        end
      end

      STATE_RUNNING: begin
        running = 1'b1;
        if (i_enable) begin
          if (duration == 0) begin
            done = 1'b1;
            state_nxt = STATE_STOPPED;
          end else begin
            duration_nxt = duration - 1;
          end
        end
      end

      default: begin
        state_nxt = STATE_STOPPED;
      end
    endcase
  end

  always @(posedge i_clk) begin
    if (i_rst) begin
      state <= STATE_STOPPED;
    end else begin
      state <= state_nxt;
      duration <= duration_nxt;
    end
  end

  assign o_done = done;
  assign o_running = running;

endmodule
