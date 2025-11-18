%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This class implements a digital IIR-filter. It should act as a
%   substitute for the dsp.IIRFilter-class in Matlab. The goal of this 
%   class is to have a license-free substitute.
% Description end

classdef IIRFilter < digfilt.digfiltabstract
    properties
        num
        den
    end
    
    methods
        function self = IIRFilter(varargin)
            function validate_state(x)
                if isempty(x)
                    % do nothing
                else
                    nElem = max(length(self.num), length(self.den))-1;
                    validateattributes(x, {'numeric'}, {'vector', 'numel', nElem}, mfilename, 'state', 3);
                end
            end
            
            p = inputParser();
            p.addOptional('num', [1, 1], @(x) validateattributes(x, {'numeric'}, {'row'}, mfilename, 'num', 1));    % Must be a row vector, same restriction as in dsp.IIRFilter-object
            p.addOptional('den', [1, 0.1], @(x) validateattributes(x, {'numeric'}, {'row'}, mfilename, 'den', 2));    % Must be a row vector, same restriction as in dsp.IIRFilter-object
            p.addOptional('state', []);
            p.parse(varargin{:});
            self.num   = p.Results.num;
            self.den   = p.Results.den;
            validate_state(p.Results.state);
            self.state = p.Results.state;
        end
        
        function [num, den] = tf(self)
            % gets the transfer-function 
            num = self.num;
            den = self.den;
        end
   end
end