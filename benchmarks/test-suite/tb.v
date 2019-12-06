``timescale 1ns / 1ps

module file_master(
  input clk, input rst, input ready,
  output [DATA_WIDTH_BYTES*8-1:0] data,
  output valid, output last);
  parameter FILE_NAME = "foo";
  parameter DATA_WIDTH_BYTES = 4;

  reg valid;
  wire last = 0;
  reg [DATA_WIDTH_BYTES*8-1:0] data;
  reg [639:0] error_out;

  integer file_ptr;
  integer file;

  always @(posedge clk) begin
    if(rst) begin
      file = $fopenr(FILE_NAME);
      valid <= 0;
    end else if(ready || !valid) begin
      if($ferror(file, error_out)) begin
        $display("error: could not open file %s", error_out);
        $finish;
      end

      valid <= 0;
      if(!$feof(file)) begin
        valid <= 1;
        #0.1 file_ptr = $fscanf(file, "%x", data);
      end
    end
  end
endmodule

module file_slave(
  input clk, input rst, output ready,
  input [DATA_WIDTH_BYTES*8-1:0] data,
  input valid, input last);
  parameter FILE_NAME = "bar";
  parameter DATA_WIDTH_BYTES = 4;

  assign ready = ~rst;
  reg [639:0] error_out;

  integer file_ptr;
  integer file;

  always @(posedge clk) begin
    if(rst) begin
      file = $fopenw(FILE_NAME);
    end else if(valid) begin
      if($ferror(file, error_out)) begin
        $display("error: could not open file %s", error_out);
        $finish;
      end

      $fwrite(file, "%x\n", data);
    end
  end
endmodule

module test;

  reg clk;
  reg rst;
  wire run = ~rst;

  `>inputs`

  wire d_in_ready_`$i`;
  wire [31:0] d_in_vec_`$i`;
  wire file_valid_`$i`;
  wire d_in_valid_`$i` = file_valid_`$i` && run;
  wire d_in_last_`$i`;
  `<

  `>outputs`

  wire d_out_ready_`$i`;
  wire [31:0] d_out_vec_`$i`;
  wire d_out_valid_`$i`;
  wire d_out_last_`$i`;
  `<

  initial begin
    $dumpfile("dump.fst");
    $dumpvars();
    rst <= 1;
    #50 rst <= 0;
    #`steps` $finish;
  end

  `>inputs`

  wire file_ready_`$i` = d_in_ready_`$i` & run;
  file_master #(.FILE_NAME("`.`")) data_in_`$i`(
    .clk(clk), .rst(rst), .ready(file_ready_`$i`),
    .data(d_in_vec_`$i`), .valid(file_valid_`$i`), .last(d_in_last_`$i`));
  `<

  `>outputs`

  file_slave #(.FILE_NAME("`.`")) data_out_`$i`(
    .clk(clk), .rst(rst), .ready(d_out_ready_`$i`),
    .data(d_out_vec_`$i`), .valid(d_out_valid_`$i`), .last(d_out_last_`$i`));
  `<

  `chip_name` uut(
    .clk(clk),
    `>inputs`

    .`.`(d_in_vec_`$i`),
    .`.`_stb(d_in_valid_`$i`),
    .`.`_ack(d_in_ready_`$i`),
    `<
    `>outputs`

    .`.`(d_out_vec_`$i`),
    .`.`_stb(d_out_valid_`$i`),
    .`.`_ack(d_out_ready_`$i`),
    `<
    .rst(rst)
    );

  initial begin
    clk <= 0;
    while(1)
      #5 clk <= ~clk;
  end

endmodule
