%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This class implements a Kalman filter for tracking using measured
%   range. The doppler and variable sampling time are taken into account.
% Description end

classdef Localization_KF < LocalizationEngineAbstract
    properties
        initial_cov    = diag([2,2,1,1]);
        Q              = 1*eye(4);
        min_dis2anchor = 0.05; 
        AgentTrack     = [];
    end
    
    methods
        function [xhat] = do(self, d_est, Ts, d_var)
            fix_anchors = self.anchors_pos;
            
            %             if isempty(self.AgentTrack) || ~isfield(self.AgentTrack,'x_est') || isnan(self.AgentTrack.x_est(1,end))
            %             % initialize the filter
            %                 self.AgentTrack.a_prev = [];
            %                 xhat = [0 0 0 0]';
            %                 P = self.initial_cov;
            %             else
            if isempty(self.AgentTrack) || ~isfield(self.AgentTrack,'x_est') || isnan(self.AgentTrack.x_est(1,end))
                % initialize the filter
%                 init = Localization_lateration();
%                 init.fix_anchors_pos = Reference.sensorpos;
%                 [loc_est(:,iEst)] = init.do(d_est, d_var,Ts,QI);%target.Likeliness(:,k));
% 
                x_old = [0, 0, 0, 0].';
                P = self.initial_cov;
            else
                x_old = self.AgentTrack.x_est;
                P = self.AgentTrack.P;
            end
            %                 [xhat_plus,Pplus] = self.predict(x_old,Ts,P);
            %% prediction
            F = [1 0 Ts 0; 0 1 0 Ts; 0 0 0.99 0; 0 0 0 0.99];
            xhat_plus = F*x_old;
            Pplus = F*P*F'+self.Q;
            %% update
%             c = find(QI<0.5);
%             if not(isempty(c))
%                 d(c) = [];
%                 QI(c) = [];
%                 fix_anchors(:,c) = [];
%             end
            if isempty(d_est) || isnan(sum(d_est))
                xhat = xhat_plus;
                P = Pplus;
            else
                Y = sqrt((xhat_plus(1)-fix_anchors(1,:)).^2+(xhat_plus(2)-fix_anchors(2,:)).^2)';
                Y(Y<self.min_dis2anchor) = self.min_dis2anchor;
                for kk = 1:length(d_est)
                    H(kk,:) = [(xhat_plus(1)-fix_anchors(1,kk))/Y(kk) ...
                        (xhat_plus(2)-fix_anchors(2,kk))/Y(kk) 0 0];
                end
                R = diag(d_var);%(self.d_var*diag(1./QI)); %eye(length(d));
                %                 [H,R] = model.ObservationModel(self.dss_std, self.alpha);
                Z = d_est-Y;
                S =  R+H*Pplus*H';
                %                 if Z^2>9*S
                %                     self.AgentTrack.outlier = 1;
                %                 end
                K = Pplus*H'/S;
                xhat= xhat_plus+K*Z;
                P = (eye(size(Pplus))-K*H)*Pplus;
            end
            self.AgentTrack.x_est = xhat(:);                                % Make sure it is a column-vector
            self.AgentTrack.P = (P+P')/2;
            xhat = xhat(:).';                                               % Make sure it is a row-vector
        end
        
        function [xhat_plus,Pplus] = predict(self,x_old,Ts,P)
            F = [1 0 Ts 0; 0 1 0 Ts; 0 0 1 0; 0 0 0 1];
            xhat_plus = F*x_old;
            Pplus = F*P*F'+self.Q;
        end

        function reset(self)
            self.AgentTrack = [];
        end
    end
end