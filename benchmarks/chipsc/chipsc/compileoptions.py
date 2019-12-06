import os

class Toolchain(object):
  Python = object()
  Llvm = object()
def toolchain_name(toolchain):
  if toolchain is Toolchain.Python:
    return "python"
  else:
    return "llvm"
class OptLevel(object):
  def __init__(self, n):
    self.n = n
  def __str__(self):
    return "O{}".format(self.n)

class CompileOptions(object):
  def __init__(self, toolchain, opt_level=OptLevel(0), misc_opts=[],
      stack_size=None, heap_size=0, dont_initialize_memory=False):
    self.toolchain = toolchain
    self.opt_level = opt_level
    self.stack_size = stack_size
    self.heap_size = heap_size
    self.dont_initialize_memory = dont_initialize_memory
    self.misc_opts = misc_opts

  def with_stack_size(self, stack_size):
    return CompileOptions(self.toolchain, self.opt_level, self.misc_opts,
        stack_size, self.heap_size, self.dont_initialize_memory)
  def with_misc_opts(self, *misc_opts):
    return CompileOptions(self.toolchain, self.opt_level,
        self.misc_opts + list(misc_opts), self.stack_size,
        self.heap_size, self.dont_initialize_memory)
  def with_no_memory_initialization(self):
    return CompileOptions(self.toolchain, self.opt_level,
        self.misc_opts, self.stack_size, self.heap_size, True)
  def with_heap_size(self, heap_size):
    return CompileOptions(self.toolchain, self.opt_level,
        self.misc_opts, self.stack_size, heap_size, self.dont_initialize_memory)

  @property
  def cmd_line_options(self):
    return ["-{}".format(self.opt_level)] + self.misc_opts

Python = CompileOptions(Toolchain.Python, OptLevel(3))
LlvmO0 = CompileOptions(Toolchain.Llvm, OptLevel(0))
LlvmO1 = CompileOptions(Toolchain.Llvm, OptLevel(1))
LlvmO2 = CompileOptions(Toolchain.Llvm, OptLevel(1))
LlvmO3 = CompileOptions(Toolchain.Llvm, OptLevel(3))
LlvmOs = CompileOptions(Toolchain.Llvm, OptLevel("s"))

def normalize_string(s):
  return "".join("_" if c == "-" else c for c in s if 
      c.isalpha() or c.isdigit() or c in "-_")

def generic_chip_id(name, options_list):
  if len(options_list) == 0:
    return name
  else:
    return "{}-{}".format(normalize_string("".join(options_list)), name)

def compilation_id(code_file_name, compile_options):
  return toolchain_name(compile_options.toolchain) + "-" + generic_chip_id(
      os.path.basename(code_file_name),
      str(compile_options.opt_level))
