#ifndef __STLIB_H__
#define __STLIB_H__

///stdlib.h
///----------
///
///macros
///******
///
///The header <stdlib.h> defines the following macros:
///
///+ NULL
///+ RAND_MAX
///+ MB_CUR_MAX
///+ MB_LEN_MAX
///
///.. note::
///
///    The EXIT_FAILURE and EXIT_SUCCESS macros are not defined.
///

#define RAND_MAX 0xfffffffful

#include <stddef.h>

///`RAND_MAX` expands to an integral constant expression, the value of which
///is the maximum value returned by the rand function.

#define MB_CUR_MAX 4
#define MB_LEN_MAX 4

///`MB_CUR_MAX` expands to a positive integer expression whose value is the
///maximum number of bytes in a multibyte character for the extended
///character set specified by the current locale (category LC_CTYPE ),
///and whose value is never greater than `MB_LEN_MAX`.
///
///.. note::
///
///    The EXIT_FAILURE and EXIT_SUCCESS macros are not defined.
///

#define RAND_MAX 0xfffffffful

///`RAND_MAX` expands to an integral constant expression, the value of which
///is the maximum value returned by the rand function.
///

#define MB_CUR_MAX 4
#define MB_LEN_MAX 4

///
///`MB_CUR_MAX` expands to a positive integer expression whose value is the
///maximum number of bytes in a multibyte character for the extended
///character set specified by the current locale (category LC_CTYPE ),
///and whose value is never greater than `MB_LEN_MAX`.
///
///types
///*****
///
///The header <stdlib.h> defines the following types:
///
///+ div_t
///

typedef struct {int quot; int rem;} div_t;
typedef struct {long quot; long rem;} ldiv_t;

///
///The atof function
///*****************
///
///Synopsis:
///
///    .. code-block:: c
///
///         #include <stdlib.h>
///         double atof(const char *nptr);
///
///.. note::
///	
///	This function is not implemented!!!
///

///
///The atoi function
///*****************
///
///Synopsis:
///
///    .. code-block::
///
///         #include <stdlib.h>
///         int atoi(const char *nptr);
///
///.. note::
///	
///	This function is not implemented!!!
///


///
///The atol function
///*****************
///
///Synopsis:
///
///    .. code-block:: c
///
///         #include <stdlib.h>
///         long int atol(const char *nptr);
///
///.. note::
///	
///	This function is not implemented!!!
///


///The strtod function
///*******************
///
///Synopsis:
///
///    .. code-block::
///
///         #include <stdlib.h>
///         double strtod(const char *nptr, char **endptr);
///
///.. note::
///	
///	This function is not implemented!!!
///


///The strtol function
///*******************
///
///Synopsis:
///
///    .. code-block:: c
///
///         #include <stdlib.h>
///         long int strtol(const char *nptr, char **endptr, int base);
///
///.. note::
///	
///	This function is not implemented!!!
///

///
///The strtoul function
///********************
///
///Synopsis:
///
///    .. code-block:: c
///
///         #include <stdlib.h>
///         unsigned long int strtoul(const char *nptr, char **endptr,
///                  int base);
///
///.. note::
///	
///	This function is not implemented!!!
///


///
///The rand function
///*****************
///
///Synopsis:
///
///    .. code-block:: c
///
///         #include <stdlib.h>
///         int rand(void);
///
///Description:
///
///   The rand function computes a sequence of pseudo-random integers in
///   the range 0 to RAND_MAX.
///
///Returns:
///
///   The rand function returns a pseudo-random integer.  
///

static unsigned long int seed = 1;
static unsigned long rand(){
	const unsigned long a = 1103515245ul;
	const unsigned long c = 12345ul;
	seed = (a*seed+c);
	return seed;
}

///
///The srand function
///******************
///
///Synopsis:
///
///    .. code-block:: c
///
///         #include <stdlib.h>
///         void srand(unsigned int seed);
///
///Description:
///
///   The srand function uses the argument as a seed for a new sequence
///   of pseudo-random numbers to be returned by subsequent calls to rand.
///   If srand is then called with the same seed value, the sequence of
///   pseudo-random numbers is repeated.  If rand is called before any
///   calls to srand have been made, the same sequence is generated as
///   when srand is first called with a seed value of 1.
///
///Returns:
///
///   The srand function returns no value.  
///

static void srand(unsigned long int s){
	seed = s;
}

#if HEAP_SIZE > 0

void _malloc_init(void);

///
///The malloc function
///*******************
///
///Synopsis:
///
///    .. code-block:: c
///
///         #include <stdlib.h>
///         void *malloc(size_t size);
///
///Description:
///
///   The malloc function allocates space for an object whose size is
///   specified by size and whose value is indeterminate. 
///
///   In Chips, the
///   dynamic memory allocations are disabled by default. To use these
///   features, define the macro `ENABLE_DYNAMIC_MEMORY`.
///
///   The total memory available for allocation is defined by the MALLOC_SIZE
///   parameter, which should be supplied when creating a component instance.
///   The size is specified in bytes (8 bit multiples).
///
///Returns:
///
///   The malloc function returns either a null pointer or a pointer to
///   the allocated space.
void* malloc(size_t size);

///
///The calloc function
///*******************
///
///Synopsis:
///
///    .. code-block:: c
///
///         #include <stdlib.h>
///         void *calloc(size_t nmemb, size_t size);
///
///Description:
///
///   The calloc function allocates space for an array of nmemb objects,
///   each of whose size is size.  The space is initialized to all bits
///   zero.
///
///Returns:
///
///   The calloc function returns either a null pointer or a pointer to
///   the allocated space.
///
void* calloc(int size, int length);


///
///The free function
///*****************
///
///
///Synopsis:
///
///    .. code-block:: c
///
///         #include <stdlib.h>
///         void free(void *ptr);
///
///Description:
///
///   The free function causes the space pointed to by ptr to be
///   deallocated, that is, made available for further allocation.  If ptr
///   is a null pointer, no action occurs.  Otherwise, if the argument does
///   not match a pointer earlier returned by the calloc, malloc, or
///   realloc function, or if the space has been deallocated by a call to
///   free or realloc, the behavior is undefined.
///
///Returns:
///
///   The free function returns no value.  
///
void free(void* block_in);

///
///The realloc function
///********************
///
///Synopsis:
///
///    .. code-block:: c
///
///         #include <stdlib.h>
///         void *realloc(void *ptr, size_t size);
///
///
///   The realloc function changes the size of the object pointed to by
///   ptr to the size specified by size.  The contents of the object remain
///   unchanged up to the lesser of the new and old sizes.  If the new
///   size is larger, the value of the newly allocated portion of the object
///   is indeterminate.  If ptr is a null pointer, the realloc function
///   behaves like the malloc function for the specified size.  Otherwise,
///   if ptr does not match a pointer earlier returned by the calloc,
///   malloc, or realloc function, or if the space has been deallocated by
///   a call to the free or realloc function, the behavior is undefined.  If
///   the space cannot be allocated, the object pointed to by ptr is
///   unchanged.  If size is zero and ptr is not a null pointer, the object
///   it points to is freed.
///
///Returns:
///
///   The realloc function returns either a null pointer or a pointer to
///   the possibly moved allocated space.
void* realloc(void* ptr, int size);

#endif

///
///The abort function
///******************
///
///Synopsis:
///
///    .. code-block:: c
///
///         #include <stdlib.h>
///         void abort(void);
///
///.. note::
///	
///	this function is not implemented!!!
///


///
///The atexit function
///*******************
///
///Synopsis:
///
///    .. code-block:: c
///
///         #include <stdlib.h>
///         int atexit(void (*func)(void));
///
///.. note::
///	
///	this function is not implemented!!!
///


///The exit function
///*****************
///
///Synopsis:
///
///    .. code-block:: c
///
///         #include <stdlib.h>
///         void exit(int status);
///
///.. note::
///	
///	This function is not implemented!!!
///


///
///The getenv function
///*******************
///
///Synopsis:
///
///    .. code-block:: c
///
///         #include <stdlib.h>
///         char *getenv(const char *name);
///
///.. note::
///	
///	this function is not implemented!!!
///

///
///The system function
///*******************
///
///Synopsis:
///
///    .. code-block:: c
///
///         #include <stdlib.h>
///         int system(const char *string);
///
///.. note::
///	
///	This function is not implemented!!!
///


///
///The bsearch function
///********************
///
///Synopsis:
///
///    .. code-block:: c
///
///         #include <stdlib.h>
///         void *bsearch(const void *key, const void *base,
///                  size_t nmemb, size_t size,
///                  int (*compar)(const void *, const void *));
///
///.. note::
///	
///	This function is not implemented!!!
///

///
///The qsort function
///******************
///
///Synopsis:
///
///    .. code-block:: c
///
///         #include <stdlib.h>
///         void qsort(void *base, size_t nmemb, size_t size,
///                  int (*compar)(const void *, const void *));
///
///.. note::
///	
///	This function is not implemented!!!
///

///
///The abs function
///****************
///
///Synopsis:
///
///    .. code-block:: c
///
///         #include <stdlib.h>
///         int abs(int j);
///
///Description:
///
///   The abs function computes the absolute value of an integer j.  If
///   the result cannot be represented, the behavior is undefined.
///
///Returns:
///
///   The abs function returns the absolute value.  
///

int abs(int x){
    if (x < 0) {
        return - x;
    } else {
        return x;
    }
}

///
///The div function
///****************
///
///Synopsis:
///
///    .. code-block:: c
///
///         #include <stdlib.h>
///         div_t div(int numer, int denom);
///
///Description:
///
///   The div function computes the quotient and remainder of the
///   division of the numerator numer by the denominator denom .  If the
///   division is inexact, the sign of the resulting quotient is that of the
///   algebraic quotient, and the magnitude of the resulting quotient is the
///   largest integer less than the magnitude of the algebraic quotient.  If
///   the result cannot be represented, the behavior is undefined;
///   otherwise, `quot * denom + rem` equals numer.
///
///Returns:
///
///   The div function returns a structure of type div_t, comprising
///   both the quotient and the remainder.  The structure contains the
///   following members:
///
///   .. code-block:: c
///
///         int quot;   /*  quotient */
///         int rem;    /*  remainder */
///

div_t div(int n, int d){
	div_t retval;
//	retval.quot = unsigned_divide_xxxx(n, d);
//	retval.rem = unsigned_modulo_yyyy;
    retval.quot = n / d;
    retval.rem = n % d;
	return retval;
}

///
///The labs function
///*****************
///
///Synopsis:
///
///    ..code-block::
///
///         #include <stdlib.h>
///         long int labs(long int j);
///
///Description:
///
///   The labs function is similar to the abs function, except that the
///   argument and the returned value each have type long int.
///

long labs(long x){
    if (x < 0) {
        return - x;
    } else {
        return x;
    }
}

///
///The ldiv function
///*****************
///
///Synopsis:
///
///    .. code-block:: c
///
///         #include <stdlib.h>
///         ldiv_t ldiv(long int numer, long int denom);
///
///Description:
///
///   The ldiv function is similar to the div function, except that the
///   arguments and the members of the returned structure (which has type
///   ldiv_t ) all have type long int.
///

//ldiv_t ldiv(long int n, long int d){
//	ldiv_t retval;
//	retval.quot = long_unsigned_divide_xxxx(n, d);
//	retval.rem = long_unsigned_modulo_yyyy;
//	return retval;
//}

///
///The mblen function
///******************
///
///Synopsis:
///
///    .. code-block:: c
///
///         #include <stdlib.h>
///         int mblen(const char *s, size_t n);
///
///.. note::
///	
///	This function is not implemented!!!
///

///
///The mbtowc function
///*******************
///
///Synopsis:
///
///    .. code-block:: c
///
///         #include <stdlib.h>
///         int mbtowc(wchar_t *pwc, const char *s, size_t n);
///
///.. note::
///	
///	This function is not implemented!!!
///

///
///The wctomb function
///*******************
///
///Synopsis:
///
///    .. code-block:: c
///
///         #include <stdlib.h>
///         int wctomb(char *s, wchar_t wchar);
///
///.. note::
///	
///	This function is not implemented!!!
///

///
///The mbstowcs function
///*********************
///
///Synopsis:
///
///    .. code-block:: c
///
///         #include <stdlib.h>
///         size_t mbstowcs(wchar_t *pwcs, const char *s, size_t n);
///
///.. note::
///	
///	This function is not implemented!!!
///

///
///The wcstombs function
///*********************
///
///Synopsis:
///
///    .. code-block:: c
///
///         #include <stdlib.h>
///         size_t wcstombs(char *s, const wchar_t *pwcs, size_t n);
///
///.. note::
///	
///	This function is not implemented!!!
///


#endif

