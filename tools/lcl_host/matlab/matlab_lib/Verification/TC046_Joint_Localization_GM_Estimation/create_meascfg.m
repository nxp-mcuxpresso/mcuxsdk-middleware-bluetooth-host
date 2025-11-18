%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   creates table with side-info about the measurement
% Description end

function MeasCfg = create_meascfg()
IsInit             = [true, false, false, false, false].';
ID                 = [8, 1, 2, 3, 4].';
XtalOffset         = [0, 0, 0, 0, 0].';                                     % [ppm] with respect to initiator due to xtal-offset and time-difference in taking measurements
MeasTimeDiff       = 416e-6;
XtalDistanceOffset = physconst('LightSpeed')/2 * XtalOffset * 1e-6 * MeasTimeDiff;
h                  = 1.37;                                                  % [m]
Position = [NaN, NaN  , h; ...
            0.5, 11.26, h; ...
            6.5, 11.26, h; ...
            6.5,  0.5 , h; ...
            0.5,  0.5 , h];
        
CFOScaling         = zeros(size(ID));                                       % (Hz/value) NXP: Frequency step size in Hz of the transceiver for setting the center frequency

RF = {'?', '?', '?', '?', '?';
      '?', '?', '?', '?', '?'}.';                                           % Antenna polarization on the RF-ports

IQscaling          = ones(length(ID),1);                                    % Scaling factor of the IQ-data
InterDelay         = repmat([0,0]*1e-6, [length(ID), 1]);                   % (s) Inter delay; time between two consecutive samples on the initiator/reflector assuming one antenna pair
IntraDelay         = repmat([0,0]*1e-6, [length(ID), 1]);                    % (s) Intra delay; time between two consecutive samples on the initiator/reflector assuming one antenna pair
IntraAntennaDelay  = repmat(0*1e-6, [length(ID), 1]);                          % (s) Intra antenna delay; time between two samples from antenna pair n and n+1 on the initiator/reflector

MeasCfg = table(IsInit, ID, Position, XtalOffset, RF, CFOScaling, IQscaling, InterDelay, IntraDelay, IntraAntennaDelay);
MeasCfg.Properties.VariableUnits = {'', '', 'm', 'ppm', '', 'Hz', '', 's', 's', 's'};
