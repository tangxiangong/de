#ifndef ODE_H
#define ODE_H

#include <stddef.h>

typedef double (*source_func)(double);

typedef enum { METHOD_EULER, METHOD_RK4 } Method;

typedef struct {
    double duration;
    double time_step;
    Method method;
} Config;

typedef struct {
    double lambda;
    double initial_condition;
    source_func source_term;
} ODE;

typedef struct {
    double *t;
    double *u;
    size_t size;
} Solution;

Solution ode_solve(const ODE *ode, const Config *config);
void free_solution(Solution *sol);

#endif // ODE_H
