from enum import Enum
from typing import Callable, Annotated, Literal
import numpy as np
import numpy.typing as npt
from math import ceil
from numba import jit, njit

Vector = Annotated[npt.NDArray[np.float64], Literal["N"]]


class Method(Enum):
    RK4 = 1
    Euler = 2


class Config(object):
    def __init__(
        self,
        duration: float = 1.0,
        time_step: float = 0.01,
        method: Method = Method.RK4,
    ):
        self.duration: float = duration
        self.time_step: float = time_step
        self.method: Method = method


def _rk4_impl(
    lambda_: float,
    initial_condition: float,
    source_term: Callable[[float], float],
    duration: float,
    time_step: float,
) -> tuple[Vector, Vector]:
    num_steps = ceil(duration / time_step)
    t = np.zeros(num_steps + 1)
    u = np.zeros(num_steps + 1)
    t[0] = 0.0
    u[0] = initial_condition

    current_t = 0.0
    current_u = initial_condition

    def derivative(t, u):
        return source_term(t) - lambda_ * u

    for i in range(num_steps - 1):
        k1 = derivative(current_t, current_u)
        k2 = derivative(current_t + time_step / 2.0, current_u + (time_step / 2.0) * k1)
        k3 = derivative(current_t + time_step / 2.0, current_u + (time_step / 2.0) * k2)
        k4 = derivative(current_t + time_step, current_u + time_step * k3)
        current_u += (time_step / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4)
        current_t += time_step
        t[i + 1] = current_t
        u[i + 1] = current_u

    last_step = duration - current_t
    k1 = derivative(current_t, current_u)
    k2 = derivative(current_t + last_step / 2.0, current_u + (last_step / 2.0) * k1)
    k3 = derivative(current_t + last_step / 2.0, current_u + (last_step / 2.0) * k2)
    k4 = derivative(current_t + last_step, current_u + last_step * k3)
    current_u += (last_step / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4)

    t[num_steps] = duration
    u[num_steps] = current_u

    return t, u


def _euler_impl(
    lambda_: float,
    initial_condition: float,
    source_term: Callable[[float], float],
    duration: float,
    time_step: float,
) -> tuple[Vector, Vector]:
    num_steps = ceil(duration / time_step)
    t = np.zeros(num_steps + 1)
    u = np.zeros(num_steps + 1)
    t[0] = 0.0
    u[0] = initial_condition

    current_t = 0.0
    current_u = initial_condition

    for i in range(num_steps - 1):
        k = source_term(current_t) - lambda_ * current_u
        current_u += time_step * k
        current_t += time_step
        t[i + 1] = current_t
        u[i + 1] = current_u

    last_step = duration - current_t
    k = source_term(current_t) - lambda_ * current_u
    current_u += last_step * k

    t[num_steps] = duration
    u[num_steps] = current_u

    return t, u


def rk4(
    lambda_: float,
    initial_condition: float,
    source_term: Callable[[float], float],
    duration: float,
    time_step: float,
) -> tuple[Vector, Vector]:
    return _rk4_impl(lambda_, initial_condition, source_term, duration, time_step)


def euler(
    lambda_: float,
    initial_condition: float,
    source_term: Callable[[float], float],
    duration: float,
    time_step: float,
) -> tuple[Vector, Vector]:
    return _euler_impl(lambda_, initial_condition, source_term, duration, time_step)


rk4_numba = jit(nopython=True)(_rk4_impl)

euler_numba = jit(nopython=True)(_euler_impl)


class ODE(object):
    def __init__(
        self,
        lambda_: float,
        initial_condition: float,
        source_term: Callable[[float], float],
    ):
        self.lambda_: float = lambda_
        self.initial_condition: float = initial_condition
        self.source_term: Callable[[float], float] = source_term
        self._numba_source_term = None

    def _get_numba_source_term(self):
        if self._numba_source_term is None:
            self._numba_source_term = njit(self.source_term)
        return self._numba_source_term

    def solve(self, config: Config) -> tuple[Vector, Vector]:
        if config.method == Method.RK4:
            return rk4(
                self.lambda_,
                self.initial_condition,
                self.source_term,
                config.duration,
                config.time_step,
            )
        else:
            return euler(
                self.lambda_,
                self.initial_condition,
                self.source_term,
                config.duration,
                config.time_step,
            )

    def solve_numba(self, config: Config) -> tuple[Vector, Vector]:
        numba_source = self._get_numba_source_term()
        if config.method == Method.RK4:
            return rk4_numba(
                self.lambda_,
                self.initial_condition,
                numba_source,
                config.duration,
                config.time_step,
            )
        else:
            return euler_numba(
                self.lambda_,
                self.initial_condition,
                numba_source,
                config.duration,
                config.time_step,
            )
