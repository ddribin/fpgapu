`default_nettype none

module strobe (
  input wire        clk,
  input wire        reset,
  input wire        strobe_in,

  output reg [3:0]  counter_1,
  output reg [3:0]  counter_2
);
  
  // Count clock ticks for simulation
  reg [7:0] clk_count;
  reg       strobe_in_q;
  
  always @(posedge clk) begin
    if (reset) begin
      clk_count <= 0;
    end else begin
      clk_count <= clk_count + 1;
    end
  end

  always @(posedge clk) begin
    if (reset) begin
      strobe_in_q <= 0;
    end else begin
      strobe_in_q <= strobe_in;
    end
  end

  always @(posedge clk) begin
    if (reset) begin
      counter_1 <= 0;
    end else if (strobe_in) begin
      counter_1 <= counter_1 + 1;
    end
  end

  always @(posedge clk) begin
    if (reset) begin
      counter_2 <= 0;
    end else if (strobe_in_q) begin
      counter_2 <= counter_2 + 1;
    end
  end

endmodule
