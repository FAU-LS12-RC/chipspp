"""Generate a python model equivalent to the generated verilog"""

__author__ = "Jon Dawson"
__copyright__ = "Copyright (C) 2013, Jonathan P Dawson"
__version__ = "0.1"

import chips_c
import sys
import math
import register_map
from chips.compiler.exceptions import StopSim, BreakSim, ChipsAssertionFail
from chips.compiler.exceptions import NoProfile
from utils import calculate_jumps
from chips_c import bits_to_float, float_to_bits, bits_to_double, double_to_bits, add, subtract
from chips_c import greater, greater_equal, unsigned_greater, unsigned_greater_equal
from chips_c import shift_left, shift_right, unsigned_shift_right
from chips.compiler import scaled_byte_addr

def to_32_signed(a):
    if a & 0x80000000:
        return a | (~0xffffffff)
    return a

def to_64_signed(a):
    if a & 0x8000000000000000:
        return a | (~0xffffffffffffffff)
    return a

def interpret_twos_complement(value, no_of_bits):
    value &= (1 << no_of_bits) - 1
    if value & (1 << (no_of_bits - 1)) != 0:
        return value | ~((1 << no_of_bits) - 1)
    else:
        return value

class Memory(object):
    def __init__(self, size_in_bytes, initial_content=None):
        self.size_in_bytes = size_in_bytes
        if initial_content is None:
          self.dict = dict()
        else:
          self.dict = dict(initial_content)

    def __getitem__(self, address):
        if address < 0 or address >= scaled_byte_addr(self.size_in_bytes):
            raise Exception("trying to access too large a memory address",
                    address)
#        if address not in self.dict:
#            raise Exception("trying to access uninitialized memory")
        return self.dict.get(address, 0)
    def __setitem__(self, address, value):
        if address < 0 or address >= scaled_byte_addr(self.size_in_bytes):
            raise Exception("trying to access too large a memory address",
                    address)
        self.dict[address] = value

    def get(self, address, default):
        return self.dict.get(address, default)

    def load(self, operand_a):
        if scaled_byte_addr(4) == 4:
            if operand_a % 4 != 0:
                raise Exception("unaligned load", operand_a)
            result = self[operand_a]
            result = (result << 8) | self[operand_a + 1]
            result = (result << 8) | self[operand_a + 2]
            result = (result << 8) | self[operand_a + 3]
        else:
            result = self[operand_a]
        return result

    def store(self, operand_a, operand_b):
        if scaled_byte_addr(4) == 4:
            if operand_a % 4 != 0:
                raise Exception("unaligned store", operand_a)
            self[operand_a] = (operand_b >> 24) & 0xff
            self[operand_a + 1] = (operand_b >> 16) & 0xff
            self[operand_a + 2] = (operand_b >> 8) & 0xff
            self[operand_a + 3] = operand_b & 0xff
        else:
            self[operand_a] = operand_b

    def store_8(self, dest_addr, value):
        if scaled_byte_addr(4) != 4:
            raise Exception("cannot perform byte access")
        self[dest_addr] = value & 0xff

    def store_16(self, dest_addr, value):
        if scaled_byte_addr(4) != 4:
            raise Exception("cannot perform byte access")
        if dest_addr % 2 != 0:
            raise Exception("unaligned store16", operand_a)
        self[dest_addr] = (value >> 8) & 0xff
        self[dest_addr + 1] = value & 0xff

    def load_8(self, src_addr, is_signed):
        if scaled_byte_addr(4) != 4:
            raise Exception("cannot perform byte access")
        ret = self[src_addr]
        if is_signed and ret & 0x80 != 0:
            return ret | 0xffffff00
        return ret

    def load_16(self, operand_a, is_signed):
        if scaled_byte_addr(4) != 4:
            raise Exception("cannot perform byte access")
        if operand_a % 2 != 0:
            raise Exception("unaligned load16", operand_a)
        ret = (self[operand_a] << 8) | self[operand_a + 1]
        if is_signed and ret & 0x8000 != 0:
            return ret | 0xffff0000
        return ret

    def __str__(self):
        return str(self.dict)

def generate_python_model(
        debug,
        program,
        inputs,
        outputs,
        profile=False
):

    instructions = calculate_jumps([dict(instr.items()) for instr in
        program.instructions])

    input_files = set(
        [i["file_name"] for i in instructions if "file_read" == i["op"]]
    )

    output_files = set(
        [i["file_name"] for i in instructions if
         i["op"].endswith("file_write")]
    )

    # map input numbers to port models
    numbered_inputs = {}
    for number, input_name in program.id_to_input.items():
        if input_name in inputs:
            numbered_inputs[number] = inputs[input_name]
    numbered_outputs = {}
    for number, output_name in program.id_to_output.items():
        if output_name in outputs:
            numbered_outputs[number] = outputs[output_name]

    return PythonModel(
        debug,
        instructions,
        dict(),
        program.memory_size_in_bytes,
        input_files,
        output_files,
        numbered_inputs,
        numbered_outputs,
        profile,
    )


class PythonModel:

    """create a python model equivalent to the generated verilog"""

    def __init__(
            self,
            debug,
            instructions,
            memory_content,
            memory_size_in_bytes,
            input_files,
            output_files,
            inputs, outputs,
            profile=False
    ):
        self.debug = debug
        self.profile = profile
        self.instructions = instructions
        self.memory_content = memory_content
        self.memory_size_in_bytes = memory_size_in_bytes

        self.input_file_names = input_files
        self.output_file_names = output_files
        self.inputs = inputs
        self.outputs = outputs

        self.breakpoints = {}

    def simulation_reset(self):
        """reset the python model"""

        self.program_counter = 0
        self.register_hi = 0
        self.register_hib = 0
        self.carry = 0
        self.memory = Memory(self.memory_size_in_bytes, self.memory_content)
        self.registers = {}
        self.address = 0
        self.write_state = "wait_ack"
        self.read_state = "wait_stb"
        self.a_lo = 0
        self.b_lo = 0
        self.a_hi = 0
        self.b_hi = 0
        self.max_stack = 0
        self.timer = 0
        self.clock = 0

        self.files = {}

        self.input_files = {}
        for file_name in self.input_file_names:
            file_ = open(file_name)
            self.input_files[file_name] = file_

        self.output_files = {}
        for file_name in self.output_file_names:
            file_ = open(file_name, "w")
            self.output_files[file_name] = file_

    def get_line(self):
        trace = self.instructions[self.program_counter]["trace"]
        return trace.lineno

    def get_file(self):
        trace = self.instructions[self.program_counter]["trace"]
        return trace.filename

    def get_profile(self):
        if not self.profile:
            raise NoProfile
        return self.files

    def get_registers(self):
        return self.registers

    def get_memory(self):
        return self.memory

    def get_instruction(self):
        return self.instructions[self.program_counter]

    def get_program_counter(self):
        return self.program_counter

    def set_breakpoint(self, f, l):
        lines = self.breakpoints.get(f, {})
        lines[l] = True
        self.breakpoints[f] = lines

    def clear_breakpoint(self, f, l):
        lines = self.breakpoints.get(f, {})
        lines.pop(l)
        self.breakpoints[f] = lines

    def step_into(self):
        """run until a different line (e.g jump into functions)"""

        l = self.get_line()
        f = self.get_file()
        while(l == self.get_line() and f == self.get_file()):
            self.simulation_step()

    def step_over(self):
        """run until the next line (e.g. skip over functions)"""

        l = self.get_line()
        f = self.get_file()
        while(self.get_line() <= l and self.get_file != f):
            self.simulation_step()

    def simulation_step(self):
        """execute the python simulation by one step"""

        l = self.get_line()
        f = self.get_file()
        if f in self.breakpoints:
            if l in self.breakpoints[f]:
                raise BreakSim

        instruction = self.instructions[self.program_counter]
        current_stack = self.registers.get(register_map.tos, 0)
        self.max_stack = max([current_stack, self.max_stack])

        if self.profile:
            trace = instruction.get("trace", "-")
            lines = self.files.get(trace.filename, {})
            lines[trace.lineno] = lines.get(trace.lineno, 0) + 1
            self.files[trace.filename] = lines

        literal = 0
        if "literal" in instruction:
            literal = instruction["literal"]

        if "label" in instruction:
            literal = instruction["label"]

        # read operands
        #
        a = instruction.get("a", 0)
        b = instruction.get("b", 0)
        z = instruction.get("z", 0)

        operand_b = self.registers.get(b, 0)
        operand_a = self.registers.get(a, 0)
        operand_z = self.registers.get(z, 0)

        this_instruction = self.program_counter
        self.program_counter += 1
        wait = False
        result = None

        unsigned_literal = literal
        #literal = interpret_twos_complement(unsigned_literal, 16)
        signed_literal = interpret_twos_complement(unsigned_literal, 16)
        if instruction["op"] == "addl":
            literal = literal & 0xffff
#        if literal < 0:
#            print(instruction["op"])

        if instruction["op"] == "stop":
            self.program_counter = this_instruction
            wait = True
            for file_ in self.input_files.values():
                file_.close()
            for file_ in self.output_files.values():
                file_.close()
            raise StopSim

        elif instruction["op"] == "literal":
            if literal & 0x8000:
                result = -65536 | literal
            else:
                result = literal
            result &= 0xffffffff
        elif instruction["op"] == "addl":
            if literal & 0x8000:
                sext = -65536 | literal
            else:
                sext = literal
            result = sext + operand_a
            result &= 0xffffffff
        elif instruction["op"] == "literal_hi":
            if literal & 0x8000:
                sext = -65536 | literal
            else:
                sext = literal
            result = (sext << 16) | (operand_a & 0x0000ffff)
            result &= 0xffffffff
        elif instruction["op"] == "store":
            offset = interpret_twos_complement(literal, 14)
            self.memory.store(operand_z + offset, operand_a)
        elif instruction["op"] == "store8":
            offset = interpret_twos_complement(literal, 14)
            self.memory.store_8(operand_z + offset, operand_a)
        elif instruction["op"] == "store16":
            offset = interpret_twos_complement(literal, 14)
            self.memory.store_16(operand_z + offset, operand_a)
        elif instruction["op"] == "load":
            result = self.memory.load(operand_a)
        elif instruction["op"] == "load_disp":
            offset = interpret_twos_complement(literal, 13)
            result = self.memory.load(operand_a + offset)
        elif instruction["op"] == "load8":
            offset = interpret_twos_complement(literal, 13)
            result = self.memory.load_8(operand_a + offset,
                    literal & 0x8000 != 0)
        elif instruction["op"] == "load16":
            offset = interpret_twos_complement(literal, 13)
            result = self.memory.load_16(operand_a + offset,
                    literal & 0x8000 != 0)
        elif instruction["op"] == "call":
            result = this_instruction + 1
            self.program_counter = literal
        elif instruction["op"] == "return":
            self.program_counter = operand_a
        elif instruction["op"] == "a_lo":
            result = self.a_lo
            self.a_lo = operand_a
        elif instruction["op"] == "b_lo":
            result = self.b_lo
            self.b_lo = operand_a
        elif instruction["op"] == "a_hi":
            result = self.a_hi
            self.a_hi = operand_a
        elif instruction["op"] == "b_hi":
            result = self.b_hi
            self.b_hi = operand_a
        elif instruction["op"] == "a_lo_in":
            self.a_lo = operand_a
        elif instruction["op"] == "b_lo_in":
            self.b_lo = operand_a
        elif instruction["op"] == "a_hi_in":
            self.a_hi = operand_a
        elif instruction["op"] == "b_hi_in":
            self.b_hi = operand_a
        elif instruction["op"] == "a_lo_out":
            result = self.a_lo
        elif instruction["op"] == "b_lo_out":
            result = self.b_lo
        elif instruction["op"] == "a_hi_out":
            result = self.a_hi
        elif instruction["op"] == "b_hi_out":
            result = self.b_hi
        elif instruction["op"] == "not":
            result = (~operand_a) & 0xffffffff
        elif instruction["op"] == "int_to_long":
            if operand_a & 0x80000000:
                result = 0xffffffff
            else:
                result = 0
        elif instruction["op"] == "int_to_float":
            f = float(to_32_signed(self.a_lo))
            self.a_lo = float_to_bits(f)
        elif instruction["op"] == "float_to_int":
            i = bits_to_float(self.a_lo)
            if math.isnan(i):
                self.a_lo = 0
            else:
                self.a_lo = int(i) & 0xffffffff
        elif instruction["op"] == "long_to_double":
            double = float(to_64_signed(chips_c.join_words(self.a_hi, self.a_lo)))
            if math.isnan(double):
                self.a_hi = 0
                self.a_lo = 0
            else:
                self.a_hi = chips_c.high_word(double_to_bits(double))
                self.a_lo = chips_c.low_word(double_to_bits(double))
        elif instruction["op"] == "double_to_long":
            bits = int(bits_to_double(chips_c.join_words(self.a_hi, self.a_lo)))
            bits &= 0xffffffffffffffff
            self.a_hi = chips_c.high_word(bits)
            self.a_lo = chips_c.low_word(bits)
        elif instruction["op"] == "float_to_double":
            f = bits_to_float(self.a_lo)
            bits = double_to_bits(f)
            self.a_hi = chips_c.high_word(bits)
            self.a_lo = chips_c.low_word(bits)
        elif instruction["op"] == "double_to_float":
            f = bits_to_double(chips_c.join_words(self.a_hi, self.a_lo))
            self.a_lo = float_to_bits(f)
        elif instruction["op"] == "add":
            total = add(operand_a, operand_b, 0);
            result = total.lo
            self.carry = total.hi
        elif instruction["op"] == "add_with_carry":
            total = add(operand_a, operand_b, self.carry);
            result = total.lo
            self.carry = total.hi
        elif instruction["op"] == "subtract":
            total = subtract(operand_a, operand_b, 1);
            result = total.lo
            self.carry = total.hi
        elif instruction["op"] == "subtract_with_carry":
            total = subtract(operand_a, operand_b, self.carry);
            result = total.lo
            self.carry = total.hi
        elif instruction["op"] == "multiply":
            lw = operand_a * operand_b
            self.carry = chips_c.high_word(lw)
            result = chips_c.low_word(lw)
        elif instruction["op"] == "divide":
            a = operand_a
            b = operand_b
            result = chips_c.divide(a, b)
        elif instruction["op"] == "unsigned_divide":
            a = operand_a
            b = operand_b
            result = chips_c.unsigned_divide(a, b)
        elif instruction["op"] == "modulo":
            a = operand_a
            b = operand_b
            result = chips_c.modulo(a, b)
        elif instruction["op"] == "unsigned_modulo":
            a = operand_a
            b = operand_b
            result = chips_c.unsigned_modulo(a, b)
        elif instruction["op"] == "long_divide":
            a = chips_c.join_words(self.a_hi, self.a_lo)
            b = chips_c.join_words(self.b_hi, self.b_lo)
            quotient = chips_c.long_divide(a, b)
            self.a_hi = chips_c.high_word(quotient)
            self.a_lo = chips_c.low_word(quotient)
        elif instruction["op"] == "long_modulo":
            a = chips_c.join_words(self.a_hi, self.a_lo)
            b = chips_c.join_words(self.b_hi, self.b_lo)
            remainder = chips_c.long_modulo(a, b)
            self.a_hi = chips_c.high_word(remainder)
            self.a_lo = chips_c.low_word(remainder)
        elif instruction["op"] == "unsigned_long_divide":
            a = chips_c.join_words(self.a_hi, self.a_lo)
            b = chips_c.join_words(self.b_hi, self.b_lo)
            quotient = chips_c.unsigned_long_divide(a, b)
            self.a_hi = chips_c.high_word(quotient)
            self.a_lo = chips_c.low_word(quotient)
        elif instruction["op"] == "unsigned_long_modulo":
            a = chips_c.join_words(self.a_hi, self.a_lo)
            b = chips_c.join_words(self.b_hi, self.b_lo)
            remainder = chips_c.unsigned_long_modulo(a, b)
            self.a_hi = chips_c.high_word(remainder)
            self.a_lo = chips_c.low_word(remainder)
        elif instruction["op"] == "carry":
            result = self.carry
        elif instruction["op"] == "or":
            result = operand_a | operand_b
        elif instruction["op"] == "and":
            result = operand_a & operand_b
        elif instruction["op"] == "xor":
            result = operand_a ^ operand_b
        elif instruction["op"] == "shift_left":
            total = shift_left(operand_a, operand_b, 0)
            result = total.lo
            self.carry = total.hi
        elif instruction["op"] == "shift_left_with_carry":
            total = shift_left(operand_a, operand_b, self.carry)
            result = total.lo
            self.carry = total.hi
        elif instruction["op"] == "shift_right":
            total = shift_right(operand_a, operand_b)
            result = total.lo
            self.carry = total.hi
        elif instruction["op"] == "unsigned_shift_right":
            total = unsigned_shift_right(operand_a, operand_b, 0)
            result = total.lo
            self.carry = total.hi
        elif instruction["op"] == "shift_right_with_carry":
            total = unsigned_shift_right(operand_a, operand_b, self.carry)
            result = total.lo
            self.carry = total.hi
        elif instruction["op"] == "greater":
            result = greater(operand_a, operand_b)
        elif instruction["op"] == "greater_equal":
            result = greater_equal(operand_a, operand_b)
        elif instruction["op"] == "unsigned_greater":
            result = unsigned_greater(operand_a, operand_b)
        elif instruction["op"] == "unsigned_greater_equal":
            result = unsigned_greater_equal(operand_a, operand_b)
        elif instruction["op"] == "equal":
            result = operand_a == operand_b
        elif instruction["op"] == "not_equal":
            result = operand_a != operand_b
        elif instruction["op"] == "jmp_if_false":
            if operand_a == 0:
                self.program_counter = literal
        elif instruction["op"] == "jmp_if_true":
            if operand_a != 0:
                self.program_counter = literal
        elif instruction["op"] == "goto":
            self.program_counter = literal
        elif instruction["op"] == "timer_low":
            result = self.clock&0xffffffff
        elif instruction["op"] == "timer_high":
            result = self.clock>>32
        elif instruction["op"] == "file_read":
            value = self.input_files[instruction["filename"]].getline()
            result = value
        elif instruction["op"] == "float_file_write":
            self.output_files[instruction["file_name"]].write(
                "%.7f\n" %
                bits_to_float(operand_a))
        elif instruction["op"] == "unsigned_file_write":
            self.output_files[instruction["file_name"]].write(
                "%i\n" %
                operand_a)
        elif instruction["op"] == "file_write":
            self.output_files[instruction["file_name"]].write(
                "%i\n" %
                to_32_signed(operand_a))
        elif instruction["op"] == "read":
            if operand_a not in self.inputs:
                result = 0
            else:
                input_ = self.inputs[operand_a]
                if input_.src_rdy and input_.dst_rdy:
                    result = input_.q
                    input_.next_dst_rdy = False
                else:
                    input_.next_dst_rdy = True
                    wait = True
        elif instruction["op"] == "ready":
            if operand_a not in self.inputs:
                operand_a = 0
            else:
                input_ = self.inputs[operand_a]
                if input_.src_rdy:
                    result = 1
                else:
                    result = 0
        elif instruction["op"] == "output_ready":
            if operand_a not in self.outputs:
                operand_a = 0
            else:
                output_ = self.outputs[operand_a]
                if output_.dst_rdy:
                    result = 1
                else:
                    result = 0
        elif instruction["op"] == "write":
            if operand_a not in self.outputs:
                pass
            else:
                output_ = self.outputs[operand_a]
                if output_.src_rdy and output_.dst_rdy:
                    output_.next_src_rdy = False
                else:
                    output_.q = operand_b
                    output_.next_src_rdy = True
                    wait = True
        elif instruction["op"] == "float_add":
            a = operand_a
            b = operand_b
            float_ = bits_to_float(a)
            floatb = bits_to_float(b)
            result = float_to_bits(float_ + floatb)
        elif instruction["op"] == "float_subtract":
            a = operand_a
            b = operand_b
            float_ = bits_to_float(a)
            floatb = bits_to_float(b)
            result = float_to_bits(float_ - floatb)
        elif instruction["op"] == "float_multiply":
            a = operand_a
            b = operand_b
            float_ = bits_to_float(a)
            floatb = bits_to_float(b)
            result = float_to_bits(float_ * floatb)
        elif instruction["op"] == "float_divide":
            a = operand_a
            b = operand_b
            float_ = bits_to_float(a)
            floatb = bits_to_float(b)
            try:
                result = float_to_bits(float_ / floatb)
            except ZeroDivisionError:
                result = float_to_bits(float("nan"))
        elif instruction["op"] == "long_float_add":
            double = bits_to_double(chips_c.join_words(self.a_hi, self.a_lo))
            doubleb = bits_to_double(chips_c.join_words(self.b_hi, self.b_lo))
            self.a_hi = chips_c.high_word(double_to_bits(double + doubleb))
            self.a_lo = chips_c.low_word(double_to_bits(double + doubleb))
        elif instruction["op"] == "long_float_subtract":
            double = bits_to_double(chips_c.join_words(self.a_hi, self.a_lo))
            doubleb = bits_to_double(chips_c.join_words(self.b_hi, self.b_lo))
            self.a_hi = chips_c.high_word(double_to_bits(double - doubleb))
            self.a_lo = chips_c.low_word(double_to_bits(double - doubleb))
        elif instruction["op"] == "long_float_multiply":
            double = bits_to_double(chips_c.join_words(self.a_hi, self.a_lo))
            doubleb = bits_to_double(chips_c.join_words(self.b_hi, self.b_lo))
            self.a_hi = chips_c.high_word(double_to_bits(double * doubleb))
            self.a_lo = chips_c.low_word(double_to_bits(double * doubleb))
        elif instruction["op"] == "long_float_divide":
            double = bits_to_double(chips_c.join_words(self.a_hi, self.a_lo))
            doubleb = bits_to_double(chips_c.join_words(self.b_hi, self.b_lo))
            try:
                self.a_hi = chips_c.high_word(double_to_bits(double / doubleb))
                self.a_lo = chips_c.low_word(double_to_bits(double / doubleb))
            except ZeroDivisionError:
                self.a_hi = chips_c.high_word(double_to_bits(float("nan")))
                self.a_lo = chips_c.low_word(double_to_bits(float("nan")))
        elif instruction["op"] == "long_float_file_write":
            long_word = chips_c.join_words(self.a_hi, self.a_lo)
            self.output_files[instruction["file_name"]].write(
                "%.16f\n" %
                bits_to_double(long_word))
        elif instruction["op"] == "long_file_write":
            long_word = chips_c.join_words(self.a_hi, self.a_lo)
            self.output_files[instruction["file_name"]].write(
                "%f\n" %
                long_word)
        elif instruction["op"] == "assert":
            if operand_a == 0:
                raise ChipsAssertionFail(
                    instruction["file"],
                    instruction["line"])
        elif instruction["op"] == "report":
            print "%d (report (int) at line: %s in file: %s)" % (
                to_32_signed(self.a_lo),
                instruction["line"],
                instruction["file"],
            )
        elif instruction["op"] == "long_report":
            print "%d (report (long) at line: %s in file: %s)" % (
                to_64_signed(chips_c.join_words(self.a_hi, self.a_lo)),
                instruction["line"],
                instruction["file"],
            )
        elif instruction["op"] == "float_report":
            print "%f (report (float) at line: %s in file: %s)" % (
                bits_to_float(self.a_lo),
                instruction["line"],
                instruction["file"],
            )
        elif instruction["op"] == "long_float_report":
            print "%s (report (double) at line: %s in file: %s)" % (

                bits_to_double(chips_c.join_words(self.a_hi, self.a_lo)),
                instruction["line"],
                instruction["file"],
            )
        elif instruction["op"] == "unsigned_report":
            print "%d (report (unsigned) at line: %s in file: %s)" % (
                self.a_lo,
                instruction["line"],
                instruction["file"],
            )
        elif instruction["op"] == "long_unsigned_report":
            print "%d (report (unsigned long) at line: %s in file: %s)" % (
                chips_c.join_words(self.a_hi, self.a_lo),
                instruction["line"],
                instruction["file"],
            )
        elif instruction["op"] == "wait_clocks":
            if self.timer == operand_a:
                wait = False
                self.timer = 0
            else:
                wait = True
                self.timer += 1

        else:
            print "Unknown machine instruction", instruction["op"]
            sys.exit(-1)

        # Write data back
        if result is not None:
            self.registers[z] = result

        # manipulate stack pointer
        if wait:
            self.program_counter = this_instruction

        self.clock += 1
