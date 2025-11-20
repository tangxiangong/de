module;

#include <cmath>
#include <concepts>
#include <numeric>
#include <ranges>
#include <type_traits>
#include <utility>
#include <vector>

export module de.ode;

using std::vector;

export template <typename F>
concept Source = std::invocable<F, double> &&
                 std::is_same_v<std::invoke_result_t<F, double>, double>;

export enum class Method {
    Euler,
    RK4,
};

export struct Config {
    double duration = 1.0;
    double time_step = 0.01;
    Method method = Method::RK4;
};

export template <Source F> struct ODE {
    double lambda;
    double initial_condition;
    F source_term;

    std::pair<vector<double>, vector<double>> solve(const Config &config) const;

    double error(const F &exact, const Config &config) const;
};

export template <Source F>
std::pair<vector<double>, vector<double>>
rk4(const F &f, double lambda, double initial_condition, double duration,
    double time_step) {
    size_t num_steps = std::ceil(duration / time_step);

    vector<double> t(num_steps + 1);
    vector<double> u(num_steps + 1);

    t[0] = 0.0;
    u[0] = initial_condition;

    double current_t = 0.0;
    double current_u = initial_condition;

    auto derivative = [lambda, &f](double time, double value) {
        return f(time) - lambda * value;
    };

    double k1;
    double k2;
    double k3;
    double k4;

    for (size_t i = 0; i < num_steps - 1; ++i) {
        k1 = derivative(current_t, current_u);
        k2 = derivative(current_t + time_step / 2.0,
                        current_u + (time_step / 2.0) * k1);
        k3 = derivative(current_t + time_step / 2.0,
                        current_u + (time_step / 2.0) * k2);
        k4 = derivative(current_t + time_step, current_u + time_step * k3);

        current_u += (time_step / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
        current_t += time_step;

        t[i + 1] = current_t;
        u[i + 1] = current_u;
    }

    double last_step = duration - current_t;
    k1 = derivative(current_t, current_u);
    k2 = derivative(current_t + last_step / 2.0,
                    current_u + (last_step / 2.0) * k1);
    k3 = derivative(current_t + last_step / 2.0,
                    current_u + (last_step / 2.0) * k2);
    k4 = derivative(current_t + last_step, current_u + last_step * k3);

    current_u += (last_step / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4);

    t[num_steps] = duration;
    u[num_steps] = current_u;

    return std::make_pair(t, u);
}

export template <Source F>
std::pair<vector<double>, vector<double>>
euler(const F &f, double lambda, double initial_condition, double duration,
      double time_step) {
    size_t num_steps = std::ceil(duration / time_step);

    vector<double> t(num_steps + 1);
    vector<double> u(num_steps + 1);

    t[0] = 0.0;
    u[0] = initial_condition;

    double current_t = 0.0;
    double current_u = initial_condition;

    for (size_t i = 0; i < num_steps - 1; ++i) {
        current_u += time_step * (f(current_t) - lambda * current_u);
        current_t += time_step;

        t[i + 1] = current_t;
        u[i + 1] = current_u;
    }

    double last_step = duration - current_t;
    current_u += last_step * (f(current_t) - lambda * current_u);

    t[num_steps] = duration;
    u[num_steps] = current_u;

    return std::make_pair(t, u);
}

template <Source F>
std::pair<vector<double>, vector<double>>
ODE<F>::solve(const Config &config) const {
    switch (config.method) {
    case Method::RK4:
        return rk4(source_term, lambda, initial_condition, config.duration,
                   config.time_step);
    case Method::Euler:
        return euler(source_term, lambda, initial_condition, config.duration,
                     config.time_step);
    }
}

template <Source F>
double ODE<F>::error(const F &exact, const Config &config) const {
    auto [t, u] = solve(config);
    auto squared_errors = std::views::zip(t, u) |
                          std::views::transform([&exact](const auto &pair) {
                              const auto &[time, value] = pair;
                              double diff = exact(time) - value;
                              return diff * diff;
                          });
    auto error = std::sqrt(
        std::accumulate(squared_errors.begin(), squared_errors.end(), 0.0));
    return error;
}
