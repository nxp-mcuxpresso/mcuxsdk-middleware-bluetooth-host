%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------

classdef testAdditiveNoise < spectrumTestShared & matlab.unittest.TestCase
    properties (TestParameter)
        Alpha           = struct('zero', 0, 'one', 1, 'two', 2, 'two_half', 2.5);
        dBm             = struct('plus_20', 20, 'minus_60', -60);
        DeltaT          = struct('us_1', 1e-6, 'us_10', 1e-5, 'us_100', 1e-4, 'us_1000', 1e-3);
        impedance       = {50, 83.3, 100*pi}
        upsample_factor = struct('one', 1, 'two', 2, 'four', 4);
        method          = {'closed_form', 'configuration'}
    end
    
    methods (Test, ParameterCombination='exhaustive', TestTags  = {'Slow', 'Unit', 'MCPD'})  % These functions describe all tests that can be performed within this class.
        function PowerTest(self, dBm, DeltaT, impedance, upsample_factor)
            import matlab.unittest.Verbosity
            self.log(Verbosity.Detailed, sprintf('alpha = %.1f, dBm = %.1fdBc/Hz, f_ref = %.1fHz delta_t = %.2f', 0, dBm, DeltaT))

            fc           = 32e6;
            N            = 4e6;
            fs           = 1/DeltaT;
            f_low        = 1e-4 * fs;
            powerRefFreq = 15e-4 * fs;                                        % There must be enough room between f_low and the powerRefFreq, because the filter response needs some room the achieve the required slope. Most critical point: alpha=2
            f_high       = 0.2 * fs / upsample_factor;
            
            an = signal.AdditiveNoise('PowerValue', dBm, ...
                                      'noiseColor', 'white', ...
                                      'nSections', 5, ...,
                                      'impedance', impedance, ...
                                      'f_low', f_low, ...
                                      'f_high', f_high, ...
                                      'fs', fs, ...
                                      'powerRefFreq', powerRefFreq, ...
                                      'upsample_factor', upsample_factor);
           
            x = Signal_BB(zeros(1,N), fc, DeltaT, impedance);               % Generate a baseband-equivalent signal at frequency fc
            y = an.do(x);                                                   % Apply phase-noise to signal x

            [freq, PSD] = pwelch(y);
            
            % Measure the noise density in the band-of-interest (BOI) (f_low .. f_high]
            I = inrange(freq, [f_low, f_high], 'includeboth');
            
            PLOT = false;
            if PLOT
                figure; hold on; %#ok<UNRCH>
                plot(freq, 10*log10(PSD+eps(1)));
                plot(freq(I), 10*log10(PSD(I)+eps(1)));
                xlabel('Frequency [kHz]');
                ylabel('PSD [dBm/Hz]');
                title('PSD of generated signal');
            end
            PSD_avg = 10*log10(mean(PSD(I))+eps(1));                        % mean PSD in band-of-interest
            self.verifyEqual(PSD_avg,  dBm, 'abstol', 1)
        end

        function SpectrumTest(self, Alpha, dBm, DeltaT, upsample_factor, method)
            Cfg.N               = 10e6;
            Cfg.NFFT            = 2^16;
            Cfg.PLOT            = true;
            Cfg.deltaT          = DeltaT;
            Cfg.upsample_factor = upsample_factor;
            Cfg.method          = method;
            Cfg.Alpha           = Alpha;
            Cfg.powerVal        = dBm;
            if strcmpi(method, 'closed_form')
                Cfg.abstol      = 2;
            else
                Cfg.abstol      = 2.5;
            end
            Cfg.f_low_scaling   = 15;
            Cfg.noiseObj        = @signal.AdditiveNoise;
            Cfg.impedance       = randi([15, 100], 1, 1);
            
            self.spectrumtest_shared(Cfg);
        end
    end
    
    methods (Test, ParameterCombination='exhaustive', TestTags  = {'Quick', 'Unit', 'MCPD'})  % These functions describe all tests that can be performed within this class.
        function test_construction_without_input_args(self)
            % Class must be able to be constructed without input arguments
            % (https://nl.mathworks.com/help/matlab/matlab_oop/class-constructor-methods.html#btn2kiy)
            self.verifyWarningFree(@signal.AdditiveNoise);
        end
       
        function ParamMismatchTest(self)
            deltaT = exp(1)*1e-6;
            imp    = 100*pi;
            fc     = 32e6;
            N      = 4e6;
            
            an = signal.AdditiveNoise('PowerValue', 10, ...
                                      'noiseColor', 'custom', ...
                                      'alpha', -1.5, ...
                                      'nSections', 5, ...,
                                      'impedance', imp, ...
                                      'fs', 1/deltaT);
            
            x = Signal_BB(zeros(1,N), fc, deltaT/2, imp);                   % Generate a baseband-equivalent signal at frequency fc
            self.verifyError(@() an.do(x), '');                             % Check that assertion is triggered in do-function
            
            x = Signal_BB(zeros(1,N), fc, deltaT, imp*10);                  % Generate a baseband-equivalent signal at frequency fc
            self.verifyError(@() an.do(x), '');                             % Check that assertion is triggered in do-function
        end
    end
end
