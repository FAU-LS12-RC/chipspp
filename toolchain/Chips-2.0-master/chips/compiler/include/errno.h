#ifndef __errno_h_
#define __errno_h_

#define ERRNO(suffix) errno##suffix
#define ERRNO2(suffix) ERRNO(suffix)
#ifndef ERRNO_SUFFIX
#define ERRNO_SUFFIX
#endif
#define errno ERRNO2(ERRNO_SUFFIX)

int errno;

const int EDOM = 1;
const int ERANGE = 2;

#endif

