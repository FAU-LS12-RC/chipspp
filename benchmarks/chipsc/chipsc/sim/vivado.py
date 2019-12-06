from chipsc.sim.dataformatter import DataFormatter
from chipsc.util import relative_file_path
import subprocess
from chipsc.sim.simulator import simulation_file_results

class Vivado(object):
  def __init__(self, post_impl):
    self.waveform_dump_format = "vcd"
    self.post_impl = post_impl

  def choose_test_bench(self, test_benches):
    for test_bench in test_benches:
      if test_bench.is_verilog:
        return test_bench
    assert False, "verilog test bench preferred"

  def run(self, chip_interface, sources, test_bench, inputs, stop_condition,
      constants_to_set):
    src_files = list(sources.design) + test_bench.source_files
    formatter = DataFormatter()

    script_call = ["vivado", "-mode", "batch", "-source",
        relative_file_path("run-simulation.tcl"), "-tclargs",
        sources.project_file, "test", "1" if self.post_impl else "0"]

    #TODO put into Simulator
    for io_port in chip_interface.io_ports:
      have_input = io_port in inputs
      constants_to_set.update(test_bench.configure_for_io_port(
        formatter, have_input, io_port,
        inputs[io_port] if have_input else None))

    for name, value in constants_to_set.items():
      if value.endswith("\""):
        value = value + ".vivadobugworkaround"
      script_call.append("{}={}".format(name, value))

    subprocess.check_call(script_call)

    return simulation_file_results(test_bench, chip_interface)
