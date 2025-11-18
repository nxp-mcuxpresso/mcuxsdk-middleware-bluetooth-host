%--------------------------------------------------------------------------
% Project   : NRB ranging
% Version   : GIT
% Author    : Pepijn Boer (pepijn.boer@imec.nl)
% Copyright : Stichting Imec Nederland (http://www.imec-nl.nl)
% Disclaimer: Imec strictly confidential
%--------------------------------------------------------------------------
% Description
%   This function generates IQ-data given a channel model. The 
%   actual-distance and RicianK factor are inputs. There is IQ data
%   generates with and without phase-coherency between
%   tones.
% Description end
%--------------------------------------------------------------------------

function Out = generate_channel(NrTests, ActualDist, RicianK, varargin)
p = inputParser;
% p.KeepUnmatched = true;
p.addOptional('delta_F', 1e6, @(x) isnumeric(x));
p.addOptional('Kf', 80, @(x) isnumeric(x));
p.addOptional('f_start', 2.4e9, @(x) isnumeric(x));
p.addOptional('SignalPowerRange', 20*log10(mean(abs(10*rand(1,1)+2))), @(x) validateattributes(x, {'numeric'}, {'increasing', 'vector'}, mfilename, 'SignalPowerRange'));
p.addOptional('SNR', 42, @(x) isnumeric(x));
p.parse(varargin{:});

Kf         = p.Results.Kf;
delta_F    = p.Results.delta_F;
freq_meas  = p.Results.f_start+(0:Kf-1)*delta_F;
SignalPower = p.Results.SignalPowerRange;
if isscalar(SignalPower)
    SignalPower = SignalPower * [1, 1];
end
SNR        = p.Results.SNR;

% Channel model
ChanModel           = RV_chanmodel();
c = physconst('lightspeed');
ChanModel.Delay_LOS = ActualDist/c*1e9;
ChanModel.K         = RicianK;

%pre-allocate arrays
H_is   = zeros(NrTests, Kf);
IQ_Anc = zeros(size(H_is));                                                 % Non-phase-coherent data as measured by device A
IQ_Bnc = zeros(size(H_is));                                                 % Non-phase-coherent data as measured by device B
IQ_Ac  = zeros(size(H_is));                                                 % Phase-coherent data as measured by device A
IQ_Bc  = zeros(size(H_is));                                                 % Phase-coherent data as measured by device B

for k=1:NrTests
    ChanData  = ChanModel.do_n(1);                               % Generate a CIR.
    SigPow    = (SignalPower(2) - SignalPower(1)) * rand(1,1) + SignalPower(1); % Generate random signal power 
    X         = 10^(SigPow/20);                                                       % Convert to linear domain
    H_is(k,:) = X.*H_freq(ChanData.tau,ChanData.h,freq_meas);
    
    % Random phase rotatation from tone to tone
    RandPhase      = exp(1i*2*pi*rand(size(freq_meas)));
    AmplDistortion = (1+0.1*(0:Kf-1)/Kf);
    NoisePow       = 10^((SigPow - SNR)/20);
    Noise_A        = NoisePow/sqrt(2) * [1 1i]*randn(2, Kf);
    Noise_B        = NoisePow/sqrt(2) * [1 1i]*randn(2, Kf);
    
    temp_A = H_is(k,:).*RandPhase;
    temp_B = H_is(k,:).*conj(RandPhase);
    temp_A = temp_A.*AmplDistortion;
    %Add a bit of noise
    IQ_Anc(k,:) = temp_A + Noise_A;
    IQ_Bnc(k,:) = temp_B + Noise_B;
    
    % Correlated phase rotation from tone to tone
    T_offset  = rand(1,1)/delta_F;
    RandPhase = exp(2i*pi*freq_meas*T_offset);
    temp_A    = H_is(k,:).*RandPhase;
    temp_B    = H_is(k,:).*conj(RandPhase);
    
    temp_A    = temp_A.*AmplDistortion;
    %Add a bit of noise
    IQ_Ac(k,:) = temp_A + Noise_A;
    IQ_Bc(k,:) = temp_B + Noise_B;    
end

% The input-data of the RangeEstimator must be normalized to the
% ADC-fullscale. Therefore the data must be between +-1.
maxIQ = max([real(IQ_Anc(:)); imag(IQ_Anc(:)); real(IQ_Bnc(:)); imag(IQ_Bnc(:))]);
IQ_Anc = IQ_Anc / maxIQ * 0.25;        % normalize data to 0.25FS
IQ_Bnc = IQ_Bnc / maxIQ * 0.25;        % normalize data to 0.25FS

% The input-data of the RangeEstimator must be normalized to the
% ADC-fullscale. Therefore the data must be between +-1.
maxIQ = max([real(IQ_Ac(:)); imag(IQ_Ac(:)); real(IQ_Bc(:)); imag(IQ_Bc(:))]);
IQ_Ac = IQ_Ac / maxIQ * 1;        % normalize data to 1FS
IQ_Bc = IQ_Bc / maxIQ * 1;        % normalize data to 1FS

Out.Hactual = H_is;
Out.IQ_A_nc = IQ_Anc;                                                       % Non-phase-coherent data as measured by device A
Out.IQ_B_nc = IQ_Bnc;                                                       % Non-phase-coherent data as measured by device B
Out.IQ_A_c  = IQ_Ac;                                                        % Phase-coherent data as measured by device A
Out.IQ_B_c  = IQ_Bc;                                                        % Phase-coherent data as measured by device B
end