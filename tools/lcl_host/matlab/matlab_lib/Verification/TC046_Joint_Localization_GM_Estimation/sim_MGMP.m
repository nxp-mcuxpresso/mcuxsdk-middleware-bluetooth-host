%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   Using MGMP algorithm for joint localization and Gaussian mixture estimation 
%   The performances of MGMP, ML, LS are also compared.
% Description end

function sim_MGMP(varargin)
p = inputParser();
p.addParameter('use_opti_toolbox', true, @(x) validateattributes(x, {'logical'}, {'scalar'}, mfilename, 'use_opti_toolbox'));
p.parse(varargin{:});

close all;

rng(530)
%rng('shuffle')
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

%% Initializing parameters
% Number of repetition of trajectory 
Tra_rep = 1; 

% 'square' or 'circle' trajectory
shape = 'square'; 

% Step size in square grid or circle
step_size = 0.5;  

% For circle trajectory only
radius = 4;     

% LS-type opt. for comp.
optcomp.type = 'LS';   
%optcomp.type='L1';                 

% Measurement Bias on top of distribution 
optcomp.bias = 0;                     

%Anchors position (2 dimensional scenario definition of the anchors)

%4 anchors in a square of 10x10 meters.
%Anchors=[0 0;0 1; 1 0 ;1 1]*10;    

%8 anchors in a square of 10x10 meters.
Anchors = [0 0; 0 0.5; 0 1; 0.5 1; 0.5 0; 1 0; 1 0.5; 1 1]*10;

%16 anchors in a square of 10x10 meters.
%Anchors=[0 0;0 0.25;0 0.5;0 0.75;0 1;0.25 1;0.25 0;0.5 1;0.5 0;0.75 1;0.75 0;1 0;1 0.25;1 0.5;1 0.75;1 1]*10;        

Anchors = Anchors-ones(size(Anchors,1),1)*mean(Anchors);                 

%Trajectory genertion
traj = trajectory_gen(shape, step_size,Anchors,radius);
x_is = traj.pos_xy;

% X-axis grid for init of opt.
X_grid = min(Anchors(:,1)):0.5:max(Anchors(:,1));  
% Y-axis grid for init of opt.
Y_grid = min(Anchors(:,2)):0.5:max(Anchors(:,2));  

% lower_bound for init of opt
lb = [min(Anchors(:,1)); min(Anchors(:,2))];   
% upper_bound for init of opt
ub = [max(Anchors(:,2)); max(Anchors(:,2))];  

%%% Initializing the GM for ECM
gm = gmdistr_ECM_imec([1 2], [0.5 0.5], [0.5 0.5]);
%Err_model_est{1}=gmdistr_ECM_imec([1.5],[0.5],[1]);

% Increasing the No. Measurement point (performance evaluation)
x_temp = x_is;
for i=1:Tra_rep
    x_is = [x_is; x_temp]; %#ok<*AGROW>
end

NofSim = size(x_is, 1);

% Define the true error distribution of the GM
Err_model_is = gmdistr_imec();
%Err_model_is = gmdistr_imec([1 2], [0.5 0.5], [0 0]);  

% Add a systematic bias to the range estimates
Err_model_is.mu = Err_model_is.mu+optcomp.bias;                          
optcomp.bias = 0;

%% Defining the initial GM-distribution for joint localization and GM update

% Creating the class
if opti_flag
    MGMP = on_the_fly_MGMP(gm, Anchors, optcomp.type, optcomp.bias, X_grid, Y_grid, lb, ub);
else
    MGMP = on_the_fly_MGMP(gm, Anchors, optcomp.type, optcomp.bias, X_grid, Y_grid);
end

% Generating noise samples between anchors for GM initialization
range_err_Anch = rand(Err_model_is, [1 ((size(Anchors,1))*(size(Anchors,1)-1))/2]);
MGMP = MGMP.GM_up_anch(range_err_Anch);

% Generating the Measurements of the agent for GM initialization
for cnt=1:NofSim
    dist(cnt,:) = sqrt(sum((Anchors-ones(size(Anchors,1),1)*x_is(cnt,:)).^2, 2)); %#ok<*SAGROW>
end

% Generate ranging error according to Err_model_is.
range_err = rand(Err_model_is, size(dist));       


% The distance estimates, incl. range errors.
Ranges = dist+range_err;                                                                    

%% Computing the best rms of errors with perfect knowledge of channel/ML approach
x_init=x_is;      
x_best_est = zeros(size(x_is));

for cnt=1:NofSim
       [x_best_est(cnt,:)] = MGMP.ML_pos(Ranges(cnt,:).', x_is(cnt,:), Err_model_is); %#ok<*PFBNS>
end

err_best_est = x_best_est-x_is;
rms_best_est = sqrt( mean( sum((err_best_est).^2,2) ) );

%% Computing the rms of errors with no knowledge of channel/LS-type approach
x_LStype_est = zeros(size(x_is));

for cnt=1:NofSim
    x_LStype_est(cnt,:) = MGMP.LS_pos(Ranges(cnt,:).'); 
end

err_LStype = x_LStype_est-x_is;
rms_LStype=sqrt(mean(sum((err_LStype).^2,2)));

%% MGMP loop

loop_no = size(Ranges,1);
for loo=1:loop_no
    [MGMP,x_est(loo,:)] = MGMP.do(Ranges(loo,:).',loo);
    %MGMP.Err_model_est{3}
end

% RMS of MGMP computation
err_est = x_est-x_is;
rms_est = sqrt(mean(sum((err_est).^2,2))); 

%% Results

%     profreport

    MARKERS={'rx','go','bs','c>','k<','rx','cp','ms','g>','k<','rx','cp','ms','g>','k<'};
    marker={'bh:','bo:','bd:','bs:'};
    
% Plotting the localization error for different Measurements

    figure(111);
    hdl111(1) = plot(sqrt(sum(abs(err_est).^2, 2)), MARKERS{1});
    hold on;
    hdl111(2) = plot(xlim,[1 1]*rms_est, '--');
    set(hdl111(2),'color',get(hdl111(1), 'color'));
    LegendTXT111{1} = 'err momentum-based';
    LegendTXT111{2} = 'rms momentum-based';   
    
    hdl111(3) = plot(sqrt(sum(abs(err_best_est).^2, 2)), MARKERS{2});
    hdl111(4) = plot(xlim,[1 1]*rms_best_est, '--');
    set(hdl111(4),'color',get(hdl111(3), 'color'));
    LegendTXT111{3} = 'err ML';
    LegendTXT111{4} = 'rms ML';
    
    hdl111(5) = plot(sqrt(sum(abs(err_LStype).^2, 2)), MARKERS{3});
    hdl111(6) = plot(xlim,[1 1]*rms_LStype, '--');
    set(hdl111(6),'color',get(hdl111(5), 'color'));
    LegendTXT111{5} = ['err' ' ' optcomp.type];
    LegendTXT111{6} = ['rms' ' ' optcomp.type];
    
    legend(hdl111(:),LegendTXT111{:})

% Plotting the CDF of the initial GM, GM updated by anchors, and estimated GM       
    
    figure(222);
    bins = -1:0.1:5;
    hdlsFig222 = plot(bins,cdf(Err_model_is, bins));hold on
    legendTXT222{1} = 'Original';
    hdlsFig222(2) = plot(bins,cdf(MGMP.Err_model_est{1}, bins));
    legendTXT222{2} = 'Initial';
    hdlsFig222(3) = plot(bins,cdf(MGMP.Err_model_est{2}, bins));
    legendTXT222{3} = 'GM EM by Anchors';
    hdlsFig222(4) = plot(bins,cdf(MGMP.Err_model_est{end}, bins));
    legendTXT222{4} = 'GM momentum-based';
    legend(legendTXT222,'location','SouthEast');
    ylabel('CDF'); xlabel('error (m)')
    title('comparison of the CDFs')
     

% Ploting the estimated positions for ML, LS, MGMP, and true positions
    
    figure(444);
    hold on
    hdl444(1) = plot(x_is(:,1), x_is(:,2), 'go');
    axis(7*[-1 1 -1 1])
    set(hdl444(1), 'color', get(hdl111(3), 'color'));
    
    hdl444(2) = plot(x_best_est(:,1), x_best_est(:,2), 'gp');
    plot([x_is(:,1) x_best_est(:,1)]', [x_is(:,2) x_best_est(:,2)]', 'g:')
    set(hdl444(2), 'color', get(hdl111(3), 'color'));
    
    hdl444(3) = plot(x_est(:,1), x_est(:,2), 'rx');
    plot([x_is(:,1) x_est(:,1)]', [x_is(:,2) x_est(:,2)]', 'r:')
    set(hdl444(3), 'color', get(hdl111(1), 'color'));
    
    hdl444(4) = plot(x_LStype_est(:,1),x_LStype_est(:,2),'bs');
    plot([x_is(:,1) x_LStype_est(:,1)]',[x_is(:,2) x_LStype_est(:,2)]', 'b:')    
    set(hdl444(4),'color', get(hdl111(5), 'color'));
    
    hdl444(5) = plot(Anchors(:,1), Anchors(:,2), 'k*'); 
   
    LegendTXT444{1} = 'Trajectory';
    LegendTXT444{2} = 'Estimated position ML';
    LegendTXT444{3} = 'Estimated position momentum-based'; 
    LegendTXT444{4} = ['Estimated position' ' ' optcomp.type];     
    LegendTXT444{5} = 'Anchor position';     
    legend(hdl444(:), LegendTXT444{:});
    ylabel('y (m)'); xlabel('x (m)')
    title(sprintf('RMSE ML = %.2fm, RMSE LS = %.2fm, RMSE On-the-fly = %.2fm', rms_best_est, rms_LStype, rms_est))
    
% Ploting the CDF of error for Perfect CSI and On-the-fly Est.   
    
    figure(555);
    hdl555(1) = plot(sort(sqrt(sum(abs(err_est).^2,2))), (1:NofSim)/NofSim);
    set(hdl555(1), 'color', get(hdl111(1),'color'));
    hold on;
    hdl555(2) = plot(sort(sqrt(sum(abs(err_best_est).^2,2))), (1:NofSim)/NofSim);
    set(hdl555(2), 'color', get(hdl111(3), 'color')); 
    hdl555(3) = plot(sort(sqrt(sum(abs(err_LStype).^2,2))), (1:NofSim)/NofSim);
    set(hdl555(3), 'color', get(hdl111(5), 'color'));     
    LegendTXT555{1} = 'momentum-based error';
    LegendTXT555{2} = 'ML error'; 
    LegendTXT555{3} = [optcomp.type ' ' 'error']; 
    legend(LegendTXT555, 'location', 'SouthEast');
    ylabel('CDF'); xlabel('error (m)')
end