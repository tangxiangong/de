function [t, u] = euler_solver(lambda, initial_condition, source_term, ...
    duration, time_step)
num_steps = ceil(duration / time_step);
t = zeros(num_steps + 1, 1);
u = zeros(num_steps + 1, 1);

t(1) = 0.0;
u(1) = initial_condition;

current_t = 0.0;
current_u = initial_condition;

for i=1:num_steps-1
    current_u = current_u + (source_term(current_t) ...
        - lambda * current_u) * time_step;
    current_t = current_t + time_step;
    
    t(i+1) = current_t;
    u(i+1) = current_u;
end

last_step = duration - current_t;
current_u = current_u + (source_term(current_t) ...
    - lambda * current_u) * last_step;
t(end) = duration;
u(end) = current_u;
end