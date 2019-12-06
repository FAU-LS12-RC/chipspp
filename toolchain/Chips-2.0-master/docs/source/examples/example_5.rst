

Fast Fourier Transform
======================

This example builds on the Taylor series example. We assume that the sin and
cos routines have been placed into a library of math functions math.h, along
with the definitions of :math:`\pi`, M_PI.

The `Fast Fourier Transform (FFT) <http://en.wikipedia.org/wiki/Fast_Fourier_transform>`_ 
is an efficient method of decomposing discretely sampled signals into a frequency spectrum, it
is one of the most important algorithms in Digital Signal Processing (DSP).
`The Scientist and Engineer's Guide to Digital Signal Processing <http://www.dspguide.com/>`_ 
gives a straight forward introduction, and can be viewed on-line for free. 

The example shows a practical method of calculating the FFT using the
`Cooley-Tukey algorithm <http://en.wikipedia.org/wiki/Fast_Fourier_transform#Cooley.E2.80.93Tukey_algorithm>`_.


.. code-block:: c

    /* fft.c */
    /* Jonathan P Dawson */
    /* 2013-12-23 */
    
    #include <math.h>
    #include <stdio.h>
    
    /*globals*/
    const int n = 1024;
    const int m = 10;
    double twiddle_step_real[m];
    double twiddle_step_imaginary[m];
    
    
    /*calculate twiddle factors and store them*/
    void calculate_twiddles(){
        unsigned stage, span;
        for(stage=0; stage<m; stage++){
            span = 1 << stage;
            twiddle_step_real[stage] = cos(M_PI/span);
            twiddle_step_imaginary[stage] = -sin(M_PI/span);
        }
    }
    
    /*bit reverse*/
    unsigned bit_reverse(unsigned forward){
        unsigned reversed=0;
        unsigned i;
        for(i=0; i<m; i++){
            reversed <<= 1;
            reversed |= forward & 1;
            forward >>= 1;
        }
        return reversed;
    }
    
    /*calculate fft*/
    void fft(double reals[], double imaginaries[]){
    
        int stage, subdft_size, span, i, ip, j;
        double sr, si, temp_real, temp_imaginary, imaginary_twiddle, real_twiddle;
    
    
        //read data into array
        for(i=0; i<n; i++){
            ip = bit_reverse(i);
            if(i < ip){
                temp_real = reals[i];
                temp_imaginary = imaginaries[i];
                reals[i] = reals[ip];
                imaginaries[i] = imaginaries[ip];
                reals[ip] = temp_real;
                imaginaries[ip] = temp_imaginary;
            }
        }
    
        //butterfly multiplies
        for(stage=0; stage<m; stage++){
            subdft_size = 2 << stage;
            span = subdft_size >> 1;
    
            //initialize trigonometric recurrence
    
            real_twiddle=1.0;
            imaginary_twiddle=0.0;
    
            sr = twiddle_step_real[stage];
            si = twiddle_step_imaginary[stage];
    
            report(stage);
    
            for(j=0; j<span; j++){
                for(i=j; i<n; i+=subdft_size){
                    ip=i+span;
    
                    temp_real      = reals[ip]*real_twiddle      - imaginaries[ip]*imaginary_twiddle;
                    temp_imaginary = reals[ip]*imaginary_twiddle + imaginaries[ip]*real_twiddle;
    
                    reals[ip]       = reals[i]-temp_real;
                    imaginaries[ip] = imaginaries[i]-temp_imaginary;
    
                    reals[i]       = reals[i]+temp_real;
                    imaginaries[i] = imaginaries[i]+temp_imaginary;
    
                }
                //trigonometric recurrence
                temp_real=real_twiddle;
                real_twiddle      = temp_real*sr - imaginary_twiddle*si;
                imaginary_twiddle = temp_real*si + imaginary_twiddle*sr;
            }
    
        }
    
    }
    
    const int x_re_in = input("x_re");
    const int x_im_in = input("x_im");
    const int fft_x_re_out = output("fft_x_re");
    const int fft_x_im_out = output("fft_x_im");
    
    void main(){
        unsigned i;
        double reals[n];
        double imaginaries[n];
    
        /* pre-calculate sine and cosine*/
        calculate_twiddles();
    
        while(1){
            /* read time domain signal */
            for(i=0; i<n; i++){
                reals[i] = fget_double(x_re_in);
                imaginaries[i] = fget_double(x_im_in);
            }
    
            /* transform into frequency domain */
            fft(reals, imaginaries);
    
            /* output frequency domain signal*/
            for(i=0; i<n; i++){
                fput_double(reals[i], fft_x_re_out);
                fput_double(imaginaries[i], fft_x_im_out);
            }
        }
    }

The C code includes a simple test routine that calculates the frequency spectrum of a 64 point sine wave.

.. image:: images/example_5.png

