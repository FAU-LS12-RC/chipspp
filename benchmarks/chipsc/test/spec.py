import pytest
import subprocess
import tempfile
import main as chipsc
import glob
import os

class SpecFixture(object):
  def __init__(self, tmpdir):
    self.tmpdir = tmpdir
    self.data_dir = tmpdir.mkdir("data")
    self.name_suffix = 0
    self.output_dir = tmpdir.mkdir("out")
    self.project_dir = tmpdir / "project"
    self.counter = 0
  
  def run_chipsc(self, *args):
    chipsc.main(args)

  def run_vivado(self, project_dir, tcl_code):
    xprFile = glob.glob(os.path.join(str(project_dir), "*.xpr"))[0]
    with tempfile.NamedTemporaryFile() as output_file:
      with tempfile.NamedTemporaryFile() as script:
        script.write("""
          set xprPath [lindex $argv 0]
          set outputFilePath [lindex $argv 1]
          open_project $xprPath

          set out [open $outputFilePath "w"]
        """)
        script.write(tcl_code)
        script.flush()
        subprocess.check_call(["vivado", "-mode", "batch",
          "-source", script.name, "-tclargs", xprFile, output_file.name])
      return output_file.read()

  def write_echo_program(self, file_name=None):
    if file_name is None:
      file_name = "echo{}.c".format(self.counter)
      self.counter += 1
    return self.write_program("""
      int input_1 input("input_1");
      int output_1 output("output_1");
      void main(void) {
        while(1) { fputc(fgetc(input_1), output_1); }
      }""", file_name)

  def write_program(self, code, file_name=None):
    if file_name is None:
      file_name = "code{}.c".format(self.name_suffix)
      self.name_suffix += 1
    ret = self.tmpdir / file_name
    ret.write(code)
    return ret

  def write_data_file(self, name, data_type, *values):
    (self.data_dir / name).write(data_type + "\n" + "\n".join(values),
        mode="wb")

  def write_1_2_3_echo_input(self):
    self.write_data_file("input_1.dec", "i32\n1\n2\n3\n")
  def echo_output_starts_with_1_2_3(self):
    return self.read_output_file("output_1").startswith(
        "00000001\n00000002\n00000003\n")
  def echo_output_is_1_2_3(self):
    return self.read_output_file("output_1") == "00000001\n00000002\n00000003\n"

  def delete_data_files(self):
    self.data_dir.remove()
    self.data_dir.mkdir()

  def read_file(self, path):
    return path.read()
  def read_output_file(self, io_port_name):
    return self.read_file(self.output_dir / "{}.hex".format(io_port_name))
  def read_cycles_file(self, io_port_name):
    return self.read_file(self.output_dir / "{}.cycles".format(io_port_name))

  def make_project(self, code_file, make_vivado_project=False):
    args = [str(code_file), "-o", str(self.project_dir)]
    if make_vivado_project:
      args = ["--make-project"] + args
    self.run_chipsc(*args)

  def run_simulation(self, stop_condition, simulator=None):
    args = ["--simulate", str(self.data_dir), 
        str(self.output_dir), stop_condition, str(self.project_dir)]
    if simulator is not None:
      args += ["--simulator", simulator]
    self.run_chipsc(*args)

SkipVivado = True

@pytest.fixture
def fixture(tmpdir):
  return SpecFixture(tmpdir)

def test_should_generate_a_vivado_project_with_the_generated_sources_when_told(
    fixture):
  if SkipVivado:
    pytest.skip("vivado")
  fixture.make_project(fixture.write_echo_program("foo.c"),
      make_vivado_project=True)
  
  file_list = fixture.run_vivado(fixture.project_dir / "vivado",
      "puts $out [get_files -of_objects [get_filesets sources_1]]").split()
  assert any("foo" in name for name in file_list)
  assert any("chips_lib" in name for name in file_list)

def test_should_take_simulation_io_from_various_sources(fixture):
  fixture.make_project(fixture.write_program("""
      #include <stdio.h>
      int input_1 input("input_1");
      int input_2 input("input_2");
      int input_3 input("input_3");
      int output_1 output("output_1");
      int output_2 output("output_2");
      void main(void) {
        while(1) {
          float float_value = fget_float(input_2);
          fput_long(fgetc(input_1) + float_value, output_1);
          fput_float(float_value + fget_long(input_3), output_2);
        }
      }"""))

  fixture.write_data_file("input_1.hex", "i16", "f0")
  fixture.write_data_file("input_1.2.hex", "i16", "fffe")
  fixture.write_data_file("input_2.dec", "f32",
    "-3e6", "5.4")
  fixture.write_data_file("input_3.bin", "u32",
    "".join((
      "\x00\x00\x00\x10", "\x00\x00\x00\x00",
      "\xff\xff\xff\xff", "\xff\xff\xff\xff")))

  fixture.run_simulation("10000")

  assert fixture.read_output_file("output_1") == \
      "ffd23a30\nffffffff\n00000003\n00000000\n"
  assert fixture.read_output_file("output_2") == "ca371ac0\n408ccccd\n"

def test_should_run_simulation_until_desired_amount_of_output_has_been_produced(
    fixture):
  fixture.make_project(fixture.write_program("""
      int output_1 output("output_1");
      void main(void) {
        for(int i = 0; ; ++i) {
          for(int j = 0; j < 60; ++j) { }
          fputc(i, output_1);
        }
      }"""))

  fixture.run_simulation("output_1=100")

  assert fixture.read_output_file("output_1") == "".join(
      ["{:08x}\n".format(i) for i in range(100)])

def test_should_support_multiple_simulators(fixture):
  if SkipVivado:
    pytest.skip("vivado")
  fixture.make_project(fixture.write_echo_program(), make_vivado_project=True)

  #TODO refactor fixture.simulate_echo(simulator)
  fixture.write_1_2_3_echo_input()
  fixture.run_simulation("10000", simulator="vivado")
  assert fixture.echo_output_is_1_2_3()

def test_should_output_the_cycles_it_took_to_produce_the_desired_amount_of_vals(
    fixture):
  fixture.make_project(fixture.write_program("""
      int output_1 output("output_1");
      void main(void) {
        wait_clocks(12345);
        fputc(1, output_1);
      }
  """))

  fixture.run_simulation("output_1=1")

  difference = int(fixture.read_cycles_file("output_1")) - 12345
  assert difference < 100 and difference > 0

def test_should_allow_running_the_chips_simulator(fixture):
  fixture.make_project(fixture.write_echo_program())

  #TODO refactor fixture.simulate_echo(simulator)
  fixture.write_1_2_3_echo_input()
  fixture.run_simulation("output_1=3", simulator="chips")
  assert fixture.echo_output_is_1_2_3()

  fixture.delete_data_files()
  fixture.write_1_2_3_echo_input()
  fixture.run_simulation("100", simulator="chips")
  assert fixture.echo_output_starts_with_1_2_3()

def test_should_return_simulation_results_when_using_the_project_api(fixture):
  from chipsc.project import Project, ImplOptions
  from chipsc.compileoptions import LlvmO0
  from chipsc.sim.simulator import OutputLengthCondition
  from chipsc.sim.numbersparser import DataType

  project = Project.compile(
      str(fixture.project_dir),
      LlvmO0,
      ImplOptions(14, ""),
      [str(fixture.write_echo_program())])

  fixture.write_data_file("input_1.hex", "u32\n1\n2\nffffffff\n")
  outputs = project.run_simulation("chips", str(fixture.data_dir),
      str(fixture.output_dir), OutputLengthCondition(dict(output_1=3)),
      output_types=dict(output_1=DataType.I32))
  assert outputs["output_1"].data_type == DataType.I32
  assert isinstance(outputs["output_1"].cycles, int)
  assert outputs["output_1"].numbers == [1, 2, -1]
