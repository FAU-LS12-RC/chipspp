import subprocess
import os
from chipsc.sim.dataformatter import DataFormatter
from chipsc.sim.simulator import simulation_file_results

class Icarus(object):
  def __init__(self):
    self.waveform_dump_format = "fst"

  def choose_test_bench(self, test_benches):
    for test_bench in test_benches:
      if test_bench.is_verilog:
        return test_bench
    assert False, "need verilog test bench"

  def run(self, chip_interface, sources, test_bench, inputs, stop_condition,
      constants_to_set):
    src_files = list(sources.design) + test_bench.source_files
    formatter = DataFormatter()

    icarus_call = ["iverilog", "-o", "test_bench", "-s", "test"]

    #TODO put into Simulator
    for io_port in chip_interface.io_ports:
      have_input = io_port in inputs
      constants_to_set.update(test_bench.configure_for_io_port(
        formatter, have_input, io_port,
        inputs[io_port] if have_input else None))

    for name, value in constants_to_set.items():
      icarus_call.append("-D{}={}".format(name, value))

    icarus_call += src_files
    subprocess.check_call(icarus_call)
    vvp_call = ["vvp"]
    if "ICARUSLIB" in os.environ:
        vvp_call.extend(["-M{}/ivl".format(os.environ["ICARUSLIB"])])
    vvp_call.extend(["./test_bench", "-fst-speed"])
    subprocess.check_call(vvp_call)

    return simulation_file_results(test_bench, chip_interface)
