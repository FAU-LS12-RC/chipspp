from chipsc.compiler import compile_chip
from chipsc.compileoptions import CompileOptions, Toolchain, OptLevel, \
    compilation_id
from chipsc.project import Project, ImplOptions
from chipsc.application import make_net_project
from chipsc.sim.simulator import StepsCondition, OutputLengthCondition
import argparse
import os
import sys

def parse_args(args):
  parser = argparse.ArgumentParser(prog="chipsc")
  parser.add_argument("sources", help="source files or project directory",
      nargs="+")
  parser.add_argument("-o", help="output directory", metavar="output-dir")
  parser.add_argument("--make-subdir", help="make a subdirectory with name "
    "generated from compilation options within the output argument",
    action="store_true")

  parser.add_argument("-O", help="optimization level", metavar="opt-level",
      choices=["s", "0", "1", "2", "3"], default="0")
  parser.add_argument("--toolchain", help="python or llvm",
      choices=["python", "llvm"], default="llvm")
  parser.add_argument("--stack-size", help="stack size in bytes", type=int)
  parser.add_argument("--heap-size", help="heap size in bytes", type=int,
      default=0)
  parser.add_argument("--copts", help="Compiler options, separated by comma, "
    "e.g., --copts=-DA=1,-DB=2 passes -DA=1 and -DB=2 to the compiler.")

  parser.add_argument("--clk-period", type=float,
      help="Period in ns to put into the constraints file")
  parser.add_argument("--board",
      help="Board to use for the vendor tool project.")

  parser.add_argument("--make-project", action="store_true",
      help="generate vivado project")
  parser.add_argument("--impl",
      metavar="results-dir",
      help="implement design and store reports in argument")
  parser.add_argument("--ip", metavar="ip-repo",
      help="generate ip block into specified repo directory")

  parser.add_argument("--simulate", nargs=3,
      metavar=("stimuli-data-dir", "output-dir", "steps-or-number-of-values"),
      help="Run simulator. The third option is either a positive integer or "
      "a comma-separated list of key-value pairs denoting the number of "
      "values the simulator will wait for on each output port. Example: "
      "output_1=5,output_2=7")

  parser.add_argument("--simulator", choices=["icarus", "vivado", "chips"],
      default="icarus", metavar="simulator",
      help="Simulator to use for --simulate.")

  parser.add_argument("--make-net",
      help="Generate a net of processors. The source must be a config file.",
      action="store_true")
  ret = parser.parse_args(args)

  source_is_project = os.path.isdir(ret.sources[0])

  if ret.o is not None and source_is_project:
    sys.stderr.write("warning: -o ignored\n")
  if ret.o is None and not source_is_project:
    sys.stderr.write("error: missing option -o\n")
    parser.print_help(sys.stderr)
    sys.exit(2)

  impl_options = DefaultImplOptions
  if ret.clk_period:
    impl_options.clk_period_ns = ret.clk_period
  if ret.board:
    impl_options.board_name = ret.board
  ret.impl_options = impl_options

  if ret.copts:
    ret.copts = ret.copts.split(",")
  return ret

DefaultImplOptions = ImplOptions(14, "xc7z020clg484-1")

def make_net_project_from_args(args):
  return make_net_project(args.copts or [], args.sources[0], args.o, 
      args.impl_options, args.make_subdir)

def main(string_args):
  args = parse_args(string_args)

  toolchain = dict(llvm=Toolchain.Llvm, python=Toolchain.Python)[args.toolchain]
  compile_options = CompileOptions(toolchain, OptLevel(args.O),
      stack_size=args.stack_size, heap_size=args.heap_size)
  if args.copts:
    compile_options = compile_options.with_misc_opts(*args.copts)

  if args.make_net:
    project = make_net_project_from_args(args)
  elif not os.path.isdir(args.sources[0]):
    project_dir = args.o
    if args.make_subdir:
      project_dir = os.path.join(project_dir,
          compilation_id(args.sources[0], compile_options))
    project = Project.compile(project_dir, compile_options, args.impl_options,
        args.sources)
  else:
    project = Project.open(args.sources[0])

  make_project = args.make_project
  if args.ip or args.impl:
    make_project = True

  if args.make_project:
    project.generate_vivado_project()
  if args.ip is not None:
    project.generate_ip_block(args.ip)
  if args.impl is not None:
    project.run_implementation(args.impl)
  if args.simulate is not None:
    if all(c.isdigit() for c in args.simulate[2]):
      stop_condition = StepsCondition(int(args.simulate[2]))
    else:
      pairs = args.simulate[2].split(",")
      split_pairs = [pair.split("=") for pair in pairs]
      stop_condition = OutputLengthCondition(dict((pair[0], int(pair[1])) for
        pair in split_pairs))
    project.run_simulation(args.simulator, args.simulate[0], args.simulate[1],
        stop_condition)

  return 0

if __name__ == "__main__":
  sys.exit(main(sys.argv[1:]))
