%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
function [sim]=TRX_settings(varargin)
% Description
%   This is main settings-file for the MCPD simulator AND semi-analytical model.
%   All paramater which have an infuence on the air-interface usage are defined here.
%   Hence, the structure-sim is a full description.
% Description end
T_f=500e-6;
p = inputParser;
p.addParameter('delta_F', 1e6, @(x) validateattributes(x, {'numeric'}, {'scalar'}) );%[Hz]               %frequency step-size
p.addParameter('NofFreq', 8, @(x) validateattributes(x, {'numeric'}, {'scalar','>', 0}) );%[]
p.addParameter('T_f', 500e-6, @(x) validateattributes(x, {'numeric'}, {'scalar','>', 0}) );%[sec]                 %Duration of a single tone measurement in two directions.
p.addParameter('TX_duration', T_f*0.4, @(x) validateattributes(x, {'numeric'}, {'scalar','>', 0}) );%[sec]        %Defines which portion of T_f, each TX/PA is on (40% of the time each, 80% together)
p.addParameter('A_TX_offset', T_f*0.05, @(x) validateattributes(x, {'numeric'}, {'scalar','>', 0}) );%[sec] %Before a PA is activated (at the start of the T_f), it waits this amount.(Here 10 microsec)
p.addParameter('AB_offset_max', 10e-6, @(x) validateattributes(x, {'numeric'}, {'scalar'}) );%[sec];       %The (initial) time-offset between both transceivers A&B) is a random variable between [-10e-6 10e-6];
p.addParameter('ppm_max', 20, @(x) validateattributes(x, {'numeric'}, {'scalar'}) );
p.addParameter('profile', '2WR', @(x) validateattributes(x, {'char'},{'nonempty', 'scalartext'}) );
p.addParameter('deltaT', 2.5e-9, @(x) validateattributes(x, {'numeric'},{'positive', 'scalar', '<', 1}, mfilename) );   % (s) The sample-time of the signal
p.parse(varargin{:});

delta_F         = p.Results.delta_F;
NofFreq         = p.Results.NofFreq;
T_f             = p.Results.T_f;
AB_offset_max   = p.Results.AB_offset_max;
ppm_max         = p.Results.ppm_max;
profile         = p.Results.profile;

sim.SW.delta_F = delta_F;
sim.SW.NofFreq = NofFreq;
sim.SW.T_f     = T_f;

%Generate TRX A+B without PPM

profile_filename = ['TRX_settings_', upper(profile), '_profile.m'];
if exist(profile_filename, 'file')
    run(profile_filename);
else
    error('The file %s for profile %s does not exist.', profile_filename, profile);
end

sim.A.ABB=AnalogBB();
sim.B.ABB=AnalogBB();

%Intro PPM induced frequency-offset
PPM_A=(2*rand(1,1)-1)*ppm_max*1e-6; %generate a random PPM offset for A between [-ppm_max, ppm_max]
PPM_B=(2*rand(1,1)-1)*ppm_max*1e-6; %generate a random PPM offset for B between [-ppm_max, ppm_max]

%If CFO compensation on, define here the residual PPM error, we assume
%the residual CFO order is in the order of PPM_max/10. For now, we assume it is
%related to the original PPM-offset(w/o compensation).
PPM_A_tilde=PPM_A/10;
PPM_B_tilde=PPM_B/10;

sim.A.LO=ScaleFreq(sim.A.LO,(1+PPM_A_tilde));
sim.B.LO=ScaleFreq(sim.B.LO,(1+PPM_B_tilde));

%introduce PPM induced Timing-offset
sim.A.PA.t_activateStart=sim.A.PA.t_activateStart*(1+PPM_A); %Crystal offset will influence the time at which the PA is activated
sim.B.PA.t_activateStart=sim.B.PA.t_activateStart*(1+PPM_B);
sim.A.LO.t_freq_change=sim.A.LO.t_freq_change*(1+PPM_A);
sim.B.LO.t_freq_change=sim.B.LO.t_freq_change*(1+PPM_B);
sim.A.PhaseMeasUnit.t_sample=sim.A.PhaseMeasUnit.t_sample*(1+PPM_A); %Crystal offset will influence the time at which the IQ samples are taken
sim.B.PhaseMeasUnit.t_sample=sim.B.PhaseMeasUnit.t_sample*(1+PPM_B);

AB_offset=(2*rand(1,1)-1)*AB_offset_max;
sim.B.LO=delay(sim.B.LO,AB_offset); %All time-offset is added to device B (LO-part)
sim.B.PA=delay(sim.B.PA,AB_offset); %All time-offset is added to device B (PA-part)
sim.B.PhaseMeasUnit.t_sample=sim.B.PhaseMeasUnit.t_sample+AB_offset;
switch profile
    case '2WR'
        sim.T_sim=T_f*(NofFreq+1);
    case '1WR'
        sim.T_sim=2*T_f*(NofFreq+1);
end
sim.DeltaT = p.Results.deltaT;
sim.AB_offset=AB_offset;
sim.PPM_A=PPM_A;
sim.PPM_B=PPM_B;
sim.PPM_A_tilde=PPM_A_tilde;
sim.PPM_B_tilde=PPM_B_tilde;