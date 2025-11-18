%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
function [T]=ImpactThermalNoise(sys)
% Description
%   From the information contained in A+B, which describing both
%   transceivers, a matrix T is computated that that allows to simulate
%   correlated thermal noise present in the IQ samples in NRB ranging.
% Description end

if abs(sys.A.ABB.Filter.CuttOff_Hz-sys.B.ABB.Filter.CuttOff_Hz)>1
    error('This model is only tested/valid if the filter at A and B have the same CuttOff_Hz')
else
    ABB=sys.B.ABB;
    LNA=sys.A.LNA;
end
BW=4*ABB.Filter.CuttOff_Hz;% Depends a bit on the order of the ABB-filter, 4 times was found to be well enough
deltaF=5e2; %Set frequency resolution for numerical integration of PSD of the phase noise.
f=0:deltaF:BW;
N_dBmHz=LNA.powerValue; % (dBm/Hz)
H_ABB_lin=freqz(ABB,f);
PWR_ABB_lin=abs(H_ABB_lin).^2;
Noise_power_Hz = 10^((N_dBmHz-30)/10)*50;

tau=[sys.A.PhaseMeasUnit.t_sample sys.B.PhaseMeasUnit.t_sample]; %The first 'NofFreq' are the ones taken at A and the latter 'NofFreq' are the ones takes at B
taus=tau'*ones(1,length(tau));
tau_diff=taus-taus';%The correlation does not depend on time, just on time-difference
ACR=zeros(size(tau_diff));
for cnt_dim1=1:size(tau_diff,1)
    c1=1-1*(cnt_dim1>length(sys.A.PhaseMeasUnit.t_sample)); %c1 e
    for cnt_dim2=cnt_dim1:size(tau_diff,2)
        c2=1-1*(cnt_dim2>length(sys.A.PhaseMeasUnit.t_sample));
        %ACR(cnt_dim1,cnt_dim2)=2*(c1==c2)*trapz(f,cos(f*tau_diff(cnt_dim1,cnt_dim2)).*PWR_ABB_lin);%Using the PDS, the expected correlation can be used. The factor of two
        ACR(cnt_dim1,cnt_dim2)=2*(c1==c2)*sum(cos(f*tau_diff(cnt_dim1,cnt_dim2)).*PWR_ABB_lin)*deltaF;
        if cnt_dim2>cnt_dim1
            ACR(cnt_dim2,cnt_dim1)=ACR(cnt_dim1,cnt_dim2);
        end
    end
end
ACR=Noise_power_Hz*ACR;
[U,Eigenvalues]=eig(ACR);
T=U*sqrt(Eigenvalues);




