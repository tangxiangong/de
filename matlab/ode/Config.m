classdef Config
    properties
        duration
        time_step
        method
    end

    methods
        function obj = Config(duration, time_step, method)
            obj.duration = duration;
            obj.time_step = time_step;
            obj.method = method;
        end
    end
end