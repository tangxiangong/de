#include "ode.h"
#include <math.h>
#include <stdlib.h>

static double derivative(source_func f, double lambda, double time,
                         double value) {
    return f(time) - lambda * value;
}

static Solution rk4(source_func f, double lambda, double initial_condition,
                    double duration, double time_step) {
    size_t num_steps = (size_t)ceil(duration / time_step);
    size_t size = num_steps + 1;

    double *t = (double *)malloc(size * sizeof(double));
    double *u = (double *)malloc(size * sizeof(double));

    t[0] = 0.0;
    u[0] = initial_condition;

    double current_t = 0.0;
    double current_u = initial_condition;

    double k1, k2, k3, k4;

    double h_2 = time_step / 2.0;
    double h_6 = time_step / 6.0;

    for (size_t i = 0; i < num_steps - 1; ++i) {
        k1 = derivative(f, lambda, current_t, current_u);
        k2 = derivative(f, lambda, current_t + h_2, current_u + h_2 * k1);
        k3 = derivative(f, lambda, current_t + h_2, current_u + h_2 * k2);
        k4 = derivative(f, lambda, current_t + time_step,
                        current_u + time_step * k3);

        current_u += h_6 * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
        current_t += time_step;

        t[i + 1] = current_t;
        u[i + 1] = current_u;
    }

    double last_step = duration - current_t;
    double last_h_2 = last_step / 2.0;
    double last_h_6 = last_step / 6.0;

    k1 = derivative(f, lambda, current_t, current_u);
    k2 = derivative(f, lambda, current_t + last_h_2, current_u + last_h_2 * k1);
    k3 = derivative(f, lambda, current_t + last_h_2, current_u + last_h_2 * k2);
    k4 = derivative(f, lambda, current_t + last_step,
                    current_u + last_step * k3);

    current_u += last_h_6 * (k1 + 2.0 * k2 + 2.0 * k3 + k4);

    t[num_steps] = duration;
    u[num_steps] = current_u;

    Solution sol;
    sol.t = t;
    sol.u = u;
    sol.size = size;
    return sol;
}

static Solution euler(source_func f, double lambda, double initial_condition,
                      double duration, double time_step) {
    size_t num_steps = (size_t)ceil(duration / time_step);
    size_t size = num_steps + 1;

    double *t = (double *)malloc(size * sizeof(double));
    double *u = (double *)malloc(size * sizeof(double));

    t[0] = 0.0;
    u[0] = initial_condition;

    double current_t = 0.0;
    double current_u = initial_condition;

    for (size_t i = 0; i < num_steps - 1; ++i) {
        current_u += time_step * derivative(f, lambda, current_t, current_u);
        current_t += time_step;

        t[i + 1] = current_t;
        u[i + 1] = current_u;
    }

    double last_step = duration - current_t;
    current_u += last_step * derivative(f, lambda, current_t, current_u);

    t[num_steps] = duration;
    u[num_steps] = current_u;

    Solution sol;
    sol.t = t;
    sol.u = u;
    sol.size = size;
    return sol;
}

Solution ode_solve(const ODE *ode, const Config *config) {
    switch (config->method) {
    case METHOD_RK4:
        return rk4(ode->source_term, ode->lambda, ode->initial_condition,
                   config->duration, config->time_step);
    case METHOD_EULER:
        return euler(ode->source_term, ode->lambda, ode->initial_condition,
                     config->duration, config->time_step);
    default: {
        Solution empty = {0};
        return empty;
    }
    }
}

double ode_error(const ODE *ode, source_func exact, const Config *config) {
    Solution sol = ode_solve(ode, config);
    double sum_squared_error = 0.0;

    for (size_t i = 0; i < sol.size; ++i) {
        double diff = exact(sol.t[i]) - sol.u[i];
        sum_squared_error += diff * diff;
    }

    free_solution(&sol);
    return sqrt(sum_squared_error);
}

void free_solution(Solution *sol) {
    if (sol->t)
        free(sol->t);
    if (sol->u)
        free(sol->u);
    sol->t = NULL;
    sol->u = NULL;
    sol->size = 0;
}
