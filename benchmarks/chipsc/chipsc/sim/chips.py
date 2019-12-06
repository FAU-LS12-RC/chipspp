import os
from chipsc.sim.numbersparser import DataType
from chipsc.sim.simulator import simulation_file_results

ChipsTypes = {
      DataType.I32: "int",
      DataType.I64: "long",
      DataType.U32: "int",
      DataType.U64: "long",
      DataType.F32: "float",
      DataType.F64: "double" }
GeneralTypes = dict((chips_type, general_type) for general_type, chips_type in 
    ChipsTypes.items())
def chips_type_from_general_type(data_type):
  return ChipsTypes[data_type]
def general_type_from_chips_type(chips_type):
  return GeneralTypes[chips_type]

class Chips(object):
  def __init__(self, chip):
    self.waveform_dump_format = "csv"
    self.chip = chip

  def choose_test_bench(self, benches):
    return benches[0]

  def run(self, chip_interface, sources, test_bench, inputs, stop_condition,
      constants_to_set):
    chips_inputs = dict()
    for name, (data_type, numbers, _) in inputs.items():
      chips_inputs[name] = (chips_type_from_general_type(data_type), numbers)

    self.chip.prepare_for_simulation(
        chips_inputs,
        [("int", name) for name in chip_interface.output_names])

    expected_output = dict()
    if hasattr(stop_condition, "length_per_output"):
      expected_output = stop_condition.length_per_output
    max_steps = None
    if hasattr(stop_condition, "steps"):
      max_steps = stop_condition.steps

    step_i = [0]

    output_lengths = dict()
    for name in chip_interface.output_names:
      output_lengths[name] = 0
    steps_without_change = [0]

    def test_if_finished(chips_outputs):
      finished = True
      step_i[0] += 1
      if max_steps is not None and step_i[0] < max_steps:
        finished = False
      for name, length in expected_output.items():
        current_len = len(chips_outputs[name])
        if current_len < length:
          finished = False
        if current_len != output_lengths[name]:
          steps_without_change[0] = 0
        output_lengths[name] = current_len
      steps_without_change[0] += 1
      assert steps_without_change[0] < 10000000000
      return finished

    for _ in self.chip.simulate(test_if_finished, [], []):
      pass
#    print("steps: ", step_i[0], list(self.chip.chips_outputs["output_1"]))

    for output_name in chip_interface.output_names:
      with open("{}.hex".format(
        os.path.join(test_bench.output_dir, output_name)), "w") as output_file:
        for value in self.chip.chips_outputs[output_name]:
#          print(value)
          output_file.write("{:08x}\n".format(value))

    return simulation_file_results(test_bench, chip_interface)

    #TODO remove
#
#    for name, values in expected_output.items():
#      assert_list_equal(self, chip.output_type(name),
#          list(chip.chips_outputs[name]), values)
