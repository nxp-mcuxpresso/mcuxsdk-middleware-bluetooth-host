%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%     This function computes the baseband-equivalent tapped delay line 
%     (TDL) model from the channel impulse response.
% Description end

function [H,Zs,Rs]=H_freq(tau,h,f)
% The channel impulse response is characterized by two vectors, namely tau,
% Loss_dB. Both vectors should have the same length. The n-th radio path introduces
% a delay tau(n) and Loss give by Loss_dB(n) , where the loss is given in
% power attenuation. fc represents the carrier frequency and DeltaT is the sampling period (of
% the ADC)

if(nargin==0)
    error('Not allowed')
else
    PLOT = false;
end

%% I am lazy, if N>1 RXs are defined, compute the CIR N times, in a recursive
% manner.
if iscell(tau)
    H=[];
    for BSID=1:length(tau)
        [H_tmp]=H_freq(tau{BSID}, h{BSID}, f);
        H=[H; H_tmp];
    end
    return;
end



%f=linspace(-BW/2+fc,BW/2+fc,NFFT);


% Loss_dB=[0 -100];%TestVectors
% tau=10e-9+[0e-9 10e-9]; %TestVectors
H=sum((ones(length(f),1)*h).*exp(-2i*pi*f'*tau),2).';
if nargout>1
    Zs=exp(-2i*pi*(f(2)-f(1))*tau);
    Rs=ones(length(f),1)*(h.*exp(-2i*pi*f(1)'*tau));
    
    Z_all=(ones(length(f),1)*Zs).^((0:length(f)-1).'*ones(1,length(tau)));
    H2=sum(Rs.*Z_all,2).';
    if(norm(H-H2)/norm(H)>1e-4)
        error('damned')
    end
end
if(PLOT)
    color='rmc';
    
    figure
    plot(f,20*log10(abs(H)))
    hold on
    plot(f,20*log10(abs(window)))
    plot(f,20*log10(abs(H2)),'r--')
    
    %     figure
    %     %    subplot(2,1,1)
    %     plot(tau/1e-9,Loss_dB,['o' color(1)]);
    %     xlabel('delay [ns]')
    %     hold on
    %     ylabel('Path loss [dB]')
    %     grid on
    %     ylim([-100 -40])
    
    figure
    plot(tau/1e-9,Loss_dB,['o' color(1)]);
    hold on
    xlabel('delay [ns]')
    hold on
    ylabel('Amplitude Att. [V]')
    grid on
    plot(t/1e-9,20*log10(abs(h)))
    YL = ylim;
    ylim([YL(2)-100 YL(2)])
    
    
    figure(1)
    F=f-fc;%-500e6:0.1e6:500e6;
    Fs=1e9;
    [H_dig,~]=FREQZ(h,1,F,Fs);
    plot(F+fc,20*log10(abs(H_dig)),'g:')
end


%