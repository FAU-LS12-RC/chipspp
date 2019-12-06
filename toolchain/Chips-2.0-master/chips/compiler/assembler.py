from __future__ import print_function
from collections import namedtuple
import sys
import re

LoadInstrs = ["load_disp", "load8", "load16"]
StoreInstrs = ["store", "store8", "store16"]

class ChipInterface(namedtuple("ChipInterface",
  ["name", "id", "input_names", "output_names"])):
  @property
  def io_ports(self):
    return self.input_names + self.output_names

class AsmProgram(object):
  def __init__(self, set_up_instrs, program_instrs, globals_size_bytes):
    self.set_up_instrs = set_up_instrs
    self.program_instrs = program_instrs
    self.globals_size_bytes = globals_size_bytes
    self.stack_size_bytes = 0

  @property
  def instructions(self):
    return self.set_up_instrs + self.program_instrs

  def set_stack_size(self, size_in_bytes):
      self.stack_size_bytes = size_in_bytes

  @property
  def memory_size_in_bytes(self):
      return self.globals_size_bytes + self.stack_size_bytes

  def set_names(self, chip_id, chip_name, component_name):
      self.chip_id = chip_id
      self.chip_name = chip_name
      self.component_name = component_name
  def set_io_ports(self, id_to_input, id_to_output):
      self.id_to_input = id_to_input
      self.id_to_output = id_to_output

  @property
  def interface(self):
      return ChipInterface(self.chip_name, self.chip_id,
              self.input_names, self.output_names)

  @property
  def input_names(self):
      return [pair[1] for pair in sorted(list(self.id_to_input.items()),
          key=lambda pair: pair[0])]
  @property
  def output_names(self):
      return [pair[1] for pair in sorted(list(self.id_to_output.items()),
          key=lambda pair: pair[0])]

def scaled_addr(address):
    #return address / 4
    return address

def scaled_offset(offset):
    #return offset / 4
    return offset

def all_zero(data):
    return all(integer.n == 0 for integer in data)


class ParseState(object):
    def __init__(self):
        self.instrs = []
        self.in_data_section = False
        self.global_vars = dict()
        self.current_global_var = None

    def add_global_var(self, var):
        self.global_vars[var.name] = var

LiteralRegex = re.compile(r"^(-)?[0-9]+$")
ArithExprRegex = re.compile(
        r"^(?P<symbol>[.a-zA-Z_][.a-zA-Z_0-9]*)"
        r"((?P<op>[+-])(?P<offset>[0-9]+))?$")

class GlobalAddressExpression(object):
    def __init__(self, string):
        match = ArithExprRegex.match(string)
        self.symbol = match.group("symbol")
        self.op = match.group("op") or "+"
        self.offset = scaled_addr(int(match.group("offset") or "0"))

    def can_compute(self, global_vars):
        return True
    def compute(self, global_vars):
        base = global_vars[self.symbol].offset
#        print("resolving ", self.symbol, "to", base)
        if self.op == "+":
            return base + self.offset
        else:
            return base - self.offset

    def __repr__(self):
        return "{}{}{}".format(self.symbol, self.op, self.offset)

class ConstBitsExpression(object):
    def __init__(self, bitmask, const_bits, sub_expr, signed=True):
        self.bitmask = bitmask
        self.const_bits = const_bits
        self.sub_expr = sub_expr
        self.max_value = None
        if self.bitmask is not None:
            if signed:
                self.max_value = self.bitmask >> 2
            else:
                self.max_value = self.bitmask

    def can_compute(self, global_vars):
        if self.max_value is None:
            return True
        #TODO generalize for negative numbers
        ret = self.sub_expr.compute(global_vars) <= self.max_value
        return ret
    def compute(self, global_vars):
        ret = self.sub_expr.compute(global_vars)
        if self.bitmask is not None:
            ret &= self.bitmask
        ret |= self.const_bits
        return ret

class ConstExpression(object):
    def __init__(self, value):
        self.value = value
    def compute(self, _):
        return self.value
    def __str__(self):
        return str(self.value)

class I32(object):
    def __init__(self, n):
        self.n = n
        self.size_in_bytes = 4
class I8(object):
    def __init__(self, n):
        self.n = n
        self.size_in_bytes = 1

class GlobalVar(object):
    def __init__(self, name):
        self.name = name
        self._data = []
        self.offset = 0

    def append_data(self, data):
#        assert all(isinstance(d, I8) for d in data)
            
        self._data.extend(data)

    @property
    def data(self):
        words, rest_bytes = divmod(self.size_in_bytes, 4)
        for i in range(words):
            yield I32(
                self._data[i * 4 + 0].n << 24 |
                self._data[i * 4 + 1].n << 16 |
                self._data[i * 4 + 2].n << 8 |
                self._data[i * 4 + 3].n)
        for i in range(rest_bytes):
            yield self._data[words * 4 + i]

    def resolve_address_expressions(self, global_vars):
        data = []
        for d in self._data:
            n = d.n
            if hasattr(d.n, "compute"):
                n = d.n.compute(global_vars)
            if d.size_in_bytes == 4:
                data.append(I8((n >> 24) & 0xff))
                data.append(I8((n >> 16) & 0xff))
                data.append(I8((n >> 8) & 0xff))
                data.append(I8((n >> 0) & 0xff))
            else:
                data.append(I8(n))

        self._data = data

    @property
    def size_in_bytes(self):
        ret = 0
        for d in self._data:
            ret += d.size_in_bytes
        return ret

    def __repr__(self):
        return "{}, {} x 8 bits".format(self.name, self.size_in_bytes)

def resolve_directive_address_expressions(state):
    for var in state.global_vars.values():
        var.resolve_address_expressions(state.global_vars)

def parse_operator_and_operands(string):
    words = string.split()
    operator = words[0]
    if len(words) > 1:
        operands = " ".join(words[1:]).split(",")
    else:
        operands = []
    return operator, [operand.strip() for operand in operands]

class OpInfo(object):
    def __init__(self, z, a, b, literal, is_branch=False, operator=None):
        self.z = z
        self.a = a
        self.b = b
        self.literal = literal
        self.is_branch = is_branch
        self.operator = operator
    def parse(self, operands):
        ret = dict()
        for key, parser in zip(
                ["z", "a", "b", "literal"],
                [self.z, self.a, self.b, self.literal]):
            if parser is not Unused:
                ret[key] = parser.parse(operands)
        if self.is_branch:
            ret["label"] = ret["literal"]
            del ret["literal"]
        if self.operator is not None:
            ret["op"] = self.operator
        return ret
    def disassemble(self, operands):
        output_operands = [None] * 4
        for key, parser in zip(
                ["z", "a", "b", "literal"],
                [self.z, self.a, self.b, self.literal]):
            if parser is Unused:
                continue
            if self.is_branch and key == "literal":
                input_op = operands["label"]
            else:
                input_op = operands[key]
            parser.disassemble(output_operands, input_op)
        return ", ".join(str(operand) for operand in output_operands if
                operand is not None)

class Operand(object):
    def __init__(self, num, parser):
        self.num = num
        self.parser = parser
    def parse(self, operands):
        return self.parser.parse(operands[self.num])
    def disassemble(self, output_ops, input):
        output_ops[self.num] = self.parser.disassemble(input)
class Register(object):
    def parse(self, string):
        assert string[0:2] == "%r", string
        return int(string[2:])
    def disassemble(self, input):
        return "%r{}".format(input)
class Literal(object):
    def __init__(self, bitmask=None, const_bits=0):
        self.const_bits = const_bits
        self.bitmask = bitmask
    def parse(self, string):
        if LiteralRegex.match(string):
            ret = ConstExpression(int(string))
        else:
            ret = GlobalAddressExpression(string)
        return ConstBitsExpression(self.bitmask, self.const_bits, ret)
    def disassemble(self, input):
        return str(input & ~self.const_bits)
class Indirect(object):
    def __init__(self, sub_operand):
        self.sub_operand = sub_operand
    def parse(self, string):
        assert string[0] == "[" and string[-1] == "]", string
        return self.sub_operand.parse(string[1:-1])
    def disassemble(self, input):
        return "[{}]".format(self.sub_operand.disassemble(input))
class IoId(object):
    def __init__(self, sub_operand):
        self.sub_operand = sub_operand
    def parse(self, string):
        assert string[0] == "*"
        return self.sub_operand.parse(string[1:])
    def disassemble(self, input):
        return "*{}".format(self.sub_operand.disassemble(input))
class Label(object):
    def parse(self, string):
        return string
    def disassemble(self, input):
        return input
def reg(num):
    return Operand(num, Register())
def literal(num, **kwargs):
    return Operand(num, Literal(**kwargs))
def indirect(num):
    return Operand(num, Indirect(Register()))
def io_id(num):
    return Operand(num, IoId(Register()))
def label(num):
    return Operand(num, Label())
Unused = object()
AluOpInfo = OpInfo(reg(0), reg(1), reg(2), Unused)
#TODO refactor: FpuOpInfo

OpInfos = dict(
        literal=OpInfo(reg(0), Unused, Unused, literal(1)),
        store=OpInfo(indirect(0), reg(1), Unused, literal(2, bitmask=0x3fff)),
        store8=OpInfo(indirect(0), reg(1), Unused, literal(2, bitmask=0x3fff)),
        store16=OpInfo(indirect(0), reg(1), Unused, literal(2, bitmask=0x3fff)),
        load=OpInfo(reg(0), indirect(1), Unused, Unused),
        load_disp=OpInfo(reg(0), indirect(1), Unused, literal(2,
            bitmask=0x1fff)),
        load8=OpInfo(reg(0), indirect(1), Unused, literal(2, bitmask=0x1fff)),
        load8_signed=OpInfo(reg(0), indirect(1), Unused, literal(2,
            bitmask=0x1fff, const_bits=0x8000), operator="load8"),
        load16=OpInfo(reg(0), indirect(1), Unused, literal(2, bitmask=0x1fff)),
        load16_signed=OpInfo(reg(0), indirect(1), Unused, literal(2,
            bitmask=0x1fff, const_bits=0x8000), operator="load16"),
        literal_hi=OpInfo(reg(0), reg(1), Unused, literal(2)),
        carry=OpInfo(reg(0), Unused, Unused, Unused),

        #load_double

        read=OpInfo(reg(0), io_id(1), Unused, Unused),
        write=OpInfo(Unused, io_id(0), reg(1), Unused),

        addl=OpInfo(reg(0), reg(1), Unused, literal(2)),
        subtract=AluOpInfo,
        add=AluOpInfo,
        add_with_carry=AluOpInfo,
        subtract_with_carry=AluOpInfo,
        multiply=AluOpInfo,
        divide=AluOpInfo,
        modulo=AluOpInfo,
        unsigned_modulo=AluOpInfo,
        unsigned_shift_right=AluOpInfo,
        shift_right=AluOpInfo,
        shift_right_with_carry=AluOpInfo,
        shift_left=AluOpInfo,
        shift_left_with_carry=AluOpInfo,

        int_to_long=OpInfo(reg(0), reg(1), Unused, Unused),

        #float
        float_add=AluOpInfo,
        float_subtract=AluOpInfo,
        float_multiply=AluOpInfo,
        float_divide=AluOpInfo,
        a_lo=OpInfo(reg(0), reg(1), Unused, Unused),
        a_hi=OpInfo(reg(0), reg(1), Unused, Unused),
        b_lo=OpInfo(reg(0), reg(1), Unused, Unused),
        b_hi=OpInfo(reg(0), reg(1), Unused, Unused),
        a_lo_in=OpInfo(Unused, reg(0), Unused, Unused),
        a_hi_in=OpInfo(Unused, reg(0), Unused, Unused),
        b_lo_in=OpInfo(Unused, reg(0), Unused, Unused),
        b_hi_in=OpInfo(Unused, reg(0), Unused, Unused),
        a_lo_out=OpInfo(reg(0), Unused, Unused, Unused),
        a_hi_out=OpInfo(reg(0), Unused, Unused, Unused),
        b_lo_out=OpInfo(reg(0), Unused, Unused, Unused),
        b_hi_out=OpInfo(reg(0), Unused, Unused, Unused),
        long_float_add=OpInfo(Unused, Unused, Unused, Unused),
        long_float_subtract=OpInfo(Unused, Unused, Unused, Unused),
        long_float_divide=OpInfo(Unused, Unused, Unused, Unused),
        long_float_multiply=OpInfo(Unused, Unused, Unused, Unused),
        long_to_double=OpInfo(Unused, Unused, Unused, Unused),
        double_to_long=OpInfo(Unused, Unused, Unused, Unused),
        double_to_float=OpInfo(Unused, Unused, Unused, Unused),
        float_to_double=OpInfo(Unused, Unused, Unused, Unused),
        int_to_float=OpInfo(Unused, Unused, Unused, Unused),
        float_to_int=OpInfo(Unused, Unused, Unused, Unused),

        greater=AluOpInfo,
        greater_equal=AluOpInfo,
        unsigned_greater=AluOpInfo,
        unsigned_greater_equal=AluOpInfo,
        equal=AluOpInfo,
        not_equal=AluOpInfo,
        xor=AluOpInfo,

        call=OpInfo(reg(0), Unused, Unused, label(1), is_branch=True),
        goto=OpInfo(Unused, Unused, Unused, label(0), is_branch=True),
        stop=OpInfo(Unused, Unused, Unused, Unused),
        jmp_if_false=OpInfo(Unused, reg(0), Unused, label(1), is_branch=True),
        jmp_if_true=OpInfo(Unused, reg(0), Unused, label(1), is_branch=True),

        report=OpInfo(Unused, Unused, Unused, Unused),
        unsigned_report=OpInfo(Unused, Unused, Unused, Unused),
        long_report=OpInfo(Unused, Unused, Unused, Unused),
        timer_low=OpInfo(reg(0), Unused, Unused, Unused),
        timer_high=OpInfo(reg(0), Unused, Unused, Unused),
        wait_clocks=OpInfo(reg(0), Unused, Unused, Unused),

        asyncio_start_read=OpInfo(Unused, reg(0), reg(1), Unused),
        asyncio_start_write=OpInfo(Unused, reg(0), reg(1), Unused),
        asyncio_wait_read=OpInfo(Unused, Unused, Unused, Unused),
        asyncio_wait_write=OpInfo(Unused, Unused, Unused, Unused)
)
OpInfos["or"] = AluOpInfo
OpInfos["and"] = AluOpInfo
OpInfos["return"] = OpInfo(Unused, reg(0), Unused, Unused)

class PseudoInstrInfo(object):
    def __init__(self, *parsers):
        self.parsers = parsers

    def parse(self, operands):
        return [parser.parse(operand) for operand, parser in
                zip(operands, self.parsers)]

PseudoInfos = dict(
        select=PseudoInstrInfo(Register(), Register(), Register(), Register())
)

def get_operands_in_correct_positions(op, operands):
    return OpInfos[op].parse(operands)

def parse_instrs(*strings):
    return [parse_instr(string) for string in strings]

#TODO initialize with dwarf info
class Trace:
    def __init__(self):
        self.lineno = 0
        self.filename = ""
        self.function = ""
        self.global_scope = ""
        self.statement = ""

    def __repr__(self):
        return "%s : %s" % (self.filename, self.lineno)

class PseudoInstr(object):
    def __init__(self, operator, operands):
        self.operator = operator
        self.operands = operands

    def items(self):
        return []
    def __getitem__(self, key):
        if key == "op":
            return self.operator
        return None

def parse_instr(string):
    if string.endswith(":"):
        return dict(op="label", label=string[:-1])

    operator, operands = parse_operator_and_operands(string)
    if operator == "movl":
        operator = "literal"
    if operator == "movl_hi":
        operator = "literal_hi"
    if operator not in OpInfos:
        return PseudoInstr(operator, PseudoInfos[operator].parse(operands))
    ret = dict(op=operator)
    ret.update(get_operands_in_correct_positions(operator, operands))
    ret["trace"] = Trace()
    if operator == "report":
        ret["line"] = 0
        ret["file"] = "TODO"
    return ret

def parse_label(state, string):
    label = string[:-1]
    if state.in_data_section:
        var = GlobalVar(label)
        state.add_global_var(var)
        state.current_global_var = var
    else:
        state.instrs.append(parse_instr(string))

def parse_directive(state, string):
    directive, operands = parse_operator_and_operands(string)
    if directive == "data":
        state.in_data_section = True
    elif directive == "bss":
        state.in_data_section = True
    elif directive == "section" and "rodata" in " ".join(operands):
        state.in_data_section = True
    elif directive == "text":
        state.in_data_section = False
    elif directive == "comm":
        global_var = GlobalVar(operands[0])
        global_var.append_data([I8(0)] * int(operands[1]))
        state.add_global_var(global_var)
    elif directive == "byte":
        state.current_global_var.append_data([I8(int(operands[0]))])
    elif directive == "word":
        if LiteralRegex.match(operands[0]):
            word = I32(int(operands[0]))
        else:
            word = I32(GlobalAddressExpression(operands[0]))
        state.current_global_var.append_data([word])
    elif directive == "zero":
        state.current_global_var.append_data([I8(0)] * (int(operands[0])))

def parse(state, input):
    for line in input:
        line = line.strip()
        comment_char_index = line.find("#")
        if comment_char_index != -1:
            line = line[:comment_char_index].strip()
        if len(line) == 0:
            continue
        if line.startswith(".") and not line.endswith(":"):
            parse_directive(state, line[1:])
            continue
        if line.endswith(":"):
            label = parse_label(state, line)
            continue
        state.instrs.append(parse_instr(line))

def resolve_address_expressions(state):
    for instr in state.instrs:
        for key, value in instr.items():
            if hasattr(value, "compute") and \
                    value.can_compute(state.global_vars):
#                print("replacing {} with {}".format(
#                    value, value.compute(state.global_vars)))
                instr[key] = value.compute(state.global_vars)

def expand_instructions(state):
    ret = []
    select_i = 0
    signed_max_value = 2**15 - 1
    signed_min_value = -2**15
    for i, instr in enumerate(state.instrs):
        if instr["op"] == "literal":
            literal = instr["literal"]
            already_expanded = state.instrs[i + 1]["op"] == "literal_hi"
            if (literal > signed_max_value or literal < signed_min_value) and \
                    not already_expanded:
                lo = literal & 0xffff
                hi = (literal >> 16) & 0xffff
                code = [
                    "movl %r{dst}, {lo}",
                    "movl_hi %r{dst}, %r{dst}, {hi}"
                ]
                ret.extend(parse_instrs(*[format.format(
                    dst=instr["z"], lo=lo, hi=hi) for format in code]))
            else:
                ret.append(instr)
        elif instr["op"] == "addl" and (instr["literal"] > signed_max_value or
                instr["literal"] < signed_min_value):
            ret.extend(parse_instrs(
                "movl %r12, {}".format(instr["literal"]),
                "add %r{0}, %r{1}, %r12".format(instr["z"], instr["a"])))
        elif instr["op"] == "select":
            dst, boolean, true, false = ["%r{}".format(op) for op in
                    instr.operands]
            code = [
                "jmp_if_true {boolean}, select_label_{i}_true",
                "addl {dst}, {false}, 0",
                "goto select_label_{i}_end",
                "select_label_{i}_true:",
                "addl {dst}, {true}, 0",
                "select_label_{i}_end:"
            ]
            ret.extend(parse_instrs(*[
                format.format(dst=dst, boolean=boolean, true=true,
                    false=false, i=select_i) for format in code]))
            select_i += 1
        elif instr["op"] in LoadInstrs + StoreInstrs:
            literal = instr["literal"]
            if hasattr(literal, "compute") and \
                    not literal.can_compute(state.global_vars):
                addr_operand = "z" if instr["op"] in StoreInstrs else "a"
                addr_register = instr[addr_operand]
                ret.extend(parse_instrs(
                    "movl %r12, {}".format(literal.sub_expr),
                    "add %r12, %r{}, %r12".format(addr_register)))
                new_instr = dict(instr)
                new_instr["literal"] = 0
                new_instr[addr_operand] = 12
                ret.append(new_instr)
            else:
                ret.append(instr)
        else:
            ret.append(instr)
    return ret

def assemble(input, options):
    state = ParseState()
    parse(state, input)

    global_vars = list(state.global_vars.values())
    global_vars.sort(key=lambda var: var.size_in_bytes)

    offset = 0
    for var in global_vars:
        var.offset = scaled_offset(offset)
        offset += var.size_in_bytes
        offset = (offset + 7) & ~7

#    print(global_vars)

    resolve_directive_address_expressions(state)

    globals_size_bytes = offset
    #print("globals size: {} bytes".format(globals_size_bytes))
    set_up_instrs = parse_instrs(
            "movl %r3, {}".format(scaled_offset(offset)),
            "movl %r1, 0")

    if "no_initialize_memory" not in options:
        for i, var in enumerate(global_vars):
            offset = 0
            loop_label = "setup_loop_{i}".format(i=i)
            if all_zero(var.data) and var.size_in_bytes > 28:
                words, rest_bytes = divmod(var.size_in_bytes, 4)
                set_up_instrs.extend(parse_instrs(
                    "movl %r2, {0}".format(var.offset),
                    "movl %r5, {0}".format(var.offset + words * 4),
                    loop_label + ":",
                    "store [%r2], %r1, 0",
                    "addl %r2, %r2, 4",
                    "greater %r4, %r5, %r2",
                    "jmp_if_true %r4, " + loop_label))
                for j in range(rest_bytes):
                    set_up_instrs.extend(parse_instrs(
                        "store8 [%r2], %r1, {0}".format(j)))
                continue
            for integer in var.data:
                if isinstance(integer, I32):
                    set_up_instrs.extend(parse_instrs(
                        "movl %r0, {}".format(integer.n)))
                    set_up_instrs.extend(parse_instrs(
                        "store [%r1], %r0, {}".format(var.offset + offset)))
                    offset += scaled_offset(4)
                else:
                    set_up_instrs.extend(parse_instrs(
                        "movl %r0, {}".format(integer.n),
                        "store8 [%r1], %r0, {}".format(var.offset + offset)))
                    offset += scaled_offset(1)

    set_up_instrs.extend(parse_instrs(
        "call %r6, main",
        "stop"))

    state.instrs = set_up_instrs + state.instrs

    resolve_address_expressions(state)
    state.instrs = expand_instructions(state)
    resolve_address_expressions(state)
    state.instrs = expand_instructions(state)
    resolve_address_expressions(state)
    state.instrs = expand_instructions(state)
    return AsmProgram(
        state.instrs[:len(set_up_instrs)], 
        state.instrs[len(set_up_instrs):],
        globals_size_bytes)

def disassemble(instr):
    op = instr["op"]
    display_op = op
    if op == "literal":
        display_op = "movl"
    if op == "literal_hi":
        display_op = "movl_hi"

    if op == "label":
        return "{}:".format(instr["label"])
    return "{} {}".format(display_op, OpInfos[op].disassemble(instr))

if __name__ == "__main__":
    instrs = assemble(sys.stdin).instructions
    for instr in instrs:
        #print(instr["op"], "\t", instr)
        print(disassemble(instr))
        pass
