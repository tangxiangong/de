use criterion::{Criterion, criterion_group, criterion_main};
use de::ode::{Config, Method, ODE};
use std::hint::black_box;

fn criterion_benchmark(c: &mut Criterion) {
    let ode = ODE {
        lambda: 1.0,
        initial_condition: 1.0,
        source_term: |t| -t,
    };
    let rk4_config = Config::default();
    let euler_config = Config {
        method: Method::Euler,
        ..Default::default()
    };
    c.bench_function("ode-euler", |b| {
        b.iter(|| {
            let _ = ode.solve(black_box(&euler_config));
        })
    });
    c.bench_function("ode-rk4", |b| {
        b.iter(|| {
            let _ = ode.solve(black_box(&rk4_config));
        })
    });
}

criterion_group!(benches, criterion_benchmark);
criterion_main!(benches);
