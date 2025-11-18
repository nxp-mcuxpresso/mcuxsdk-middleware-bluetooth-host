%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This class implements an adaptive Kalman filter for tracking using range
%   estimations
% to be completed for multiple anchor
% Description end

classdef Tracking_Kalman < TrackingEngineAbstract
    properties %all the properties are public
        AgentTrack;
        iAgent;
        n_var = 5;
        outlier_removal = false;
        outlier_factor = 5;
        adaptive = true;
        likeliness_threshold = 0.1;
    end
    
    methods
        function self = Tracking_Kalman()
            self.AgentTrack.a_prev = [];
        end
        
        %%
        function [xhat] = do(self,d_ss,Ts)
            
            QI = self.quality_index;
            if isempty(self.AgentTrack) || ~isfield(self.AgentTrack,'x_est') || isnan(self.AgentTrack.x_est(1,end))
                % initialize the filter
                self.AgentTrack.a_prev = [];
                xhat = [mean(d_ss) 0]';
                P = self.initial_cov;
                self.AgentTrack.outlier = 0;
            else
                x_old = self.AgentTrack.x_est;
                P = self.AgentTrack.P;
                var_a = self.processNoise(self.AgentTrack.a_prev,self.n_var,self.a_std^2);
                [F,Q] = self.ProcessModel(Ts,var_a);
                xhat_plus = F*x_old;
                Pplus = F*P*F'+Q;
                if isnan(sum(d_ss)) || QI<self.likeliness_threshold
                    xhat = xhat_plus;
                    P = Pplus;
                    self.AgentTrack.outlier = 1;
                else
                    [H,R,self.AgentTrack.outlier] = self.ObservationModel(xhat_plus,Pplus,d_ss,QI);
                    Y = d_ss-H*xhat_plus;
                    S =  R+H*Pplus*H';
                    K = Pplus*H'/S;
                    
                    xhat= xhat_plus+K*Y;
                    P = (eye(size(Pplus))-K*H)*Pplus;
                end
            end
            self.AgentTrack.x_est = xhat;
            self.AgentTrack.P = P;
            self.AgentTrack.d_ss = d_ss;
            
            self.AgentTrack.a_prev = [self.AgentTrack.a_prev self.AgentTrack.x_est(2)/Ts];
            if length(self.AgentTrack.a_prev)>self.n_var
                self.AgentTrack.a_prev = self.AgentTrack.a_prev(end-self.n_var+1:end) ;
            end
        end
        
        %% process function
        function [F,Q] = ProcessModel(self,Ts,var_a)
            F = [1 Ts; 0 0.99];
            G = [1/2*Ts^2; Ts];
            Q = G*var_a*G'+[self.range_process_variance 0; 0 self.velocity_process_variance];
        end
        
        
        %% observation function uncertainty
        function [H,R,outlier] = ObservationModel(self,xhat_plus,Pplus,d_ss,QI)
            outlier = 0;
            H = [1 self.alpha];
            Y = d_ss-H*xhat_plus;
            
            if self.adaptive
                R = self.dss_var_params(1)*exp(self.dss_var_params(2)*QI); %self.dss_std^2;%
                S = R+H*Pplus*H';
                if Y^2>S
                    R = (R*(Y^2/S));
                end
                if Y^2>self.outlier_factor*S
                    outlier = 1;
                end
            else
                R = self.dss_std^2;
                S = R+H*Pplus*H';
                if Y^2>self.outlier_factor*S
                    outlier = 1;
                end
            end
        end
        
        %% adaptive acceleration variance
        function var_a = processNoise(self,a_prev,n_var,a_var_ref)
            var_a = a_var_ref;
            if self.adaptive && length(a_prev)>=n_var
                v = var(a_prev);
                if v>var_a
                    var_a = v;
                end
            end
        end
        
        
    end
    
    
    methods (Static)
    end
end