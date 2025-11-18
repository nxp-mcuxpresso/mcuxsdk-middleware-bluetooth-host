%--------------------------------------------------------------------------
% Project   : NRB ranging
% Version   : GIT
% Author    : Payam Nazemzadeh (payam.nazemzdeh@imec.nl)
% Copyright : Stichting Imec Nederland (http://www.imec.nl)
% Disclaimer: Imec strictly confidential
%--------------------------------------------------------------------------
% Description
%   This class contains the process and observation models of one
%   dimensional range estimation
% Description end


classdef SystemModel_1D
    properties
        Nx = 2;         % number of states
        v0 = 2;
        likeliness = 1;
    end
    
    methods
        function [x_new, Q] = do(self,x,Ts,var_a)
            G = [1/2*Ts^2; Ts];
            Q = G*var_a*G'+[0.01 0; 0 .1];
            x_new = SystemModel_1D.ProcessMatrix(Ts,self.likeliness)*x;
        end
        
        function [xhat,P] = initialization(self,params,d_ss)
            xhat = [mean(d_ss) 0]';
            P = diag([params.dss_std^2, params.v_std^2]);
        end
    end
    methods (Static)
        function x_rel = translate_abs_to_rel_of_anchor(x, anchor_pos,d_old, Ts)            
            d_rel = sqrt(sum((x(1:2)-anchor_pos).^2));
            v_rel = (d_rel-d_old)/Ts;
            x_rel = [d_rel, v_rel]';
        end
        
        function F = ProcessMatrix(varargin)
            Ts = varargin{1};
            if nargin==2
                like = varargin{2};
            else
                like = 1;
            end
            if like<0.5
                F = [1 Ts; 0 0.9];
            else
                F = [1 Ts; 0 0.99];
            end
%             F = [0.99 Ts; 0 max(like,0.9)];
        end
        
        function [H,R] = ObservationModel(dss_std, alpha)
            H = [1 alpha];
            R = dss_std^2;
        end
                
    end
end