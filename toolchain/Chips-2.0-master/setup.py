#!/usr/bin/env python2

from distutils.core import setup, Extension
version = open("version").read()
long_description = open("README.rst").read()

setup(name="Chips",
      version=version,
      description="Design hardware with Python",
      long_description=long_description,

      author="Jon Dawson",
      author_email="chips@jondawson.org.uk",
      url="http://pyandchips.org",
      download_url="http://github.com/idawsonjon/Chips-2.0",
      keywords=["Verilog", "FPGA", "C", "HDL", "Synthesis", "VHDL"],
      install_requires=["numpy"],
      ext_modules = [
            Extension("_chips_c", ["chips_c/chips_c.c", "chips_c/chips_c_wrap.c"])
      ],
      py_modules = [
          "chips_c"
      ],
      classifiers = [
          "Programming Language :: Python",
          "License :: OSI Approved :: MIT License",
          "Operating System :: OS Independent",
          "Intended Audience :: Science/Research",
          "Intended Audience :: Developers",
          "Development Status :: 3 - Alpha",
          "Topic :: Scientific/Engineering :: Electronic Design Automation (EDA)",
          "Topic :: Software Development :: Embedded Systems",
          "Topic :: Software Development :: Code Generators",
      ],
      packages=[
          "chips",
          "chips.chipsweb",
          "chips.compiler",
          "chips.api",
          "chips.components",
          "chips.utils"
      ],
      package_data = {
          "chips.compiler":[
              "builtins.h",
              "include/*.[ch]",
              "include/optional/*.[ch]",
              "*.v"
          ],
          "chips.utils":[
              "icons/*.png",
          ],
          "chips.chipsweb":[
              "templates/*.html",
              "static/*",
          ],
          "chips.components":[
              "verilog/*.v",
          ]
      },
      scripts=[
          "c2verilog",
          "csim"
      ]
)
