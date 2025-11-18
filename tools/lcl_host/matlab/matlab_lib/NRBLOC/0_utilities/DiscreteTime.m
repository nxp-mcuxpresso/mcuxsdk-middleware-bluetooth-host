%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
classdef DiscreteTime
    % Description
    %   The class is used to represent discrete-time signals, bot contineous amplitudes and discrete amplitudes
    %   This class is used as parent-class by the classes DiscreteTime and Signal_BB
    %   See also Signal_BB, TwoComplement
    % Description end
    
    properties
        signal      % The property will contain the vector representing the discrete signal 
        deltaT = 1  % The sampling period in [sec], Default=1[sec]
        Start  = 0  % Absolute time of the start of the packet[sec], Default=0[sec]
    end
    
    methods
        function t=time(DT)
            t=DT.Start+(0:(numel(DT.signal)-1))*DT.deltaT;
            t=t(:);
        end
        function DT_out=timegate(DT,minT,maxT)
            t=time(DT);
            I(1)=find(t>=minT,1,'first');
            I(2)=find(t<=maxT,1,'last');
            DT_out=DT;
            signaltmp=DT_out.signal;
            DT_out.signal=signaltmp(I(1):I(2));
            DT_out.Start=t(I(1));
        end
        
        function [a,b]=MinMaxValues(DT)
            a=NaN;b=NaN;
        end
        function plot(DT,varargin)
            ScalingFactor=1e3;unit='millisec';
            t=time(DT);
            t=t*ScalingFactor;
            if ~isreal(DT.signal)
                A(1)=subplot(2,1,1);
            end
            
            [a,b]=MinMaxValues(DT);
            plot(t,real(DT.signal(:)),varargin{:});hold on
            hdl(1)=plot([t(1) t(end)],[a a],varargin{:});
            hdl(2)=plot([t(1) t(end)],[b b],varargin{:});
            set(hdl,'LineStyle','--')
            xlabel(['time[' unit ']'])
            ylabel('')
            if ~isreal(DT.signal)
                title('real')
                A(2)=subplot(2,1,2);
                plot(t,imag(DT.signal(:)),varargin{:});
                title('imag')
                xlabel(['time[' unit ']'])
                ylabel('')
                hold on
                hdl(1)=plot([t(1) t(end)],[a a],'k--');
                hdl(2)=plot([t(1) t(end)],[b b],'k--');
                set(hdl,'LineStyle','--')
                linkaxes(A,'x');
            end
        end
        
        function varargout = psd(self, varargin)
            warning('Please use pwelch instead of psd. This function may be removed in the future.');
            varargout{:} = pwelch(self, varargin{:});
        end
        
        function varargout = pwelch(CV, varargin)
            p = inputParser;
            p.addParameter('NFFT', [], @(x) validateattributes(x, {'numeric'}, {'real', 'positive', 'integer', 'scalar'}, mfilename));
            p.addParameter('WindowType', @hann, @(x) isa(x, 'function_handle'));
            p.addParameter('WindowOpts', {}, @(x) iscell(x));
            p.addParameter('FreqRange', 'twosided', @(x) any(strcmpi(x, {'onesided', 'twosided', 'centered'})));
            p.addParameter('Overlap', 0, @(x) validateattributes(x, {'numeric'}, {'real', 'integer', 'scalar'}, mfilename));
            p.addParameter('plotUnitsX', 'kHz', @(x) any(strcmpi(x, {'Hz', 'kHz', 'MHz', 'GHz', 'THz'})));
            p.addParameter('plotUnitsY', 'A^2', @(x) any(strcmpi(x, {'dBW', 'dBmW', 'dBm', 'dBc', 'A^2'})));
            p.addParameter('plotOpts', {}, @(x) any(strcmpi(x, {'dBW', 'dBmW', 'dBm'})));
            p.addParameter('useMethod', 'auto', @(x) any(strcmpi(x, {'auto', 'mathworks', 'imec'})));  % select the desired method
            p.parse(varargin{:});
            NFFT = p.Results.NFFT;
            if isempty(NFFT)
                NFFT = min(floor([2^12 numel(CV.signal)]/2))*2;
            end
            overlap = p.Results.Overlap;
            if isempty(overlap)
               overlap = round(NFFT/2); % 50% overlap 
            end
            WindowType = p.Results.WindowType;
            WindowOpts = p.Results.WindowOpts;
            FreqRange   = p.Results.FreqRange;
            plotUnitsX = p.Results.plotUnitsX;
            plotUnitsY = p.Results.plotUnitsY;
            useMethod  = p.Results.useMethod;
            
            switch lower(plotUnitsX)
                case 'hz'
                    x_unit_scaling = 1;
                case 'khz'
                    x_unit_scaling = 1e3;
                case 'mhz'
                    x_unit_scaling = 1e6;
                case 'ghz'
                    x_unit_scaling = 1e9;
                case 'thz'
                    x_unit_scaling = 1e12;
            end
            
            switch lower(plotUnitsY)
                case {'dbw', 'dbc', 'a^2'}
                    dataScaling = 1;
                case {'dbmw', 'dbm', 'dbm/hz'}
                    dataScaling = 1000;
            end

            if strcmpi(useMethod, 'auto')
                useMathworks = license('test','Signal_Toolbox');
            elseif strcmpi(useMethod, 'mathworks')
                useMathworks = true;
            else
                useMathworks = false;
            end
            
            x = double(CV.signal);
            fs = 1/CV.deltaT;
            if useMathworks
                freqvect  = [];
                [data, f] = pwelch(x, window(WindowType, NFFT, WindowOpts{:}), overlap, freqvect, fs, FreqRange);
            else
                if strcmpi(FreqRange, 'onesided') && ~isreal(x)
                    error('DiscreteTime:ComplexInputOneSided', 'A "onesided" PSD cannot be computed for a complex signal.');
                end
                
                % Mapping of matlab-function (using signal-processing
                % toolbox) and Imec-alternative function
                wind.hann           = @signal.hann;                         %#ok<PROPLC>
                wind.hamming        = @signal.hamming;                      %#ok<PROPLC>
                wind.blackman       = @signal.blackman;                     %#ok<PROPLC>
                wind.blackmanharris = @signal.blackmanharris;               %#ok<PROPLC>
                wind.rectwin        = @signal.rectwin;                      %#ok<PROPLC>
                wind.tukeywin       = @signal.tukeywin;                     %#ok<PROPLC>
                FN = fieldnames(wind);

                % Check input
                assert(ismember(lower(func2str(WindowType)), lower(FN)), sprintf('Window-type %s is not supported when no Signal-Processing toolbox license is available. Please use any of {%s, %s, %s, %s, %s, %s}', func2str(WindowType), FN{1}, FN{2}, FN{3}, FN{4}, FN{5}, FN{6}));
                assert(ispower2(NFFT), 'NFFT must be a power of 2 (when the Signal-Process toolbox is not available)');
                
                % Calculate window-coefficients
                WindType = wind.(lower(func2str(WindowType)));              % rename window-function handle
                w        = feval(WindType, NFFT, WindowOpts{:}).';
                
                % Calculate indices of blocks
                nBlocks     = floor((length(x) - overlap ) / (NFFT - overlap));
                startIndx   = (NFFT-overlap)*((0:nBlocks-1).');
                I           = startIndx*ones(1,NFFT) + ones(nBlocks,1)*(1:NFFT);

                % Divide into blocks and apply window
                x_windowed  = repmat(w, nBlocks, 1) .* x(I);
                
                % Calculate periodogram
                pgram = fft(x_windowed, NFFT, 2);
                
                % Average squared magnitude of periodogram
                data        = mean(abs(pgram).^2) / sum(w.^2)  / fs;
                switch FreqRange
                    case 'twosided'
                        f    = (0:NFFT-1).'/NFFT * fs;
                    case 'onesided'
                        f    = (0:NFFT/2).'/NFFT * fs;
                        data = [data(1), data(2:NFFT/2)*2, data(NFFT/2+1)];                        
                    case 'centered'
                        f    = (-NFFT/2:NFFT/2-1).'/NFFT*fs;
                        data = fftshift(data);
                        
                        % This is needed to make the output exactly the
                        % same as for the mathworks implementation of
                        % pwelch
                        f = [f(2:end); -f(1)];
                        data = [data(2:end), conj(data(1))];
                end
            end
 
            data = data(:) * dataScaling/CV.Impedance;
            if nargout<2
                %[~,unit,ScalingFactor]=freq2str(max(abs(f)));
                ScalingFactor = x_unit_scaling;
                hdl           = plot(f/ScalingFactor,10*log10(data));
                xlabel(['Frequency [' plotUnitsX ']']);
                ylabel(['PSD [' plotUnitsY '/Hz]']);
                grid on
                title(['Centre Freq = ' freq2str(CV.fc_Hz) ])
                if nargout==1
                    varargout{1} = hdl;
                end
            elseif nargout==2
                varargout{1} = f;
                varargout{2} = data;
            end
        end
        
%         function varargout = psd(DT,In2,NFFT)
%             if nargin<3
%                 NFFT=min(floor([2^12 numel(DT.signal)]/2))*2;
%             end
%             h = spectrum.welch('hann',NFFT);    % Create a Welch spectral estimator.
%             Hpsd = psd(h,double(DT.signal),'NFFT',NFFT,'Fs',1/DT.deltaT,'SpectrumType','twosided');             % Calculate the PSD
%             
%             I=[NFFT/2+1:NFFT 1:NFFT/2].';
%             f=Hpsd.Frequencies(I);
%             f=f-(I>NFFT/2)/DT.deltaT;
%             data=Hpsd.data(I);
%             if nargout==0
%                 %[~,unit,ScalingFactor]=freq2str(max(abs(f)));
%                 ScalingFactor=1e3;unit='kHz'
%                 
%                 
%                 if (nargin>1)&ischar(In2)
%                     plot(f/ScalingFactor,10*log10(data),In2)
%                 else
%                      plot(f/ScalingFactor,10*log10(data))
%                 end
%                 xlabel(['freq [' unit ']'])
%                 ylabel('PSD [A^2/Hz]')
% 
%                 grid on
%                 if isprop(DT,'fc_Hz')
%                     title(['Centre Freq=' freq2str(DT.fc_Hz) ])
%                 end
%                 if isprop(DT,'BitWidth')
%                     QN_pw_dB=10*log10(1/12);
%                     QN_pw_dBpHz=QN_pw_dB+10*log10(DT.deltaT)+3*(~isreal(DT.signal));
%                     hold on
%                     plot(([f(1) f(end)])/ScalingFactor,[1 1]*QN_pw_dBpHz,'r-')
%                 end
%             else
%                 varargout{1}=f;
%                 varargout{2}=data;
%             end
%         end

        function CV_out=zeros(CV,N,M)
            CV_out=copy(CV);
            CV_out.signal=zeros(N,M);
        end
        function Out=transpose(In)
            Out=copy(In);
            Out.signal=transpose(Out.signal);
        end
        function Out=ctranspose(In)
            Out=copy(In);
            Out.signal=ctranspose(Out.signal);
        end
        function CV=times(In1,In2)
            if isa(In1,'DiscreteTime')
                A=In1; B=In2;
            else
                A=In2; B=In1;
            end
            if isa(B,'DiscreteTime')%Multiplication of two DiscreteTime
                B=makeequal(A,B);
                CV=copy(A);
                CV.signal=B.signal.*A.signal;
            else
                if numel(B)==1
                    CV=copy(A);
                    CV.signal=B*A.signal;
                elseif isequal(size(A.signal),size(B))
                    CV=copy(A);
                    CV.signal=A.signal.*B;
                else
                    error('DiscreteTime error:One of both inputs should be a scalar or both should be vectors of equal length')
                end
            end
        end
        function SZ=size(CV,varargin)
            SZ=size(CV.signal,varargin{:});
        end
        function B = subsref(CV,I)
            if numel(I)>1
                error('Unsupported use of subsref')
            end
            switch I.type
                case '()'
                    B=CV;
                    I_tmp=I.subs{1};
                    B.signal=CV.signal(I_tmp);
                    if ~isempty(I_tmp)
                        B.Start=B.deltaT*(I_tmp(1)-1)+CV.Start;
                    end
                case '.'
%                     str2eval=['B=CV.' I.subs ';'];
%                     eval(str2eval);
                    B=CV.(I.subs);
                otherwise
                    error('Unsupported use of subsref')
            end
        end
        function C=horzcat(A,B)
            if isempty(A)
                C=B;
            elseif isempty(B)
                C=A;
            else
                C=copy(A);
                C.signal=[A.signal B.signal];
            end
        end
        function C=vertcat(A,B)
            if isempty(A)
                C=B;
            elseif isempty(B)
                C=A;
            else
                C=copy(A);
                C.signal=[A.signal;B.signal];
            end
        end
        function N=end(A,k,n)
            N=numel(A.signal);
        end
        function N=length(CV)
            N=max(size(CV.signal));
        end
         function varargout = disp(obj,DisplayLevel)
            if nargin==1
                DisplayLevel=0;
            end
            Offset='';
            for cnt=1:DisplayLevel
                Offset=[Offset '   '];
            end
            
            % Return object after modification
            AllFields=fields(obj);
            TEXT=[Offset '[' class(obj) ']:\n'];
            for cnt=1:length(AllFields)
                if eval(['isobject(obj.' AllFields{cnt} ')'])
                    TEXT=[TEXT eval(['disp(obj.' AllFields{cnt} ',' num2str(DisplayLevel+1)  ')']) ];
                elseif isstruct(eval(['obj.' AllFields{cnt} ]))
                    TEXT=['   ' Offset '{' AllFields{cnt} '}:\n'];
                    TEXT=[TEXT eval(['Struct2str(obj,obj.' AllFields{cnt} ',' num2str(DisplayLevel+1)  ')']) ];
                elseif isscalar(eval(['obj.' AllFields{cnt} ]))
                    TEXT=[TEXT '   ' Offset AllFields{cnt} '=' eval(['num2str(' ['obj.' AllFields{cnt} ] ')']) ];
                elseif isnumeric(eval(['obj.' AllFields{cnt} ]))
                    SIZETXT=[ eval(['num2str(size(' ['obj.' AllFields{cnt} ] ',1))']) 'x' eval(['num2str(size(' ['obj.' AllFields{cnt} ] ',2))'])];
                    TEXT=[TEXT '   ' Offset AllFields{cnt} '=' SIZETXT ];
                elseif isempty(eval(['obj.' AllFields{cnt} ]))
                    TEXT=[TEXT '   ' Offset AllFields{cnt} '=[]' ];
                elseif ischar(eval(['obj.' AllFields{cnt} ]))
                    TEXT=[TEXT '   ' Offset AllFields{cnt} '=''' eval(['obj.' AllFields{cnt} ]) '''' ];
                else
                    TEXT=[TEXT '   ' Offset AllFields{cnt}  ];
                end
                if cnt< length(AllFields)
                    TEXT=[TEXT '\n' ];
                end
            end
            
            
            if nargout==0
                fprintf([TEXT '\n']);
            elseif nargout==1
                varargout(1) = {TEXT};
            else
                error('only 1 output argument possible')
                
            end
        end %disp
        
    end
    
    methods (Static)
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