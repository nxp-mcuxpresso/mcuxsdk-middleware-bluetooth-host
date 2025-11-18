%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
function [T]=ImpactPhaseNoise(sys)
% Description
%   From the information contained in A+B, which describing both
%   transceivers, a matrix T is computated that that allows to simulate
%   correlated phase noise as present in the IQ samples in NRB ranging.
%   see also semianalytic_MCPD
% Description end

if abs(sys.A.ABB.Filter.CuttOff_Hz-sys.B.ABB.Filter.CuttOff_Hz)>1
    error('This model is only tested/valid if the filter at A and B have the same CuttOff_Hz')
else
    ABB=sys.B.ABB;
end
BW     = 4*ABB.Filter.CuttOff_Hz;% Depends a bit on the order of the ABB-filter, 4 times should be well enough
deltaF = 5e2; %Set frequency resolution for numerical integration of PSD of the phase noise.

f=0:deltaF:BW;
H_A=freqz(sys.A.LO.PNS.filtObj, f);
H_B=freqz(sys.B.LO.PNS.filtObj, f);
TotalPN_lin = H_A + H_B; %10.^(H_dBcA/10)+10.^(H_dBcB/10); %Addition in the power domain, as both PN-sources are independent;

H_ABB_lin   = freqz(ABB,f);
PWR_ABB_lin = abs(H_ABB_lin).^2/abs(H_ABB_lin(1))^2;%Need to normalize as phase noise scales with the power of the signal.
PSD_lin     = TotalPN_lin.'.*PWR_ABB_lin;

% figure;
% semilogx(f,10*log10(TotalPN_lin),'g');
% hold on
% semilogx(f,10*log10(abs(H_ABB_lin)),'r');
% semilogx(f,10*log10(abs(PSD_lin)),'m');

%In total we have length(A.PhaseMeasUnit.t_sample)+length(B.PhaseMeasUnit.t_sample) phase measurements
%The phase-noise for measurements taken at transceiver B have an opposite sign compared
%to measurements taken at transceiver A. This has been realized using
%c1,c2.
%Let us build the Covariance matrix, first let us collect the
%time-differences
tau=[sys.A.PhaseMeasUnit.t_sample sys.B.PhaseMeasUnit.t_sample]; %The first 'NofFreq' are the ones taken at A and the latter 'NofFreq' are the ones takes at B 
taus=tau'*ones(1,length(tau));
tau_diff=taus-taus';%The correlation does not depend on time, just on time-difference
ACR=zeros(size(tau_diff));
for cnt_dim1=1:size(tau_diff,1)
    c1=1-2*(cnt_dim1>length(sys.A.PhaseMeasUnit.t_sample)); %c1 e
    for cnt_dim2=cnt_dim1:size(tau_diff,2)
        c2=1-2*(cnt_dim2>length(sys.A.PhaseMeasUnit.t_sample));
        ACR(cnt_dim1,cnt_dim2)=2*c1*c2*sum(cos(f*tau_diff(cnt_dim1,cnt_dim2)).*PSD_lin)*deltaF;%Using the PDS, the expected correlation can be used. The factor of two 
        if cnt_dim2>cnt_dim1
            ACR(cnt_dim2,cnt_dim1)=ACR(cnt_dim1,cnt_dim2);
        end
    end
end
[U,Eigenvalues]=eig(ACR);
T=U*sqrt(Eigenvalues);

