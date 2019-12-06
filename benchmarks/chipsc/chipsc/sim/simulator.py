import os
from collections import namedtuple
from chipsc.filedatasource import FileDataSource

class StepsCondition(object):
  def __init__(self, steps):
    self.steps = steps
class OutputLengthCondition(object):
  def __init__(self, length_per_output):
    self.length_per_output = length_per_output

SimulationResult = namedtuple("SimulationResult", ["data", "cycles"])

def simulation_file_results(test_bench, chip_interface):
  return FileDataSource(test_bench.output_dir,
      name_filter=lambda name: any(name.startswith(output_name + ".") for
        output_name in chip_interface.output_names),
      default_type="u32")

class Simulator(object):
  def __init__(self, chip, data_source, numbers_parser, backend, interpreter):
    self.data_source = data_source
    self.backend = backend
    self.numbers_parser = numbers_parser
    self.chip = chip
    self.interpreter = interpreter

  def run(self, sources, output_dir, stop_condition):
    constants_to_set = dict()
    test_bench = self.backend.choose_test_bench(sources.test_benches)

    constants_to_set.update(test_bench.configure_for_output_dir(output_dir,
      self.backend.waveform_dump_format))
    constants_to_set.update(test_bench.configure_for_stop_condition(
      stop_condition))

    inputs = self.numbers_parser.parse(self.data_source.read())
    result = self.backend.run(self.chip.interface, sources, test_bench, inputs,
        stop_condition, constants_to_set)
    result = self.numbers_parser.parse(result.read())
    return self.interpreter.reinterpret(result)
