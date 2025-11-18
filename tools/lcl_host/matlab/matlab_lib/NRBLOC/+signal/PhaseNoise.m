%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   Adds (colored) phase noise to signals of type Signal_BB.
% Description end
classdef PhaseNoise < signal.ColoredNoise
    methods
        function self = PhaseNoise(varargin)
            p = inputParser;
            p.KeepUnmatched = true;
            % Let the ColoredNoise-constructor handle the checking
            p.addParameter('powerUnit', 'dBc/Hz', @(x) strcmpi(x, 'dbc/hz'));  % Check will be done by set-method
            p.addParameter('powerValue', -80);      
            p.addParameter('noiseColor', 'pink');
            p.addParameter('alpha', 1);
            p.addParameter('complexNoise', false, @(x) x == false);         % must always be false
            p.parse(varargin{:})
            
            self = self@signal.ColoredNoise('powerUnit', 'dBm/Hz', ...            % must be something different than 'dBc/Hz'
                                            'powerValue', p.Results.powerValue, ...
                                            'noiseColor', p.Results.noiseColor, ...
                                            'alpha', p.Results.alpha, ...
                                            'complexNoise', false, ...
                                            p.Unmatched);
            self.PowerUnitsOpts = {'dBc/Hz'};
            self.powerUnit = p.Results.powerUnit;
        end
        
        function y = do(self, x)
            x     = Signal_BB(x);
            L     = length(x);
            noise = self.do@signal.ColoredNoise(L);                         % Generate noise with parent-function
            noise = double(noise);
            phaseNoise = Signal_BB(exp(1i*noise), 0, x.deltaT, x.Impedance, x.Start);
            y          = x .* phaseNoise;
        end
        
        function x = set_power(self, x, varargin)
            A = self.db2vrms(self.powerValue - self.filter_offset());
            x = A*x;
        end
        
        function PSD_closed_form(~, varargin)
            error('Do not use the PSD function for a phase noise object')
        end
        
        function Spectrum = Spectrum_PhaseNoise_closed_form(self, freq) %Computes in closed-form the PSD of the phase noise.
            Spectrum  = self.freq_response('w', freq, 'fs', self.fs);
            offset_db = self.filter_offset();
            A         = self.db2vrms(self.powerValue - offset_db);
            Spectrum  = abs(A*Spectrum).^2 / self.upsample_factor;
        end
    end
end
