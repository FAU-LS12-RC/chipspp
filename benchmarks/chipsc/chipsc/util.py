import os
import contextlib

@contextlib.contextmanager
def cwd(new_dir):
  current_dir = os.getcwd()
  os.chdir(new_dir)
  try:
    yield
  finally:
    os.chdir(current_dir)

def relative_file_path(name):
  current_dir = os.path.dirname(__file__)
  return os.path.join(current_dir, "..", name)
