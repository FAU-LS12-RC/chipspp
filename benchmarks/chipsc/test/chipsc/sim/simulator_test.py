import pytest
from chipsc.sim.simulator import Simulator
from chipsc.project import ProjectSources
from test.builders import MockChip

Input = object()
ParsedInput = object()

class MockInput(object):
  def __init__(self, return_value):
    self.return_value = return_value
  def read(self):
    return self.return_value

class TestParser(object):
  def parse(self, input):
    if input is Input:
      return ParsedInput
    elif input is Output:
      return ParsedOutput
    assert False

Output = object()
OutputSource = MockInput(Output)
ParsedOutput = object()
InterpretedOutput = object()

class MockInterpreter(object):
  def __init__(self):
    pass
  def reinterpret(self, data):
    assert data is ParsedOutput
    return InterpretedOutput

class MockBackend(object):
  def __init__(self):
    self.waveform_dump_format = "foo"
  def choose_test_bench(self, test_benches):
    return test_benches[0]
  def run(self, chip_interface, src_files, test_bench, inputs, stop_condition,
      constants):
    self.received_inputs = inputs
    self.received_test_bench = test_bench
    self.received_constants = constants
    return OutputSource

class MockTestBench(object):
  def __init__(self, constants_for_output_dir):
    self.constants_for_output_dir = constants_for_output_dir

  def configure_for_output_dir(self, _, _2):
    return self.constants_for_output_dir
  def configure_for_stop_condition(self, _):
    return dict()

def any_sources():
  return ProjectSources([], [MockTestBench(dict())], "")
def sources_with_test_bench(bench):
  return ProjectSources([], [bench], "")

class Fixture:
  def __init__(self):
    data_source = MockInput(Input)
    parser = TestParser()
    self.backend = MockBackend()
    self.interpreter = MockInterpreter()

    self.simulator = Simulator(MockChip(), data_source, parser, self.backend,
        self.interpreter)

  def run_simulator(self, sources=any_sources()):
    return self.simulator.run(sources, None, None)

@pytest.fixture
def fixture():
  return Fixture()

def test_should_call_backend_with_parsed_inputs(fixture):
  fixture.run_simulator()
  assert fixture.backend.received_inputs is ParsedInput

def test_should_configure_chosen_test_bench_and_pass_the_result(fixture):
  constants = dict(a="b")
  test_bench = MockTestBench(constants)
  fixture.run_simulator(sources=sources_with_test_bench(test_bench))
  assert fixture.backend.received_test_bench is test_bench
  assert fixture.backend.received_constants == constants

def test_should_parse_output_files_and_return_the_content(fixture):
  fixture.backend.output = OutputSource
  fixture.interpreter.input = ParsedOutput
  fixture.interpreter.output = InterpretedOutput

  assert fixture.run_simulator() is InterpretedOutput

def test_should_also_parse_the_cycles_files(fixture):
  pass
