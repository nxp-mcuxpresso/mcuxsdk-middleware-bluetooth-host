%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
classdef NRBLOC_LO < handle
% Description
%   Local Oscillator model that generates a contineous-phase baseband-
%   equivalent signal of which at each value in 't_freq_change' the 
%   frequency is increased by the corresponding value in 'DeltaFreq'. 
%   The start frequency is given by 'InitFreq'. A zero frequency in the 
%   baseband-equivalent model corresponds to a frequency of fc in the 
%   passband. The initial phase is given by 'InitPhase'.
%   This model is fully deterministic can be used both in simulations and in
%   analytical computations. 
% Description end
    
    properties
        InitPhase                                                           %[rad] phase of LO at t=0
        InitFreq                                                            %[Hz] baseband equivalent frequence of LO at t=0
        t_freq_change                                                       %[sec] time at which the frequency is changed
        DeltaFreq                                                           %[Hz] Amount the frequency is changed, see above
        fc                                                                  %[Hz] passband centre-frequency equal to 0 [Hz] in baseband-equivalent frequency.
        PNS           
    end
    methods
        function self = NRBLOC_LO(varargin)
           p = inputParser;
           p.KeepUnmatched = true;
           p.addParameter('InitPhase', 0, @(x) validateattributes(x, {'numeric'}, {'scalar'}, mfilename, 'InitPhase') );
           p.addParameter('InitFreq', 2.4e4, @(x) validateattributes(x, {'numeric'}, {'scalar'}, mfilename, 'InitFreq') );
           p.addParameter('t_freq_change', 2.5e-6 * (1:4), @(x) validateattributes(x, {'numeric'}, {'vector'}, mfilename, 't_freq_change') );
           p.addParameter('DeltaFreq', ones(1,4)*1e5, @(x) validateattributes(x, {'numeric'}, {'vector'}, mfilename, 'DeltaFreq') );
           p.addParameter('fc', 2.4e9, @(x) validateattributes(x, {'numeric'}, {'scalar'}, mfilename, 'fc') );
           p.addParameter('PNS', signal.PhaseNoise, @(x) validateattributes(x, {'signal.PhaseNoise'}, {}, mfilename, 'PNS') );
           p.parse(varargin{:});
           
           self.InitPhase     = p.Results.InitPhase;                        %[rad] phase of LO at t=0
           self.InitFreq      = p.Results.InitFreq;                         %[Hz] baseband equivalent frequence of LO at t=0
           self.t_freq_change = p.Results.t_freq_change;                    %[sec] time at which the frequency is changed
           self.DeltaFreq     = p.Results.DeltaFreq;                        %[Hz] Amount the frequency is changed, see above
           self.fc            = p.Results.fc;                               %[Hz] passband centre-frequency equal to 0 [Hz] in baseband-equivalent frequency.
           self.PNS           = p.Results.PNS;           
        end
        
        function self = delay(self, tau)                                    % Delays all the frequencie changes,e..g to mimic time-offsets 
            self.t_freq_change = self.t_freq_change + tau;
            self.InitPhase     = self.InitPhase - 2*pi*rem((self.InitFreq+self.fc)*tau,1);
        end
        
        function self = ScaleFreq(self, scaleFactor) %Scales the baseband frequencies, e.g. to include crystal-offset 
            CFO            = scaleFactor * self.fc - self.fc;
            self.DeltaFreq = scaleFactor * self.DeltaFreq;
            self.InitFreq  = scaleFactor * self.InitFreq + CFO;
        end
        
        function F_tones = tones(self, varargin)                            % Outputs the absolute/passband frequencies defined
            p = inputParser();
            p.addOptional('t', [], @(x) validateattributes(x, {'numeric'}, {'vector', 'real'}, mfilename, 't', 1)); 
            p.addParameter('relative', false, @(x) validateattributes(x, {'logical'}, {'scalar'}, mfilename, 'relative'));
            p.parse(varargin{:});
            t = p.Results.t;
            relative = p.Results.relative;
            
            if isempty(t)       % Return all tones
                if relative
                    offset = 0;
                else
                    offset = self.fc;
                end
                F_tones = offset + self.InitFreq + cumsum([0, self.DeltaFreq]);
            else
                F_tones = self.freq(t, 'relative', relative);
            end
        end
        
        function f_tx = freq(self, varargin)                                % outputs the (relative) frequencies(f_tx) as function of time (t)
            p = inputParser();
            p.addRequired('t', @(x) validateattributes(x, {'numeric'}, {'vector', 'real'}, mfilename, 't', 1)); 
            p.addParameter('relative', true, @(x) validateattributes(x, {'logical'}, {'scalar'}, mfilename, 'relative'));
            p.parse(varargin{:});
            t = p.Results.t;
            
            f_tx = self.InitFreq*ones(size(t));
            for cnt_t = 1:length(self.t_freq_change)
                I = find( t>=self.t_freq_change(cnt_t), 1, 'first');
                f_tx(I:end) = f_tx(I:end) + self.DeltaFreq(cnt_t);
            end
            
            if ~p.Results.relative
                f_tx = f_tx + self.fc;
            end
        end
        
        function Phi_TX_th = phase(self,t)                                  % outputs the relative phase(f_tx) as function of time (t) 
            Phi_TX_th = self.InitPhase + 2*pi*self.InitFreq*t;
            for cnt_t = 1:length(self.t_freq_change)
                Phi_TX_th = Phi_TX_th + 2*pi*(self.DeltaFreq(cnt_t))*((t-self.t_freq_change(cnt_t)).*(t>self.t_freq_change(cnt_t)));
            end
        end

        function [Th_Sum_Delta_Phase]=Theory_Phi2W(B_LO,A_LO,ChannelDelay,t,T_offset,CHECK)
            %Two way measurement, starting with a TX of B(measured by A)
            %and then the other way around!
            if size(t,1)==2
                if nargin<6
                    CHECK=0;
                else
                    CHECK=CHECK+1;
                end
                [Th_Sum_Delta_Phase]=Theory_Phi2W(B_LO,A_LO,ChannelDelay,t(1,:),t(2,:)-t(1,:));%,CHECK);
            else
                if nargin<5
                    T_offset=0;
                end
%                 if nargin<6
%                     CHECK=0;
%                 else
%                     CHECK=CHECK+1;
%                 end
                
                Th_Sum_Delta_Phase=-2*pi*(A_LO.InitFreq+A_LO.fc+B_LO.InitFreq+B_LO.fc)*ChannelDelay*ones(size(t));
                Th_Sum_Delta_Phase=Th_Sum_Delta_Phase+2*pi*(A_LO.InitFreq-B_LO.InitFreq-B_LO.fc+A_LO.fc)*T_offset;
                for k=1:length(B_LO.t_freq_change)
                    b=A_LO.t_freq_change(k)+ChannelDelay;
                    B=A_LO.DeltaFreq(k);
                    a=B_LO.t_freq_change(k);
                    A=B_LO.DeltaFreq(k);
                    Th_Sum_Delta_Phase=Th_Sum_Delta_Phase+2*pi*DiffRamp(t,B,b,A,a);
                    
                    b2=B_LO.t_freq_change(k)+ChannelDelay;
                    B2=B_LO.DeltaFreq(k);
                    a2=A_LO.t_freq_change(k);
                    A2=A_LO.DeltaFreq(k);
                    Th_Sum_Delta_Phase=Th_Sum_Delta_Phase+2*pi*DiffRamp(t+T_offset,B2,b2,A2,a2);
                end

            end
        end
        function [Th_Phi_BA]=Theory_Phi1W(B_LO,A_LO,ChannelDelay,t)
            %B_LO is the transmitter LO
            %A_LO is the local/RX LO
            %
            
            Th_Phi_BA=(B_LO.InitPhase-2*pi*(B_LO.InitFreq+B_LO.fc)*ChannelDelay-A_LO.InitPhase)*ones(size(t));
            DeltaInitFreq=B_LO.InitFreq+B_LO.fc-A_LO.InitFreq-A_LO.fc;
            Th_Phi_BA=Th_Phi_BA+2*pi*DeltaInitFreq*t;
            for k=1:length(B_LO.t_freq_change)
                b=B_LO.t_freq_change(k)+ChannelDelay;
                B=B_LO.DeltaFreq(k);
                a=A_LO.t_freq_change(k);
                A=A_LO.DeltaFreq(k);
                Th_Phi_BA=Th_Phi_BA+2*pi*DiffRamp(t,B,b,A,a);
            end
        end

        function [IQ_B2A,H]=Theory_Phi1W_MP(B_LO,A_LO,t,tau,h)
            %This function computes analytically
            Phi_1WAY_SP=Theory_Phi1W(B_LO,A_LO,tau(1),t);
            H=zeros(size(Phi_1WAY_SP));
            [F_tones]=tones(B_LO,t);
            for cnt_tau=1:length(tau)
                Delay=tau(cnt_tau)-tau(1);
                Amp=h(cnt_tau);
                H=H+Amp*exp(-2i*pi*Delay*F_tones);
            end
            IQ_B2A=H.*exp(1i*Phi_1WAY_SP);
        end
        function [IQ_B2A,H]=Theory_Phi1W_MP_TV(B_LO,A_LO,t,tau,h,v)
            %This function computes analytically
            Phi_1WAY_SP=Theory_Phi1W(B_LO,A_LO,0,t);
            H=zeros(size(Phi_1WAY_SP));
            [F_tones]=tones(B_LO);
            c0=3e8;
            for cnt_tones=1:length(F_tones)
                H(cnt_tones)=0;
                for cnt_tau=1:length(tau)
                    Delay=tau(cnt_tau)+v(cnt_tau)*t(cnt_tones)/c0;
                    Amp=h(cnt_tau);
                    H(cnt_tones)=H(cnt_tones)+Amp*exp(-2i*pi*Delay*F_tones(cnt_tones));
                end
            end
            IQ_B2A=H.*exp(1i*Phi_1WAY_SP);
        end
        
        function [IQ_BA]=Theory_Phi1W_H(B_LO,A_LO,t,H)
            Phi_1WAY_SP=Theory_Phi1W(B_LO,A_LO,0,t);
            IQ_BA=H.*exp(1i*Phi_1WAY_SP);
        end
        
        function [IQ,H1,H2]=Theory_Phi2W_MP(B_LO,A_LO,t,tau,h)
            Phi_2WAY_SP=Theory_Phi2W(B_LO,A_LO,tau(1),t);

            H1=zeros(size(Phi_2WAY_SP));
            H2=zeros(size(Phi_2WAY_SP));
            for cnt_tau=1:length(tau)
                 Delay=tau(cnt_tau)-tau(1);
                 Amp=h(cnt_tau);
                 H1=H1+Amp*exp(-2i*pi*(B_LO.InitFreq+B_LO.fc+cumsum([0 B_LO.DeltaFreq]))*Delay);
                 H2=H2+Amp*exp(-2i*pi*(A_LO.InitFreq+A_LO.fc+cumsum([0 A_LO.DeltaFreq]))*Delay);
            end
            IQ=H1.*H2.*exp(1i*Phi_2WAY_SP);
        end
    end
end

