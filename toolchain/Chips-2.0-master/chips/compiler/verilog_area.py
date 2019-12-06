#!/usr/bin/env python
"""Generate Verilog Implementation of Instructions

The area optimized implementation uses a CPU like architecture.
+ Instructions are implemented in block RAM.
+ Registers are implemented in dual port RAM.
+ Only one instruction can be executed at a time.
+ The CPU uses a pipeline architecture, and will take 2 clocks to execute a taken branch.
+ A minimal instruction set is determined at compile time, and only those instructions are implemented.

"""

__author__ = "Jon Dawson"
__copyright__ = "Copyright (C) 2013, Jonathan P Dawson"

from utils import calculate_jumps
from chips.utils.codewriter import CodeWriter
from textwrap import dedent
from collections import namedtuple

two_cycle_instrs = ["load", "load_disp", "load8", "load16", "multiply",
        "shift_left", "shift_right", "unsigned_shift_right"]

class Feature:
    def __init__(self, states, signals_code, stage2_code, stage3_code):
        pass

IntMultiplier = Feature(
        ["ex2_stalled"],
        r"""
            reg [31:0] products[3:0];
            reg [1:0] multiply_step;
            reg [47:0] sums[2:0];
        """, indent=2)

        #TODO is feature in? func?


def log2(instructions):
    """Integer only algorithm to calculate the number of bits needed to store a number"""

    bits = 1
    power = 2
    while power < instructions:
        bits += 1
        power *= 2
    return bits


def print_verilog_literal(size, value):
    """Print a verilog literal with expicilt size"""

    if(value >= 0):
        return "%s'd%s" % (size, value)
    else:
        return "-%s'd%s" % (size, abs(value))

def find_instruction_opcode(instruction_set, name):
    for opcode, instruction in enumerate(instruction_set):
        if instruction["op"] == name:
            return opcode
    return None


def generate_instruction_set(instructions):
    """Calculate the required instruction set"""

    instruction_set = [dict(op="nop")]
    instruction_memory = []
    for instruction in instructions:
        opcode = {}
        encoded_instruction = {}
        encoded_instruction["d"] = 0
        encoded_instruction["c"] = 0
        encoded_instruction["a"] = 0
        encoded_instruction["b"] = 0
        encoded_instruction["literal"] = 0
        if "trace" in instruction:
            encoded_instruction["lineno"] = instruction["trace"].lineno
            encoded_instruction["filename"] = instruction["trace"].filename
        else:
            encoded_instruction["lineno"] = 0
            encoded_instruction["filename"] = "unknown"
        opcode["op"] = instruction["op"]
        opcode["literal"] = False

        if "file_name" in instruction:
            opcode["file_name"] = instruction["file_name"]

        if "file" in instruction:
            opcode["file"] = instruction["file"]

        if "line" in instruction:
            opcode["line"] = instruction["line"]

        if "input" in instruction:
            opcode["input"] = instruction["input"]

        if "output" in instruction:
            opcode["output"] = instruction["output"]

        if "a" in instruction:
            encoded_instruction["a"] = instruction["a"]

        if "b" in instruction:
            encoded_instruction["b"] = instruction["b"]

        if "z" in instruction:
            encoded_instruction["z"] = instruction["z"]

        if "literal" in instruction:
            opcode["literal"] = True
            encoded_instruction["literal"] = instruction["literal"]

        if "label" in instruction:
            opcode["literal"] = True
            encoded_instruction["literal"] = instruction["label"]

        if opcode not in instruction_set:
            instruction_set.append(opcode)

        for op, test_opcode in enumerate(instruction_set):
            if test_opcode == opcode:
                encoded_instruction["op"] = op
                encoded_instruction["comment"] = repr(instruction)
                instruction_memory.append(encoded_instruction)
                break

    return instruction_set, instruction_memory

AsyncIoPort = namedtuple("AsyncIoPort", ["name", "number", "direction"])

def generate_declarations(
    instructions,
    no_tb_mode,
    register_bits,
    opcode_bits,
    program):
    """Generate verilog declarations"""

    # list all inputs and outputs used in the program
    inputs = program.input_names
    outputs = program.output_names
    input_files = set([i["file_name"]
                       for i in instructions if "file_read" == i["op"]])
    output_files = set([i["file_name"]
                        for i in instructions if i["op"] in ("file_write", "float_file_write", "long_float_file_write")])
    testbench = not inputs and not outputs and not no_tb_mode

    # Do not generate a port in testbench mode
    inports = [
        ("input_" + i, 32) for i in inputs
    ] + [
        ("input_" + i + "_stb", 1) for i in inputs
    ] + [
        ("output_" + i + "_ack", 1) for i in outputs
    ]

    outports = [
        ("output_" + i, 32) for i in outputs
    ] + [
        ("output_" + i + "_stb", 1) for i in outputs
    ] + [
        ("input_" + i + "_ack", 1) for i in inputs
    ]

    input_id_bits = log2(len(inputs))
    output_id_bits = log2(len(outputs))

    # create list of signals
    signals = [
        ("timer", 32),
        ("timer_clock", 64),
        ("program_counter", 16),
        ("program_counter_1", 16),
        ("program_counter_2", 16),
        ("instruction", 32 + 8 + opcode_bits),
        ("opcode_2", opcode_bits),
        ("opcode_3", opcode_bits),

        ("a", 4),
        ("b", 4),
        ("z", 4),

        ("writeback_enable", 1),
        ("writeback_enable_2", 1),
        ("address_a_2", 4),
        ("address_b_2", 4),
        ("address_z_2", 4),
        ("address_z_3", 4),
        ("address_z_4", 4),

        ("shift_amount", 2),

        ("write_output", output_id_bits),
        ("write_value", 32),
        ("read_input", input_id_bits),

        ("literal_2", 16),
        ("a_hi", 32),
        ("b_hi", 32),
        ("a_lo", 32),
        ("b_lo", 32),
        ("long_result", 64),
        ("result", 32),
        ("result_2", 32),
        ("address", 16),
        ("data_out", 32),
        ("data_in", 32),
        ("carry", 32),
    ] + [
        ("s_output_" + i + "_stb", 1) for i in outputs
    ] + [
        ("s_output_" + i, 32) for i in outputs
    ] + [
        ("s_input_" + i + "_ack", 1) for i in inputs
    ]

    if testbench:
        signals.append(("clk", 1))
        signals.append(("rst", 1))
    else:
        inports.append(("clk", 1))
        inports.append(("rst", 1))

    return inputs, outputs, input_files, output_files, testbench, inports, outports, signals

def have_asyncio(instruction_set):
    for instr in instruction_set:
        if instr["op"].startswith("asyncio"):
            return True
    return False

def floating_point_enables(instruction_set):
    floating_point_arithmetic = []
    floating_point_conversions = []
    floating_point_debug = []
    for i in instruction_set:
        if i["op"] == "float_add":
            floating_point_arithmetic.append("adder")
        if i["op"] == "float_subtract":
            floating_point_arithmetic.append("adder")
        if i["op"] == "float_multiply":
            floating_point_arithmetic.append("multiplier")
        if i["op"] == "float_divide":
            floating_point_arithmetic.append("divider")
        if i["op"] == "long_float_add":
            floating_point_arithmetic.append("double_adder")
        if i["op"] == "long_float_subtract":
            floating_point_arithmetic.append("double_adder")
        if i["op"] == "long_float_multiply":
            floating_point_arithmetic.append("double_multiplier")
        if i["op"] == "long_float_divide":
            floating_point_arithmetic.append("double_divider")
        if i["op"] == "int_to_float":
            floating_point_conversions.append("int_to_float")
        if i["op"] == "float_to_int":
            floating_point_conversions.append("float_to_int")
        if i["op"] == "long_to_double":
            floating_point_conversions.append("long_to_double")
        if i["op"] == "double_to_long":
            floating_point_conversions.append("double_to_long")
        if i["op"] == "float_to_double":
            floating_point_conversions.append("float_to_double")
        if i["op"] == "double_to_float":
            floating_point_conversions.append("double_to_float")
        if i["op"] == "float_report":
            floating_point_debug.append("float_report")
        if i["op"] == "long_float_report":
            floating_point_debug.append("long_float_report")
        if i["op"] == "float_file_write":
            floating_point_debug.append("float_report")
        if i["op"] == "long_float_file_write":
            floating_point_debug.append("long_float_report")

    floating_point_arithmetic = set(floating_point_arithmetic)
    floating_point_conversions = set(floating_point_conversions)
    floating_point_debug = set(floating_point_debug)

    return floating_point_arithmetic, floating_point_conversions, floating_point_debug


def generate_CHIP(input_file,
                  program,
                  output_file,
                  initialize_memory,
                  no_tb_mode=False,
                  options={}):
    """A big ugly function to crunch through all the instructions and generate the CHIP equivilent"""

    instructions = calculate_jumps([dict(instr.items()) for instr in
        program.instructions])
    instruction_set, instruction_memory = generate_instruction_set(
        instructions)
    opcodes = dict((i["op"], i) for i in instruction_set)
    register_bits = 16
    opcode_bits = log2(len(instruction_set))
    instruction_bits = 16 + 4 + 4 + opcode_bits
    declarations = generate_declarations(
        instructions,
        no_tb_mode,
        register_bits,
        opcode_bits,
        program)
    inputs, outputs, input_files, output_files, testbench, inports, outports, signals = declarations
    floating_point_arithmetic, floating_point_conversions, floating_point_debug  = floating_point_enables(
        instruction_set)

    input_id_bits = log2(len(inputs))
    output_id_bits = log2(len(outputs))
    address_bits = log2(program.memory_size_in_bytes)

    asyncio_ports = []
    asyncio_inputs = []
    asyncio_outputs = []
    if have_asyncio(instruction_set):
        for number, name in program.id_to_input.items():
            port = AsyncIoPort(name, number, "in")
            asyncio_ports.append(port)
            asyncio_inputs.append(port)
        for number, name in program.id_to_output.items():
            port = AsyncIoPort(name, number, "out")
            asyncio_ports.append(port)
            asyncio_outputs.append(port)
    asyncio_input_id_bits = log2(len(asyncio_inputs))
    asyncio_output_id_bits = log2(len(asyncio_inputs))

    # output the code in verilog
    output_file.write("//name : %s\n" % program.component_name)
    for i in inputs:
        output_file.write("//input : input_%s:16\n" % i)
    for i in outputs:
        output_file.write("//output : output_%s:16\n" % i)
    output_file.write("//source_file : %s\n" % input_file)
    output_file.write(dedent("""
    ///+============================================================================+
    ///|                                                                            |
    ///|                     This file was generated by Chips                       |
    ///|                                                                            |
    ///|                                  Chips                                     |
    ///|                                                                            |
    ///|                      http://github.com/dawsonjon/Chips-2.0                 |
    ///|                                                                            |
    ///|                                                             Python powered |
    ///+============================================================================+
    """))

    output_file.write("module %s" % program.component_name)

    all_ports = [name for name, size in inports + outports] + ["exception"]
    if all_ports:
        output_file.write("(")
        output_file.write(",".join(all_ports))
        output_file.write(");\n")
    else:
        output_file.write(";\n")

    output_file.write("  integer file_count;\n")

    for i in floating_point_arithmetic:

        if i.startswith("double"):
            output_file.write("  reg [63:0] %s_a;\n" % (i))
            output_file.write("  reg [63:0] %s_b;\n" % (i))
            output_file.write("  wire [63:0] %s_z;\n" % (i))
        else:
            output_file.write("  reg [31:0] %s_a;\n" % (i))
            output_file.write("  reg [31:0] %s_b;\n" % (i))
            output_file.write("  wire [31:0] %s_z;\n" % (i))

        output_file.write("  reg %s_a_stb;\n" % (i))
        output_file.write("  wire %s_a_ack;\n" % (i))
        output_file.write("  reg %s_b_stb;\n" % (i))
        output_file.write("  wire %s_b_ack;\n" % (i))
        output_file.write("  wire %s_z_stb;\n" % (i))
        output_file.write("  reg %s_z_ack;\n" % (i))

    for i in floating_point_conversions:

        if i.startswith("long") or i.startswith("double"):
            output_file.write("  reg [63:0] %s_in;\n" % (i))
        else:
            output_file.write("  reg [31:0] %s_in;\n" % (i))

        if i.endswith("long") or i.endswith("double"):
            output_file.write("  wire [63:0] %s_out;\n" % (i))
        else:
            output_file.write("  wire [31:0] %s_out;\n" % (i))

        output_file.write("  wire %s_out_stb;\n" % (i))
        output_file.write("  reg %s_out_ack;\n" % (i))
        output_file.write("  reg %s_in_stb;\n" % (i))
        output_file.write("  wire %s_in_ack;\n" % (i))

    if len(asyncio_ports) > 0:
        CodeWriter(output_file).write(r"""

        reg [`address_bits`-1:0] asyncio_address_read;
        wire [`address_bits`-1:0] asyncio_address_out_read;
        reg [`address_bits`-1:0] asyncio_address_write;
        wire [`address_bits`-1:0] asyncio_address_out_write;

        reg [23:0] asyncio_length_read;
        wire [23:0] asyncio_length_out_read;
        reg [23:0] asyncio_length_write;
        wire [23:0] asyncio_length_out_write;

        wire asyncio_store_ready;
        reg asyncio_load_ready;
        wire asyncio_store_enable;
        wire asyncio_load_enable;

        wire asyncio_will_load;

        wire [31:0] asyncio_word_to_store;
        reg [31:0] asyncio_loaded_word;

        wire [`no_of_inputs`-1:0] asyncio_ready;
        wire [`no_of_outputs`-1:0] asyncio_valid;
        wire [`no_of_outputs` * 32 - 1:0] asyncio_data;

        reg asyncio_reading = 0;
        reg asyncio_writing = 0;

        reg asyncio_wait_mode;
        """, indent=2, asyncio_ports=asyncio_ports,
        no_of_inputs=len(asyncio_inputs),
        no_of_outputs=len(asyncio_outputs),
        address_bits=address_bits)

        output_file.write("reg [{0}:0] asyncio_read_id;\n".format(
            input_id_bits - 1))
        output_file.write("reg [{0}:0] asyncio_write_id;\n".format(
            output_id_bits - 1))

    if floating_point_debug:
        output_file.write("  real fp_value;\n")

    states = [
        "stop",
        "instruction_fetch",
        "operand_fetch",
        "execute",
        "mem_access",
        #TODO remove
        #"load",
    ]

    if "wait_clocks" in opcodes:
        states.append("wait_state")

    if len(asyncio_ports) > 0:
        states.append("asyncio_wait")

    if inports:
        states.append("read")

    if outports:
        states.append("write")

    needs_divider = False
    for i in ["divide", "unsigned_divide", "modulo", "unsigned_modulo"]:
        if i in opcodes:
            states.append(i)
            needs_divider = True

    needs_long_divider = False
    for i in ["long_divide", "unsigned_long_divide", "long_modulo", "unsigned_long_modulo"]:
        if i in opcodes:
            states.append(i)
            needs_long_divider = True

#    if any(state in opcodes for state in ["multiply",
#        "unsigned_shift_right", "shift_right", "shift_left"]):
    if any(state in opcodes for state in ["multiply"]):
        states.append("ex2_stalled")

    divide_latency = options.get("divide_latency", 32)
    divide_iterations = 32/divide_latency

    long_divide_latency = options.get("long_divide_latency", 64)
    long_divide_iterations = 64/long_divide_latency

    for i in floating_point_arithmetic:
        states.append("%s_write_a" % i)
        states.append("%s_write_b" % i)
        states.append("%s_read_z" % i)

    for i in floating_point_conversions:
        states.append("%s_write_a" % i)
        states.append("%s_read_z" % i)

    state_variables = []
    for index, state in enumerate(states):
        state_variables.append(
            "%s = %s'd%s" %
            (state, log2(len(states)), index))

    signals.append(("state", log2(len(states))))
    signals.append(("prev_state", log2(len(states))))
    output_file.write("  parameter  ")
    output_file.write(",\n  ".join(state_variables))
    output_file.write(";\n")

    input_files = dict(
        zip(input_files, ["input_file_%s" %
                          i for i, j in enumerate(input_files)]))
    for i in input_files.values():
        output_file.write("  integer %s;\n" % i)

    output_files = dict(
        zip(output_files, ["output_file_%s" %
                           i for i, j in enumerate(output_files)]))
    for i in output_files.values():
        output_file.write("  integer %s;\n" % i)

    def write_declaration(object_type, name, size):
        if size == 1:
            output_file.write(object_type)
            output_file.write(name)
            output_file.write(";\n")
        else:
            output_file.write(object_type)
            output_file.write("[%i:0]" % (size - 1))
            output_file.write(" ")
            output_file.write(name)
            output_file.write(";\n")

    for name, size in inports:
        write_declaration("  input ", name, size)

    for name, size in outports:
        write_declaration("  output ", name, size)

    for name, size in signals:
        write_declaration("  reg ", name, size)

    store32_opcode = find_instruction_opcode(instruction_set, "store")
    store8_opcode = find_instruction_opcode(instruction_set, "store8")
    load32_opcode = find_instruction_opcode(instruction_set, "load")
    load32_disp_opcode = find_instruction_opcode(instruction_set, "load_disp")
    load8_opcode = find_instruction_opcode(instruction_set, "load8")
    load16_opcode = find_instruction_opcode(instruction_set, "load16")
    store16_opcode = find_instruction_opcode(instruction_set, "store16")
    #has_32_bit_load = load32_opcode is not None or load32_disp_opcode is not None

    output_file.write("  output reg exception;\n")
    output_file.write(
        "  reg [%s:0] instructions [%i:0];\n" %
        (instruction_bits - 1, len(instructions) - 1))
    CodeWriter(output_file).write(r"""
        reg [7:0] memory_0 [`size`:0];
        reg [7:0] memory_1 [`size`:0];
        reg [7:0] memory_2 [`size`:0];
        reg [7:0] memory_3 [`size`:0];
        reg [31:0] registers [15:0];
        wire [15:0] next_program_counter;
        wire [31:0] operand_a;
        wire [31:0] operand_b;
        wire [31:0] operand_z;
        wire [31:0] register_a;
        wire [31:0] register_b;
        wire [31:0] register_z;
        wire [15:0] literal;
        wire [`opcode_bits`:0] opcode;
        wire [3:0] address_a;
        wire [3:0] address_b;
        wire [3:0] address_z;
        reg [`address_bits`-1:0] load_address;
        reg [`address_bits`-1:0] store_address;
        reg signed_load;
        reg [31:0] store_data;
`if store8_opcode`        wire store_enable_8;`fi
`if store16_opcode`        wire store_enable_16;`fi
`if store32_opcode`        wire store_enable_32;`fi
        wire must_insert_bubble;
        wire forward_a;
        wire forward_b;
        wire forward_z;
        wire forward_from_mem_a;
        wire forward_from_mem_b;
        wire forward_from_mem_z;
        wire two_cycle_instr_in_ex;

    """, indent=2, size=(program.memory_size_in_bytes + 3) // 4,
        store16_opcode=store16_opcode, store8_opcode=store8_opcode,
        store32_opcode=store32_opcode,
        opcode_bits=opcode_bits - 1,
        address_bits=address_bits)

    if needs_divider:
      output_file.write("  reg [31:0] shifter;\n")
      output_file.write("  reg [32:0] difference;\n")
      output_file.write("  reg [31:0] divisor;\n")
      output_file.write("  reg [31:0] dividend;\n")
      output_file.write("  reg [31:0] quotient;\n")
      output_file.write("  reg [31:0] remainder;\n")
      output_file.write("  reg quotient_sign;\n")
      output_file.write("  reg dividend_sign;\n")

    if needs_long_divider:
      output_file.write("  reg [63:0] long_shifter;\n")
      output_file.write("  reg [64:0] long_difference;\n")
      output_file.write("  reg [63:0] long_divisor;\n")
      output_file.write("  reg [63:0] long_dividend;\n")
      output_file.write("  reg [63:0] long_quotient;\n")
      output_file.write("  reg [63:0] long_remainder;\n")
      output_file.write("  reg long_quotient_sign;\n")
      output_file.write("  reg long_dividend_sign;\n")

    if "multiply" in opcodes:
      CodeWriter(output_file).write(r"""
        reg [31:0] products[3:0];
        reg [1:0] multiply_step;
        reg [47:0] sums[2:0];
      """, indent=2)

    # generate clock and reset in testbench mode
    if testbench:

        output_file.write(
            "\n  //////////////////////////////////////////////////////////////////////////////\n")
        output_file.write(
            "  // CLOCK AND RESET GENERATION                                                 \n")
        output_file.write(
            "  //                                                                            \n")
        output_file.write(
            "  // This file was generated in test bench mode. In this mode, the verilog      \n")
        output_file.write(
            "  // output file can be executed directly within a verilog simulator.           \n")
        output_file.write(
            "  // In test bench mode, a simulated clock and reset signal are generated within\n")
        output_file.write(
            "  // the output file.                                                           \n")
        output_file.write(
            "  // Verilog files generated in testbecnch mode are not suitable for synthesis, \n")
        output_file.write(
            "  // or for instantiation within a larger design.\n")

        output_file.write("  \n  initial\n")
        output_file.write("  begin\n")
        output_file.write("    rst <= 1'b1;\n")
        output_file.write("    #50 rst <= 1'b0;\n")
        output_file.write("  end\n\n")

        output_file.write("  \n  initial\n")
        output_file.write("  begin\n")
        output_file.write("    clk <= 1'b0;\n")
        output_file.write("    while (1) begin\n")
        output_file.write("      #5 clk <= ~clk;\n")
        output_file.write("    end\n")
        output_file.write("  end\n\n")

    # Instance Floating Point Arithmetic
    if floating_point_arithmetic or floating_point_conversions:

        output_file.write(
            "\n  //////////////////////////////////////////////////////////////////////////////\n")
        output_file.write(
            "  // Floating Point Arithmetic                                                  \n")
        output_file.write(
            "  //                                                                            \n")
        output_file.write(
            "  // Generate IEEE 754 single precision divider, adder and multiplier           \n")
        output_file.write(
            "  //                                                                            \n")

        for i in floating_point_arithmetic:
            output_file.write("  %s %s_inst(\n" % (i, i))
            output_file.write("    .clk(clk),\n")
            output_file.write("    .rst(rst),\n")
            output_file.write("    .input_a(%s_a),\n" % i)
            output_file.write("    .input_a_stb(%s_a_stb),\n" % i)
            output_file.write("    .input_a_ack(%s_a_ack),\n" % i)
            output_file.write("    .input_b(%s_b),\n" % i)
            output_file.write("    .input_b_stb(%s_b_stb),\n" % i)
            output_file.write("    .input_b_ack(%s_b_ack),\n" % i)
            output_file.write("    .output_z(%s_z),\n" % i)
            output_file.write("    .output_z_stb(%s_z_stb),\n" % i)
            output_file.write("    .output_z_ack(%s_z_ack)\n" % i)
            output_file.write("  );\n")

        for i in floating_point_conversions:
            output_file.write("  %s %s_inst(\n" % (i, i))
            output_file.write("    .clk(clk),\n")
            output_file.write("    .rst(rst),\n")
            output_file.write("    .input_a(%s_in),\n" % i)
            output_file.write("    .input_a_stb(%s_in_stb),\n" % i)
            output_file.write("    .input_a_ack(%s_in_ack),\n" % i)
            output_file.write("    .output_z(%s_out),\n" % i)
            output_file.write("    .output_z_stb(%s_out_stb),\n" % i)
            output_file.write("    .output_z_ack(%s_out_ack)\n" % i)
            output_file.write("  );\n")

    if len(asyncio_ports) > 0:
        CodeWriter(output_file).write(r"""
        `name` #(
        .INPUT_ID_WIDTH(`input_id_bits`),
        .NO_OF_AXI_INPUTS(`no_of_inputs`),
        .OUTPUT_ID_WIDTH(1),
        .NO_OF_AXI_OUTPUTS(1),
        .MEMORY_IS_INPUT(0),
        .ADDRESS_WIDTH(`address_bits`)) `name`_0(
        .clk(clk),
        .rst(rst),

        .address(asyncio_address_`kind`),
        .length(asyncio_length_`kind`),
        .length_out(asyncio_length_out_`kind`),
        .address_out(asyncio_address_out_`kind`),

        .input_id(asyncio_`kind`_id),
        .output_id(1'd0),

        .axi_out_ready(asyncio_store_ready),
        .axi_out_valid(asyncio_store_enable),
        .axi_out_data(asyncio_word_to_store),

        .axi_in_valid({`input_valids`}),
        .axi_in_ready(asyncio_ready),
        .axi_in_data({`input_data`}),

        .will_load(1'd0)
        );
        """, indent=2, name="asyncio_reader", kind="read",
        input_data=", ".join("input_{}".format(port.name) for port in
            reversed(asyncio_inputs)),
        input_valids=", ".join("input_{}_stb".format(port.name) for port in
            reversed(asyncio_inputs)),
        input_id_bits=input_id_bits,
        no_of_inputs=len(inputs),
        address_bits=address_bits)

        CodeWriter(output_file).write(r"""

        `name` #(
        .INPUT_ID_WIDTH(1),
        .NO_OF_AXI_INPUTS(1),
        .OUTPUT_ID_WIDTH(`output_id_bits`),
        .NO_OF_AXI_OUTPUTS(`no_of_outputs`),
        .MEMORY_IS_INPUT(1),
        .ADDRESS_WIDTH(`address_bits`)) `name`_load_0(
        .clk(clk),
        .rst(rst),

        .address(asyncio_address_`kind`),
        .length(asyncio_length_`kind`),
        .length_out(asyncio_length_out_`kind`),
        .address_out(asyncio_address_out_`kind`),

        .input_id(1'd0),
        .output_id(asyncio_`kind`_id),

        .axi_out_ready({`output_readies`}),
        .axi_out_valid(asyncio_valid),
        .axi_out_data(asyncio_data),

        .axi_in_valid(asyncio_load_ready),
        .axi_in_ready(asyncio_load_enable),
        .axi_in_data(asyncio_loaded_word),

        .will_load(asyncio_will_load)
        );
        """, indent=2, name="asyncio_reader", kind="write",
        output_readies=", ".join("output_{}_ack".format(port.name) for
            port in reversed(asyncio_outputs)),
        output_id_bits=output_id_bits,
        no_of_outputs=len(outputs),
        address_bits=address_bits)

    # Generate a state machine to execute the instructions
    # if initialize_memory and allocator.memory_content:
#
        # output_file.write("\n  //////////////////////////////////////////////////////////////////////////////\n")
        # output_file.write("  // MEMORY INITIALIZATION                                                      \n")
        # output_file.write("  //                                                                            \n")
        # output_file.write("  // In order to reduce program size, array contents have been stored into      \n")
        # output_file.write("  // memory at initialization. In an FPGA, this will result in the memory being \n")
        # output_file.write("  // initialized when the FPGA configures.                                      \n")
        # output_file.write("  // Memory will not be re-initialized at reset.                                \n")
        # output_file.write("  // Dissable this behaviour using the no_initialize_memory switch              \n")
#
        # output_file.write("  \n  initial\n")
        # output_file.write("  begin\n")
        # for location, content in allocator.memory_content.iteritems():
            # output_file.write("    memory[%s] = %s;\n"%(location, content))
        # output_file.write("  end\n\n")
    output_file.write(
        "\n  //////////////////////////////////////////////////////////////////////////////\n")
    output_file.write(
        "  // INSTRUCTION INITIALIZATION                                                 \n")
    output_file.write(
        "  //                                                                            \n")
    output_file.write(
        "  // Initialise the contents of the instruction memory                          \n")
    output_file.write("  //\n")
    output_file.write("  // Intruction Set\n")
    output_file.write("  // ==============\n")
    for num, opcode in enumerate(instruction_set):
        output_file.write("  // %s %s\n" % (num, opcode))

    output_file.write("  // Intructions\n")
    output_file.write("  // ===========\n")
    output_file.write("  \n  initial\n")
    output_file.write("  begin\n")
    for location, instruction in enumerate(instruction_memory):
        output_file.write("    instructions[%s] = {%s, %s, %s, %s};//%s : %s %s\n" % (
            location,
            print_verilog_literal(opcode_bits, instruction["op"]),
            print_verilog_literal(4, instruction.get("z", 0)),
            print_verilog_literal(4, instruction.get("a", 0)),
            print_verilog_literal(
            16, instruction["literal"] | instruction.get("b", 0)),
            instruction["filename"],
            instruction["lineno"],
            instruction["comment"],
        ))
    output_file.write("  end\n\n")

    if input_files or output_files:

        output_file.write(
            "\n  //////////////////////////////////////////////////////////////////////////////\n")
        output_file.write(
            "  // OPEN FILES                                                                 \n")
        output_file.write(
            "  //                                                                            \n")
        output_file.write(
            "  // Open all files used at the start of the process                            \n")

        output_file.write("  \n  initial\n")
        output_file.write("  begin\n")
        for file_name, file_ in input_files.iteritems():
            output_file.write(
                "    %s = $fopenr(\"%s\");\n" %
                (file_, file_name))
        for file_name, file_ in output_files.iteritems():
            output_file.write(
                "    %s = $fopen(\"%s\");\n" %
                (file_, file_name))
        output_file.write("  end\n\n")

    CodeWriter(output_file).write(r"""
        //////////////////////////////////////////////////////////////////////////////
        // PIPELINE STAGE 1 -- FETCH INSTRUCTION
        //                                                                            
        always @(posedge clk)
        begin
          //implement memory for instructions
          if ((state == instruction_fetch || state == operand_fetch ||
              state == execute || state == mem_access) &&
              !must_insert_bubble) begin
            instruction <= instructions[program_counter];
            program_counter_1 <= program_counter;
          end
        end

        assign opcode    = instruction[`opcode_upper_index`:`opcode_lower_index`];
        assign address_z = instruction[23:20];
        assign address_a = instruction[19:16];
        assign address_b = instruction[3:0];
        assign literal   = instruction[15:0];
        assign forward_a = (address_a_2 == address_z_3 && writeback_enable);
        assign forward_b = (address_b_2 == address_z_3 && writeback_enable);
        assign forward_z = (address_z_2 == address_z_3 && writeback_enable);
        assign forward_from_mem_a = (address_a_2 == address_z_4 && writeback_enable_2);
        assign forward_from_mem_b = (address_b_2 == address_z_4 && writeback_enable_2);
        assign forward_from_mem_z = (address_z_2 == address_z_4 && writeback_enable_2);
        assign must_insert_bubble =
          (address_a == address_z_2 ||
          address_b == address_z_2 ||
          address_z == address_z_2) && two_cycle_instr_in_ex;

    """, indent=2, 
        opcode_upper_index=instruction_bits - 1,
        opcode_lower_index=instruction_bits - opcode_bits)
    output_file.write("assign two_cycle_instr_in_ex = (state == execute\n")
    output_file.write("    || state == mem_access) && (0")
    for two_cycle_instr in two_cycle_instrs:
        if two_cycle_instr in opcodes:
            output_file.write("\n    || opcode_2 == {}".format(
                find_instruction_opcode(instruction_set, two_cycle_instr)))
    output_file.write(");")

    CodeWriter(output_file).write(r"""

        //////////////////////////////////////////////////////////////////////////////
        // PIPELINE STAGE 2 -- FETCH OPERANDS
        //
        always @(posedge clk)
        begin
            if (state == operand_fetch || state == execute ||
                state == mem_access) begin
                if(must_insert_bubble)
                  opcode_2 <= 0; //nop
                else
                  opcode_2 <= opcode;
                literal_2 <= literal;
                address_a_2 <= address_a;
                address_b_2 <= address_b;
                address_z_2 <= address_z;
                program_counter_2 <= program_counter_1;
            end
        end
        assign register_a = registers[address_a_2];
        assign register_b = registers[address_b_2];
        assign register_z = registers[address_z_2];
        assign operand_a = 
          (forward_a ? result : (forward_from_mem_a ? result_2 : register_a));
        assign operand_b = 
          (forward_b ? result : (forward_from_mem_b ? result_2 : register_b));
        assign operand_z =
          (forward_z ? result : (forward_from_mem_z ? result_2 : register_z));

        assign next_program_counter = must_insert_bubble ? 
            program_counter : program_counter + 1;

    """, indent=2)

    output_file.write(
        "\n\n  //////////////////////////////////////////////////////////////////////////////\n")
    output_file.write("  // PIPELINE STAGE 3 -- EXECUTE\n")
    output_file.write(
        "  //                                                                            \n")

    output_file.write("  \n  always @(posedge clk)\n")
    output_file.write("  begin\n\n")
    output_file.write("  writeback_enable <= 0;\n")
    if len(asyncio_ports) > 0:
        output_file.write("  asyncio_length_write <= 0;\n")
        output_file.write("  asyncio_length_read <= 0;\n")
            
    output_file.write("  timer_clock <= timer_clock + 1;\n")
    for input_name in program.input_names:
        output_file.write("  s_input_%s_ack <= 0;\n" % input_name)
    output_file.write("  if(state == instruction_fetch) begin\n")
    output_file.write("    program_counter <= next_program_counter;\n")
    output_file.write("    state <= operand_fetch;\n")
    output_file.write("  end if(state == operand_fetch) begin\n")
    output_file.write("    program_counter <= next_program_counter;\n")
    output_file.write("    state <= execute;\n")
    output_file.write("  end if(state == execute || state == mem_access) begin\n")
    output_file.write("      program_counter <= next_program_counter;\n")
    output_file.write("      prev_state <= state;\n")
    output_file.write("      state <= mem_access;\n")
    output_file.write("      address_z_3 <= address_z_2;\n")
    output_file.write("      opcode_3 <= opcode_2;\n")
    output_file.write("      case(opcode_2)\n\n")

    # A frame is executed in each state
    for opcode, instruction in enumerate(instruction_set):

        output_file.write("        //%s\n" % (instruction["op"]))
        output_file.write("        16'd%s:\n" % (opcode))
        output_file.write("        begin\n")

        if instruction["op"] == "nop":
            pass

        elif instruction["op"] == "literal":
            output_file.write("          result<=$signed(literal_2);\n")
            output_file.write("          writeback_enable <= 1;\n")

        #TODO refactor using $signed()?
        elif instruction["op"] == "addl":
            output_file.write("          if(literal_2[15]) begin\n")
            output_file.write("            result <= operand_a + {-16'd1, literal_2};\n")
            output_file.write("          end else begin\n")
            output_file.write("            result <= operand_a + literal_2;\n")
            output_file.write("          end\n")
            #output_file.write("          result<=operand_a + literal_2;\n")
            output_file.write("          writeback_enable <= 1;\n")

        elif instruction["op"] == "literal_hi":
            output_file.write(
                "          result<= {literal_2, operand_a[15:0]};\n")
            output_file.write("          writeback_enable <= 1;\n")

        elif instruction["op"] in ["store", "store_disp", "store8", "store16"]:
            output_file.write("          store_address <= operand_z + {{2{literal_2[13]}}, literal_2[13:0]};\n")
            output_file.write("          store_data <= operand_a;\n")

        elif instruction["op"] in ["load", "load_disp", "load8", "load16"]:
            output_file.write("          load_address <= operand_a + {{3{literal_2[12]}}, literal_2[12:0]};\n")
            output_file.write("          signed_load <= literal_2[15];\n")
            output_file.write("          writeback_enable <= 1;\n")

        elif instruction["op"] == "call":
            output_file.write("          result <= program_counter_2 + 1;\n")
            output_file.write("          writeback_enable <= 1;\n")
            output_file.write("          program_counter <= literal_2;\n")
            output_file.write("          state <= instruction_fetch;\n")

        elif instruction["op"] == "return":
            output_file.write("          program_counter <= operand_a;\n")
            output_file.write("          state <= instruction_fetch;\n")

        elif instruction["op"] == "a_lo":
            output_file.write("          a_lo <= operand_a;\n")
            output_file.write("          result <= a_lo;\n")
            output_file.write("          writeback_enable <= 1;\n")

        elif instruction["op"] == "b_lo":
            output_file.write("          b_lo <= operand_a;\n")
            output_file.write("          result <= b_lo;\n")
            output_file.write("          writeback_enable <= 1;\n")

        elif instruction["op"] == "a_hi":
            output_file.write("          a_hi <= operand_a;\n")
            output_file.write("          result <= a_hi;\n")
            output_file.write("          writeback_enable <= 1;\n")

        elif instruction["op"] == "b_hi":
            output_file.write("          b_hi <= operand_a;\n")
            output_file.write("          result <= b_hi;\n")
            output_file.write("          writeback_enable <= 1;\n")

        elif instruction["op"] == "a_lo_in":
            output_file.write("          a_lo <= operand_a;\n")
        elif instruction["op"] == "b_lo_in":
            output_file.write("          b_lo <= operand_a;\n")
        elif instruction["op"] == "a_hi_in":
            output_file.write("          a_hi <= operand_a;\n")
        elif instruction["op"] == "b_hi_in":
            output_file.write("          b_hi <= operand_a;\n")

        elif instruction["op"] == "a_lo_out":
            output_file.write("          result <= a_lo;\n")
            output_file.write("          writeback_enable <= 1;\n")
        elif instruction["op"] == "b_lo_out":
            output_file.write("          result <= b_lo;\n")
            output_file.write("          writeback_enable <= 1;\n")
        elif instruction["op"] == "a_hi_out":
            output_file.write("          result <= a_hi;\n")
            output_file.write("          writeback_enable <= 1;\n")
        elif instruction["op"] == "b_hi_out":
            output_file.write("          result <= b_hi;\n")
            output_file.write("          writeback_enable <= 1;\n")

        elif instruction["op"] == "not":
            output_file.write("          result <= ~operand_a;\n")
            output_file.write("          writeback_enable <= 1;\n")

        elif instruction["op"] == "int_to_long":
            output_file.write("          if(operand_a[31]) begin\n")
            output_file.write("            result <= -1;\n")
            output_file.write("          end else begin\n")
            output_file.write("            result <= 0;\n")
            output_file.write("          end\n")
            output_file.write("          writeback_enable <= 1;\n")

        elif instruction["op"] == "int_to_float":
            output_file.write("          int_to_float_in <= a_lo;\n")
            output_file.write("          state <= int_to_float_write_a;\n")

        elif instruction["op"] == "float_to_int":
            output_file.write("          float_to_int_in <= a_lo;\n")
            output_file.write("          state <= float_to_int_write_a;\n")

        elif instruction["op"] == "long_to_double":
            output_file.write("          long_to_double_in <= {a_hi, a_lo};\n")
            output_file.write("          state <= long_to_double_write_a;\n")

        elif instruction["op"] == "double_to_long":
            output_file.write("          double_to_long_in <= {a_hi, a_lo};\n")
            output_file.write("          state <= double_to_long_write_a;\n")

        elif instruction["op"] == "float_to_double":
            output_file.write("          float_to_double_in <= a_lo;\n")
            output_file.write("          state <= float_to_double_write_a;\n")

        elif instruction["op"] == "double_to_float":
            output_file.write(
                "          double_to_float_in <= {a_hi, a_lo};\n")
            output_file.write("          state <= double_to_float_write_a;\n")

        elif instruction["op"] == "add":
            output_file.write(
                "          long_result = operand_a + operand_b;\n")
            output_file.write("          result <= long_result[31:0];\n")
            output_file.write("          carry[0] <= long_result[32];\n")
            output_file.write("          writeback_enable <= 1;\n")

        elif instruction["op"] == "add_with_carry":
            output_file.write(
                "          long_result = operand_a + operand_b + carry[0];\n")
            output_file.write("          result <= long_result[31:0];\n")
            output_file.write("          carry[0] <= long_result[32];\n")
            output_file.write("          writeback_enable <= 1;\n")

        elif instruction["op"] == "subtract":
            output_file.write(
                "          long_result = operand_a + (~operand_b) + 1;\n")
            output_file.write("          result <= long_result[31:0];\n")
            output_file.write("          carry[0] <= ~long_result[32];\n")
            output_file.write("          writeback_enable <= 1;\n")

        elif instruction["op"] == "subtract_with_carry":
            output_file.write(
                "          long_result = operand_a + (~operand_b) + carry[0];\n")
            output_file.write("          result <= long_result[31:0];\n")
            output_file.write("          carry[0] <= ~long_result[32];\n")
            output_file.write("          writeback_enable <= 1;\n")

        elif instruction["op"] == "multiply":
          CodeWriter(output_file).write(r"""
            products[0] <= operand_a[15:0]  * operand_b[15:0];
            products[1] <= operand_a[15:0]  * operand_b[31:16];
            products[2] <= operand_a[31:16] * operand_b[15:0];
            products[3] <= operand_a[31:16] * operand_b[31:16];

            multiply_step <= 0;

            prev_state <= state;
            state <= ex2_stalled;
          """, indent=2)

#TODO put these into MEM
        elif instruction["op"] == "unsigned_divide":
            output_file.write("          dividend  <= operand_a;\n")
            output_file.write("          divisor <= operand_b;\n")
            output_file.write("          timer <= %i;\n"%divide_latency)
            output_file.write("          remainder <= 0;\n")
            output_file.write("          quotient  <= 0;\n")
            output_file.write("          state <= unsigned_divide;\n")

        elif instruction["op"] == "divide":
            output_file.write("          quotient_sign <= operand_a[31] ^ operand_b[31];\n")
            output_file.write("          dividend  <= operand_a;\n")
            output_file.write("          divisor <= operand_b;\n")
            output_file.write("          if (operand_a[31]) begin\n")
            output_file.write("            dividend <= -operand_a;\n")
            output_file.write("          end\n")
            output_file.write("          if (operand_b[31]) begin\n")
            output_file.write("            divisor <= -operand_b;\n")
            output_file.write("          end\n")
            output_file.write("          timer <= %i;\n"%divide_latency)
            output_file.write("          remainder <= 0;\n")
            output_file.write("          quotient <= 0;\n")
            output_file.write("          state <= divide;\n")

        elif instruction["op"] == "unsigned_modulo":
            output_file.write("          dividend  <= operand_a;\n")
            output_file.write("          divisor <= operand_b;\n")
            output_file.write("          timer <= %i;\n"%divide_latency)
            output_file.write("          remainder <= 0;\n")
            output_file.write("          quotient  <= 0;\n")
            output_file.write("          state <= unsigned_modulo;\n")

        elif instruction["op"] == "modulo":
            output_file.write("          dividend_sign  <= operand_a[31];\n")
            output_file.write("          dividend <= operand_a;\n")
            output_file.write("          divisor <= operand_b;\n")
            output_file.write("          if (operand_a[31]) begin\n")
            output_file.write("            dividend <= -operand_a;\n")
            output_file.write("          end\n")
            output_file.write("          if (operand_b[31]) begin\n")
            output_file.write("            divisor <= -operand_b;\n")
            output_file.write("          end\n")
            output_file.write("          timer <= %i;\n"%divide_latency)
            output_file.write("          remainder <= 0;\n")
            output_file.write("          quotient <= 0;\n")
            output_file.write("          state <= modulo;\n")

        elif instruction["op"] == "unsigned_long_divide":
            output_file.write("          long_dividend <= {a_hi, a_lo};\n")
            output_file.write("          long_divisor <= {b_hi, b_lo};\n")
            output_file.write("          timer <= %i;\n"%long_divide_latency)
            output_file.write("          long_remainder <= 0;\n")
            output_file.write("          long_quotient  <= 0;\n")
            output_file.write("          state <= unsigned_long_divide;\n")

        elif instruction["op"] == "long_divide":
            output_file.write("          long_quotient_sign <= a_hi[31] ^ b_hi[31];\n")
            output_file.write("          long_dividend <= {a_hi, a_lo};\n")
            output_file.write("          long_divisor <= {b_hi, b_lo};\n")
            output_file.write("          if (a_hi[31]) begin\n")
            output_file.write("            long_dividend <= -{a_hi, a_lo};\n")
            output_file.write("          end\n")
            output_file.write("          if (b_hi[31]) begin\n")
            output_file.write("            long_divisor <= -{b_hi, b_lo};\n")
            output_file.write("          end\n")
            output_file.write("          timer <= %i;\n"%long_divide_latency)
            output_file.write("          long_remainder <= 0;\n")
            output_file.write("          long_quotient <= 0;\n")
            output_file.write("          state <= long_divide;\n")

        elif instruction["op"] == "unsigned_long_modulo":
            output_file.write("          long_dividend <= {a_hi, a_lo};\n")
            output_file.write("          long_divisor <= {b_hi, b_lo};\n")
            output_file.write("          timer <= %i;\n"%long_divide_latency)
            output_file.write("          long_remainder <= 0;\n")
            output_file.write("          long_quotient  <= 0;\n")
            output_file.write("          state <= unsigned_long_modulo;\n")

        elif instruction["op"] == "long_modulo":
            output_file.write("          long_dividend_sign <= a_hi[31];\n")
            output_file.write("          long_dividend <= {a_hi, a_lo};\n")
            output_file.write("          long_divisor <= {b_hi, b_lo};\n")
            output_file.write("          if (a_hi[31]) begin\n")
            output_file.write("            long_dividend <= -{a_hi, a_lo};\n")
            output_file.write("          end\n")
            output_file.write("          if (b_hi[31]) begin\n")
            output_file.write("            long_divisor <= -{b_hi, b_lo};\n")
            output_file.write("          end\n")
            output_file.write("          timer <= %i;\n"%long_divide_latency)
            output_file.write("          long_remainder <= 0;\n")
            output_file.write("          long_quotient <= 0;\n")
            output_file.write("          state <= long_modulo;\n")

        elif instruction["op"] == "carry":
            output_file.write("          result <= carry;\n")
            output_file.write("          writeback_enable <= 1;\n")

        elif instruction["op"] == "or":
            output_file.write("          result <= operand_a | operand_b;\n")
            output_file.write("          writeback_enable <= 1;\n")

        elif instruction["op"] == "and":
            output_file.write("          result <= operand_a & operand_b;\n")
            output_file.write("          writeback_enable <= 1;\n")

        elif instruction["op"] == "xor":
            output_file.write("          result <= operand_a ^ operand_b;\n")
            output_file.write("          writeback_enable <= 1;\n")

        elif instruction["op"] == "shift_left":
            CodeWriter(output_file).write(r"""
              result <= operand_a << operand_b[3:0];
              shift_amount <= operand_b[5:4];
            """, indent=4)

        elif instruction["op"] == "shift_left_with_carry":
            output_file.write("          if(operand_b < 32) begin\n")
            output_file.write(
                "            result <= (operand_a << operand_b) | carry;\n")
            output_file.write("          end else begin\n")
            output_file.write("            result <= carry;\n")
            output_file.write("          end\n")
            output_file.write("          writeback_enable <= 1;\n")

        elif instruction["op"] == "shift_right":
            CodeWriter(output_file).write(r"""
              result <= $signed(operand_a) >>> operand_b[3:0];
              shift_amount <= operand_b[5:4];
            """, indent=4)

#            output_file.write("          if(operand_b < 32) begin\n")
#            output_file.write(
#                "            result <= $signed(operand_a) >>> operand_b;\n")
#            output_file.write(
#                "            carry <= operand_a << (32-operand_b);\n")
#            output_file.write("          end else begin\n")
#            output_file.write("            result <= operand_a[31]?-1:0;\n")
#            output_file.write("            carry <= operand_a;\n")
#            output_file.write("          end\n")
#            output_file.write("          writeback_enable <= 1;\n")

        elif instruction["op"] == "unsigned_shift_right":
            CodeWriter(output_file).write(r"""
              result <= operand_a >> operand_b[3:0];
              shift_amount <= operand_b[5:4];
            """, indent=4)

        elif instruction["op"] == "shift_right_with_carry":
            output_file.write("          if(operand_b < 32) begin\n")
            output_file.write(
                "            result <= (operand_a >> operand_b) | carry;\n")
            output_file.write("          end else begin\n")
            output_file.write("            result <= carry;\n")
            output_file.write("          end\n")
            output_file.write("          writeback_enable <= 1;\n")

        elif instruction["op"] == "greater":
            output_file.write(
                "          result <= $signed(operand_a) > $signed(operand_b);\n")
            output_file.write("          writeback_enable <= 1;\n")

        elif instruction["op"] == "greater_equal":
            output_file.write(
                "          result <= $signed(operand_a) >= $signed(operand_b);\n")
            output_file.write("          writeback_enable <= 1;\n")

        elif instruction["op"] == "unsigned_greater":
            output_file.write(
                "          result <= $unsigned(operand_a) > $unsigned(operand_b);\n")
            output_file.write("          writeback_enable <= 1;\n")

        elif instruction["op"] == "unsigned_greater_equal":
            output_file.write(
                "          result <= $unsigned(operand_a) >= $unsigned(operand_b);\n")
            output_file.write("          writeback_enable <= 1;\n")

        elif instruction["op"] == "equal":
            output_file.write("          result <= operand_a == operand_b;\n")
            output_file.write("          writeback_enable <= 1;\n")

        elif instruction["op"] == "not_equal":
            output_file.write("          result <= operand_a != operand_b;\n")
            output_file.write("          writeback_enable <= 1;\n")

        elif instruction["op"] == "float_add":
            output_file.write("          adder_a_stb <= 1;\n")
            output_file.write("          adder_a <= operand_a;\n")
            output_file.write("          adder_b <= operand_b;\n")
            output_file.write("          state <= adder_write_a;\n")

        elif instruction["op"] == "float_subtract":
            output_file.write("          adder_a_stb <= 1;\n")
            output_file.write("          adder_a <= operand_a;\n")
            output_file.write(
                "          adder_b <= {~operand_b[31], operand_b[30:0]};\n")
            output_file.write("          state <= adder_write_a;\n")

        elif instruction["op"] == "float_multiply":
            output_file.write("          multiplier_a_stb <= 1;\n")
            output_file.write("          multiplier_a <= operand_a;\n")
            output_file.write("          multiplier_b <= operand_b;\n")
            output_file.write("          state <= multiplier_write_a;\n")


        elif instruction["op"] == "float_divide":
            output_file.write("          divider_a_stb <= 1;\n")
            output_file.write("          divider_a <= operand_a;\n")
            output_file.write("          divider_b <= operand_b;\n")
            output_file.write("          state <= divider_write_a;\n")

        elif instruction["op"] == "long_float_add":
            output_file.write("          double_adder_a <= {a_hi, a_lo};\n")
            output_file.write("          double_adder_b <= {b_hi, b_lo};\n")
            output_file.write("          state <= double_adder_write_a;\n")

        elif instruction["op"] == "long_float_subtract":
            output_file.write("          double_adder_a <= {a_hi, a_lo};\n")
            output_file.write(
                "          double_adder_b <= {~b_hi[31], b_hi[30:0], b_lo};\n")
            output_file.write("          state <= double_adder_write_a;\n")

        elif instruction["op"] == "long_float_multiply":
            output_file.write(
                "          double_multiplier_a <= {a_hi, a_lo};\n")
            output_file.write(
                "          double_multiplier_b <= {b_hi, b_lo};\n")
            output_file.write(
                "          state <= double_multiplier_write_a;\n")

        elif instruction["op"] == "long_float_divide":
            output_file.write("          double_divider_a <= {a_hi, a_lo};\n")
            output_file.write("          double_divider_b <= {b_hi, b_lo};\n")
            output_file.write("          state <= double_divider_write_a;\n")

        elif instruction["op"] == "jmp_if_false":
            output_file.write("          if (operand_a == 0) begin\n")
            output_file.write("            program_counter <= literal_2;\n")
            output_file.write("            state <= instruction_fetch;\n")
            output_file.write("          end\n")

        elif instruction["op"] == "jmp_if_true":
            output_file.write("          if (operand_a != 0) begin\n")
            output_file.write("            program_counter <= literal_2;\n")
            output_file.write("            state <= instruction_fetch;\n")
            output_file.write("          end\n")

        elif instruction["op"] == "goto":
            output_file.write("          program_counter <= literal_2;\n")
            output_file.write("          state <= instruction_fetch;\n")

        elif instruction["op"] == "file_read":
            output_file.write("        16'd%s:\n" % (opcode))
            output_file.write("        begin\n")
            output_file.write("          file_count = $fscanf(%s, \"%%d\\n\", result);\n" % (
                              input_files[instruction["file_name"]]))
            output_file.write("          writeback_enable <= 1;\n")
            output_file.write("        end\n\n")

        elif instruction["op"] == "float_file_write":
            output_file.write('          long_result[63] = operand_a[31];\n')
            output_file.write('          if (operand_a[30:23] == 0) begin\n')
            output_file.write('              long_result[62:52] = 0;\n')
            output_file.write(
                '          end else if (operand_a[30:23] == 127) begin\n')
            output_file.write('              long_result[62:52] = 1023;\n')
            output_file.write('          end else begin\n')
            output_file.write(
                '              long_result[62:52] = (operand_a[30:23] - 127) + 1023;\n')
            output_file.write('          end\n')
            output_file.write(
                '          long_result[51:29] = operand_a[22:0];\n')
            output_file.write('          long_result[28:0] = 0;\n')
            output_file.write(
                '          fp_value = $bitstoreal(long_result);\n')
            output_file.write('          $fdisplay (%s, "%%g", fp_value);\n' % (
                              output_files[
                              instruction["file_name"]]))

        elif instruction["op"] == "long_float_file_write":
            output_file.write(
                '          fp_value = $bitstoreal({a_hi, a_lo});\n')
            output_file.write('          $fdisplay (%s, "%%g", fp_value);\n' % (
                              output_files[
                              instruction["file_name"]]))

        elif instruction["op"] == "unsigned_file_write":
            output_file.write("          $fdisplay (%s, \"%%d\", $unsigned(operand_a));\n" % (
                              output_files[instruction["file_name"]]))

        elif instruction["op"] == "file_write":
            output_file.write("          $fdisplay (%s, \"%%d\", $signed(operand_a));\n" % (
                              output_files[instruction["file_name"]]))

        elif instruction["op"] == "read":
            if len(program.input_names) > 0:
                output_file.write("          case(operand_a)\n")
                for handle, input_name in program.id_to_input.items():
                    output_file.write("      %s:\n" % (handle))
                    output_file.write("      begin\n")
                    output_file.write("        if (input_%s_stb && !s_input_%s_ack) begin\n" % (input_name, input_name))
                    output_file.write("          result <= input_%s;\n" % input_name)
                    output_file.write("          writeback_enable <= 1;\n")
                    output_file.write("          s_input_%s_ack <= 1;\n" % input_name)
                    output_file.write("        end else begin\n")
                    output_file.write("          s_input_%s_ack <= 1;\n" % input_name)
                    output_file.write("          state <= read;\n")
                    output_file.write("          read_input <= operand_a;\n")
                    output_file.write("        end\n")
                    output_file.write("      end\n")
                output_file.write("          endcase\n")
            else:
                output_file.write("          state <= read;\n")
                output_file.write("          read_input <= operand_a;\n")

        elif instruction["op"] == "ready":
            output_file.write("          result <= 0;\n")
            output_file.write("          case(operand_a)\n\n")
            for handle, input_name in program.id_to_input.items():
                output_file.write("            %s:\n" % (handle))
                output_file.write("            begin\n")
                output_file.write(
                    "              result[0] <= input_%s_stb;\n" %
                    input_name)
                output_file.write("            end\n")
            output_file.write("          endcase\n")
            output_file.write("          writeback_enable <= 1;\n")

        elif instruction["op"] == "output_ready":
            output_file.write("          result <= 0;\n")
            output_file.write("          case(operand_a)\n\n")
            for handle, output_name in program.id_to_output.items():
                output_file.write("            %s:\n" % (handle))
                output_file.write("            begin\n")
                output_file.write(
                    "              result[0] <= output_%s_ack;\n" %
                    output_name)
                output_file.write("            end\n")
            output_file.write("          endcase\n")
            output_file.write("          writeback_enable <= 1;\n")

        elif instruction["op"] == "write":
            output_file.write("          state <= write;\n")
            output_file.write("          write_output <= operand_a;\n")
            output_file.write("          write_value <= operand_b;\n")

        elif instruction["op"] == "assert":
            output_file.write("          if (operand_a == 0) begin\n")
            output_file.write("            $display(\"Assertion failed at line: %s in file: %s\");\n" % (
                              instruction["line"],
                              instruction["file"]))
            output_file.write("            $finish_and_return(1);\n")
            output_file.write("          end\n")

        elif instruction["op"] == "wait_clocks":
            output_file.write("          timer <= operand_a;\n")
            output_file.write("          state <= wait_state;\n")

        elif instruction["op"] == "timer_low":
            output_file.write("          result <= timer_clock[31:0];\n")
            output_file.write("          writeback_enable <= 1;\n")

        elif instruction["op"] == "timer_high":
            output_file.write("          result <= timer_clock[63:32];\n")
            output_file.write("          writeback_enable <= 1;\n")

        elif instruction["op"] == "report":
            output_file.write('          $display ("%%d (report (int) at line: %s in file: %s)", $signed(a_lo));\n' % (
                instruction["line"],
                instruction["file"],))

        elif instruction["op"] == "long_report":
            output_file.write('          $display ("%%d (report (long) at line: %s in file: %s)", $signed({a_hi, a_lo}));\n' % (
                instruction["line"],
                instruction["file"],))

        elif instruction["op"] == "float_report":
            output_file.write('          long_result[63] = a_lo[31];\n')
            output_file.write('          if (a_lo[30:23] == 0) begin\n')
            output_file.write('              long_result[62:52] = 0;\n')
            output_file.write(
                '          end else if (a_lo[30:23] == 255) begin\n')
            output_file.write('              long_result[62:52] = 2047;\n')
            output_file.write('          end else begin\n')
            output_file.write(
                '              long_result[62:52] = (a_lo[30:23] - 127) + 1023;\n')
            output_file.write('          end\n')
            output_file.write('          long_result[51:29] = a_lo[22:0];\n')
            output_file.write('          long_result[28:0] = 0;\n')
            output_file.write(
                '          fp_value = $bitstoreal(long_result);\n')
            output_file.write('          $display ("%%f (report (float) at line: %s in file: %s)", fp_value);\n' % (
                              instruction["line"],
                              instruction["file"]))

        elif instruction["op"] == "long_float_report":
            output_file.write(
                '          fp_value = $bitstoreal({a_hi, a_lo});\n')
            output_file.write('          $display ("%%f (report (double) at line: %s in file: %s)", fp_value);\n' % (
                              instruction["line"],
                              instruction["file"]))

        elif instruction["op"] == "unsigned_report":
            output_file.write('          $display ("%%d (report (unsigned) at line: %s in file: %s)", $unsigned(a_lo));\n' % (
                instruction["line"],
                instruction["file"]))

        elif instruction["op"] == "long_unsigned_report":
            output_file.write('          $display ("%%d (report (unsigned long) at line: %s in file: %s)", $unsigned({a_hi, a_lo}));\n' % (
                instruction["line"],
                instruction["file"]))

        elif instruction["op"] == "stop":
            # If we are in testbench mode stop the simulation
            # If we are part of a larger design, other C programs may still be
            # running
            for file_ in input_files.values():
                output_file.write("          $fclose(%s);\n" % file_)
            for file_ in output_files.values():
                output_file.write("          $fclose(%s);\n" % file_)
            if testbench:
                output_file.write('          $finish;\n')
            output_file.write('        state <= stop;\n')

        elif instruction["op"] == "asyncio_start_read":
            CodeWriter(output_file).write(r"""
            asyncio_length_read <= operand_a[23:0];
            asyncio_address_read <= operand_b;
            asyncio_read_id <= operand_a[31:24];
            asyncio_reading <= 1;
            """, indent=2)
        elif instruction["op"] == "asyncio_start_write":
            CodeWriter(output_file).write(r"""
            asyncio_length_write <= operand_a[23:0];
            asyncio_address_write <= operand_b;
            asyncio_write_id <= operand_a[31:24];
            asyncio_writing <= 1;
            """, indent=2)
        elif instruction["op"] == "asyncio_wait_read":
            CodeWriter(output_file).write(r"""
            prev_state <= state;
            state <= asyncio_wait;
            asyncio_wait_mode <= 0;
            """, indent=2)
        elif instruction["op"] == "asyncio_wait_write":
            CodeWriter(output_file).write(r"""
            prev_state <= state;
            state <= asyncio_wait;
            asyncio_wait_mode <= 1;
            """, indent=2)

        else:
            raise Exception("unsupported instruction", instruction["op"],
                    instruction)

        output_file.write("        end\n\n")
    output_file.write("      endcase\n\n")
    output_file.write("    end \n\n")

    output_file.write("    case(state)\n")

#TODO remove
    if "ex2_stalled" in states:
        CodeWriter(output_file).write(r"""
                ex2_stalled:
                begin
                  case(opcode_3)
                    0: begin end
        """, indent=2)
        for opcode in ["multiply"]:
            if opcode not in opcodes:
                continue
            #TODO compute carry!
            #TODO remove shift_with_carry??
            if opcode == "multiply":
                code = """
//                        long_result = products[0] +
//                                        (products[1] << 16) +
//                                        (products[2] << 16) +
//                                        (products[3] << 32);
                    if(multiply_step == 0) begin
                        multiply_step <= 1;
                        sums[0] <= products[0] + (products[1] << 16);
                        sums[1] <= products[2] + (products[3] << 16);
                    end else if(multiply_step == 1) begin
                        multiply_step <= 2;
                        sums[2] <= {sums[1][15:0], 16'd0} + sums[0][31:0];
                    end else if(multiply_step == 2) begin
                        long_result[31:0] = sums[2];
                        long_result[63:32] = 
                          {16'd0, sums[0][47:32]} + sums[1][47:16] + sums[2][32];
                        //long_result = sums[0] + (sums[1] << 16);
                        result <= long_result[31:0];
                        carry <= long_result[63:32];
                        writeback_enable <= 1;
                        state <= prev_state;
                    end

                """
            elif opcode == "shift_left":
                code = """
                    result <= result << { shift_amount, 4'd0 };
                """
            elif opcode == "unsigned_shift_right":
                code = """
                    result <= result >> { shift_amount, 4'd0 };
                """
            elif opcode == "shift_right":
                code = """
                    result <= $signed(result) >>> { shift_amount, 4'd0 };
                """
            if opcode == "multiply":
                CodeWriter(output_file).write(r"""

                    `opcode`: begin
                        `code`
                    end
                """, indent=6, opcode=find_instruction_opcode(
                    instruction_set, opcode), code=code)
            else:
                CodeWriter(output_file).write(r"""

                    `opcode`: begin
                        `code`
                        writeback_enable <= 1;
                        state <= prev_state;
                    end
                """, indent=6, opcode=find_instruction_opcode(
                    instruction_set, opcode), code=code)

        CodeWriter(output_file).write(r"""

            endcase
          end

        """, indent=4)


    if "unsigned_divide" in opcodes:
        output_file.write("    unsigned_divide:\n")
        output_file.write("    begin\n")
        output_file.write("      if (timer) begin\n")
        output_file.write("        timer <= timer - 1;\n")
        output_file.write("      end else begin\n")
        output_file.write("        result <= quotient;\n")
        output_file.write("        state <= prev_state;\n")
        output_file.write("        writeback_enable <= 1;\n")
        output_file.write("      end\n")
        output_file.write("    end\n\n")

    if "divide" in opcodes:
        output_file.write("    divide:\n")
        output_file.write("    begin\n")
        output_file.write("      if (timer) begin\n")
        output_file.write("        timer <= timer - 1;\n")
        output_file.write("      end else begin\n")
        output_file.write("        if (quotient_sign) begin\n")
        output_file.write("          result <= -quotient;\n")
        output_file.write("        end else begin\n")
        output_file.write("          result <= quotient;\n")
        output_file.write("        end\n")
        output_file.write("        state <= prev_state;\n")
        output_file.write("        writeback_enable <= 1;\n")
        output_file.write("      end\n")
        output_file.write("    end\n\n")

    if "unsigned_modulo" in opcodes:
        output_file.write("    unsigned_modulo:\n")
        output_file.write("    begin\n")
        output_file.write("      if (timer) begin\n")
        output_file.write("        timer <= timer - 1;\n")
        output_file.write("      end else begin\n")
        output_file.write("        result <= remainder;\n")
        output_file.write("        state <= prev_state;\n")
        output_file.write("        writeback_enable <= 1;\n")
        output_file.write("      end\n")
        output_file.write("    end\n\n")

    if "modulo" in opcodes:
        output_file.write("    modulo:\n")
        output_file.write("    begin\n")
        output_file.write("      if (timer) begin\n")
        output_file.write("        timer <= timer - 1;\n")
        output_file.write("      end else begin\n")
        output_file.write("        if (dividend_sign) begin\n")
        output_file.write("          result <= -remainder;\n")
        output_file.write("        end else begin\n")
        output_file.write("          result <= remainder;\n")
        output_file.write("        end\n")
        output_file.write("        state <= prev_state;\n")
        output_file.write("        writeback_enable <= 1;\n")
        output_file.write("      end\n")
        output_file.write("    end\n\n")

    if "unsigned_long_divide" in opcodes:
        output_file.write("    unsigned_long_divide:\n")
        output_file.write("    begin\n")
        output_file.write("      if (timer) begin\n")
        output_file.write("        timer <= timer - 1;\n")
        output_file.write("      end else begin\n")
        output_file.write("        a_hi <= long_quotient[63:32];\n")
        output_file.write("        a_lo <= long_quotient[31:0];\n")
        output_file.write("        state <= prev_state;\n")
        output_file.write("        writeback_enable <= 1;\n")
        output_file.write("      end\n")
        output_file.write("    end\n\n")

    if "long_divide" in opcodes:
        output_file.write("    long_divide:\n")
        output_file.write("    begin\n")
        output_file.write("      if (timer) begin\n")
        output_file.write("        timer <= timer - 1;\n")
        output_file.write("      end else begin\n")
        output_file.write("        if (long_quotient_sign) begin\n")
        output_file.write("          long_result = -long_quotient;\n")
        output_file.write("          a_hi <= long_result[63:32];\n")
        output_file.write("          a_lo <= long_result[31:0];\n")
        output_file.write("        end else begin\n")
        output_file.write("          a_hi <= long_quotient[63:32];\n")
        output_file.write("          a_lo <= long_quotient[31:0];\n")
        output_file.write("        end\n")
        output_file.write("        state <= prev_state;\n")
        output_file.write("        writeback_enable <= 1;\n")
        output_file.write("      end\n")
        output_file.write("    end\n\n")

    if "unsigned_long_modulo" in opcodes:
        output_file.write("    unsigned_long_modulo:\n")
        output_file.write("    begin\n")
        output_file.write("      if (timer) begin\n")
        output_file.write("        timer <= timer - 1;\n")
        output_file.write("      end else begin\n")
        output_file.write("        a_hi <= long_remainder[63:32];\n")
        output_file.write("        a_lo <= long_remainder[31:0];\n")
        output_file.write("        state <= prev_state;\n")
        output_file.write("        writeback_enable <= 1;\n")
        output_file.write("      end\n")
        output_file.write("    end\n\n")

    if "long_modulo" in opcodes:
        output_file.write("    long_modulo:\n")
        output_file.write("    begin\n")
        output_file.write("      if (timer) begin\n")
        output_file.write("        timer <= timer - 1;\n")
        output_file.write("      end else begin\n")
        output_file.write("        if (long_dividend_sign) begin\n")
        output_file.write("          long_result = -long_remainder;\n")
        output_file.write("          a_hi <= long_result[63:32];\n")
        output_file.write("          a_lo <= long_result[31:0];\n")
        output_file.write("        end else begin\n")
        output_file.write("          a_hi <= long_remainder[63:32];\n")
        output_file.write("          a_lo <= long_remainder[31:0];\n")
        output_file.write("        end\n")
        output_file.write("        state <= prev_state;\n")
        output_file.write("        writeback_enable <= 1;\n")
        output_file.write("      end\n")
        output_file.write("    end\n\n")

    if len(program.id_to_input) > 0:
        output_file.write("    read:\n")
        output_file.write("    begin\n")
        output_file.write("      case(read_input)\n")
        for handle, input_name in program.id_to_input.items():
            output_file.write("      %s:\n" % (handle))
            output_file.write("      begin\n")
            output_file.write("        s_input_%s_ack <= 1;\n" % input_name)
            output_file.write("        if (input_%s_stb) begin\n" % (
                              input_name))
            output_file.write("          result <= input_%s;\n" % input_name)
            output_file.write("          writeback_enable <= 1;\n")
            #output_file.write("          s_input_%s_ack <= 0;\n" % input_name)
            output_file.write("          state <= prev_state;\n")
            output_file.write("          s_input_%s_ack <= 0;\n" % input_name)
            output_file.write("        end\n")
            output_file.write("      end\n")
        output_file.write("      endcase\n")
        output_file.write("    end\n\n")

    if len(program.output_names) > 0:
        output_file.write("    write:\n")
        output_file.write("    begin\n")
        output_file.write("      case(write_output)\n")
        for handle, output_name in program.id_to_output.items():
            output_file.write("      %s:\n" % (handle))
            output_file.write("      begin\n")
            output_file.write("        s_output_%s_stb <= 1;\n" % output_name)
            output_file.write(
                "        s_output_%s <= write_value;\n" %
                output_name)
            output_file.write("        if (output_%s_ack && s_output_%s_stb) begin\n" % (
                              output_name,
                              output_name))
            output_file.write(
                "          s_output_%s_stb <= 0;\n" %
                output_name)
            output_file.write("          state <= prev_state;\n")
            output_file.write("        end\n")
            output_file.write("      end\n")
        output_file.write("      endcase\n")
        output_file.write("    end\n\n")

    if len(asyncio_ports) > 0:
        CodeWriter(output_file).write(r"""
        asyncio_wait: begin
          case(asyncio_wait_mode)
          0: begin
            if(asyncio_length_out_read == 0) begin
              state <= prev_state;
              asyncio_reading <= 0;
            end
          end
          1: begin
            if(asyncio_length_out_write == 0) begin
              state <= prev_state;
              asyncio_writing <= 0;
            end
          end
          endcase
        end
        """, indent=2)

    if "wait_clocks" in opcodes:
        output_file.write("    wait_state:\n")
        output_file.write("    begin\n")
        output_file.write("      if (timer) begin\n")
        output_file.write("        timer <= timer - 1;\n")
        output_file.write("      end else begin\n")
        output_file.write("        state <= prev_state;\n")
        output_file.write("      end\n")
        output_file.write("    end\n\n")

    output_file.write("    stop:\n")
    output_file.write("    begin\n")
    output_file.write("    end\n\n")

    for i in floating_point_arithmetic:
        output_file.write("    %s_write_a:\n" % i)
        output_file.write("    begin\n")
        output_file.write("      %s_a_stb <= 1;\n" % i)
        output_file.write("      if (%s_a_stb && %s_a_ack) begin\n" % (i, i))
        output_file.write("        %s_a_stb <= 0;\n" % i)
        output_file.write("        state <= %s_write_b;\n" % i)
        output_file.write("      end\n")
        output_file.write("    end\n\n")
        output_file.write("    %s_write_b:\n" % i)
        output_file.write("    begin\n")
        output_file.write("      %s_b_stb <= 1;\n" % i)
        output_file.write("      if (%s_b_stb && %s_b_ack) begin\n" % (i, i))
        output_file.write("        %s_b_stb <= 0;\n" % i)
        output_file.write("        state <= %s_read_z;\n" % i)
        output_file.write("      end\n")
        output_file.write("    end\n\n")
        output_file.write("    %s_read_z:\n" % i)
        output_file.write("    begin\n")
        output_file.write("      %s_z_ack <= 1;\n" % i)
        output_file.write("      if (%s_z_stb && %s_z_ack) begin\n" % (i, i))
        if i.startswith("double"):
            output_file.write("        a_lo <= %s_z[31:0];\n" % i)
            output_file.write("        a_hi <= %s_z[63:32];\n" % i)
        else:
            output_file.write("        result <= %s_z;\n" % i)
            output_file.write("        writeback_enable <= 1;\n")
        output_file.write("        %s_z_ack <= 0;\n" % i)
        output_file.write("        state <= prev_state;\n")
        output_file.write("      end\n")
        output_file.write("    end\n\n")

    for i in floating_point_conversions:
        output_file.write("     %s_write_a:\n" % i)
        output_file.write("     begin\n")
        output_file.write("       %s_in_stb <= 1;\n" % i)
        output_file.write(
            "       if (%s_in_stb && %s_in_ack) begin\n" %
            (i, i))
        output_file.write("         %s_in_stb <= 0;\n" % i)
        output_file.write("         state <= %s_read_z;\n" % i)
        output_file.write("       end\n")
        output_file.write("     end\n\n")
        output_file.write("     %s_read_z:\n" % i)
        output_file.write("     begin\n")
        output_file.write("       %s_out_ack <= 1;\n" % i)
        output_file.write(
            "       if (%s_out_stb && %s_out_ack) begin\n" %
            (i, i))
        output_file.write("         %s_out_ack <= 0;\n" % i)
        if (i.startswith("double") and not i.endswith("float")) or i.endswith("double") or i.startswith("long"):
            output_file.write("         a_lo <= %s_out[31:0];\n" % i)
            output_file.write("         a_hi <= %s_out[63:32];\n" % i)
        else:
            output_file.write("         a_lo <= %s_out;\n" % i)
        output_file.write("         state <= prev_state;\n")
        output_file.write("       end\n")
        output_file.write("     end\n\n")
    output_file.write("    endcase\n\n")

    if needs_divider:
        output_file.write("    //divider kernel logic\n")
        output_file.write("    repeat (%u) begin\n"%(divide_iterations))
        output_file.write("      shifter = {remainder[30:0], dividend[31]};\n")
        output_file.write("      difference = shifter - divisor;\n")
        output_file.write("      dividend = dividend << 1;\n")
        output_file.write("      if (difference[32]) begin\n")
        output_file.write("        remainder = shifter;\n")
        output_file.write("        quotient = quotient << 1;\n")
        output_file.write("      end else begin\n")
        output_file.write("        remainder = difference[31:0];\n")
        output_file.write("        quotient = quotient << 1 | 1;\n")
        output_file.write("      end\n")
        output_file.write("    end\n\n")

    if needs_long_divider:
        output_file.write("    //long divider kernel logic\n")
        output_file.write("    repeat (%u) begin\n"%(divide_iterations))
        output_file.write("      long_shifter = {long_remainder[62:0], long_dividend[63]};\n")
        output_file.write("      long_difference = long_shifter - long_divisor;\n")
        output_file.write("      long_dividend = long_dividend << 1;\n")
        output_file.write("      if (long_difference[64]) begin\n")
        output_file.write("        long_remainder = long_shifter;\n")
        output_file.write("        long_quotient = long_quotient << 1;\n")
        output_file.write("      end else begin\n")
        output_file.write("        long_remainder = long_difference[63:0];\n")
        output_file.write("        long_quotient = long_quotient << 1 | 1;\n")
        output_file.write("      end\n")
        output_file.write("    end\n\n")

    # Reset program counter and control signals
    output_file.write("    if (rst == 1'b1) begin\n")
    output_file.write("      timer <= 0;\n")
    output_file.write("      timer_clock <= 0;\n")
    output_file.write("      program_counter <= 0;\n")
    output_file.write("      address_z_3 <= 0;\n")
    output_file.write("      result <= 0;\n")
    output_file.write("      a = 0;\n")
    output_file.write("      b = 0;\n")
    output_file.write("      z = 0;\n")
    output_file.write("      state <= instruction_fetch;\n")

    for i in inputs:
        output_file.write("      s_input_%s_ack <= 0;\n" % (i))

    for i in outputs:
        output_file.write("      s_output_%s_stb <= 0;\n" % (i))

    for i in floating_point_arithmetic:
        output_file.write("      %s_a_stb <= 0;\n" % (i))
        output_file.write("      %s_b_stb <= 0;\n" % (i))
        output_file.write("      %s_z_ack <= 0;\n" % (i))

    for i in floating_point_conversions:
        output_file.write("      %s_in_stb <= 0;\n" % (i))
        output_file.write("      %s_out_ack <= 0;\n" % (i))

    output_file.write("    end\n")
    output_file.write("  end\n")

    for store_opcode, name in [[store32_opcode, "32"], [store8_opcode, "8"],
            [store16_opcode, "16"]]:
        if store_opcode is None:
            continue
        CodeWriter(output_file).write(r"""
          assign store_enable_`name` = (opcode_3 == `opcode`);
        """, indent=2, name=name, opcode=store_opcode)

    ex2_code = ""
#    for opcode in ["multiply", "shift_left", "unsigned_shift_right",
#            "shift_right"]:
#        output_file.write("        //%s\n" % (instruction["op"]))
#        output_file.write("        16'd%s:\n" % (opcode))
#        output_file.write("        begin\n")
#
#        if instruction["op"] == "multiply":
#            CodeWriter(output_file).write(r"""
#                long_result = product_a +
#                              (product_b << 16) +
#                              (product_c << 16) +
#                              (product_d << 32);
#                result_2 <= long_result[31:0];
#                //carry <= long_result[63:32];
#                writeback_enable_2 <= 1;
#                """, indent=6)
#
#        output_file.write("        end\n")
    for opcode in ["shift_left", "unsigned_shift_right",
            "shift_right"]:
        if opcode not in opcodes:
            continue
        ex2_code += """
          {}: begin
        """.format(find_instruction_opcode(instruction_set, opcode))
        #TODO compute carry! -> forward carry
        #TODO remove shift_with_carry??
        if opcode == "shift_left":
            ex2_code += """
                result_2 <= result << { shift_amount, 4'd0 };
            """
        elif opcode == "unsigned_shift_right":
            ex2_code += """
                result_2 <= result >> { shift_amount, 4'd0 };
            """
        elif opcode == "shift_right":
            ex2_code += """
                result_2 <= $signed(result) >>> { shift_amount, 4'd0 };
            """

        ex2_code += """
          writeback_enable_2 <= 1;
          end
        """

    CodeWriter(output_file).write(r"""
                         
        //////////////////////////////////////////////////////////////////////////////
        // PIPELINE STAGE 4 -- ACCESS MEMORY
        //

        `if have_asyncio`
        assign asyncio_store_ready = (state != execute && state != mem_access)
          || ( 1
          `if store8_opcode`&& opcode_3 != `store8_opcode``fi
          `if store16_opcode`&& opcode_3 != `store16_opcode``fi
          `if store32_opcode`&& opcode_3 != `store32_opcode``fi
          );
        assign asyncio_will_load = (state != execute && state != mem_access)
          || ( 1
          `if load8_opcode`&& opcode_3 != `load8_opcode``fi
          `if load16_opcode`&& opcode_3 != `load16_opcode``fi
          `if load32_opcode`&& opcode_3 != `load32_opcode``fi
          `if load32_disp_opcode`&& opcode_3 != `load32_disp_opcode``fi
          );
        `fi

        always @(posedge clk)
        begin
          `if have_asyncio`
          asyncio_load_ready <= 0;
          `fi
          writeback_enable_2 <= writeback_enable;
          result_2 <= result;
          address_z_4 <= address_z_3;
          if(state == mem_access) begin
            case(opcode_3)
              //nop
              0: begin end

              `ex2_code`

            endcase
          end

          if(0) begin
          end
          `if load8_opcode`
          //load8
          else if(state == mem_access && opcode_3 == `load8_opcode`) begin
            if(signed_load) begin
              case(load_address[1:0])
                  2'd0: result_2 <= {
                      {24{memory_0[load_address >> 2][7]}}, 
                      memory_0[load_address >> 2]};
                  2'd1: result_2 <= {
                      {24{memory_1[load_address >> 2][7]}}, 
                      memory_1[load_address >> 2]};
                  2'd2: result_2 <= {
                      {24{memory_2[load_address >> 2][7]}}, 
                      memory_2[load_address >> 2]};
                  2'd3: result_2 <= {
                      {24{memory_3[load_address >> 2][7]}}, 
                      memory_3[load_address >> 2]};
              endcase
            end else begin
              case(load_address[1:0])
                  2'd0: result_2 <= {24'd0, memory_0[load_address >> 2]};
                  2'd1: result_2 <= {24'd0, memory_1[load_address >> 2]};
                  2'd2: result_2 <= {24'd0, memory_2[load_address >> 2]};
                  2'd3: result_2 <= {24'd0, memory_3[load_address >> 2]};
              endcase
            end
          end
          `fi
          `if load16_opcode`
          //load16
          else if(state == mem_access && opcode_3 == `load16_opcode`) begin
            if(signed_load) begin
              case(load_address[1])
                  1'd0: result_2 <= {
                      {16{memory_0[load_address >> 2][7]}},
                      memory_0[load_address >> 2],
                      memory_1[load_address >> 2]};
                  1'd1: result_2 <= {
                      {16{memory_2[load_address >> 2][7]}},
                      memory_2[load_address >> 2],
                      memory_3[load_address >> 2]};
              endcase
            end else begin
              case(load_address[1])
                  1'd0: result_2 <= {16'd0,
                      memory_0[load_address >> 2],
                      memory_1[load_address >> 2]};
                  1'd1: result_2 <= {16'd0,
                      memory_2[load_address >> 2],
                      memory_3[load_address >> 2]};
              endcase
            end
          end
          `fi
          `if load32_opcode`
          //load
          else if(state == mem_access && opcode_3 == `load32_opcode`) begin
            result_2 <= {
              memory_0[load_address >> 2],
              memory_1[load_address >> 2],
              memory_2[load_address >> 2],
              memory_3[load_address >> 2]
            };
          end
          `fi
          `if load32_disp_opcode`
          //load_disp
          else if(state == mem_access && opcode_3 == `load32_disp_opcode`) begin
            result_2 <= {
              memory_0[load_address >> 2],
              memory_1[load_address >> 2],
              memory_2[load_address >> 2],
              memory_3[load_address >> 2]
            };
          end
          `fi

          `if have_asyncio`
          else if(asyncio_writing) begin
            if(asyncio_load_enable) begin
              asyncio_loaded_word <= {
                memory_0[asyncio_address_out_write >> 2],
                memory_1[asyncio_address_out_write >> 2],
                memory_2[asyncio_address_out_write >> 2],
                memory_3[asyncio_address_out_write >> 2]
              };
              asyncio_load_ready <= 1;
            end
          end
          `fi

          if(0) begin
          end
          `if store32_opcode`
          else if(state == mem_access && store_enable_32) begin
            memory_0[store_address >> 2] <= store_data[31:24];
            memory_1[store_address >> 2] <= store_data[23:16];
            memory_2[store_address >> 2] <= store_data[15:8];
            memory_3[store_address >> 2] <= store_data[7:0];
          end
          `fi

          `if store8_opcode`
          else if(state == mem_access && store_enable_8) begin
            case(store_address[1:0])
              2'd0: memory_0[store_address >> 2] <= store_data[7:0];
              2'd1: memory_1[store_address >> 2] <= store_data[7:0];
              2'd2: memory_2[store_address >> 2] <= store_data[7:0];
              2'd3: memory_3[store_address >> 2] <= store_data[7:0];
            endcase
          end
          `fi

          `if store16_opcode`
          else if(state == mem_access && store_enable_16) begin
            case(store_address[1])
              1'd0: begin
                memory_0[store_address >> 2] <= store_data[15:8];
                memory_1[store_address >> 2] <= store_data[7:0];
              end
              1'd1: begin
                memory_2[store_address >> 2] <= store_data[15:8];
                memory_3[store_address >> 2] <= store_data[7:0];
              end
            endcase
          end
          `fi

          `if have_asyncio`
          else if(asyncio_reading) begin
            if(asyncio_store_enable && asyncio_store_ready) begin
              memory_0[asyncio_address_out_read >> 2] <= asyncio_word_to_store[31:24];
              memory_1[asyncio_address_out_read >> 2] <= asyncio_word_to_store[23:16];
              memory_2[asyncio_address_out_read >> 2] <= asyncio_word_to_store[15:8];
              memory_3[asyncio_address_out_read >> 2] <= asyncio_word_to_store[7:0];
            end
          end
          `fi

        end

        """, indent=2, store32_opcode=store32_opcode,
        store8_opcode=store8_opcode, load32_opcode=load32_opcode,
        load32_disp_opcode=load32_disp_opcode,
        load8_opcode=load8_opcode,
        load16_opcode=load16_opcode, store16_opcode=store16_opcode,
        have_asyncio=len(asyncio_ports) > 0,
        ex2_code=ex2_code)

#    CodeWriter(output_file).write(r"""
#        end
#    """, indent=2)

    CodeWriter(output_file).write(r"""

        //////////////////////////////////////////////////////////////////////////////
        // PIPELINE STAGE 5 -- WRITEBACK
        //
        always @(posedge clk)
        begin
            if (writeback_enable_2) begin
                registers[address_z_4] <= result_2;
            end
        end
    """, indent=2)

    if len(asyncio_ports) > 0:
        for number, name in enumerate(inputs):
            CodeWriter(output_file).write(r"""

            assign input_`name`_ack =
                asyncio_reading && asyncio_read_id == `number` ?
                    asyncio_ready[`number`] : s_input_`name`_ack;
            """, indent=2, number=number, name=name)
        for number, name in enumerate(outputs):
            CodeWriter(output_file).write(r"""

            assign output_`name`_stb =
            asyncio_writing && asyncio_write_id == `number` ?
                asyncio_valid[`number`] : s_output_`name`_stb;

            assign output_`name` =
              asyncio_writing && asyncio_write_id == `number` ?
                asyncio_data[(`number` + 1) * 32 - 1:`number` * 32] : s_output_`name`;
            """, indent=2, number=number, name=name)

#            output_file.write("  assign output_%s = s_output_%s;\n" % (i, i))
    else:
        for i in inputs:
            output_file.write("  assign input_%s_ack = s_input_%s_ack;\n" % (i, i))
        for i in outputs:
            output_file.write(
                "  assign output_%s_stb = s_output_%s_stb;\n" %
                (i, i))
            output_file.write("  assign output_%s = s_output_%s;\n" % (i, i))

    output_file.write("\nendmodule\n")

    return inputs, outputs
