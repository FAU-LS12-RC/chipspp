from chipsc.net.config import Chip, Signal, In, Out
#from chipsc.compileoptions import LlvmO0, LlvmO1, LlvmO2, \
#    LlvmO3, LlvmOs, Toolchain, compilation_id, CompileOptions
from chipsc.compileoptions import LlvmO3, LlvmOs

import os
import glob

Folder = os.path.join(os.path.abspath(os.path.dirname(__file__)),
    "jpegDecoder_chip")
def cpp(pattern):
    pattern = "*{}*".format(pattern)
    dir = glob.glob(os.path.join(Folder, pattern) + "/")[0]
    return glob.glob(os.path.join(dir, pattern + ".cpp"))[0]

def generate(options):
  if "Os" in options:
    compiler_opts = LlvmOs
  else:
    compiler_opts = LlvmO3

  chip = Chip("jpeg_decoder", os.path.dirname(__file__))
  chip.alias_external("clk", "in_clk")
  chip.alias_external("rst", "async_rst")

  def make_fifo():
    ret = chip.hdl_module("generic_fwft_fifo.vhd", "generic_fwft_fifo",
        dict(DEPTH_BITS=6))
    ret.add_axis_slave_interface("in")
    ret.add_axis_master_interface("out")
    ret.add_signal(Signal("in_clk", 1, In))
    ret.add_signal(Signal("async_rst", 1, In))
    chip.make_external(ret.single_signals["in_clk"])
    chip.make_external(ret.single_signals["async_rst"])
    return ret

  def make_internal_connection(src, dest):
    if "fifo" in options:
      fifo = make_fifo()
      chip.connect(src, fifo.in_stream(0))
      chip.connect(fifo.out_stream(0), dest)
    else:
      chip.connect(src, dest)

  if "doublebuf" in options:
    compiler_opts = compiler_opts.with_misc_opts("-DDOUBLE_BUF_IDCT")
  if "dist" in options or "dist_double_idct" in options:
    compiler_opts = compiler_opts.with_misc_opts("-DASYNCIO")
    if "dist_double_idct" in options:
      compiler_opts = compiler_opts.with_misc_opts("-DDOUBLE_IDCT")

    splitter = chip.c_module(cpp("splitter"), compiler_opts)
    colorconv = chip.c_module(cpp("colorspaceconv"), compiler_opts)

    chip.make_external(splitter.in_stream(0))
    chip.make_external(colorconv.out_stream(0))

    def generate_channel(i):
      rlc = chip.c_module(cpp("zrl1"), compiler_opts)
      zigzag = chip.c_module(cpp("zigzag"), compiler_opts)
      idcts = [chip.c_module(cpp("idct"), compiler_opts)]
      if "dist_double_idct" in options:
        idcts.append(chip.c_module(cpp("idct"), compiler_opts))

      for m in [rlc, zigzag] + idcts:
        chip.make_external(m.single_signals["clk"])
        chip.make_external(m.single_signals["rst"])

      make_internal_connection(splitter.out_stream(i), rlc.in_stream(0))
      make_internal_connection(rlc.out_stream(0), zigzag.in_stream(0))
      make_internal_connection(zigzag.out_stream(0), idcts[0].in_stream(0))
      make_internal_connection(idcts[0].out_stream(0),
          colorconv.in_stream("inputArg{}".format(i + 1)))
      if "dist_double_idct" in options:
        make_internal_connection(zigzag.out_stream(1), idcts[1].in_stream(0))
        make_internal_connection(idcts[1].out_stream(0),
            colorconv.in_stream("inputArg{}".format(i + 1 + 3)))

    generate_channel(0)
    generate_channel(1)
    generate_channel(2)

    for m in [splitter, colorconv]:
      chip.make_external(m.single_signals["clk"])
      chip.make_external(m.single_signals["rst"])
  elif "external_idct" in options:
    compiler_opts = compiler_opts.with_misc_opts("-DASYNCIO")

    main = chip.c_module(os.path.join(Folder, "main_all_except_idct.cpp"),
        compiler_opts)
    chip.make_external(main.in_stream("inputArg1"))
    chip.make_external(main.out_stream("outputArg1"))

    idcts = [chip.c_module(cpp("idct"), compiler_opts) for i in range(3)]

    for i, idct in enumerate(idcts):
      make_internal_connection(
          main.out_stream("output_idct_{}".format(i + 1)),
          idct.in_stream(0))
      make_internal_connection(
          idct.out_stream(0),
          main.in_stream("input_idct_{}".format(i + 1)))

    for m in [main] + idcts:
      chip.make_external(m.single_signals["clk"])
      chip.make_external(m.single_signals["rst"])
  elif "double_idct" in options:
    compiler_opts = compiler_opts.with_misc_opts("-DASYNCIO")

    main = chip.c_module(os.path.join(Folder, "main_double_idct.cpp"),
        compiler_opts)
    chip.make_external(main.in_stream("inputArg1"))
    chip.make_external(main.out_stream("outputArg1"))

    idcts = [chip.c_module(cpp("idct"), compiler_opts) for i in range(6)]
    idct_pairs = zip(idcts[::2], idcts[1::2])

    for i, double_idcts in enumerate(idct_pairs):
      for j, idct in enumerate(double_idcts):
        make_internal_connection(
            main.out_stream("output_idct_{}_{}".format(i + 1, j + 1)),
            idct.in_stream(0))
        make_internal_connection(
            idct.out_stream(0),
            main.in_stream("input_idct_{}_{}".format(i + 1, j + 1)))

    for m in [main] + idcts:
      chip.make_external(m.single_signals["clk"])
      chip.make_external(m.single_signals["rst"])
  elif "all" in options:
    compiler_opts = compiler_opts.with_misc_opts("-DASYNCIO")

    main = chip.c_module(os.path.join(Folder, "main_all.cpp"), compiler_opts)
    chip.make_external(main.in_stream(0))
    chip.make_external(main.out_stream(0))
    chip.make_external(main.single_signals["clk"])
    chip.make_external(main.single_signals["rst"])

  return chip


#    generic(
#               DATA_WIDTH : integer range 1 to 4096 := 32;
#               DEPTH_BITS : integer range 4 to 21 := 10; --9 would be 512 deep
#               PROG_EMPTY : integer range 7 to 4096 := 7;
#               PROG_FULL  : integer range 7 to 4096 := 7;
#               HAS_LAST   : boolean := false; --use last
#               ASYNC      : boolean := false;
#               DEBUG      : boolean := false
#           );
