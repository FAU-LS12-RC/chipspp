#TODO infrastructure
import os
from chipsc.sim.numbersparser import RawInputNumbers

def _is_input_name(name):
  return name.endswith(".dec") or name.endswith(".hex") or \
      name.endswith(".bin")

class FileDataSource(object):
  def __init__(self, directory, name_filter=lambda _: True, default_type=None):
    self.directory = directory
    self.name_filter = name_filter
    self.default_type = default_type

  def _read_file(self, path):
    with open(path, "rb") as file:
      if self.default_type is not None:
        data_type = self.default_type
      else:
        data_type = file.readline()
      string = file.read()
    return RawInputNumbers(data_type, string, self._read_cycles(path))

  def _read_cycles(self, path):
    directory = os.path.dirname(path)
    name = os.path.basename(path).split(".")[0]
    cycles_file_path = os.path.join(directory, name + ".cycles")
    if os.path.isfile(cycles_file_path):
      with open(cycles_file_path, "r") as cycles_file:
        return int(cycles_file.readline())
    return -1

  def read(self):
    ret = dict()
    for name in os.listdir(self.directory):
      path = os.path.join(self.directory, name)
      if os.path.isfile(path) and _is_input_name(name) and \
          self.name_filter(name):
        ret[name] = self._read_file(os.path.join(self.directory, name))
    return ret
