pub struct Config {
    pub duration: f64,
    pub time_step: f64,
    pub method: Method,
}

impl Default for Config {
    fn default() -> Self {
        Config {
            duration: 1.0,
            time_step: 0.01,
            method: Method::RK4,
        }
    }
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum Method {
    Euler,
    RK4,
}

/// du/dt + \lambda u = f(t), u(0) = u\_0
#[derive(Debug, Clone)]
pub struct ODE<F: Fn(f64) -> f64> {
    pub lambda: f64,
    pub initial_condition: f64,
    pub source_term: F,
}

impl<F: Fn(f64) -> f64> ODE<F> {
    pub fn solve(&self, config: &Config) -> (Vec<f64>, Vec<f64>) {
        match config.method {
            Method::Euler => euler(
                &self.source_term,
                self.lambda,
                self.initial_condition,
                config.duration,
                config.time_step,
            ),
            Method::RK4 => rk4(
                &self.source_term,
                self.lambda,
                self.initial_condition,
                config.duration,
                config.time_step,
            ),
        }
    }

    pub fn error(&self, exact: impl Fn(f64) -> f64, config: &Config) -> f64 {
        let (t, u) = self.solve(config);
        t.into_iter()
            .zip(u)
            .map(|(t, u)| (u - exact(t)).powi(2))
            .sum::<f64>()
            .sqrt()
    }
}

fn rk4<F: Fn(f64) -> f64>(
    f: &F,
    lambda: f64,
    initial_condition: f64,
    duration: f64,
    time_step: f64,
) -> (Vec<f64>, Vec<f64>) {
    let num_steps = (duration / time_step).ceil() as usize;

    let mut t = Vec::with_capacity(num_steps + 1);
    let mut u = Vec::with_capacity(num_steps + 1);

    t.push(0.0);
    u.push(initial_condition);

    let mut current_t = 0.0;
    let mut current_u = initial_condition;

    let derivative = |time: f64, value: f64| f(time) - lambda * value;

    let mut k1;
    let mut k2;
    let mut k3;
    let mut k4;

    for _ in 0..num_steps - 1 {
        k1 = derivative(current_t, current_u);
        k2 = derivative(
            current_t + time_step / 2.0,
            current_u + (time_step / 2.0) * k1,
        );
        k3 = derivative(
            current_t + time_step / 2.0,
            current_u + (time_step / 2.0) * k2,
        );
        k4 = derivative(current_t + time_step, current_u + time_step * k3);

        current_u += (time_step / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
        current_t += time_step;

        t.push(current_t);
        u.push(current_u);
    }

    let last_step = duration - current_t;
    k1 = derivative(current_t, current_u);
    k2 = derivative(
        current_t + last_step / 2.0,
        current_u + (last_step / 2.0) * k1,
    );
    k3 = derivative(
        current_t + last_step / 2.0,
        current_u + (last_step / 2.0) * k2,
    );
    k4 = derivative(current_t + last_step, current_u + last_step * k3);

    current_u += (last_step / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4);

    t.push(duration);
    u.push(current_u);

    (t, u)
}

fn euler<F: Fn(f64) -> f64>(
    f: &F,
    lambda: f64,
    initial_condition: f64,
    duration: f64,
    time_step: f64,
) -> (Vec<f64>, Vec<f64>) {
    let num_steps = (duration / time_step).ceil() as usize;

    let mut t = Vec::with_capacity(num_steps + 1);
    let mut u = Vec::with_capacity(num_steps + 1);

    t.push(0.0);
    u.push(initial_condition);

    let mut current_t = 0.0;
    let mut current_u = initial_condition;

    for _ in 0..num_steps - 1 {
        current_u += time_step * (f(current_t) - lambda * current_u);
        current_t += time_step;

        t.push(current_t);
        u.push(current_u);
    }

    let last_step = duration - current_t;
    current_u += last_step * (f(current_t) - lambda * current_u);

    t.push(duration);
    u.push(current_u);

    (t, u)
}
