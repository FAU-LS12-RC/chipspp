from chipsc.sim.numbersparser import DataType
import chips_c
import struct

class DataFormatter(object):
  def format(self, output, data):
    data_type = data.data_type
    for n in data.numbers:
      output.write(self._format_value(data_type, n))
      output.write("\n")

  def _format_value(self, data_type, value):
    if data_type == DataType.F32:
      return "".join("{:02x}".format(ord(byte)) for byte in
          struct.pack(">f", value))
    if data_type.is_integer and data_type.length <= 32:
      return "".join("{:02x}".format(ord(byte)) for byte in
          struct.pack(">I", value))
    elif data_type.is_integer and data_type.length >= 64:
      return self._format_value(DataType.U32, chips_c.low_word(value)) + \
          "\n" + self._format_value(DataType.U32, chips_c.high_word(value))
    elif data_type == DataType.F64:
      return self._format_value(DataType.U64, chips_c.double_to_bits(value))
    else:
      assert False, data_type
