%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This is the abstract-class for the ranging-engines.
% Description end

classdef RangingEngineAbstract < Util
    properties (Abstract)
        delta_F                                                             %[Hz]   frequency step size
    end
    
    properties
        L      = RangingEngineAbstract.default_L;                           % Height of the coveriance matrix - Controls the frequency domain smoothing, i.e. shape of the Hankel
        Report = RangingEngineAbstract.default_Report;                      % Placeholder to store internal data to be used after method-call
    end
    properties (Abstract, Constant)
        SupportsMultiAntenna                                                % Indicates whether multi-antenna input is supported
    end
   
    properties (Hidden, Constant)
       default_L       = 40;
       default_delta_F = 1e6;
       default_Report  = struct('Likeliness', NaN);
    end
    
    methods (Abstract)
        [D, AoA] = do(self, varargin);
    end
    methods
        function defaults(self)
            self.L      = RangingEngineAbstract.default_L;                  % Height of the coveriance matrix - Controls the frequency domain smoothing, i.e. shape of the Hankel
            self.Report = RangingEngineAbstract.default_Report;
        end
        
        function [Cov]=GetCov(self,H_est)
            Cov=zeros(self.L,self.L,class(H_est));
            for cnt=1:size(H_est,1)
                Cov=Cov+H_2_Cov(H_est(cnt,:),self.L);%Result is Hermitian
            end
            %Cov=Cov/norm(Cov);
            if self.DoForwardBackward
                T=eye(size(Cov));
                T=T(:,end:-1:1);
                Cov=Cov+T*conj(Cov)*T;
                Cov=Cov/2;
            end
        end
        function plot_pseudospectrum(self, varargin)
            if ~isfield(self.Report,'w')
                fprintf('Report is still empty, please run do-function first\n');
            else
                p = inputParser; % Axes, PlotLikeliness, ActualDistance
                p.addParameter('PlotLikeliness', false, @(x) islogical(x));
                p.addParameter('ActualDistance', [], @(x) isnumeric(x));
                p.addParameter('DistanceOffset', 0, @(x) isnumeric(x));
                p.addParameter('Axes', []);
                p.parse(varargin{:});
                PlotLikeliness = p.Results.PlotLikeliness;
                ActualDistance = p.Results.ActualDistance;
                DistanceOffset = p.Results.DistanceOffset;
                Axes = p.Results.Axes;

                if ~isempty(Axes)
                    if strcmpi(get(Axes, 'type'), 'figure')
                        figure(Axes); hold on; grid on;
                        Hax = gca;
                    else
                        Hax = Axes;
                        hold(Hax, 'on');
                        grid(Hax, 'on');
%                        axes(Axes); hold on; grid on;
                    end
                else
                    figure; hold on; grid on;
                    Hax = gca;
                end
                SpeedOfLight = physconst('LightSpeed');

                distance = self.Report.w/(2*pi*self.delta_F)*SpeedOfLight - DistanceOffset;
                dist_peak = self.Report.omega/(2*pi*self.delta_F)*SpeedOfLight - DistanceOffset;
                plot(Hax, distance, log10(1./self.Report.s), 'DisplayName', 'Pseudospectrum');
                plot(Hax, dist_peak, log10(1./self.Report.Peak), 'ro', 'MarkerFaceColor', 'r', 'DisplayName', sprintf('Found distance (%.2fm)', dist_peak));
                plot(Hax, distance([1, end]), log10(self.MinLevel*self.L*[1,1]), 'k', 'linewidth', 1.5, 'DisplayName', 'Threshold');

                if PlotLikeliness
                    likeliness = zeros(1,length(self.Report.w));
                    for k=1:length(self.Report.w)
                        likeliness(k) = self.ComputeLikeliness(self.Report.w(k),self.Report.eigenvects,self.Report.EVs,trace(self.Report.Cov),self.Report.StartNSS);
                    end
                    plot(Hax, distance, likeliness, 'DisplayName', 'Likeliness');
                end

                if ~isempty(ActualDistance)
                    d_step = diff(distance(1:2));
                    w_idx  = ceil((ActualDistance+DistanceOffset)/d_step)+1;
                    plot(Hax, distance(w_idx), log10(1./self.Report.s(w_idx)), 'gs', 'MarkerFaceColor', 'g', 'DisplayName', sprintf('Actual distance (%.2fm)', ActualDistance));
                end

                xlabel(Hax, 'Distance [m]');
                ylabel(Hax, 'Pseudospectrum (log10)');
                legend(Hax, 'show', 'Location', 'SouthEast');
            end
        end
        
    end
    
    methods (Static)
        function f = fftvec(N_b, omega)
            %returns a column-vector if omega is a scalar, returns a matrix
            %if omage is a row-vector. ERROR if omega is a column-vector!
            f=exp(-1i*((0:N_b-1)'*omega));
        end

        function Likeliness=ComputeLikeliness(omega,eigenvects,EVs,TRACE,Ns)
            if iscolumn(EVs)
                EVs=EVs.';
            end
            L = size(eigenvects,1);     % Size of the covariance matrix (LxL)
            FFTMATRIX = RangingEngineAbstract.fftvec(L,omega);
            SigVects  = eigenvects(:,1:Ns).';
            P         = zeros(1,Ns);
            for cnt=1:Ns
                h = SigVects(cnt,:)*FFTMATRIX;
                P(cnt) = (abs(h).^2)/L;
            end
            t = P*EVs(1:Ns).';            
            Likeliness = t/TRACE;
        end
    end
end