__author__ = "Jon Dawson"
__copyright__ = "Copyright (C) 2012, Jonathan P Dawson"
__version__ = "0.1"

import os.path
import subprocess

from chips.compiler.exceptions import C2CHIPError

operators = [
    "!", "~", "+", "-", "*", "/", "//", "%", "=", "==", "<", ">", "<=", ">=",
    "!=", "|", "&", "^", "||", "&&", "(", ")", "{", "}", "[", "]", ";", "<<",
    ">>", ",", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "<<=", ">>=", "^=",
    "++", "--", "?", ":", ".", "->",
]


class Tokens:

    """Break the input file into a stream of tokens,
    provide functions to traverse the stream."""

    def __init__(self, filename, parameters={}):
        self.tokens = []
        self.definitions = []
        self.filename = None
        self.lineno = None
        self.scan(
            os.path.join(os.path.dirname(__file__), "builtins.h"),
            external_preprocessor=False)
        self.scan(os.path.abspath(filename))

        tokens = []
        for token in self.tokens:
            f, l, t = token
            if t in parameters:
                tokens.append((f, l, str(parameters[t])))
            else:
                tokens.append(token)
        self.tokens = tokens

    def scan(self,
             filename,
             input_file=None,
             parameters={},
             external_preprocessor=True):
        """Convert the test file into tokens"""

        self.filename = filename

        if external_preprocessor:

            directory = os.path.abspath(__file__)
            directory = os.path.dirname(directory)
            directory = os.path.join(directory, "include")

            cpp_commands = [
                "cpp",
                "-nostdinc",
                "-isystem",
                directory,
                filename]
            pipe = subprocess.Popen(cpp_commands, stdout=subprocess.PIPE)
            input_file = pipe.stdout
        else:
            if input_file is None:
                try:
                    input_file = open(self.filename)
                except IOError:
                    raise C2CHIPError("Cannot open file: " + self.filename)

        token = []
        tokens = []
        self.lineno = 1
        jump = False
        for line in input_file:

            # include files
            line = line + " "
            if jump:
                if line.strip().startswith("#endif"):
                    jump = False
                if line.strip().startswith("#else"):
                    jump = False
                self.lineno += 1
                continue

            elif external_preprocessor and line.strip().startswith("#"):
                l = line.strip()
                l = l.lstrip("#")
                l = l.split('"')
                lineno = int(l[0].strip())
                self.lineno = lineno
                filename = l[1].strip().strip('"')
                self.filename = filename
                continue

            elif line.strip().startswith("#include"):
                filename = self.filename
                lineno = self.lineno
                self.tokens.extend(tokens)
                if line.strip().endswith(">"):
                    directory = os.path.abspath(__file__)
                    directory = os.path.dirname(directory)
                    directory = os.path.join(directory, "include")
                else:
                    directory = os.path.abspath(self.filename)
                    directory = os.path.dirname(directory)
                self.filename = line.strip().replace(
                    "#include", "").strip(' ><"')
                self.filename = os.path.join(directory, self.filename)
                self.scan(self.filename)
                self.lineno = lineno
                self.filename = filename
                tokens = []
                self.lineno += 1
                continue

            elif line.strip().startswith("#define"):
                definition = line.strip().split(" ")[1]
                self.definitions.append(definition)
                self.lineno += 1
                continue

            elif line.strip().startswith("#undef"):
                definition = line.strip().split(" ")[1]
                self.definitions.remove(definition)
                self.lineno += 1
                continue

            elif line.strip().startswith("#ifdef"):
                definition = line.strip().split(" ")[1]
                if definition not in self.definitions:
                    jump = True
                self.lineno += 1
                continue

            elif line.strip().startswith("#ifndef"):
                definition = line.strip().split(" ")[1]
                if definition in self.definitions:
                    jump = True
                self.lineno += 1
                continue

            elif line.strip().startswith("#else"):
                jump = True
                self.lineno += 1
                continue

            elif line.strip().startswith("#endif"):
                self.lineno += 1
                continue

            newline = True
            for char in line:

                if not token:
                    token = char

                # c style comment
                elif (token + char).startswith("/*"):
                    if (token + char).endswith("*/"):
                        token = ""
                    else:
                        token += char

                # c++ style comment
                elif token.startswith("//"):
                    if newline:
                        token = char
                    else:
                        token += char

                # identifier
                elif token[0].isalpha():
                    if char.isalnum() or char == "_":
                        token += char
                    else:
                        tokens.append((self.filename, self.lineno, token))
                        token = char

                # number
                elif token[0].isdigit():
                    if char.upper() in "0123456789ABCDEFXUL.":
                        token += char
                    elif token.upper().endswith("E") and char in ["+", "-"]:
                        token += char
                    else:
                        tokens.append((self.filename, self.lineno, token))
                        token = char

                # string literal
                elif token.startswith('"'):
                    if char == '"' and previous_char != "\\":
                        token += char
                        tokens.append((self.filename, self.lineno, token))
                        token = ""
                    else:
                        # remove dummy space from the end of a line
                        if newline:
                            token = token[:-1]
                        previous_char = char
                        token += char

                # character literal
                elif token.startswith("'"):
                    if char == "'":
                        token += char
                        tokens.append((self.filename, self.lineno, token))
                        token = ""
                    else:
                        token += char

                # operator
                elif token in operators:
                    if token + char in operators:
                        token += char
                    else:
                        tokens.append((self.filename, self.lineno, token))
                        token = char

                else:
                    token = char

                newline = False
            self.lineno += 1

        self.tokens.extend(tokens)

    def error(self, string):
        """
        Generate an error message (including the filename and line number)
        """

        raise C2CHIPError(string + "\n", self.filename, self.lineno)

    def peek(self):
        """
        Return the next token in the stream, but don't consume it.
        """

        if self.tokens:
            return self.tokens[0][2]
        else:
            return ""

    def peek_next(self):
        """
        Return the next next token in the stream, but don't consume it.
        """

        if len(self.tokens) > 1:
            return self.tokens[1][2]
        else:
            return ""

    def get(self):
        """Return the next token in the stream, and consume it."""

        if self.tokens:
            self.lineno = self.tokens[0][1]
            self.filename = self.tokens[0][0]
        try:
            filename, lineno, token = self.tokens.pop(0)
        except IndexError:
            self.error("Unexpected end of file")
        return token

    def end(self):
        """Return True if all the tokens have been consumed."""

        return not self.tokens

    def expect(self, expected):
        """Consume the next token in the stream,
        generate an error if it is not as expected."""

        try:
            filename, lineno, actual = self.tokens.pop(0)
        except IndexError:
            self.error("Unexpected end of file")
        if self.tokens:
            self.lineno = self.tokens[0][1]
            self.filename = self.tokens[0][0]
        if actual == expected:
            return
        else:
            self.error("Expected: %s, got: %s" % (expected, actual))
