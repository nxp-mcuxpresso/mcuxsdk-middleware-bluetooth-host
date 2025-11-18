%--------------------------------------------------------------------------
% Project   : NRB ranging
% Version   : GIT
% Author    : Payam Nazemzadeh (payam.nazemzdeh@imec-nl.nl)
% Copyright : Stichting Imec Nederland (http://www.imec-nl.nl)
% Disclaimer: Imec strictly confidential
%--------------------------------------------------------------------------
% Description
%   This class implements a Kalman filter for tracking using measured
%   range. The doppler and variable sampling time are taken into account.
% Description end

classdef Estimator_KF < TrackingEngineAbstract
    properties %all the properties are public
        AgentTrack;
        outlier_removal = false;
    end
    
    methods
        %%
        function [xhat] = do(self,model,d_ss,Ts)
            self.AgentTrack.outlier = 0;
            
            if isempty(self.AgentTrack) || ~isfield(self.AgentTrack,'x_est') || isnan(self.AgentTrack.x_est(1,end))
            % initialize the filter
                self.AgentTrack.a_prev = [];
                xhat = [d_ss 0]';
                P = self.initial_cov;
            else
                x_old = self.AgentTrack.x_est;
                P = self.AgentTrack.P;
                [xhat_plus,Q] = model.do(x_old,Ts,self.a_std);
                F = model.ProcessMatrix(Ts);
                Pplus = F*P*F'+Q;
                if isnan(sum(d_ss))
                    xhat = xhat_plus;
                    P = Pplus;
                else
                    [H,R] = model.ObservationModel(self.dss_std, self.alpha);
                    Y = H*xhat_plus;
                    Z = d_ss-Y;
                    S =  R+H*Pplus*H';
                    if abs(Z)>5*S
                        self.AgentTrack.outlier = 1;
                    end
                    K = Pplus*H'/S;
                    xhat= xhat_plus+K*Z;
                    P = (eye(size(Pplus))-K*H)*Pplus;
                end
            end
            self.AgentTrack.x_est = xhat;
            self.AgentTrack.P = (P+P')/2;
        end
    end
end