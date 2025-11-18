%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This is the abstract-class for the Tracking engine.
% Description end
classdef (Abstract) TrackingEngineAbstract < Util
    properties (Abstract)
    end
    properties
        Number_Agents = 1;
        SamplingTime = 0.1;
        alpha = 1.2;
        dss_std = 1;
        a_std = 1;
        MCPD_weight = 1;
        fix_anchor_pos = [0,0]';
        initial_cov = [100 0; 0 10];
        quality_index = 1;
        dss_var_params = [35, -4];
        range_process_variance = 1e-2;
        velocity_process_variance = 1e-2;
    end
    
    methods (Abstract)
        Out = do(self, d_ss, Ts, varargin);
    end
    
end