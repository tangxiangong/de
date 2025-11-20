clc
clear
close all

lambda = 1.0;
initial_condition = 1.0;
source_term = @(t) -t;

duration = 1.0;
time_step = 0.01;

euler_config = Config(duration, time_step, Method.Euler);
rk4_config = Config(duration, time_step, Method.RK4);

eq = ODE(lambda, initial_condition, source_term);

rk4 = @() eq.solve(rk4_config);
euler = @() eq.solve(euler_config);

bench("ode-rk4", rk4, 1000);
bench("ode-euler", euler, 1000);