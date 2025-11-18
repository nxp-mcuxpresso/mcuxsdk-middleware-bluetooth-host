classdef PhaseNoiseSource < Util
    %--------------------------------------------------------------------------
    % Project   : NRB Ranging
    % Version   : GIT
    % Author    : Jac Romme (jac.romme@imec-nl.nl)
    % Copyright : Stichting Imec Nederland (http://www.imec-nl.nl)
    % Disclaimer: Imec strictly confidential
    %--------------------------------------------------------------------------
    % Description
    %   Adds pink, flicker,1/f^(alpha) noise to Signals of type Signal_BB with a user-definable
    %  (using class-constructor) spectral densitity in [dBm/Hz] at a user-definable reference frequency.
    %   See also HAR_LNA, HAR_Mixer, HAR_AnalogFilter, HAR_RXFE
    % Description end
    %---------------------------------------------------------------------------------------------------
    % Change Log
    % Date        Author      Changes
    % 08/06/2016  Jac Romme   1e version
    %---------------------------------------------------------------------------------------------------
    properties
        % A mixture of phase-noise components can be defined, using the arrays 
        % PN_dBcPerHzAtRefFreq,RefFreq_Hz, alpha, whihc should all have
        % equal length. %The length of all arrays describes the number of 
        % phase-noise components. 
        PN_dBcPerHzAtRefFreq=[-90 -90]; %[dBc] %The phase noise power in dBc at the reference-frequency
        RefFreq_Hz=[50e3 100e3]         %[Hz]  %The reference frequency 
        alpha=[1 0];                    %[]    %slope of phase noise. Has been tested with 0 and 1, other may work as well.
        K=10000;                        %[]    %expressed the length of the filter coefficient, the longer, the more realistic, i.e. closer to DC is will be valid
        USF=40;                         %[]    %upsampling factor(using linear interpolation) We select a very high upsampling factor(USF), to be able to model 1/f noise AND use an air-interface with a larger bandwidth. 
        deltaT=1/(400e6); 
    end
    properties (Hidden)
        h
        WhiteNoise = white_phase_noise(0);
        FilterState=[];
    end
    methods
        function [self]=PhaseNoiseSource(AlphaIn,PN_dBcPerHzAtRefFreqIn,RefFreq_HzIn)
            if nargin>0
                self.alpha=AlphaIn;
            end
            if nargin>1
                self.PN_dBcPerHzAtRefFreq=PN_dBcPerHzAtRefFreqIn;
            end
            if nargin>2
                self.RefFreq_Hz=RefFreq_HzIn;
            end
            ComputeFIRCoeff(self)
            if nargout==0
                test(self)
            end
        end
        function PSD_dBcPerHz=freqz(self,f_Hz)
            if nargin<2
                f_Hz=16*1024;
            end
            f_sample=1/self.deltaT;
            [H,W]=freqz(self.h,1,f_Hz,f_sample/self.USF);
            [~,B] = interp(ones(1,10),self.USF);    %Trick matlab to get just the coefficients of the interpolation filter B.
            [H_interp,~]=freqz(B,1,f_Hz,f_sample);
            PSD_wns=freqz(self.WhiteNoise,f_Hz);
            PSD_dBcPerHz=10*log10(abs(H.*H_interp/max(abs(H_interp))).^2)+10*log10(PSD_wns);
            if nargout==0
                figure
                semilogx(W/1e3,PSD_dBcPerHz)
                xlabel('freq [kHz]')
            end
        end
        function ComputeFIRCoeff(self)
            if (length(self.alpha)==1)&(self.alpha==0)
                h_tmp(1)=1;
            else
                for cnt=1:length(self.alpha)
                    h_tmp=zeros(1,self.K);
                    h_tmp(1)=1;
                
                    for k=1:self.K-1
                        h_tmp(k+1)=h_tmp(k)*(self.alpha(cnt)/2+k-1)/k;
                    end
                    window=hanning(2*length(h_tmp)-1).';
                    h_tmp=h_tmp.*window(length(h_tmp):end);
                    f_sample=1/self.deltaT;
                    H = freqz(h_tmp,1,self.RefFreq_Hz(cnt)*[1 1.001],f_sample/self.USF);
                    if cnt==1
                        self.h=h_tmp/(abs(H(1)))*10.^(self.PN_dBcPerHzAtRefFreq(cnt)/20);
                    else
                        self.h=self.h+h_tmp/(abs(H(1)))*10.^(self.PN_dBcPerHzAtRefFreq(cnt)/20);
                    end
                end
            end
        end
        function SignalOut=do(self,SignalIn)
            if isempty(self.PN_dBcPerHzAtRefFreq)|isinf(self.PN_dBcPerHzAtRefFreq)
                SignalOut = SignalIn;
            else
                PhaseNoise            = gen_noise(self,SignalIn);
                SignalOut             = SignalIn;
                signal_tmp            = SignalOut.signal;
                SignalOut.signal      = signal_tmp.*exp(1i*PhaseNoise);
            end
        end
        
        function PinkNoise=gen_noise(self,SignalIn)
            if abs(self.deltaT-SignalIn.deltaT)>1e-12;
                error('sample rate do not match')
            end
            if isempty(self.h)
                self.deltaT=SignalIn.deltaT;
                ComputeFIRCoeff(self);
            end
            
            if isempty(self.FilterState)
                SignalInTmp=SignalIn;
                SignalInTmp.signal=zeros(self.K+1,1);
                WhiteNoiseSig=gen_noise(self.WhiteNoise,SignalIn);
                [~,self.FilterState]=filter(self.h,1,WhiteNoiseSig);
            end
            SignalInTmp=SignalIn;
            SignalInTmp.signal=zeros([ceil(numel(SignalInTmp.signal)/self.USF) 1]);
            WhiteNoiseSig=gen_noise(self.WhiteNoise,SignalInTmp); %Generate white noise
            [PinkNoise,self.FilterState]=filter(self.h,1,WhiteNoiseSig,self.FilterState); %Filter Noise to 'color' it.
            PinkNoise=interp(PinkNoise,self.USF)/sqrt(self.USF);      %Normalize for the interpolation filter gain
            PinkNoise=PinkNoise(1:length(SignalIn));
        end
        
        function test(self)
            close all
            LO=Signal_BB(ones(1,10000000),0,self.deltaT);
            LO=SetMeanPower_dBm(LO,0);
            x=do(self,LO);
            [f,PSD_LO]=psd(LO);
            hold on
            [f,PSD_x]=psd(x);
            [H]=freqz(self,f);
            figure
            plot(f/1e3,10*log10(PSD_LO))
            hold on
            plot(f/1e3,10*log10(PSD_x),'r')
            plot(f/1e3,H,'g')
            
            set(gca, 'XScale', 'log')
            ylabel('dBm/Hz = dBc/Hz (as reference is 0dBm)')
            xlabel('freq[kHz]')
            legend('Sim:No Phase Noise','Sim:w Phase Noise','freqz phase noise')
        end
        
    end
end

% Old code
%         function OK=TEST(NM1)
%             NM2=copy(NM1);
%             Signal1=do(NM1,[1000],0,1e-6,50);
%             Signal2a=do(NM2,[400],0,1e-6,50);
%             Signal2b=do(NM2,[600],0,1e-6,50);
%             Signal2=[Signal2a;Signal2b];
%             OK=isequal(Signal1,Signal2);
%
%         end
