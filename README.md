Chips++ is a C/C++-to-Verilog design flow for generating FPGA-targeting CPUs with an application-specific instruction set.
What kind of restricted instruction set is used wholly depends on the input program written in C/C++.
A custom compiler then analyzes this program and instructs the processor generator to change bits of the CPU architecture accordingly.
This is useful if you want to place existing or generated C/C++ code within hardware designs but do not want to bear the increased hardware cost or restricted performance of traditional soft-core processors.

A more comprehensive description of this method can be found in the paper [1] which has been presented at the [2019 International Conference on ReConFigurable Computing and FPGAs (ReConFig)](http://www.reconfig.org/).
If you want to reference our work, please cite the paper as follows:
```
@inproceedings{plagwitz2019compiler,
  title={Compiler-Based High-Level Synthesis of Application-Specific Processors on FPGAs},
  author = {Plagwitz, Patrick and Streit, Franz-Josef and Becher, Andreas and Wildermann, Stefan and Teich, Jürgen},
  booktitle = {2019 International Conference on ReConFigurable Computing and FPGAs (ReConFig)},
  pages={1--8},
  year={2019},
  organization={IEEE}
}

Installation
====================
Dependencies
---------------
Make sure you have clang/clang++ in your PATH.
Also, Python 2 is required for the Python library.
All other required libraries will be downloaded automatically during the next steps.

Procedure
------------
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

Then, change back to the root folder, then to the backend folder, which is the second part of the compiler toolchain.
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
If it works, you can run all enabled tests with
```bash
$ ./run.sh
```

Usage
===================
`chipsc` is the Chips++ compiler that can map **C/C++** code to **Verilog**.
After sourcing the `setenv.sh`, this binary should be in your PATH.

`int_adder`
------------------

You can try it out with the simple `int_adder` program shown in the paper [1].
First, write the code to a file:
```bash
$ cat >main.c <<EOF
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
$ chipsc main.c -O0 -o out
```

The folder `out` will now contain the generated RTL code as well as some intermediate code.
All the `*.v` files inside `out/src` will be the hardware design.

Run `chipsc` with the `--help` option to see other ways of using it.
For instance, you can automatically make a Vivado project and implement the design with
```bash
$ mkdir impl-results
$ chipsc main.c --make-project --impl impl-results/ main.c -o out
```
This requires that you have Vivado installed.
The designs were tested with Vivado 2018.3.

More programs to test are in the `chips-programs/llvm-toolchain` folder.
Also check out the script under `benchmarks/recompile-all.sh` which has many commented out lines generating designs with varying configurations.
This will also show you how to configure generation of networks with `chipsc`.

License
================
See LICENSE.

References
=================
[1] Plagwitz, Streit, Becher, Wildermann, Teich: Compiler-Based High-Level Synthesis of Application-Specific Processors on FPGAs
