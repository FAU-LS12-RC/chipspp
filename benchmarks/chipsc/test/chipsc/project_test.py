import pytest
from chipsc.project import Project, ImplOptions
from chips.compiler.assembler import AsmProgram, ChipInterface
from test.builders import MockChip

def any_impl_opts():
  return ImplOptions(10, "foo")
def any_interface():
  return ChipInterface("foo", "bar", { } , [])

class Fixture(object):
  def __init__(self, tmpdir):
    self.tmpdir = tmpdir

  def restore_project(self, *create_args, **kw_create_args):
    project = Project.create(str(self.tmpdir), *create_args,
        **kw_create_args)
    return Project.open(str(self.tmpdir))

@pytest.fixture
def fixture(tmpdir):
  return Fixture(tmpdir)

def test_should_restore_chip_interface_info_when_opened(fixture):
  interface = ChipInterface(u"foo", u"bar",
      [u"first_in"], [u"blah", u"over_and_out"])
  project = fixture.restore_project(interface, any_impl_opts())
  assert project.chip_interface == interface

def test_should_restore_the_assembly_language_program_if_there_is_one(fixture):
  instruction = dict(op="addl", z=3, literal=500)
  name_1, name_2 = "foo", "bar"

  program = AsmProgram([], [instruction], 0)
  program.set_names(None, name_1, name_2)
  id_to_input = { 0: "foo", 1: "bar" }
  program.set_io_ports(id_to_input, dict())

  chip = MockChip(asm_program=program)
  project = fixture.restore_project(any_interface(), any_impl_opts(),
      chip=chip)
  assert project.asm_program.instructions[0] == instruction
  assert project.asm_program.chip_name == name_1
  assert project.asm_program.component_name == name_2
  assert project.asm_program.id_to_input == id_to_input
