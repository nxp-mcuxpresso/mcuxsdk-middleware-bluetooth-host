%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This file provides different parameters required to simulate human
%   motion using the headed social force model.
% Description end

function setup = HSFM_setup(varargin)
ParsedInputs= inputParser;
ParsedInputs.KeepUnmatched = true;
addOptional(ParsedInputs, 'Ts'               , 0.1             , @(x) isnumeric(x));
addOptional(ParsedInputs, 'Na'               , 1             , @(x) isa(x, 'number of agents'));
addOptional(ParsedInputs, 'Scenario'               , 'open_space'             , @isstring);
% agents repulsive force constants
addOptional(ParsedInputs, 'A_i'               , 2000             , @(x) isnumeric(x));
addOptional(ParsedInputs, 'B_i'               , 0.08             , @(x) isnumeric(x));
% walls repulsive force constants
addOptional(ParsedInputs, 'A_w'               , 2000             , @(x) isnumeric(x));
addOptional(ParsedInputs, 'B_w'               , 0.08             , @(x) isnumeric(x));
% walls compression pushing force coefficient
addOptional(ParsedInputs, 'k1'               , 10             , @(x) isnumeric(x));
% walls friction force coefficient
addOptional(ParsedInputs, 'k2'               , 10             , @(x) isnumeric(x));
% scale for projection of interaction force along the orthogonal direction
addOptional(ParsedInputs, 'ko'               , 1             , @(x) isnumeric(x));
% orthogonal velocity damping factor
addOptional(ParsedInputs, 'kd'               , 500             , @(x) isnumeric(x));
addOptional(ParsedInputs, 'klambda'               , 0.2             , @(x) isnumeric(x));
addOptional(ParsedInputs, 'alpha'               , 0.5             , @(x) isnumeric(x));
% Modulus of the desired velocity
addOptional(ParsedInputs, 'vd'               , 2             , @(x) isnumeric(x));
% standard deviation of the desired velocity
addOptional(ParsedInputs, 'sigma_vd'               , .1             , @(x) isnumeric(x));
% maximum forward velocity of an agent
addOptional(ParsedInputs, 'v_max'               , 3             , @(x) isnumeric(x));

% maximum angular velocity of an agent
addOptional(ParsedInputs, 'omega_max'               , pi/2             , @(x) isnumeric(x));  % rad/s

% agents radius
addOptional(ParsedInputs, 'rMin'               , 0.25             , @(x) isnumeric(x));         % m
addOptional(ParsedInputs, 'rMax'               , 0.35             , @(x) isnumeric(x));         % m
% agents Mass
addOptional(ParsedInputs, 'mMin'               , 60             , @(x) isnumeric(x));           % kg
addOptional(ParsedInputs, 'mMax'               , 90             , @(x) isnumeric(x));           % kg

% wall and obstacles points expected radius
addOptional(ParsedInputs, 'r_obst'               , 0.1             , @(x) isnumeric(x));

% Reaction time
addOptional(ParsedInputs, 'tau'               , 1             , @(x) isnumeric(x));

% Number of agents
addOptional(ParsedInputs, 'N_Agent'               , 1             , @(x) isnumeric(x));
% the final maximum distance
addOptional(ParsedInputs, 'maxdis2dest'               , 0.1             , @(x) isnumeric(x));
% maximum rotation rate
addOptional(ParsedInputs, 'MaxAbsOmega'               , pi             , @(x) isnumeric(x));

ParsedInputs.parse(varargin{:})
setup = ParsedInputs.Results;
end

