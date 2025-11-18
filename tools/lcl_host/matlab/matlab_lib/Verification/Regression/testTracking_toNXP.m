%--------------------------------------------------------------------------
% Project   : NRB ranging
% Version   : GIT
% Author    : Payam Nazemzadeh (payam.nazemzdeh@imec.nl)
% Copyright : Stichting Imec Nederland (http://www.imec.nl)
% Disclaimer: Imec strictly confidential
%--------------------------------------------------------------------------
% Description
%   This file tests the accuracy of several tracking algorithms.
% Description end

classdef testTracking_toNXP < testTracking
    properties (TestParameter)
        Model     = struct('Model_1D', SystemModel_1D);
        Estimator = struct('KF', Estimator_KF, 'AKF', Estimator_AKF)
        Ts = struct('a', 0.1, 'b', 0.3, 'c', 1)
    end
end