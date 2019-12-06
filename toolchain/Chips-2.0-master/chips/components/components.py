from chips.api.api import Chip, Component, Wire, VerilogComponent
from chips_c import float_to_bits, double_to_bits, low_word, high_word


def async(chip, a, out=None):
    if out is None:
        out = Wire(chip)
    async = Component(
        """void main(){
        int in = input("in");
        int out = output("out");
        int data;
        while(1){
            if(ready(in)){
                data = fgetc(in);
            }
            if(ready(out)){
                fputc(data, out);
            }
        }
    }""",
        inline=True)
    async(
        chip,
        inputs={"in": a},
        outputs={"out": out},
        parameters={}
    )
    return out


def constant(chip, value, type_="int", out=None):
    if out is None:
        out = Wire(chip)

    verilog_value = value
    if type_ in ["int", "float"]:

        if type_ == "float":
            verilog_value = float_to_bits(value)

        verilog_file = """
        module {name} (output_out_ack,clk,rst,output_out,output_out_stb,exception);
          input output_out_ack;
          input clk;
          input rst;
          output [31:0] output_out;
          output output_out_stb;
          output exception;

          assign output_out = %s;
          assign output_out_stb = 1;
          assign exception = 0;
        endmodule
        """ % verilog_value

    elif type_ in ["long", "double"]:
        high = high_word(value)
        low = low_word(value)

        if type_ == "double":
            high = high_word(double_to_bits(value))
            low = low_word(double_to_bits(value))

        verilog_file = """
        module {name} (output_out_ack,clk,rst,output_out,output_out_stb,exception);
          input output_out_ack;
          input clk;
          input rst;
          output [31:0] output_out;
          output output_out_stb;
          output exception;
          reg [31:0] s_output_out_stb;
          reg [31:0] s_output_out;

          reg high;

          always @(posedge clk)
          begin

            if (high) begin
                s_output_out_stb <= 1;
                s_output_out <= %s;
                if (output_out_ack && s_output_out_stb) begin
                  s_output_out_stb <= 0;
                  high <= 0;
                end
            end else begin
                s_output_out_stb <= 1;
                s_output_out <= %s;
                if (output_out_ack && s_output_out_stb) begin
                  s_output_out_stb <= 0;
                  high <= 1;
                end
            end

            if (rst == 1'b1) begin
              high <= 0;
              s_output_out_stb <= 0;
            end

          end

          assign output_out = s_output_out;
          assign output_out_stb = s_output_out_stb;
          assign exception = 0;
        endmodule
        """ % (high, low)

    constant_component = VerilogComponent(

        C_file="""
            #include <stdio.h>
            int out = output("out");
            void main(){
                while(1){
                    fput_%s(%s, out);
                }
            }
        """ % (type_, value),
        V_file=verilog_file,
        inline=True
    )
    constant_component(
        chip,
        inputs={},
        outputs={"out": out},
        parameters={},
    )
    return out


def cycle(chip, args, type_="int", out=None):

    if out is None:
        out = Wire(chip)

    c_component = """
        #include <stdio.h>
        int out = output("out");
        void main(){
            %s list[%i] = {%s};
            int i;
            %s data;
            while(1){
              for(i=0; i<%i; i++){
                  data = list[i];
                  fput_%s(data, out);
              }
            }
        }
    """ % (
        type_,
        len(args),
        ", ".join(["%s" % i for i in args]),
        type_,
        len(args),
        type_
    )

    cycle_component = Component(c_component, inline=True)

    cycle_component(
        chip,
        inputs={},
        outputs={"out": out},
        parameters={},
    )
    return out


def report_all(chip, stream, type_="int"):

    report_all_component = Component(
        """
        #include <stdio.h>
        int in = input("in");
        void main(){
            while(1){
                report(fget_%s(in));
            }
        }
    """ % type_, inline=True)

    report_all_component(
        chip,
        inputs={"in": stream},
        outputs={},
        parameters={},
    )


def tee(chip, a, out1=None, out2=None):

    verilog_file = """
        module {name} (clk,rst,exception, 
            input_in, 
            input_in_stb,
            input_in_ack,
            output_out1,
            output_out1_stb,
            output_out1_ack,
            output_out2,
            output_out2_stb,
            output_out2_ack);

          input clk;
          input rst;
          output exception;

          input [31:0] input_in;
          input input_in_stb;
          output input_in_ack;

          output reg [31:0] output_out1;
          output output_out1_stb;
          input output_out1_ack;

          output reg [31:0] output_out2;
          output output_out2_stb;
          input output_out2_ack;

          reg s_output_out1_stb;
          reg s_output_out2_stb;
          wire s_input_in_ack;

          always @(posedge clk)
          begin

            if (s_input_in_ack & input_in_stb) begin
              s_output_out1_stb <= 1;
              s_output_out2_stb <= 1;
              output_out1 <= input_in;
              output_out2 <= input_in;
            end else begin
              if (output_out1_ack) begin
                s_output_out1_stb <= 0;
              end
              if (output_out2_ack) begin
                s_output_out2_stb <= 0;
              end
            end

            if (rst == 1'b1) begin
              s_output_out1_stb <= 0;
              s_output_out2_stb <= 0;
            end

          end

          assign s_input_in_ack =
              (output_out1_ack | (~s_output_out1_stb)) &
              (output_out2_ack | (~s_output_out2_stb));

          assign input_in_ack = s_input_in_ack;
          assign output_out1_stb = s_output_out1_stb;
          assign output_out2_stb = s_output_out2_stb;
          assign exception = 0;

        endmodule
    """

    tee_component = VerilogComponent("""
        int out1 = output("out1");
        int out2 = output("out2");
        int in = input("in");
        void main(){
            int data;
            while(1){
                data = fgetc(in);
                fputc(data, out1);
                fputc(data, out2);
            }
        }""", 
        V_file=verilog_file,
        inline=True)

    if out1 is None:
        out1 = Wire(chip)
    if out2 is None:
        out2 = Wire(chip)
    tee_component(
        chip,
        inputs={"in": a},
        outputs={"out1": out1, "out2": out2},
        parameters={}
    )
    return out1, out2


def delay(chip, a, initial=0, type_="int", out=None):
    delay_component = Component("""
        #include <stdio.h>
        int out = output("out");
        int in = input("in");
        void main(){
            fput_%s(INITIAL, out);
            while(1){
                fput_%s(fget_%s(in), out);
            }
        }""" % (type_, type_, type_), inline=True)
    if out is None:
        out = Wire(chip)
    delay_component(
        chip,
        inputs={"in": a},
        outputs={"out": out},
        parameters={"INITIAL": initial}
    )
    return out


def _arithmetic(chip, a, b, operation, type_="int", out=None):
    if out is None:
        out = Wire(chip)
    arithmetic_component = Component("""
        #include <stdio.h>
        /* Adder component model */
        int out = output("out");
        int in1 = input("in1");
        int in2 = input("in2");
        void main(){
            while(1){
                fput_%s(fget_%s(in1) %s fget_%s(in2), out);
            }
        }""" % (type_, type_, operation, type_), inline=True)
    arithmetic_component(
        chip,
        inputs={"in1": a, "in2": b},
        outputs={"out": out},
        parameters={}
    )
    return out


def add(chip, a, b, type_="int", out=None):
    return _arithmetic(chip, a, b, "+", type_, out)


def sub(chip, a, b, type_="int", out=None):
    return _arithmetic(chip, a, b, "-", type_, out)


def mul(chip, a, b, type_="int", out=None):
    return _arithmetic(chip, a, b, "*", type_, out)


def div(chip, a, b, type_="int", out=None):
    return _arithmetic(chip, a, b, "/", type_, out)


def _comparison(chip, a, b, operation, type_="int", out=None):
    if out is None:
        out = Wire(chip)
    code = """
        #include <stdio.h>
        int out = output("out");
        int in1 = input("in1");
        int in2 = input("in2");
        void main(){
            while(1){
                fput_int(fget_%s(in1) %s fget_%s(in2), out);
            }
        }""" % (type_, operation, type_)
    comparison = Component(code, inline=True)
    comparison(
        chip,
        inputs={"in1": a, "in2": b},
        outputs={"out": out},
        parameters={}
    )
    return out


def eq(chip, a, b, type_="int", out=None):
    return _comparison(chip, a, b, "==", type_, out)


def ne(chip, a, b, type_="int", out=None):
    return _comparison(chip, a, b, "!=", type_, out)


def lt(chip, a, b, type_="int", out=None):
    return _comparison(chip, a, b, "<", type_, out)


def le(chip, a, b, type_="int", out=None):
    return _comparison(chip, a, b, "<=", type_, out)


def gt(chip, a, b, type_="int", out=None):
    return _comparison(chip, a, b, ">", type_, out)


def ge(chip, a, b, type_="int", out=None):
    return _comparison(chip, a, b, ">=", type_, out)


def line_arbiter(chip, streams, out=None):
    """ Merge many streams of data into a single stream giving each stream equal priority

    Once a stream is selected by the arbiter, it will remain selected until an entire line has been output.
    Used to combine text based streams from multiple sources into a single stream, without merging lines.

    arguments
    ---------

    chip - the chip
    streams - a list (or iterable) of streams of data to combine
    out=None - Optionaly, an output Wire() object may be passed in.

    If out is None, then an output wire will be created.

    returns
    -------

    stream - a stream of data

    """

    if out is None:
        out = Wire(chip)

    arbiter_component = VerilogComponent(
        C_file="""int in1 = input("in1");
            int in2 = input("in2");
            int out = output("out");

            void main(){
                int temp;

                while(1){
                    if(ready(in1)){
                        while(1){
                            temp = fgetc(in1);
                            fputc(temp, out);
                            if(temp == '\\n') break;
                        }
                    }
                    if(ready(in2)){
                        while(1){
                            temp = fgetc(in2);
                            fputc(temp, out);
                            if(temp == '\\n') break;
                        }
                    }
                }

            }
        """,

        V_file="""module {name}(input_in1,input_in2,input_in1_stb,input_in2_stb,
          output_out_ack,clk,rst,output_out,output_out_stb,input_in1_ack,input_in2_ack,exception);
          parameter
          arbitrate_0 = 3'd0,
          arbitrate_1 = 3'd1,
          read_0 = 3'd2,
          read_1 = 3'd3,
          write_0 = 3'd4,
          write_1 = 3'd5;
          input [31:0] input_in1;
          input [31:0] input_in2;
          input input_in1_stb;
          input input_in2_stb;
          input output_out_ack;
          input clk;
          input rst;
          output [31:0] output_out;
          output output_out_stb;
          output input_in1_ack;
          output input_in2_ack;
          output exception;
          reg [31:0] s_output_out_stb;
          reg [31:0] s_output_out;
          reg [31:0] s_input_in1_ack;
          reg [31:0] s_input_in2_ack;
          reg [31:0] write_value;
          reg [2:0] state;

          always @(posedge clk)
          begin

           case(state)

            arbitrate_0:
            begin
                state <= arbitrate_1;
                if (input_in1_stb) begin
                    state <= read_0;
                end
            end

            arbitrate_1:
            begin
                state <= arbitrate_0;
                if (input_in2_stb) begin
                    state <= read_1;
                end
            end

            read_0:
            begin
                s_input_in1_ack <= 1;
                if (s_input_in1_ack && input_in1_stb) begin
                  write_value <= input_in1;
                  s_input_in1_ack <= 0;
                  state <= write_0;
                end
            end

            read_1:
            begin
                s_input_in2_ack <= 1;
                if (s_input_in2_ack && input_in2_stb) begin
                  write_value <= input_in2;
                  s_input_in2_ack <= 0;
                  state <= write_1;
                end
            end

            write_0:
            begin
                s_output_out_stb <= 1;
                s_output_out <= write_value;
                if (output_out_ack && s_output_out_stb) begin
                  s_output_out_stb <= 0;
                  if (write_value == 10) begin
                      state <= arbitrate_1;
                  end else begin
                      state <= read_0;
                  end
                end
            end

            write_1:
            begin
                s_output_out_stb <= 1;
                s_output_out <= write_value;
                if (output_out_ack && s_output_out_stb) begin
                  s_output_out_stb <= 0;
                  if (write_value == 10) begin
                      state <= arbitrate_0;
                  end else begin
                      state <= read_1;
                  end
                end
            end
           endcase

           if (rst == 1'b1) begin
             state <= arbitrate_0;
             s_input_in1_ack <= 0;
             s_input_in2_ack <= 0;
             s_output_out_stb <= 0;
           end

          end
          assign input_in1_ack = s_input_in1_ack;
          assign input_in2_ack = s_input_in2_ack;
          assign output_out_stb = s_output_out_stb;
          assign output_out = s_output_out;
          assign exception = 0;

        endmodule""",

        inline=True
    )

    tree_combine(chip, arbiter_component, streams, out)
    return out


def arbiter(chip, streams, out=None):
    """ Merge many streams of data into a single stream giving each stream equal priority

    arguments
    ---------

    chip - the chip
    args - a list (oriterable) of streams of data to combine
    out=None - a Wire in which to output the arbitrated data

    if out is None, then a wire will be created.

    returns
    -------

    stream - a stream of data

    """

    if out is None:
        out = Wire(chip)

    arbiter_component = VerilogComponent(

        C_file="""
        int out = output("out");
        int in1 = input("in1");
        int in2 = input("in2");
        void main(){
            while(1){
                if(ready(in1)) fputc(fgetc(in1), out);
                if(ready(in2)) fputc(fgetc(in2), out);
            }
        }""",

        V_file="""module {name}(input_in1,input_in2,input_in1_stb,input_in2_stb,
          output_out_ack,clk,rst,output_out,output_out_stb,input_in1_ack,input_in2_ack,exception);
          parameter
          arbitrate_0 = 3'd0,
          arbitrate_1 = 3'd1,
          read_0 = 3'd2,
          read_1 = 3'd3,
          write_0 = 3'd4,
          write_1 = 3'd5;
          input [31:0] input_in1;
          input [31:0] input_in2;
          input input_in1_stb;
          input input_in2_stb;
          input output_out_ack;
          input clk;
          input rst;
          output [31:0] output_out;
          output output_out_stb;
          output input_in1_ack;
          output input_in2_ack;
          output exception;
          reg [31:0] s_output_out_stb;
          reg [31:0] s_output_out;
          reg [31:0] s_input_in1_ack;
          reg [31:0] s_input_in2_ack;
          reg [31:0] write_value;
          reg [2:0] state;

          always @(posedge clk)
          begin

           case(state)

            arbitrate_0:
            begin
                state <= arbitrate_1;
                if (input_in1_stb) begin
                    state <= read_0;
                end
            end

            arbitrate_1:
            begin
                state <= arbitrate_0;
                if (input_in2_stb) begin
                    state <= read_1;
                end
            end

            read_0:
            begin
                s_input_in1_ack <= 1;
                if (s_input_in1_ack && input_in1_stb) begin
                  write_value <= input_in1;
                  s_input_in1_ack <= 0;
                  state <= write_0;
                end
            end

            read_1:
            begin
                s_input_in2_ack <= 1;
                if (s_input_in2_ack && input_in2_stb) begin
                  write_value <= input_in2;
                  s_input_in2_ack <= 0;
                  state <= write_1;
                end
            end

            write_0:
            begin
                s_output_out_stb <= 1;
                s_output_out <= write_value;
                if (output_out_ack && s_output_out_stb) begin
                  s_output_out_stb <= 0;
                  state <= arbitrate_1;
                end
            end

            write_1:
            begin
                s_output_out_stb <= 1;
                s_output_out <= write_value;
                if (output_out_ack && s_output_out_stb) begin
                  s_output_out_stb <= 0;
                  state <= arbitrate_0;
                end
            end
           endcase

           if (rst == 1'b1) begin
             state <= arbitrate_0;
             s_input_in1_ack <= 0;
             s_input_in2_ack <= 0;
             s_output_out_stb <= 0;
           end

          end
          assign input_in1_ack = s_input_in1_ack;
          assign input_in2_ack = s_input_in2_ack;
          assign output_out_stb = s_output_out_stb;
          assign output_out = s_output_out;
          assign exception = 0;

        endmodule""",

        inline=True
    )

    tree_combine(chip, arbiter_component, streams, out)
    return out


def discard(chip, a):
    """Discard a stream of data.

    arguments
    ---------

    chip - the chip
    a - the stream of data to discard

    returns
    -------

    N/A

    """

    discard_component = VerilogComponent(

        C_file="""/* Discard Component */
        int in = input("in");
        void main(){
            while(1){
                fgetc(in);
            }
        }""",

        V_file="""
        //Discard stream contents
        module {name} (input_in_ack,clk,rst,input_in,input_in_stb,exception);
          input clk;
          input rst;
          input [31:0] input_in;
          input input_in_stb;
          output input_in_ack;
          output exception;

          assign input_in_ack = 1;
          assign exception = 0;
        endmodule
        """,

        inline=True
    )

    discard_component(
        chip,
        inputs={"in": a},
        outputs={},
        parameters={}
    )


def assert_all(chip, a):
    """Assert that a stream of data is never 0

    arguments
    ---------

    chip - the chip
    a - the stream of data to check

    returns
    -------

    N/A

    """

    assert_component = Component(

        C_file="""/* Discard Component */
        int in = input("in");
        void main(){
            while(1){
                assert(fgetc(in));
            }
        }""",

        inline=True
    )

    assert_component(
        chip,
        inputs={"in": a},
        outputs={},
        parameters={}
    )


def tree_combine(chip, component, args, out):
    children = list(args)
    while len(children) > 2:
        parents = []
        while len(children) > 2:
            wire = Wire(chip)
            component(
                chip,
                inputs={"in1": children.pop(),
                        "in2": children.pop(0)},
                outputs={"out": wire},
                parameters={})
            parents.append(wire)
        parents.append(children.pop(0))
        children = parents
    component(
        chip,
        inputs={"in1": children.pop(),
                "in2": children.pop(0)},
        outputs={"out": out},
        parameters={})

if __name__ == "__main__":
    from chips.api.api import Chip
    from chips.compiler.exceptions import C2CHIPError, ChipsAssertionFail

    def test_chip(chip, test_name):
        print test_name,
        try:
            try:
                chip.simulation_reset()
                for i in range(1000):
                    chip.simulation_step()
            except ChipsAssertionFail:
                print "....failed in simulation"
                return False
            chip.generate_verilog()
            chip.generate_testbench(1000)
            retval = chip.compile_iverilog(True)
            if retval != 0:
                print "....failed in verilog simulation"
                return False

        except C2CHIPError as e:
            print "....compile error"
            print e
        print "....passed"
        return True

    # Test Arbiter
    chip = Chip("test_chip")
    first = [0, 1, 2, 3, 4, 5, 6]
    second = [10, 11, 12, 13, 14, 15, 16]
    expected = [0, 10, 1, 11, 2, 12, 3, 13, 4, 15, 6, 16]
    stream_1 = cycle(chip, first)
    stream_2 = cycle(chip, second)
    stream_3 = cycle(chip, expected)
    assert_all(chip, eq(chip, arbiter(chip, [stream_1, stream_2]), stream_3))
    test_chip(chip, "arbiter")

    # Test Delay
    test = [1, 2, 3, 0]
    expected = [0, 1, 2, 3]
    chip = Chip("test_chip")
    assert_all(
        chip, eq(chip, delay(chip, cycle(chip, test)), cycle(chip, expected)))
    test_chip(chip, "delay")

    test = [1, 2, 3, 0]
    expected = [0, 1, 2, 3]
    chip = Chip("test_chip")
    assert_all(
        chip,
        eq(chip,
           delay(chip,
                 cycle(chip,
                       test,
                       type_="float")),
            cycle(chip,
                  expected,
                  type_="float"),
            type_="float"))
    test_chip(chip, "float delay")

    test = [1, 2, 3, 0]
    expected = [0, 1, 2, 3]
    chip = Chip("test_chip")
    assert_all(chip,
               eq(chip,
                  delay(chip,
                        cycle(chip, test, type_="double"),
                        type_="double",
                        ),
                  cycle(chip, expected, type_="double"),
                  type_="double"
                  )
               )
    test_chip(chip, "double delay")

    test = [1, 2, 3, 0]
    expected = [0, 1, 2, 3]
    chip = Chip("test_chip")
    assert_all(chip,
               eq(chip,
                  delay(chip,
                        cycle(chip, test, type_="long"),
                        type_="long",
                        ),
                  cycle(chip, expected, type_="long"),
                  type_="long"
                  )
               )
    test_chip(chip, "long delay")

    # Test Tee/Discard
    chip = Chip("test_chip")
    expected = [0, 1, 2, 3]
    a, b = tee(chip,
               cycle(chip, expected),
               )
    discard(chip, b)
    assert_all(chip,
               eq(chip,
                  a,
                  cycle(chip, expected),
                  )
               )
    test_chip(chip, "tee discard")

    chip = Chip("test_chip")
    expected = [0, 1, 2, 3]
    a, b = tee(chip,
               cycle(chip, expected),
               )
    assert_all(chip,
               eq(chip,
                  b,
                  cycle(chip, expected),
                  )
               )
    assert_all(chip,
               eq(chip,
                  a,
                  cycle(chip, expected),
                  )
               )
    test_chip(chip, "tee")

    # Test Constant
    chip = Chip("test_chip")
    first = [1, 1]
    expected = 1
    stream_1 = cycle(chip, first)
    stream_3 = constant(chip, expected)
    assert_all(chip, eq(chip, stream_1, stream_3))
    test_chip(chip, "constant")

    chip = Chip("test_chip")
    first = [2, 2]
    expected = 2
    stream_1 = cycle(chip, first, type_="float")
    stream_3 = constant(chip, expected, type_="float")
    assert_all(chip, eq(chip, stream_1, stream_3, type_="float"))
    test_chip(chip, "float constant")

    chip = Chip("test_chip")
    first = [2, 2]
    expected = 2
    stream_1 = cycle(chip, first, type_="double")
    stream_3 = constant(chip, expected, type_="double")
    assert_all(chip, eq(chip, stream_1, stream_3, type_="double"))
    test_chip(chip, "double constant")

    chip = Chip("test_chip")
    first = [2, 2]
    expected = 2
    stream_1 = cycle(chip, first, type_="long")
    stream_3 = constant(chip, expected, type_="long")
    assert_all(chip, eq(chip, stream_1, stream_3, type_="long"))
    test_chip(chip, "long constant")

    # Test Arithmetic
    test_vectors = {
        "add": [
            [100.0, 100.0],
            [-100.0, 100.0],
            [0.0, 200.0],
        ],
        "sub": [
            [100.0, 100.0],
            [-100.0, 100.0],
            [200.0, 0.0],
        ],
        "mul": [
            [100.0, 100.0],
            [-100.0, -2.0],
            [-10000.0, -200.0],
        ],
        "div": [
            [100.0, ],
            [-100.0, ],
            [-1.0, ],
        ],
    }

    for f, fname in zip([add, sub, mul, div], ["add", "sub", "mul", "div"]):
        for type_ in ["float", "double", "int", "long"]:

            a, b, c = test_vectors[fname]
            chip = Chip("test_chip")
            assert_all(chip,
                       eq(chip,
                          f(chip,
                            cycle(chip, a, type_=type_),
                            cycle(chip, b, type_=type_),
                            type_=type_,
                            ),
                           cycle(chip, c, type_=type_),
                          ),
                       )
            test_chip(chip, type_ + " " + fname)

    # Test Comparators
    test_vectors = {
        "eq": [
            [100.0, 100.0],
            [-100.0, 100.0],
            [0, 1],
        ],
        "ne": [
            [100.0, 100.0],
            [-100.0, 100.0],
            [1, 0],
        ],
        "gt": [
            [100.0, 100.0, 0.0, 0.0],
            [-100.0, -2.0, -1.0, 0.0],
            [1, 1, 1, 0],
        ],
        "ge": [
            [100.0, 100.0, 0.0, 0.0],
            [-100.0, -2.0, -1.0, 0.0],
            [1, 1, 1, 1],
        ],
        "lt": [
            [100.0, 100.0, 0.0, 0.0, -1.0],
            [-100.0, -2.0, -1.0, 0.0, 0.0],
            [0, 0, 0, 0, 1],
        ],
        "le": [
            [100.0, 100.0, 0.0, 0.0],
            [-100.0, -2.0, -1.0, 0.0],
            [0, 0, 0, 1],
        ],
    }

    for f, fname in zip([eq, ne, gt, ge, lt, le], ["eq", "ne", "gt", "ge", "lt", "le"]):
        for type_ in ["float", "double", "int", "long"]:

            a, b, c = test_vectors[fname]
            chip = Chip("test_chip")
            assert_all(chip,
                       eq(chip,
                          f(chip,
                            cycle(chip, a, type_=type_),
                            cycle(chip, b, type_=type_),
                            type_=type_,
                            ),
                           cycle(chip, c),
                          ),
                       )
            test_chip(chip, type_ + " " + fname)
