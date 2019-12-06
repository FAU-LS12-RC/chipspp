import pytest
from StringIO import StringIO

from chips.compiler.assembler import assemble

def assert_instrs(actual, expected):
  actual_list = list(actual)
  assert len(actual_list) == len(expected)
  for actual_instr, expected_instr in zip(actual_list, expected):
    for key, value in expected_instr.items():
      assert actual_instr[key] == value

def test_should_translate_assembly_language_into_instr_dicts():
  input = StringIO("subtract %r0, %r1, %r2\naddl %r0, %r0, 4")
  program = assemble(input)
  assert_instrs(program.program_instrs, [
    dict(op="subtract", z=0, a=1, b=2),
    dict(z=0, a=0, literal=4)])

#def test_should_correctly_expand_a_load_double_into_a_double_reg():
#  input = StringIO("load_double %a, [%r12], 0")
#  assert_instrs(assemble(input).program_instrs,
#      assemble("""
#      load %r
#      """).program_instrs)
    
