%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This class tests the EVD functionality.
% Description end

classdef testEVD < SeedRng & genChannelResponse
    properties
        Hfig1
    end
  
    properties (TestParameter)
        RangingEngineImplementation = struct('mathworks', RangingEngine_Music_emb('EVDobj', evd_complex_perfect()), ...
            'matlab_power_iter_dp', RangingEngine_Music_emb('EVDobj', evd_complex()), ...
            'matlab_power_iter_sp', RangingEngine_Music_emb('EVDobj', evd_complex_singleprecision()), ...
            'cpp_power_iter', RangingEngine_Music_cpp('evd_type', 0), ...
            'cpp_saes', RangingEngine_Music_cpp('evd_type', 1))
        CPPmethod = struct('power_iter', 0, 'seas', 1);
        Embmethod = struct('power_iter', evd_complex_singleprecision(), 'mathworks', evd_complex_perfect());
        evdMethod = struct('power_iter_dp', evd_complex(), 'power_iter_sp', evd_complex_singleprecision(), 'mathworks', evd_complex_perfect());
    end
    
    methods (TestClassSetup)                                                % These functions will be called once, at the start of the test-run
        function setup(self)
            self.applyFixture( setupEnv );
            import matlab.unittest.fixtures.SuppressedWarningsFixture
            
            self.generate_channel();
            self.Hfig1 = figure('name', 'testEVD_methods');
        end
    end   
    
    methods
        function self = testEVD(varargin)
            self@SeedRng(varargin{:});
            self@genChannelResponse(varargin{:});
        end
    end
    
    methods (Test, ParameterCombination='exhaustive', TestTags  = {'Quick', 'Unit'})  % These functions describe all tests that can be performed within this class.
        function testEVD_implementations(self, RangingEngineImplementation)
            % Test whether the methods work, and checks the speed and accuracy
            % The RangingEngine_Music_* is used to execute the EVD with
            % appropriate data.
            
            % Configure
            CR = ChannelReconstruct_square();
            % Because of the default settings of MethodSubspace and TOL,
            % the EVD_complex (power-iterations method) stops early, and
            % will therefore calculate <=MethodSubspace eigenvectors
            % -values.
            
            err = nan(self.NrTests,1);
            for k=1:self.NrTests
                CRout = CR.do(self.IQ_A_nc(k,:), self.IQ_B_nc(k,:));
                [~, ~] = RangingEngineImplementation.do(CRout);
                err(k) = RangingEngineImplementation.Report.evd_error;
            end
            
            % determine implementation
            FN = fieldnames(self.RangingEngineImplementation);
            indx = cellfun(@(x) x==RangingEngineImplementation, struct2cell(self.RangingEngineImplementation));
            EVDimpl = FN{indx};

            figure(self.Hfig1); hold on;
            plot(err,'.-', 'DisplayName', EVDimpl);
            Hleg = legend('show');
            set(Hleg, 'Interpreter', 'none');
            title('Accuracy of various methods', 'Interpreter', 'none');
            xlabel('Iteration');
            ylabel('Error');
            
            self.log(matlab.unittest.Verbosity.Concise, sprintf('%s: Median error = %.3e, max error = %.3e', EVDimpl, median(err), max(err)));
        end
    end
    
    methods (Test, ParameterCombination='sequential', TestTags  = {'Quick', 'Unit'})  % These functions describe all tests that can be performed within this class.
        function testEVD_emb_vs_cpp(self, CPPmethod, Embmethod)
           % This function compares the result of the Matlab and
           % C++-implementation of the EVD.
           
            % Configure
            CR = ChannelReconstruct_square();
            RE_ml = RangingEngine_Music_emb();
            RE_ml.FP = Embmethod;
            RE_ml.TOL(1) = 1e-7;
            RE_ml.TOL(2) = 1;
            RE_ml.MethodSubspaceSep = 10;                                   % calculate/return the 10 largest eigen-values

            RE_cpp = RangingEngine_Music_cpp();
            RE_cpp.evd_type = CPPmethod;
            RE_cpp.TOL(1) = 1e-7;
            RE_cpp.TOL(2) = 1;
            RE_cpp.MethodSubspaceSep = 10;                                  % calculate/return the 10 largest eigen-values

            maxerr = nan(self.NrTests,1);
            for k=1:self.NrTests
                CRout = CR.do(self.IQ_A_nc(k,:), self.IQ_B_nc(k,:));
                RE_ml.do(CRout);
                RE_cpp.do(CRout);
               
                self.verifyEqual(length(RE_ml.Report.EVs), length(RE_cpp.Report.EVs));
                self.verifyEqual(double(RE_ml.Report.EVs(:)), RE_cpp.Report.EVs(:), 'reltol', 0.01, 'abstol', 1e-6);
                
                % calculate length of each eigen-vector
                normEvect_ml = zeros(size(RE_cpp.Report.EVs));
                normEvect_cpp = zeros(size(RE_cpp.Report.EVs));
                
                ml_eigenvects = double(RE_ml.Report.eigenvects);
                cpp_eigenvects = RE_cpp.Report.eigenvects;
                for n=1:length(RE_ml.Report.EVs)
                    normEvect_ml(n) = norm(ml_eigenvects(:,n));
                    normEvect_cpp(n) = norm(cpp_eigenvects(:,n));
                end
                self.verifyEqual(normEvect_ml, ones(size(normEvect_ml)), 'abstol', 3e-6);
                self.verifyEqual(normEvect_cpp, ones(size(normEvect_ml)), 'abstol', 3e-6);
                
                maxerr(k) = max(abs(abs(ml_eigenvects(:))-abs(cpp_eigenvects(:))));
            end
            
            P90 = percentile(maxerr, 90);
            self.log(matlab.unittest.Verbosity.Detailed, sprintf('evd_type = %d: difference in eigenvects: max = %.8f, P90 = %.8f', CPPmethod, max(maxerr), P90));
            self.verifyLessThan(P90, 5e-4, sprintf('evd_type = %d: difference in eigenvects: max = %.8f, P90 = %.8f', CPPmethod, max(maxerr), P90));
            self.verifyLessThan(maxerr, 3.5e-2, sprintf('evd_type = %d: difference in eigenvects: max = %.8f, P90 = %.8f', CPPmethod, max(maxerr), P90));
        end
        
        function test_copy(self, evdMethod)
            % This function check whether the EVD-obj can be copied
            self.verifyWarningFree(@(x) evdMethod.copy());
        end
    end
end