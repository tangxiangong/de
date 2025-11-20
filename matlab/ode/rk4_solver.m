function [t, u] = rk4_solver(lambda, initial_condition, source_term, ...
    duration, time_step)
num_steps = ceil(duration / time_step);
t = zeros(num_steps + 1, 1);
u = zeros(num_steps + 1, 1);

t(1) = 0.0;
u(1) = initial_condition;

current_t = 0.0;
current_u = initial_condition;

derivative = @(t, u) lambda * u - source_term(t);

for i=1:num_steps-1
    k1 = derivative(current_t, current_u);
    k2 = derivative(current_t + time_step / 2.0, ...
                    current_u + (time_step / 2.0) * k1);
    k3 = derivative(current_t + time_step / 2.0, ...
                    current_u + (time_step / 2.0) * k2);
    k4 = derivative(current_t + time_step, ...
                    current_u + time_step * k3);

    current_u = current_u + (time_step / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
    current_t = current_t + time_step;
    
    t(i+1) = current_t;
    u(i+1) = current_u;
end

last_step = duration - current_t;
k1 = derivative(current_t, current_u);
k2 = derivative(current_t + last_step / 2.0, ...
                    current_u + (last_step / 2.0) * k1);
k3 = derivative(current_t + last_step / 2.0, ...
                    current_u + (last_step / 2.0) * k2);
k4 = derivative(current_t + last_step, ...
                    current_u + last_step * k3);

current_u = current_u + (time_step / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
t(end) = duration;
u(end) = current_u;
end