  input clk,
  input rst,
  input [`LENGTH_WIDTH-1:0] length,
  input [`ADDRESS_WIDTH-1:0] address,
  input [ID_WIDTH-1:0] io_id,

  input [NO_OF_AXI_IN_PORTS-1:0] axi_in_valid,
  input [NO_OF_AXI_IN_PORTS*32-1:0] axi_in_data,
  output [NO_OF_AXI_IN_PORTS-1:0] axi_in_ready,

  output [NO_OF_AXI_OUT_PORTS-1:0] axi_out_valid,
  output [NO_OF_AXI_OUT_PORTS*32-1:0] axi_out_data,
  input [NO_OF_AXI_OUT_PORTS-1:0] axi_out_ready,

  input memory_op_ready,
  output memory_op_enable,
  output reg [31:0] word_to_store,

  input memory_op_ready,
  output memory_op_enable,
  output reg [31:0] word_to_store,


  output reg [`ADDRESS_WIDTH-1:0] address_out,
  output reg [`LENGTH_WIDTH-1:0] length_out


  assign axi_out_valid = s_axi_out_valid << output_id;
  for(i = 0; i < NO_OF_AXI_OUT_PORTS; i = i + 1)
    assign axi_out_data[(i + 1) * 32:i * 32] = output_id == i ?
      s_axi_out_data : 32'd0;

//--------------------------------------

`timescale 1ns / 1ps

`define ADDRESS_WIDTH 32
`define LENGTH_WIDTH 24

module test;

  reg clk;
  reg rst;

  reg [`LENGTH_WIDTH-1:0] length;
  reg [`ADDRESS_WIDTH-1:0] address;
  reg [0:0] io_id;

  reg store_ready;
  reg [1:0] axi_valids;
  reg [63:0] axi_data;

  wire [1:0] axi_readys;
  wire [31:0] word_to_store;
  wire store_enable;
  wire [`ADDRESS_WIDTH-1:0] address_out;
  wire [`LENGTH_WIDTH-1:0] length_out;

  //should adopt length and start array transaction when non-zero
  //should properly reset
  //should set inactive AXI bits to zero
  //should wait ...
  //should store a word only once (store_enable must become low again)
  //should stop when reaching a length of zero

  asyncio_reader #(
    .ID_WIDTH(1),
    .NO_OF_AXI_PORTS(2)) uut(
    .clk(clk),
    .rst(rst),

    .length(length),
    .address(address),
    .io_id(io_id),

    .memory_op_ready(store_ready),
    .axi_valid(axi_valids),
    .axi_data(axi_data),

    .axi_ready(axi_readys),
    .word_to_store(word_to_store),
    .memory_op_enable(store_enable),
    .address_out(address_out),
    .length_out(length_out));

  initial begin
    rst = 1;
    length = 0;

    #299 rst = 0;
    #1.1
    length = 5;
    address = 4;
    io_id = 0;

    store_ready = 0;
    axi_valids = 2'b11;
    axi_data = {32'd0, 32'd0};

    #10
    length = 0;
    axi_data = {32'd0, 32'd1};

    #10
    axi_data = {32'd0, 32'd2};
    store_ready = 1;

    #10
    axi_data = {32'd0, 32'd3};
    store_ready = 0;

    #20
    store_ready = 1;

    #10
    axi_valids <= 2'b00;

    #20
    axi_valids <= 2'b11;


    //#10;

  end

  initial begin
    $dumpfile("/tmp/dump.fst");
    $dumpvars();
    #10000 $finish;
  end

  initial begin
    clk <= 1;
    while(1)
      #5 clk <= ~clk;
  end

endmodule

//axi_valid <= 1;
//#5 axi_ready <= 0;
//#15 axi_ready <= 1;
//
//
//
//ready == (state != execute && state != mem_access) || opcode_2 != ...
//
//else if(asyncio_reading) begin
//  if(asyncio_store_enable && asyncio_store_ready) begin
//    //memory...
//    
//  end
//end
