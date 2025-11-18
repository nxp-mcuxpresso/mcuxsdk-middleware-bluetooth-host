%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This file tests the one-dimensional distance tracking algorithm using
%   Kalman filter and proposed Adaptive Kalman filter.
% Description end

classdef testTracking < matlab.unittest.TestCase
    properties
        TrajectoryPoints = {[]}
        alpha = 1.2                                                         % Related to the time between two tones
        x_true
        x_rel
        d_ss
        num_samples
        SamplingTime = 0.1                                                  % (s) Time between two measurements in social-force model
    end
    
    properties (TestParameter)
        Estimator = struct('KF', Tracking_Kalman)
        adaptive = [{false},{true}]
        Ts = [{.1},{.3},{1}];                                               % (s) Time between two measurements in ranging-system
    end                                                                     
    
    methods (TestClassSetup)
        function generate_reference_trajectory(self)
            trajectory = generate_trajectory(self.TrajectoryPoints,self.SamplingTime,self.alpha);
            self.x_true = trajectory.x_true;
            self.x_rel = trajectory.x_rel;
            self.d_ss = trajectory.d_MCPD;
            self.num_samples = length(self.d_ss);
        end
    end
    methods
        function x_est = test_tracking(self,TE,Ts)
            if mod(Ts,self.SamplingTime)~=0
                error('cannot analyze the tracking engine using the desired sampling time');
            else
                
                Ts_ratio = round(Ts/self.SamplingTime);
                self.x_true = self.x_true(:,1:Ts_ratio:end);
                self.x_rel = self.x_rel(:,1:Ts_ratio:end);
                self.d_ss = self.d_ss(1:Ts_ratio:end);
                self.num_samples = length(self.d_ss);
                x_est = nan(2,self.num_samples);
                % state and Covariance Initialization
                for k = 1:self.num_samples
                    [x_est(:,k)] = TE.do(self.d_ss(k), Ts);
                end
            end
        end
    end
    
    methods (Test, ParameterCombination='exhaustive', TestTags  = {'Quick', 'Unit'})  % These functions describe all tests that can be performed within this class.
        function test_trackingsystem(self, Estimator, Ts, adaptive)
            PLOT                 = false;
            Estimator.AgentTrack = [];
            Estimator.alpha      = self.alpha;
            Estimator.adaptive   = adaptive;
            x_est                = self.test_tracking(Estimator, Ts);
            d_err                = x_est(1,:) - self.x_rel(1,:);
            dss_err              = self.d_ss-self.x_rel(1,:);
            P0                   = Estimator.initial_cov;
            
            import matlab.unittest.Verbosity
            self.log(Verbosity.Detailed, sprintf('%s error [m]: %.2f ± %.2f', class(Estimator), mean(d_err), std(d_err)));
            self.log(Verbosity.Detailed, sprintf('covariance: \n[%.2f %.2f;\n %.2f %.2f] -> \n[%.2f %.2f;\n %.2f %.2f]', P0, Estimator.AgentTrack.P));
            if PLOT
                v_err = x_est(2,:) - self.x_rel(2,:); %#ok<UNRCH>

                figure;
                subplot(2,1,1); hold on;
                plot(d_err);
                plot(dss_err); 
                ylabel('distance estimation error [m]');
                legend('Tracking','single shot','location','best')
                title({[class(Estimator),' - ',' - sampling time=',num2str(Ts), ' s'];...
                       ['RMSE=',num2str(rms(d_err)),' m']},'Interpreter','none');
                subplot(2,1,2);
                plot(v_err);
                ylabel('velocity estimation error [m/s]');
                title(['RMSE=',num2str(rms(d_err)),' m/s'],'Interpreter','none');
                legend('Tracking')
            end
            self.verifyGreaterThan(rms(dss_err), rms(d_err))
        end
        
        function test_trackingengine_passthrough(self)
            TE = Estimator_Passthrough();
            [dist, velocity] = TE.do(10, 0.1234);
            self.verifyEqual(dist, 10);
            self.verifyEqual(velocity, NaN);
        end
    end
end