module rng #(
  parameter DPI_WIDTH       = 32,
  parameter KEY_WIDTH       = 64,
  parameter VALUE_WIDTH     = 128,
  parameter RAND_SEED       = 0,
  parameter RAND_INCR       = 12341,
  parameter RAND_MULT       = 22,
  parameter RAND_SHAMT      = 13
) (
  input  wire                   clk_i,
  input  wire                   rst_ni,
  output reg  [KEY_WIDTH-1:0]   last_wa1_o,
  output reg  [VALUE_WIDTH-1:0] last_wd1_o,
  output reg                    last_we1_o,
  output reg  [KEY_WIDTH-1:0]   last_wa2_o,
  output reg  [VALUE_WIDTH-1:0] last_wd2_o,
  output reg                    last_we2_o
);

localparam RANDOM_WIDTH = VALUE_WIDTH*2;
reg [RANDOM_WIDTH-1:0] random_state;
wire [RANDOM_WIDTH-1:0] random_state_incremented;
wire [RANDOM_WIDTH-1:0] random_state_rotated;

assign random_state_incremented = (random_state + RAND_INCR) * RAND_MULT;
assign random_state_rotated = {random_state_incremented[RANDOM_WIDTH-1:RAND_SHAMT],random_state_incremented[RAND_SHAMT-1:0]};

always @(posedge clk_i or negedge rst_ni) begin
  if (!rst_ni) begin
    random_state <= RAND_SEED;
  end else begin
    random_state <= random_state_rotated;
    last_wa1_o <= random_state[KEY_WIDTH-1:0];
    last_wd1_o <= random_state[VALUE_WIDTH-1:0];
    last_we1_o <= random_state[0];
    last_wa2_o <= random_state[KEY_WIDTH*2-1:KEY_WIDTH];
    last_wd2_o <= random_state[VALUE_WIDTH*2-1:VALUE_WIDTH];
    last_we2_o <= random_state[KEY_WIDTH];
  end
end


endmodule