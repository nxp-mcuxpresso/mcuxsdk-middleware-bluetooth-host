%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
function [tau,CIR]=H_to_CIR_DT(freq_meas,H,f_c,DeltaT)
% Description
%   Transfers frequency domain measurements (freq_meas + H) to a delay domain channel impulse
%   response(CIR). The desired sampling rate(oversampling is assumed!) and
%   Baseband euivalent center frequency can be set
%   Note that some of the setting used in this file are specific for the NRB ranging 
%   project,like e.g. noise floor in the delay domain.
% Description end

% If H is a matrix, we process them one by one. 
for cnt=1:size(H,1)
    [tau,CIR(cnt,:)]=S2h(freq_meas.',H(cnt,:).',f_c,DeltaT);
end




function [t,CIR,Param]=S2h(freq,S,fc,deltaT)
%CIR denotes Channel Impulse Response.
BW_wanted=1/deltaT; %Here BW is directly derived from deltaT(1/sample-rate) 
if(fc+BW_wanted/2>freq(end))
    error('request for data outside the measured frequency range, decrease fc and/or increase deltaT')
elseif(fc-BW_wanted/2<freq(1))
    error('request for data outside the measured frequency range, increase fc and/or decrease deltaT')
else
    % setting OK
end


I=find((freq>fc-BW_wanted/2)&(freq<fc+BW_wanted/2));%select frequency range if interest.
w=signal.tukeywin(length(I),0.5);   %selected a tukey-window
fL=freq(I(1));                      %lowerst frequency

CIR  = ifft((w.*S(I)));             %convert to delay domain
t    = (0:length(CIR)-1)*deltaT;    %compute the corresponding delays
CIR  = CIR.*exp(2i*pi*(fL-fc)*t');  %mix to the desired centre-frequency

%reorder the CIR to put the significant part at the center of the vector.  
CIR  = CIR([ceil(length(CIR)/2):end 1: ceil(length(CIR)/2)-1]);
%due to aliasing, the very large delays are in fact negative delays, which
%potentially have contributions due to windowing.
t([ceil(length(CIR)/2):end])=t([ceil(length(CIR)/2):end])-length(CIR)*deltaT;
t=t([ceil(length(CIR)/2):end 1: ceil(length(CIR)/2)-1]);

%The CIR is trimmed to allow for faster simulation and cancels part of the
%measurement noise.
[~,I_max]=max(abs(CIR));
I = find(20*log10(abs(CIR))<-115);   % find indices of noise dominated delay bins.
if ~isempty(I)
    I_plus=find(I>I_max,1,'first');         % the first index beyond the max with an amplitude below -115
    I_minus=find(I<I_max,1,'last');         % the last index before the max with an amplitude below -115
    I_minus=I(I_minus)-100;                 % go well before, just to be same
    I_plus=I(I_plus)+100;                   % go well before, just to be same

    %The CIR can now be trimmed.
    t=t(I_minus:I_plus);
    CIR=CIR(I_minus:I_plus);
else
    % nothing to trimmm
end

