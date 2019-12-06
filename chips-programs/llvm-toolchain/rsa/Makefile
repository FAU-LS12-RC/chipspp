#helloworld: hello.c
#	gcc -o hello hello.c -I.

#rsa: rsa.c
#	gcc -o rsa rsa.c -lgmp -I.

#rsa_keygen: rsa_keygen.c
#	gcc -o rsa_keygen rsa_keygen.c -lgmp -I.

mini_rsa_keygen: mini_rsa_keygen.o mini-gmp.o
	gcc -o mini_rsa_keygen mini_rsa_keygen.o mini-gmp.o -I.

mini_rsa_keygen.o: mini_rsa_keygen.c mini-gmp.h
	gcc -g -Wall -c mini_rsa_keygen.c

mini-gmp.o: mini-gmp.c mini-gmp.h
	gcc -g -Wall -c mini-gmp.c
