from chipsc.sim.numbersparser import RawInputNumbers, ParsedNumbers

def raw_input_numbers(data_type, string, cycles=-1):
  return RawInputNumbers(data_type, string, cycles)
def parsed_numbers(data_type, numbers, cycles=-1):
  return ParsedNumbers(data_type, numbers, cycles)

class MockChip(object):
  def __init__(self, interface=None, asm_program=None):
    self.interface = interface
    self.asm_program = asm_program

