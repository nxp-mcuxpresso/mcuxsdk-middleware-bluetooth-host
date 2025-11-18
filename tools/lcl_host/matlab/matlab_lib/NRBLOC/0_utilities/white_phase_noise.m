% -------------------------------------------------------------------------------
%  Project   : NRB ranging
%  Content   : white phase_noise source
%  Version   : GIT
%  Author    : Jac Romme (jac.romme@imec-nl.nl)
%  Copyright : Stichting Imec Nederland (http://www.imec-nl.nl)  (or IMEC)
%  Disclaimer: Imec strictly confidential
% -------------------------------------------------------------------------------
% Description
%   Adds white noise to Signals of type Signal_BB with a user-definable
%   (using class-constructor) spectral densitity in [dBm/Hz].
%   See also Signal_BB, LNA, AnalogFilter, NonLinear
% Description end
% -------------------------------------------------------------------------------
classdef white_phase_noise < Util
    properties
        PhaseNoise_dBcPerHz=-100;
        RandomSeed; % To store the Noise Seed
    end
    properties (Hidden)
        color=[];   % for future use
    end
    methods
        function [self]=white_phase_noise(PhaseNoise_dBcPerHz_in)
            if nargin>0
                self.PhaseNoise_dBcPerHz=PhaseNoise_dBcPerHz_in;
            end
            self.RandomSeed=[];
        end
        function self = set.PhaseNoise_dBcPerHz(self,PhaseNoise_dBcPerHz_in)
            self.PhaseNoise_dBcPerHz = PhaseNoise_dBcPerHz_in;
        end
        function SignalOut=do(self,SignalIn)
            rad_rms = sqrt(10^((self.PhaseNoise_dBcPerHz)/10)/SignalIn.deltaT);
            if rad_rms==0 % to speed up simulations, if no noise is wanted
                SignalOut = SignalIn;
            else
                PhaseNoise            = gen_noise(self,SignalIn);
                SignalOut             = SignalIn;
                signal_tmp            = SignalOut.signal;
                SignalOut.signal      = signal_tmp.*exp(1i*PhaseNoise);
            end
        end
        function PhaseNoise=gen_noise(self,SignalIn)
            %The voltage of the noise can only be computed, once the impedance level of the input signal is known
            rad_rms = sqrt(10^((self.PhaseNoise_dBcPerHz)/10)/SignalIn.deltaT);
            if ~isempty(self.RandomSeed)
                rng(self.RandomSeed);
            end
            TMP                   = randn(size(SignalIn.signal));
            self.RandomSeed       = rng; %#ok<MCHV3>
            PhaseNoise            = rad_rms*TMP;
        end
        function PSD=freqz(self,f_Hz)
            PSD=(10^((self.PhaseNoise_dBcPerHz)/10))*ones(size(f_Hz));
        end
        function []=test(self)
            close all;
            DeltaT=1/32e6;
            LO=Signal_BB(ones(1,1e6),0,DeltaT);
            LO=SetMeanPower_dBm(LO,0);
            x=do(self,LO);
            psd(x)
            ylabel('dBm/Hz = dBc/Hz (as reference is 0dBm)')
        end
    end
end

