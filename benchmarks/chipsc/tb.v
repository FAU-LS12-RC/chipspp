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
      file = $fopen(FILE_NAME, "r");
      valid <= 0;
    end else if($feof(file)) begin
      valid <= 0;
    end else if(ready || !valid) begin
      if($ferror(file, error_out) && !$feof(file)) begin
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
  input valid, input last, output full);
  parameter FILE_NAME = "bar";
  parameter CYCLES_FILE_NAME = "cycles";
  parameter DATA_WIDTH_BYTES = 4;
  parameter NO_OF_VALUES = 0;

  assign ready = ~rst;
  reg [639:0] error_out;

  integer file_ptr;
  integer file;

  integer no_of_values_received = 0;

  reg full = 0;

  integer cycles_file;

  integer cycles_counter = 0;

  always @(posedge clk) begin
    cycles_counter <= cycles_counter + 1;
    if(rst) begin
      file = $fopen(FILE_NAME, "w");
      ``ifdef WAIT_FOR_VALUES
      cycles_file = $fopen(CYCLES_FILE_NAME, "w");
      ``endif
      full <= 0;
      cycles_counter <= 0;
    end else if(valid) begin
      if($ferror(file, error_out) 
        ``ifdef WAIT_FOR_VALUES
          || $ferror(cycles_file, error_out)
        ``endif) begin
        $display("error: could not open file %s", error_out);
        $finish;
      end

      $fwrite(file, "%x\n", data);
      no_of_values_received = no_of_values_received + 1;
      if(no_of_values_received >= NO_OF_VALUES) begin
        ``ifdef WAIT_FOR_VALUES
        $fwrite(cycles_file, "%d\n", cycles_counter);
        ``endif
        full <= 1;
      end
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
  wire d_out_full_`$i`;
  `<

  wire enough_values = 1
  `>outputs`
  & d_out_full_`$i`
  `<;

  ``ifdef WAIT_FOR_VALUES
  always @(posedge clk) begin
    if(enough_values) begin
      $finish;
    end
  end
  ``endif

  initial begin
    $dumpfile(``DUMP_PATH);
    $dumpvars();
    rst <= 1;
    #300 rst <= 0;
    ``ifdef RESTRICT_STEPS
    #``STEPS $finish;
    ``endif
  end

  `>inputs`

  wire file_ready_`$i` = d_in_ready_`$i` & run;
  file_master #(.FILE_NAME(```.`_file)) data_in_`$i`(
    .clk(clk), .rst(rst), .ready(file_ready_`$i`),
    .data(d_in_vec_`$i`), .valid(file_valid_`$i`), .last(d_in_last_`$i`));
  `<

  `>outputs`

  ``ifndef NO_OF_VALUES_`.`
  ``define NO_OF_VALUES_`.` 0
  ``endif
  ``ifndef `.`_cycles_file
  ``define `.`_cycles_file "foo"
  ``endif
  file_slave #(
    .FILE_NAME(```.`_file),
    .NO_OF_VALUES(``NO_OF_VALUES_`.`),
    .CYCLES_FILE_NAME(```.`_cycles_file))
    data_out_`$i`(
      .clk(clk), .rst(rst), .ready(d_out_ready_`$i`),
      .data(d_out_vec_`$i`), .valid(d_out_valid_`$i`), .last(d_out_last_`$i`),
      .full(d_out_full_`$i`));
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
