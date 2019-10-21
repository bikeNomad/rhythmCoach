#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "config.h"

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h> // unlink, close
#endif

#ifdef HAVE_LIMITS_H
#include <limits.h> // PATH_MAX
#endif /* HAVE_LIMITS_H */
#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

// This macro is used to pass a string to msvc compiler: since msvc's -D flag
// strips the quotes, we define the string without quotes and re-add them with
// this macro.

#define REDEFINESTRING(x) #x
#define DEFINEDSTRING(x) REDEFINESTRING(x)

#ifdef HAVE_C99_VARARGS_MACROS
#define PRINT_ERR(...)   fprintf(stderr, "AUBIO-TESTS ERROR: " __VA_ARGS__)
#define PRINT_MSG(...)   fprintf(stdout, __VA_ARGS__)
#define PRINT_DBG(...)   fprintf(stderr, __VA_ARGS__)
#define PRINT_WRN(...)   fprintf(stderr, "AUBIO-TESTS WARNING: " __VA_ARGS__)
#else
#define PRINT_ERR(format, args...)   fprintf(stderr, "AUBIO-TESTS ERROR: " format , ##args)
#define PRINT_MSG(format, args...)   fprintf(stdout, format , ##args)
#define PRINT_DBG(format, args...)   fprintf(stderr, format , ##args)
#define PRINT_WRN(format, args...)   fprintf(stderr, "AUBIO-TESTS WARNING: " format, ##args)
#endif

#ifndef M_PI
#define M_PI         (3.14159265358979323846)
#endif

#ifndef RAND_MAX
#define RAND_MAX 32767
#endif
