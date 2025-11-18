%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This class tests the Rician-K quality indicator.
% Description end

classdef testRicianK < SeedRng & genChannelResponse
    properties (TestParameter)
        QI_RicianK = struct('QualityIndicator_RicianK', QualityIndicator_RicianK())
        riciank = {0.2, 0.5, 0.8, 1, 1.2, 3, 5, 8};
    end
    
    methods (TestClassSetup)                                                % These functions will be called once, at the start of the test-run
        function setup(self)
            set(0,'DefaultAxesYgrid','on')          % Enable grid
            set(0,'DefaultAxesXgrid','on')          % Enable grid
            FolderPath = fileparts(mfilename('fullpath'));
            
            import matlab.unittest.fixtures.CurrentFolderFixture
            self.applyFixture( CurrentFolderFixture(FolderPath) );
        end
    end
    
    methods
        function self = testRicianK(varargin)
            self@SeedRng(varargin{:});
            self@genChannelResponse(varargin{:});
            
            p = inputParser;
            p.addParameter('RicianK', 100, @(x) validateattributes(x, {'numeric'}, {'scalar'}) );   % Different default as in genChannelResponse
            p.parse(varargin{:});
            self.RicianK = p.Results.RicianK;

            if strcmpi(self.Logger.fullpath, fullfile('Logging', [mfilename, '.log']))      % if the logger is created by this function
                self.Logger.setCommandWindowLevel(self.Logger.INFO);
                self.Logger.(self.Logger.INFO);
            end
        end
    end
    
    methods (Test, ParameterCombination='exhaustive', TestTags  = {'Quick', 'Unit'})  % These functions describe all tests that can be performed within this class.
        function testRicianK_quality(self, QI_RicianK, riciank)
            Kest = NaN(self.NrTests,1);
            
            self.RicianK = riciank;
            Out = self.generate_channel();
            
            IQ_A   = Out.IQ_A_nc;
            IQ_B   = Out.IQ_B_nc;
            
            RE = RangeEstimator('ChannelReconstructor', ChannelReconstruct_NonPhaseCoherent_cpp(), 'RangingEngine', RangingEngine_Music_cpp());

            for k=1:self.NrTests
                [DistEst, CRout] = RE.do(IQ_A(k,:), IQ_B(k,:));
                Kest(k) = QI_RicianK.do(CRout.Hest, DistEst);
            end
            self.Logger.info('Rician-K (actual/estimated) = %.1f/%.1f', self.RicianK, mean(Kest));
        end            
    end
end