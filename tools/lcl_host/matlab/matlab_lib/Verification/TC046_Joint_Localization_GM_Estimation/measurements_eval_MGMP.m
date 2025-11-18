%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   Employing 'on_the_fly_MGMP' obj to perform localization using the measurement data
% Description end

function measurements_eval_MGMP(varargin)
p = inputParser();
p.addParameter('use_opti_toolbox', true, @(x) validateattributes(x, {'logical'}, {'scalar'}, mfilename, 'use_opti_toolbox'));
p.parse(varargin{:});

close all
% clc
if ispc && contains(path, 'OPTI_toolbox')
    % If the opti-toolbox is installed, it should be in the path.
    % The opti-toolbox only works on Windows.
    % The opti-toolbox can be found at: https://github.com/jonathancurrie/OPTI
    opti_flag = p.Results.use_opti_toolbox;
else
    opti_flag = false;
    if p.Results.use_opti_toolbox
        warning('The OPTI-toolbox is not used, because either the opti-toolbox is not installed or you''re running this script on a non-Windows machine');
    end
end   
    
ProjectDataPath = set_paths_NRBLOC;

% Select the measurement from this list:
MeasName = '8_shape_HVpolarized_VNA.mat';
%MeasName = '8_shape_HVpolarized_FW153_no_interference.mat';

% load measurement matrices and configuration
MeasCfg = create_meascfg();

% Load the matrix from the current folder or ProjectDataPath folder
if isfile(MeasName)
    load(MeasName);   
else
    cpwd = pwd;
    mat_root = fullfile(ProjectDataPath,'measurement_data', 'TC046_Joint_Localization_GM_Estimation');
    cd(mat_root)
    load(MeasName);
    cd(cpwd)
end

% Create masks
MASK_RSSI_TOO_LOW_sum = (Tsum.initiatorRSSI<-70 | Tsum.reflectorRSSI<-70);
%% MGMP Initializing parameters
% Constrained or unconstrained optimization
Initial_S.opti_flag=opti_flag;

% Least square opt. for comp.
Initial_S.optcomp.type = 'LS';
% L1-norm opt. for comp.
% optcomp.type='L1';

% Measurement Bias on top of distribution
Initial_S.optcomp.bias = 0;

% Initial GM for on-the-fly
Initial_S.gm = gmdistr_ECM_imec([1], [1], [1]); %#ok<NBRAK>
%% Plot location
KEEP_sum = ~MASK_RSSI_TOO_LOW_sum;

[~, ~, MixGaussObj] = CRLBwFittedGM(Tsum.Distance(KEEP_sum) - Tsum.ActualDistance(KEEP_sum), false);
Initial_S.Err_model_is = gmdistr_imec((MixGaussObj.mu).', squeeze(MixGaussObj.Sigma).', MixGaussObj.ComponentProportion);

%KEEP_sum = KEEP_sum(1:100);
plot_location_measurements_ML_LS_MGMP(Tsum(KEEP_sum,:), MeasCfg, Initial_S, 'Localization.comp.Proc');
set(gcf, 'Position', [800, 194, 580, 575]);
end