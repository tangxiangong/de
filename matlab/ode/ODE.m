classdef ODE
    properties
        lambda
        initial_condition
        source_term
    end

    methods
        function obj = ODE(lambda, initial_condition, source_term)
            obj.lambda = lambda;
            obj.initial_condition = initial_condition;
            obj.source_term = source_term;
        end

        function [t, u] = solve(obj, config)
            if(config.method == Method.Euler)
                [t, u] = euler_solver(obj.lambda, obj.initial_condition, ...
                    obj.source_term, config.duration, config.time_step);
            else 
                [t, u] = rk4_solver(obj.lambda, obj.initial_condition, ...
                    obj.source_term, config.duration, config.time_step);
            end
        end
    end
end