%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This class implements a digital integrator-filter.
% Description end

classdef Integrator < digfilt.IIRFilter
    methods
        function self = Integrator(varargin)
            self@digfilt.IIRFilter(1, [1, -1], varargin{:});
        end
   end
end