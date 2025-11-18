%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This class implements a digital FIR-filter class. It should act as a
%   substitute for the dsp.FIRfilter-class in Matlab. The goal of this 
%   class is to have a license-free substitute.
% Description end

classdef FIRFilter < digfilt.digfiltabstract
    properties
        num
    end
    
    methods 
        function self = FIRFilter(varargin)
            function validate_state(x)
                if isempty(x)
                    % do nothing
                else
                    nElem = length(self.num)-1;
                    validateattributes(x, {'numeric'}, {'vector', 'numel', nElem}, mfilename, 'state', 2);
                end
            end
            
            p = inputParser();
            p.addOptional('num', [0.5, 0.5], @(x) validateattributes(x, {'numeric'}, {'row'}, mfilename, 'num', 1));        % Must be a row vector, same restriction as in dsp.FIRFilter-object
            p.addOptional('state', []);
            p.parse(varargin{:});
            
            self.num   = p.Results.num;
            self.state = p.Results.state;
            validate_state(p.Results.state);
            self.state = p.Results.state;
        end
        
        function [num, den] = tf(self)
            num = self.num;
            den = 1;
        end
   end
end