%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
function [Output_SignalBB]=ImecChanfilter(tau,h,Input_SignalBB)
% Description
%   generalization of the default filter function, to allow for non-causal
%   filters. The non-causality is typically the result of frequency domain
%   windowing, i.e. artificial to reduce filter length, i.e. delay domain leakage
%   tau is the delay introduced by the channel.
%   The Input signal to be filtered should be of type Signal_BB 
%   see also Signal_BB
% Description end
deltaT = tau(2)-tau(1);
if norm(Input_SignalBB.deltaT-deltaT)>eps
    error('Filter and Input signal hould have the same sampling rate')
end
if tau(1)<0
    NofSamplesPreZero      = round(-tau(1)/deltaT);
    Output_SignalBB        = filter(h,1,Input_SignalBB,[]);
    TMP                    = Output_SignalBB.signal;
    Output_SignalBB.signal = [TMP(NofSamplesPreZero+1:end);zeros(NofSamplesPreZero,1)];
else
    NofSamplesPreZero      = round(tau(1)/deltaT);
    Output_SignalBB        = filter(h,1,Input_SignalBB,[]);
    TMP                    = Output_SignalBB.signal;
    Output_SignalBB.signal = [zeros(NofSamplesPreZero,1);TMP(1:end-NofSamplesPreZero)];
end

