%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
function [IQ_B2A_th,IQ_A2B_th,sys]=semianalytic_MCPD(sys,tau,h,NofSim)
    % Description
    % This is a semi-analytical model for MCPD with amplitude info, taking into
    % account. It is called semi-analytical, because partially it is 
    % deterministic(taking into account carrier frequency offset,
    % sample timing drift, channel response, etc) 
    % and partially simulation-driven (phase noise and thermal noise).
    % Description end
if isstruct(tau)
    chanData = tau;
    tau      = chanData.tau;
    h        = chanData.h;
    v        = chanData.v;
end
    
if nargin<4
    NofSim=1;
end
if ~exist('chanData','var')
    [IQ_B2A_th]=10.^((sys.B.PA.Power_dBm+sys.A.ABB.NonLin.Gain_dB-30+10*log10(50))/20)*Theory_Phi1W_MP(sys.B.LO,sys.A.LO,sys.A.PhaseMeasUnit.t_sample,tau,h);
    [IQ_A2B_th]=10.^((sys.A.PA.Power_dBm+sys.B.ABB.NonLin.Gain_dB-30+10*log10(50))/20)*Theory_Phi1W_MP(sys.A.LO,sys.B.LO,sys.B.PhaseMeasUnit.t_sample,tau,h);
else
    %[IQ_B2A_th_static]=10.^((sys.B.PA.Power_dBm+sys.A.ABB.NonLin.Gain_dB-30+10*log10(50))/20)*Theory_Phi1W_MP(sys.B.LO,sys.A.LO,sys.A.PhaseMeasUnit.t_sample,tau,h);
    %[IQ_A2B_th_static]=10.^((sys.A.PA.Power_dBm+sys.B.ABB.NonLin.Gain_dB-30+10*log10(50))/20)*Theory_Phi1W_MP(sys.A.LO,sys.B.LO,sys.B.PhaseMeasUnit.t_sample,tau,h);
    [IQ_B2A_th]=10.^((sys.B.PA.Power_dBm+sys.A.ABB.NonLin.Gain_dB-30+10*log10(50))/20)*Theory_Phi1W_MP_TV(sys.B.LO,sys.A.LO,sys.A.PhaseMeasUnit.t_sample,tau,h,v);
    [IQ_A2B_th]=10.^((sys.A.PA.Power_dBm+sys.B.ABB.NonLin.Gain_dB-30+10*log10(50))/20)*Theory_Phi1W_MP_TV(sys.A.LO,sys.B.LO,sys.B.PhaseMeasUnit.t_sample,tau,h,v);
end

if ~isfield(sys,'T_PN')
    %We need to compute the statistical properties of the phase noise.
    sys.T_PN = ImpactPhaseNoise(sys);
    %and we can re-use it only the channel changes. 
end
if ~isfield(sys,'T_TN')
    %We need to compute the statistical properties of the thermal noise.
    sys.T_TN = ImpactThermalNoise(sys);
    %and we can re-use it only the channel changes. 
end

PhaseNoise   = (sys.T_PN*randn(size(sys.T_PN,2),NofSim)).';
ThermalNoise = (sys.T_TN*(randn(size(sys.T_TN,2),NofSim)+1i*randn(size(sys.T_TN,2),NofSim))).'*sqrt(2) ;

PhaseNoise   = zeros(size(PhaseNoise));
ThermalNoise = zeros(size(ThermalNoise));

N_a = length(sys.A.PhaseMeasUnit.t_sample);

IQ_B2A_th = (ones(NofSim,1)*IQ_B2A_th).*exp(1i*PhaseNoise(:,1:N_a));
IQ_A2B_th = (ones(NofSim,1)*IQ_A2B_th).*exp(1i*PhaseNoise(:,N_a+1:end));

IQ_B2A_th = IQ_B2A_th + ThermalNoise(:,1:N_a);
IQ_A2B_th = IQ_A2B_th + ThermalNoise(:,N_a+1:end);
