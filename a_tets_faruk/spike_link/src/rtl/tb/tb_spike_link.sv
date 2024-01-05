module tb_spike_link;

   localparam DPI_WIDTH       = 32;
  localparam KEY_WIDTH       = 64;
  localparam VALUE_WIDTH     = 128;

  // -verilator asagidaki hatayi verdigi icin MAX_ENTRY_COUNT tanimladim
  // -verilator: passing dynamic arrays to c function as argument not yet supported.
  localparam MAX_ENTRY_COUNT = 16;


  
  //     packed dimension size (dim0)              num entries (dim1)   entry size in packets (dim2)
  logic [DPI_WIDTH-1:0]   key_array_from_c_side   [0:MAX_ENTRY_COUNT-1][0:KEY_WIDTH/DPI_WIDTH-1];
  logic [DPI_WIDTH-1:0]   value_array_from_c_side [0:MAX_ENTRY_COUNT-1][0:VALUE_WIDTH/DPI_WIDTH-1];


  // c tarafindaki datayi verilog tarafina yazacak fonksiyon
  import "DPI-C" function void write_unordered_map_to_sv_open_arrays(
    output logic [KEY_WIDTH-1:0]   key_array  [][],
    output logic [VALUE_WIDTH-1:0] value_array[][],
    output int num_elements_inserted
  );

  // int init(int argc, char **argv)
  import "DPI-C" function int init();
  // !!! daha sonra bunu daha kullanilabilir hale getirmek icin bu sekile donust
  // import "DPI-C" function int init(input int argc, input string argv[]);

  import "DPI-C" function void step();

  import "DPI-C" function void get_last_commit(
    output logic [KEY_WIDTH-1:0]   key_array  [][],
    output logic [VALUE_WIDTH-1:0] value_array[][],
    output int num_elements_inserted
  );

  import "DPI-C" function void clear_last_commit();

  initial begin
    $display("==========================================");
    $display("Hello from tb_spike_link.sv");
    $display("==========================================");
    /* verilator lint_off IGNOREDRETURN */
    init();
    /*lint_on*/
    step();
    $finish();
    // !!! daha sonra bunu daha kullanilabilir hale getirmek icin bu sekile donust
    // init(0, {"spike", "-d", "hello.elf"});
  end

endmodule