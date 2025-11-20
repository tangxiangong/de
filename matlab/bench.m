function bench(name, func, epoch)
elapseds = zeros(epoch, 1);
for i = 1:epoch
    elapseds(i) = timeit(func);
end

elapseds_mean = mean(elapseds);
elapseds_std = std(elapseds);
elapseds_min = min(elapseds);
elapseds_max = max(elapseds);

[mean_val, mean_unit] = format_time(elapseds_mean);
[std_val, std_unit] = format_time(elapseds_std);
[min_val, min_unit] = format_time(elapseds_min);
[max_val, max_unit] = format_time(elapseds_max);

fprintf("%s\n", name);
fprintf("mean: %.3f %s, std: %.3f %s, min: %.3f %s, max: %.3f %s\n", ...
    mean_val, mean_unit, std_val, std_unit, min_val, min_unit, max_val, max_unit);
end

function [value, unit] = format_time(seconds)
if seconds < 1e-6
    value = seconds * 1e9;
    unit = "ns";
elseif seconds < 1e-3
    value = seconds * 1e6;
    unit = "µs";
elseif seconds < 1
    value = seconds * 1e3;
    unit = "ms";
else
    value = seconds;
    unit = "s";
end
end