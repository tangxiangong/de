import de.ode;
import de.utils;

int main() {
    Config rk4_config{};
    Config euler_config{.method = Method::Euler};
    auto f = [](double t) -> double { return -t; };
    ODE ode{.lambda = 1.0, .initial_condition = 1.0, .source_term = f};

    auto euler_solver = [ode, euler_config]() {
        return ode.solve(euler_config);
    };
    auto rk4_solver = [ode, rk4_config]() { return ode.solve(rk4_config); };

    bench("ode-euler", euler_solver, 10000);
    bench("ode-rk4", rk4_solver, 10000);

    return 0;
}
