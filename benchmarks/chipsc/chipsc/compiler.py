import os
import sys
import shutil
try:
  import chips.api.api as chips
  from chips.compiler.exceptions import StopSim
except:
  pass
  raise
import subprocess
import json
import re
from chipsc.compileoptions import Toolchain, compilation_id
from chipsc.util import cwd, relative_file_path
from collections import namedtuple

IoPortRegex = re.compile(
    r"^\s*(const)?\s*(?P<type>[a-z]+)\s+(?P<name>[a-zA-Z_0-9]+)"
    r"\s*=?\s*(?P<dir>in|out)put\s*\("
    r"\s*\"(?P=name)\"\s*"
    r"\)\s*;\s*$")
def _parse_io_ports_from_file(source_file_path):
  inputs = OrderedDict()
  outputs = []

  with open(source_file_path, "r") as file:
    for line in file:
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

class Chip(object):
  def __init__(self, chip, component):
    self.chip  = chip
    self.component = component

  def instantiate(self, directory, chip_name, chip_id):
    self.component(self.chip, chip_id)
    return ChipInstance(self.chip, directory, chip_name, chip_id)

  def load(self, program, chip_name, chip_id):
    self.component.load_from_program(self.chip, program)
    return ChipInstance(self.chip, None, chip_name, chip_id)

class ChipInstance(object):
  def __init__(self, chip, directory, chip_name, chip_id):
    self.chip = chip
    self.directory = directory
    self.step_i = -1
    self.component = self.chip.instances[0]

    self.name = chip_name
    self.chip_id = chip_id
    self.interface = self.component.asm_program.interface

  def prepare_for_simulation(self, inputs, outputs):
    chips_inputs = dict()
    for name, (input_type, values) in inputs.items():
      chips_inputs[name] = chips.Stimulus(self.chip, name, input_type,
          values)

    chips_outputs = dict()
    for output_type, name in outputs:
      chips_outputs[name] = chips.Response(self.chip, name, output_type)

    self.chips_inputs = chips_inputs
    self.chips_outputs = chips_outputs

    self.component.prepare_for_simulation(chips_inputs, chips_outputs)

    self.inputs = inputs
    self.outputs = outputs

  def output_type(self, name_to_find):
    return next(ty for ty, name in self.outputs if name == name_to_find)

  def simulate(self, finished_predicate, funcs_before_step, funcs_after_step):
    self.chip.simulation_reset()
    model = self.chip.instances[0].model

    self.step_i = -1
    while True:
      for func in funcs_before_step:
        func(model)
      try:
        self.chip.simulation_step()
      except StopSim:
        break
      self.step_i += 1
      yield
      for func in funcs_after_step:
        func(model)

      if finished_predicate(self.chips_outputs):
        break
  
  @property
  def memory(self):
    return self.chip.instances[0].model.get_memory()
  @property
  def program_counter(self):
    return self.chip.instances[0].model.get_program_counter()
  @property
  def registers(self):
    return self.chip.instances[0].model.get_registers()
  @property
  def instructions(self):
    return self.chip.instances[0].model.instructions
  @property
  def asm_program(self):
    return self.chip.instances[0].asm_program

def _parse_io_ports(source_file_paths):
  inputs = dict()
  outputs = []
  for source_file_path in source_file_paths:
    ins, outs = _parse_io_ports_from_file(source_file_path)
    inputs.update(ins)
    outputs.extend(outs)
  return inputs, outputs

def include_dir():
  return os.path.join(os.path.dirname(chips.__file__), "..", "compiler",
      "include")

def mandatory_library_files():
  ret = []
  for name in os.listdir(include_dir()):
    if name.endswith(".c"):
      ret.append(library_file(name))
  return ret

def library_file(path):
  return os.path.join(include_dir(), path)

def compile_chip(options, temp_dir, code_files, chip_name=None):
  if options.toolchain is Toolchain.Python:
    assert len(code_files) == 1, "can't compile multiple c files"

#  inputs, outputs = _parse_io_ports(code_files)

  main_file_name = os.path.basename(code_files[0])
  if chip_name is None:
    chip_name = os.path.splitext(main_file_name)[0]
  compiler = os.path.abspath(relative_file_path("../../toolchain/compile.sh"))
  options = options.with_misc_opts("-I",
          os.path.abspath(os.path.dirname(code_files[0])))
  code_file_paths = [os.path.abspath(code_file) for code_file in code_files]
  chip_options = dict(dump_to=os.path.join(temp_dir, "instructions"))

  cmd_line_options = options.cmd_line_options
  code_file_paths.extend(mandatory_library_files())

  if options.heap_size > 0:
    cmd_line_options.insert(0, "-DHEAP_SIZE={}".format(options.heap_size))
    code_file_paths.append(library_file("optional/lib_malloc_memory.c"))

  if options.dont_initialize_memory:
    chip_options["no_initialize_memory"] = True

  if options.stack_size is not None:
    chip_options["stack_size"] = options.stack_size
  args = dict(options=chip_options)
  if options.toolchain is Toolchain.Llvm:
    with cwd(temp_dir):
      subprocess.check_call([compiler] + code_file_paths +
      cmd_line_options)

    with open(os.path.join(temp_dir, "out.opt.json"), "rb") as json_file:
      args["options"]["asm_params"] = json.load(json_file)
    with open(os.path.join(temp_dir, "out.stack-analysis.json"), "rb") as \
        json_file:
      stack_analysis = json.load(json_file)
    if "stack_size" in stack_analysis:
      args["options"]["stack_size"] = stack_analysis["stack_size"]
    elif options.stack_size is None:
      sys.stderr.write(
          "Warning: No stack size defined; using default of 2000 bytes\n")
      args["options"]["stack_size"] = 2000

    args["C_file"] = os.path.join(temp_dir, "code.bc.opt.s")
    args["options"]["is_asm"] = True
  else:
    args["C_file"] = os.path.join(temp_dir, code_file_paths[0])

  return _instantiate_chip(chip_name, compilation_id(main_file_name, options),
      args, temp_dir)

def _instantiate_chip(chip_name, chip_id, args, directory):
  chip = chips.Chip(chip_name)
  component = chips.Component(**args)
  chip = Chip(chips.Chip(chip_name), component)
  ret = chip.instantiate(directory, chip_name, chip_id)
  return ret

def chip_from_program(program, chip_name, chip_id):
  chip = chips.Chip(chip_name)
  component = chips.Component(None)
  chip = Chip(chips.Chip(chip_name), component)
  ret = chip.load(program, chip_name, chip_id)
  return ret
