%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This class implements shared function for the MGMP.
% Description end

classdef (Abstract) AbstractMGMP < Util
    properties (Abstract)
    end
    properties
        w_th_anch = 5*10^-4;                  % Threshold on GM component weights for the GM update using anchors
        STD_th_anch = 5*10^-4;                % Threshold on GM standard deviation for the GM update using anchors
        Flag_red_anch = 0;                    % Flag for allowing (1) or preventing (0) the reduction of GM to lower order at the intitialization using anchors
        Flag_inputGM_init = 1                 % Flag which selects whether input GM (1) or samples (0) are used as the input for GM initialization
        anch_comp_init = 2;                   % Initial no. components where samples (Flag_inputGM_init=0) are used for GM initialization
        KL_evol = [inf 0];                    % Online tracking the avg. of KL div. between two consecutive estimations for updating based on condition (18)...
                                              % First component is the avg.  of KL div. between two consecutive estimations...
                                              % till time "l-1" and second component is time instant "l-1"                                    
        beta = [0.05*ones(1,2) 0.04*ones(1,6)...
        0.03*ones(1,6) 0.02*ones(1,6) 0.01];  % Momentum coefficient        
        %beta=0.02;
    end
    methods
        
        
    end
        
end