%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This file configures the LO and PA for the 2WR profile.
% Description end
delta_T         = p.Results.deltaT;
TX_duration     = p.Results.TX_duration;
A_TX_offset     = p.Results.A_TX_offset;

WN_powervalue   = -174+6;
PNS_powerValue  = -Inf;

%Generate TRX A+B without PPM
sim.A.LNA = signal.AdditiveNoise('powerUnit', 'dBm/Hz', 'powerValue', WN_powervalue, 'fs', 1/delta_T); % +6 dB Noise figure
sim.A.PA  = NRBLOC_PA('t_activateStart', (0:NofFreq-1)*T_f+A_TX_offset, ...
                      't_duration', TX_duration);

sim.A.LO = NRBLOC_LO('InitPhase', 2*pi*(rand(1,1)-0.5), ...
                     't_freq_change', (1:NofFreq-1)*T_f, ...
                     'DeltaFreq', ones(1,NofFreq-1)*delta_F, ...
                     'InitFreq', -sum(ones(1,NofFreq-1)*delta_F)/2, ...
                     'PNS', signal.PhaseNoise('powerValue', PNS_powerValue, 'fs', 1/delta_T, 'powerRefFreq', 1.5e-5*1/delta_T));

sim.B.LNA = signal.AdditiveNoise('powerUnit', 'dBm/Hz', 'powerValue', WN_powervalue, 'fs', 1/p.Results.deltaT); % +6 dB Noise figure
sim.B.PA = NRBLOC_PA('t_activateStart', (sim.A.PA.t_activateStart+T_f/2), ...
                     't_duration', TX_duration);

sim.B.LO = NRBLOC_LO('InitPhase', 2*pi*(rand(1,1)-0.5), ...
                     't_freq_change', (1:NofFreq-1)*T_f, ...
                     'DeltaFreq', ones(1,NofFreq-1)*delta_F, ...
                     'InitFreq', -sum(ones(1,NofFreq-1)*delta_F)/2, ...
                     'PNS', signal.PhaseNoise('powerValue', PNS_powerValue, 'fs', 1/delta_T, 'powerRefFreq', 1.5e-5*1/delta_T));

sim.A.PhaseMeasUnit.t_sample=sim.B.PA.t_activateStart+TX_duration-10e-6;
sim.B.PhaseMeasUnit.t_sample=sim.A.PA.t_activateStart+TX_duration-10e-6;
