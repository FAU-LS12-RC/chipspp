from chipsc.net.config import Chip
#from chipsc.compileoptions import LlvmO0, LlvmO1, LlvmO2, \
#    LlvmO3, LlvmOs, Toolchain, compilation_id, CompileOptions
from chipsc.compileoptions import LlvmO3

import os

def generate(*_):
  chip = Chip("pipeline", os.path.dirname(__file__))

  a = chip.c_module("net_foo.c", LlvmO3)
  b = chip.c_module("net_foo.c", LlvmO3)
  c = chip.c_module("net_bar.c", LlvmO3)

  for m in [a, b, c]:
    chip.make_external(m.single_signals["clk"])
    chip.make_external(m.single_signals["rst"])

  chip.make_external(a.in_stream(0))
  chip.connect(a.out_stream(0), b.in_stream(0))
  chip.connect(b.out_stream(0), c.in_stream(0))
  chip.make_external(c.out_stream(0))

  return chip
