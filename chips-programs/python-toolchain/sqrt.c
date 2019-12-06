#include <stdio.h>

/* approximate sqrt using newton's method*/
double sqrt(double n) {
    double square, x, old;
    x = n;
    old = 0.0;
    while(old != x){
        old = x;
        x = (x + n/x)*0.5;
    }
    return x;
}

/* test sqrt function*/
const int input_1 = input("input_1");
const int output_1 = output("output_1");
void main(){
    double x;
    while(1){
        x = fget_float(input_1);
        fput_float(sqrt(x), output_1);
    }
}
