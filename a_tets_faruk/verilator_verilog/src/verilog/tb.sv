
module tb;
  
  import "DPI-C" function int cpp_test();
  reg clk;
  int ii;
  localparam CLK_PERIOD = 20;
  always #(CLK_PERIOD/2) clk= ~clk;
  initial begin: init_blk
    $dumpfile("outputs/output.vcd");
    $dumpvars(0,tb);
    clk =0;
    ii = cpp_test();
    $display("ii: %d",ii);
    #100;
    $finish();
  end
endmodule
