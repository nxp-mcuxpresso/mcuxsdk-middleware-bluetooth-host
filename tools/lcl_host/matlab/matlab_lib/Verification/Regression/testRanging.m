%--------------------------------------------------------------------------
% Project   : NRB ranging
% Version   : GIT
% Author    : Pepijn Boer (pepijn.boer@imec.nl)
% Copyright : Stichting Imec Nederland (http://www.imec-nl.nl)
% Disclaimer: Imec strictly confidential
%--------------------------------------------------------------------------
% Description
%   This is the base-class to test several range-estimators. Each range-estimator
%   contains a channel-reconstructor and a ranging-engine. This file also
%   compares the CPP implementation against the Matlab-implementation.
% Description end
%--------------------------------------------------------------------------

classdef testRanging < matlab.unittest.TestCase & genChannelResponse
    properties
        Logger
        Hfig_dist_est
        Hfig_long_dist
        PLOT       = false
        CRCoherent = {'ChannelReconstruct_PhaseCoherent'};
    end
    
    properties (TestParameter, Abstract)
        REall
        CRall
        CRset1
        REset1
        delta_f
    end
    
    methods (TestClassSetup)                                                % These functions will be called once, at the start of the test-run
        function setup(self)
            self.applyFixture( setupEnv );
            import matlab.unittest.fixtures.SuppressedWarningsFixture
            self.applyFixture( SuppressedWarningsFixture({'signal:findpeaks:largeMinPeakHeight'}) );
            
            if self.PLOT
                self.Hfig_dist_est = figure; 
                subplot(2,2,1); hold on;
                subplot(2,2,2); hold on;
                subplot(2,2,3); hold on;
                subplot(2,2,4); hold on;

                self.Hfig_long_dist = figure;
                subplot(2,2,1); hold on;
                subplot(2,2,2); hold on;
                subplot(2,2,3); hold on;
                subplot(2,2,4); hold on;

                self.addTeardown(@self.save_figure);
            end
        end
    end   
    
    methods
        function self = testRanging(varargin)
            self@genChannelResponse(varargin{:});

            self.Logger = log4m.getLogger(sprintf('%s.log', mfilename));
            if strcmpi(self.Logger.fullpath, sprintf('%s.log', mfilename))
                self.Logger.setCommandWindowLevel(self.Logger.INFO);
                self.Logger.setLogLevel(self.Logger.INFO);
            end
            self.generate_channel();
        end
        
        function save_figure(self)
            figure(self.Hfig_dist_est);
            for k=1:length(self.CRall)
                subplot(2,2,k);
                xlabel('Simulation number');
                ylabel('Distance [m]');
                Hleg = legend('show');
                Hleg.Interpreter = 'none';
            end
%             savefig(gcf, fullfile('Results', 'RangeEstimator_combinationtest.png'));
%            
%             figure(self.Hfig_long_dist);
%             savefig(gcf, fullfile('Results', 'Estimated_vs_actual_distance_ambiguity.png'));
        end
        
        function [dist_est, aoa_est] = test_ranging(self, RE, IQ_A, IQ_B)
            dist_est = zeros(self.NrTests, 1);
            aoa_est  = zeros(self.NrTests, 1);
            for k=1:self.NrTests
                [~, CRout, ~] = RE.do(IQ_A(k,:), IQ_B(k,:));
                dist_est(k) = CRout.Distance;
                aoa_est(k)  = CRout.AoA;
            end
        end
    end 
    
    methods (Test, ParameterCombination='exhaustive', TestTags  = {'Quick', 'Unit'})  % These functions describe all tests that can be performed within this class.
        function test_construction_without_input_args(self)
            % Class must be able to be constructed without input arguments
            % (https://nl.mathworks.com/help/matlab/matlab_oop/class-constructor-methods.html#btn2kiy)
            self.verifyWarningFree(@RangeEstimator);
        end
        
        function testRangingEngine_copy(self, REall)
            % This function checks whether the objects are copyable
            self.verifyWarningFree(@(x) REall.copy());
        end        
        
        function testRangeEstimator_combinations(self, CRall, REall)
            RE = RangeEstimator('ChannelReconstructor', CRall, 'RangingEngine', REall);
            if any(strcmp(class(CRall), self.CRCoherent))
                IQ_A = self.IQ_A_c;
                IQ_B = self.IQ_B_c;
            else
                IQ_A = self.IQ_A_nc;
                IQ_B = self.IQ_B_nc;
            end
            [dist_est, ~] = self.test_ranging(RE, IQ_A, IQ_B);
            
            self.Logger.info('%s, %s error [m]: %.2f ± %.2f', class(CRall), class(REall), mean(dist_est - self.DistActual), std(dist_est-self.DistActual));
            
            DispName = sprintf('%s - %s', class(CRall), class(REall));
            
            % find index of current CR in self.CRall
            temp = struct2cell(self.CRall);
            for k=1:length(temp)
                if isa(CRall, class(temp{k}))
                    break
                end
            end
            
            if self.PLOT
                figure(self.Hfig_dist_est);
                subplot(2,2,k);
                plot(1:self.NrTests, dist_est, '.-', 'DisplayName', DispName);
                drawnow
                FN = fieldnames(self.CRall);
                temp = strrep(FN{k}, '_', '-');
                title(sprintf('Channel reconstruct - %s', temp));
            end
        end            
        
        function testCPPimplementation_Music(self)
            RE     = RangingEngine_Music_emb();
            RE_cpp = RangingEngine_Music_cpp();
            CR     = ChannelReconstruct_NonPhaseCoherent_cpp();
            Dest1  = zeros(self.NrTests, 1);
            Dest2  = zeros(self.NrTests, 1);
            Likeliness1 = zeros(self.NrTests, 1);
            Likeliness2 = zeros(self.NrTests, 1);
            for k=1:self.NrTests
                CRout = CR.do(self.IQ_A_nc(k,:), self.IQ_B_nc(k,:));

                [Dest1(k), AoA1] = RE.do(CRout);
                [Dest2(k), AoA2] = RE_cpp.do(CRout);
                
                Likeliness1(k) = RE.Report.Likeliness;
                Likeliness2(k) = RE_cpp.Report.Likeliness;
                
                % Check that Matlab and C++ variant behave similar
                self.verifyEqual(AoA1, AoA2)
            end
            
            P = percentile(abs(Dest1 - Dest2), 90);                         % Calculate 90% percentile
            self.verifyEqual(P, 0, 'abstol', 0.05)
            
            P = percentile(abs(Likeliness1 - Likeliness2), 90);             % Calculate 90% percentile
            self.verifyEqual(P, 0, 'abstol', 5e-3)
        end  
        
        function testRangeEstimator_IQdata(self, CRall)
            % The purpose of this function is to test the RangeEstimator
            % function. Can it properly handle multi-dimensional arrays?
            RE  = RangeEstimator('ChannelReconstructor', CRall, 'RangingEngine', RangingEngine_Music_cpp);
            out = generate_channel(1, 5, Inf);
            
            [Dref, ~, Href] = RE.do(out.IQ_A_c, out.IQ_B_c);
            
            % Apply three rows or zeros and one with actual IQ data. Expect
            % the same output as when applying only the single
            if contains(class(CRall), 'cpp')
                % The mex-function requires complex values, therefore the
                % appended values are not 0.
                IQ_A = [out.IQ_A_c; complex(eps(1), eps(1)) * ones(3, 80)];     
                IQ_B = [out.IQ_B_c; complex(eps(1), eps(1)) * ones(3, 80)];
            else
                IQ_A = [out.IQ_A_c; zeros(3, 80)];
                IQ_B = [out.IQ_B_c; zeros(3, 80)];
            end
            for k=0:3
                A = circshift(IQ_A, k, 1);
                B = circshift(IQ_B, k, 1);
                
                [D, ~, H] = RE.do(A, B);
                ZeroRows = all(abs(H)<1e-15, 2);
                self.verifyEqual(sum(ZeroRows), 3);
                self.verifyEqual(Dref, D, 'abstol', 1e-10);
                self.verifyEqual(Href, H(~ZeroRows, :), 'abstol', 1e-10);
            end
            
            % Apply four equal rows
            IQ_A = repmat(out.IQ_A_c, 4, 1);
            IQ_B = repmat(out.IQ_B_c, 4, 1);
            [D, ~, H] = RE.do(IQ_A, IQ_B);
            self.verifyEqual(Dref, D, 'abstol', 1e-10);
            self.verifyEqual(repmat(Href, 4, 1), H, 'abstol', 1e-10);
        end
        
        function testRangeEstimator_distance(self, CRset1, REset1, delta_f)
            if isa(REset1, 'RangingEngine_Music_AoA')
                self.assumeFail('The RangingEngine_Music_AoA shows strange behavior for distance larger than the ambiquity bound');
            end
            c = physconst('lightspeed');                                    % (m/s) Speed of light in vacuum
            AmbDist = c / (2*delta_f);                                  % (m) Ambiguity distance
            ActDist = linspace(0.5, 1.25*AmbDist, 20);                      % (m) Actual distance
            RE = RangeEstimator('ChannelReconstructor', CRset1, 'RangingEngine', REset1);
            RE.RangingEngine.delta_F = delta_f;
            if isprop(RE.RangingEngine, 'MaxDist')
                RE.RangingEngine.MaxDist = max(AmbDist);
            end
            if isprop(RE.RangingEngine, 'maxiter') 
                RE.RangingEngine.maxiter = 1000;
            end
            
            EstDist = NaN(size(ActDist));
            Hest = NaN(2, 80);
            for k = 1:length(ActDist)
               Out = generate_channel(1, ActDist(k), Inf, 'delta_F', delta_f, 'SignalPowerRange', [-10, 10], 'SNR', 100);     % Generate signals with high SNR, with varying signal levels
               
               % Always use the phase-coherent data, all CR's can deal with this.
               [EstDist(k), CRout] = RE.do(Out.IQ_A_c, Out.IQ_B_c);
               Hest(k,:) = CRout.Hest;
            end
            
            if self.PLOT
                subplotIndx = log2(delta_f / 1e6) + 2;

                figure(self.Hfig_long_dist);
                subplot(2,2,subplotIndx); 
                hold on; grid on;
                DistName = sprintf('%s - %s', strrep(class(CRset1), 'ChannelReconstruct', 'CR'), strrep(class(REset1), 'RangingEngine', 'RE'));
                plot(ActDist, EstDist, 'DisplayName', DistName)
                xlabel('Actual distance (m)');
                ylabel('Estimated distance (m)');
                title(sprintf('\\Deltaf=%.1fMHz', delta_f/1e6));
                if any(subplotIndx==[2,4])
                    legend('show', 'interpreter', 'none', 'location', 'eastoutside')
                end
            end
            self.verifyEqual(EstDist, mod(ActDist, AmbDist), 'reltol', 0.1, 'abstol', 0.5);
        end
    end
end