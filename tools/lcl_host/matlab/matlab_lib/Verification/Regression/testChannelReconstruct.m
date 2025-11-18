%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This file tests the accuracy of several channel-reconstructors.
% Description end

classdef testChannelReconstruct < matlab.unittest.TestCase & genChannelResponse
    properties
        Logger
    end
    
    properties (TestParameter, Abstract)
        CRall_1D
        CRall_2D        
        nPairs
        nPairs2D
    end
    
    properties (TestParameter)
        delta_f = struct('half_MHZ', 0.5e6, 'one_MHZ', 1e6, ...
                         'two_MHz', 2e6, 'four_MHz', 4e6)
    end
    
    methods (TestClassSetup)                                                % These functions will be called once, at the start of the test-run
        function setup(self)
            self.applyFixture( setupEnv );
            
            % this part is to check whether the unexplained occasional
            % error "Undefined function 'predict' for input arguments of
            % type 'double'." is because of a license issue
            [status, errmsg] = license('checkout', 'Statistics_Toolbox');
            if ~status
                self.Logger.warning(errmsg)
            end            
        end
    end    
    
    methods
        function self = testChannelReconstruct(varargin)
            self@genChannelResponse(varargin{:});
            self.Logger = log4m.getLogger(sprintf('%s.log', mfilename));
            if strcmpi(self.Logger.fullpath, sprintf('%s.log', mfilename))
                self.Logger.setCommandWindowLevel(self.Logger.INFO);
                self.Logger.setLogLevel(self.Logger.INFO);
            end
            self.generate_channel_2D();
        end
    end
    
    methods (Test, ParameterCombination='exhaustive', TestTags  = {'Quick', 'Unit'})  % These functions describe all tests that can be performed within this class.
        function testCRPassThrough(self)
            IQ_A = complex(rand(80,1), rand(80,1));
            CR = ChannelReconstruct_Passthrough();
            % Test function with 1 input argument
            CRout = CR.do(IQ_A);
            self.verifyTrue(all(isnan(CRout.Uncertainty)));
            self.verifyTrue(all(isnan(CRout.ProbCorrectSign)));
            self.verifyEqual(CRout.Hest, IQ_A);
            
            % Test function with 2 input arguments
            CRout = CR.do(IQ_A, IQ_A);
            self.verifyTrue(all(isnan(CRout.Uncertainty)));
            self.verifyTrue(all(isnan(CRout.ProbCorrectSign)));
            self.verifyEqual(CRout.Hest, IQ_A);
            
            % Test function with 2 input arguments - second argument is wrong
            self.verifyError(@(x) CR.do(IQ_A, conj(IQ_A)), '');
        end
        
        function testCPPimplementation_NonPhaseCoherent(self, delta_f, nPairs)
            CR     = ChannelReconstruct_NonPhaseCoherent();
            CR_cpp = ChannelReconstruct_NonPhaseCoherent_cpp();
            
            self.deltaF = delta_f;
            self.nTones = 1e6 / delta_f * 80;
            self.generate_channel_2D();
            
            for k=1:nPairs:self.NrTests
                range = k+(0:nPairs-1);
                CRout1 = CR.do(self.IQ_A_nc(range,:), self.IQ_B_nc(range,:));
                CRout2 = CR_cpp.do(self.IQ_A_nc(range,:), self.IQ_B_nc(range,:));
               
                % Check that Matlab and C++ variant behave similar
                self.verifyLessThanOrEqual(norm(CRout1.Hest - CRout2.Hest), 1e-5);
                self.verifyLessThanOrEqual(norm(CRout1.Uncertainty - CRout2.Uncertainty), 1e-10);
            end
        end
        
        function testCPPimplementation_PhaseCoherent(self, delta_f)
            CR     = ChannelReconstruct_PhaseCoherent();
            CR_cpp = ChannelReconstruct_PhaseCoherent_cpp();
            
            self.deltaF = delta_f;
            self.nTones = 1e6 / delta_f * 80;
            self.generate_channel_2D();
            
            for k=1:self.NrTests
                CRout1 = CR.do(self.IQ_A_c(k,:), self.IQ_B_c(k,:));
                CRout2 = CR_cpp.do(self.IQ_A_c(k,:), self.IQ_B_c(k,:));
               
                % Check that Matlab and C++ variant behave similar
                self.verifyLessThanOrEqual(norm(CRout1.Hest - CRout2.Hest), 9e-5);
            end
        end
        
        function testChannelCorrectness_1D(self, CRall_1D, nPairs)
            Corr = zeros(self.NrTests, nPairs);
            for k=1:self.NrTests
                Hact = self.Channel_2D(k).Hactual(1:nPairs,:);
                CRout = CRall_1D.do(self.Channel_2D(k).IQ_A_c(1:nPairs,:), self.Channel_2D(k).IQ_B_c(1:nPairs,:));
                
                for ipair = 1:nPairs
                    if CRall_1D.ScalingFactor == 0.5
                        Corr(k,ipair) = self.calc_channel_correlation(Hact(ipair,:).^2, CRout.Hest(ipair,:));
                    else
                        Corr(k,ipair) = self.calc_channel_correlation(Hact(ipair,:), CRout.Hest(ipair,:));
                    end
                end
            end
%             SUCCESS_RATE  = mean(abs((Corr(:)))>0.99);
            Avg = mean(abs(Corr(:)));
            Std = std(abs(Corr(:)));
            self.Logger.info('nPair = %d,%s: Correlation = %.2f%% ± %.2f%%', nPairs, class(CRall_1D), Avg*100, Std*100)
            
            self.verifyFalse(isnan(Avg));
        end
        
        function testChannelCorrectness_2D(self, CRall_2D, nPairs2D)
            Corr = zeros(self.NrTests, 1);
            for k=1:self.NrTests
                Hact = self.Channel_2D(k).Hactual(1:nPairs2D,:);
                CRout = CRall_2D.do(self.Channel_2D(k).IQ_A_c(1:nPairs2D,:), self.Channel_2D(k).IQ_B_c(1:nPairs2D,:));
                
                if CRall_2D.ScalingFactor == 0.5
                    Corr(k) = self.calc_channel_correlation(Hact(:).'.^2, CRout.Hest(:).');
                else
                    Corr(k) = self.calc_channel_correlation(Hact(:).', CRout.Hest(:).');
                end
            end
%             SUCCESS_RATE  = mean(abs((Corr(:)))>0.99);
            Avg = mean(abs(Corr(:)));
            Std = std(abs(Corr(:)));
            self.Logger.info('nPair = %d, %s: Correlation = %.2f%% ± %.2f%%', nPairs2D, class(CRall_2D), Avg*100, Std*100)
            
            self.verifyFalse(isnan(Avg));
        end
    end
    
    methods (Static)
        function Ccorr = calc_channel_correlation(Hact, Hest)
            Ccorr = abs( Hact*Hest'/sqrt( (Hact*Hact')*(Hest*Hest') ) );
        end
    end
end