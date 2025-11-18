%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
classdef AnalogBB < Util
    % Description
    %   This class models the analog baseband filter in a receiver.
    %   The model is written generally, i.e. it can model add noise, add filtering, 
    %   non-linear distortion and a DC offset. As most aspects(Noise, Non-linear, DC-offset)
    %   are implementation-specific, at this stage of the project we only onsider the (linear filtering)
    %   The main method is 'do', which expects a signal of the type
    %   Signal_BB.
    % Description end
   
    properties
        NoiseModel      = signal.AdditiveNoise('powerUnit', 'dBm/Hz', 'powerValue', -inf, 'fs', 400e6);
        Filter          = AnalogBBFilter('butter',0,3,200e3,1/(400e6));
        DCOffsetAtInput = 0*1e-3;
        NonLin          = NonLinear(0,inf);   %;
    end
    properties (Hidden)
        %there is nothing to hide;-)
    end
    methods
        function [self]=AnalogBB(NoiseModel,Filter,DCOffsetAtInput)
            if nargin>0
                self.NoiseModel=NoiseModel;
            end
            if nargin>1
                self.Filter=Filter;
            end
            if nargin>2
                self.DCOffsetAtInput=DCOffsetAtInput;
            end
        end
        
        function y=do(self,x)
            %The input 
            x=set_freq(x,0);
            x.signal=x.signal+self.DCOffsetAtInput;
            x=do(self.NonLin,x);
            x=do(self.NoiseModel,x); 
            y=do(self.Filter,x);
        end
        
        function [SigGain_dB,Noise_dBmHz]=GainAndNoise(self,f_Hz)
            if nargin<2
                f_Hz=-16e6:1e3:16e6;
            end
            SigGain_dB=zeros(size(f_Hz))+self.NonLin.Gain_dB;
            Noise_dBmHz=self.NoiseModel.Power_dBmPerHz*ones(size(f_Hz));
            
            H=freqz(self.Filter,f_Hz);
            H2_dB=20*log10(abs(H));
            SigGain_dB=SigGain_dB+H2_dB;
            Noise_dBmHz=Noise_dBmHz+H2_dB;
        end
        function [H]=freqz(self,f_Hz)
           H=freqz(self.Filter,f_Hz);
        end
        
        
    end
end