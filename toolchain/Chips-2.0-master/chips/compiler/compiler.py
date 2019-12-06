#!/usr/bin/env python
"""A C to Verilog compiler"""

__author__ = "Jon Dawson"
__copyright__ = "Copyright (C) 2013, Jonathan P Dawson"
__version__ = "0.1"

import sys
import os
from contextlib import contextmanager

from chips.compiler.parser import Parser
from chips.compiler.exceptions import C2CHIPError
from chips.compiler.macro_expander import expand_macros
from chips.compiler.verilog_area import generate_CHIP as generate_CHIP_area
from chips.compiler.python_model import generate_python_model
from chips.compiler.assembler import assemble, disassemble
from allocator import Allocator
import fpu


def generate_library():
    output_file = open("chips_lib.v", "w")
    output_file.write(fpu.adder)
    output_file.write(fpu.divider)
    output_file.write(fpu.multiplier)
    output_file.write(fpu.double_divider)
    output_file.write(fpu.double_multiplier)
    output_file.write(fpu.double_adder)
    output_file.write(fpu.int_to_float)
    output_file.write(fpu.float_to_int)
    output_file.write(fpu.long_to_double)
    output_file.write(fpu.double_to_long)
    output_file.write(fpu.float_to_double)
    output_file.write(fpu.double_to_float)
    #TODO /compiler/: open asyncio.v and include
    with open(os.path.join(os.path.dirname(__file__), "asyncio.v"), "r") as \
      asyncio_file:
        output_file.write(asyncio_file.read())

    output_file.close()


#def print_instruction_mnemonic(out, index, instr):
#  op = instr.get("op", "-")
#  if op == "literal":
#      op = "movl"
#  if op == "literal_hi":
#      op = "movl2"
#  op_z = "r{}".format(instr.get("z", "-"))
#  op_a = "r{}".format(instr.get("a", "-"))
#  op_b = "r{}".format(instr.get("b", "-"))
#  literal = instr.get("literal", "-")
#
#  if op == "label":
#      out.write("{}: \n".format(instr["label"]))
#      return
#
#  operands = (
#          [op_z, literal] if op == "movl" else
#          [op_z, op_a, literal] if op == "addl" else
#          ["[" + op_a + "]", op_b] if op == "store" else
#          [op_z, "[" + op_a + "]"] if op == "load" else
#          [op_z, instr["label"]] if op == "call" else
#          [op_a] if op == "return" else
#          [instr["label"]] if op == "goto" else
#          [] if op == "stop" else
#          [op_z, "*" + op_a] if op == "read" else
#          ["*" + op_a, op_b] if op == "write" else
#          [op_a, instr["label"]] if op == "jmp_if_false" else
#          [op_z, op_a] if op == "a_lo" else
#          [op_z, literal, op_a] if op == "movl2" else
#          [op_z, op_a, op_b])
#
#  encoded_instruction = (
#          (0 << 24) |
#          (instr.get("z", 0) << 20) |
#          (instr.get("a", 0) << 16) |
#          instr.get("b", 0) |
#          (instr.get("literal", 0) & 0xffff))
#  encoded_instruction &= 0xfffffff
#
#  #print("{}: {} {}   ;{}".format(
#  out.write("[0x?{:06x}] {}: {} {}\n".format(
#      encoded_instruction,
#      index, op,
#      ", ".join([str(operand) for operand in operands]),
#      instr.get("trace", "")))

def print_instructions(out, instructions):
    i = -1
    for instr in instructions:
        instr_str = disassemble(instr)
        #instr_str = ""
        if instr["op"] == "label":
            out.write("{}\n".format(instr_str))
            continue
        i += 1

        encoded_instruction = (
                (0 << 24) |
                (instr.get("z", 0) << 20) |
                (instr.get("a", 0) << 16) |
                instr.get("b", 0) |
                (instr.get("literal", 0) & 0xffff))
        encoded_instruction &= 0xfffffff
        out.write("[0x?{:06x}] {}: {}\n".format(
            encoded_instruction, i, instr_str))

def get_instructions_from_asm(file_path, allocator, options):
    with open(file_path, "rb") as input:
        instructions = assemble(input, options)
    for io_id, io_name in options["asm_params"]["io_ids"]:
        io_type, name = io_name.split(" ")
        if io_type == "input":
            allocator.new_input(name, handle=io_id)
        if io_type == "output":
            allocator.new_output(name, handle=io_id)
    return instructions

def _compile(chip_name, chip_id, input_file, options, parameters, reuse,
        initialize_memory, sn=0):
    if "is_asm" in options:
        name = chip_name + "_main" + "_%s" % sn
        allocator = Allocator(False)
        asm_program = get_instructions_from_asm(input_file,
                allocator, options)
    else:
        # Optimize for area
        parser = Parser(input_file, reuse, initialize_memory, parameters)
        allocator = parser.allocator
        process = parser.parse_process()
        name = chip_name + "_" + process.main.name + "_%s" % sn
        asm_program = process.generate()
        asm_program.program_instrs = expand_macros(asm_program.instructions,
            parser.allocator)

    if "dump" in options:
        print_instructions(sys.stdout, asm_program.instructions)
    if "dump_to" in options:
        with open(options["dump_to"], "w") as instruction_dump_file:
            print_instructions(instruction_dump_file, asm_program.instructions)

    asm_program.set_stack_size(options.get("stack_size", 300))
    print "%s: used memory size: %s (stack: %s, globals: %s)" % (chip_name,
            asm_program.memory_size_in_bytes,
            asm_program.stack_size_bytes, asm_program.globals_size_bytes)

    asm_program.set_names(chip_id, chip_name, name)
    asm_program.set_io_ports(allocator.input_names, allocator.output_names)

    return asm_program

@contextmanager
def error_handler():
    try:
        yield
    except C2CHIPError as err:
        print "Error in file:", err.filename, "at line:", err.lineno
        print err.message
        #TODO -1 is wrong
        sys.exit(-1)

def generate_hdl(asm_program):
    with error_handler():
        generate_library()
        output_file = asm_program.component_name + ".v"
        with open(output_file, "w") as output_file:
          generate_CHIP_area(
              "<c file>",
              asm_program,
              output_file,
              False,
              asm_program.memory_size_in_bytes)

def compile_to_asm(
    chip_name,
    chip_id,
    input_file,
    options={},
    parameters={},
    debug=False,
    profile=False,
    sn=0):
    reuse = "no_reuse" not in options
    initialize_memory = "no_initialize_memory" not in options
    with error_handler():
        return _compile(chip_name, chip_id, input_file, options, parameters,
                False, initialize_memory, sn)

def make_python_model(asm_program, inputs={}, outputs={},
    debug=False, profile=False):
    with error_handler():
#        debug = debug or ("debug" in options)
#        profile = profile or ("profile" in options)
        model = generate_python_model(
            debug,
            asm_program,
            inputs,
            outputs,
            profile)

        return (
            model,
            asm_program.input_names,
            asm_program.output_names,
        )
