%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   Ranging using the 2D Music approach, to obtain range and AoA information.
% Description end

classdef RangingEngine_Music_AoA < RangingEngineAbstract
    properties
        delta_F           = 1e6                                             %[Hz]   frequency step size
        La                = 1;                                              % Controls the spatial domain smoothing, i.e. shape of the Hankel Block-Hankel
        MaxDist           = 150;                                            %[m]    Maximum distance considered
        TOL               = 0.999;                                          % Tolerances used by evd_complex
        MethodSubspaceSep = 8;                                              % Can be 'AIC' or a fixed number.
        maxiter           = 200;                                            % maximum number of iterations before FirstPeak_PS stops.
        MinLevel          = 0.5;                                            % Criteria to start Nelder-Mead (should be between 0 and 1, where )
        NFFT              = 2048;                                           % Determines the step-size used by peak-search
        type              = 'left';                                         % Options are 'left', 'right' or 'left&right'
        antSpace          = 0.5;                                            % antenna spacing in the unit of wavelength
    end
    
    properties (Constant)
        SupportsMultiAntenna = true;                                        % Indicates whether multi-antenna input is supported
    end
    
    methods
        function defaults(self)
            defaults@RangingEngineAbstract(self);
            warning('This function is not yet implemented for this class');
        end

        function set.delta_F(self, val)
            assert(any(val == [0.5e6, 1e6, 2e6, 4e6]));
            self.delta_F = val;
        end
        
        function self = RangingEngine_Music_AoA(varargin)
            parser               = inputParser();
            parser.KeepUnmatched = true;
            parser.addOptional('TOL', 0.999, @(x) isnumeric(x));
            parser.addOptional('MinLevel', 0.5, @(x) isnumeric(x));
            parser.addOptional('type', 'left', @(x) any(strcmpi('left', {'left', 'right', 'left&right'})));
            parser.addOptional('La', 1, @(x) isnumeric(x));
            parser.parse(varargin{:})

            self.TOL = parser.Results.TOL;
            self.MinLevel = parser.Results.MinLevel;
            self.type = lower(parser.Results.type);
            self.La = parser.Results.La;
            
            if nargout==0
                self.SelfTest()
            end
            warning('off', 'signal:findpeaks:largeMinPeakHeight');
        end
        
        function delete(~)
            % Setting the state of 'on' is a bit dangerous, because it
            % might interfere with other functions that set the same
            % warning to off. Capturing the state in the constructor and
            % restoring it here, has the same effects. 
            % I believe the side-effects are very small in this project, 
            % therefore I just enable the warning again.
            warning('on', 'signal:findpeaks:largeMinPeakHeight');
        end
        
        function set.type(self, type)
            assert(any(strcmpi(type, {'left', 'right', 'left&right'})), sprintf('Type must be ''left'', ''right'' or ''left&right'', but is %s instead', type));
            self.type = lower(type);
        end
        
        function SelfTest(self)
            close all
            
            NrSims       = 20;

            true_distance=1*pi;
            True_angle=-2*pi/10;
            dx=physconst('lightspeed')/2.4395e9/2;
            Lz=-1.5:1:1.5;
            self.La=1;
            self.type='left&right';
            
            for cnt=1:NrSims
                D=true_distance-dx*Lz*sin(True_angle);
                taus=D/physconst('lightspeed');
                hs=ones(size(taus));
                f=2.4e9+(0:79)*1e6;
                H=zeros(4,numel(f));
                for cnt2=1:4
                    [H(cnt2,:)]=H_freq(taus(cnt2),hs(cnt2),f); 
                end
                
                RE=self;
                TYPES={'left&right'};%{'left','right','left&right'};
                figure(1);clf;
                for cnt_type=1:length(TYPES)
                    RE.type=TYPES{cnt_type};
                    [d_est,AOA]=do(RE,H);
                    
                    subplot(3,1,cnt_type);
                    mesh(RE.Report.Distances,RE.Report.angles,RE.Report.PseudoSpectrum)
                    hold on;
                    plot3(true_distance,True_angle,max(RE.Report.PseudoSpectrum(:)),'go')
                    plot3(d_est,AOA,max(RE.Report.PseudoSpectrum(:)),'r+')
                    view(0,90);
                    xlabel('Distance [m]');
                    ylabel('Angle [\times\pi rad]');
                    legend('Pseudo-spectrum', 'Actual', 'Estimated');
                    title(['Distance and angle estimation - ' RE.type]);
                end
                drawnow
                true_distance = true_distance+0.25;
                True_angle    = True_angle+2/180*pi;
            end
        end

        
        function [d, AOA] = do(self, CRout)
            H = CRout.Hest;
            Kf=size(H,2);           %Number of tones
            Ka=size(H,1);           %Number of antennae
            
            if Kf<self.L
                error('Too small number of frequency')
            end
            if Ka<self.La
                error('Too small number of antennae')
            end
            
            A=[];
            for cnt2=1:Ka
                temp = self.H_2_Hankel(H(cnt2,:),Kf-self.L);
                A=[A;temp];   %#ok<AGROW>
            end
            I1=1:(self.L*self.La);
            A2=[];
            for cnt2=0:Ka-self.La
                A2=[A2 A(I1+(self.L)*cnt2,:)];           %#ok<AGROW>
            end
            A=A2;clear A2;
            
            [U,S,V] = svd(A);
            if isvector(S)
                X  = S(1).^2;
            else
                X  = diag(S).^2;
            end
            Ns = find((cumsum(X)/sum(X))>self.TOL,1,'first');               % Number of sources
            Ns = min([Ns,self.MethodSubspaceSep, size(U,2), size(V,2)]);
            U=U(:,1:Ns);
            V=V(:,1:Ns);
            Distances = 0:1:self.MaxDist; 
            AoAs = (-90:2:90)/180*pi;
            P=zeros(numel(Distances),numel(AoAs));
            switch self.type
                case 'right'
                    %Use V, i.e. right-hand singular vector
                    La2=1:Ka-self.La+1;
                    F2=2*pi*self.delta_F*(0:Kf-self.L);
                    for cnt_tau=1:length(Distances)
                        for cnt_sin=1:length(AoAs)
                            P(cnt_tau,cnt_sin) = RangingEngine_Music_AoA.invPS([Distances(cnt_tau) AoAs(cnt_sin)],F2,La2,conj(V),self.antSpace);
                        end
                    end
                    E=size(V,1);
                case 'left'
                    %Use U, i.e. left-hand singular vector
                    La1=1:self.La;
                    F1=2*pi*self.delta_F*(0:self.L-1);
                    for cnt_tau=1:length(Distances)
                        for cnt_sin=1:length(AoAs)
                            P(cnt_tau,cnt_sin) = RangingEngine_Music_AoA.invPS([Distances(cnt_tau) AoAs(cnt_sin)],F1,La1,U,self.antSpace);
                        end
                    end
                    E=size(U,1);
                case 'left&right'
                    La2=1:Ka-self.La+1;
                    F2=2*pi*self.delta_F*(0:Kf-self.L);
                    La1=1:self.La;
                    F1=2*pi*self.delta_F*(0:self.L-1);
                    for cnt_tau=1:length(Distances)
                        for cnt_sin=1:length(AoAs)
                            P(cnt_tau,cnt_sin) = RangingEngine_Music_AoA.invPS2D([Distances(cnt_tau) AoAs(cnt_sin)],F1,La1,U,F2,La2,V,self.antSpace);%P(cnt_tau,cnt_sin)=invPS(obj,[taus(cnt_tau)*physconst('lightspeed') Sin_AoAs(cnt_sin)],F,La,Un);
                        end
                    end
                    E=size(U,1)*size(V,1);
                otherwise
                    error('unknown type')
            end
            tmp = E-min(P,[],2);
            [~,I] = findpeaks([0;tmp],'MinPeakHeight',(1-self.MinLevel)*E);

            if isempty(I)
                % It could be that the peak is at the border
                if tmp(1)>(1-self.MinLevel)*E
                    I = 1;
                elseif tmp(end)>(1-self.MinLevel)*E
                    I = length(tmp);
                end
            end
            
            if ~isempty(I)
                I=I-1;
                i=min(I);
                [~,j]=min(P(i,:));
                switch self.type
                    case 'right'
                        Solution = fminsearch(@(x) RangingEngine_Music_AoA.invPS(x,F2,La2,conj(V),self.antSpace),[Distances(i) AoAs(j)]);
                    case 'left'
                        Solution = fminsearch(@(x) RangingEngine_Music_AoA.invPS(x,F1,La1,U,self.antSpace),[Distances(i) AoAs(j)]);
                    case 'left&right'
                        Solution = fminsearch(@(x) RangingEngine_Music_AoA.invPS2D(x,F1,La1,U,F2,La2,V,self.antSpace),[Distances(i) AoAs(j)]);
                    otherwise
                        error('unknown type')
                end
                d   = Solution(1);
                AOA = Solution(2);

                if any(self.La == [1, Ka])
                    if self.La == 1
                        eigenvects = conj(U);                               % it is negative and conjugated with respect to EVD output in Music_emb
                    elseif self.La == Ka
                        eigenvects = V;
                    end
                    omega = d * (2*pi*self.delta_F) / physconst('lightspeed');
                    EVs = X;
                    sum_eigenvals = sum(X);
                    self.Report.Likeliness = self.ComputeLikeliness(omega,eigenvects,EVs,sum_eigenvals,Ns);
                else
                    self.Report.Likeliness = NaN;
                end
%fprintf('type = %s, La = %d\t U = %d x %d\t V = %d x %d \t => %.10f\n', self.type, self.La, size(U), size(V), self.Report.Likeliness);

            else
                d   = NaN;
                AOA = NaN;
            end
            self.Report.Distances      = Distances;
            self.Report.angles         = AoAs;
            self.Report.PseudoSpectrum = 1./P';
            self.Report.Ns             = Ns;

% figure;
% [X, Y] = meshgrid(Distances, AoAs);
% mesh(X.', Y.', P); hold on;
% plot3(Solution(1), Solution(2), RangingEngine_Music_AoA.invPS2D(Solution,F1,La1,U,F2,La2,V), 'r.', 'MarkerSize', 10);
% xlabel('Distances [m]');
% ylabel('Angles [rad]');
% zlabel('Inverse pseudospectrum');
        end
    end
    
    methods (Static)
        function x = s(tau, Sin_AoA, F, L)
            %Create the steering vector assuming half wavelength-spacing of
            %the elements
            x = exp(-1i*F*tau).'*exp(-1i*L*Sin_AoA);
            x = x(:);
        end
        
        function COST = invPS(X,F,L,U,antSpace)
            if nargin == 4
                antSpace = 1/2;
            end
            speed_of_light = physconst('lightspeed');
            sv   = exp(-1i*F*X(1)/speed_of_light).'*exp(2i*pi*antSpace*L*sin(X(2)));
            sv   = sv(:);
            COST = (length(sv)-sum(abs(sv'*U).^2));
        end
        
        function COST = invPS2D(X,F,L,U,F2,L2,V,antSpace)
            if nargin == 7
                antSpace = 1/2;
            end
            COST1 = RangingEngine_Music_AoA.invPS(X,F,L,U,antSpace);
            COST2 = RangingEngine_Music_AoA.invPS(X,F2,L2,conj(V),antSpace);
            COST  = 1./((1./COST1)*(1./COST2));
        end
        
        function H_hankel = H_2_Hankel(H_est,L)
            if nargin==0
                N=5;
                H_est=(randn(N,1)+1i*randn(N,1)).';
                L=3;
                H_hankel = H_2_Hankel(H_est,L-1);
            else
                N=size(H_est,2);
                Indices=(1:N-L)'*ones(1,L+1)+ones(N-L,1)*[0:L];
                H_hankel=H_est(Indices);                                    %Not identical, but equivalent to X2 = corrmtx(s2,2*k-1,'cov');
            end
        end
    end
end