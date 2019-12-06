#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mini-gmp.h"

int main(){
    printf("Hello World!\n");

    int BUFFER_SIZE_IN_BIT = 32;
    char buf[BUFFER_SIZE_IN_BIT];

    mpz_t p;
    mpz_t q;
    mpz_t n;
    mpz_t e;
    mpz_t d;
    mpz_t phi;

    mpz_init(p);
    mpz_init(q);
    mpz_init(n);
    mpz_init(e);
    mpz_init(d);
    mpz_init(phi);

    srand((unsigned) 0);

    for (int i = 0; i < BUFFER_SIZE_IN_BIT; i++){
        buf[i] = rand() % 0xFF;
    }
    buf[0] |= 0xC0;
    buf[BUFFER_SIZE_IN_BIT - 1] |= 0x01;

    mpz_import(p, BUFFER_SIZE_IN_BIT, 1, sizeof(buf[0]), 0, 0, buf);
    mpz_set_str(e, "2", 10);
    while(!mpz_probab_prime_p(p, 5)){
        mpz_add(p, p, e);
    }
    
//    mpz_set_str(e, "20", 10);
    
//    mpz_add(p, p, e);
    printf("\n\np is \n[%s]\n\n", mpz_get_str(NULL, 16, p));
    
    srand((unsigned) 3);
    for (int i = 0; i < BUFFER_SIZE_IN_BIT; i++){
        buf[i] = rand() % 0xFF;
    }
    buf[0] |= 0xC0;
    buf[BUFFER_SIZE_IN_BIT - 1] |= 0x01;
    
    mpz_import(q, BUFFER_SIZE_IN_BIT, 1, sizeof(buf[0]), 0, 0, buf);
    while(!mpz_probab_prime_p(q, 5)){
        mpz_add(q, q, e);
    }
//    mpz_set_str(e, "80", 10);
//    mpz_add(q, q, e);
    printf("\n\nq is \n[%s]\n\n", mpz_get_str(NULL, 16, q));
    
    mpz_set_ui(e, 65537);
 //   mpz_set_str(p, "104049403672171273376431130943375503215243803619252552659594627766276948916393", 10);
 //   mpz_set_str(q, "89262900112161570763765061296696228560466069985196701634444153154507049556659", 10);  

    mpz_mul(n, p, q);
    mpz_sub_ui(p, p, 1);
    mpz_sub_ui(q, q, 1);
    mpz_mul(phi, p, q);

    if(mpz_invert(d, e, phi) == 0){
        mpz_gcd(p, e, phi);
    }

//    char* s = mpz_get_str(NULL, 16, n);

    printf("n is \n[%s]\n\n", mpz_get_str(NULL, 16, n));
    printf("e is \n[%s]\n\n", mpz_get_str(NULL, 16, e));
    printf("d is \n[%s]\n\n", mpz_get_str(NULL, 16, d));
//    printf("p is \n[%s]\n\n", mpz_get_str(NULL, 16, p));
//    printf("q is \n[%s]\n\n", mpz_get_str(NULL, 16, q));

    return 0;
}
