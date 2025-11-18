%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This class implements a digital differentiator-filter.
% Description end

classdef Differentiator < digfilt.FIRFilter
    methods
        function self = Differentiator(varargin)
            self@digfilt.FIRFilter([1, -1]/2, varargin{:});
        end
   end
end