%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This class implements a dummy tracking-engine. The intention of this 
%   is to support systems without tracking capabilities.
% Description end

classdef Estimator_Passthrough < TrackingEngineAbstract    
    properties
    end
    
    methods
        function [x, v] = do(~, d_ss, ~, varargin)
            x = d_ss;
            v = NaN;
        end
    end
end

