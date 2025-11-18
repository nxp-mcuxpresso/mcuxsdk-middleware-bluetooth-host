%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This class implements a digital-filter abstract class. It contains an
%   interface definition and some common functions.
% Description end

classdef digfiltabstract < matlab.mixin.SetGet & matlab.mixin.Heterogeneous & matlab.mixin.Copyable
    properties
        state = []
    end

    properties (Hidden)
        ismultirate = false;
    end

    methods (Abstract)
        [num, den] = tf(self);                                              % gets the transfer-function
    end
    
    methods
        function reset(self)
            self.state = [];
        end
        
        function varargout = do(self, varargin)
            [varargout{1:nargout}] = self.step(varargin{:});
        end
        
        function varargout = filter(self, varargin)
            [varargout{1:nargout}] = self.step(varargin{:});
        end
        
        function y = step(self, x)
            [N, D] = self.tf();
            [y, self.state] = filter(N, D, x, self.state);
        end
        
        function Hfig = fvtool(self, varargin)
            p = inputParser();
            p.KeepUnmatched = true;
            p.addParameter('Grid', 'on', @(x) ischar(x) && ismember(lower(x), {'on', 'off'}));
            p.addParameter('Legend', false, @(x) validateattributes(x, {'logical'}, {'scalar'}, 'fvtool', 'Legend'));
            p.addParameter('Fs', [], @(x) validateattributes(x, {'numeric'}, {'positive', 'scalar'}, 'fvtool', 'Fs'));
            p.addParameter('Hfig',[], @(x) validateattributes(x, {'matlab.ui.Figure'}, {'scalar'}, 'fvtool', 'Hfig'));
            p.addParameter('plotcfg', {}, @(x) validateattributes(x, {'cell'}, {}, 'fvtool', 'plotcfg'));
            p.parse(varargin{:});
            
            Fs = p.Results.Fs;
            
            if isempty(Fs)
                [h, w]          = self.freqz(8192);
                scale           = pi;
                xlabel_unit     = '\times\pirad/sample';
            else
                [h, w]          = self.freqz(8192, Fs);
                [prefix, scale] = self.SIprefix(Fs/2);
                xlabel_unit     = sprintf('%sHz', prefix);
            end

            Hfig = p.Results.Hfig;
            if isempty(Hfig)
                Hfig = figure('name', 'Filter response');
            end
            
            figure(Hfig);
            subplot(2,1,1); hold on; 
            grid(gca, p.Results.Grid);
            plot(w/scale, 20*log10(abs(h)+eps(1)), p.Results.plotcfg{:});
            xlabel(sprintf('Frequency [%s]', xlabel_unit));
            ylabel('Magnitude [dB]');
            title('Magnitude response');

            subplot(2,1,2); hold on;
            grid(gca, p.Results.Grid);
            plot(w/scale, angle(h)/pi, p.Results.plotcfg{:});
            xlabel(sprintf('Frequency [%s]', xlabel_unit));
            ylabel('Phase [rad]');
            title('Phase response');            
        end
        
        function varargout = impz(self, varargin)
            % [H,T] = impz([N],[Fs]) computes the impulse response of the digital
            % filter, where N and Fs are optional inputs, with default
            % N=[], Fs=1;
            p = inputParser();
            p.addOptional('N', [], @(x) validateattributes(x, {'numeric'}, {'nonnegative', 'integer'}, 'impz', 'N', 1));
            p.addOptional('Fs', 1, @(x) validateattributes(x, {'numeric'}, {'positive', 'scalar'}, 'impz', 'Fs', 2));
            p.parse(varargin{:});
            N = p.Results.N;
            Fs = p.Results.Fs;
            if ~isempty(N) && ~isscalar(N)
                error('Current implementation only supports a scalar N. To be implemented.');
            end

            [num, den] = self.tf();
            if isempty(N)   % determine N
                warning('When N is not specified, a function from the signal processing toolbox is used');
                N = impzlength(num, den, 5e-5);
            end
            
            % create an impulse of length N and filter it
            x = [1; zeros(N-1,1)];
            h = filter(num, den, x);
            t = (0:(N-1)).'/Fs;
            
            if nargout
                varargout{1} = h;
                varargout{2} = t;
            else
                [prefix, scale] = self.SIprefix(1/Fs);
                figure;
                stem(t/scale, h);
                xlabel(sprintf('Time [%ss]', prefix));
                ylabel('Amplitude');
                title('Impulse response');
            end                
        end
        
        function [H, w] = freq_response_calc(self, opts)
            [num, den] = self.tf();
            N = opts.N;
            if opts.freqspec                                                % the frequency-vector is specified by the user
                if isempty(opts.Fs)                                         % using normalized frequencies
                    w = opts.w;
                else                                                        % using physical frequencies
                    w = 2*pi*opts.w/opts.Fs;
                end
                omega = exp(1i*w);
                if isscalar(den) && den==1
                    D = exp(1i*w*(length(num)-1));
                else
                    den = [den, zeros(1, length(num)-length(den))];
                    D = polyval(den, omega);
                end
                num = [num, zeros(1, length(den)-length(num))];
                H = polyval(num, omega) ./ D;
            else
                if strcmpi(opts.range, 'onesided')                          % w = 0 ... pi
                    H = fft(num, 2*N) ./ fft(den, 2*N);
                    H = H(1:N);
                else                                                        % w = 0 ... 2pi
                    H = fft(num, N) ./ fft(den, N);
                end
            end
            H = H(:);                                                       % Make sure H is a column
            w = opts.w(:);                                                  % Make sure w is a column
        end
        
        function varargout = freq_response(self, varargin)
            % This function is identical to the built-in freqz-function,
            % however it has a modern interface. This makes it more
            % convenient to use and configure,
            opts = self.parse_freq_response_inputs(varargin{:});
            
            if nargout
                [varargout{1:2}] = self.freq_response_calc(opts);                     % This part is different for the FilterCascase
            else
                if isempty(opts.Fs)
                    self.fvtool();
                else
                    self.fvtool('Fs', opts.Fs);
                end
            end
        end        
        
        function varargout = freqz(self, varargin)
            % [Hr,W] = freqz(filt,N) returns the N-point complex frequency response
            % vector Hr and the N-point frequency vector W in radians/sample of
            % the filter System object filt. The frequency response is evaluated at
            % N points equally spaced around the upper half of the unit circle.
            % If N is not specified, it defaults to 8192.
            % For more options, please have a look at the freqz-function.
            
            % The way the input-arguments of the freqz-function are
            % organized is a bit of a mess (ancient style Matlab-code).
            % Therefore they are parsed in this seperate function.
            opts = self.parse_freqz_inputs(varargin{:});
            [varargout{1:nargout}] = self.freq_response(opts);
        end
        
        function val = total_interpolation_factor(~)
            val = 1;
        end
        
        function val = total_decimation_factor(~)
            val = 1;
        end        
    end
    
    methods (Static)
        function opts = parse_freq_response_inputs(varargin)
            % This function is identical to the built-in freqz-function,
            % however it has a modern interface. This makes it more
            % convenient to use and configure,
            N_default = 8192;
            p = inputParser();
            p.KeepUnmatched = true;
            p.addParameter('N', [], @(x) validateattributes(x, {'numeric'}, {'real', 'integer', 'positive', 'scalar'}, 'freq_response', 'N'));
            p.addParameter('range', 'onesided', @(x) validateattributes(x, {'char'}, {'scalartext'}, 'freq_response', 'range'));
            p.addParameter('Fs', []);
            p.addParameter('w', [], @(x) validateattributes(x, {'numeric'}, {'real', 'vector'}, 'freq_response', 'w'));
            p.addParameter('freqspec', [], @(x) validateattributes(x, {'logical'}, {'scalar'}, 'freq_response', 'freqspec'));       % This one is typically no provided by users, but will be provided when called from the freqz-function
            p.parse(varargin{:});

            opts = p.Results;
            
            if ~isempty(opts.Fs)    % Fs may be empty or a real, positive, scalar
                validateattributes(opts.Fs, {'numeric'}, {'real', 'positive', 'scalar'}, 'freq_response', 'Fs');
            end
            
            assert(ismember(lower(opts.range), {'half','onesided','whole','twosided'}), sprintf('range must be one of {half, onesided, whole, twosided}, but is %s instead', opts.range))
            if ~isempty(opts.N) && ~isempty(opts.w)
                assert(opts.N == length(opts.w), sprintf('N and the length of w should be the same, however N=%d and length(w)=%d', opts.N, length(opts.w)));
            end
            
            if isempty(opts.w)
                if isempty(opts.N)
                    opts.N = N_default;
                end
                if isempty(opts.Fs)
                    F = 2*pi;
                else
                    F = opts.Fs;
                end
                if ismember(opts.range, {'onesided', 'half'})
                    deltaF = F/2/opts.N;
                    opts.w = linspace(0, F/2-deltaF, opts.N);
                else
                    deltaF = F/opts.N;
                    opts.w = linspace(0, F-deltaF, opts.N);
                end
                opts.freqspec = false;
            else
                if isempty(opts.N)
                    opts.N = length(opts.w);
                end
                if isempty(opts.freqspec)
                    opts.freqspec = true;
                end
            end
            assert(~isempty(opts.w), 'It is expected that w is not empty');
        end
        
        function opts = parse_freqz_inputs(varargin)
            N        = 8192;
            range    = 'onesided';
            Fs       = [];
            w        = [];
            freqspec = false;   % is frequency specified by user yes (true) or no (false)
            
            % process first argument; can be either a scalar (N) or a
            % vector (w or f)
            if nargin>=1
                temp = varargin{1};
                if isnumeric(temp)
                    if isscalar(temp)    % must be N
                        N = temp;
                        validateattributes(N, {'numeric'}, {'scalar', 'positive'}, 'freqz', 'N', 1);
                    else                 % is either w or f
                        w = temp;
                        N = length(w);
                        freqspec = true;
                        validateattributes(w, {'numeric'}, {'vector'}, 'freqz', 'w', 1);
                    end
                else
                    error('The first argument of freqz must either be a scalar (N) or a vector (w or f)');
                end
            end
            % process second argument, can be either scalar (fs), string (range)
            
            if nargin>=2
                temp = varargin{2};
                if ischar(temp)
                    assert(ismember(lower(temp), {'half','onesided','whole','twosided'}), sprintf('Second input of freqz must be one of {half, onesided, whole, twosided}, but is %s instead', temp))
                    if any(strcmpi(temp, {'whole', 'twosided'}))
                        range = 'twosided';
%                     else
                        % use default
                    end
                    if length(varargin)==3
                        Fs = varargin{3};
                        validateattributes(Fs, {'numeric'}, {'positive', 'scalar'}, 'freqz', 'Fs', 3);
                    end
                elseif isnumeric(temp) && isscalar(temp)
                    Fs = temp;
                    validateattributes(Fs, {'numeric'}, {'positive', 'scalar'}, 'freqz', 'Fs', 2);
                else
                    error('The second argument of freqz must either be a string or a scalar');
                end
            end
            
            if isempty(w)
                if isempty(Fs)
                    F = 2*pi;
                else
                    F = Fs;
                end
                if strcmpi(range, 'onesided')
                    deltaF = F/2/N;
                    w = linspace(0, F/2-deltaF, N);
                else
                    deltaF = F/N;
                    w = linspace(0, F-deltaF, N);
                end
            end
            
            opts.w        = w(:);
            opts.range    = range;
            opts.Fs       = Fs;
            opts.N        = N;
            opts.freqspec = freqspec;
        end
        
        function [prefix, scale] = SIprefix(x)
            % convert x into an SI-prefix and a scale factor
            prefixList = {'y', 'z', 'a', 'f', 'p', 'n', '\mu', 'm', '', 'k', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y'};
            indx_none = find(strcmp(prefixList, ''));
            indx = floor(log10(x)/3) + indx_none;
            if indx>length(prefixList)
                indx = length(prefixList);
            elseif indx<=0
                indx = 1;
            end
            scale = 10^((indx - indx_none)*3);
            prefix = prefixList{indx};
        end
    end
end