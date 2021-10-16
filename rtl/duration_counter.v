`default_nettype none

module duration_counter (
  input wire          i_clk,
  input wire          i_rst,
  input wire          i_enable,

  input wire          i_load,
  input wire [4:0]    i_duration,

  output wire         o_done
);


// `define LOGIC 1
`define FSM 1

`ifdef FSM
  localparam STATE_STOPPED    = 0;
  localparam STATE_RUNNING    = 1;

  reg         state, state_nxt;
  reg [4:0]   duration, duration_nxt = 0;
  // reg         done, done_nxt;
  reg         done;

  always @(*) begin
    state_nxt = state;
    duration_nxt = duration;
    // done_nxt = done;
    done = 1'b0;

    case (state)
      STATE_STOPPED: begin
        // done_nxt = 0;
        if (i_load) begin
          duration_nxt = i_duration;

          state_nxt = STATE_RUNNING;
        end
      end

      STATE_RUNNING: begin
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
      // done <= (state == STATE_RUNNING) && (state_nxt == STATE_STOPPED);
    end
  end

  // assign o_done = (state == STATE_RUNNING) && (state_nxt == STATE_STOPPED);
  assign o_done = done;
`endif

`ifdef LOGIC
  reg [4:0]   duration = 0;
  reg         running = 0;

  always @(posedge i_clk) begin
    if (i_rst) begin
      duration <= 0;
    end else if (i_load) begin
      duration <= i_duration;
      running <= 1;
    end else if (i_enable) begin
      if (duration > 0) begin
        duration <= duration - 1;
      end else begin
        running <= 0;
      end
    end
  end

  assign o_done = ((duration == 0) & running & i_enable);
`endif
  
endmodule
