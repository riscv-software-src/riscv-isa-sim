
module operation #(
  parameter MEM_WIDTH = 32,
  parameter MEM_DEPTH = 8
) (
  input wire                          clk_i,
  input wire                          rst_ni,
  input wire [MEM_WIDTH-1:0]          operand1_i,
  input wire [MEM_WIDTH-1:0]          operand2_i,
  output wire [$clog2(MEM_DEPTH)-1:0] operand1_addr_o,
  output wire [$clog2(MEM_DEPTH)-1:0] operand2_addr_o,
  output wire [$clog2(MEM_DEPTH)-1:0] result_addr_o,
  output wire [MEM_WIDTH-1:0]         result_o
);

  reg [$clog2(MEM_DEPTH)-1:0] addr;
  always @(posedge clk_i or negedge rst_ni) begin: ff
    if (!rst_ni) begin
      addr <= 0;
    end else begin
      addr <= addr + 1;
    end
  end // ff

  assign operand1_addr_o = addr;
  assign operand2_addr_o = addr;
  assign result_addr_o = addr;
  assign result_o = operand1_i + operand2_i;
  
endmodule