module utils

import BenchmarkTools: @benchmark, prettytime
import Statistics: mean, std

export bench

function bench(name::AbstractString, func::F) where F
    trial = @benchmark $func()
    times = trial.times
    stats = (
        min=minimum(times),
        max=maximum(times),
        mean=mean(times),
        std=std(times),
    )

    println(name)
    println("min: $(prettytime(stats.min)), mean: $(prettytime(stats.mean)), max: $(prettytime(stats.max)), std: $(prettytime(stats.std))")
end

end # module utils