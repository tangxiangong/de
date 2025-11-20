from de.utils import bench
from de.ode import ODE, Method, Config


if __name__ == "__main__":
    rk4_config = Config()
    euler_config = Config(method=Method.Euler)
    ode = ODE(
        1.0,
        1.0,
        lambda t: -t,
    )

    def rk4():
        ode.solve(rk4_config)

    def euler():
        ode.solve(euler_config)

    def rk4_numba():
        ode.solve_numba(rk4_config)

    def euler_numba():
        ode.solve_numba(euler_config)

    bench("ode-rk4", rk4)
    bench("ode-euler", euler)
    bench("ode-rk4-numba", rk4_numba)
    bench("ode-euler-numba", euler_numba)
