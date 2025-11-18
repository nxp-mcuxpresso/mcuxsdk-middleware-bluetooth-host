% -------------------------------------------------------------------------------
%  Project   : NRB ranging
%  Content   : white noise source
%  Version   : GIT
%  Author    : Jac Romme (jac.romme@imec-nl.nl)
%  Copyright : Stichting Imec Nederland (http://www.imec-nl.nl)
%  Disclaimer: Imec strictly confidential
% -------------------------------------------------------------------------------
% Description
%   Adds white noise to Signals of type Signal_BB with a user-definable 
%   (using class-constructor) spectral densitity in [dBm/Hz].
%   See also AnalogBB, AnalogBBFilter, NonLinear, NRBLOC_LO, NRBLOC_PA
% Description end 
% -------------------------------------------------------------------------------
classdef SigBB_noise < Util
    properties
        Power_dBmPerHz=-174;
    end
    properties (Hidden)
        color=[];   % for future use
        RandomSeed; % To store the Noise Seed
    end
    methods
        function [NoiseModel]=SigBB_noise(NoisePower_dBmPerHz_in)
            if nargin>0
                NoiseModel.Power_dBmPerHz=NoisePower_dBmPerHz_in;
            end
            NoiseModel.RandomSeed=[];
        end
        function obj = set.Power_dBmPerHz(obj,NoisePower_dBmPerHz_in)
            obj.Power_dBmPerHz = NoisePower_dBmPerHz_in;
        end
        function SignalOut=do(NoiseModel,SignalIn)
            %The voltage of the noise can only be computed, once the impedance level of the input signal is known
            V_rms = sqrt(10^((NoiseModel.Power_dBmPerHz-30)/10)*SignalIn.Impedance/SignalIn.deltaT);
            if V_rms==0 %       to speed up simulations, if no noise is wanted
                SignalOut = SignalIn;
            else
                if ~isempty(NoiseModel.RandomSeed)
                    rng(NoiseModel.RandomSeed);
                end
                TMP                   = randn(2,numel(SignalIn.signal));
                NoiseModel.RandomSeed = rng; %#ok<MCHV3>
                Noise                 = V_rms/sqrt(2)*([1 1i]*TMP);
                SignalOut             = SignalIn;
                SignalOut.signal      = SignalOut.signal + Noise.';
            end
        end
    end
end
