%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This is the base-class to test several range-estimators. Each range-estimator
%   contains a channel-reconstructor and a ranging-engine. This file also
%   compares the CPP implementation against the Matlab-implementation.
% Description end

classdef testRanging_AoA < matlab.unittest.TestCase & genChannelResponse
    properties (TestParameter)
        algo_type = {'left', 'right', 'left&right'}
        La = struct('one', 1, 'two', 2, 'four', 4);
    end
   
    methods 
        function self = testRanging_AoA(varargin)
            self@genChannelResponse(varargin{:}, 'NrTests', 4);             % Configure the generate_channel function for 4 antenna-pairs
        end
    end
    
    methods (Test, ParameterCombination='exhaustive', TestTags  = {'Quick', 'Unit'})  % These functions describe all tests that can be performed within this class.
        function testRangeEstimator_likeliness(self, algo_type, La)
            % This test compares the likiliness calculation which is done
            % in Music_AoA and Music_emb. The AoA-version uses an SVD and
            % the emb-version uses an EVD. Since the SVD is the regular
            % Matlab-implementation, the emb-version should also use the
            % regular Matlab-implementation of the EVD.
            import matlab.unittest.Verbosity
            self.log(Verbosity.Detailed, sprintf('Executing with %s, La=%d', algo_type, La))
            
            if La==2
                % When La=2, the Music_AoA should give a Likeliness-value
                % of NaN. This needs to be verified only once.
                nTests     = 1;
            else
                nTests     = 10;
            end
            Likeliness_AoA = NaN(nTests,1);
            ref            = NaN(nTests,1);
            RE_AoA         = RangingEngine_Music_AoA('type', algo_type, 'La', La);
            RE_emb         = RangingEngine_Music_emb();
            RE_AoA.type    = algo_type;
            RE_AoA.La      = La;
            
            max_error = 0;
            for k=1:nTests
                % The RangingEngine_Music_AoA uses an SVD, and the
                % RangingEngine_Music_emb uses an EVD. This test is to check
                % whether the likeliness function behaves the same, independent
                % of the used method to calculate the eigen-values/vectors.
                self.generate_channel();
                CRout.Hest = self.Hactual / max(abs(self.Hactual(:)));
                RE_AoA.do(CRout);
                RE_emb.do(CRout);

                Likeliness_AoA(k) = RE_AoA.Report.Likeliness;
                if La ~= 2          % If La==2, the calculated likeliness by Music-AoA should be NaN.
                    ref(k) = RE_emb.Report.Likeliness;
                end
                
                if abs(Likeliness_AoA(k) - ref(k)) > max_error
                    max_error = abs(Likeliness_AoA(k) - ref(k));
                end
                self.log(Verbosity.Detailed, sprintf('k = %2d: emb/AoA = %.5fe/%.5f => abs-error = %.5f => max_error = %.5f', k, ref(k), Likeliness_AoA(k), abs(ref(k)-Likeliness_AoA(k)), max_error));
            end
            
            % Discard the highest errors
            abs_error = abs(Likeliness_AoA - ref);
            P80 = percentile(abs_error, 80);
            
            % Check that P90-error is below abstol (or that both are NaN when La=2)
            if La == 2
                self.verifyEqual(P80, NaN)
            else
                if (La == 1 && strcmpi(algo_type, 'right')) || (La == 4 && strcmpi(algo_type, 'left'))
                    atol = 0.03;
                else
                    atol = 0.01;
                end
                nAboveAtol = sum(abs_error>=atol);
                self.verifyLessThan(P80, atol, sprintf('%d simulation were above %.2f; P80-error = %.5f and max-error = %.5f', nAboveAtol, atol, +P80, max_error));
            end
        end
    end
end