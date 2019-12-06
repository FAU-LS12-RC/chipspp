#include <failureno.h>

int failureno;

int foo(int x) {
    if(x == 0) {
        failureno = EINVAL;
        return -1;
    }
    if(x == 5) {
        failureno = ERANGE;
        return -1;
    }
    return x;
}
