Installation
====================
Chips++ can be run from the source directory but a few build steps are required.
First, set up all dependencies by calling
```bash
$ source setenv.sh
```
in the root folder.
Note that this will download dependencies when first run.
Among them is LLVM which will take a while to build.

Next, you will have to install the Chips Python library to a local virtualenv.
This can be done by running the following command:
```bash
$ ./create_local_install.sh
```

The compiler toolchain consists of two parts that must be built as well.
First change to the optimization pass
```bash
$ cd toolchain/opt
```
and then build it
```bash
$ cmake .
$ make
```

Then, change back to the root folder, then to the backend folder which is the second part of the compiler toolchain.
```bash
$ cd ../..
$ cd toolchain/backend
$ cmake .
$ make
```

Tests
========================
After you have completed the installation steps, you can try running some integration tests by issuing the following commands to make sure everything works as expected.
From the root folder:
```bash
$ cd benchmarks/test-suite
$ ./run.sh EndToEndTest.test_int_adder
```

This will test a very simple 32-bit integer adder.
If this works, you can run all enabled tests with
```bash
$ ./run.sh
```
.

Usage
===================
`chipsc` is the Chips++ compiler that can map C/C++ code to Verilog.
After sourcing the `setenv.sh`, this binary should be in your PATH.

`int_adder`
------------------

You can try it out with the simple `int_adder` program shown in the paper [1].
First, write the code to a file:
```bash
cat >code.c <<<EOF
int input_1 input("input_1");
int input_2 input("input_2");
int output_1 output("output_1");
void main() {
  while(1) {
    fputc((int)fgetc(input_1) + (int)fgetc(input_2), output_1);
  }
}
EOF
```

Then, run `chipsc` on it:
```bash
$ chipsc code.c -O0 -o out
```

The folder `out` will now contain the generated RTL code as well as some intermediate code.


Check out the file ...
* network
* ...

References
=================
[1]
