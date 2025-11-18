%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
classdef Signal_BB < DiscreteTime
    % Description
    %   This class is used to represent discrete time complex baseband
    %   signals. A discrete time complex baseband signal is  characterized by;
    %       a) vector of with complex-valued rms Voltage amplitudes
    %       b) Sampling period
    %       c) Center-frequency
    %       d) Impedance level (optional)
    %   Example 1
    %       A=Signal_BB(ones(1,1e3),1e6,1e-3,50)       %= 1 MHz sinus tone with a power of 13dBm
    %   Example 2
    %       A=Signal_BB(ones(1,1e3),1e6,1e-4,50)       %= 2 MHz, 13dBm sinus tone
    %       B=Signal_BB(ones(1,1e3),1e6+1000,1e-4,50)  %= Same a 'A', but with a 1kHz higher frequency.
    %       C=A+B; %=> Aver.Power=16dBm, centre freq=1[MHz](inherited from A), f_sample=10[kHz]
    %       figure; plot(C) % Visualize the real and imag part of C
    %       figure; psd(C)  % Visualize the power spectral density of C
    
    %
    %   The following operators are support; plus, minus, abs, angle,
    %   conj, scalar multiplication and multiplication of two signals of type 'Signal_BB'.
    %   The operators with two 'Signal_BB' signals only works if their sampling rate and
    %   impedance-level are the same!
    %   Disclaimer: With all these operations, the user should validate whether the result is correct!!!!
    %   See also DiscreteTime, TwoComplement
    % Description end
    properties
        fc_Hz
        Impedance
    end
    
    properties (Dependent)
         AmplitudeType
    end
    
    methods
        function self = Signal_BB(varargin)     % Constructor
            % Usage:
            % Signal_BB(samples, fc, deltaT, impedance, start)

            if nargin && isa(varargin{1}, 'Signal_BB')
                % If 'samples' is a Signal_BB, calling this function will 
                % be regarded as 'making sure it is a Signal_BB' and 
                % the input is simply returned. To enforce this approach, 
                % any other inputs will not be accepted.
                assert(length(varargin)==1, 'If the first input is a Signal_BB, no other inputs are allowed');
                self = varargin{1};
            else
                p = inputParser();
                p.KeepUnmatched = true;
                p.addOptional('samples', 1, @(x) validateattributes(x, {'numeric'}, {'vector'}, mfilename, 'samples', 1));                         % Samples
                p.addOptional('fc', 0, @(x) validateattributes(x, {'numeric'}, {'real', 'scalar'}, mfilename, 'fc', 2));                        % (Hz) Carrier frequency
                p.addOptional('deltaT', 1, @(x) validateattributes(x, {'numeric'}, {'real', 'positive', 'scalar'}, mfilename, 'deltaT', 3));    % (s) Sampling period
                p.addOptional('Imp', 50, @(x) validateattributes(x, {'numeric'}, {'real', 'positive', 'scalar'}, mfilename, 'Imp', 4));         % (Ohm) Impedance
                p.addOptional('Start', 0, @(x) validateattributes(x, {'numeric'}, {'real', 'scalar'}, mfilename, 'Start', 5));                  % (s) Absolute time of the start of the packet
                p.parse(varargin{:});
            
                self.signal    = p.Results.samples(:);
                self.fc_Hz     = p.Results.fc;
                self.deltaT    = p.Results.deltaT;
                self.Impedance = p.Results.Imp;
                self.Start     = p.Results.Start;
            end
        end

        function at = get.AmplitudeType(self)
            at = sprintf('Volt [@%sOhm]', num2str(self.Impedance));
        end
        
        function Out = set_deltaT(self, new_deltaT, varargin)
            if new_deltaT > self.deltaT
                warning('Potential loss of information')

                % Use the old-version
                t_in    = time(In);
                t_out   = t_in(1) + (0:floor(t_in(end)/new_deltaT))*new_deltaT;
                samples = interp1(t_in, In.signal, t_out);
                
            elseif new_deltaT == self.deltaT
                Out = copy(self);
                return;
            else
                p = inputParser();
                p.addParameter('filter_factor', 4, @(x) validateattributes(x, {'numeric'}, {'positive', 'real', 'integer', 'scalar'}, 'set_deltaT', 'filter_factor'));
                p.addParameter('cutoff', 0.5, @(x) validateattributes(x, {'numeric'}, {'positive', 'real', 'scalar', '<=', 1}, 'set_deltaT', 'cutoff'));
                p.addParameter('filter', [], @(x) validateattributes(x, {'digfilt.digfiltabstract'}, {'scalar'}, 'set_deltaT', 'filter'));
                p.parse(varargin{:});

                H = p.Results.filter;
                
                upsample_factor = self.deltaT / new_deltaT;
                if isempty(H)
                    filter_factor   = p.Results.filter_factor;
                    cutoff          = p.Results.cutoff;

                    [samples, ~] = interp(self.signal, upsample_factor, filter_factor, cutoff);
                else
                    samples = H.filter(self.signal);
                end
                PLOT = false;
                if PLOT
                    figure; hold on;        %#ok<UNRCH>
                    if ~isreal(samples)
                        subplot(2,1,1); hold on;
                    end
                    plot((0:length(self)-1)*self.deltaT, real(self.signal), 'b.-'); 
                    plot((0:length(Out)-1)*new_deltaT, real(samples), 'r.-');
                    if ~isreal(samples)
                        subplot(2,1,2); hold on;
                        plot((0:length(self)-1)*self.deltaT, imag(self.signal), 'b.-');
                        plot((0:length(Out)-1)*new_deltaT, imag(samples), 'r.-');
                    end

                    fvtool(H, 'Fs', 1/new_deltaT);
                end
            end
            Out = Signal_BB(samples, self.fc_Hz, new_deltaT, self.Impedance);
        end
        
        function values = double(self)
            values = GetValues(self);
        end
        
        function values = GetValues(self)
            temp = copy(self);
            if temp.fc_Hz
                temp = set_freq(temp, 0);
            end
            values = temp.signal;
        end
        
        function self = set_freq(self, NewFreq)
            t           = time(self);
            self.signal = self.signal .* exp(-2i*pi*t*(NewFreq-self.fc_Hz));
            self.fc_Hz  = NewFreq;
        end
        
        function CV2=decimate(CV_In,R,varargin)
            CV2=CV_In;
            CV2.deltaT=CV_In.deltaT*R;
            CV2.signal=decimate(CV_In.signal,R,varargin{:});
        end
        
        function CV2=interp(CV_In,R,varargin)
            CV2=CV_In;
            CV2.deltaT=CV_In.deltaT/R;
            CV2.signal=interp(CV_In.signal,R,varargin{:});
        end
        
        function other = makeequal(self, other)
            self.check_other(other, 'sameStart', true);
            if self.fc_Hz ~= other.fc_Hz
                %disp('[Signal_BB/plus]: frequencies different. Taking freq of 1e argument');
                other = set_freq(other,self.fc_Hz);
            end
        end
        
        % Arithmic
        function C = plus(A, B)
            B = makeequal(A, B);
            C = copy(A);                                                    % Copy all properties of A
            C.signal = A.signal + B.signal;                                 
        end
        
        function out = minus(self, other)
            % All checks on the inputs are done in the plus operation
            out = self + -other;
        end
        
        function CV=power(CV)
            if nargout>0
                CV=copy(CV);
            end
            CV=CV.signal.^2;
        end
        function out=angle(CV)
            if nargout>0
                CV=copy(CV);
            end
            out=angle(CV.signal);
        end
        function CV=real(CV)
            if nargout>0
                CV=copy(CV);
            end
            CV=real(CV.signal);
        end
        function CV=imag(CV)
            if nargout>0
                CV=copy(CV);
            end
            CV=imag(CV.signal);
        end
        function CV=abs(CV)
            if nargout>0
                CV=copy(CV);
            end
            CV=abs(CV.signal);
        end
        function CV=conj(CV)
            if nargout>0
                CV=copy(CV);
            end
            CV.signal=conj(CV.signal);
            CV.fc_Hz=-CV.fc_Hz;
        end

        function y = mtimes(self, other)
            % Since the data in the Signal_BB can only be a vector or
            % scalar, it is assumed that when two vectors/scalars are multiplied,
            % it means that it is an element-wise multiplication. Therefore
            % the 'times' function is executed.
            y = self .* other;
        end
        
        function y = times(self, other)
            if isnumeric(self) && isa(other, 'Signal_BB') % This happens when we multiply a numeric value with a Signal_BB
                temp  = other;
                other = self;
                self  = temp;
            end
            if ~isa(other, 'Signal_BB')
                %TODO: should I use self.Start or 0 as Start-time?
                other = Signal_BB(other, 0, self.deltaT, self.Impedance, 0);   % make other a Signal_BB as well
            end
            self.check_other(other);
            y = Signal_BB(self.signal .* other.signal, self.fc_Hz + other.fc_Hz, self.deltaT, self.Impedance, self.Start);
        end
        
        function check_other(self, other, varargin)
            p = inputParser();
            p.addParameter('sameStart', true, @(x) validateattributes(x, {'logical'}, {'scalar'}, 'check_other', 'sameStart'));
            p.addParameter('sameSize', true, @(x) validateattributes(x, {'logical'}, {'scalar'}, 'check_other', 'sameSize'));
            p.parse(varargin{:});
            sameStart = p.Results.sameStart;
            sameSize = p.Results.sameSize;
            
            assert(isa(other, 'Signal_BB'), sprintf('Both signals should be of type Signal_BB, but the other signal is of class %s instead', class(other)));
            assert(abs(self.deltaT - other.deltaT)<1e-16, 'Sampling rate should be identical');
            assert(self.Impedance == other.Impedance, 'Both signals should have equal impedance');
            if sameSize
                assert(all(size(self.signal) == size(other.signal)) || any([isscalar(self.signal), isscalar(other.signal)]), 'Signals should be equally sized, or one should be a scalar');
            end
            if sameStart
                assert(self.Start == other.Start, 'Both signals should have equal start-time');
            end
        end
        
        function [y, state] = filter(B, A, x, state)
            if nargin==3
                state = [];
            end
            y = copy(x);
            [y.signal, state] = filter(B, A, x.signal, state);
        end
        
        function [CV_out]=mrdivide(CV,scalar)
            if isscalar(scalar)
                CV_out=copy(CV);
                CV_out.signal=CV_out.signal/scalar;
            else
                error('Not Supported:Ask Jac')
            end
        end
        
        function [MP_dBm]=MeanPower_dBm(CV)
            MP_dBm=10*log10(mean(abs(CV.signal(:)).^2)/CV.Impedance)+30;
        end
        
        function [MaxP_dBm]=PeakPower_dBm(CV)
            MaxP_dBm=10*log10(max(abs(CV.signal(:)).^2)/CV.Impedance)+30;
        end
        
        function CV_out = SetMeanPower_dBm(CV,WantedPower_dBm)
            CV_out        = CV;
            CV_out.signal = CV_out.signal/10^((CV.MeanPower_dBm-WantedPower_dBm)/20);
        end

        function plot_power(CV)
            tmp=CV.signal;
            plot(time(CV)*1e3,20*log10(abs(tmp))-10*log10(CV.Impedance)+30)
            xlabel('time[millisec]')
            ylabel('Power [dBm]')
        end
        
        function varargout = pwelch(self, varargin)
            p = inputParser;
            p.KeepUnmatched = true;
            p.addParameter('plotUnitsY', 'dBm', @(x) any(strcmpi(x, {'dBW', 'dBmW', 'dBm', 'dBc'})));
            p.parse(varargin{:});

           [varargout{1:nargout}] = self.pwelch@DiscreteTime('plotUnitsY', p.Results.plotUnitsY, p.Unmatched);
        end
        
        function varargout = disp(CV)
            TEXT=['Signal_BB: Aver.Power=' num2str(MeanPower_dBm(CV)) 'dBm, centre freq=' freq2str(CV.fc_Hz) ', f_sample=' freq2str(1/CV.deltaT) ', start=' time2str(CV.Start)];
            if nargout==0
                fprintf([TEXT '\n']);
            elseif nargout==1
                varargout(1) = {TEXT};
            else
                error('only 1 output argument possible')
            end
        end
        
        function H = plot(self, varargin)
            plotargs = varargin;
            if any(strcmpi(varargin, 'plotPhaseMag'))
                % remove plotPhaseMag from plotargs
                indx = find(strcmpi(varargin, 'plotPhaseMag'));
                plotargs(indx+[0,1]) = [];
                plotPhaseMag = varargin{indx+1};
                validateattributes(plotPhaseMag, {'logical'}, {'scalar'}, 'plot', 'plotPhaseMag')
            else
                plotPhaseMag = false;
            end
            
            t = time(self);
            [prefix, scale] = self.SIprefix(t(end));
            t = t/scale;
            if isreal(self.signal)
                H(1) = gca;
                H(2) = plot(t, double(self.signal), plotargs{:});
                xlabel(sprintf('Time [%ss]', prefix));
                ylabel('Amplitude');
            else
                if plotPhaseMag
                    H(1) = subplot(2,1,1); hold on;
                    H(end+1) = plot(t, abs(double(self.signal)), plotargs{:});
                    title('Magnitude');
                    xlabel(sprintf('Time [%ss]', prefix));
                    ylabel('Magnitude');

                    H(end+1) = subplot(2,1,2); hold on;
                    H(end+1) = plot(t, angle(double(self.signal)), plotargs{:});
                    title('Angle');
                    xlabel(sprintf('Time [%ss]', prefix));
                    ylabel('Phase');
                else
                    H(1) = subplot(2,1,1); hold on;
                    H(end+1) = plot(t, real(double(self.signal)), plotargs{:});
                    title('Real part');
                    xlabel(sprintf('Time [%ss]', prefix));
                    ylabel('Amplitude');

                    H(end+1) = subplot(2,1,2); hold on;
                    H(end+1) = plot(t, imag(double(self.signal)), plotargs{:});
                    title('Imaginary part');
                    xlabel(sprintf('Time [%ss]', prefix));
                    ylabel('Amplitude');
                end
            end
        end
        
        function [f,y_as,Ylin] = plotfft(x, varargin)
            %[f,y_as,Ylin] = plotfft(x, Cfg)
            % This function plots the FFT of the signal x, with N=length(x) samples
            % INPUT:
            %   x           -   Input signal
            %   Cfg         -   Configuration struct with the following fields:
            %                   * Fs        -   Sampling frequency (Hz)
            %                   * WindType  - the name of the desired window. Type
            %                                 'help window' to see which window-names
            %                                 are valid. (Default = @hanning)
            %                   * WindArgs  - cell-array with window-function arguments
            %                   * normalize - string or numeric value:       (Default = 'none')
            %                                 o 0) 'none'; no normalization
            %                                 o 1) 'Length'; normalize with respect to the
            %                                       length of the input signal.
            %                                 o 2) 'ComplexNormalize'; normalizes a complex
            %                                       tone with amplitude 1 to 0dB:
            %                                       1.0 * exp(j*2*pi*f*t+phi) => 0dB
            %                                 o 3) 'RealNormalize'; normalizes a real
            %                                       tone with amplitude 1 to 0dB:
            %                                       1.0 * cos(2*pi*f*t+phi) => 0dB
            %                   * PlotArgs  - cell-array with plot-function arguments
            % OUTPUT:
            % with 1 output argument:
            %   f           -   handle to plotted line
            % with more than 1 output argument:
            %   f           -   values for x-axis
            %   y_as        -   values for y-axis
            %   Ylin        -   values for y-axis (linear)
            %
            % If this function is used for a sine/cosine with a single frequency (f),
            % then two vertical bars are expected. To obtain this, the number of input
            % samples (N), the sampling frequency Fs and f must have the relation:
            % fN/Fs must be an integer.
            
            p = inputParser();
            p.KeepUnmatched = true;
            p.addParameter('Fs', 1e6, @(x) validateattributes(x, {'numeric'}, {'real', 'positive', 'scalar'}, 'plotfft', 'Fs'));
            p.addParameter('WindType', @signal.rectwin, @(x) validateattributes(x, {'function_handle'}, {}, 'plotfft', 'WindType'));
            p.addParameter('WindArgs', {}, @(x) validateattributes(x, {'cell'}, {}, 'plotfft', 'WindArgs'));
            p.addParameter('normalize', 1, @(x) validateattributes(x, {'numeric'}, {'real', 'nonnegative', 'scalar', '<=', 3}, 'plotfft', 'normalize'));
            p.addParameter('PlotArgs', {}, @(x) validateattributes(x, {'cell'}, {}, 'plotfft', 'PlotArgs'));
            p.parse(varargin{:});
            Cfg = p.Results;
            Fs  = p.Results.Fs;
            
            x = double(x);                                                  % Convert Signal_BB into double array
            N = length(x);
            
            W = window(Cfg.WindType, length(x), Cfg.WindArgs{:});
            W = reshape(W, size(x));
            
            [fs_prefix, fs_scale] = DiscreteTime.SIprefix (Fs);
                        
            %calculate the x-axis values
            if mod(N,2)                                                     % N is odd
                f = (-N/2+1/2:N/2-1/2)/N*Fs;
            else                                                            % N is even
                f = (-Fs/2:Fs/N:Fs/2-Fs/N);
            end
            f = f.';
            
            %calculate the y-axis values
            x       = W.*x;                                                 % Apply window
            Ylin    = fftshift(fft(x));
            switch Cfg.normalize
                case 0
                    norm_factor = 1;
                case 1
                    norm_factor = 1/length(x);
                case 2
                    norm_factor = 1/sum(W);
                case 3
                    norm_factor = 2/sum(W);
            end
            Ylin = Ylin * norm_factor;
            y_as = 20*log10(abs(Ylin)+eps(1));
            
            if nargout==0 || nargout==1
                f = plot(f / fs_scale, y_as, Cfg.PlotArgs{:});
                ylabel('Magnitude [dBFS^2/bin]');
                xlabel(sprintf('Frequency [%sHz]', fs_prefix));
                grid on;
            end
        end

        function B = copy(A)
            if isa(A,'handle')
                B           = Signal_BB();
                B.signal    = A.signal;
                B.fc_Hz     = A.fc_Hz;
                B.deltaT    = A.deltaT;
                B.Impedance = A.Impedance;
                B.Start     = A.Start;
            else
                B = A;
            end
        end       
        
        function C = uminus(A)
            % C = -A 
            % negates the elements of A and stores the result in C.
            C = copy(A);
            C.signal = -C.signal;
        end
        
        function C = uplus(A)
            % C = +A 
            % returns A and stores it in C.
            C = copy(A);
        end
    end
end

% Local functions
function [varargout]=freq2str(value_Hz)
    if nargin==0
        ValuesToTest=[1.1 pi 100 1001 1e10];
        for value=ValuesToTest
            TXT=freq2str(value);
            disp(TXT);
        end
        
    else
        Accuracy=10;
        log_value_Hz=log10(value_Hz);
        if log_value_Hz<3
            ScalingFactor=1e0;
            unit='Hz';
        elseif log_value_Hz<6
            ScalingFactor=1e3;
            unit='kHz';
        elseif log_value_Hz<9
            ScalingFactor=1e6;
            unit='MHz';
        elseif log_value_Hz<12
            ScalingFactor=1e9;
            unit='GHz';
        else
            error('out of range')
        end
        valuestr=num2str(round(value_Hz/ScalingFactor*Accuracy)/Accuracy);
        if nargout==1
            varargout{1}=[valuestr, unit];
        elseif nargout==3
            varargout{1}=valuestr ;
            varargout{2}=unit;
            varargout{3}=ScalingFactor;
        end
    end
end

function [varargout]=time2str(value_sec)
    if nargin==0
        ValuesToTest=-[1.1 1/pi 0.0001 1e-9 1e-11];
        for value=ValuesToTest
            TXT=time2str(value);
            disp(TXT);
        end
        
    else
        Accuracy=10;
        log_value_sec=-log10(abs(value_sec));
        if value_sec==0
            ScalingFactor=1e-15;
            unit='femtosec';
        elseif log_value_sec<0
            ScalingFactor=1e0;
            unit='sec';
        elseif log_value_sec<3
            ScalingFactor=1e-3;
            unit='millisec';
        elseif log_value_sec<6
            ScalingFactor=1e-6;
            unit='microsec';
        elseif log_value_sec<9
            ScalingFactor=1e-9;
            unit='nanosec';
        elseif log_value_sec<12
            ScalingFactor=1e-12;
            unit='picosec';
        elseif log_value_sec<15
            ScalingFactor=1e-15;
            unit='femtosec';
        else
            error('out of range')
        end
        valuestr=num2str(round(value_sec/ScalingFactor*Accuracy)/Accuracy);
        if nargout==1
            varargout{1}=[valuestr '[' unit ']'];
        elseif nargout==3
            varargout{1}=valuestr ;
            varargout{2}=unit;
            varargout{3}=ScalingFactor;
        end
        
    end
end