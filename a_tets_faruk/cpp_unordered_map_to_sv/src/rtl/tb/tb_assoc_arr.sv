module tb;
  localparam DPI_WIDTH       = 32;
  localparam KEY_WIDTH       = 64;
  localparam VALUE_WIDTH     = 128;

  // -verilator asagidaki hatayi verdigi icin MAX_ENTRY_COUNT tanimladim
  // -verilator: passing dynamic arrays to c function as argument not yet supported.
  localparam MAX_ENTRY_COUNT = 16;



  //     packed dimension size (dim0)              num entries (dim1)   entry size in packets (dim2)
  logic [DPI_WIDTH-1:0]   key_array_from_c_side   [0:MAX_ENTRY_COUNT-1][0:KEY_WIDTH/DPI_WIDTH-1];
  logic [DPI_WIDTH-1:0]   value_array_from_c_side [0:MAX_ENTRY_COUNT-1][0:VALUE_WIDTH/DPI_WIDTH-1];

  typedef logic [KEY_WIDTH-1:0]   key_t;
  typedef logic [VALUE_WIDTH-1:0] value_t;

  // bu verilog tarafinda yapilan simulasyonun urettigi sonucmus
  value_t verilog_side_data [key_t];

  initial begin:some_dummy_operations
    verilog_side_data[1235] = 112312;
    verilog_side_data[123544] = 11 << 32;
    verilog_side_data[1231] = 11 << 64;
  end

  // c tarafindaki datayi verilog tarafina yazacak fonksiyon
  import "DPI-C" function void write_unordered_map_to_sv_open_arrays(
    output logic [KEY_WIDTH-1:0]   key_array  [][],
    output logic [VALUE_WIDTH-1:0] value_array[][],
    output int num_elements_inserted
  );

  // c tarafindan verilog tarafina data tarnsfer yapilirken
  // kac tane eleman gonderilmis
  int num_elements_inserted_from_c_side;
  key_t k;
  initial begin: comparing_with_c_side_data
    #20;

    /* verilator lint_off WIDTHTRUNC */
    // bu yukardaki yorum truncation error'u gormezden geliyor
    if (verilog_side_data.first(k))
    /* lint_on */
    do
      $display("%10d: %0d", k, verilog_side_data[k]);
    while (verilog_side_data.next(k)); // 7.9.6

    // !!! verilog_side_data'nin icerigini yazdir. exists surekli false vermis.
    write_unordered_map_to_sv_open_arrays(
      key_array_from_c_side,
      value_array_from_c_side,
      num_elements_inserted_from_c_side
    );

    $display("\n");
    for (int ii = 0; ii < num_elements_inserted_from_c_side; ii = ii + 1) begin: itr

      key_t key = packed_key(key_array_from_c_side[ii]);

      // $display("key exists: %b", (verilog_side_data.exists(key)));

      // key doesn't exist
      if ((!(verilog_side_data.exists(key)))) begin

        $display(
          "key: %0d inserted from c side not in verilog side\n", key
        );
      // key exists, but values are not equal
      end else if (verilog_side_data[key] != packed_value(value_array_from_c_side[ii])) begin
        $display(
          "key-value-pair: (%0d, %0d) inserted from c side",
          key,
          packed_value(value_array_from_c_side[ii]),
        );
        $display("not equal to the one in verilog");
        $display(
          "side (verilog value = %0d)\n",verilog_side_data[key]
        );

      // key exists, values are equal
      end else begin
        $display(
          "success on key-value-pair: (%0d, %0d)\n",
          key,
          verilog_side_data[key]
        );
      end
    end
    $finish();
  end

  // automatic -> defined variables are auto
  // default (static) -> defined variables are shared among
  // concurrent calls of that function. 13.4.2
  function automatic logic [KEY_WIDTH-1:0] packed_key (
    input  logic [DPI_WIDTH-1:0] parts[0:KEY_WIDTH/DPI_WIDTH-1]
  );
    for (int ii = 0; ii < KEY_WIDTH/DPI_WIDTH; ii = ii + 1) begin:packing
      packed_key[(ii+1)*DPI_WIDTH-1-:DPI_WIDTH] = parts[ii];
    end
  endfunction

  function automatic logic [VALUE_WIDTH-1:0] packed_value (
    input  logic [DPI_WIDTH-1:0]   parts[0:VALUE_WIDTH/DPI_WIDTH-1]
  );
    for (int ii = 0; ii < VALUE_WIDTH/DPI_WIDTH; ii = ii + 1) begin:packing
      packed_value[(ii+1)*DPI_WIDTH-1-:DPI_WIDTH] = parts[ii];
    end
  endfunction

endmodule

