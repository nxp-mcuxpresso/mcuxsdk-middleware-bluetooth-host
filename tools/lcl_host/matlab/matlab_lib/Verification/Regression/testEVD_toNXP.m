%--------------------------------------------------------------------------
% Project   : NRB ranging
% Version   : GIT
% Author    : Pepijn Boer (pepijn.boer@imec.nl)
% Copyright : Stichting Imec Nederland (http://www.imec-nl.nl)
% Disclaimer: Imec strictly confidential
%--------------------------------------------------------------------------
% Description
%   This class tests the EVD functionality.
% Description end
%--------------------------------------------------------------------------

classdef testEVD_toNXP < testEVD
    properties (TestParameter)
        EVDmethod = struct('power_iter', 0);
        RangingEngineImplementation = struct('matlab', RangingEngine_Music_emb('EVDobj', evd_complex_perfect()), ...
                                'matlab_power_iter', RangingEngine_Music_emb('EVDobj', evd_complex()), ...
                                'cpp_power_iter', RangingEngine_Music_cpp('evd_type', 0));
        CPPmethod = struct('power_iter', 0);
        Embmethod = struct('power_iter',evd_complex());
    end
end