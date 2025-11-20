using BenchmarkTools
using BenchmarkTools: prettytime, @benchmark
using Statistics: mean, median, std
using de.ode: Config, ODE, solve, RK4, Euler

function bench(name::AbstractString, ode::ODE, config::Config; samples=10_000)
    trial = @benchmark solve($ode, $config) samples = samples
    times = trial.times
    stats = (
        min=minimum(times),
        max=maximum(times),
        mean=mean(times),
        mid=median(times),
        std=std(times),
    )

    println(name)
    println("min: $(prettytime(stats.min)), mid: $(prettytime(stats.mid)), mean: $(prettytime(stats.mean)), max: $(prettytime(stats.max)), std: $(prettytime(stats.std))")
end

function main()
    rk4_config = Config(1.0, 0.01, RK4)
    euler_config = Config(1.0, 0.01, Euler)
    f(t) = -t
    ode = ODE(1.0, 1.0, f)

    bench("ode-rk4", ode, rk4_config)
    bench("ode-euler", ode, euler_config)
end

main()
