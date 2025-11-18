%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------

classdef testDigfilt < SeedRng
    properties (TestParameter)
        filtObj = struct( ...
            'FIR_real', digfilt.FIRFilter(rand(1,32)-0.5), ...
            'FIR_complex', digfilt.FIRFilter(complex(rand(1,32)-0.5, rand(1,32)-0.5)), ...
            'IIR_real', digfilt.IIRFilter(rand(1,32)-0.5, poly((rand(5,1)-0.5)*1.99)), ...;     % For the denominator we need poles within the unit-circle.
            'IIR_complex', digfilt.IIRFilter(complex(rand(1,32)-0.5, rand(1,32)-0.5), poly(complex((rand(5,1)-0.5)*1.99/sqrt(2), (rand(5,1)-0.5)*1.99/sqrt(2)))), ...
            'Differentiator', digfilt.Differentiator, ...
            'Integrator', digfilt.Integrator, ...
            'FIRInterpolator_real', digfilt.FIRInterpolator(randi([2,13],1,1), rand(1,32)-0.5), ...                            % Random coeffs and random interpolation factor between 2 and 13
            'FIRInterpolator_complex', digfilt.FIRInterpolator(randi([2,13],1,1), complex(rand(1,32)-0.5, rand(1,32)-0.5)), ... % Random coeffs and random interpolation factor between 2 and 13
            'FilterCascade_singlerrate', digfilt.FilterCascade(digfilt.FIRFilter(complex(rand(1,32)-0.5, rand(1,32)-0.5)), digfilt.IIRFilter(rand(1,32)-0.5, poly((rand(5,1)-0.5)*1.99))), ...
            'FilterCascade_multirate', digfilt.FilterCascade(digfilt.FIRInterpolator(3, rand(1,32)-0.5), digfilt.FIRInterpolator(2, complex(rand(1,32)-0.5, rand(1,32)-0.5))), ...
            'FilterCascade_mixedrate', digfilt.FilterCascade(digfilt.FIRFilter(complex(rand(1,32)-0.5, rand(1,32)-0.5)), digfilt.FIRInterpolator(randi([2,13],1,1), rand(1,32)-0.5))...
            );

          respType = {'onesided', 'twosided', 'half', 'whole'};
          nPoints = struct('odd_value', 1001, 'even_value', 8192);
          InterpolationFactor = num2cell(2:13);
    end
   
    methods (TestClassSetup)                                                % These functions will be called once, at the start of the test-run
        function setup(self)
            self.applyFixture( setupEnv );
        end
    end   
    
    methods (Test, ParameterCombination='exhaustive', TestTags  = {'Quick', 'Unit'})  % These functions describe all tests that can be performed within this class.
        function test_construction_without_input_args(self, filtObj)
            % Class must be able to be constructed without input arguments
            % (https://nl.mathworks.com/help/matlab/matlab_oop/class-constructor-methods.html#btn2kiy)
            fh = str2func(class(filtObj));
            self.verifyWarningFree(fh);
        end
        
        function testDigfilt_filter_step_do(self, filtObj)
            dspObj = testDigfilt.filt2dspObj(filtObj);
            
            x = randn(1e6, 1);
            
            filtObj.reset();
            y1 = filtObj.step(x);
            filtObj.reset();
            y2 = filtObj.do(x);
            filtObj.reset();
            y3 = filtObj.filter(x);
            dspObj.reset()
            z = dspObj(x);

            abstol = 3e-11; 
            if max(abs(y1-z))>abstol
                [~,den] = filtObj.tf();
                self.log(3, sprintf('The largest pole of the fiilter is: %.4f', max(abs(roots(den)))));
            end
            self.verifyEqual(y1, y2);
            self.verifyEqual(y1, y3);
            self.verifyEqual(y1, z, 'abstol', abstol, sprintf('The maximum error is %.5e\n', max(abs(y1-z))));
        end
        
        function testDigfilt_preservestate(self, filtObj)
            x = randn(1,1e5);
            nBlocks = 10;
            assert(floor(length(x)/nBlocks) == length(x)/nBlocks, 'length of x must be divisible by nBlocks')
            blockLength = length(x)/nBlocks;
            y = [];
            filtObj.reset();
            for k=1:nBlocks
                temp = filtObj.filter(x((1:blockLength) + (k-1)*blockLength));
                y = [y, temp];                                              %#ok<AGROW>
            end
            filtObj.reset();
            z = filtObj.filter(x);
            
            abstol = 3e-11;
            self.verifyEqual(y, z, 'abstol', abstol, sprintf('The maximum error is %.5e\n', max(abs(y-z))));
        end
        
        function testDigfilt_freqz_default(self, filtObj)
            % testing filtObj.freqz()
            self.freqz_common(filtObj);
        end

        function testDigfilt_freqz_nPoints(self, filtObj, nPoints)
            % testing filtObj.freqz(N)
            self.freqz_common(filtObj, nPoints);
        end

        function testDigfilt_freqz_respType(self, filtObj, nPoints, respType)
            % testing filtObj.freqz(N, 'whole')
            self.freqz_common(filtObj, nPoints, respType);
        end

        function testDigfilt_freqz_Fs(self, filtObj, nPoints)
            % testing filtObj.freqz(N, fs)
            self.freqz_common(filtObj, nPoints, 32000);
        end

        function testDigfilt_freqz_nPoints_respType_Fs(self, filtObj, nPoints, respType)
            % testing filtObj.freqz(N, 'whole', fs)
            self.freqz_common(filtObj, nPoints, respType, 32000);
        end
        
        function testDigfilt_freqz_freqvect(self, filtObj, nPoints)
            % testing filtObj.freqz(w)
            self.freqz_common(filtObj, linspace(0, pi/10, nPoints));
        end
        
        function testDigfilt_freqz_freqvect_fs(self, filtObj, nPoints)
            Fs = 32000;
            self.freqz_common(filtObj, linspace(0, Fs/4, nPoints), Fs);
        end
        
        function testDigfilt_impz(self, filtObj, nPoints)
            [num, den] = filtObj.tf();
            Fs = abs(randn(1,1));
            [h1, t1] = filtObj.impz(nPoints, Fs);
            [h2, t2] = impz(num, den, nPoints, Fs);

            self.verifyEqual(h1, h2);
            self.verifyEqual(t1, t2);
        end
        
        function testDigfilt_fvtool(self, filtObj) %#ok<INUSL>
            %I don't know how to check that the visualization is
            %correct. Let's at least test whether the function does not
            %crash.
            filtObj.fvtool();
            close all
        end
        
        function FIRInterpolator_upsample(self, InterpolationFactor)
            % This test verifies the FIRInterpolator.upsample function
            % against the matlab upsample-function
            H = digfilt.FIRInterpolator(InterpolationFactor, ones(1, 10));
            x = rand(100,1);
            for k=0:(InterpolationFactor-1)
                y = H.upsample(x, k);
                z = upsample(x,InterpolationFactor,k);       % Matlab function from the signal-processing toolbox
                self.verifyEqual(y, z);
            end
        end
    end
    
    methods 
        function freqz_common(self, filtObj, varargin)
            dspObj = testDigfilt.filt2dspObj(filtObj);
            
            [h1, w1] = filtObj.freqz(varargin{:});
            [h2, w2] = dspObj.freqz(varargin{:});

% figure; subplot(2,1,1); hold on; 
% plot(w1, 20*log10(abs(h1)+eps(1)), '.-'); 
% plot(w2, 20*log10(abs(h2)+eps(1)), 'o--');
% subplot(2,1,2); hold on; 
% plot(w1, unwrap(angle(h1)), '.-'); 
% plot(w2, unwrap(angle(h2)), 'o--')

            if length(varargin)==3 && rem(varargin{1},2)==1
                % In this case we have N=odd_value and Fs=specified by user
                % due to round-off error, the frequency pi and 2pi might have slight round-off errors. In the Mathworks implementation there is a correction for this.
                abstol_w = 2e-12;       
            else
                abstol_w = 0;
            end
            abstol_h = 8e-12;
            self.verifyEqual(w1(:), w2(:), 'abstol', abstol_w, sprintf('The maximum absolute error in w is %.5e', max(abs(w1(:)-w2(:)))));
            self.verifyEqual(real(h1(:)), real(h2(:)), 'abstol', abstol_h, sprintf('The maximum absolute error in real(h) is %.5e', max(abs(real(h1(:))-real(h2(:))))));
            if ~isreal(h1)
                self.verifyEqual(imag(h1(:)), imag(h2(:)), 'abstol', abstol_h, sprintf('The maximum absolute error in imag(h) is %.5e', max(abs(imag(h1(:))-imag(h2(:))))));
            end                
        end
    end
    
    methods (Static)
        function dspObj = filt2dspObj(filtObj)
            % create dsp-object of same type as filt-object
            if isa(filtObj, 'digfilt.FilterCascade')
                temp = cell(length(filtObj.Stage), 1);
                for k=1:length(filtObj.Stage)
                    temp{k} = testDigfilt.filt2dspObj(filtObj.Stage(k));
                end
                dspObj = dsp.FilterCascade(temp{:});
            else
                [num, den] = filtObj.tf();
                switch class(filtObj)
                    case {'digfilt.FIRFilter', 'digfilt.Differentiator'}
                        [num, ~] = filtObj.tf();
                        dspObj = dsp.FIRFilter('Numerator', num);
                    case {'digfilt.IIRFilter', 'digfilt.Integrator'}
                        
                        dspObj = dsp.IIRFilter('Numerator', num, 'Denominator', den);
                    case {'digfilt.FIRInterpolator'}
                        [num, ~] = filtObj.tf();
                        dspObj = dsp.FIRInterpolator('InterpolationFactor', filtObj.InterpolationFactor, 'Numerator', num);
                    otherwise
                        error('No implementation yet for class "%s"', class(filtObj));
                end
            end
        end
    end
end
