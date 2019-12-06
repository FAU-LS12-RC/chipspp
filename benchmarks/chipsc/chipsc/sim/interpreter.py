from chipsc.sim.numbersparser import ParsedNumbers, DataType
import struct

class IdentityInterpreter(object):
  def reinterpret(self, data):
    return data

class Interpreter(object):
  def __init__(self, data_types_per_name):
    self.data_types_per_name = data_types_per_name

  def _reinterpret_numbers(self, old_type, new_type, numbers):
    assert old_type == DataType.U32
    if old_type == new_type:
      return numbers
    if old_type.length < new_type.length:
      numbers = [hi << 32 | lo for lo, hi in zip(numbers[::2], numbers[1::2])]
    if not (new_type.is_signed or new_type.is_floating_point):
      return numbers
    return map(lambda n: self._reinterpret_number(new_type, n),
        numbers)

  def _reinterpret_number(self, new_type, n):
    if new_type.is_floating_point:
      if new_type.length > 32:
        return struct.unpack(">d", struct.pack(">Q", n))[0] 
      return struct.unpack(">f", struct.pack(">I", n))[0]
    negative = (1 << (new_type.length - 1)) & n
    if negative:
      return n - (1 << new_type.length)
    return n

  def reinterpret(self, data):
    ret = dict()
    for name, sequence in data.items():
      data_type = self.data_types_per_name[name]
      ret[name] = ParsedNumbers(
          data_type, self._reinterpret_numbers(sequence.data_type, data_type,
            sequence.numbers),
          sequence.cycles)
    return ret

def parse_value(value_type, bits):
  if value_type == "float":
    return struct.unpack(">f", struct.pack(">I", bits))[0]
  elif value_type == "int":
    return bits
  else:
    assert False
def read_values(value_type, input_file):
  low_word = None
  ret = []
  for line in input_file:
    bits = int(line, 16)
    if value_type in ["long", "double"]:
      if low_word is None:
        low_word = parse_value("int", bits)
      else:
        value = chips_c.join_words(parse_value("int", bits), low_word)
        ret.append(value if value_type == "long" else
            chips_c.bits_to_double(value))
        low_word = None
    else:
      ret.append(parse_value(value_type, bits))
  return ret
