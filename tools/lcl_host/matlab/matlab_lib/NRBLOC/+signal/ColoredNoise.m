%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This function generates 1/f^alpha-noise. There are implementations that
%   use an approach using parallel random stream, sampled at logorithmic
%   divided sample-rates (aka Voss, Gartner, Lorentzian sum). These
%   algorithms can only generate alpha=1 (thus 1/f) noise.
%   Other implementations use an approach of filtering a single random
%   stream. These methods are more flexible in the noise-type to generate.
%   The RC-method generates noise with 0<=alpha<=2 and the Kasdin-method
%   generates noise with -2<=alpha<2. The Kasdin method is also implemented
%   by Mathworks in dsp.ColoredNoise.
% Description end

classdef ColoredNoise < matlab.mixin.SetGet & matlab.mixin.Copyable
    properties
        method                                                              % Method to generate noise 'RC', 'Voss', 'VossMcCartney', 'VossTramell', 'Kasdin'
        ComplexNoise                                                        % Flag to indicate whether real (false) or complex (true) noise is expected
        powerUnit                                                           % Power-unit; must be: 'dBm/Hz'
        powerValue                                                          % Output power(density)
        powerRefFreq                                                        % (Hz) Reference frequency at which the output-power density value must be met
        impedance                                                           % (Ohm) Impedance
        % Only applicable for 'RC' and 'Kasdin'
        noiseColor                                                          % Noise color, any of: 'white', 'pink', 'brown', 'brownian', 'red', 'blue', 'purple', 'violet', 'custom'
        alpha                                                               % determines slope of the noise-psd; slope = alpha * -10dB/decade
        % Only applicable to 'RC'
%         f_low_ratio                                                         % () low cut-off ratio for filter used in RC-method; ratio of fs_internal (=fs/upsample_factor)
%         f_high_ratio                                                        % () high cut-off ratio for filter used in RC-method; ratio of fs_internal (=fs/upsample_factor)
        f_low                                                               % (Hz) low cut-off frequency for filter used in RC-method
        f_high                                                              % (Hz) high cut-off frequency for filter used in RC-method
        % Only applicable to any of the Voss methods and 'RC'
        nSections                                                           % Number of sections
        UpsampleFilter                                                      % Filter used to upsample the noise-signal
    end
    
    properties (AbortSet)       % MATLAB® does not set the property value if the new value is the same as the current value. MATLAB does not call the property set method, if one exists.
        upsample_factor                                                     % () upsample-factor of the internal signal
    end
    
    properties (Dependent)
        fs                                                                  % (Hz) Sample-rate of the output signal
    end
    
    properties (Dependent, Hidden)
        funcName
    end
    
    properties (SetAccess = protected)                                      % acces to this property from this class and any subclass
        filtObj                                                             % Filter to be used by method; must be either empty or a digfilt-object.
    end
    
    properties (Hidden, SetAccess = protected)                              % acces to this property from this class and any subclass
        fs_internal                                                         % (Hz) Sample-rate of the initial generated signal
        ColorSet = {'white', 'pink', 'brown', 'brownian', 'red', 'blue', 'purple', 'violet', 'custom'};
        PowerUnitsOpts = {'dBm/Hz'};
        MethodOpts = {'RC', 'Voss', 'VossMcCartney', 'VossTramell', 'Kasdin'};
        UpsampleFilter_init = [];
    end
    
    methods
        function set.upsample_factor(self, val) 
            % To avoid calling the design_upsample_filter function 
            % everytime the setter-function is called, this property has
            % the attribute 'AbortSet'
            validateattributes(val, {'numeric'}, {'real', 'positive', 'integer', 'scalar'}, 'set.upsample_factor', 'upsample_factor');
            self.upsample_factor = val;
            if isempty(self.UpsampleFilter_init) %#ok<MCSUP>
                self.design_upsample_filter();  % If you change the upsample_factor, we need to redesign the filter, unless the UpsampleFilter is specified during construction
            end
        end

        function set.method(self, val)
            S = sprintf('%s, ', string(self.MethodOpts));                   %#ok<MCSUP>
            assert(ismember(lower(val), lower(self.MethodOpts)), sprintf('method can be any of: {%s}, but is %s instead', S(1:end-2), val)); %#ok<MCSUP>
            self.method = lower(val);
        end
        
        function set.powerUnit(self, val)
            S = sprintf('%s, ', string(self.PowerUnitsOpts));               %#ok<MCSUP>
            assert(ismember(lower(val), lower(self.PowerUnitsOpts)), sprintf('powerUnit can be any of: {%s}, but is %s instead', S(1:end-2), val)); %#ok<MCSUP>
            self.powerUnit = lower(val);
        end
        
        function set.noiseColor(self, val)
            S = sprintf('%s, ', string(self.ColorSet));                     %#ok<MCSUP>
            assert(ismember(lower(val), lower(self.ColorSet)), sprintf('noiseColor can be any of: {%s}, but is %s instead', S(1:end-2), val)); %#ok<MCSUP>
            self.noiseColor = lower(val);
        end
        
        function set.powerRefFreq(self, val)
            self.powerRefFreq = val;
        end

        function val = get.fs(self)
           val = self.fs_internal * self.upsample_factor; 
        end
        
        function set.fs(self, val)
            self.fs_internal = val / self.upsample_factor;
        end
        
        function funcname = get.funcName(self)
            switch lower(self.method)
                case {'rc', 'kasdin'}
                    funcname = 'method_rc_kasdin_super';
                case {'voss', 'vossmccartney'}
                    funcname = 'voss_super';
                case 'vosstramell'
                    funcname = 'vosstramell';
            end
        end
       
        function self = ColoredNoise(varargin)
            p = inputParser;
%             p.KeepUnmatched = true;
            p.addParameter('method', 'RC');                                 % Check will be done by set-method
            p.addParameter('powerValue', 0, @(x) validateattributes(x, {'numeric'}, {'real', 'scalar'}, mfilename));
            p.addParameter('ComplexNoise', true, @(x) validateattributes(x, {'logical'}, {'scalar'}, mfilename));
            p.addParameter('powerUnit', 'dBm/Hz');                          % Check will be done by set-method
            p.addParameter('powerRefFreq', 100, @(x) validateattributes(x, {'numeric'}, {'scalar'}, mfilename, 'RefFreq_Hz_in'));
            p.addParameter('impedance', 50, @(x) validateattributes(x, {'numeric'}, {'real', 'scalar'}, mfilename));
            p.addParameter('nSections', 5, @(x) validateattributes(x, {'numeric'}, {'real', 'positive', 'integer', 'scalar'}, mfilename, 'NumberOfSections'));
            p.addParameter('noiseColor', 'custom');                         % Check will be done by set-method
            p.addParameter('alpha', 0, @(x) validateattributes(x, {'numeric'}, {'real', 'scalar'}, mfilename, 'alpha'));
            p.addParameter('f_low', 50);                                    % Check will be done by set-method
            p.addParameter('f_high', 1e5);                                  % Check will be done by set-method
            p.addParameter('upsample_factor', 1);                           % Check will be done by set-method
            p.addParameter('UpsampleFilter', [], @(x) validateattributes(x, {'digfilt.digfiltabstract'}, {'scalar'}, mfilename, 'UpsampleFilter'));
            p.addParameter('fs', 1e6, @(x) validateattributes(x, {'numeric'}, {'real', 'positive', 'scalar'}, mfilename, 'fs'));
            p.parse(varargin{:});
            
            % Dynamically assign inputparser-parameters to properties
            FN = fieldnames(p.Results);
            for k=1:length(FN)
                if ~ismember(FN{k}, {'fs', 'powerRefFreq', 'upsample_factor'})
                    self.(FN{k}) = p.Results.(FN{k});
                end
            end
            self.fs_internal  = p.Results.fs / p.Results.upsample_factor;   % Outside the above for-loop, otherwise the argument checking in the set-method will not work
            self.upsample_factor = p.Results.upsample_factor;
            self.powerRefFreq = p.Results.powerRefFreq;                     % Outside the above for-loop, otherwise the argument checking in the set-method will not work
            
            self.create_filtobj();
            
            if isempty(p.Results.UpsampleFilter)
                self.design_upsample_filter();
            else
                self.UpsampleFilter = p.Results.UpsampleFilter;
            end
        end
        
        function reset(self)
            self.filtObj.reset();
            self.UpsampleFilter.reset();
        end
        
        function x = do(self, L, varargin)
            % Check properties
            if  self.is_not_white_nor_filter_method()
                % We need to check the parameters when the noise is NOT
                % white AND when a filter-method is used.
                validateattributes(self.powerRefFreq, {'numeric'}, {'real', 'positive', 'scalar', '>=', self.f_low, '<=', self.f_high}, 'do', 'powerRefFreq');
            end
            validateattributes(self.powerRefFreq, {'numeric'}, {'real', 'positive', 'scalar', '<=', self.fs}, 'do', 'powerRefFreq');
            if ismember(lower(self.method), {'kasdin', 'rc'})   % Using a filter-method
%                 validateattributes(self.fs, {'numeric'}, {'real', 'positive', 'scalar', '>=', 2*self.f_high*self.upsample_factor}, 'do', 'fs');
                validateattributes(self.f_high, {'numeric'}, {'real', 'positive', 'scalar', '<=', self.fs/(2*self.upsample_factor)}, 'do', 'f_high');
                validateattributes(self.f_low, {'numeric'}, {'real', 'positive', 'scalar', '<', self.f_high}, 'do', 'f_low');
            end           
            
            if self.powerValue == -Inf
                x = zeros(1, L);
                x = Signal_BB(x, 0, 1/self.fs, self.impedance, 0);
                return
            else
                if self.upsample_factor>1
                    assert(L/self.upsample_factor == round(L/self.upsample_factor), sprintf('When the upsample-factor > 1, the number of output samples must be an integer multiple of the upsample-factor (currently L=%d and I=%d).', L, self.upsample_factor));
                end
                L_noise = L/self.upsample_factor;
                x = self.(self.funcName)(L_noise);                          % Generate noise with 'method'
                x = x(:).';                                                 % Make sure it is a row-vector
                x = Signal_BB(x, 0, 1/self.fs_internal, self.impedance, 0);
                % figure; hold on;
                % pwelch(x, 'NFFT', 2^13, 'freqrange', 'centered', 'plotUnitsY', 'dBm');
                x = set_deltaT(x, 1/self.fs, 'filter', self.UpsampleFilter);% Interpolate to output sample-rate
                x = x(1:L);
                x = self.set_power(x);
            end
        end
        
        function design_upsample_filter(self)
            if self.upsample_factor == 1
                self.UpsampleFilter = digfilt.FIRFilter(1);
            else
                if license('checkout', 'signal_blocks') && license('checkout', 'signal_toolbox')
                    % The function below will design a multistage FIR interpolator
                    Fs    = self.fs;
                    Fpass = self.f_high/(Fs/2);
                    Fstop = 0.95/self.upsample_factor;      % fs/(2*upsample_factor)
                    Apass = 0.1;    % (dB) passband ripple
                    Astop = 60;     % (dB) stopband attenuation

                    assert(Fstop > Fpass, sprintf('Fst is expected to be larger than Fp, however Fstop=%.3eHz and Fpass=%.3eHz', Fstop, Fpass))

                    D = fdesign.interpolator(self.upsample_factor,'lowpass','Fp,Fst,Ap,Ast',Fpass,Fstop,Apass,Astop);
                    hm = design(D,'multistage', 'SystemObject', false);
                    
                    % Convert hm into a digfilt-object
                    if isa(hm, 'mfilt.firinterp')
                        self.UpsampleFilter = digfilt.FIRInterpolator(hm.InterpolationFactor, hm.Numerator);
                    else    % class(H) == 'mfilt.cascade'
                        for k=1:length(hm.Stage)
                            H(k) = digfilt.FIRInterpolator(hm.Stage(k).InterpolationFactor, hm.Stage(k).Numerator); %#ok<AGROW>
                        end
                        self.UpsampleFilter = digfilt.FilterCascade(H);
                    end                    
%                     fvtool(hm, 'Fs', self.fs)
%                     fvtool(self.Upsample, 'Fs', self.fs)
                else
                    assert('The licence for the signal-processing blockset and/or toolbox could not be checked out, please specify an alternative for designing a upsampling-filter or specify the filter directly through the constructor of this class.');
                end
            end
        end
        
        function y = set_power(self, x, varargin)
            switch lower(self.powerUnit)
                case 'dbm/hz'
                    imp_dB  = self.pow2db(self.impedance);
                    filt_dB = self.filter_offset();
                    up_rate = self.pow2db(self.upsample_factor);
                    offset  = -30 + imp_dB - filt_dB + self.powerValue + up_rate;
                    y       = x * self.db2vrms(offset);
                otherwise
                    error('The powerUnit %s is not supported.', self.powerUnit);
            end
        end
        
        function offset_dB = filter_offset(self)
            [out1,~]  = self.freq_response('w', self.powerRefFreq);
            offset_dB = self.vrms2db(abs(out1));
        end
        
        function Spectrum = PSD_closed_form(self,freq)
            Spectrum  = self.freq_response('w',freq);
            offset_db = self.filter_offset();
            A         = self.db2vrms(self.powerValue - offset_db);
            Spectrum  = abs(A*Spectrum).^2;
        end
        
        function x = voss_super(self, L)
            % This function follows the description as discussed in:
            % * https://www.dsprelated.com/showarticle/908.php
            % * http://www.firstpr.com.au/dsp/pink-noise/#Stochastic_Voss_McCartney
            % Difference between the (modified) Voss-algorithms:
            % * Voss (as described by Gardner); no white-noise component, regular grid
            %   for updates, updates happen at same time-instance
            % * Voss-McCartney; white-noise component, regular grid
            %   for updates, updates happen at successive time-instances.
            % * Voss-Tramell; white-noise component, randomized grid
            %   for updates.
            % This function implements the Voss- and Voss-McCartney-algorithm.
            
            %TODO: make it work with any arbitrary length
            assert(ispower2(L), sprintf('Length must be a power of 2, but is %d instead', L));
            assert(log2(L)>=self.nSections, sprintf('This method only works when 2^number-of-sections (2^%d=%d) is larger or equal then the length of input (%d)', self.nSections, 2^self.nSections, L))
            
            switch lower(self.method)
                case 'voss'
                    dec_factor = 2.^(1:self.nSections);
                case 'vossmccartney'
                    dec_factor = 2.^(0:self.nSections-1);
            end
            
            x = zeros(self.nSections,L);
            for k=1:self.nSections
                if self.ComplexNoise
                    temp = 1/sqrt(2) * complex(randn(L/dec_factor(k),1), randn(L/dec_factor(k),1));
                else
                    temp = randn(L/dec_factor(k),1);
                end
                dup = repmat(temp.', dec_factor(k), 1);                     % All samples are copied 2^(k) times
                x(k,:) = dup(:).';
            end
            x = sum(x,1);
        end
        
        function x = vosstramell(self, L)
            % This function follows the description as discussed in:
            % * https://www.dsprelated.com/showarticle/908.php
            % * http://www.firstpr.com.au/dsp/pink-noise/#Stochastic_Voss_McCartney
            % Difference between the (modified) Voss-algorithms:
            % * Voss (as described by Gardner); no white-noise component, regular grid
            %   for updates, updates happen at same time-instance
            % * Voss-McCartney; white-noise component, regular grid
            %   for updates, updates happen at successive time-instances.
            % * Voss-Tramell; white-noise component, randomized grid
            %   for updates.
            % This function implements the Voss-Tramell algorithm.
            x    = zeros(1,L);
            if self.ComplexNoise
                Rows = 1/sqrt(2) * complex(randn(self.nSections,1), randn(self.nSections,1));
            else
                Rows = randn(self.nSections,1);
            end
            x(1) = sum(Rows);
            
            for n=2:1:L
                index_rows = floor(log(rand(1,1))/log(1-0.5))+1;
                % index_rows = geornd(0.5)+1;
                if index_rows<= self.nSections
                    if self.ComplexNoise
                        Rows(index_rows) = 1/sqrt(2) * complex(randn(1,1), randn(1,1));
                    else
                        Rows(index_rows) = randn(1,1);
                    end
                end
                x(n) = sum(Rows);
            end
        end
        
        function x = method_rc_kasdin_super(self, L)
            if self.ComplexNoise
                temp = randn(2,L);          % This is done is this way, to ensure that when processing in blocks, the same noise is generated as when it is done in 1 large stream
                x = 1/sqrt(2) * complex(temp(1,:), temp(2,:))  * sqrt(self.fs);
            else
                x = randn(L,1) * sqrt(self.fs);
            end
            x = self.filtObj.filter(x);
        end
        
        function create_filtobj(self)
            switch self.noiseColor
                case 'white'
                    Alpha = 0;
                case 'pink'
                    Alpha = 1;
                case {'brown', 'brownian', 'red'}
                    Alpha = 2;
                case 'blue'
                    Alpha = -1;
                case {'purple', 'violet'}
                    Alpha = -2;
                case 'custom'
                    Alpha = self.alpha;        % !! Do not use the name alpha, since it is a built-in Matlab function !!
            end
            
            switch lower(self.method)
                case 'rc'
                % RC accepts all alpha between 0 and 2
                alpha_partly  = mod(Alpha, 2+2*eps(1));
                nStages       = round((Alpha - alpha_partly)/2);
                self.filtObj  = signal.ColoredNoise.rc('NumberOfSections', self.nSections, ...
                    'f_low', self.f_low, ...
                    'f_high', self.f_high, ...
                    'fs', self.fs_internal, ...
                    'alpha', alpha_partly);
                case 'kasdin'
                    % Kasdin accepts all alpha between -2 and 2
                    if Alpha == 2
                        alpha_partly = 2;
                    else
                        alpha_partly = mod(Alpha+2, 4+2*eps(1)) - 2;
                    end
                    nStages      = round((Alpha - alpha_partly)/2);
                    self.filtObj = signal.ColoredNoise.kasdin('alpha', alpha_partly);
                otherwise
                    nStages = 0;
                    self.filtObj = [];
            end
            
            for k = 1:abs(nStages)
                if ~isa(self.filtObj, 'digfilt.FilterCascade')     % make sure it is a cascaded filter; Kasdin returns a single-stage filter, RC returns a single stage when alpha=1
                    self.filtObj = digfilt.FilterCascade(self.filtObj);
                end
                if nStages<0    % add differentiator
                    self.filtObj.addstage(digfilt.Differentiator);
                else            % add integrator
                    self.filtObj.addstage(digfilt.Integrator);
                end
            end
        end
        
        function varargout = freq_response(self,varargin)
            assert(~isempty(self.filtObj),[class(self) ': filter object not defined.'])
            
            G = digfilt.FilterCascade(self.filtObj, self.UpsampleFilter);
            [varargout{1:nargout}] = G.freq_response(varargin{:}, 'Fs', self.fs);
        end
        
        function val = is_not_white_nor_filter_method(self)
            % This function returns true is the noise is NOT
            % white AND when a filter-method is used.
            if ismember(lower(self.method), {'kasdin', 'rc'})                
                val = (strcmpi(self.noiseColor, 'custom') && self.alpha==0); 
                val = val || strcmpi(self.noiseColor, 'white');              
                val = ~val;
            else
                val = false;
            end
        end
    end
    
    methods (Static)
        function y = pow2db(x)
            y = 10*log10(x);
        end
        
        function y = vrms2db(x)
            y = 20*log10(x);
        end
        
        function y = db2pow(x)
            y = 10.^(x/10);
        end
        
        function y = db2vrms(x)
            y = 10.^(x/20);
        end
        
        function y = mean_power(x, impedance)
            y = (x(:)' * x(:)) / length(x) / impedance;
        end
        
        function FilterObj = rc(varargin)
            % This function follows "Generating long streams of 1/f^alpha noise" by
            % S.Paszczynski (http://hal.in2p3.fr/in2p3-00024797v3/document).
            
            % The paper of G. Corsini and R. Saletti "A 1_div_f^gamma power spectrum
            % noise sequence generator" from 1988, describes a similar method. To be
            % investigated whether that is much different from the here implemented approach.
            p = inputParser;
            p.KeepUnmatched = true;
            p.addParameter('NumberOfSections', 5, @(x) validateattributes(x, {'numeric'}, {'real', 'positive', 'integer', 'scalar'}, mfilename, 'NumberOfSections'));
            p.addParameter('f_low', 1e1, @(x) validateattributes(x, {'numeric'}, {'real', 'positive', 'scalar'}, mfilename, 'f_low'));
            p.addParameter('f_high', 1e5, @(x) validateattributes(x, {'numeric'}, {'real', 'positive', 'scalar'}, mfilename, 'f_high'));
            p.addParameter('fs', 1e6, @(x) validateattributes(x, {'numeric'}, {'real', 'positive', 'scalar'}, mfilename, 'fs'));
            p.addParameter('alpha', 0, @(x) validateattributes(x, {'numeric'}, {'real', 'scalar', 'nonnegative', '<=', 2, '>=', 0}, mfilename, 'alpha'));
            p.parse(varargin{:});
            N            = p.Results.NumberOfSections;
            Fs           = p.Results.fs;                                    % (Hz) Sample-frequency
            f_high       = 2*pi*p.Results.f_high;                           % (Hz) High cut-off frequency
            f_low        = 2*pi*p.Results.f_low;                            % (Hz) Low cut-off frequency
            Alpha        = p.Results.alpha;      % !! Do not use the name alpha, since it is a built-in Matlab function !!
            
            if Alpha == 0
                FilterObj = digfilt.FIRFilter(1);
            else
                % Calculate pole-positions
                delta_p = (log10(f_high) - log10(f_low)) / N;                               % Difference in pole locations; equation 35
                p = zeros(N,1);
                p(1) = 10^(log10(f_low) + 1/2*(1-Alpha/2) * delta_p);                       % First pole-location; equation 38
                for n=1:N-1
                    p(n+1) = 10^(log10(p(n)) + delta_p);                                    % Other pole-locations; equation 36
                end
                
                % Calculate zero-locations
                z = zeros(N,1);
                for n=1:N
                    z(n) = 10^(log10(p(n)) + Alpha/2 * delta_p);                             % Zero-locations; equation 37
                end
                
                % Translate the zero- and pole-locations into filter-coefficient using
                % first-order-sections (FOS).
                FilterObj = digfilt.FilterCascade();
                for k=1:length(p)
                    r0 = pi * p(k)/Fs;
                    r1 = pi * z(k)/Fs;
                    a0 = (1+r1)/(1+r0);                                                     % Equation 20
                    a1 = -(1-r1)/(1+r0);                                                    % Equation 20
                    b1 = -(1-r0)/(1+r0);                                                    % Equation 20
                    
                    norm = sum([1, b1]) / sum([a0, a1]);                                    % Normalize transfer function to have 0dB gain at 0Hz
                    FilterObj.addstage(digfilt.IIRFilter([a0, a1] * norm, [1, b1]));
                end
            end
        end
        
        function FilterObj = kasdin(varargin)
            % This method is described in "Discrete simulation of colored noise and
            % stochastic processes and 1_div_fa power law noise generation" by N.J.
            % Kasdin from 1995. The same method is also used by Mathworks in the
            % dsp.ColoredNoise generator.
            p = inputParser;
            p.KeepUnmatched = true;
            p.addParameter('alpha', 1, @(x) validateattributes(x, {'numeric'}, {'real', 'scalar', '>=', -2, '<=', 2}, mfilename, 'alpha'));
            p.parse(varargin{:});
            Alpha = p.Results.alpha;        % !! Do not use the name alpha, since it is a built-in Matlab function !!
            
            if Alpha > 0
                ARFilterLength = 64;
                % Use all-pole model for negative alpha
                if Alpha == 2      % Alpha = 2 yields an unstable filter
                    den = [1 -0.999];
                else
                    den = ones(1, ARFilterLength);
                    for idx = 2:ARFilterLength
                        den(idx) = (idx - 2 - Alpha/2) * den(idx-1) / (idx-1);
                    end
                end
                num = sum(den);
                FilterObj = digfilt.IIRFilter(num, den);
            elseif Alpha < 0
                MAFilterLength = 256;
                % Use all-zero model for positive alpha
                if Alpha == -2
                    % Optimize purple noise (remove all-zero coefficients)
                    num = [1 -1]/2;
                else
                    num = ones(1, MAFilterLength);
                    for idx = 2:MAFilterLength
                        num(idx) = (Alpha/2 + idx - 2) * num(idx-1)/(idx-1);
                    end
                    num = num / sum(num);
                end
                FilterObj = digfilt.FIRFilter(num);
            else % Alpha == 0
                FilterObj = digfilt.FIRFilter(1);
            end
        end
    end
end