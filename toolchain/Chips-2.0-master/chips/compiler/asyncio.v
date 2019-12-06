`define LENGTH_WIDTH 24

`define MAX_BUFFER_SIZE 2

module asyncio_reader(
  input clk,
  input rst,
  input [`LENGTH_WIDTH-1:0] length,
  input [ADDRESS_WIDTH-1:0] address,
  input [INPUT_ID_WIDTH-1:0] input_id,
  input [OUTPUT_ID_WIDTH-1:0] output_id,

  input [NO_OF_AXI_INPUTS-1:0] axi_in_valid,
  input [NO_OF_AXI_INPUTS*32-1:0] axi_in_data,
  output [NO_OF_AXI_INPUTS-1:0] axi_in_ready,

  input [NO_OF_AXI_OUTPUTS-1:0] axi_out_ready,
  output [NO_OF_AXI_OUTPUTS-1:0] axi_out_valid,
  output [NO_OF_AXI_OUTPUTS*32-1:0] axi_out_data,

  output reg [ADDRESS_WIDTH-1:0] address_out,
  output reg [`LENGTH_WIDTH-1:0] length_out,

  input will_load
);
  parameter INPUT_ID_WIDTH = 1;
  parameter NO_OF_AXI_INPUTS = 1;
  parameter OUTPUT_ID_WIDTH = 1;
  parameter NO_OF_AXI_OUTPUTS = 1;
  parameter MEMORY_IS_INPUT = 0;
  parameter ADDRESS_WIDTH = 32;

  genvar i;

  reg [`LENGTH_WIDTH-1:0] read_length;
  reg [31:0] s_axi_out_data;

  reg [1:0] buffer_size;
  reg [31:0] buffer[0:0];


  wire [31:0] axi_in_data_array[NO_OF_AXI_INPUTS-1:0];
  for(i = 0; i < NO_OF_AXI_INPUTS; i = i + 1)
    assign axi_in_data_array[i] = axi_in_data[(i + 1) * 32 - 1:i * 32];

  wire s_axi_in_ready = buffer_size < `MAX_BUFFER_SIZE &&
    (read_length != 0 || loading_last_word);
  assign axi_in_ready = s_axi_in_ready << input_id;


  wire s_axi_out_valid = buffer_size > 0;
  assign axi_out_valid = s_axi_out_valid << output_id;

  for(i = 0; i < NO_OF_AXI_OUTPUTS; i = i + 1)
    assign axi_out_data[(i + 1) * 32 - 1:i * 32] = output_id == i ?
      s_axi_out_data : 32'd0;

  wire read_transaction = s_axi_in_ready && axi_in_valid[input_id];
  wire read_transaction_from_mem = MEMORY_IS_INPUT ? read_transaction && !(
    have_read_prev_address && word_from_prev_address) : read_transaction;

  wire write_transaction = s_axi_out_valid && axi_out_ready[output_id];

  reg cycles_since_last_address_change;
  reg have_read_prev_address;

  wire word_from_prev_address = cycles_since_last_address_change == 0;
  reg loading_last_word;

  always @(posedge clk) begin
    loading_last_word <= 0;
    if(rst) begin
      length_out <= 0;
      read_length <= 0;
      buffer_size <= 0;
    end else if(length != 0) begin
      length_out <= length;
      read_length <= length;
      address_out <= address;
      buffer_size <= 0;
      cycles_since_last_address_change <= 0;
      have_read_prev_address <= 0;
    end else if(length_out != 0) begin
      cycles_since_last_address_change <= 1;
      if(write_transaction) begin
        length_out <= length_out - 1;
        if(!MEMORY_IS_INPUT)
          address_out <= address_out + 4;
      end
      if(read_transaction) begin
        if(!MEMORY_IS_INPUT)
          read_length <= read_length - 1;
        if(MEMORY_IS_INPUT) begin
          if(!word_from_prev_address || (will_load && axi_out_ready)) begin
            address_out <= address_out + 4;
            if(read_length == 1 && will_load)
              loading_last_word <= 1;
            if(read_length != 0)
              read_length <= read_length - 1;
            cycles_since_last_address_change <= 0;
            have_read_prev_address <= !word_from_prev_address;
          end
        end
      end

      if(read_transaction_from_mem && write_transaction) begin
        if(buffer_size == 1) begin
          s_axi_out_data <= axi_in_data_array[input_id];
        end else begin
          s_axi_out_data <= buffer[0];
          buffer[0] <= axi_in_data_array[input_id];
        end
      end else if(read_transaction_from_mem) begin
        buffer_size <= buffer_size + 1;
        if(buffer_size == 0) begin
          s_axi_out_data <= axi_in_data_array[input_id];
        end else begin
          buffer[0] <= axi_in_data_array[input_id];
        end
      end else if(write_transaction) begin
        buffer_size <= buffer_size - 1;
        if(buffer_size == 2) begin
          s_axi_out_data <= buffer[0];
        end
      end
    end

  end

endmodule
