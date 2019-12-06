import os
import subprocess
import shutil

from chipsc.project import Project

class Net(object):
  def __init__(self, config_file, options=None):
    import imp
    import chipsc.net
    if options is None:
      options = []
    config = imp.load_source("chipsc.net.foo", config_file)
    self.chip = config.generate(options)
    self.name = self.chip.name

  def compile(self, out_dir, impl_options):
    build_dir = os.path.join(out_dir, "build")
    os.mkdir(build_dir)
    built_modules = []
    for m in self.chip.modules:
      if not hasattr(m, "compile_options"):
        shutil.copyfile(m.source_file_path, os.path.join(out_dir,
          os.path.basename(m.source_file_path)))
        continue
      name = m.module_class
      if name in built_modules:
        continue
      built_modules.append(name)
      Project.compile(os.path.join(build_dir, name), m.compile_options,
          impl_options, [m.source_file_path], chip_name=name)

    inputs, outputs = self.chip.chips_io_ports()

    with open(os.path.join(out_dir, self.name + ".v"), "w") as wrapper_file:
      self.chip.generate_wrapper(wrapper_file)
    #TODO program in Python
    subprocess.check_call(["cp -u {0}/*/src/*.v {1}".format(build_dir, out_dir)],
        shell=True)

    shutil.rmtree(build_dir)

    return inputs, outputs
