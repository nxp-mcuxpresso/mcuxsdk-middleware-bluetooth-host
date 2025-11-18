%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------

classdef testPhaseNoise < spectrumTestShared & matlab.unittest.TestCase
    properties (TestParameter)
        Alpha           = struct('zero', 0, 'one', 1, 'two', 2, 'two_half', 2.5);
        powerVal        = struct('minus_70', -70, 'minus_80', -80)
        deltaT          = struct('us_1', 1e-6, 'us_10', 1e-5, 'us_100', 1e-4, 'us_1000', 1e-3);
        upsample_factor = struct('one', 1, 'two', 2, 'four', 4);
        method          = {'closed_form', 'configuration'}
    end
    
    methods (Test, ParameterCombination='exhaustive', TestTags  = {'Quick', 'Unit', 'MCPD'})  % These functions describe all tests that can be performed within this class.
        function test_construction_without_input_args(self)
            % Class must be able to be constructed without input arguments
            % (https://nl.mathworks.com/help/matlab/matlab_oop/class-constructor-methods.html#btn2kiy)
            self.verifyWarningFree(@signal.PhaseNoise);
        end
    end
    
    methods (Test, ParameterCombination='exhaustive', TestTags  = {'Slow', 'Unit', 'MCPD'})  % These functions describe all tests that can be performed within this class.
        function SpectrumTest(self, Alpha, powerVal, deltaT, upsample_factor, method)
            % In this test the simulated noise-spectrum is compared against
            % a reference. When method equals:
            %  * 'closed_form', the reference is the filter response of 
            %     the noise-generator.
            %  * anything else, the reference is the configuration
            if strcmpi(method, 'closed_form')
                abstol        = 2;
                f_low_scaling = 2;
            else
                abstol        = 3.5;
                if Alpha==2             % For higher alpha's (mod 2), the filter gets a correct slope later
                    f_low_scaling = 8;
                else
                    f_low_scaling = 4;
                end
            end
            
            Cfg.N               = 10e6;
            Cfg.NFFT            = 2^16;
            Cfg.PLOT            = false;
            Cfg.deltaT          = deltaT;
            Cfg.upsample_factor = upsample_factor;
            Cfg.method          = method;
            Cfg.Alpha           = Alpha;
            Cfg.powerVal        = powerVal;
            Cfg.abstol          = abstol;
            Cfg.f_low_scaling   = f_low_scaling;
            Cfg.noiseObj        = @signal.PhaseNoise;
            Cfg.impedance       = randi([15, 100], 1, 1);
            
            self.spectrumtest_shared(Cfg);
        end
    end
end
