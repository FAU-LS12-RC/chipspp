import os

from chipsc.project import parse_io_ports
from collections import namedtuple

In = True
Out = False

names = []

def legalized_name(name):
  return "".join(c for c in name if c.isalpha())
def unique_name(base):
  ret = base
  i = 0
  while ret in names:
    ret = base + "_{}".format(i)
    i += 1
  names.append(ret)
  return ret

def type_spec(width):
  if width == 1:
    return ""
  else:
    return "[{}:0]".format(width - 1)

def sig_name(signal_ref):
  return "{0}_{1}".format(
      signal_ref.module.name, signal_ref.signal.name)

Signal = namedtuple("Signal", ["name", "width", "direction"])
Signal.signals = lambda self: [self]
SignalRef = namedtuple("SignalRef", ["signal", "module"])

class PortGroup(object):
  def __init__(self, name, signals, direction):
    self.name = name
    self._signals = signals
    self.direction = direction

  def inverse(self):
    return PortGroup(self.name,
        [signal._replace(direction=not signal.direction) for
          signal in self._signals], not self.direction)

  def signals(self):
    for s in self._signals:
      yield s

def stream_in_port(name):
  return PortGroup(name, [
    Signal(name, 32, In),
    Signal("{}_stb".format(name), 1, In),
    Signal("{}_ack".format(name), 1, Out)], In)
def stream_out_port(name):
  return stream_in_port(name).inverse()

def axis_in_port(name):
  return PortGroup(name, [
    Signal("{}_data".format(name), 32, In),
    Signal("{}_valid".format(name), 1, In),
    Signal("{}_ready".format(name), 1, Out)], In)
def axis_out_port(name):
  return axis_in_port(name).inverse()

class Module(object):
  def in_stream(self, i):
    if isinstance(i, str):
      for group in self.in_streams:
        if group.name == i:
          return SignalRef(group, self)
      assert False, i
    else:
      return SignalRef(self.in_streams[i], self)
  def out_stream(self, i):
    if isinstance(i, str):
      for group in self.out_streams:
        if group.name == i:
          return SignalRef(group, self)
      assert False, i
    else:
      return SignalRef(self.out_streams[i], self)
  def signals(self):
    for s in self.single_signals.values():
      yield s
    for stream in self.in_streams:
      for s in stream.signals():
        yield SignalRef(s, self)
    for stream in self.out_streams:
      for s in stream.signals():
        yield SignalRef(s, self)

class CModule(Module):
  def __init__(self, source_file_path, compile_opts):
    self.module_class = legalized_name(os.path.basename(source_file_path))
    self.name = unique_name(self.module_class)
    self.compile_options = compile_opts
    self.source_file_path = source_file_path
    self.generics = dict()

    self.single_signals = dict(
        clk=SignalRef(Signal("clk", 1, In), self),
        rst=SignalRef(Signal("rst", 1, In), self))
    self.in_streams = []
    self.out_streams = []

    inputs, outputs = parse_io_ports(source_file_path, compile_opts)
    #print(source_file_path, inputs, outputs)
    for input_name in inputs:
      self.in_streams.append(stream_in_port(input_name))
    for _, output_name in outputs:
      self.out_streams.append(stream_out_port(output_name))

class HDLModule(Module):
  def __init__(self, source_file_path, module_class, generics):
    self.module_class = module_class
    self.name = unique_name(self.module_class)
    self.generics = generics
    self.source_file_path = source_file_path

    self.single_signals = dict()
    self.in_streams = []
    self.out_streams = []

  def add_signal(self, signal):
    self.single_signals[signal.name] = SignalRef(signal, self)
  def add_axis_slave_interface(self, name):
    self.in_streams.append(axis_in_port(name))
  def add_axis_master_interface(self, name):
    self.out_streams.append(axis_out_port(name))

class Chip(object):
  def __init__(self, name, base_dir):
    self.name = name
    self.wires = []
    self.modules = []
    self.io_ports = []
    self.base_dir = base_dir
    self.aliases = dict()

  def c_module(self, source_path, compile_opts):
    if not os.path.isabs(source_path):
      source_path = os.path.join(self.base_dir, source_path) 
    ret = CModule(source_path, compile_opts)
    self.modules.append(ret)
    return ret
  def hdl_module(self, source_path, class_name, generics):
    ret = HDLModule(os.path.join(self.base_dir, source_path), class_name,
        generics)
    self.modules.append(ret)
    return ret

  def alias_external(self, canonical_name, alternative_name):
    self.aliases[alternative_name] = canonical_name
  def normalize(self, signal_name):
    if signal_name in self.aliases:
      return self.aliases[signal_name]
    return signal_name

  def make_external(self, signal):
    #TODO generalize
    if isinstance(signal.signal, PortGroup):
      self.io_ports.append(signal.signal)
  def connect(self, a, b):
    if a.signal.direction is Out:
      self.wires.append((a, b))
    else:
      self.wires.append((b, a))

  def signals(self):
    for m in self.modules:
      for s in m.signals():
        yield s

  def unconnected_signals(self):
    ret = list(set(self.signals()))
    for a, b in self.wires:
      for port in [a, b]:
        for s in port.signal.signals():
          ref = SignalRef(s, port.module)
          if ref in ret:
            ret.remove(ref)
    return ret

  def chips_io_ports(self):
#    inputs = dict()
#    outputs = []
#    for io_port in self.io_ports:
#      #TODO generalize
#      name = io_port._signals[0].name
#      if io_port.direction is In:
#        inputs[name] = ("int", [])
#      else:
#        outputs.append(("int", name))
#    return inputs, outputs
    inputs = []
    outputs = []
    for io_port in self.io_ports:
      #TODO generalize
      name = io_port._signals[0].name
      if io_port.direction is In:
        inputs.append(name)
      else:
        outputs.append(name)
    return inputs, outputs

  def generate_wrapper(self, out):
    out.write("module {}(".format(self.name))
    signals = [
        ("output" if sig.signal.direction is Out else "input") +
        " {} ".format(type_spec(sig.signal.width)) +
        self.normalize(sig.signal.name) for sig in self.unconnected_signals()]
    signals = list(set(signals))
    out.write(", ".join(signals))
    out.write(");\n\n")
    for m in self.modules:
      for s in m.signals():
        out.write("wire {1} {0};\n".format(sig_name(s),
          type_spec(s.signal.width)))
    out.write("\n")

    for m in self.modules:
      out.write(m.module_class)
      if len(m.generics) > 0:
        generic_settings = [".{0}({1})".format(generic, value) for
            generic, value in m.generics.items()]
        out.write(" #({0})".format(",\n".join(generic_settings)))
      out.write(" {}_0(\n".format(m.name))
      port_mappings = []
      for s in m.signals():
        port_mappings.append(".{0}({1})".format(s.signal.name, sig_name(s)))
      out.write(",\n".join(port_mappings))
      out.write(");\n\n")

    for port_a, port_b in self.wires:
      module_a = port_a.module
      module_b = port_b.module
      for s_a, s_b in zip(port_a.signal.signals(), port_b.signal.signals()):
        in_signal = SignalRef(s_a, module_a)
        out_signal = SignalRef(s_b, module_b)
        if s_a.direction is Out:
          tmp = in_signal
          in_signal = out_signal
          out_signal = tmp

        out.write("assign {0} = {1};\n".format(
          sig_name(in_signal), sig_name(out_signal)))


    out.write("//unconnected\n")
    for sig_ref in self.unconnected_signals():
      m = sig_ref.module
      for s in sig_ref.signal.signals():
        format = "{0} = {1}"
        if s.direction is In:
          format = "{1} = {0}"

        out.write(("assign " + format + ";\n").format(
          self.normalize(s.name), sig_name(SignalRef(s, m))))

    out.write("endmodule\n")
