%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This file tests the on-the-fly positioning and Gaussian mixture estimation
%   using the proposed MGMP algorithm
% Description end

classdef testMGMP < matlab.unittest.TestCase
    
    properties
        shape = 'square';
        step_size = 0.5;
        Anchors = [0 0; 0 0.5; 0 1; 0.5 1; 0.5 0; 1 0; 1 0.5; 1 1]*10;
        Tra_rep = 1;
        opti_flag = ispc;
    end
    
    properties (TestParameter)
        Seed_test = {530, 4000, 7800};
        rms_test = {0.5494, 0.5792, 0.5948};
        
    end                                                                     
    
    methods (TestClassSetup)
        function Init_setting(obj)
            obj.applyFixture( setupEnv );

            W = which('set_paths_NRBLOC');
            [BaseDir, ~, ~] = fileparts(W);                                 % Must run from this dir, because many scripting is using this dir as base
            if contains(path, 'OPTI_toolbox')   % If it is present in the path the OPTI_toolbox is installed
                obj.opti_flag = ispc;           % The OPTI_toolbox only works on a PC
            else                                
                if ispc     % The toolbox is not installed yet, we will try to install on a PC
                    opti_tool_path = fullfile(BaseDir,'ThirdParty', 'OPTI_toolbox');
                    if exist(opti_tool_path, 'dir') ~= 0    % Check whether the path exists
                        cpwd = pwd;
                        cd(opti_tool_path)
                        if isfile('opti_Install.m') % Check whether the install-file exists (if it does not exist, the submodule needs to be updated)
                            opti_Install(true, true, false)
                            obj.opti_flag = true;
                        else
                            warning('OPTI_toolbox is not found. Fminsearch is used for optimization.')
                            obj.opti_flag = false;
                        end
                        cd(cpwd);
                    else
                        warning('OPTI_toolbox is not found. Fminsearch is used for optimization.')
                        obj.opti_flag = false;
                    end
                end
            end
        end
    end
    
    methods
        
        % Generate the relative position of anchors to the origin
        function obj = testMGMP(obj) %#ok<*INUSD>
            obj.Anchors = obj.Anchors-ones(size(obj.Anchors,1),1)*mean(obj.Anchors);
        end
        
        % Generate Trajectory for testing the positioning
        function traj = generate_trajectory(obj)
            
            traj = trajectory_gen(obj.shape,obj.step_size,obj.Anchors);
            x_is = traj.pos_xy;
            x_temp = x_is;
            for i=1:obj.Tra_rep
                x_is = [x_is;x_temp]; %#ok<*AGROW>
            end
            traj.pos_xy = x_is;
        end
        
        % Initialize the MGMP and provides the ranges for testing
        function [MGMP, Ranges] = MGMP_Engine(obj, traj, seed_init, Err_model_is)
            
            rng(seed_init)
            
            x_is = traj.pos_xy;

            %%% Initializing the GM
            gm = gmdistr_ECM_imec( [1 2], [0.5 0.5], [0.5 0.5] );
            
            % X-axis grid for init of opt.
            X_grid = min(obj.Anchors(:,1)):0.5:max(obj.Anchors(:,1));  
            % Y-axis grid for init of opt.
            Y_grid = min(obj.Anchors(:,2)):0.5:max(obj.Anchors(:,2));  
            % lower_bound for init of opt
            lb = [min(obj.Anchors(:,1)); min(obj.Anchors(:,2))];   
            % upper_bound for init of opt
            ub = [max(obj.Anchors(:,2)); max(obj.Anchors(:,2))];  
            
            if obj.opti_flag
                MGMP = on_the_fly_MGMP(gm, obj.Anchors, '', 0, X_grid, Y_grid, lb, ub);
            else
                MGMP = on_the_fly_MGMP(gm, obj.Anchors, '', 0, X_grid, Y_grid);
            end
            
            
            % Generating noise samples between anchors for GM initialization
            range_err_Anch = rand(Err_model_is, [1 ((size(obj.Anchors,1))*(size(obj.Anchors,1)-1))/2]);
            MGMP = MGMP.GM_up_anch(range_err_Anch);
            
            NofSim = size(x_is,1);
            % Generating the measurements of the agent for GM initialization
            for cnt=1:NofSim
                dist(cnt,:) = sqrt(sum((obj.Anchors-ones(size(obj.Anchors,1),1)*x_is(cnt,:)).^2, 2));
            end

            % Generate ranging error according to Err_model_is
            range_err = rand(Err_model_is, size(dist));       

            % The distance estimates, incl. range errors
            Ranges = dist+range_err;
  
        end
        
    end
    
    methods (Test, ParameterCombination='exhaustive', TestTags  = {'Quick', 'Unit'})  % These functions describe all tests that can be performed within this class.

        % Class must be able to be constructed without input arguments
        % (https://nl.mathworks.com/help/matlab/matlab_oop/class-constructor-methods.html#btn2kiy)
        function Test_construction_without_input_args(obj)
            obj.verifyWarningFree(@on_the_fly_MGMP);
        end
        
        % Test the MGMP no ranging error
        function Test_MGMP_no_ranging_error(obj)
            
            traj = generate_trajectory(obj);
            traj.pos_xy(1:floor(size(traj.pos_xy,1))/2,:);
            
            % Define the true error distribution of the GM
            Err_model_is = gmdistr_imec([0.5 1], [0.5 1], [0 0]);
            Seed_check = randi([1 100000],1,1);
            
            [MGMP, Ranges] = MGMP_Engine(obj, traj, Seed_check, Err_model_is);
            loop_no = size(Ranges, 1);
            msg4 = 'RMS of localization algorithm is not correct';
            for loo=1:loop_no
                [MGMP,x_est(loo,:)] = MGMP.do(Ranges(loo,:).', loo);
            end
            x_is = traj.pos_xy;
            err_est = x_est-x_is;
            rms_est = sqrt(mean(sum((err_est).^2,2)));
            
            % Check no error in case of no ranging error
            obj.verifyEqual(rms_est, 0, 'AbsTol', 10^(-4), msg4);
            
        end
        
        % Test the MGMP for a random seed          
        function Test_rand_seed_MGMP_system(obj)
            
            traj = generate_trajectory(obj);
            traj.pos_xy(1:floor(size(traj.pos_xy,1))/2, :);
            
            % Define the true error distribution of the GM
            Err_model_is = gmdistr_imec();
            Seed_rand = randi([1 100000], 1, 1);
            
            [MGMP,Ranges]=MGMP_Engine(obj, traj, Seed_rand, Err_model_is);
            loop_no = size(Ranges,1);
            msg1 = 'The standard deviation of components is non-negative';
            msg2 = 'The probabilities of components is non-negative';
            msg3 = 'The sum of components probabilities should be equal to one';
            
            for loo=1:loop_no
                [MGMP,x_est(loo,:)] = MGMP.do(Ranges(loo,:).', loo);
                
                % Check the parameters of MGMP are valid
                obj.verifySize(x_est(loo,:), [1 2]);
                obj.assertLessThanOrEqual(-MGMP.Err_model_est{3}.sigma, zeros(size(MGMP.Err_model_est{3}.sigma)), msg1)
                obj.assertLessThanOrEqual(-MGMP.Err_model_est{3}.w, zeros(size(MGMP.Err_model_est{3}.w)), msg2)
                obj.assertEqual(sum(MGMP.Err_model_est{3}.w), 1, 'AbsTol', sqrt(eps), msg3); 
                
            end
            
        
        end

        % Test the MGMP for some predifiened seeds and also check the
        % estimated GM and the result of positioning are valid 
        function Test_def_seed_MGMP_system(obj, Seed_test)
            
            traj = generate_trajectory(obj);
            
            % Define the true error distribution of the GM
            Err_model_is = gmdistr_imec();  
            
            ind_temp = find([obj.Seed_test{:}]==Seed_test);
            rms_vec = [obj.rms_test{:}];
            rms_rng = rms_vec(ind_temp(1));
            
            [MGMP,Ranges]=MGMP_Engine(obj, traj, Seed_test, Err_model_is);
            loop_no = size(Ranges, 1);
            msg1 = 'The standard deviation of components is non-negative';
            msg2 = 'The probabilities of components is non-negative';
            msg3 = 'The sum of components probabilities should be equal to one';
            msg4 = 'RMS of localization algorithm is not correct';
            
            for loo=1:loop_no
                [MGMP,x_est(loo,:)] = MGMP.do(Ranges(loo,:).', loo);           
                % Check the parameters of MGMP are valid
                obj.verifySize(x_est(loo,:), [1 2]);
                obj.assertLessThanOrEqual(-MGMP.Err_model_est{3}.sigma, zeros(size(MGMP.Err_model_est{3}.sigma)), msg1)
                obj.assertLessThanOrEqual(-MGMP.Err_model_est{3}.w, zeros(size(MGMP.Err_model_est{3}.w)), msg2)
                obj.assertEqual(sum(MGMP.Err_model_est{3}.w), 1, 'AbsTol', sqrt(eps), msg3); 
            end
            x_is = traj.pos_xy;
            err_est = x_est-x_is;
            rms_est = sqrt(mean(sum((err_est).^2,2)));
            % Check the rms of MGMP positioning
            obj.verifyEqual(rms_est, rms_rng, 'AbsTol', 10^(-2), msg4);
            
            % Plot the positining performance and CDF of the GM estimation
            figure;
            subplot 121; box on; 
            hdl(1) = plot(x_is(:,1), x_is(:,2), 'go');
            axis(7*[-1 1 -1 1])
            hold on;
            hdl(2) = plot(x_est(:,1), x_est(:,2), 'rx');
            plot([x_is(:,1) x_est(:,1)]', [x_is(:,2) x_est(:,2)]', 'r:')
            
            hdl(3) = plot(obj.Anchors(:,1), obj.Anchors(:,2), 'k*'); 

            LegendTXT{1} = 'Trajectory';
            LegendTXT{2} = 'Estimated position momentum-based';   
            LegendTXT{3} = 'Anchor position';     
            legend(hdl(:), LegendTXT{:});
            ylabel('y (m)'); xlabel('x (m)')
            title(sprintf('RMSE MGMP = %.2fm', rms_est)) 
            
            subplot 122;
            bins=-1:0.1:6;
            plot(bins,cdf(Err_model_is, bins));hold on
            legendTXT{1} = 'Original';
            plot(bins,cdf(MGMP.Err_model_est{1}, bins));
            legendTXT{2} = 'Initial';
            plot(bins,cdf(MGMP.Err_model_est{2}, bins));
            legendTXT{3} = 'GM EM by Anchors';
            plot(bins,cdf(MGMP.Err_model_est{end}, bins));
            legendTXT{4} = 'GM momentum-based';
            legend(legendTXT,'location','SouthEast');
            ylabel('CDF'); xlabel('error (m)')
            title('comparison of the CDFs')
            
        end
         
        
    end
    
    
end