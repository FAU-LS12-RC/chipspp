import pytest
from chipsc.sim.dataformatter import DataFormatter
from chipsc.sim.numbersparser import ParsedNumbers, DataType
from StringIO import StringIO


def test_should_format_every_number_as_hex_words():
  formatter = DataFormatter()
  output = StringIO()
  formatter.format(output, ParsedNumbers(DataType.U32, [5, 0xf0000000]))
  assert output.getvalue() == "00000005\nf0000000\n"
    
def test_should_format_64_bit_integers_in_little_endian():
  formatter = DataFormatter()
  output = StringIO()
  formatter.format(output, ParsedNumbers(DataType.I64, [0xf00000000000000e]))
  assert output.getvalue() == "0000000e\nf0000000\n"
