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

function [Out_2D, Out_1D] = generate_channel_2D(NrTests, ActualDist, RicianK, nAnt, varargin)
p = inputParser;
% p.KeepUnmatched = true;
p.addOptional('delta_F', 1e6, @(x) isnumeric(x));
p.addOptional('Kf', 80, @(x) isnumeric(x));
p.addOptional('f_start', 2.4e9, @(x) isnumeric(x));
p.parse(varargin{:});

Kf         = p.Results.Kf;
delta_F    = p.Results.delta_F;
freq_meas  = p.Results.f_start+(0:Kf-1)*delta_F;

% Channel model
ChanModel           = RV_chanmodel();
ChanModel.Delay_LOS = ActualDist/3e8*1e9;
ChanModel.K         = RicianK;

%pre-allocate arrays
H_is      = zeros(nAnt, Kf);
H_is_1D   = zeros(NrTests, Kf);
IQ_Anc_1D   = zeros(size(H_is_1D));                                         % Non-phase-coherent data as measured by device A
IQ_Bnc_1D   = zeros(size(H_is_1D));                                         % Non-phase-coherent data as measured by device B
IQ_Ac_1D = zeros(size(H_is_1D));                                            % Phase-coherent data as measured by device A
IQ_Bc_1D = zeros(size(H_is_1D));                                            % Phase-coherent data as measured by device B
Noise_A   = zeros(size(H_is));
Noise_B   = zeros(size(H_is));
Out_2D(NrTests) = struct();
for k=1:NrTests
    ChanData  = ChanModel.do_n(1);                               % Generate a CIR.
    X         = 10*rand(1,1)+2;                                  % Random magnitude scaling to test
    
    % generate channel for ULA of nAnt antennas
    Lz=0:3;
    dx=3e8/2.4395e9/2;
    f=freq_meas;
    H_is=zeros(nAnt,numel(f));
    for iant=1:nAnt
        temp = ChanData.tau-dx*Lz(iant)*sin(ChanData.AoA)/3e8;
        [H_is(iant,:)]=X.*H_freq(temp,ChanData.h,f);
    end
    
    % Random phase rotatation from tone to tone
    RandPhase      = repmat(exp(1i*2*pi*rand(size(freq_meas))), nAnt, 1);
    AmplDistortion = repmat(1+0.1*(0:Kf-1)/Kf, nAnt, 1);
    for iant = 1:nAnt
        Noise_A(iant,:) = X*0.02*[1 1i]*rand(2, Kf);
        Noise_B(iant,:) = X*0.02*[1 1i]*rand(2, Kf);
    end
    
    temp_A = H_is.*RandPhase;
    temp_B = H_is.*conj(RandPhase);
    temp_A = temp_A.*AmplDistortion;
    %Add a bit of noise
    IQ_Anc = temp_A + Noise_A;
    IQ_Bnc = temp_B + Noise_B;
    
    % The input-data of the RangeEstimator must be normalized to the
    % ADC-fullscale. Therefore the data must be between +-1.
    maxIQ = max([real(IQ_Anc(:)); imag(IQ_Anc(:)); real(IQ_Bnc(:)); imag(IQ_Bnc(:))]);
    IQ_Anc = IQ_Anc / maxIQ * 0.25;        % normalize data to 0.25FS
    IQ_Bnc = IQ_Bnc / maxIQ * 0.25;        % normalize data to 0.25FS
        
    % Correlated phase rotation from tone to tone
    T_offset  = rand(1,1)/delta_F;
    RandPhase = repmat(exp(2i*pi*freq_meas*T_offset), nAnt, 1);
    temp_A    = H_is.*RandPhase;
    temp_B    = H_is.*conj(RandPhase);
    
    temp_A    = temp_A.*AmplDistortion;
    %Add a bit of noise
    IQ_Ac = temp_A+Noise_A;
    IQ_Bc = temp_B+Noise_B;
    
    % The input-data of the RangeEstimator must be normalized to the
    % ADC-fullscale. Therefore the data must be between +-1.
    maxIQ = max([real(IQ_Ac(:)); imag(IQ_Ac(:)); real(IQ_Bc(:)); imag(IQ_Bc(:))]);
    IQ_Ac = IQ_Ac / maxIQ * 1;        % normalize data to 1FS
    IQ_Bc = IQ_Bc / maxIQ * 1;        % normalize data to 1FS
    
    Out_2D(k).Hactual = H_is;
    Out_2D(k).IQ_A_nc = IQ_Anc;
    Out_2D(k).IQ_B_nc = IQ_Bnc;
    Out_2D(k).IQ_A_c  = IQ_Ac;
    Out_2D(k).IQ_B_c  = IQ_Bc;
    
    H_is_1D(k,:) = H_is(1,:);
    IQ_Anc_1D(k,:) = IQ_Anc(1,:);
    IQ_Bnc_1D(k,:) = IQ_Bnc(1,:);
    IQ_Ac_1D(k,:) = IQ_Ac(1,:);
    IQ_Bc_1D(k,:) = IQ_Bc(1,:);
end

Out_1D.Hactual    = H_is_1D;
Out_1D.IQ_A_nc   = IQ_Anc_1D;
Out_1D.IQ_B_nc   = IQ_Bnc_1D;
Out_1D.IQ_A_c = IQ_Ac_1D;
Out_1D.IQ_B_c = IQ_Bc_1D;

end