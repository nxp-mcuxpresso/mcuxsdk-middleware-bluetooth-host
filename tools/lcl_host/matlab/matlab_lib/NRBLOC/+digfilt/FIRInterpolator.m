%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This class implements a digital FIRInterpolator. It should act as a
%   substitute for the dsp.FIRInterpolator-class in Matlab. The goal of this 
%   class is to have a license-free substitute.
% Description end

classdef FIRInterpolator < digfilt.FIRFilter
    properties
        InterpolationFactor
    end
    
    methods 
        function self = FIRInterpolator(varargin)
            p = inputParser();
            p.addOptional('InterpolationFactor', 2, @(x) validateattributes(x, {'numeric'}, {'real', 'positive', 'integer', 'scalar'}, mfilename, 'InterpolationFactor', 1));
            p.addOptional('num', [0.5, 0.5], @(x) validateattributes(x, {'numeric'}, {'row'}, mfilename, 'num', 2));        % Must be a row vector, same restriction as in dsp.FIRFilter-object
            p.addOptional('state', []);
            p.parse(varargin{:});
            
            self = self@digfilt.FIRFilter(p.Results.num, p.Results.state);
            self.InterpolationFactor = p.Results.InterpolationFactor;
            self.ismultirate = true;
        end
        
        function z = step(self, x)
            % This implementation is very crude. It simply upsamples and filters. 
            % It does not make use of the polphase decomposition to
            % speed-up the filtering process.            
            y = self.upsample(x);                                           % Using default phase of 1
            z = self.step@digfilt.FIRFilter(y);                             % Call function in superclass
        end
        
        function y = upsample(self, x, varargin)
           p = inputParser();
           p.addOptional('phase', 0, @(x) validateattributes(x, {'numeric'}, {'real', 'nonnegative', 'integer', 'scalar', '<', self.InterpolationFactor}, 'upsample', 'phase'));
           p.parse(varargin{:});
           phase = p.Results.phase;
           
           K = self.InterpolationFactor;
           y = zeros(K*length(x),1);
           y((phase+1):K:K*length(x)) = x;
           if isrow(x)
               y = y.';
           end               
        end
        
        function val = total_interpolation_factor(self)
            val = self.InterpolationFactor;
        end
   end
end