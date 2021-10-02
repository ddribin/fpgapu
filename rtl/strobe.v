`default_nettype none

module strobe (
  input wire  clk,
  input wire  strobe_in
);
  
  // Count clock ticks for simulation
  reg [7:0] clk_count;
  reg       strobe_in_q;
  reg [3:0] counter_1;
  reg [3:0] counter_2;
  
  always @(posedge clk) begin
    clk_count <= clk_count + 1;
  end

  always @(posedge clk) begin
    strobe_in_q <= strobe_in;
  end

  always @(posedge clk) begin
    if (strobe_in) begin
      counter_1 <= counter_1 + 1;
    end
  end

  always @(posedge clk) begin
    if (strobe_in_q) begin
      counter_2 <= counter_2 + 1;
    end
  end

endmodule
