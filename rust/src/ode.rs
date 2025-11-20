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

#[inline]
fn rk4<F: Fn(f64) -> f64>(
    f: &F,
    lambda: f64,
    initial_condition: f64,
    duration: f64,
    time_step: f64,
) -> (Vec<f64>, Vec<f64>) {
    let num_steps = (duration / time_step).ceil() as usize;
    let size = num_steps + 1;

    let mut t = vec![0.0; size];
    let mut u = vec![0.0; size];

    unsafe {
        *u.get_unchecked_mut(0) = initial_condition;
    }

    let mut current_t = 0.0;
    let mut current_u = initial_condition;

    let h_2 = time_step / 2.0;
    let h_6 = time_step / 6.0;

    for i in 0..num_steps - 1 {
        let k1 = f(current_t) - lambda * current_u;
        let k2 = f(current_t + h_2) - lambda * (current_u + h_2 * k1);
        let k3 = f(current_t + h_2) - lambda * (current_u + h_2 * k2);
        let k4 = f(current_t + time_step) - lambda * (current_u + time_step * k3);

        current_u += h_6 * (k1 + 2.0 * k2 + 2.0 * k3 + k4);
        current_t += time_step;

        unsafe {
            *t.get_unchecked_mut(i + 1) = current_t;
            *u.get_unchecked_mut(i + 1) = current_u;
        }
    }

    let last_step = duration - current_t;
    let last_h_2 = last_step / 2.0;
    let last_h_6 = last_step / 6.0;

    let k1 = f(current_t) - lambda * current_u;
    let k2 = f(current_t + last_h_2) - lambda * (current_u + last_h_2 * k1);
    let k3 = f(current_t + last_h_2) - lambda * (current_u + last_h_2 * k2);
    let k4 = f(current_t + last_step) - lambda * (current_u + last_step * k3);

    current_u += last_h_6 * (k1 + 2.0 * k2 + 2.0 * k3 + k4);

    unsafe {
        *t.get_unchecked_mut(num_steps) = duration;
        *u.get_unchecked_mut(num_steps) = current_u;
    }

    (t, u)
}

#[inline]
fn euler<F: Fn(f64) -> f64>(
    f: &F,
    lambda: f64,
    initial_condition: f64,
    duration: f64,
    time_step: f64,
) -> (Vec<f64>, Vec<f64>) {
    let num_steps = (duration / time_step).ceil() as usize;
    let size = num_steps + 1;

    let mut t = vec![0.0; size];
    let mut u = vec![0.0; size];

    unsafe {
        *u.get_unchecked_mut(0) = initial_condition;
    }

    let mut current_t = 0.0;
    let mut current_u = initial_condition;

    for i in 0..num_steps - 1 {
        current_u += time_step * (f(current_t) - lambda * current_u);
        current_t += time_step;

        unsafe {
            *t.get_unchecked_mut(i + 1) = current_t;
            *u.get_unchecked_mut(i + 1) = current_u;
        }
    }

    let last_step = duration - current_t;
    current_u += last_step * (f(current_t) - lambda * current_u);

    unsafe {
        *t.get_unchecked_mut(num_steps) = duration;
        *u.get_unchecked_mut(num_steps) = current_u;
    }

    (t, u)
}
