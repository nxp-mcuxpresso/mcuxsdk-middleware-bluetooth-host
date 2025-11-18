%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------

classdef testSignalBB < matlab.unittest.TestCase
    properties (TestParameter)
        freqRange_real    = struct('onesided', 'onesided');
        freqRange_complex = struct('twosided', 'twosided', 'centered', 'centered');
        windowType = struct('rectwin', @rectwin, 'hann', @hann) % 
        plotUnitsY = struct('dBmW', 'dBmW', 'dBW', 'dBW')
        NFFT       = struct('pow2_13', 2^13, 'pow2_11', 2^11);
        overlap    = struct('zero', 0, 'thousand', 2^10);
        fs         = struct('pow2_16', 2^16, 'random', rand(1,1) * 2^16);
        fc_a       = struct('negative', -50, 'zero', 0, 'positive', 20);
        fc_b       = struct('negative', -20, 'zero', 0, 'positive', 50);
        Length     = struct('scalar', 1, 'vector', 10e3);
        % Arguments for plotfft
        Length_FFT = struct('even', 2^14, 'odd', 2^14+1);
        WindType   = struct('Hamming', @signal.hamming, 'Blackmanharris', @signal.blackmanharris);
        WindArgs   = struct('empty', {{}}, 'symmetric', {{'symmetric'}}, 'periodic', {{'periodic'}});   % For one or another reason we need double-brackets for the cell-arrays
        PlotArgs   = struct('empty', {{}}, 'simple', {{'b.-'}}, 'complex', {{'b', 'MarkerFaceColor', [0.5, 0.5, 0.5], 'LineWidth', 2}});
        normalize  = struct('none', 0, 'length', 1, 'ComplexMagnitude', 2, 'RealMagnitude', 3);
    end
   
    methods (Test, ParameterCombination='exhaustive', TestTags  = {'Quick', 'Unit'})  % These functions describe all tests that can be performed within this class.
        function test_construction_without_input_args(self)
            % Class must be able to be constructed without input arguments
            % (https://nl.mathworks.com/help/matlab/matlab_oop/class-constructor-methods.html#btn2kiy)
            self.verifyWarningFree(@Signal_BB);
        end
        
        function PSD_complexInput_onesided(self)
            % the pwelch-function should throw and error when the input
            % signal is complex and for one-sided PSD.
            fc = -2^10;
            Fs = 2^16;
            L  = 2^14;
            x  = Signal_BB(exp(2j*pi*fc*(0:L-1)/Fs), 0, 1/Fs);
            self.verifyError(@() pwelch(x, 'FreqRange', 'onesided', 'useMethod', 'imec'), 'DiscreteTime:ComplexInputOneSided');
        end
        
        function PSD_realInput(self, freqRange_real, windowType, plotUnitsY, NFFT, overlap, fs)
            fc = 2^10;
            L  = 2^14;
            x  = Signal_BB(sin(2*pi*fc*(0:L-1)/fs), 0, 1/fs);
            x  = SetMeanPower_dBm(x, 0);
            
            [f_m, Pxx_m] = pwelch(x, 'NFFT', NFFT, 'overlap', overlap, 'freqRange', freqRange_real, 'windowType', windowType, 'plotUnitsY', plotUnitsY, 'useMethod', 'mathworks');
            [f_i, Pxx_i] = pwelch(x, 'NFFT', NFFT, 'overlap', overlap, 'freqRange', freqRange_real, 'windowType', windowType, 'plotUnitsY', plotUnitsY, 'useMethod', 'imec');
            
            self.verifyEqual(f_m, f_i, 'abstol', 1e-11, 'reltol', 1e-13);
            self.verifyEqual(10*log10(Pxx_m), 10*log10(Pxx_i), 'abstol', 1e-12);
        end
        
        function testPSD_complexInput(self, freqRange_complex, windowType, plotUnitsY, NFFT, overlap, fs)
            fc = -2^10;
            L  = 2^14;
            x  = Signal_BB(exp(2j*pi*fc*(0:L-1)/fs), 0, 1/fs);
            x  = SetMeanPower_dBm(x, 0);
            
            [f_m, Pxx_m] = pwelch(x, 'NFFT', NFFT, 'overlap', overlap, 'freqRange', freqRange_complex, 'windowType', windowType, 'plotUnitsY', plotUnitsY, 'useMethod', 'mathworks');
            [f_i, Pxx_i] = pwelch(x, 'NFFT', NFFT, 'overlap', overlap, 'freqRange', freqRange_complex, 'windowType', windowType, 'plotUnitsY', plotUnitsY, 'useMethod', 'imec');
            
            self.verifyEqual(f_m, f_i, 'abstol', 1e-11, 'reltol', 1e-13);
            self.verifyEqual(10*log10(Pxx_m), 10*log10(Pxx_i), 'abstol', 1e-12);
        end
        
        function Multiply(self, Length, fc_a, fc_b)
            % Here we test the mtimes function, where both argument are of
            % class Signal_BB
            Fs        = randi([90,250], 1);
            impedance = randi([1,100], 1);
            fa        = randi([1,25], 1);
            fb        = randi([1,25], 1);
            a = Signal_BB(fa*ones(Length,1), fc_a, 1/Fs, impedance);
            b = Signal_BB(fb*ones(Length,1), fc_b, 1/Fs, impedance);
            c = a*b;

            abstol = 3e-9;
            self.verifyEqual(real(double(c)), real(double(a) .* double(b)), 'abstol', abstol, sprintf('The maximum error is %.5e', max(abs(real(double(c)) - (real(double(a) .* double(b)))))));
            self.verifyEqual(imag(double(c)), imag(double(a) .* double(b)), 'abstol', abstol, sprintf('The maximum error is %.5e', max(abs(imag(double(c)) - (imag(double(a) .* double(b)))))));
        end
        
        function Multiply_nonSignalBB_1(self, Length, fc_a)
            % Here we test the mtimes function, where the FIRST argument
            % is a numeric value
            Fs        = randi([90,250], 1);
            impedance = randi([1,100], 1);
            fa        = randi([1,25], 1);
            fb        = randi([1,25], 1);
            a         = Signal_BB(fa*ones(Length,1), fc_a, 1/Fs, impedance);
            b         = fb*ones(Length,1);
            c         = a*b;
            
            self.verifyEqual(real(double(c)), real(double(a) .* double(b)), 'abstol', 5e-10, sprintf('The maximum error is %.5e', max(real(double(c)) - (real(double(a) .* double(b))))));
            self.verifyEqual(imag(double(c)), imag(double(a) .* double(b)), 'abstol', 5e-10, sprintf('The maximum error is %.5e', max(imag(double(c)) - (imag(double(a) .* double(b))))));
        end

        function Multiply_nonSignalBB_2(self, Length, fc_a)
            % Here we test the mtimes function, where the SECOND argument
            % is a numeric value
            Fs        = randi([90,250], 1);
            impedance = randi([1,100], 1);
            fa        = randi([1,25], 1);
            fb        = randi([1,25], 1);
            a         = Signal_BB(fa*ones(Length,1), fc_a, 1/Fs, impedance);
            b         = fb*ones(Length,1);
            c         = b*a;
            
            self.verifyEqual(real(double(c)), real(double(a) .* double(b)), 'abstol', 5e-10, sprintf('The maximum error is %.5e', max(real(double(c)) - (real(double(a) .* double(b))))));
            self.verifyEqual(imag(double(c)), imag(double(a) .* double(b)), 'abstol', 5e-10, sprintf('The maximum error is %.5e', max(imag(double(c)) - (imag(double(a) .* double(b))))));
        end
        
        function Add(self, Length, fc_a, fc_b)
            Fs        = 100;
            impedance = 83;
            fa        = 10;
            fb        = 5;
            a = Signal_BB(fa*ones(Length,1), fc_a, 1/Fs, impedance);
            b = Signal_BB(fb*ones(Length,1), fc_b, 1/Fs, impedance);
            c = a + b;

            self.verifyEqual(real(double(c)), real(double(a) + double(b)), 'abstol', 5e-11, sprintf('The maximum error is %.5e', max(real(double(c)) - (real(double(a) + double(b))))));
            self.verifyEqual(imag(double(c)), imag(double(a) + double(b)), 'abstol', 5e-11, sprintf('The maximum error is %.5e', max(imag(double(c)) - (imag(double(a) + double(b))))));
        end
        
        function Subtract(self, Length, fc_a, fc_b)
            Fs        = 100;
            impedance = 83;
            fa        = 10;
            fb        = 5;
            a = Signal_BB(fa*ones(Length,1), fc_a, 1/Fs, impedance);
            b = Signal_BB(fb*ones(Length,1), fc_b, 1/Fs, impedance);
            c = a - b;

            self.verifyEqual(real(double(c)), real(double(a) - double(b)), 'abstol', 5e-11, sprintf('The maximum error is %.5e', max(real(double(c)) - (real(double(a) - double(b))))));
            self.verifyEqual(imag(double(c)), imag(double(a) - double(b)), 'abstol', 5e-11, sprintf('The maximum error is %.5e', max(imag(double(c)) - (imag(double(a) - double(b))))));
        end
        
        function Construct_NonVectorInput(self)
            self.verifyError(@() Signal_BB(ones(10,10)), 'MATLAB:Signal_BB:expectedVector')
        end
        
        function Construct_SignalBBInput(self)
            a = Signal_BB(rand(10,1), rand(1,1), rand(1,1), rand(1,1), rand(1,1));
            b = Signal_BB(a);
            
            self.verifyEqual(a.signal, b.signal)
            self.verifyEqual(a.fc_Hz, b.fc_Hz)
            self.verifyEqual(a.Impedance, b.Impedance)
            self.verifyEqual(a.deltaT, b.deltaT)
            self.verifyEqual(a.Start, b.Start)
            
            % Check for error when input is Signal_BB with extra input arguments
            self.verifyError(@() Signal_BB(a, rand(1,1), rand(1,1), rand(1,1), rand(1,1)), '');
        end
        
        function MultiplyElementWise(self, Length, fc_a, fc_b)
            % Here we test the times function, where both argument are of
            % class Signal_BB
            Fs        = randi([90,250], 1);
            impedance = randi([1,100], 1);
            fa        = randi([1,25], 1);
            fb        = randi([1,25], 1);
            a = Signal_BB(fa*ones(Length,1), fc_a, 1/Fs, impedance);
            b = Signal_BB(fb*ones(Length,1), fc_b, 1/Fs, impedance);
            c = a.*b;

            abstol = 3e-9;
            self.verifyEqual(real(double(c)), real(double(a) .* double(b)), 'abstol', abstol, sprintf('The maximum error is %.5e', max(real(double(c)) - (real(double(a) .* double(b))))));
            self.verifyEqual(imag(double(c)), imag(double(a) .* double(b)), 'abstol', abstol, sprintf('The maximum error is %.5e', max(imag(double(c)) - (imag(double(a) .* double(b))))));
        end
        
        function PlotFFT(self, Length_FFT, WindType, WindArgs, PlotArgs, normalize)
            % This function checks whether the function can handle all
            % combination of input arguments. Besides this, it checks some
            % very basic properties of its output.
            Fs = 2e6;
            fc = 100e3;
            x = Signal_BB(ones(Length_FFT,1), fc, 1/Fs);
            H = plotfft(x, 'WindType', WindType, 'WindArgs', WindArgs, 'PlotArgs', PlotArgs, 'normalize', normalize, 'Fs', Fs);
            [f, y_dB, y_lin] = plotfft(x, 'WindType', WindType, 'WindArgs', WindArgs, 'PlotArgs', PlotArgs, 'normalize', normalize, 'Fs', Fs);
            
            self.verifyTrue(ishandle(H));
            self.verifyTrue(max(abs(f))<Fs);
            switch normalize
                case 2
                    self.verifyEqual(max(abs(y_lin)), 1, 'reltol', 0.1);
                    self.verifyEqual(max(y_dB), 0, 'abstol', 1);
                case 3
                    self.verifyEqual(max(abs(y_lin)), 2, 'reltol', 0.1);
                    self.verifyEqual(max(y_dB), 20*log10(2), 'abstol', 1);
            end
            close all;
        end
    end
end
