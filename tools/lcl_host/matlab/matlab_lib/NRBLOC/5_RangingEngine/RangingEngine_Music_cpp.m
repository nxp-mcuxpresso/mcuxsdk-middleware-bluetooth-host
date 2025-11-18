%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   Ranging using MUSIC algorithm as described in
%   TN-17-WATS-TP2-198:�Proof of concept ranging platform�. It matches the
%   embedded implementation.
% Description end

classdef RangingEngine_Music_cpp < RangingEngine_Music_emb
    properties
        evd_type = RangingEngine_Music_cpp.default_evd_type;                % EVD type; 0 = power-interations, 1 = SAES
    end
        
    properties (Hidden, Constant)
       default_evd_type = 1;                                                % EVD type; 0 = power-interations, 1 = SAES
    end
    
    methods
        function defaults(self)
            defaults@RangingEngine_Music_emb(self);
            self.evd_type = RangingEngine_Music_cpp.default_evd_type;       % EVD type; 0 = power-interations, 1 = SAES
        end
        
        function self = RangingEngine_Music_cpp(varargin)
            self@RangingEngine_Music_emb(varargin{:});
            p = inputParser();
            p.KeepUnmatched = true;
            p.addParameter('evd_type', RangingEngine_Music_cpp.default_evd_type, @(x) validateattributes(x, {'numeric'}, {'scalar', 'integer', '>=', 0, '<=' 3}, mfilename, 'evd_type'));
            p.parse(varargin{:})
            
            self.evd_type = p.Results.evd_type;
        end
        
        function [Dist_est, AoA] = do(self, CRout)
            Hest = CRout.Hest;            
            AoA = NaN;                                                      % Unused output
            [Dist_est, EigenVects, EigenVals] = mexMusic(self.delta_F, self.L, Hest, self.TOL(1), self.TOL(2), self.MethodSubspaceSep, self.PS_approx, self.maxiter, self.MinLevel, self.InitPos, self.NFFT, self.evd_type);
                      
            omega = Dist_est * (2*pi*self.delta_F) / physconst('lightspeed');
            Cov = GetCov(self, Hest);
            Ns  = self.SubSpaceSep(EigenVals);
            self.Report.Likeliness = self.ComputeLikeliness(omega,EigenVects,EigenVals,trace(Cov),Ns);
            self.Report.Peak       = 1./self.InvPS(EigenVects(:,1:Ns).', omega);
            self.Report.EVs        = EigenVals;
            self.Report.eigenvects = EigenVects;
            self.Report.Cov        = Cov;
            self.Report.omega      = omega;
            NFFTx                  = 4*4096;
            omega                  = 2*pi*(0:NFFTx)/NFFTx;
            self.Report.w          = omega;
            self.Report.s          = 1./self.InvPS(EigenVects(:,1:Ns).',omega);
            self.Report.StartNSS   = Ns;
            self.Report.evd_error  = evd_superclass.performance(EigenVects, EigenVals, Cov);
        end
    end
end
