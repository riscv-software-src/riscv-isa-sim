
module tb_1
#(
  parameter MEM_DEPTH = 8,
  parameter MEM_WIDTH = 32
);

  reg [MEM_WIDTH-1:0] operand1_mem [0:MEM_DEPTH-1];
  reg [MEM_WIDTH-1:0] operand2_mem [0:MEM_DEPTH-1];


  reg                          clk_i;
  reg                          rst_ni;
  wire [$clog2(MEM_DEPTH)-1:0] operand1_addr_o;
  wire [$clog2(MEM_DEPTH)-1:0] operand2_addr_o;
  wire [$clog2(MEM_DEPTH)-1:0] result_addr_o;
  wire [MEM_WIDTH-1:0]         result_o;

  operation #(
    .MEM_WIDTH(MEM_WIDTH),
    .MEM_DEPTH(MEM_DEPTH)
  ) op_inst (
    .clk_i          (clk_i),
    .rst_ni         (rst_ni),
    .operand1_i     (operand1_mem[operand1_addr_o]),
    .operand2_i     (operand2_mem[operand2_addr_o]),
    .operand1_addr_o(operand1_addr_o),
    .operand2_addr_o(operand2_addr_o),
    .result_addr_o  (result_addr_o),
    .result_o       (result_o)
  );

  result_mem #(
    .MEM_WIDTH(MEM_WIDTH),
    .MEM_DEPTH(MEM_DEPTH)
  ) mem_inst (
    .clk_i (clk_i),
    .rst_ni(rst_ni),
    .data_i(result_o),
    .addr_i(result_addr_o)
  );

  localparam CLK_PERIOD = 10;
  always #(CLK_PERIOD/2) clk_i=~clk_i;


  import"DPI-C" function int test_c(int operand1,int operand2);

  int test_res;
  int val1,val2;

  initial begin: iteration

    clk_i = 0;
    rst_ni = 0;
    #20 rst_ni = 1;

    for (int ii = 0; ii < MEM_DEPTH; ii = ii+1) begin
      operand2_mem[ii] = $random % 10;
      operand1_mem[ii] = $random % 10;
    end


    for (int ii= 0;ii<MEM_DEPTH; ii = ii + 1) begin
      #40 wait(clk_i);
      val1 = operand1_mem[ii];
      val2 = operand2_mem[ii];
      test_res = test_c(val1,val2);

      if (mem_inst.mem[ii] != test_res)begin
        $display("mismatch detected %d: mem:%d; operands: %d + %d = %d", ii,mem_inst.mem[ii], val1, val2, test_res);
      end else begin
        $display("success on element %d: %d + %d = %d", ii, val1, val2, test_res);
      end

    end
    $finish();
  end

endmodule
