%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------

classdef testColoredNoise < matlab.unittest.TestCase
    properties (TestParameter)
        noiseColorName = struct('white', 'white', 'pink', 'pink', 'blue', 'blue', 'brown', 'brown', 'purple', 'purple');
        method = {'RC', 'kasdin'};
        Alpha = num2cell((rand(1,5))*2);                                    % Alpha between 0 and 2
        AllAlpha = num2cell((rand(1,10)-0.5)*20);                           % Alpha in interval -10 ... 10
        complexNoise = struct('false', false, 'true', true);
        upsample_factor = struct('one', 1, 'two', 2, 'five', 5);
    end
   
    methods (Test, ParameterCombination='exhaustive', TestTags  = {'Quick', 'Unit'})  % These functions describe all tests that can be performed within this class.
        function test_construction_without_input_args(self)
            % Class must be able to be constructed without input arguments
            % (https://nl.mathworks.com/help/matlab/matlab_oop/class-constructor-methods.html#btn2kiy)
            self.verifyWarningFree(@signal.ColoredNoise);
        end
        
        function ColorName(self, noiseColorName)
            L = 10e3;
            Impedance = randi([10,100],1,1);
            sprev = rng('shuffle');
            
            dsp_cn = dsp.ColoredNoise('Color', noiseColorName, 'SamplesPerFrame', L);
            my_cn  = signal.ColoredNoise('method', 'kasdin', 'noiseColor', noiseColorName, 'Impedance', Impedance, 'ComplexNoise', false);
            
            % Generate noise with built-in Matlab-function
            rng(sprev);
            x = dsp_cn();
            
            % Generate noise with Imec-function
            rng(sprev)
            y = my_cn.do(L);
            
            % the values of x and y are different, however a their ratio 
            % should be constant. This is verified here.
            scaling_factors = x(:) ./ double(y(:));  
            scaling_factor_pp  = max(scaling_factors) - min(scaling_factors);   % peak-peak value
            
            import matlab.unittest.Verbosity
            self.log(Verbosity.Detailed, sprintf('%s: %.5e', noiseColorName, scaling_factor_pp))
            
            % These abstol are chosen such that the test does not fail
            % when it is executed 1000 times after setting rng('default').
            if ismember(lower(noiseColorName), {'white', 'blue', 'purple'})
                abstol = 1.7e-13;
            elseif strcmpi(noiseColorName, 'brown')
                abstol = 3.3e-7;
            else % Pink
                abstol = 1e-10;
            end
            self.verifyLessThan(scaling_factor_pp, abstol);
        end
        
        function Kasdin_alpha_0_2(self, Alpha)
            L = 10e3;
            Impedance = randi([10,100],1,1);
            sprev = rng('shuffle');
            
            dsp_cn = dsp.ColoredNoise('Color', 'custom', 'InverseFrequencyPower', Alpha, 'SamplesPerFrame', L);
            my_cn  = signal.ColoredNoise('method', 'kasdin', 'noiseColor', 'custom', 'alpha', Alpha, 'Impedance', Impedance, 'ComplexNoise', false);
            
            % Generate noise with built-in Matlab-function
            rng(sprev);
            x = dsp_cn();
            
            % Generate noise with Imec-function
            rng(sprev)
            y = my_cn.do(L);
            
            % the values of x and y are different, however a their ratio 
            % should be constant. This is verified here.
            scaling_factors = x(:) ./ double(y(:));  
            scaling_factor_pp  = max(scaling_factors) - min(scaling_factors);   % peak-peak value
            
            import matlab.unittest.Verbosity
            self.log(Verbosity.Detailed, sprintf('alpha = %.5e: %.5e', Alpha, scaling_factor_pp))

            % These abstol are chosen such that the test does not fail
            % when it is executed 1000 times after setting rng('default').
            self.verifyLessThan(scaling_factor_pp, 4e-7);
        end
        
        function BlockProcessing(self, Alpha, method, complexNoise, upsample_factor)
            % This tests verifies that generating noise in blocks gives the
            % same result as generating 1 long stream. 
            sprev = rng('shuffle');
            L       = 10e3;
            nBlocks = 5;
            assert(mod(10e3,5)==0, 'L must be divisable by nBlocks')
            
            my_cn = signal.ColoredNoise('method', method, ...
                                        'powerValue', 10, ...
                                        'noiseColor', 'custom', ...
                                        'alpha', Alpha, ...
                                        'complexNoise', complexNoise, ...
                                        'upsample_factor', upsample_factor, ...
                                        'f_low', 50, ...
                                        'f_high', 100000, ...
                                        'powerRefFreq', 100, ...
                                        'fs', upsample_factor * 1e6);
            
            % Generate noise in blocks
            rng(sprev);
            y = [];
            for k=1:nBlocks
                temp = my_cn.do(L/nBlocks);
                y = [y; temp];                                              %#ok<AGROW>
            end
            my_cn.reset();
            
            % Generate noise in 1 stream
            rng(sprev);
            z = my_cn.do(L);
            if strcmpi(method, 'kasdin')
                abstol = 1e-13;
                reltol = 1e-12;
            elseif strcmpi(method, 'RC') && upsample_factor==5
                abstol = 2e-13;
                reltol = 5e-15;
            else
                abstol = 0;
                reltol = 0;
            end                
            self.verifyEqual(double(y), double(z), 'abstol', abstol, 'reltol', reltol', sprintf('The maximum abs error is %.5e, the maximum relative error is %.5e', max(abs(y-z)), max(abs(y-z)./double(z))));
        end
        
        function Method_vs_Alpha_vs_complex(self, AllAlpha, method, complexNoise)
            Fs    = 1e6;
            my_cn = signal.ColoredNoise('method', method, 'noiseColor', 'custom', 'alpha', AllAlpha, 'Fs', Fs, 'complexNoise', complexNoise);
            H     = my_cn.filtObj.freqz([0.01, 0.1]*Fs, Fs);
            
            slope_actual = diff(20*log10(abs(H)+eps(1)));
            slope_expect = AllAlpha * -10;
            self.log(2, sprintf('%s: Expected slope - actual slope = %.2f - %.2f = %.2f', method, slope_expect, slope_actual, slope_expect- slope_actual));
            self.verifyEqual(slope_actual, slope_expect, 'abstol', 1);
        end
        
        function CopyObj(self)
            % This test checks whether the copy of the object does create a
            % different object, instead of copying the handle to the
            % object.
            cn1 = signal.ColoredNoise('method', 'kasdin');
            cn2 = copy(cn1);                                                % creates a copy of cn1

            cn1.method = 'rc';
            self.verifyTrue(strcmpi(cn2.method, 'kasdin'));
            self.verifyTrue(strcmpi(cn1.method, 'rc'));
            
            % Test referencing to object
            cn1 = signal.ColoredNoise('method', 'kasdin');
            cn3 = cn1;                                                      % Creates a link to cn1
            cn1.method = 'rc';
            self.verifyTrue(strcmpi(cn3.method, 'rc'));
            self.verifyTrue(strcmpi(cn1.method, 'rc'));
        end
    end     
end
