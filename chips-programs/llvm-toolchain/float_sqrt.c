#include <stdio.h>

/* approximate sqrt using newton's method*/
float sqrt(float n) {
    float square, x, old;
    x = n;
    old = 0.0f;
    while(old != x){
        old = x;
        x = (x + n/x)*0.5f;
    }
    return x;
}

/* test sqrt function*/
const int input_1 input("input_1");
const int output_1 output("output_1");
void main(void) {
    float x;
    while(1){
        x = fget_float(input_1);
        fput_float(sqrt(x), output_1);
    }
}
