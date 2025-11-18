%--------------------------------------------------------------------------
% Project   : NRB ranging
% Version   : GIT
% Author    : Payam Nazemzadeh (payam.nazemzdeh@imec.nl)
% Copyright : Stichting Imec Nederland (http://www.imec.nl)
% Disclaimer: Imec strictly confidential
%--------------------------------------------------------------------------
% Description
%   This class implements an adaptive Kalman filter for tracking using range
%   estimations
% to be completed for multiple anchor
% Description end
classdef Estimator_AKF < TrackingEngineAbstract
    properties %all the properties are public
        AgentTrack;
        iAgent;
        n_var = 5;
        outlier_removal = false;
    end
    
    methods
        function self = Estimator_AKF()
            self.AgentTrack.a_prev = [];
        end
        
        %%
        function [xhat] = do(self,model,d_ss,Ts)
            var_v = 0;
            var_a = 0;
            likeliness = 1;
            body_block = 0;
            body_offset = 1.5;
            self.AgentTrack.outlier = 0;
            if isempty(self.AgentTrack) || ~isfield(self.AgentTrack,'x_est') || isnan(self.AgentTrack.x_est(1,end))
                % initialize the filter
                self.AgentTrack.a_prev = [];
                xhat = [d_ss 0]';
                P = self.initial_cov;
            else
                x_old = self.AgentTrack.x_est;
                P = self.AgentTrack.P;
                var_a = Estimator_AKF.adaptive_processNoise(self.AgentTrack.a_prev,self.n_var,self.a_std^2);
                [xhat_plus,Q] = model.do(x_old,Ts,var_a);
                F = model.ProcessMatrix(Ts);
                Pplus = F*P*F'+Q;
                if isnan(sum(d_ss))
                    xhat = xhat_plus;
                    P = Pplus;
                else
                    [H,R] = model.ObservationModel(self.dss_std, self.alpha);
                    Y = H*xhat_plus;
                    Z = d_ss-Y;
                    if body_block %&& Z>0 && likeliness<0.3
                        Z = Z-1.5;
                    end
                    
                    S = R+H*Pplus*H';
                    if abs(Z)>S
                        R = (R*(Z/S)^2);
                    end
                    if abs(Z)>5*S
                        self.AgentTrack.outlier = 1;
                    end
                    R = R/likeliness;
                    S =  R+H*Pplus*H';
                    K = Pplus*H'/S;
                    
                    xhat= xhat_plus+K*Z;
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
%             self.AgentTrack.var_v = var_v;
%             self.AgentTrack.var_a = var_a;
        end
    end
    methods (Static)
        function var_a = adaptive_processNoise(a_prev,n_var,a_var_ref)
            var_a = a_var_ref;
            if length(a_prev)>=n_var
                v = var(a_prev);
                if v>var_a
                    var_a = v;
                end
            end
        end
    end
end