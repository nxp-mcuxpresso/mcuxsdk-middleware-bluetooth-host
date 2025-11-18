%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This class tests the CFO-compensation functionality.
% Description end

classdef testCfoCompensation < matlab.unittest.TestCase & genChannelResponse
    properties
        Logger
    end
    
    properties (TestParameter)
        CFOCompAll = struct('CfoCompensatorConstant', CfoCompensatorConstant())
        CfoVal = num2cell(linspace(-50e3, 50e3, 10))
        npairs = struct('one', 1, 'two', 2, 'four', 4);
    end
    
    methods (TestClassSetup)                                                % These functions will be called once, at the start of the test-run
        function setup(self)
            self.applyFixture( setupEnv );
            import matlab.unittest.fixtures.SuppressedWarningsFixture
            self.generate_channel();
        end
    end   
    
    methods
        function self = testCfoCompensation(varargin)
            self.Logger = log4m.getLogger(sprintf('%s.log', mfilename));
            if strcmpi(self.Logger.fullpath, sprintf('%s.log', mfilename))
                self.Logger.setCommandWindowLevel(self.Logger.INFO);
                self.Logger.setLogLevel(self.Logger.INFO);
            end
            
            p = inputParser;
            p.addParameter('NrTests', 10, @(x) validateattributes(x, {'numeric'}, {'scalar'}) );
            p.addParameter('RicianK', 100, @(x) validateattributes(x, {'numeric'}, {'scalar'}) );
            p.addParameter('DistActual', 12.5, @(x) validateattributes(x, {'numeric'}, {'scalar'}) );
            
            p.parse(varargin{:});
            self.NrTests = p.Results.NrTests;
            self.RicianK = p.Results.RicianK;
            self.DistActual = p.Results.DistActual;
        end
        
        function [dist_est, aoa_est] = test_compensation(self, RE, IQ_A, IQ_B)
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
        function testCfoCompensation_distance_change(self, CFOCompAll, CfoVal)
            % This tests check whether the CFO-compensation is implemented
            % correctly, by checking that the distance is changed due to
            % the CFO-compensation.
            
            Tintra = 400e-6;
            c0 = physconst('lightspeed');
            xtal_offset = CfoCompensatorAbstract.cfo2xtaloffset(CfoVal);
            alpha_0 = -c0/2 * xtal_offset;
            dist_delta_ref = alpha_0 * Tintra;
            
            if isprop(CFOCompAll, 'CfoConstant')
                CFOCompAll.CfoConstant = CfoVal;
            else
                CFOCompAll.Cfo = CfoVal;
            end
            CFOCompAll.IntraDelay = Tintra;
            
            RE2 = RangeEstimator('CfoCompensator', CFOCompAll, 'RangingEngine', RangingEngine_Music_cpp());             % Range-estimator with CFO-compensation
            
            dist = zeros(self.NrTests, 1);
            for k=1:self.NrTests
                [dist(k), ~] = RE2.do(self.IQ_A_nc(k,:), self.IQ_B_nc(k,:));
            end
            
            dist_delta_meas = self.DistActual - mean(dist);
            self.verifyEqual(dist_delta_ref, dist_delta_meas, 'abstol', 0.05);
            end
        
        function testCfoCompensation_antenna_pairs(self, npairs)
            % This tests check whether the CFO-compensation is able to
            % handle data with 1 or more antenna-pairs.
            % We expect that the data from all antenna pairs is treated the
            % same.
            
            CfoComp = CfoCompensatorConstant('Cfo', 10e3, 'IntraDelay', 400e-6, 'IntraAntennaDelay', 0);
            iq_a = repmat(self.IQ_A_nc(1,:), npairs, 1);
            iq_b = repmat(self.IQ_B_nc(1,:), npairs, 1);
            [Z_A, Z_B] = CfoComp.do(iq_a, iq_b);
            
            self.verifyEqual(size(Z_A), size(iq_a));
            self.verifyEqual(size(Z_B), size(iq_b));
            if npairs>1
                self.verifyEqual(Z_A(2:end,:), repmat(Z_A(1,:), npairs-1, 1));
            end
            self.verifyEqual(flipud(Z_B), iq_b);
        end
        
        function testCfoCompensation_phase_change(self)
            % This tests checks whether there is a phase rotation per
            % antenna-pair and that it is according to expectation.
            
            cfo = -15000 + (15000 - -15000) * rand(1,1);                    % Generate a CFO-value between -15000 and 15000Hz
            CfoComp = CfoCompensatorConstant('Cfo', cfo, 'IntraDelay', 0);
            iq_a = repmat(self.IQ_A_nc(1,:), 4, 1);                        % Create 4 antenna-pairs of identical data
            iq_b = repmat(self.IQ_B_nc(1,:), 4, 1);                        % Create 4 antenna-pairs of identical data
            [Z_A, Z_B] = CfoComp.do(iq_a, iq_b);
            
            calc_phase_diff_A = mod(unwrap(angle(iq_a) - angle(Z_A), [], 2), 2*pi);
            calc_phase_diff_B = mod(unwrap(angle(iq_b) - angle(Z_B), [], 2), 2*pi);
            
            [nrows, ncols] = size(calc_phase_diff_A);
            expected_phase_diff_A = mod(-2*pi*CfoComp.CfoConstant * CfoComp.IntraAntennaDelay, 2*pi); % Expected phase rotation for each pair
            expected_phase_diff_B = mod( 2*pi*CfoComp.CfoConstant * CfoComp.IntraAntennaDelay, 2*pi); % Expected phase rotation for each pair
            self.verifyEqual(mod(diff(calc_phase_diff_A), 2*pi), expected_phase_diff_B * ones(nrows-1, ncols), 'abstol', 1e-14);
            self.verifyEqual(mod(diff(calc_phase_diff_B), 2*pi), expected_phase_diff_A * ones(nrows-1, ncols), 'abstol', 1e-14);
        end
    end
end