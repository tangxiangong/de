import de.ode: Config, ODE, solve, RK4, Euler
import de.utils: bench

function main()
    rk4_config = Config(1.0, 0.01, RK4)
    euler_config = Config(1.0, 0.01, Euler)
    f(t) = -t
    ode = ODE(1.0, 1.0, f)

    solve(ode, rk4_config)
    solve(ode, euler_config)

    bench("ode-rk4", () -> solve(ode, rk4_config))
    bench("ode-euler", () -> solve(ode, euler_config))
end

main()
