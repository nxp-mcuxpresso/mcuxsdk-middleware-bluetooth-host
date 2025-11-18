%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This file comprise the core of MGMP algorithm 
%   (on-the-fly positioning and Gaussian mixture estimation) as an object
% Description end

classdef on_the_fly_MGMP < AbstractMGMP
    
    properties
        
        LS_compare                          % Parameters of Least square optim. for comparision
        X_grid                              % X-axis grid for init. of opt.
        Y_grid                              % Y-axis grid for init. of opt.
        lb                                  % lower_bound for init. of opt.
        ub                                  % upper_bound for init. of opt.
        Err_model_est                       % Cell containing initial GM Err_model_est{1}, initial GM updated by anchors GM Err_model_est{2} ...
                                            % and current GM estimation GM Err_model_est{3}
        Anchors                             % Matrix containing the relative X-Y positions of anchors with respoct to origin (0,0)
        
    end
    
    methods
        
        function obj = on_the_fly_MGMP(varargin)                           %% Constructor
            
            p = inputParser;
            
            % Checks on the constructor inputs
            p.addOptional('gm', gmdistr_ECM_imec([1 2], [0.5 0.5], [0.5 0.5]), @isobject)
            p.addOptional('Anchors', [-10 10;-10 0;-10 10;0 10;0 -10;10 -10;10 0;10 10],@(x) validateattributes(x, {'numeric'}, {'2d'}, mfilename, 'Anchors'))
            p.addOptional('LStype', 'LS', @ischar)
            p.addOptional('bias', 0, @isnumeric)
            p.addOptional('X_grid', -10:0.5:10, @(x) validateattributes(x, {'numeric'}, {'vector'}, mfilename, 'X-plane Grid'))
            p.addOptional('Y_grid', -10:0.5:10, @(x) validateattributes(x, {'numeric'}, {'vector'}, mfilename, 'Y-plane Grid'))
            p.addOptional('lb', [], @(x) validateattributes(x, {'numeric'}, {'2d'}, mfilename, 'lb'))
            p.addOptional('ub', [], @(x) validateattributes(x, {'numeric'}, {'2d'}, mfilename, 'ub'))
            p.parse(varargin{:})
            
            % Define prop. of class
            obj.X_grid = p.Results.X_grid;
            obj.Y_grid = p.Results.Y_grid;
            obj.lb = p.Results.lb;
            obj.ub = p.Results.ub;
            obj.Anchors = p.Results.Anchors;
            obj.Err_model_est{1} = p.Results.gm;
            obj.LS_compare.type = p.Results.LStype;
            obj.LS_compare.bias = p.Results.bias;
            
        end
        
        %% MGMP update using range measurments between anchors (Step 1 in MGMP alg.)
        function  obj = GM_up_anch(obj, range_err)
            if range_err ~= zeros( size(range_err) )
                [obj.Err_model_est{2}, ~] = gmfit_Anchor( obj.Err_model_est{1}, range_err(:), obj.anch_comp_init, obj.Flag_inputGM_init,...
                    obj.w_th_anch, obj.STD_th_anch, obj.Flag_red_anch );
            else
                obj.Err_model_est{2} = obj.Err_model_est{1};
                obj.Err_model_est{2}.w = 0*obj.Err_model_est{2}.w;
            end
        end
        
        %% Main routine for MGMP algorithm
        function  [obj, x_est] = do(obj, Ranges, timestep, beta)
            
            if obj.Err_model_est{end}.w==zeros( size(obj.Err_model_est{end}.w) )
              
                obj.LS_compare.type = 'LS';
                x_est = LS_pos(obj, Ranges);
                obj.Err_model_est{3} = obj.Err_model_est{end};
                
            else
                % Choose whether input beta or predefined beta is used
                if nargin~=4
                    % Merge parameter (beta) selection
                    if timestep <= numel(obj.beta)
                        beta = obj.beta(timestep);
                    else
                        beta = obj.beta(end);
                    end
                end
                
                % Last GM estimation as the input GM for MGMP
                Err_model_is_on_fly_in = obj.Err_model_est{end};
                
                ind_sel = 1;
                
                % Grid search to find init. point for bounded/unbounded optim. of localization
                COST = NaN*zeros( numel(obj.X_grid), numel(obj.Y_grid) );
                len_xg = numel(obj.X_grid);
                len_yg = numel(obj.Y_grid);
                for cnt_x = 1:len_xg
                    temp = zeros(1,len_yg);
                    for cnt_y = 1:len_yg
                        temp(cnt_y) = Cost_Localization_ECM_Mstep( [obj.X_grid(cnt_x) obj.Y_grid(cnt_y)].',...
                            obj.Anchors(:,1:2), Ranges, Err_model_is_on_fly_in );
                    end
                    COST(cnt_x,:) = temp;
                end
                minMatrix = min(COST(:));
                [row,col] = find(COST == minMatrix);
                x_init2=[obj.X_grid(row(1)) obj.Y_grid(col(1))];
                
                x_init = x_init2;
                
                COST = []; %#ok<NASGU>
                
                %% Bounded/unbounded optim. of localization
                if ~isempty(obj.lb) || ~isempty(obj.ub)
                    opts = optiset('solver','NOMAD');
                    Opt = opti('fun',@(x) Cost_Localization_ECM_Mstep_multi( x, obj.Anchors(:,1:2), Ranges, Err_model_is_on_fly_in), ...
                        'bounds', obj.lb, obj.ub, 'x0', x_init.', 'options', opts );
                    x_est(ind_sel, :) = solve(Opt);
                else
                    [x_est(ind_sel,:)] = fminsearch( @(x) Cost_Localization_ECM_Mstep_multi(x, obj.Anchors(:,1:2), Ranges, ...
                        Err_model_is_on_fly_in), x_init.' );
                end
                
                % compute the noise samples based on ranging to anchors
                dist_est(ind_sel, :) = sqrt(sum((obj.Anchors-ones(size(obj.Anchors, 1), 1)*x_est(ind_sel,:)).^2, 2));
                range_err_est(ind_sel, :) = Ranges.'-dist_est(ind_sel, :);
                
                %% (ECM, momentum-based GM update, GM merging, and localization (Step 2-4 in MGMP alg.)
                
                Err_model_is_on_fly_temp = Err_model_is_on_fly_in;
                
                % GM update based on noise samples
                [Err_model_is_on_fly_temp] = Update_GM_MGMP( Err_model_is_on_fly_temp, range_err_est(ind_sel, :) );
                
                % Find which components should be merged with each other
                [comp_ind] = GM_compare_MGMP( Err_model_is_on_fly_in, Err_model_is_on_fly_temp );
                
                % Momentum-based GM update and GM merging
                Err_model_is_on_fly_out = GM_merge( Err_model_is_on_fly_in, Err_model_is_on_fly_temp, beta,comp_ind );
                
                % Find the KL div. between two consecutive GM est.
                [Q_evol] = KL_div( Err_model_is_on_fly_out,Err_model_is_on_fly_in );
                
                % Check whether the merging should be accepted or not
                mean_KL = (obj.KL_evol(1));
                
                if Q_evol > 20*mean_KL
                    Err_model_is_on_fly_out = Err_model_is_on_fly_in;
                    obj.KL_evol(1) = (obj.KL_evol(2)*obj.KL_evol(1))/(obj.KL_evol(2)+1);
                    obj.KL_evol(2) = obj.KL_evol(2)+1;
                    
                elseif timestep ~= 1
                    obj.KL_evol(1) = (obj.KL_evol(2)*obj.KL_evol(1)+Q_evol)/(obj.KL_evol(2)+1);
                    obj.KL_evol(2) = obj.KL_evol(2)+1;
                    
                else
                    obj.KL_evol(1) = Q_evol;
                    obj.KL_evol(2) = obj.KL_evol(2)+1;
                    
                end
                
                % Localization based on the updated GM estimation
                if ~isempty(obj.lb) || ~isempty(obj.ub)
                    opts = optiset('solver','NOMAD');
                    Opt = opti( 'fun',@(x) Cost_Localization_ECM_Mstep_multi(x, obj.Anchors(:,1:2), Ranges,Err_model_is_on_fly_out),...
                        'bounds', obj.lb, obj.ub, 'x0', x_est(ind_sel,:).', 'options',opts );
                    x_est(ind_sel,:) = solve(Opt);
                else
                    [x_est(ind_sel,:)] = fminsearch( @(x) Cost_Localization_ECM_Mstep_multi(x, obj.Anchors(:,1:2), Ranges, Err_model_is_on_fly_out)...
                        ,x_est(ind_sel,:).' );
                end
                
                % Update GM for localization in the next time-step
                obj.Err_model_est{3} = Err_model_is_on_fly_out;
                
                
            end
            
        end
        
        %% ML positioning based on true position (upper bound)
        function  [x_est] = ML_pos(obj, Ranges, x_is, Err_model_is)
            
            if ~isempty(obj.lb) || ~isempty(obj.ub)
                opts = optiset('solver','NOMAD');
                Opt = opti( 'fun', @(x) Cost_Localization_cin(x, obj.Anchors(:,1:2), Ranges, Err_model_is),...
                            'bounds', obj.lb, obj.ub, 'x0', x_is(1:2).', 'options', opts );
                x_est = solve(Opt);
            else
                [x_est] = fminsearch( @(x) Cost_Localization_cin(x, obj.Anchors(:,1:2), Ranges, Err_model_is), x_is(1:2).');
            end
        end
        
        %% LS-type positioning
        function  [x_est] = LS_pos(obj, Ranges)
            
            % Grid search to find init. point for bounded/unbounded opt. of localization
            COST = NaN*zeros(numel(obj.X_grid), numel(obj.Y_grid));
            len_xg = numel(obj.X_grid);
            len_yg = numel(obj.Y_grid);
            
            for cnt_x = 1:numel(len_xg)
                temp = zeros(1,numel(len_yg));
                for cnt_y = 1:numel(len_yg)
                    temp(cnt_y) = Cost_Localization_cin( [obj.X_grid(cnt_x) obj.Y_grid(cnt_y)].', obj.Anchors(:,1:2), ...
                                                         Ranges, [], obj.LS_compare );
                end
                COST(cnt_x,:) = temp;
            end
            minMatrix = min(COST(:));
            [row,col] = find(COST == minMatrix);
            x_init2=[obj.X_grid(row(1)) obj.Y_grid(col(1))];
            
            % Final Localization
            if ~isempty(obj.lb) || ~isempty(obj.ub)
                opts = optiset('solver','NOMAD');
                Opt = opti('fun',@(x) Cost_Localization_cin( x, obj.Anchors(:,1:2), Ranges, [], obj.LS_compare),...
                                                            'bounds', obj.lb, obj.ub, 'x0', x_init2.', 'options', opts );
                x_est=solve(Opt);
            else
                [x_est]=fminsearch(@(x) Cost_Localization_cin( x, obj.Anchors(:,1:2), Ranges, [], obj.LS_compare), x_init2.' );
            end
            
        end
        
    end
end