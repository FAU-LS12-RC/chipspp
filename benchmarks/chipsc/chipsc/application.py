from chipsc.net.compiler import Net
import os
from chips.compiler.assembler import ChipInterface
from chipsc.compileoptions import generic_chip_id
from chipsc.project import Project

def make_net_project(options, config_file, out_dir, impl_options,
    make_subdir=False):
  net = Net(config_file, options)
  chip_id = generic_chip_id(net.name, options)
  if make_subdir:
    out_dir = os.path.join(out_dir, chip_id)
  if not os.path.exists(out_dir):
    os.mkdir(out_dir)
  src_dir = os.path.join(out_dir, "src")
  os.mkdir(src_dir)
  inputs, outputs = net.compile(src_dir, impl_options)
  interface = ChipInterface(net.name, chip_id, inputs, outputs)
  return Project.create(out_dir, interface, impl_options)
