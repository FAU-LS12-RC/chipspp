from collections import namedtuple
import binascii
import struct
from StringIO import StringIO

#TODO move into common file
class DataType(namedtuple("DataType", ["number_type", "length"])):
  @property
  def is_signed(self):
    return self.number_type == "i"
  @property
  def is_floating_point(self):
    return self.number_type == "f"
  @property
  def is_integer(self):
    return not self.is_floating_point
  @property
  def length_in_nibbles(self):
    return self.length / 4

  @property
  def is_word_integer(self):
    return self.length == 32 and self.is_integer
  @property
  def is_double_word_integer(self):
    return self.length == 64 and self.is_integer
  @property
  def is_c_float(self):
    return self.length == 32 and self.is_floating_point
  @property
  def is_c_double(self):
    return self.length == 64 and self.is_floating_point

  def __str__(self):
    return "{}{}".format(self.number_type, self.length)

DataType.F32 = DataType("f", 32)
DataType.F64 = DataType("f", 64)
DataType.U32 = DataType("u", 32)
DataType.U64 = DataType("u", 64)
DataType.I32 = DataType("i", 32)
DataType.I64 = DataType("i", 64)
DataType.U16 = DataType("u", 16)
DataType.I16 = DataType("i", 16)
RawInputNumbers = namedtuple("RawInputNumbers",
    ["data_type", "string", "cycles"])
ParsedNumbers = namedtuple("ParsedNumbers", ["data_type", "numbers", "cycles"])

_Name = namedtuple("_Name", ["name", "index", "format"])

def _parse_name(name):
  if name.count(".") == 2:
    return _Name(*name.split("."))
  else:
    name, format = name.split(".")
    return _Name(name, 0, format)

def _hex_extend(data_type, line):
  dest_length = 8 if data_type.length <= 32 else 16
  extension = "F" if data_type.is_signed and \
      len(line) == data_type.length_in_nibbles and \
      int(line[0], base=16) >= 8 else "0"
  return extension * (dest_length - len(line)) + line

def _parse_hex_string(data_type, string):
  lines = string.splitlines()
  struct_format = ">I" if data_type.length <= 32 else ">Q"
  return [struct.unpack(struct_format,
    binascii.unhexlify(_hex_extend(data_type, line)))[0] 
    for line in lines]

def _parse_dec_number(data_type, word):
  if data_type.is_floating_point:
    return float(word)
  return int(word) & ((1 << data_type.length) - 1)

def _parse_dec_string(data_type, string):
  lines = string.splitlines()
  ret= []
  for line in lines:
    if line.startswith("#"):
      continue
    ret.extend([_parse_dec_number(data_type, word) for word in line.split()])
  return ret

def _parse_bin_string(data_type, string):
  assert data_type == DataType.U32
  ret = []
  string = StringIO(string)
  while True:
    buf = string.read(4)
    if len(buf) == 0:
      break
    buf = list(buf) + ['\x00'] * (4 - len(buf))
    buf = [ord(n) for n in buf]
    ret.append(
        (buf[0] << 24) |
        (buf[1] << 16) |
        (buf[2] << 8) |
        buf[3])
  return ret

def _parse_data_type(string):
  number_type = string[0]
  return DataType(number_type, int(string[1:]))

def _parse_string(format, numbers):
  data_type = _parse_data_type(numbers.data_type)
  parse_funcs = dict(hex=_parse_hex_string, dec=_parse_dec_string,
      bin=_parse_bin_string)
  return ParsedNumbers(
      data_type,
      parse_funcs[format](data_type, numbers.string),
      numbers.cycles)

class NumbersParser(object):
  def parse(self, raw_numbers):
    parsed_numbers = dict()

    raw_numbers = [(_parse_name(name), string) for name, string in
      raw_numbers.items()]
    raw_numbers.sort(key=lambda (name, _): name.index)

    for (name, _, format), numbers in raw_numbers:
      if name not in parsed_numbers:
        parsed_numbers[name] = []
      parsed_numbers[name].append(_parse_string(format, numbers))

    for name, nss in parsed_numbers.items():
      parsed_numbers[name] = ParsedNumbers(nss[0].data_type, 
          [n for ns in nss for n in ns.numbers],
          nss[0].cycles)
    return parsed_numbers
