import os
import re
import shutil
from collections import OrderedDict, namedtuple
import subprocess
import json

from chipsc.util import cwd, relative_file_path
from chipsc.compiler import chip_from_program, compile_chip
from chips.compiler.assembler import ChipInterface
from chipsc.compileoptions import compilation_id
from chipsc.sim.simulator import Simulator
from chipsc.sim.numbersparser import NumbersParser
from chipsc.sim.interpreter import Interpreter, IdentityInterpreter
from chipsc.sim.icarus import Icarus
from chipsc.sim.vivado import Vivado
from chipsc import sim
import sim.chips
from chipsc.filedatasource import FileDataSource
from chipsc.verilogtestbench import VerilogTestBench
from chips.compiler.assembler import AsmProgram
from chips.compiler.parse_tree import Trace

IoPortRegex = re.compile(
    r"^\s*(const)?\s*(?P<type>[a-z]+)\s+(?P<name>[a-zA-Z_0-9]+)"
    r"\s*=?\s*(?P<dir>in|out)put\s*\("
    r"\s*\"(?P=name)\"\s*"
    r"\)\s*;\s*$")

ProjectSources = namedtuple("ProjectSources", ["design", "test_benches",
  "project_file"])

def parse_io_ports(source_file_path, compile_options):
  inputs = OrderedDict()
  outputs = []

  with open(os.devnull, "w") as dev_null:
    defines = [opt for opt in compile_options.misc_opts if opt.startswith("-D")]
    cpp_out = subprocess.Popen(["cpp", "-nostdinc", "-E"] + defines +
        [source_file_path], stderr=dev_null, stdout=subprocess.PIPE)

    for line in cpp_out.stdout:
      match = IoPortRegex.match(line)
      if match:
        name = match.group("name")
        is_input = match.group("dir") == "in"
        if is_input:
          inputs[name] = ("int", [])
        else:
          outputs.append(("int", name))
        #print("{} {}".format("input" if is_input else "output", name))

  return inputs, outputs

class ImplOptions(object):
  def __init__(self, clk_period_ns, board_name):
    self.clk_period_ns = clk_period_ns
    self.board_name = board_name

def write_constraints_xdc(output, interface, clk_period_ns):
  output.write(
      "create_clock -period {0} -name clk -waveform {{0.000 {1}}} "
      "[get_ports clk]\n".format(clk_period_ns, clk_period_ns / 2))

  set_input_delay_min = "set_input_delay -clock [get_clocks clk] -min " \
      "-add_delay 5.000 "
  set_input_delay_max = "set_input_delay -clock [get_clocks clk] -max " \
      "-add_delay 10.000 "
  set_output_delay = "set_output_delay -clock [get_clocks clk] 5.000 "

#  for input_port in inputs:
#    output.write(set_input_delay_min +
#        get_ports_call(input_port + "[*]") + "\n")
#    output.write(set_input_delay_max +
#        get_ports_call(input_port + "[*]") + "\n")
#    output.write(set_input_delay_min + 
#        get_ports_call(input_port + "_stb") + "\n")
#    output.write(set_input_delay_max + 
#        get_ports_call(input_port + "_stb") + "\n")
#    output.write(set_output_delay +
#        get_ports_call(input_port + "_ack") + "\n")
#
#  for output_port in outputs:
#    output.write(set_output_delay +
#        get_ports_call(output_port + "[*]") + "\n")
#    output.write(set_output_delay +
#        get_ports_call(output_port + "_stb") + "\n")
#    output.write(set_input_delay_min +
#        get_ports_call(output_port + "_ack") + "\n")
#    output.write(set_input_delay_max +
#        get_ports_call(output_port + "_ack") + "\n")
#
#  output.write(set_input_delay_min + get_ports_call("rst") + "\n")
#  output.write(set_input_delay_max + get_ports_call("rst") + "\n")

def write_uut_vhdl(out, interface):
  for i, _ in enumerate(interface.input_names):
    out.write("""
      signal d_in_vec_{i} : std_logic_vector(DATA_WIDTH-1 downto 0) := (others => '0');
      signal d_in_valid_{i} : std_logic := '0';
      signal d_in_last_{i} : std_logic := '0';
      signal d_in_ready_{i} : std_logic;
      signal file_ready_{i} : std_logic := '0';
      signal file_valid_{i} : std_logic := '0';
    """.format(i=i))

  for i, _ in enumerate(interface.output_names):
    out.write("""
      signal d_out_vec_{i} : std_logic_vector(DATA_WIDTH-1 downto 0) := (others => '0');
      signal d_out_valid_{i} : std_logic := '0';
      signal d_out_last_{i} : std_logic := '0';
      signal d_out_ready_{i} : std_logic;
    """.format(i=i))

  out.write("\nbegin\n")

  #TODO needs one less .. for behavioral simulation
  for i, output_name in enumerate(interface.output_names):
    out.write("""
      data_out_i_{i} : entity xil_defaultlib.file_slave
        generic map(
                    FILENAME  => "../../../../../{output_file}",
                    USE_HEX  => DATA_FILE_IS_HEX,
                    DATA_WIDTH_BYTES => DATA_WIDTH/8)
        port map(clk => clk,
              rst => rst,
              ready => d_out_ready_{i},
              data => d_out_vec_{i},
              valid =>d_out_valid_{i},
              last => d_out_last_{i}
                          );
    """.format(i=i, output_file=output_name))

  for i, input_name in enumerate(interface.input_names):
    out.write("""
      file_ready_{i} <= d_in_ready_{i} and run;
      d_in_valid_{i} <= file_valid_{i} and run;
      data_in_i_{i} : entity xil_defaultlib.file_master
          generic map(
                      FILENAME  => "../../../../../{stim_file}",
                      USE_HEX  => DATA_FILE_IS_HEX,
                      DATA_WIDTH_BYTES => DATA_WIDTH/8)
          port map( clk => clk,
                rst => rst,
                ready => file_ready_{i},
                data => d_in_vec_{i},
                valid =>file_valid_{i},
                last => d_in_last_{i}
                );
    """.format(i=i, stim_file=input_name))

  out.write("uut_i : entity xil_defaultlib.{}\n".format(interface.name))

  out.write("""
    port map(
  """)

  for i, input_name in enumerate(interface.input_names):
    out.write("""
        {name} => d_in_vec_{i},
        {name}_stb => d_in_valid_{i},
        --d_in_last => d_in_last,
        {name}_ack => d_in_ready_{i},
    """.format(name=input_name, i=i))

  for i, output_name in enumerate(interface.output_names):
    out.write("""
        {name} => d_out_vec_{i},
        {name}_ack => d_out_ready_{i},
        {name}_stb => d_out_valid_{i},
        --d_out_last => d_out_last,
    """.format(name=output_name, i=i))

  out.write("""
      clk => clk,
      rst => rst);\n
  """)

def write_package_ip_script(out, interface):
  out.write("""
set xprPath [lindex $argv 0]
set repoPath [lindex $argv 1]

open_project $xprPath

#create_bd_design "design_1"
#create_bd_cell -type module -reference hls_fft hls_fft_0

#export_ip_user_files -of_objects [get_files .../*.srcs/sources_1/bd/design_1/design_1.bd] -no_script -reset -force -quiet

ipx::package_project -root_dir $repoPath -vendor FAU -library simulink_blocks -import_files
#-set_current false
#ipx::unload_core /tmp/foo/component.xml
#ipx::edit_ip_in_project -upgrade true -name tmp_edit_project -directory /tmp/foo /tmp/foo/component.xml

foreach input [list {input_names}] {{
  ipx::add_bus_interface $input [ipx::current_core]
  set_property abstraction_type_vlnv xilinx.com:interface:axis_rtl:1.0 [ipx::get_bus_interfaces $input -of_objects [ipx::current_core]]
  set_property bus_type_vlnv xilinx.com:interface:axis:1.0 [ipx::get_bus_interfaces $input -of_objects [ipx::current_core]]
  set_property interface_mode slave [ipx::get_bus_interfaces $input -of_objects [ipx::current_core]]

  ipx::add_port_map TDATA [ipx::get_bus_interfaces $input -of_objects [ipx::current_core]]
  set_property physical_name $input [ipx::get_port_maps TDATA -of_objects [ipx::get_bus_interfaces $input -of_objects [ipx::current_core]]]

  ipx::add_port_map TVALID [ipx::get_bus_interfaces $input -of_objects [ipx::current_core]]
  set_property physical_name ${{input}}_stb [ipx::get_port_maps TVALID -of_objects [ipx::get_bus_interfaces $input -of_objects [ipx::current_core]]]

  ipx::add_port_map TREADY [ipx::get_bus_interfaces $input -of_objects [ipx::current_core]]
  set_property physical_name ${{input}}_ack [ipx::get_port_maps TREADY -of_objects [ipx::get_bus_interfaces $input -of_objects [ipx::current_core]]]

  ipx::associate_bus_interfaces -busif $input -clock clk [ipx::current_core]
}}

foreach output [list {output_names}] {{
  ipx::add_bus_interface $output [ipx::current_core]
  set_property abstraction_type_vlnv xilinx.com:interface:axis_rtl:1.0 [ipx::get_bus_interfaces $output -of_objects [ipx::current_core]]
  set_property bus_type_vlnv xilinx.com:interface:axis:1.0 [ipx::get_bus_interfaces $output -of_objects [ipx::current_core]]
  set_property interface_mode master [ipx::get_bus_interfaces $output -of_objects [ipx::current_core]]

  ipx::add_port_map TDATA [ipx::get_bus_interfaces $output -of_objects [ipx::current_core]]
  set_property physical_name $output [ipx::get_port_maps TDATA -of_objects [ipx::get_bus_interfaces $output -of_objects [ipx::current_core]]]

  ipx::add_port_map TVALID [ipx::get_bus_interfaces $output -of_objects [ipx::current_core]]
  set_property physical_name ${{output}}_stb [ipx::get_port_maps TVALID -of_objects [ipx::get_bus_interfaces $output -of_objects [ipx::current_core]]]

  ipx::add_port_map TREADY [ipx::get_bus_interfaces $output -of_objects [ipx::current_core]]
  set_property physical_name ${{output}}_ack [ipx::get_port_maps TREADY -of_objects [ipx::get_bus_interfaces $output -of_objects [ipx::current_core]]]

  ipx::associate_bus_interfaces -busif $output -clock clk [ipx::current_core]
}}

set_property core_revision 2 [ipx::current_core]
ipx::create_xgui_files [ipx::current_core]
ipx::update_checksums [ipx::current_core]
ipx::save_core [ipx::current_core]

update_ip_catalog
  """.format(input_names=" ".join(interface.input_names),
      output_names=" ".join(interface.output_names)))

def run_vivado_script(script_path, args):
  subprocess.check_call(["vivado", "-mode", "batch", "-source", script_path,
    "-tclargs"] + args)

class Project(object):
  def __init__(self, directory, chip_interface, board_name, chip=None):
    self.directory = directory
    self.board_name = board_name
    self.project_id = chip_interface.id
    self.chip_interface = chip_interface
    self.chip = chip
    self.chip_name = self.chip_interface.name
    self.asm_program = None
    if chip is not None:
      self.asm_program = chip.asm_program

  @property
  def project_file_path(self):
    return os.path.join(self.directory, "project.json")
  @property
  def package_ip_script(self):
    return os.path.join(self.directory, "package-ip.tcl")
  @property
  def vivado_project_dir(self):
    return os.path.join(self.directory, "vivado")
  @property
  def xpr_path(self):
    return os.path.join(self.vivado_project_dir, self.project_id + ".xpr")

  @property
  def src_dir(self):
    return os.path.join(self.directory, "src")
  @property
  def sim_dir(self):
    return os.path.join(self.directory, "sim")

  def generate_vivado_project(self):
    run_vivado_script(relative_file_path("make-project.tcl"),
        [self.directory, self.project_id, self.vivado_project_dir,
          self.board_name])

  def generate_ip_block(self, repo_dir):
    assert os.path.isfile(self.xpr_path)

    run_vivado_script(self.package_ip_script, [self.xpr_path, repo_dir])

  def _get_hdl_files(self, directory):
    for name in os.listdir(directory):
      if name.endswith(".vhd") or name.endswith(".v"):
        yield os.path.join(directory, name)

  @property
  def sources(self):
    return ProjectSources(
        list(self._get_hdl_files(self.src_dir)),
        [VerilogTestBench(self.sim_dir)],
        self.xpr_path)

  def run_simulation(self, backend, input_dir, output_dir, stop_condition,
      output_types=None):
    #TODO only interface?
    class Chip(object):
      def __init__(self, interface):
        self.interface = interface
    if not os.path.exists(output_dir):
      os.mkdir(output_dir)

    backend = Icarus() if backend == "icarus" else \
        sim.chips.Chips(self.chip) if backend == "chips" else \
        Vivado(True) if backend == "post-impl" else \
        Vivado(False)

    data_interpreter = Interpreter(output_types) if output_types is not None \
        else IdentityInterpreter()
    simulator = Simulator(Chip(self.chip_interface),
        FileDataSource(input_dir), NumbersParser(),
        backend, data_interpreter)
    return simulator.run(self.sources, output_dir, stop_condition)
#    simulation = IcarusSimulation(self, sim.read_inputs(input_dir))
#    simulation.run(output_dir)

  def run_implementation(self, results_dir):
    assert os.path.isfile(self.xpr_path)

    run_vivado_script(relative_file_path("synthesize.tcl"), [self.xpr_path])

    impl_dir = os.path.join(self.vivado_project_dir, self.project_id + ".runs",
        "impl_1")

    for report_suffix in [
        "_utilization_placed.rpt", "_timing_summary_routed.rpt"]:
      shutil.copyfile(
          os.path.join(impl_dir, self.chip_name + report_suffix),
          os.path.join(results_dir, self.project_id + report_suffix))

  #TODO move somewhere else (some repo)
  @classmethod
  def load_chip_from_asm(cls, json_dict, interface):
    class SomeChip(object):
      def __init__(self, program):
        self.asm_program = program

    id_to_input = dict()
    id_to_output = dict()
    io_ports = [id_to_input, id_to_output]
    for name, index in [("id_to_input", 0), ("id_to_output", 1)]:
      for key, value in json_dict[name].items():
        io_ports[index][int(key)] = value

    instructions = json_dict["instructions"]
    for instr in instructions:
      if "trace" in instr:
        instr["trace"] = Trace.from_components(
            instr["trace"][0], instr["trace"][1])

    ret = None
    program = AsmProgram(
        [],
        json_dict["instructions"],
        json_dict["globals_size"])
    program.set_stack_size(json_dict["stack_size"])
    program.set_names(json_dict["chip_id"], json_dict["chip_name"],
        json_dict["component_name"])
    program.set_io_ports(id_to_input, id_to_output)

    return chip_from_program(program, interface.name, interface.id)

  def turn_program_into_dict(self, program):
    instructions = []
    for instr in program.instructions:
      instr = dict(instr)
      if "trace" in instr:
        trace = instr["trace"]
        instr["trace"] = [
            trace.lineno,
            trace.filename]
            #trace.function,
            #trace.global_scope,
            #trace.statement]
      instructions.append(instr)

    ret = dict(
        stack_size=program.stack_size_bytes,
        instructions=instructions,
        globals_size=program.globals_size_bytes,
        chip_id=program.chip_id,
        chip_name=program.chip_name,
        component_name=program.component_name,
        id_to_input=program.id_to_input,
        id_to_output=program.id_to_output)
    return ret

  @classmethod
  def open(cls, directory):
    with open(os.path.join(directory, "project.json"), "r") as project_file:
      project = json.load(project_file)
    chip = None
    interface = ChipInterface(*project["chip_interface"])
    if "program" in project:
      chip = cls.load_chip_from_asm(project["program"], interface)
    return cls(directory, interface, project["board_name"], chip)

  def write_project_file(self):
    with open(self.project_file_path, "w") as project_file:
      dump = dict(
        project_id=self.project_id,
        chip_interface=self.chip_interface,
        board_name=self.board_name)
      if self.asm_program is not None:
        dump["program"] = self.turn_program_into_dict(self.asm_program)
      json.dump(dump, project_file)

  @classmethod
  def compile(cls, directory, compile_options, impl_opts, source_file_paths,
      chip_name=None):
    if chip_name is None:
      chip_name = os.path.splitext(os.path.basename(source_file_paths[0]))[0]

    if os.path.isdir(directory):
      assert list(os.listdir(directory)) == []
    else:
      os.mkdir(directory)
    src_dir = os.path.join(directory, "src")
    os.mkdir(src_dir)

    chip = compile_chip(compile_options, src_dir, source_file_paths,
        chip_name=chip_name)

    with cwd(src_dir):
      chip.chip.generate_verilog()

    return cls.create(directory, chip.asm_program.interface, impl_opts, chip)

  @classmethod
  def create(cls, directory, interface, impl_opts, chip=None):
    sim_dir = os.path.join(directory, "sim")
    os.mkdir(sim_dir)
    with open(os.path.join(directory, "constraints.xdc"), "w") as constraints:
      write_constraints_xdc(constraints, interface, impl_opts.clk_period_ns)

    write_vhdl_testbench(sim_dir, interface)
    test_bench = VerilogTestBench(sim_dir)
    test_bench.write(interface)

    ret = cls(directory, interface, impl_opts.board_name, chip)
    with open(ret.package_ip_script, "w") as package_ip_script:
      write_package_ip_script(package_ip_script, interface)
    ret.write_project_file()
    return ret

def write_vhdl_testbench(sim_dir, interface):
  file_name = "tb_file.vhd"
  with open(relative_file_path(file_name), "r") as test_bench_template:
    with open(os.path.join(sim_dir, "tb_file.vhd"), "w") as test_bench:
      for line in test_bench_template:
        if line == "--CHIPS UUT HERE\n":
          write_uut_vhdl(test_bench, interface)
        if line == "--use xil_defaultlib.foo;\n":
          test_bench.write("use xil_defaultlib.{};\n".format(interface.name))
        else:
          test_bench.write(line)

    shutil.copyfile(
      relative_file_path("file_io.vhd"),
      os.path.join(sim_dir, "file_io.vhd"))
