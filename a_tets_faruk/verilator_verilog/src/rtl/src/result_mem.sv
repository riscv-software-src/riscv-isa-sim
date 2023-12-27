
module result_mem #(
  parameter MEM_WIDTH = 32,
  parameter MEM_DEPTH = 8
) (
  input wire clk_i,
  input wire rst_ni,
  input wire [MEM_WIDTH-1:0] data_i,
  input wire [$clog2(MEM_DEPTH)-1:0] addr_i
);

  reg [MEM_WIDTH-1:0] mem [0:MEM_DEPTH-1];

  always @(posedge clk_i or negedge rst_ni) begin: ff
    integer ii;
    if (!rst_ni) begin
      for (ii = 0; ii < MEM_DEPTH; ii= ii + 1 ) begin
        mem[ii] <= 0;
      end // for
    end else begin
      mem[addr_i] <= data_i;
    end // rst clk
  end // ff
  

endmodule