%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This file tests the accuracy of several channel-reconstructors.
% Description end

classdef testChannelReconstruct_toNXP < testChannelReconstruct
    properties (TestParameter)
        CRall_1D = struct('Square', ChannelReconstruct_square, ...
                          'NonPhaseCoherent', ChannelReconstruct_NonPhaseCoherent, ...
                          'NonPhaseCoherent_2D', ChannelReconstruct_2D)
        CRall_2D = struct('NonPhaseCoherent_2D', ChannelReconstruct_2D)
        nPairs = {1,4}
        nPairs2D = {2,4}
    end
    
    methods (Test, ParameterCombination='exhaustive', TestTags  = {'Quick', 'Unit'})  % These functions describe all tests that can be performed within this class.
        function testCPPimplementation_PhaseCoherent(~)
            % No functionality is implemented, because the PhaseCoherent CR is not
            % delivered
        end
        function testCRPassThrough(~)
           % No functionality is implemented, because the CR_Passthrough is not delivered. 
        end
    end
    
    methods
        function self = testChannelReconstruct_toNXP()
            self@testChannelReconstruct('NrTests', 500, 'nAntPairs', 4);
        end
    end
end