#!/usr/bin/env python2

import subprocess
import atexit
import sys
from math import pi, sin, cos
from chips.api.api import Chip, Stimulus, Response, Wire, Component

try:
    import scipy as s
except ImportError:
    print "You need scipy to run this script!"
    exit(0)

try:
    import numpy as n
except ImportError:
    print "You need numpy to run this script!"
    exit(0)

try:
    from matplotlib import pyplot
except ImportError:
    print "You need matplotlib to run this script!"
    exit(0)

def test():

    chip = Chip("fft")

    x_re = [0.0 for i in range(1024)]
    x_im = [0.0 for i in range(1024)]
    x_re[0:63] = [sin(2.0 * pi * (i/64.0)) for i in range(64)]

    x_re = Stimulus(chip, "x_re", "double", x_re)
    x_im = Stimulus(chip, "x_im", "double", x_im)
    fft_x_re = Response(chip, "fft_x_re", "double")
    fft_x_im = Response(chip, "fft_x_im", "double")
    
    #create a filter component using the C code
    fft = Component("fft.c")

    #add an instance to the chip
    fft(
        chip, 
        inputs = {
            "x_re":x_re,
            "x_im":x_im,
        },
        outputs = {
            "fft_x_re":fft_x_re,
            "fft_x_im":fft_x_im,
        },
    )

    #run the simulation
    chip.simulation_reset()
    while len(fft_x_im) < len(x_im):
        chip.simulation_step()

    x_re = list(x_re)
    x_im = list(x_im)
    fft_x_re = list(fft_x_re)[:len(x_re)]
    fft_x_im = list(fft_x_im)[:len(x_im)]

    time_complex = [i + (j*1.0) for i, j in zip(x_re, x_im)]
    numpy_complex = s.fft(time_complex)
    numpy_magnitude = n.abs(numpy_complex)

    chips_complex = [i + (j*1.0j) for i, j in zip(fft_x_re, fft_x_im)]
    chips_magnitude = n.abs(chips_complex)

    f, subplot = pyplot.subplots(3, sharex=True)

    pyplot.subplots_adjust(hspace=1.0)
    subplot[0].plot(x_re, 'g')
    subplot[1].plot(numpy_magnitude, 'r')
    subplot[2].plot(chips_magnitude, 'b')
    pyplot.xlim(0, 1023)
    subplot[0].set_title("Time Domain Signal (64 point sine)")
    subplot[1].set_title("Frequency Spectrum - Numpy")
    subplot[2].set_title("Frequency Spectrum - Chips")
    subplot[0].set_xlabel("Sample")
    subplot[1].set_xlabel("Sample")
    subplot[2].set_xlabel("Sample")
    pyplot.savefig("../docs/source/examples/images/example_5.png")
    pyplot.show()

def indent(lines):
    return "\n    ".join(lines.splitlines())

def generate_docs():

    documentation = """

Fast Fourier Transform
======================

This example builds on the Taylor series example. We assume that the sin and
cos routines have been placed into a library of math functions math.h, along
with the definitions of :math:`\\pi`, M_PI.

The `Fast Fourier Transform (FFT) <http://en.wikipedia.org/wiki/Fast_Fourier_transform>`_ 
is an efficient method of decomposing discretely sampled signals into a frequency spectrum, it
is one of the most important algorithms in Digital Signal Processing (DSP).
`The Scientist and Engineer's Guide to Digital Signal Processing <http://www.dspguide.com/>`_ 
gives a straight forward introduction, and can be viewed on-line for free. 

The example shows a practical method of calculating the FFT using the
`Cooley-Tukey algorithm <http://en.wikipedia.org/wiki/Fast_Fourier_transform#Cooley.E2.80.93Tukey_algorithm>`_.


.. code-block:: c

    %s

The C code includes a simple test routine that calculates the frequency spectrum of a 64 point sine wave.

.. image:: images/example_5.png

"""%indent(open("fft.c").read())

    document = open("../docs/source/examples/example_5.rst", "w").write(documentation)

test()
generate_docs()
