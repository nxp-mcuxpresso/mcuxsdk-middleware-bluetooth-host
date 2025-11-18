%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This file tests several of range-estimators. Each range-estimator
%   contains a channel-reconstructor and a ranging-engine. This file also
%   compares the CPP implementation against the Matlab-implementation.
% Description end

classdef testRanging_toNXP < testRanging
    properties (TestParameter)
        REall   = struct('Music_emb', RangingEngine_Music_emb, 'Music_cpp', RangingEngine_Music_cpp, 'PhaseDiff', RangingEngine_phasediff, 'Music_AoA', RangingEngine_Music_AoA);
        CRall   = struct('Square', ChannelReconstruct_square, 'NonPhaseCoherent', ChannelReconstruct_NonPhaseCoherent, 'NonPhaseCoherent_cpp', ChannelReconstruct_NonPhaseCoherent_cpp);
        CRset1  = struct('Square', ChannelReconstruct_square, 'NonPhaseCoherent', ChannelReconstruct_NonPhaseCoherent, 'NonPhaseCoherent_cpp', ChannelReconstruct_NonPhaseCoherent_cpp);
        REset1  = struct('Music_emb', RangingEngine_Music_emb, 'Music_cpp', RangingEngine_Music_cpp, 'PhaseDiff', RangingEngine_phasediff, 'Music_AoA', RangingEngine_Music_AoA);
        delta_f = struct('half_MHz', 0.5e6, 'one_MHz', 1e6, 'two_MHz', 2e6, 'four_MHz', 4e6);
    end
    
    methods (Test, ParameterCombination='exhaustive', TestTags  = {'Quick', 'Unit'})
        function testRangeEstimator_PMdata(self)
            % nothing implemented, because no CR is delivered which accepts
            % phase-magnitude data
        end
    end
end