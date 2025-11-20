#include "ode.h"
#include "utils.h"
#include <stdio.h>

double source_term(double t) { return -t; }

typedef struct {
  ODE ode;
  Config config;
} BenchContext;

void run_solve(void *ctx) {
  BenchContext *context = (BenchContext *)ctx;
  Solution sol = ode_solve(&context->ode, &context->config);
  free_solution(&sol);
}

int main() {
  Config rk4_config = {
      .duration = 1.0, .time_step = 0.01, .method = METHOD_RK4};

  Config euler_config = {
      .duration = 1.0, .time_step = 0.01, .method = METHOD_EULER};

  ODE ode = {
      .lambda = 1.0, .initial_condition = 1.0, .source_term = source_term};

  BenchContext euler_ctx = {.ode = ode, .config = euler_config};
  BenchContext rk4_ctx = {.ode = ode, .config = rk4_config};

  bench("ode-euler", run_solve, &euler_ctx, 10000);
  bench("ode-rk4", run_solve, &rk4_ctx, 10000);

  return 0;
}
