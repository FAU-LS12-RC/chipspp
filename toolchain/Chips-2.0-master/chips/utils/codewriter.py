from __future__ import print_function
import contextlib
import textwrap

class IgnoredObject(object):
  def __iter__(self):
    return iter([])
Ignore = IgnoredObject()

class Parser(object):
  def __init__(self, variables):
    self.variables = variables
    self.current_var = None

  def parse_name(self, format):
    closing_control_char_index = format.find("`")
    assert closing_control_char_index != -1, "` expected"
    name = format[:closing_control_char_index]
    return name, format[closing_control_char_index+1:]

  def parse_var_reference(self, format):
    name, format = self.parse_name(format)
    if self.current_var is Ignore:
      return Ignore, format
    if name == "$i":
      return self.iteration_index, format
    if name.startswith("."):
      if name == ".":
        return self.current_var, format
      return getattr(self.current_var, name[1:]), format
    return self.variables[name], format

  def parse_repeat_block(self, format):
    var, format = self.parse_var_reference(format)
    newline_index = format.index("\n")
    non_whitespace_index = 0
    for i in range(len(format)):
      if not format[i] in ["\t", "\n", " "]:
        break
    non_whitespace_index = i
    if format[non_whitespace_index] == "`":
      format = format[non_whitespace_index:]
    else:
      format = format[newline_index+1:]

    ret = ""
    for i, element in enumerate(var):
      self.current_var = element
      self.iteration_index = i
      text, _ = self.parse_text(format)
      ret += text
    current_var = self.current_var
    self.current_var = Ignore
    _, format = self.parse_text(format)
    self.current_var = current_var
    return ret, format

  def parse_control_seq(self, format):
    if format[0] == "`":
      return "`", format[1:]
    if format[0] == ">":
      return self.parse_repeat_block(format[1:])
    if format[0:3] == "if ":
      var, format = self.parse_var_reference(format[3:])
      if var:
        return self.parse_text(format)
      else:
        return "", self.parse_text(format)[1]

    var, format = self.parse_var_reference(format)
    return str(var), format

  def parse_text(self, format):
    ret = ""
    while True:
      control_char_index = format.find("`")
      if control_char_index == -1:
        ret += format
        format = ""
        break
      if format[control_char_index+1] == "<":
        newline_index = format.rindex("\n", 0, control_char_index)
        ret += format[:newline_index]
        format = format[control_char_index+2:]
        break
      if format[control_char_index+1:control_char_index+3] == "fi":
        ret += format[:control_char_index]
        format = format[control_char_index+3:]
        break
      if format[control_char_index+1] == ">":
        newline_index = format.rfind("\n", 0, control_char_index)
        if newline_index == -1:
          newline_index = control_char_index
        text = format[:newline_index]
      else:
        text = format[:control_char_index]
      text_2, format = self.parse_control_seq(format[control_char_index+1:])
      ret += text + text_2
    return ret, format

  def parse(self, format):
    text, format = self.parse_text(format)
    assert len(format) == 0, format
    return text

class CodeWriter(object):
  def __init__(self, out):
    self.out = out
  
  def write(self, format, **variables):
    if "indent" in variables:
      indent_width = variables["indent"]
      format = "\n".join(format[1:].splitlines()[:-1]) + "\n"
    parser = Parser(variables)
    output = parser.parse(format)

    if "indent" in variables:
      output = "\n".join([" "*indent_width + line for line in 
          textwrap.dedent(output).split("\n")][:-1])

    self.out.write(output)

  @contextlib.contextmanager
  def indented(self):
    yield

if __name__ == "__main__":
  import sys
  writer = CodeWriter(sys.stdout)
  writer.write(r"""``
      ``int array[`n`];
      int main(void) {
      """, n=5)
  with writer.indented():
    writer.write(r"""
          return 5;
    """)
  writer.write(r"""
      }
      """)
      
  foo = True
  writer.write(r"""
      int array[`n`];
      int main(`if foo`void`fi`if notfoo`int`fi) {
        `>iss`
        `>.`

        array[`.`] = 2; //`$i`
        `<
        `<
      }
      """, n=10, iss=[[1, 2, 3], [4], [[5]]], foo=foo, notfoo=not foo, indent=4)
