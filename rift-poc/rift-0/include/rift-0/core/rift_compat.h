#ifndef RIFT_COMPAT_H
#define RIFT_COMPAT_H

/* Feature test macros */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <string.h>

/* strdup compatibility */
#ifndef HAVE_STRDUP
#ifdef __cplusplus
extern "C" {
#endif
char* strdup(const char* s);
#ifdef __cplusplus
}
#endif
#endif

/* strptime compatibility */
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif
#include <time.h>

#endif /* RIFT_COMPAT_H */
