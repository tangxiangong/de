#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

typedef void (*bench_func)(void *ctx);

void bench(const char *name, bench_func func, void *ctx, size_t bench_size);

#endif // UTILS_H
