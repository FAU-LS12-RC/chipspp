from __future__ import print_function
import unittest
import os
import contextlib
import tempfile
import shutil
import subprocess
import numpy as np
import math
import json
import sys
import struct
import binascii
def import_chips():
  pass
#  global chips_c, disassemble, ImplOptions, Project, OutputLengthCondition, \
#      make_net_project, general_type_from_chips_type

from chips.compiler.assembler import disassemble
import chips_c
from chipsc.net.compiler import Net
from chipsc.project import Project, ImplOptions
from chipsc.sim.chips import general_type_from_chips_type
from chipsc.sim.simulator import OutputLengthCondition
from chipsc.application import make_net_project

import distutils.spawn
from chipsc.compileoptions import OptLevel, Python, LlvmO0, LlvmO1, LlvmO2, \
    LlvmO3, LlvmOs, Toolchain, compilation_id, generic_chip_id, CompileOptions

def flatten(xss):
  return [x for xs in xss for x in xs]

DefaultWithArg = True
def default_output_name(i, with_arg=DefaultWithArg):
  if with_arg:
    return "outputArg{}".format(i)
  else:
    return "output_{}".format(i)
def default_input_name(i, with_arg=DefaultWithArg):
  if with_arg:
    return "inputArg{}".format(i)
  else:
    return "input_{}".format(i)
def default_name_inputs(n, with_arg=DefaultWithArg):
  return [("int", default_input_name(i + 1, with_arg=with_arg)) for 
      i in range(n)]
def default_name_outputs(n, with_arg=DefaultWithArg):
  return [("int", default_output_name(i + 1, with_arg=with_arg)) for
      i in range(n)]

@contextlib.contextmanager
def cwd(new_dir):
  current_dir = os.getcwd()
  os.chdir(new_dir)
  try:
    yield
  finally:
    os.chdir(current_dir)

PostImpl = object()
Vivado = object()
Icarus = object()
ChipsSimulator = object()

def relative_file_path(name):
  current_dir = os.path.dirname(__file__)
  return os.path.join(current_dir, name)
def code_file_path(name):
  return relative_file_path(os.path.join("..", "..", "chips-programs", name))
def python_toolchain_code_file_path(name):
  return code_file_path(os.path.join("python-toolchain", name))
def llvm_toolchain_code_file_path(name):
  return code_file_path(os.path.join("llvm-toolchain", name))
def jpeg_data(name):
  return relative_file_path(os.path.join("jpeg-test-data", name))

def compile_test_program(code_file_names, options):
  temp_dir = tempfile.mkdtemp(prefix=compilation_id(code_file_names[0], options))
  impl_opts = ImplOptions(14, "xc7z020clg484-1")
  print("generated code in: {}".format(temp_dir))
  if not code_file_names[0].endswith(".py"):
    code_file_paths = [python_toolchain_code_file_path(name) if \
        options.toolchain is Toolchain.Python else \
        llvm_toolchain_code_file_path(name) for name in code_file_names]
    return Project.compile(temp_dir, options, impl_opts, code_file_paths)
  else:
    return make_net_project(options.misc_opts,
        llvm_toolchain_code_file_path(code_file_names[0]),
        temp_dir,
        impl_opts,
        make_subdir=False)

def run_while_annotating_instructions(file_name, compile_options, inputs,
    outputs, no_of_values_to_wait_for, setup, must_annotate,
    after_step_visitor, max_step_i=None):
  chip = compile_test_program([file_name], compile_options).chip
  chip.prepare_for_simulation(inputs, outputs)
  model = chip.chip.instances[0].model
  instruction_str = [""]
  program_counter = [None]
  step_i = [None]

  setup(chip)

  def before_step(model):
    program_counter[0] = model.get_program_counter()
    step_i[0] = chip.step_i
    if must_annotate(program_counter[0], chip.step_i):
      instruction_str[0] = "{} {}".format(program_counter[0],
          disassemble(model.get_instruction()))
  def after_step(model):
    if must_annotate(program_counter[0], step_i[0]):
      string = "{}\t\t\t{}\t\t{}".format(
          instruction_str[0],
          dict((key, chips_c.bits_to_float(value) if value > 0 and value < 0 else value) 
            for key, value in model.get_registers().items()),
          #"")
          #model.get_memory())
          "")
      after_step_visitor(model, string, program_counter[0])
  def finished(chips_outputs):
    if max_step_i is not None and chip.step_i > max_step_i:
      return True
    return len(chips_outputs[outputs[0][1]]) >= no_of_values_to_wait_for

  for _ in chip.simulate(finished, [before_step], [after_step]):
    pass

def annotated_instructions(file_name, compile_options, inputs, outputs,
    no_of_values_to_wait_for):
  ret = []
  def setup(chip):
    ret.extend([None for _ in chip.instructions])
  def after_step(model, instruction_str, program_counter):
    if ret[program_counter] is None:
      ret[program_counter] = instruction_str

  run_while_annotating_instructions(file_name, compile_options, inputs,
      outputs, no_of_values_to_wait_for, setup,
      lambda program_counter, _: ret[program_counter] is None,
      after_step)
  return ret

def last_n_annotated_instructions(file_name, compile_options, inputs, outputs,
    no_of_values_to_wait_for, last_step_i, no_of_steps_to_annotate):
  ret = []
  first_step_i = last_step_i - no_of_steps_to_annotate
  def after_step(model, instruction_str, program_counter):
#    ret.append(instruction_str)
    print(instruction_str)

  run_while_annotating_instructions(file_name, compile_options, inputs,
      outputs, no_of_values_to_wait_for, lambda *_: None,
      lambda _, step_i: step_i > first_step_i,
      after_step, max_step_i=last_step_i)
  return ret

class MemorySimulation(object):
  def __init__(self, chip, simulation, memory_range):
    self.chip = chip
    self.sim = simulation
    self.memory_range = memory_range
    self.memory = dict()

  def run_until_change(self):
    while True:
      next(self.sim)
      memory = memory_range(self.chip.memory, self.memory_range)
      changed = self.memory != memory
      self.memory = memory
      if changed:
        break

  @property
  def program_counter(self):
    return self.chip.program_counter
  @property
  def registers(self):
    return self.chip.registers

def compare_memory_regions(code_file, reference_options,
    test_options, ref_range, test_range, inputs, outputs,
    no_of_values_to_wait_for):
  ref = compile_test_program([code_file], reference_options, inputs, outputs)
  test = compile_test_program([code_file], test_options, inputs, outputs)

  finished_predicate = lambda chips_outputs: \
      len(chips_outputs[outputs[0][1]]) >= no_of_values_to_wait_for
  
  ref_simulation = MemorySimulation(ref,
      ref.simulate(finished_predicate, [], []), ref_range)
  test_simulation = MemorySimulation(test,
      test.simulate(finished_predicate, [], []), test_range)
  while True:
    try:
      ref_simulation.run_until_change()
      test_simulation.run_until_change()

#      print("at {} after {} steps ({} ref)\n{}".format(
#        test_simulation.program_counter,
#        test_simulation.chip.step_i,
#        ref_simulation.chip.step_i,
#        test_simulation.memory))
#      print("  should be \n{}".format(ref_simulation.memory))
#      print("registers: {}".format(test_simulation.registers))

      if ref_simulation.memory != test_simulation.memory:
        print("at {} after {} steps ({} ref)\n{}".format(
          test_simulation.program_counter,
          test_simulation.chip.step_i,
          ref_simulation.chip.step_i,
          test_simulation.memory))
        print("  should be \n{}".format(ref_simulation.memory))
        print("registers: {}".format(test_simulation.registers))
        break
    except StopIteration:
      break

def pack_data(struct_format, instances):
  def get_byte(bytes, i):
    if i >= len(bytes):
      return 0
    return ord(bytes[i])

  ret = []
  for instance in instances:
    instance_in_bytes = struct.pack(struct_format, *instance)
    for i in range((len(instance_in_bytes) + 3) / 4):
      integer = 0
      for j in range(4):
        integer = (integer << 8) | get_byte(instance_in_bytes, 4 * i + j)
      ret.append(integer)
  return ret

def pack_u8s(numbers):
  return pack_data(">BBBB",
      zip(numbers[::4], numbers[1::4], numbers[2::4], numbers[3::4]))
def pack_u16s(numbers):
  numbers = [n & 0xffff for n in numbers]
  return pack_data(">HH", zip(numbers[::2], numbers[1::2]))

def assert_list_equal(test, value_type, list_1, list_2):
  #print(list_1, list_2, value_type)
  if value_type.is_floating_point:
    test.assertEqual(len(list_1), len(list_2))
    for x_1, x_2 in zip(list_1, list_2):
      test.assertAlmostEqual(x_1, x_2, delta=1e-2)
  elif value_type.is_integer:
    test.assertListEqual(list_1, list_2)
  else:
    assert False

def find_compiled_project_for_chip(chip_id):
  path = relative_file_path(os.path.join("..", "generated-models", "temp",
    chip_id))
  assert os.path.isdir(path), "no project for {}".format(chip_id)
  return path

#TODO remove
def run_vivado_testbench(chip, xpr_file_name, steps):
  assert distutils.spawn.find_executable("vivado") is not None, \
      "vivado must be in PATH"

  with tempfile.NamedTemporaryFile() as tcl_script:
    tcl_script.write("open_project \"{}\"\n".format(xpr_file_name))
    tcl_script.write("set_property top tb_file [get_fileset sim_1]\n")
#    tcl_script.write("launch_simulation\n")
#    tcl_script.write("reset_run impl_1\nlaunch_runs impl_1 -jobs 4\nwait_on_run impl_1\n")
    tcl_script.write(
        "launch_simulation -mode post-implementation -type functional\n")
    tcl_script.write("restart\n")
    #hack
    tcl_script.write("run {}ms\n".format(steps / 1e6 * 1.2))
    tcl_script.flush()
    subprocess.check_call(["vivado", "-mode", "batch",
      "-source", tcl_script.name])
#vivado -mode batch -source synthesize.tcl -tclargs "$xprPath"

def write_input_file(directory, port_name, data_type, values):
  with open(os.path.join(directory, "{}.dec".format(port_name)), "wb") as file:
    file.write(str(data_type) + "\n")
    for value in values:
      file.write(str(value) + "\n")

@contextlib.contextmanager
def temp_dir(prefix=""):
  path = tempfile.mkdtemp(prefix=prefix)
#  print("temp_dir is here: ", path)
  try:
    yield path
  finally:
    shutil.rmtree(path)

class EndToEndTest(unittest.TestCase):
  def assert_outputs(self, project, simulator, inputs, outputs, expected_output):
    output_types = dict((name, general_type_from_chips_type(chips_type)) for
        chips_type, name in outputs)
    stop_cond = OutputLengthCondition(dict((name,
        len(values) * (1 if output_types[name].length <= 32 else 2)) for
        name, values in expected_output.items()))

    with temp_dir("input") as input_dir:
      for name, (chips_type, values) in inputs.items():
        write_input_file(input_dir, name, general_type_from_chips_type(
          chips_type), values)

      result = project.run_simulation(simulator, input_dir, project.directory,
          stop_cond, output_types=output_types)

    for name, values in expected_output.items():
      assert_list_equal(self, output_types[name],
          list(result[name].numbers), values)
    return result

  def assert_chip_outputs_correct(self,
      compile_options, code_file_names, inputs, outputs, expected_output,
      test_verilog=True, test_in_chips_simulator=True, test_vivado=False,
      verilog_steps=100000):
    if compile_options.toolchain is Toolchain.Python:
      return
    if not test_verilog and not test_in_chips_simulator and not test_vivado:
      return
    project = compile_test_program(code_file_names, compile_options)

    if test_in_chips_simulator:
      self.assert_outputs(project, "chips", inputs, outputs, expected_output)
    if test_verilog:
      self.assert_outputs(project, "icarus", inputs, outputs, expected_output)
    if test_vivado:
      project.generate_vivado_project()
      self.assert_outputs(project, "vivado", inputs, outputs, expected_output)

  def assert_post_impl_outputs_correct(self, chip_id, inputs, outputs,
      expected_output, verilog_steps=100000):
    project_dir = find_compiled_project_for_chip(chip_id)
    project = Project.open(project_dir)
    self.assert_outputs(project, "post-impl", inputs, outputs, expected_output)


#TODO prevent chips simulation from repeating the input values

  def test_float_adder(self):
    file_name = ["float_adder.c"]
    inputs = dict(input_1=("float", [0.0, 1.0, 0.1, 5.0e3]))
    outputs = [("float", "output_1")]
    expected_output = dict(output_1=[1.0, 2.0, 1.1, 5.0e3 + 1.0])

    self.assert_chip_outputs_correct(CompileOptions(Toolchain.Python), file_name,
        inputs, outputs, expected_output)
    self.assert_chip_outputs_correct(CompileOptions(Toolchain.Llvm), file_name,
        inputs, outputs, expected_output)
    self.assert_chip_outputs_correct(
        CompileOptions(Toolchain.Llvm, opt_level=OptLevel(1)),
        file_name, inputs, outputs, expected_output)
    self.assert_chip_outputs_correct(
        CompileOptions(Toolchain.Llvm, opt_level=OptLevel(3)),
        file_name, inputs, outputs, expected_output)

  def test_long_adder(self):
    file_name = ["adder_with_long.c"]
    inputs = dict(input_1=("long", [0, 3, 2**50]))
    outputs = [("long", "output_1")]
    expected_output = dict(output_1=[2, 5, 2**50 + 2])

    self.assert_chip_outputs_correct(CompileOptions(Toolchain.Python), file_name,
        inputs, outputs, expected_output)
    self.assert_chip_outputs_correct(CompileOptions(Toolchain.Llvm), file_name,
        inputs, outputs, expected_output)
    self.assert_chip_outputs_correct(
        CompileOptions(Toolchain.Llvm, OptLevel(3)),
        file_name, inputs, outputs, expected_output)

  def test_double_adder(self):
    file_name = ["adder_with_double.c"]
    inputs = dict(input_1=("double", [0.0, -1.0, 5.1, 6e15]))
    outputs = [("double", "output_1")]
    expected_output = dict(output_1=[1.0, 0.0, 6.1, 6e15 + 1.0])

#    self.assert_chip_outputs_correct(CompileOptions(Toolchain.Python), file_name,
#        inputs, outputs, expected_output)
#    self.assert_chip_outputs_correct(LlvmO0, file_name,
#        inputs, outputs, expected_output)
    self.assert_chip_outputs_correct(LlvmO3, file_name,
        inputs, outputs, expected_output)

  def test_hls_sqrt(self):
    file_name = ["hls_sqrt.c"]
    inputs = dict(input_1=("float", [0.0, 9.0, 2.0]))
    outputs = [("float", "output_1")]
    expected_output = dict(output_1=[0.0, 3.0, math.sqrt(2.0)])

    self.assert_chip_outputs_correct(CompileOptions(Toolchain.Python), file_name,
        inputs, outputs, expected_output, verilog_steps=1000000)
#    self.assert_chip_outputs_correct(CompileOptions(Toolchain.Llvm), file_name,
#        inputs, outputs, expected_output)
#    self.assert_chip_outputs_correct(Toolchain.Llvm, file_name,
#        inputs, outputs, expected_output, opt_level=OptLevel(3))

  def test_float_sqrt(self):
    file_name = ["float_sqrt.c"]
    inputs = dict(input_1=("float", [0.0, 9.0, 2.0]))
    outputs = [("float", "output_1")]
    expected_output = dict(output_1=[0.0, 3.0, math.sqrt(2.0)])

    self.assert_chip_outputs_correct(CompileOptions(Toolchain.Python), file_name,
        inputs, outputs, expected_output, verilog_steps=1000000)
    self.assert_chip_outputs_correct(CompileOptions(Toolchain.Llvm), file_name,
        inputs, outputs, expected_output)
    #TODO setune
#    self.assert_chip_outputs_correct(Toolchain.Llvm, file_name,
#        inputs, outputs, expected_output, opt_level=OptLevel(3))

  def test_pointers_and_recursion(self):
    file_name = ["pointers_and_recursion.c"]
    input_values = [1, 4, 
        1, 2, 3, 4]

    inputs = dict(input_1=("int", input_values))
    outputs = [("int", "output_1")]

    expected_output = dict(output_1=[2, 3, 4, 5])

    self.assert_chip_outputs_correct(
        CompileOptions(Toolchain.Llvm, stack_size=500), file_name,
        inputs, outputs, expected_output)
    self.assert_chip_outputs_correct(
        CompileOptions(Toolchain.Llvm, opt_level=OptLevel(3),
          stack_size=300),
        file_name, inputs, outputs, expected_output)

  def test_test(self):
    file_name = ["test.c"]

    inputs = dict()
    outputs = [("int", "output_1")]

    expected_output = dict(output_1=[0])

#TODO rename -> global_constant.c
#    self.assert_chip_outputs_correct(CompileOptions(Toolchain.Python), file_name,
#        inputs, outputs, expected_output, verilog_steps=10000000, test_verilog=False)
    self.assert_chip_outputs_correct(CompileOptions(Toolchain.Llvm), file_name,
        inputs, outputs, expected_output,
        verilog_steps=10000000, test_verilog=False)
    self.assert_chip_outputs_correct(
        CompileOptions(Toolchain.Llvm, opt_level=OptLevel(3)), file_name,
        inputs, outputs, expected_output,
        verilog_steps=10000000, test_verilog=False)

  def test_struct(self):
    #TODO rename
    file_name = ["test2.c"]
    input_values = [50, 1, 2]

    inputs = dict(input_1=("int", input_values))
    outputs = [("int", "output_1")]

    expected_output = dict(output_1=[1])
    stack_size = 2100

    #TODO test verilog
    self.assert_chip_outputs_correct(
        CompileOptions(Toolchain.Llvm, stack_size=stack_size),
        file_name, inputs, outputs, expected_output, test_verilog=False)
    self.assert_chip_outputs_correct(
        CompileOptions(Toolchain.Llvm, opt_level=OptLevel(3),
          stack_size=stack_size),
        file_name, inputs, outputs, expected_output,
        verilog_steps=10000000, test_verilog=False)

  def test_struct_copy(self):
    file_name = ["struct_copy.c"]
    input_values = [0, 1, 2, 3, 4, 5]

    inputs = dict(input_1=("int", input_values))
    outputs = [("int", "output_1")]

    expected_output = dict(output_1=[1, 2, 3, 4, 5])

    self.assert_chip_outputs_correct(CompileOptions(Toolchain.Llvm), file_name,
        inputs, outputs, expected_output, test_verilog=False)
    self.assert_chip_outputs_correct(
        CompileOptions(Toolchain.Llvm, opt_level=OptLevel(3)), file_name,
        inputs, outputs, expected_output,
        test_verilog=False)

  def test_simple_net(self):
    file_name = ["simple_net.py"]
    inputs = dict(input_1=("int", [0]))
    outputs = [("int", "output_1")]
    expected_output = dict(output_1=[4])
    opts = LlvmO3

    self.assert_chip_outputs_correct(opts, file_name,
        inputs, outputs, expected_output, test_in_chips_simulator=False)

    #TODO use option "all"
  def test_jpeg_decoder(self):
    no_of_frames = 30
#    no_of_frames = 1

    file_name = ["jpeg_decoder.py"]
    inputs = dict()
    inputs[default_input_name(1)] = ("int", read_hex_file(
        jpeg_data("in_img_splitter.hex")) * no_of_frames)
    outputs = default_name_outputs(1)

    output_numbers = read_hex_file(jpeg_data("in_combine.hex")) * \
        (no_of_frames - 5)
    expected_output = dict()
    expected_output[outputs[0][1]] = output_numbers
    expected_output_packed = dict()
    expected_output_packed[outputs[0][1]] = pack_u8s(output_numbers)
    steps = 550000 * no_of_frames

    def test_post_impl(options):
      chip_id = generic_chip_id("jpeg_decoder", options)
      self.assert_post_impl_outputs_correct(chip_id, inputs, outputs,
          expected_output, steps)

#    for opt_level in ["O3", "Os"]:
    for opt_level in ["O3"]:
#      for config in ["dist", "all", "external_idct", "double_idct", "dist_double_idct"]:
      for config in ["dist"]:
#        options = [opt_level, "fifo"]
        options = [opt_level]
        options.append(config)
        compile_opts = LlvmO3.with_misc_opts(*options)
        if TestOnlyWithVivado and config is None:
          test_post_impl(options)
        else:
          self.assert_chip_outputs_correct(compile_opts, file_name,
              inputs, outputs,
              expected_output if False else #config == "dist" else
                expected_output_packed,
              test_in_chips_simulator=False, verilog_steps=steps,
#              test_verilog=False, test_vivado=True)
)

    if TestOnlyWithVivado:
      test_post_impl(["O3", "fifo"])

#  def test_jpeg_decoder_steady_state(self):
#    no_of_blocks = 25
#    file_name = ["jpeg_decoder.py"]
#    inputs = dict()
#    inputs[default_input_name(1)] = ("int", no_of_blocks * read_hex_file(
#        jpeg_data("in_img_splitter.hex")))
#    outputs = default_name_outputs(1)
#    expected_output = dict()
#    expected_output[outputs[0][1]] = no_of_blocks * read_hex_file(
#        jpeg_data("in_combine.hex"))
#    steps = no_of_blocks * 550000
#
#    options = ["Os"]
#    compile_opts = LlvmO3.with_misc_opts(*options)
#    self.assert_chip_outputs_correct(compile_opts, file_name,
#        inputs, outputs, expected_output, test_in_chips_simulator=False,
#        verilog_steps=steps)

  def test_should_support_asynchronous_io_instructions(self):
    inputs = dict(
        input_1=("int", list(range(5000))),
        input_2=("int", [0] * 4999 + [25]))
    outputs = default_name_outputs(3, with_arg=False)
    expected_output = dict(
        output_1=[25],
        output_2=list(range(5000)))
    program = compile_test_program(["asyncio.c"],
        LlvmO3.with_no_memory_initialization())
    result = self.assert_outputs(program, "icarus", inputs, outputs,
        expected_output)

    difference = max(
        result["output_1"].cycles,
        result["output_2"].cycles) - 10000
    assert difference < 1000 and difference > 0

#TODO make fail again
#  def test_test3(self):
#    file_name = "test3.c"
#    input_values = []
#    for i in range(255):
#      input_values.append(i)
#    input_values.append(256.0)
#
#    inputs = dict(input_1=("float", input_values))
#    outputs = [("float", "output_1")]
#
#    fft = np.fft.fft(input_values)
#    output_values = []
#    for i in range(256):
#      output_values.append(fft[i].real)
#      output_values.append(fft[i].imag)
#    expected_output = dict(output_1=output_values)
#
#    self.assert_chip_outputs_correct(Toolchain.Llvm, file_name,
#        inputs, outputs, expected_output, opt_level=OptLevel(3),
#        verilog_steps=10000000, test_verilog=False)

#  def test_test4(self):
#    file_name = "test4.c"
#    input_values = []
#    for i in range(255):
#      input_values.append(i)
#    input_values.append(256)
#
#    inputs = dict(input_1=("int", input_values))
#    outputs = [("int", "output_1")]
#
#    fft = np.fft.fft(input_values)
#    output_values = []
#    for i in range(256):
#      output_values.append(fft[i].real)
#      output_values.append(fft[i].imag)
#    expected_output = dict(output_1=output_values)
#
#    self.assert_chip_outputs_correct(Toolchain.Llvm, file_name,
#        inputs, outputs, expected_output, opt_level=OptLevel(1),
#        verilog_steps=10000000, test_verilog=False)

TestOnlyWithVivado = False

class TestRunConfig(object):
  def __init__(self, compilation_config, runs, steps=100000):
    self.compilation_config = compilation_config
    self.runs = runs
    self.steps = steps

class TestFunction(object):
  def __init__(self, sources, inputs, outputs, test_run_configs,
      test_values, name=None):
    if isinstance(sources, str):
      self.sources = [sources]
    else:
      self.sources = sources
    self.inputs = inputs
    self.outputs = outputs
    self.test_run_configs = test_run_configs
    self.test_values = test_values
    self.name = name

  def description(self):
    return "test_{}".format(self.name or
        os.path.splitext(os.path.basename(self.sources[0]))[0])

  def func(self):
    def ret(end_to_end_test):
      for test_run in self.test_run_configs:
        for test_values in self.test_values:
          if callable(test_values):
            test_values = test_values()
          inputs = dict()
          expected_output = dict()
          for (value_type, name) in self.inputs:
            inputs[name] = (value_type, test_values[name])
          for (_, name) in self.outputs:
            expected_output[name] = test_values[name]
          
          test_verilog = Icarus in test_run.runs
          test_chips = ChipsSimulator in test_run.runs
          test_vivado = Vivado in test_run.runs
          test_post_impl = PostImpl in test_run.runs

          end_to_end_test.assert_chip_outputs_correct(
              test_run.compilation_config,
              self.sources,
              inputs, self.outputs, expected_output,
              test_verilog=test_verilog and not TestOnlyWithVivado,
              test_in_chips_simulator=test_chips and not TestOnlyWithVivado,
              test_vivado=test_vivado and TestOnlyWithVivado,
              verilog_steps=test_run.steps)
          if PostImpl in test_run.runs and TestOnlyWithVivado:
            end_to_end_test.assert_post_impl_outputs_correct(
                compilation_id(self.sources[0], test_run.compilation_config),
                inputs, self.outputs, expected_output, test_run.steps)


    return ret

FastTestRuns = [ChipsSimulator, Icarus]
AdHocTestRuns = FastTestRuns + [Vivado]
AllTestRuns = FastTestRuns + [PostImpl, Vivado]

#TODO remove
def hex_extend(line, signed=False):
  extension = "F" if signed and int(line[0], base=16) >= 8 else "0"
  return extension * (8 - len(line)) + line

def read_hex_file(path, signed=False):
  with open(path, "r") as hex_file:
    return [struct.unpack(">I", 
      binascii.unhexlify(hex_extend(line[:-1], signed=signed)))[0] 
      for line in hex_file]

def read_decimal_file(path):
  ret = []
  with open(path, "r") as dec_file:
    for line in dec_file:
      if line.startswith("#"):
        continue
      ret.extend([int(word) for word in line.split()])
  return ret

def read_binary_file(path):
  ret = []
  with open(path, "rb") as bin_file:
    while True:
      buf = bin_file.read(4)
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

def generate_fft_values():
  ret = dict()
  input_values = []
  for i in range(255):
    input_values.append(i)
  input_values.append(256.0)

  ret["input_1"] = input_values

  fft = np.fft.fft(input_values)
  output_values = []
  for i in range(256):
    output_values.append(fft[i].real)
    output_values.append(fft[i].imag)

  ret["output_1"] = output_values
  return ret

def generate_splitter_values():
    ret = dict()
    ret[default_input_name(1)] = read_hex_file(jpeg_data("in_img_splitter.hex"))
    for i, channel in enumerate(["Y", "Cb", "Cr"]):
        ret[default_output_name(i + 1)] = read_hex_file(jpeg_data(
            "in_{}_rlc_decoder_inv_zrl.hex".format(channel)))
    return ret

def generate_idct_values(with_arg=DefaultWithArg):
  ret = dict()
#  ret["input_1"] = 64 * [0]
#  ret["input_1"][0] = 5
#  ret["output_1"] = 64 * [0]
#  ret[default_input_name(1, with_arg=with_arg)] = pack_u16s(read_hex_file(
#      relative_file_path("inv_quant_idct_data_in.hex"), signed=True) * 20)
#  ret[default_output_name(1, with_arg=with_arg)] = pack_u8s(read_hex_file(
#      relative_file_path("inv_quant_idct_data_out.hex")) * 15)
  ret[default_input_name(1, with_arg=with_arg)] = read_hex_file(
      relative_file_path("inv_quant_idct_data_in.hex"), signed=True)
  ret[default_output_name(1, with_arg=with_arg)] = read_hex_file(
      relative_file_path("inv_quant_idct_data_out.hex"))
  return ret

def generate_zrl_values(**kwargs):
  ret = dict()
  ret[default_input_name(1, **kwargs)] = read_hex_file(relative_file_path(
    "rlc_decoder_inv_zrl_data_in.hex"))
  ret[default_output_name(1, **kwargs)] = read_hex_file(relative_file_path(
    "rlc_decoder_inv_zrl_data_out.hex"), signed=True)
  return ret

def generate_zigzag_values(**kwargs):
  ret = dict()
  ret[default_input_name(1, **kwargs)] = read_hex_file(
      jpeg_data("in_inv_zigzag.hex"), signed=True)
  ret[default_output_name(1, **kwargs)] = read_hex_file(
      jpeg_data("in_inv_quant_idct.hex"), signed=True)
  return ret

def generate_colorconv_values(with_arg=DefaultWithArg):
  ret = dict()
  ret[default_input_name(1, with_arg=with_arg)] = 64 * [123]
  ret[default_input_name(2, with_arg=with_arg)] = 64 * [91]
  ret[default_input_name(3, with_arg=with_arg)] = 64 * [175]
  ret[default_output_name(1, with_arg=with_arg)] = \
      64 * [200] + 64 * [101] + 64 * [50]
  return ret

def generate_sort_values():
  ret = dict()
  ret["input_1"] = [10, 9, 8, 7, 6, 5, 4, 2, 1, 4]
  ret["output_1"] = [1, 2, 4, 4, 5, 6, 7, 8, 9, 10]
  return ret

def generate_geom_values():
  ret = dict()
  ret[default_input_name(1)] = read_hex_file(
      relative_file_path("geom-data/input1/0.hex"), signed=True)
  ret[default_input_name(2)] = read_hex_file(
      relative_file_path("geom-data/input2/0.hex"), signed=True)
  ret[default_input_name(3)] = read_hex_file(
      relative_file_path("geom-data/input3/0.hex"))
  ret[default_output_name(1)] = read_hex_file(
      relative_file_path("geom-data/output/0.hex"))
  return ret

def compute_double_buffer_results():
  #TODO * 10
  input_1 = list(range(50)) * 10
  input_2 = list(range(50, 100)) * 10
  return dict(
      input_1=input_1,
      input_2=input_2,
      output_1=[a + b for a, b in zip(input_1[::2], input_2[::2])],
      output_2=[(a - b + i % 25) & 0xffffffff for i, (a, b) in
        enumerate(zip(input_1[1::2], input_2[1::2]))])

def get_jpeg_decoder_data(packed):
  ret = dict()
  ret[default_input_name(1)] = read_hex_file(jpeg_data("in_img_splitter.hex"))
  
  output_numbers = read_hex_file(jpeg_data("in_combine.hex"))

  if not packed:
    ret[default_output_name(1)] = output_numbers
  else:
    ret[default_output_name(1)] = pack_data(">BBBB",
        zip(output_numbers[::4], output_numbers[1::4],
          output_numbers[2::4], output_numbers[3::4]))

  return ret

TestFunctions = [
  TestFunction("int_adder.c",
    [("int", "input_1"), ("int", "input_2")],
    [("int", "output_1")],
    [
      TestRunConfig(Python.with_stack_size(8), FastTestRuns),
      TestRunConfig(LlvmO0, FastTestRuns),
      TestRunConfig(LlvmO3, AllTestRuns)],
    [dict(input_1=[1, 2, 3], input_2=[0, 4, 5], output_1=[1, 6, 8])]),
  TestFunction("fft/hls_fft.c",
    [("float", "input_1")],
    [("float", "output_1")],
    [
      TestRunConfig(Python.with_stack_size(7315), FastTestRuns,
        steps=10000000),
      TestRunConfig(LlvmO0.with_stack_size(8400), FastTestRuns,
        steps=10000000),
      TestRunConfig(LlvmO1.with_stack_size(8400), FastTestRuns,
        steps=5000000),
      TestRunConfig(LlvmO3.with_stack_size(4400), FastTestRuns,
        steps=3000000)
      ],
    [generate_fft_values], name="fft_changed"),
  TestFunction("fft.original/fft.c",
    [("float", "input_1")],
    [("float", "output_1")],
    [
      TestRunConfig(LlvmO0.with_stack_size(8400), FastTestRuns,
        steps=10000000),
      TestRunConfig(LlvmO1.with_stack_size(8400), FastTestRuns,
        steps=5000000),
      TestRunConfig(LlvmO3, AllTestRuns, steps=3000000)],
    [generate_fft_values], name="fft_orig"),
  TestFunction("quicksort.c",
    [("int", "input_1")],
    [("int", "output_1")],
    [
      TestRunConfig(Python.with_stack_size(224), FastTestRuns),
      TestRunConfig(LlvmO0, FastTestRuns),
      TestRunConfig(LlvmOs.with_stack_size(200), AllTestRuns),
      TestRunConfig(LlvmO3.with_stack_size(116), FastTestRuns)],
    [generate_sort_values]),
  TestFunction("quicksort8.c",
    [("int", "input_1")],
    [("int", "output_1")],
    [
      TestRunConfig(LlvmO0, FastTestRuns),
      TestRunConfig(LlvmOs.with_stack_size(140), AllTestRuns),
      TestRunConfig(LlvmO3.with_stack_size(92), FastTestRuns)],
    [generate_sort_values]),
  TestFunction("quicksort16.c",
    [("int", "input_1")],
    [("int", "output_1")],
    [
      TestRunConfig(LlvmO0, FastTestRuns),
      TestRunConfig(LlvmOs.with_stack_size(158), AllTestRuns),
      TestRunConfig(LlvmO3.with_stack_size(92), FastTestRuns)],
    [generate_sort_values]),
  TestFunction("malloc.c",
    [("int", "input_1")],
    [("int", "output_1")],
    [
#        TestRunConfig(Python, FastTestRuns),
      TestRunConfig(LlvmO1.with_heap_size(800), FastTestRuns),
      TestRunConfig(LlvmO3.with_heap_size(800), FastTestRuns)],
    [dict(input_1=[1, 3, 1, 4, 1, 5, 1, 6, 0], output_1=[3, 4, 5, 6])]),
  TestFunction("strings.c",
    [("int", "input_1")],
    [("int", "output_1")],
    [
#        TestRunConfig(Python, FastTestRuns),
#        TestRunConfig(LlvmO0, FastTestRuns),
      TestRunConfig(LlvmO1.with_heap_size(100), FastTestRuns),
      TestRunConfig(LlvmO3.with_heap_size(100), FastTestRuns)],
    [dict(
      input_1=flatten([[len(string)] + [ord(c) for c in string] for string in
        ["foo\0", "bar\0", "blah\0"]]),
      output_1=[ord(c) for c in "abc\0fbboalora\0"])]),
  TestFunction("scale_8_16_bit.c",
    [("int", "input_1")],
    [("int", "output_1")],
    [
      TestRunConfig(LlvmO0, FastTestRuns),
      TestRunConfig(LlvmO3, FastTestRuns)],
    [dict(
      input_1=[-5] + pack_data(">BBBB", [[52, 55, 61, 66], [70, 61, 64, 73]]),
      output_1=pack_data(">hh", [[-81 + -78, -72 + -67],
        [-63 + -72, -69 + -60]]))]),
  TestFunction("fixed_point.cpp",
    [("int", "input_1")],
    [("int", "output_1")],
    [
      TestRunConfig(LlvmO0, FastTestRuns),
      TestRunConfig(LlvmO3, FastTestRuns)],
    [dict(
      input_1=pack_data(">bBbB", [
        [1, 0, 100, 0.5 * 256],
        [-10, 0.4 * 256, 4, 0.25 * 256]]),
      output_1=pack_data(">xxbB", [[100, 0.5 * 256], [59, 0.695 * 256]]))]),

  TestFunction("img_splitter/mdl_img_splitter_72_hw.cpp",
    default_name_inputs(1, with_arg=False),
    default_name_outputs(3, with_arg=False),
    [
      TestRunConfig(LlvmO3.with_stack_size(600), FastTestRuns, steps=145000),
      TestRunConfig(LlvmOs.with_stack_size(390), FastTestRuns, steps=145000),
    ],
    [dict(
      input_1=[1] + 14 * [0] + [1] + 14 * [0] + [1] + 14 * [0],
      output_1=[1] + 14 * [0],
      output_2=[1] + 14 * [0],
      output_3=[1] + 14 * [0])]),
  TestFunction(
      "mdl_rlc_decoder_inv_zrl1_251_hw/mdl_rlc_decoder_inv_zrl1_251_hw.cpp",
    default_name_inputs(1, with_arg=False),
    default_name_outputs(1, with_arg=False),
    [
      TestRunConfig(LlvmO0.with_stack_size(1000), FastTestRuns, steps=400000),
      TestRunConfig(LlvmO3.with_stack_size(250), FastTestRuns, steps=145000)
    ],
    [lambda: generate_zrl_values(with_arg=False)]),
#      [dict(
#        input_1=[0xf01f01f0, 0x1f010000] + 13 * [0],
#        output_1=64 * [0])])

  TestFunction("inv_zigzag/mdl_inv_zigzag_196_hw.cpp",
    default_name_inputs(1, with_arg=False),
    default_name_outputs(1, with_arg=False),
    [
      TestRunConfig(LlvmOs.with_stack_size(350), FastTestRuns, steps=145000),
      TestRunConfig(LlvmO3.with_stack_size(350), FastTestRuns, steps=145000),
    ],
    [lambda: generate_zigzag_values(with_arg=False)]),

  TestFunction("mdl_inv_quant_idct1_130_hw/mdl_inv_quant_idct1_130_hw.cpp",
    default_name_inputs(1, with_arg=False),
    default_name_outputs(1, with_arg=False),
    [
      TestRunConfig(Python.with_stack_size(900), FastTestRuns, steps=10000000),
      #TestRunConfig(LlvmOs.with_stack_size(900), FastTestRuns, steps=350000),
#        TestRunConfig(LlvmO3.with_stack_size(450), FastTestRuns, steps=350000),
#        TestRunConfig(LlvmO3.with_stack_size(1000), FastTestRuns, steps=145000),
#        TestRunConfig(LlvmOs.with_stack_size(800), FastTestRuns, steps=145000),
    ],
    [lambda: generate_idct_values(with_arg=False)]),

  TestFunction("idct.orig/idct_py.cpp",
    default_name_inputs(1),
    default_name_outputs(1),
    [
      TestRunConfig(Python.with_stack_size(900), AllTestRuns, steps=1800000),
    ],
    [generate_idct_values]),

  TestFunction("mdl_colorspaceconv_8_hw/mdl_colorspaceconv_8_hw.cpp",
    default_name_inputs(3, with_arg=False),
    default_name_outputs(1, with_arg=False),
    [
      TestRunConfig(LlvmO0.with_stack_size(1000), FastTestRuns, steps=400000),
      TestRunConfig(LlvmO3.with_stack_size(740), FastTestRuns, steps=145000),
      TestRunConfig(LlvmOs.with_stack_size(800), FastTestRuns, steps=145000),
    ],
    [lambda: generate_colorconv_values(with_arg=False)]),

  TestFunction("large_memory.c",
    [("int", "input_1")],
    [("int", "output_1")],
    [
#        TestRunConfig(LlvmO0.with_misc_opts("-DSIZE=70000"),
#          [ChipsSimulator]),
#        TestRunConfig(LlvmO3.with_misc_opts("-DSIZE=70000"),
#          FastTestRuns, steps=15000000),
      TestRunConfig(LlvmO3.with_misc_opts("-DSIZE=70000"), [ChipsSimulator]),
      TestRunConfig(LlvmO3.with_misc_opts("-DSIZE=16000"),
        FastTestRuns, steps=5000000),
    ],
    [dict(
      input_1=[1, 2],
      output_1=[1, 2])]),

  TestFunction("stack_size.c",
    [("int", "input_1")],
    [("int", "output_1")],
    [
      TestRunConfig(LlvmO0, FastTestRuns),
#        TestRunConfig(LlvmO3, FastTestRuns)
    ],
    [dict(
      input_1=[5],
      output_1=[21])]),

  TestFunction("dijkstra/dijkstra.c",
    [("int", "input_1")],
    [("int", "output_1")],
    [
#takes about 2 minutes
#        TestRunConfig(LlvmO3
#          .with_stack_size(185)
#          .with_heap_size(7000), [ChipsSimulator]), #6500
#takes about 5 minutes
#        TestRunConfig(LlvmO3
#          .with_stack_size(185)
#          .with_heap_size(7000), [Icarus], steps=19000000),

#        TestRunConfig(Python.with_stack_size(50000), FastTestRuns, steps=1529000),
    ],
    [dict(
      input_1=read_decimal_file(relative_file_path("dijkstra_input")),
      output_1=read_decimal_file(relative_file_path("dijkstra_output")))]),

  TestFunction([
    "stringsearch/pbmsrch_small.c"],
#      "stringsearch/bmhasrch.c",
#      "stringsearch/bmhisrch.c",
#      "stringsearch/bmhsrch.c"],
    [("int", "input_1")],
    [("int", "output_1")],
    [
      TestRunConfig(LlvmO3, FastTestRuns, steps=1529000),
#        TestRunConfig(Python.with_stack_size(50000), FastTestRuns, steps=1529000),
    ],
    [dict(
      input_1=[0],
      output_1=read_decimal_file(relative_file_path(
        "stringsearch_small_output")))], name="stringsearch"),

  TestFunction([
    "aes/aesxam.c",
    "aes/aes.c"],
    [("int", "input_1")],
    [("int", "output_1")],
    [
      TestRunConfig(LlvmO3.with_misc_opts("-w"),
        FastTestRuns, steps=11529000),
#        TestRunConfig(LlvmO0.with_misc_opts("-w"),
#          FastTestRuns, steps=21529000),
    ],
    [dict(
      #input produced by truncating the MiBench small input to 5000 bytes
      input_1=read_hex_file(relative_file_path("aes_key")) +
          read_binary_file(relative_file_path("aes_input")),
      output_1=read_binary_file(relative_file_path("aes_output")))],
    name="aes"),

  TestFunction("jpegDecoder_chip/" +
      "mdl_img_splitter_66_hw/mdl_img_splitter_66_hw.cpp",
    default_name_inputs(1),
    default_name_outputs(3),
    [
#        TestRunConfig(LlvmO0, AllTestRuns, steps=1150000),
#        TestRunConfig(LlvmO3, FastTestRuns, steps=9000),
      TestRunConfig(LlvmO0, FastTestRuns, steps=1150000),
      TestRunConfig(LlvmO3, AllTestRuns, steps=9000),
      TestRunConfig(LlvmOs, FastTestRuns, steps=13500),
    ],
    [generate_splitter_values], name="splitter"),
  TestFunction("jpegDecoder_chip/" +
      "mdl_rlc_decoder_inv_zrl1_233_hw/mdl_rlc_decoder_inv_zrl1_233_hw.cpp",
    default_name_inputs(1),
    default_name_outputs(1),
    [
      TestRunConfig(LlvmO3, AdHocTestRuns, steps=54000),
      TestRunConfig(LlvmOs, AdHocTestRuns, steps=54000)
    ],
    [generate_zrl_values], name="zrl"),
##      [dict(
##        input_1=[0xf01f01f0, 0x1f010000] + 13 * [0],
##        output_1=64 * [0])])
  TestFunction("jpegDecoder_chip/" +
      "mdl_inv_zigzag1_200_hw/mdl_inv_zigzag1_200_hw.cpp",
    default_name_inputs(1),
    default_name_outputs(1),
    [
      TestRunConfig(LlvmOs, AdHocTestRuns, steps=19000),
      TestRunConfig(LlvmO3, AdHocTestRuns, steps=19000),
    ],
    [generate_zigzag_values], name="zigzag"),
  TestFunction("jpegDecoder_chip/" +
      "mdl_inv_quant_idct1_126_hw/mdl_inv_quant_idct1_126_hw.cpp",
    default_name_inputs(1),
    default_name_outputs(1),
    [
      TestRunConfig(LlvmOs, FastTestRuns, steps=290000),
      TestRunConfig(LlvmO3, [Icarus], steps=270000),
    ],
    [generate_idct_values], name="idct"),
  TestFunction("jpegDecoder_chip/" +
      "mdl_colorspaceconv_4_hw/mdl_colorspaceconv_4_hw.cpp",
    default_name_inputs(3),
    default_name_outputs(1),
    [
#        TestRunConfig(LlvmO0.with_stack_size(1000), FastTestRuns, steps=320000),
      TestRunConfig(LlvmO3, AdHocTestRuns, steps=125000),
      TestRunConfig(LlvmOs, AdHocTestRuns, steps=125000),
    ],
    [generate_colorconv_values], name="cconv"),

#  TestFunction([
#    "rsa/mini_rsa_keygen.c",
#    "rsa/mini-gmp.c"],
#    [("int", "input_1")],
#    [("int", "output_1")],
#    [
#      TestRunConfig(LlvmO3, FastTestRuns, steps=11529000),
#    ],
#    [dict(
#      input_1=[0],
#      output_1=[0])],
#    name="rsa"),

#  TestFunction("echo.c",
#    [("int", "input_1")],
#    [("int", "output_1")],
#    [
#      TestRunConfig(LlvmO3, FastTestRuns, steps=100),
#    ],
#    [dict(
#      input_1=[0, 1, 2, 3],
#      output_1=[0, 1, 2, 3])], name="rsa"),

#    TestFunction([
#        "mdl_estgeomtrans0_380_hw/mdl_estgeomtrans0_380_hw.cpp",
#        "mdl_estgeomtrans0_380_hw/svd_r_rt.cpp",
#        "mdl_estgeomtrans0_380_hw/data_types.cpp"],
#        #"mdl_estgeomtrans0_380_hw/svd_helper_rt.cpp"],
#      default_name_inputs(3),
#      default_name_outputs(1),
#      [
#        TestRunConfig(LlvmO0.with_stack_size(50000), FastTestRuns, steps=125000)
#      ],
#      [generate_geom_values], name="geom"),
#
#    TestFunction("soft_float.c",
#      [("float", "input_1")],
#      [("float", "output_1")],
#      [
#        TestRunConfig(LlvmO3, FastTestRuns)
#      ],
#      [dict(input_1=[1.0, -3.0, 2.0], output_1=[2.0, -2.0, 2.0])]),
#
#    TestFunction(["failureno_test.c", "failureno_test2.c",
#      "../../toolchain/Chips-2.0-master/chips/compiler/include/failureno.c"],
#      [("int", "input_1")],
#      [("int", "output_1")],
#      [
#        TestRunConfig(LlvmO0, FastTestRuns)
#      ],
#      [dict(input_1=[0], output_1=[0, 0])])

  TestFunction("long_plus_double.c",
    [("long", "input_1")],
    [("double", "output_1")],
    [
      TestRunConfig(LlvmO0, FastTestRuns),
      TestRunConfig(LlvmO3, FastTestRuns)
    ],
    [dict(input_1=[1], output_1=[2.5])]),

  TestFunction("double_plus_long.c",
    [("double", "input_1")],
    [("long", "output_1")],
    [
      TestRunConfig(LlvmO0, FastTestRuns),
      TestRunConfig(LlvmO3, FastTestRuns)
    ],
    [dict(input_1=[2e12], output_1=[3e12])]),

  TestFunction("conversions.c",
    [("int", "input_1"),
      ("float", "input_2"),
      ("long", "input_3")],
    [("long", "output_1"),
      ("float", "output_2")],
    [
      TestRunConfig(LlvmO0, FastTestRuns),
      TestRunConfig(LlvmO3, FastTestRuns)
    ],
    [dict(input_1=[1], input_2=[2], input_3=[3], output_1=[3],
      output_2=[5])]),

  TestFunction("loads.c",
    [],
    [("int", "output_1")],
    [
      TestRunConfig(LlvmO3, FastTestRuns)
    ],
    [dict(output_1=[3])]),

  TestFunction("double_buffer.c",
    [("int", "input_1"), ("int", "input_2")],
    [("int", "output_1"), ("int", "output_2")],
    [
      TestRunConfig(LlvmO3, [Icarus, Vivado]),
      TestRunConfig(LlvmO0, [Icarus]),
      TestRunConfig(LlvmOs, [Icarus])
    ],
    [compute_double_buffer_results]),

  TestFunction("jpegDecoder_chip/main_all.cpp",
    [("int", "inputArg1")],
    [("int", "outputArg1")],
    [
      TestRunConfig(LlvmO3.with_misc_opts("-DIO=SIMPLE"), [ChipsSimulator])
    ],
    [lambda: get_jpeg_decoder_data(True)], name="jpeg_all")


#    TestFunction("multiplication.c",
#      [("int", "input_1")],
#      [("int", "output_1")],
#      [
#        TestRunConfig(LlvmO3.with_stack_size(1000), FastTestRuns, steps=400000),
##        TestRunConfig(LlvmO3.with_stack_size(1000), FastTestRuns, steps=145000),
##        TestRunConfig(LlvmOs.with_stack_size(800), FastTestRuns, steps=145000),
#      ],
#      [dict(input_1=[-1, 1], output_1=[-1])])
]

for test_func in TestFunctions:
  setattr(EndToEndTest, test_func.description(), test_func.func())

if __name__ != "__main__":
  import_chips()
else:
  if len(sys.argv) > 1 and sys.argv[1] == "only-vivado":
    TestOnlyWithVivado = True
    del sys.argv[1]
  else:
    import_chips()

if len(sys.argv) > 1 and sys.argv[1] == "custom":
  file_name = sys.argv[2]

#    for instr in annotated_instructions(file_name,
#        CompileOptions(Toolchain.Python),
#        dict(input_1=("int", [42, 4, 1, 2, 3, 4, 5, 6, 7])),
#        [("int", "output_1")], 1):
#      print(instr)
#    file_name = "malloc.c"
#    for instr in last_n_annotated_instructions(file_name,
#        LlvmO1,
#        dict(input_1=("int", [1, 3, 1, 4, 1, 5, 1, 6, 0])),
#        [("int", "output_1")], 1, 3000, 3000): #74090
#      print(instr)
  outputs = [("int", "output_1")]
  inputs = dict(input_1=("int", read_decimal_file(relative_file_path("dijkstra_input"))))
  file_name = "dijkstra/dijkstra.c"
  for instr in last_n_annotated_instructions(file_name,
      LlvmO3.with_stack_size(185).with_heap_size(8000),
      inputs,
      outputs, 1, 10000000, 10000000): #74090
    print(instr)
  sys.exit(0)

  outputs = [("int", "output_1")]
  inputs = dict(input_1=("int", generate_idct_values()["input_1"]))
  file_name = "mdl_inv_quant_idct1_130_hw/mdl_inv_quant_idct1_130_hw.cpp"
#    for instr in last_n_annotated_instructions(file_name,
#        LlvmO0.with_stack_size(1000),
#        inputs,
#        outputs, 5, 9333, 1000): #74090
#      print(instr)
#    sys.exit(0)
  compare_memory_regions(
      file_name,
      Python.with_stack_size(1000),
      LlvmO3.with_stack_size(1000),
      (572, 572 + 32),
      #(300, 300 + 128),
      (380, 380 + 32),
      #(412, 412 + 256),
      inputs,
      outputs,
      64)
  sys.exit(0)
#    for instr in annotated_instructions(file_name,
#        CompileOptions(Toolchain.Llvm, OptLevel(3)),
#        dict(input_1=("int", [42, 4, 1, 2, 3, 4, 5, 6, 7])),
#        [("int", "output_1")], 4):
#      print(instr)
  for instr in last_n_annotated_instructions(file_name,
      CompileOptions(Toolchain.Llvm, OptLevel(1)),
      dict(input_1=("int", flatten([[len(string)] + [ord(c) for c in string] for string in
        ["foo\0", "bar\0", "blah\0"]]))),
      [("int", "output_1")], 5, 3000, 3000): #74090
    print(instr)

#    file_name = "hls_fft.c"
#    input_values = []
#    for i in range(255):
#      input_values.append((i + 1) * 2.1)
#    input_values.append(256.0)
#    inputs = dict(input_1=("float", input_values))
#    outputs = [("float", "output_1")]
#
#    for instr in annotated_instructions(file_name,
#        CompileOptions(Toolchain.Llvm, OptLevel(1)),
#        inputs,
#        outputs, 256):
#      print(instr)

#    compare_memory_regions(
#        file_name,
#        CompileOptions(Toolchain.Python),
#        CompileOptions(Toolchain.Llvm),
#        (968, 968 + 512),
#        (974, 974 + 512),
#        inputs,
#        outputs,
#        256)

#    for instr in last_n_annotated_instructions(file_name,
#        CompileOptions(Toolchain.Llvm),
#        inputs,
#        outputs, 256, 76300, 200): #74090
#      print(instr)

#    for instr in last_n_annotated_instructions(file_name,
#        CompileOptions(Toolchain.Python),
#        inputs,
#        outputs, 256, 173941, 200): #74090
#      print(instr)

else:
  unittest.main()
