from chipsc.util import relative_file_path
from chips.utils.codewriter import CodeWriter
import os

class VerilogTestBench(object):
  def __init__(self, directory):
    self.directory = directory
    self.is_verilog = True
    self.source_files = [os.path.join(self.directory, "tb.v")]
    self.output_dir = None
    self.waits_for_number_of_values = False

  def configure_for_output_dir(self, path, dump_format):
    self.output_dir = path
    return dict(DUMP_PATH="\"{}\"".format(
      os.path.join(path, "dump.{}".format(dump_format))))

  def configure_for_io_port(self, formatter, is_input, name, values):
    formatted_file_path = os.path.join(self.output_dir, "{}.hex".format(name))
    if is_input:
      with open(formatted_file_path, "wb") as formatted_file:
        formatter.format(formatted_file, values)

    ret = dict()
    ret["{}_file".format(name)] = "\"{}\"".format(formatted_file_path)
    if self.waits_for_number_of_values:
      ret["{}_cycles_file".format(name)] = "\"{}\"".format(
          os.path.join(self.output_dir, "{}.cycles".format(name)))
    return ret

  def configure_for_stop_condition(self, condition):
    ret = dict()
    if hasattr(condition, "length_per_output"):
      self.waits_for_number_of_values = True
      ret["WAIT_FOR_VALUES"] = "1"
      for output, length in condition.length_per_output.items():
        ret["NO_OF_VALUES_{}".format(output)] = str(length)
    if hasattr(condition, "steps"):
      ret["RESTRICT_STEPS"] = "1"
      ret["STEPS"] = str(condition.steps)
    return ret

  def write(self, interface):
    with open(relative_file_path("tb.v"), "rb") as tb_file:
      testbench_format = tb_file.read()

    with open(os.path.join(self.directory, "tb.v"), "wb") as tb_file:
      writer = CodeWriter(tb_file)
      writer.write(testbench_format,
          inputs=interface.input_names,
          outputs=interface.output_names,
          chip_name=interface.name)
#
#    output_values = run_hdl_testbench(chip, self.run_icarus)
#
#  for name, values in expected_output.items():
#    assert_list_equal(self, chip.output_type(name),
#        output_values[name], values)
