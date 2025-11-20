module ode

export Config, Method, ODE, solve, error

@enum Method Euler RK4

Base.@kwdef struct Config
    duration::Float64 = 1.0
    time_step::Float64 = 0.01
    method::Method = RK4
end

struct ODE
    lambda::Float64
    initial_condition::Float64
    source_term::Function
end

function rk4(f, lambda, initial_condition, duration, time_step)
    num_steps = ceil(Int, duration / time_step)
    t = zeros(num_steps + 1)
    u = zeros(num_steps + 1)
    t[1] = 0.0
    u[1] = initial_condition
    derivative(t, u) = f(t) - lambda * u
    current_t = 0.0
    current_u = initial_condition
    h_2 = time_step / 2.0
    h_6 = time_step / 6.0
    @inbounds for i in 1:num_steps-1
        k1 = derivative(current_t, current_u)
        k2 = derivative(current_t + h_2, current_u + h_2 * k1)
        k3 = derivative(current_t + h_2, current_u + h_2 * k2)
        k4 = derivative(current_t + time_step, current_u + time_step * k3)
        current_u += h_6 * (k1 + 2.0 * k2 + 2.0 * k3 + k4)
        current_t += time_step
        t[i+1] = current_t
        u[i+1] = current_u
    end
    last_step = duration - current_t
    last_h_2 = last_step / 2.0
    last_h_6 = last_step / 6.0
    k1 = derivative(current_t, current_u)
    k2 = derivative(current_t + last_h_2, current_u + last_h_2 * k1)
    k3 = derivative(current_t + last_h_2, current_u + last_h_2 * k2)
    k4 = derivative(current_t + last_step, current_u + last_step * k3)
    current_u += last_h_6 * (k1 + 2.0 * k2 + 2.0 * k3 + k4)
    @inbounds t[num_steps] = duration
    @inbounds u[num_steps] = current_u
    (t, u)
end

function euler(f, lambda, initial_condition, duration, time_step)
    num_steps = ceil(Int, duration / time_step)
    t = zeros(num_steps + 1)
    u = zeros(num_steps + 1)
    t[1] = 0.0
    u[1] = initial_condition
    current_t = 0.0
    current_u = initial_condition
    @inbounds for i in 1:num_steps-1
        current_u += time_step * (f(current_t) - lambda * current_u)
        current_t += time_step
        t[i+1] = current_t
        u[i+1] = current_u
    end
    last_step = duration - current_t
    current_u += last_step * (f(current_t) - lambda * current_u)
    @inbounds t[num_steps] = duration
    @inbounds u[num_steps] = current_u
    (t, u)
end

function solve(ode::ODE, config::Config)
    if config.method == RK4
        return rk4(ode.source_term, ode.lambda, ode.initial_condition,
            config.duration, config.time_step)
    elseif config.method == Euler
        return euler(ode.source_term, ode.lambda, ode.initial_condition,
            config.duration, config.time_step)
    end
end

function error(ode::ODE, exact::Function, config::Config)
    (t, u) = solve(ode, config)
    return sqrt(sum((exact(ti) - ui)^2 for (ti, ui) in zip(t, u)))
end

end # module ode
