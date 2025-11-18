%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This is the abstract-class for the CFO-estimator.
% Description end

classdef CfoEstimatorAbstract < matlab.mixin.SetGet & matlab.mixin.Copyable
    properties (Abstract)
    end
    
    methods (Abstract)
        CFOest = do(self, CFO_A, CFO_B, varargin);
    end
end