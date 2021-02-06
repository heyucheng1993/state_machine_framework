#ifndef UTIL_H
#define UTIL_H

#include <assert.h>

/* runtime assert */
#define RUNTIME_ASSERT                  assert

/* compile-time assert */
#if (defined __STDC_VERSION__ && __STDC_VERSION__ >= 201112L) || \
    ((_GNUC_ > 4 || (_GNUC_ == 4 && _GNUC_MINOR_ >= 6)) && !defined __cplusplus)
#define STATIC_ASSERT(exp)         _Static_assert(exp, "static assertion failed")
#else
#define STATIC_ASSERT(exp)         typedef char static_assertion_failed[2 * (!!(exp)) - 1]
#endif

#endif