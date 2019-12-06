import pytest
from chipsc.sim.numbersparser import NumbersParser, DataType
from test.builders import raw_input_numbers, parsed_numbers

def test_should_parse_and_concatenate_input_strings():
  test_input = {
    "foo.hex": raw_input_numbers("i16", "001\nffff\nfff"),
    "bar.bin": raw_input_numbers("u32", "\xff\x00\x00\x00"),
    "bar.5.dec": raw_input_numbers("u32", "25")
  }
  parser = NumbersParser()
  assert parser.parse(test_input) == dict(
      foo=parsed_numbers(DataType("i", 16), [1, 0xffffffff, 0x00000fff]),
      bar=parsed_numbers(DataType("u", 32), [0xff000000, 25]))

def test_should_parse_floating_point_literals():
  parser = NumbersParser()
  assert parser.parse({ "foo.dec": raw_input_numbers("f32", "-2.5e5") }) == \
      dict(foo=parsed_numbers(DataType("f", 32), [-2.5e5]))

def test_should_be_able_to_parse_numbers_longer_than_the_word_size():
  parser = NumbersParser()
  assert parser.parse({ "foo.hex": raw_input_numbers("i64",
    "100ff\nfffffffffffffffe") }) == \
      dict(foo=parsed_numbers(DataType("i", 64), [0x100ff, 0xfffffffffffffffe]))

def test_should_parse_negative_numbers_as_twos_complement():
  parser = NumbersParser()
  assert parser.parse({ "foo.dec": raw_input_numbers("i32", "-1") }) == \
      dict(foo=parsed_numbers(DataType.I32, [0xffffffff]))

def test_should_pass_on_cycles_values():
  parser = NumbersParser()
  assert parser.parse({ 
    "foo.dec": raw_input_numbers("i32", "1", cycles=100),
    "foo.2.dec": raw_input_numbers("i32", "2", cycles=100)}) == \
      dict(foo=parsed_numbers(DataType.I32, [1, 2], cycles=100))

#TODO error when data types differ
#TODO make word size variable
