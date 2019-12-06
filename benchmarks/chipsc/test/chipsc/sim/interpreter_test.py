import pytest
from chipsc.sim.interpreter import Interpreter
from chipsc.sim.numbersparser import DataType
from test.builders import parsed_numbers

class Fixture(object):
  def reinterpret(self, new_type, *ns):
    interpreter = Interpreter(dict(foo=new_type))
    actual = interpreter.reinterpret(
        dict(foo=parsed_numbers(DataType.U32, list(ns))))
    assert actual["foo"].data_type == new_type
    actual_ns = actual["foo"].numbers
    assert len(actual_ns) == 1
    return actual_ns[0]

@pytest.fixture
def fixture():
  return Fixture()

def test_should_shorten_number_by_simply_cutting_off_the_high_part(fixture):
  assert fixture.reinterpret(DataType.U16, 0x0000ff11) == 0xff11
#TODO
#def test_should_not_cut_off_non_zero_bits(fixture):
#  with pytest.raises(Exception):
#    fixture.reinterpret(DataType.U16, 0xf000ff11)

def test_should_connect_multiple_words_into_longer_results_with_le_ordering(
    fixture):
  assert fixture.reinterpret(DataType.U64, 0x8000ff11, 0x76543210) == \
      0x765432108000ff11

def test_should_reinterpret_bits_as_float(fixture):
  assert fixture.reinterpret(DataType.F32, 0x3f800000) == 1.0
  assert fixture.reinterpret(DataType.F64, 0x00000000, 0x3ff00000) == 1.0
