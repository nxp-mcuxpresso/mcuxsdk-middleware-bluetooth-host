%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   Adds (colored) additive noise to signals of type Signal_BB.
% Description end
classdef AdditiveNoise < signal.ColoredNoise
    methods
        function self = AdditiveNoise(varargin)
            self = self@signal.ColoredNoise(varargin{:});
        end
         
        function y = do(self, x)
            if ~isa(x, 'Signal_BB')
                x = Signal_BB(x, 0, self.fs, 50);
            end
            L     = length(x);
            assert(abs(self.fs - 1/x.deltaT) < 1e-10, sprintf('There is a mismatch between the fs (%.1f) used to construct the filter-obj and the fs (%.1f) of the signal.', self.fs, 1/x.deltaT));
            assert(abs(self.impedance - x.Impedance)<1e-15, sprintf('There is a mismatch between the Impedance (%.1f) used to construct the filter-obj and the Impedance (%.1f) of the signal.', self.impedance, x.Impedance));
            noise = self.do@signal.ColoredNoise(L);                         % Generate noise with parent-function
            noise.Start = x.Start;
            noise.fc_Hz = x.fc_Hz;
            y           = x + noise;
        end
    end
end
