#include <stddef.h>
/* Copy operations */

static void strcpy(char to[], char from[]){
	unsigned i=0;
	while(from[i]){
		to[i] = from[i];
		i++;
	}
	to[i] = from[i];
}

static void strncpy(char to[], char from[], unsigned n){
	unsigned i=0;
	while(from[i]){
		if(i >= n) return;
		to[i] = from[i];
		i++;
	}
	to[i] = from[i];
}

#ifdef LLVM_TOOLCHAIN
static void* memcpy(void* dest, void const* src, unsigned int n) {
    unsigned char* dest_i8 = (unsigned char*)dest;
    unsigned char const* src_i8 = (unsigned char const*)src;
    unsigned int* dest_i32 = (unsigned int*)dest;
    unsigned int const* src_i32 = (unsigned int const*)src;
    unsigned int n_i32;
    unsigned int n_i8;
    if(((size_t)src & 3) == 0 && ((size_t)dest & 3) == 0) {
        n_i32 = n >> 2;
        n_i8 = n & 3;
    } else {
        n_i32 = 0;
        n_i8 = n;
    }

    for(unsigned int i = 0; i < n_i32; ++i)
        dest_i32[i] = src_i32[i];
    for(unsigned int i = 0; i < n_i8; ++i)
        dest_i8[i + n_i32 * 4] = src_i8[i + n_i32 * 4];

    return dest;
}
#else
void memcpy(unsigned to[], unsigned from[], unsigned n){
	unsigned i=0;
	while(i < n){
		to[i] = from[i];
		i++;
	}
}
#endif

static void memmove(char to[], char from[], unsigned n){
	memcpy(from, to, n); /* cannot overlap */ 
}

/* Miscelaneous String Operations */

static unsigned strlen(char const s[]){
	unsigned i = 0;
	while(s[i]) i++;
	return i;
}

static void memset(char s[], unsigned value, unsigned n){
  unsigned i = 0;
	for(i=0; i<n; i++){
		s[i] = value;
	}
}

/* String Concatonation Operations */

//void strcat(char to[], char from[]){
//	unsigned i=0, j=0;
//	while(a[i]) i++;
//	i++;
//	while(b[j]){
//		a[i] = b[j];
//		i++;
//		j++;
//	}
//}
//
//void strncat(char to[], char from[], unsigned n){
//	unsigned i=0, j=0;
//	while(a[i]) i++;
//	i++;
//	while(b[j] and j < n){
//		a[i] = b[j];
//		i++;
//		j++;
//	}
//}

/* String Comparison Operations */

static unsigned strcmp(char a[], char b[]){
	unsigned i = 0;
	while(a[i] && b[i]){
		if(a[i] > b[i]){
			return 1;
		} else if(a[i] < b[i]) {
			return -1;
		}
		i++;
	}
	if(a[i]) return  1; //if a is longer
	if(b[i]) return -1; //if b is longer
	return 0;
}

static unsigned strncmp(char a[], char b[], unsigned n){
	unsigned i=0;
	while(a[i] && b[i] && i < n){
		if(a[i] > b[i]){
			return 1;
		} else if(a[i] < b[i]) {
			return -1;
		}
		i++;
	}
	return 0;
}

static unsigned memcmp(char a[], char b[], unsigned n){
	unsigned i=0;
	while(i < n){
		if(a[i] > b[i]){
			return 1;
		} else if(a[i] < b[i]) {
			return -1;
		}
		i++;
	}
	return 0;
}

/* String Search Operations */

/* return the position of character f in string s starting from start of s */
static unsigned strchr(char s[], char f){
	unsigned i;
	for(i=0; i<strlen(s); i++){
		if(s[i] == f) return i;
	}
	return -1;
}

/* return the position of character f in string s starting from end of s */
static unsigned strrchr(char s[], char f){
	unsigned i;
	for(i=strlen(s)-1; i; i--){
		if(s[i] == f) return i;
	}
	return -1;
}

/* return the number of characters at the start of string a that contain any character in string b */
static unsigned strspn(char a[], char b[]){
	unsigned i, j, match;
	for(i=0; i<strlen(a); i++){
		match = 0;
		for(j=0; j<strlen(b); j++){ 
			if(a[i] == b[j]){
				match = 1;
				break;
			}
		}
		if(!match) return i;
	}
    return 0;
}

/* return the number of characters at the start of string a that do not contain any character in b */
static unsigned strcspn(char a[], char b[]){
	unsigned i, j, match;
	for(i=0; i<strlen(a); i++){
		match = 0;
		for(j=0; j<strlen(b); j++){
			if(a[i] == b[j]){
				match = 1;
				break;
			}
		}
		if(match) return i;
	}
    return 0;
}

/* return first occurance of the any character in string b within string a */
/* return -1 if not found */
static unsigned strpbrk(char a[], char b[]){
	unsigned i, j;
	for(i=0; i<strlen(a); i++){
		for(j=0; j<strlen(b); j++){
			if(a[i] == b[j]) return i;
		}
	}
	return -1;
}

/* return first occurance of the whole of string b within string a */
/* return -1 if not found */
static unsigned strstr(char a[], char b[]){
	unsigned i, j, match;
	for(i=0; i<strlen(a); i++){
		match = 1;
		for(j=0; j<strlen(b); j++){
			if(a[i+j] != b[j]){
				match=0;
				break;
			}		
		}
		if(match) return match;
	}
	return -1;
}
