#define _POSIX_C_SOURCE 199309L
#include "utils.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    double value;
    const char *unit;
} TimeResult;

static TimeResult format_time(double nanoseconds) {
    TimeResult result;
    if (nanoseconds < 1e3) {
        result.value = nanoseconds;
        result.unit = "ns";
    } else if (nanoseconds < 1e6) {
        result.value = nanoseconds / 1e3;
        result.unit = "μs";
    } else if (nanoseconds < 1e9) {
        result.value = nanoseconds / 1e6;
        result.unit = "ms";
    } else {
        result.value = nanoseconds / 1e9;
        result.unit = "s";
    }
    return result;
}

void bench(const char *name, bench_func func, void *ctx, size_t bench_size) {
    double *results = (double *)malloc(bench_size * sizeof(double));
    struct timespec start, end;

    func(ctx);

    for (size_t i = 0; i < bench_size; ++i) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        func(ctx);
        clock_gettime(CLOCK_MONOTONIC, &end);

        double elapsed =
            (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
        results[i] = elapsed;
    }

    double sum = 0.0;
    double min_val = DBL_MAX;
    double max_val = 0.0;

    for (size_t i = 0; i < bench_size; ++i) {
        sum += results[i];
        if (results[i] < min_val)
            min_val = results[i];
        if (results[i] > max_val)
            max_val = results[i];
    }

    double mean_ns = sum / bench_size;
    TimeResult mean = format_time(mean_ns);

    double variance_sum = 0.0;
    for (size_t i = 0; i < bench_size; ++i) {
        double diff = results[i] - mean_ns;
        variance_sum += diff * diff;
    }
    double variance = variance_sum / bench_size;
    TimeResult stddev = format_time(sqrt(variance));
    TimeResult min = format_time(min_val);
    TimeResult max = format_time(max_val);

    printf("%s\n", name);
    printf("mean: %.4f %s, stddev: %.4f %s, min: %.4f %s, max: %.4f %s\n",
           mean.value, mean.unit, stddev.value, stddev.unit, min.value,
           min.unit, max.value, max.unit);

    free(results);
}
