%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   Ranging using MUSIC algorithm as described in
%   TN-17-WATS-TP2-198:?Proof of concept ranging platform?. It matches the
%   embedded implementation.
% Description end

classdef RangingEngine_Music_emb < RangingEngineAbstract
    properties % all the properties are public
        delta_F           = RangingEngineAbstract.default_delta_F;          %[Hz]   frequency step size
        FP                = RangingEngine_Music_emb.default_FP;             % Object used for eigen-value decomposition
        TOL               = RangingEngine_Music_emb.default_TOL;            % Tolerances used by evd_complex
        MethodSubspaceSep = RangingEngine_Music_emb.default_MethodSubspaceSep; % Can be 'AIC' or a fixed number.
        PS_approx         = RangingEngine_Music_emb.default_PS_approx;      %[m]  Stop criteria for the (first) peak-search in Pseudo-Spectrum
        maxiter           = RangingEngine_Music_emb.default_maxiter;        % maximum number of iterations before FirstPeak_PS stops.
        MinLevel          = RangingEngine_Music_emb.default_MinLevel;       % Criteria to start Nelder-Mead (should be between 0 and 1, where )
        InitPos           = RangingEngine_Music_emb.default_InitPos;        %[m] Start position used for peak-search
        NFFT              = RangingEngine_Music_emb.default_NFFT;           % Determines the step-size used by peak-search
        PLOT              = RangingEngine_Music_emb.default_PLOT;           % Do you want info in the command line on convergence behaviour
        DoForwardBackward = RangingEngine_Music_emb.default_DoForwardBackward;% Enable/disable forward/backward smoothing
    end
    
    properties (Constant)
        SupportsMultiAntenna = true;                                        % Indicates whether multi-antenna input is supported
    end
    
    properties (Hidden, Constant)
       default_FP = evd_complex_perfect(); 
       default_TOL = [1e-7 0.999 -inf];
       default_MethodSubspaceSep = 8;
       default_PS_approx = 0.005;
       default_maxiter = 200;
       default_MinLevel = 0.1;
       default_InitPos = 0;
       default_NFFT = 512;
       default_PLOT = false;
       default_DoForwardBackward = false;
    end
    
    methods
        function defaults(self)
            defaults@RangingEngineAbstract(self);
            self.delta_F           = RangingEngineAbstract.default_delta_F;          %[Hz]   frequency step size
            self.FP                = RangingEngine_Music_emb.default_FP;             % Object used for eigen-value decomposition
            self.TOL               = RangingEngine_Music_emb.default_TOL;            % Tolerances used by evd_complex
            self.MethodSubspaceSep = RangingEngine_Music_emb.default_MethodSubspaceSep; % Can be 'AIC' or a fixed number.
            self.PS_approx         = RangingEngine_Music_emb.default_PS_approx;      %[m]  Stop criteria for the (first) peak-search in Pseudo-Spectrum
            self.maxiter           = RangingEngine_Music_emb.default_maxiter;        % maximum number of iterations before FirstPeak_PS stops.
            self.MinLevel          = RangingEngine_Music_emb.default_MinLevel;       % Criteria to start Nelder-Mead (should be between 0 and 1, where )
            self.InitPos           = RangingEngine_Music_emb.default_InitPos;        %[m] Start position used for peak-search
            self.NFFT              = RangingEngine_Music_emb.default_NFFT;           % Determines the step-size used by peak-search
            self.PLOT              = RangingEngine_Music_emb.default_PLOT;           % Do you want info in the command line on convergence behaviour
            self.DoForwardBackward = RangingEngine_Music_emb.default_DoForwardBackward;% Enable/disable forward/backward smoothing
            self.Report            = RangingEngine_Music_emb.default_Report;
        end
        
        function self = RangingEngine_Music_emb(varargin)
            p = inputParser();
            p.KeepUnmatched = true;
            p.addOptional('delta_F', RangingEngine_Music_emb.default_delta_F, @(x) validateattributes(x, {'numeric'}, {'scalar', 'integer', '>=', 500e3, '<=' 4e6}, mfilename, 'delta_F', 1));
            p.addOptional('L', RangingEngineAbstract.default_L, @(x) validateattributes(x, {'numeric'}, {'scalar', 'integer', '>=', 1}, mfilename, 'L', 2));
            p.addOptional('NFFT', RangingEngine_Music_emb.default_NFFT, @(x) validateattributes(x, {'numeric'}, {'scalar', 'integer'}, mfilename, 'NFFT', 3));
            p.addParameter('EVDobj', RangingEngine_Music_emb.default_FP, @(x) validateattributes(x, {'evd_superclass'}, {'scalar'}, mfilename, 'EVDobj'))
            p.addParameter('TOL', RangingEngine_Music_emb.default_TOL, @(x) validateattributes(x, {'double'}, {'numel', 3}, mfilename, 'TOL'))
            p.addParameter('maxiter', RangingEngine_Music_emb.default_maxiter, @(x) validateattributes(x, {'numeric'}, {'real', 'positive', 'integer', 'scalar'}, mfilename, 'maxiter'));
            p.addParameter('MethodSubspaceSep', RangingEngine_Music_emb.default_MethodSubspaceSep, @(x) validateattributes(x, {'numeric'}, {'scalar', 'integer'}, mfilename, 'MethodSubspaceSep'))
            p.parse(varargin{:})
            
            self.delta_F           = p.Results.delta_F;
            self.L                 = p.Results.L;
            self.NFFT              = p.Results.NFFT;
            self.FP                = p.Results.EVDobj;
            self.TOL               = p.Results.TOL;
            self.maxiter           = p.Results.maxiter;
            self.MethodSubspaceSep = p.Results.MethodSubspaceSep;
        end
        
        function [Dist_est, AoA] = do(self, CRout, varargin)
            AoA = NaN;                                                      % Unused output
            Hest = CRout.Hest;
            Cov = GetCov(self,Hest);
            if any(isnan(Cov))
                self.Report.Peak       = NaN;
                self.Report.EVs        = NaN;
                self.Report.eigenvects = NaN;
                self.Report.omega      = NaN;
                self.Report.w          = NaN;
                self.Report.s          = NaN;
                self.Report.StartNSS   = NaN;
                self.Report.Cov        = NaN;
                Dist_est               = NaN;
            else
                [eigenvects,EVs] = self.FP.doRun(Cov, self.TOL(1), self.TOL(2), self.MethodSubspaceSep);
                Ns               = self.SubSpaceSep(EVs);
                [omega,y]        = self.FirstPeak_PS(eigenvects(:,1:Ns).');

                speed_of_light = physconst('lightspeed');
                Dist_est = omega/(2*pi*self.delta_F)*speed_of_light;
                
                %The code below is for evaluation/gather analytics only. DO NOT
                %IMPLEMENT!
                self.Report.Likeliness = self.ComputeLikeliness(omega,eigenvects,EVs,trace(Cov),Ns);
                self.Report.Peak       = 1./y;
                self.Report.EVs        = EVs;
                self.Report.eigenvects = eigenvects;
                self.Report.Cov        = Cov;
                self.Report.omega      = omega;
                NFFTx                  = 4*4096;
                omega                  = 2*pi*(0:NFFTx)/NFFTx;
                self.Report.w          = omega;
                self.Report.s          = 1./self.InvPS(eigenvects(:,1:Ns).',omega);
                self.Report.StartNSS   = Ns;
                self.Report.evd_error  = self.FP.REPORT.err;
            end
        end
        
        function Ns = SubSpaceSep(self,EVs)
            if isscalar(self.MethodSubspaceSep)
                Ns=min(length(EVs),self.MethodSubspaceSep);
                %TODO: the following if statement is not implemented (yet, 2019-08-13)
                %in the C++ implementation
                if numel(self.TOL)>2
                    I=find(diff(10*log10(EVs))< self.TOL(3),1,'first');
                    if ~isempty(I)
                        Ns=min(Ns,I);
                    end
                end
            elseif strcmp(self.MethodSubspaceSep,'AIC')
                AIC = zeros(length(EVs),1);
                for d=1:length(EVs)
                    AIC(d)=Ld(d,EVs);
                end
                [~,Ns] = min(AIC);
                Ns = Ns+1;
            else
            end
        end
        
        function  den = InvPS(self,SigVects,omega)
            %output is always a value in the range [0 self.L]
            FFTMATRIX = self.fftvec(self.L,omega);
            h=SigVects*FFTMATRIX;
            den=self.L-sum(abs(h).^2,1);
            if isfield(self.Report,'Nof_InvPS_calls')
                self.Report.Nof_InvPS_calls=self.Report.Nof_InvPS_calls+1;
            else
                self.Report.Nof_InvPS_calls=1;
            end
        end

        function [x, fval] = FirstPeak_PS(self,SigVects)
            speed_of_light = physconst('lightspeed');
            CONSTANT=(2*pi*self.delta_F)/speed_of_light;
            tolx=self.PS_approx*CONSTANT;
            
            v1 =  self.InitPos*(2*pi*self.delta_F)/speed_of_light ; % initial guess, could be zero..
            fv1 =  InvPS(self,SigVects,v1);
            step = 2*pi/self.NFFT;      %Initial step-size in forward (increasing omega direction).
            itercount = 1;
            IsUnderThresshold=false;
            while itercount < self.maxiter
                v2 = v1+step;
                fv2 = InvPS(self,SigVects,v2);
                if IsUnderThresshold==false
                    %First we step forwards until the InvPS =< self.MinLevel*self.L
                    how = 'initial forward stepping';
                    if fv2<self.MinLevel*self.L && fv2<fv1
                        %We have reached it
                        IsUnderThresshold=true;
                    end
                else
                    if fv2<fv1
                        %If the next point is better(lower) than the
                        %previous, we keep on stepping in the direction of
                        %step with same step-size
                        if self.PLOT
                            how = 'forward';
                        end
                    else
                        %If the next point is worse(higher in value) than the
                        %previous, we passed the minimum. We take the worse
                        %point, but change direction and reduce step-size
                        if self.PLOT
                            how = 'backward';
                        end
                        step=-step/2;
                    end
                end
                if abs(step) <= tolx
                    %If the step-size(abs(step)) is below tolx, we stop.
                    break
                else
                    fv1=fv2;
                    v1=v2;
                end
                if self.PLOT
                    fprintf(' %5.0f,   %12.6g,    %12.6g,        %s\n', itercount, v1/(2*pi*self.delta_F)*speed_of_light,  fv1, how)
                end
                itercount=itercount+1;
            end
            %We select the best (lowest in value)
            if itercount >= self.maxiter
                x=NaN;
                fval=NaN;
            elseif fv1>fv2
                x=fv2;
                fval=v2;
            else
                x = v1;
                fval = fv1;
            end
            self.Report.itercount=itercount;
        end
        

    end
end