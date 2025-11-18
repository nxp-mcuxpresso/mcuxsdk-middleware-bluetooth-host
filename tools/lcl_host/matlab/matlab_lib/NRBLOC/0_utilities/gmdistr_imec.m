%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   Gaussian mixture model, whihc is simpler than the one implemented in matlab and does not require any toolbox.
% Description end

classdef gmdistr_imec
    
    %  gmdistr_imec Gaussian mixture distribution class.
    %   An object of the gmdistr_imec class defines a 1 dimensional Gaussian mixture
    %   distribution, which is a distribution that consists of a
    %   mixture of one or more Gaussian distribution components.  Each
    %   Gaussian component is defined by its mean and std-dev, and
    %   the mixture is defined by a vector of weights.
    %
    %   To fit a Gaussian mixture distribution model to data, use FITGMDIST.
    %
    %   GMDISTRIBUTION properties:
    %   A Gaussian mixture distribution with K components, in 1 dimension, has
    %   the following properties:
    %      NumComponents         - Number of mixture components.
    %      w                     - weight/Mixing proportion of each component.
    %      mu                    - vector containing component mean.
    %      sigma                 - vector containing component std-dev
    %   gmdistr_imec methods:
    %      cdf            - CDF for the Gaussian mixture distribution.
    %      gmdistr_imec   - Create a Gaussian mixture model.
    %      pdf            - PDF for Gaussian mixture distribution.
    %      rand           - Random numbers from Gaussian mixture distribution
    %      gmfit          - fit gmdistr_imec to data
    %   See also GMDISTRIBUTION, FITGMDIST.
    properties
        w=[]
        mu = [];          % NumComponents-by-NumVariables array for means
        sigma = [];       % standard deviation of the components
    end
    properties (Dependent)
        NComponents = 0;  % number of mixture components
    end
    methods
        function obj = gmdistr_imec(mu, sigma, p)
            if nargin==0
                %warning('gmdistr_imec: TooFewInputs');
                
                %                p       = [0.5325 0.3937 0.0738];
                %                mu      = [0.1504 0.6165 2.5103];
                %                 %mu      = [0.9504 0.9165 2.5103];
                %                sigma   = [0.1673 0.2287 1.5524];
                %                 %sigma   = [0.3673 0.8287 1.5524];
                %
                
                                 p       = [0.7 0.3];
                                 mu      = [1.5 4.5];
                                 sigma   = [0.5 0.5];
                
                %
                % '8_shape_HVpolarized_FW153_no_interference'
                %                  p       = [0.8599,0.1154,0.0247];
                %                  mu      = [1.9342,3.3582,-0.3473];
                %                  sigma   = [0.1141,1.4655,1.0791];
                
                % '8_shape_HVpolarized_VNA'
                %                  p        =[0.4142 0.0234 0.5624];
                %                  mu       =[1.5149 4.0240 1.1140];
                %                  sigma    =[0.1432 3.1260 0.0657];
                
            elseif nargin < 2
                error('gmdistr_imec: TooFewInputs');
            end
            
            
            [k,d] = size(mu);
            if k>1
                if d>1
                    error(message('mu must be a vector'));
                else
                    mu=mu(:)';sigma=sigma(:)';p=p(:)';
                end
            end
            if sum(size(mu)~=size(sigma))
                error(message('mu, sigma and P must have the same size'));
            end
            if sum(size(mu)~=size(p))
                error(message('mu, sigma and p must have the same size'));
            end
            
            
            if ~ismatrix(mu) || ~isnumeric(mu)
                error('gmdistr_imec: Check mu is numeric and is matrix');
            end
            
            if length(mu) ~= length(unique(mu))
                error('Gaussian mixture components with the same mean is not valid');
            end
            
            if sum(p==0)>0
                
                I=find(p~=0);
                p = p(I);
                mu = mu(I);
                sigma = sigma(I);
                
                if isempty(p)
                    p = [];
                    mu = [];
                    sigma = [];
                elseif  any(p < 0)
                    error(message('p must contain positive values'));
                elseif abs(sum(p)- 1)>1e-10
                    error(message('sum(p) must must be unit'));
                end
            end
            
            obj.mu = mu;
            obj.sigma = sigma;
            obj.w=p;
        end % constructor
        
        function value=CRLB(obj)
            h=min(sqrt(obj.sigma))/40;
            Minrange=min(obj.mu-20*squeeze(sqrt(obj.sigma)));
            Maxrange=max(obj.mu+20*squeeze(sqrt(obj.sigma)));
            bins=Minrange:h:Maxrange;
            LogPDF= logpdf(obj,bins.');
            Der2LogPDF=(LogPDF(1:end-2)+LogPDF(3:end)-2*LogPDF(2:end-1))/h^2;
            BinsDer=bins(2:end-1);
            
            F=-trapz(BinsDer,pdf(obj,BinsDer.').*Der2LogPDF);
            value=1/F;
        end
        function value = get.NComponents(obj)
            value=numel(obj.mu);
        end
        
        function y=pdf(obj,x)
            y=zeros(size(x));
            for cnt=1:obj.NComponents
                y =y + obj.w(cnt) * obj.norm_density(x, obj.mu(cnt), obj.sigma(cnt));
            end
        end
        function y=logpdf(obj,x)
            y=zeros(size(x,1),obj.NComponents);
            for cnt=1:obj.NComponents
                y(:,cnt) = log(obj.w(cnt))-log((obj.sigma(cnt)*sqrt(2*pi))) -(x-obj.mu(cnt)).^2 / (2*obj.sigma(cnt)^2) ;
            end
            y=log(sum(exp(y),2));
            %y=max(y.').';
        end
        function y=maxlogpdf(obj,x)
            y=zeros(size(x,1),obj.NComponents);
            for cnt=1:obj.NComponents
                y(:,cnt) = log(obj.w(cnt))-log((obj.sigma(cnt)*sqrt(2*pi))) -(x-obj.mu(cnt)).^2 / (2*obj.sigma(cnt)^2) ;
            end
            %y=log(sum(exp(y),2));
            y=max(y.').';
        end
        function y=cdf(obj,x)
            y=zeros(size(x));
            for cnt=1:obj.NComponents
                y= y+obj.w(cnt) * obj.norm_cdf(x, obj.mu(cnt), obj.sigma(cnt));
            end
        end
        function values=rand(obj,N)
            if isscalar(N) & obj.NComponents~=0
                uniform_RV=rand(N,1);
                edges=cumsum(obj.w(1:end-1));
                I=sum(uniform_RV*ones(1,obj.NComponents-1)>ones(N,1)*edges,2)+1;
                mu_d = obj.mu(I);
                sigma_d =  obj.sigma(I);
                values = mu_d + sigma_d .* randn(1,N);
            elseif isscalar(N) & obj.NComponents==0
                values = zeros(1, N);
            elseif numel(N)==2
                for cnt=1:N(1)
                    values(cnt,:)=rand(obj,N(2));
                end
            end
        end
        
        function values=randAnch(obj,N)
            
            mutot=sum(obj.w.*obj.mu);
            sigmatot=sqrt(sum(obj.w.*(obj.mu.^2+obj.sigma.^2))-mutot.^2);
            
            if isscalar(N)
                
                values=inf*randn(1,N);
                
                while (sum(values>ones(1,N)*(mutot+sigmatot))) | (sum(values<ones(1,N)*(mutot-sigmatot))) % mimics the case where there is more trust on measurments between anchors
                    uniform_RV=rand(N,1);
                    edges=cumsum(obj.w(1:end-1));
                    I=sum(uniform_RV*ones(1,obj.NComponents-1)>ones(N,1)*edges,2)+1;
                    mu_d = obj.mu(I);
                    sigma_d =  obj.sigma(I);
                    values = mu_d + sigma_d .* randn(1,N);
                end
                
            elseif numel(N)==2
                for cnt=1:N(1)
                    values(cnt,:)=randAnch(obj,N(2));
                end
            end
        end
        
        
    end
    methods
        function [obj_est]=test(obj,NofValues)
            if nargin<2
                NofValues=1e3;
            end
            
            % Test out GAUSSIAN_MIXTURE_MODEL
            x = min(obj.mu-3*obj.sigma):min(obj.sigma)/4:max(obj.mu+3*obj.sigma);
            % generate a set of data using default parameters
            values=rand(obj,NofValues);
            % now estimate the parametes of the distribution
            [obj_est, difference] = gmfit(obj,values, obj.NComponents, 1.0e-4);
            
            y=pdf(obj,x);
            % compare empirical data to estimated distribution
            y_est=pdf(obj_est,x);
            
            figure(3)
            hold off
            plot1 = plot(x, y, 'b-', 'linewidth', 2);
            hold on
            plot2 = plot(x, y_est, 'r--', 'linewidth', 2);
            set(gca,'linewidth', 2, 'fontsize', 16)
            legend([plot1 plot2], 'mixture model', 'estimated model', ...
                'Location', 'NorthEast');
            
            set(gcf, 'PaperPosition', [0 0 5 3]);
            
            figure(4);clf
            semilogy(difference)
            figure(5);clf
            plot(sort(values),(1:length(values))/length(values))
            hold on
            plot(x,cdf(obj,x))
            plot(x,cdf(obj_est,x))
            legend('CDF sim','CDF true','CDF est')
            xlim([min(x) max(x)])
        end
        function [MemProb]=classify(obj,values)
            %1) Given the old parameter estimates, we can compute for
            %each value the membership probabilities(MP)
            % E step: soft classification of the values into one of the mixtures
            C=obj.NComponents;
            MemProb=zeros(C,numel(values));
            for j=1:C
                MemProb(j, :) = obj.w(j) * obj.norm_density(values, obj.mu(j), obj.sigma(j));
            end
            % normalize
            MemProb = MemProb ./ repmat(sum(MemProb), C, 1);
        end
        function [obj]=ML_estimate(obj,values,MemProb)
            % M step: ML estimate the parameters of each class (i.e., p, mu, sigma)
            for j=1:size(MemProb,1)
                obj.mu(j)    = sum( MemProb(j,:).*values ) / sum(MemProb(j,:));
                obj.sigma(j) = sqrt( sum(MemProb(j,:).*(values - obj.mu(j)).^2) /  sum(MemProb(j,:)) );
                obj.w(j)     = mean(MemProb(j,:));
            end
            if sum(obj.sigma==0)>0
                I=find(obj.sigma~=0);
                obj.mu=obj.mu(I);
                obj.sigma=obj.sigma(I);
                obj.w=obj.w(I)/sum(obj.w(I));
            end
        end
        
        
        function [obj_est, FIT] = ...
                gmfit(obj,values, C, epsilon,ind_init,maxIter,minIter)
            % Estimate the parameters of a 1D Gaussian Mixture model using EM.
            %
            % file:      	gaussian_mixture_model.m, (c) Matthew Roughan,  2009
            % created: 	July 20, 2009
            % author:  	Matthew Roughan
            % email:   	matthew.roughan@adelaide.edu.au
            %
            %   Inputs:
            %      values  = row vector of the observed values. Only works for 1D data
            %      C       = number of components (mixtures), which should be fairly small
            %      epsilon = precision for convergence
            %
            %   Outputs:
            %      mu_est    = vectors means of each class
            %      sigma_est = vector of standard deviations of each class
            %      p_est     = class membership probability estimates
            %      counter   = number of iterations required
            %      difference= total absolute difference in parameters at each iteration to get
            %                  an idea of convergence rate
            %
            %   A Gaussian mixture model means that each data point is drawn (randomly) from one of C
            %   classes of data, with probability p_i of being drawn from class i, and each class is
            %   distributed as a Gaussian with mean standard deviation mu_i and sigma_i.
            %
            %   For this code purpose, all distributions are 1D cause thats all we need.
            %
            %   The algorithm used here for estimation is EM (Expectation Maximization). Simply put, if
            %   we knew the class of each of the N input data points, we could separate them, and use
            %   Maximum Likelihood to estimate the parameters of each class. This is the M step. The E
            %   step makes (soft) choises of (unknown) classes for each of the data points based on the
            %   previous round of parameter estimates for each class.
            %
            %   References: Estimating Gaussian Mixture Densities with EM, Carlo Tomasi, Duke University
            %               http://en.wikipedia.org/wiki/Mixture_model
            %
            values=values(:)';
            if sum(isnan(values))>0
                disp('warning:removing NaNs')
                values=values(~isnan(values));
            end
            
            % get and check inputs
            N = length(values);
            if (nargin < 4)
                epsilon = 1.0e-4;
            end
            if (nargin < 5)
                ind_init = 1;
            end
            if (nargin < 6)
                maxIter = 25000;
            end
            if (nargin < 7)
                minIter = 50;
            end
            
            if ind_init
                % initialize
                counter=obj.NComponents;
                mu_est=obj.mu;
                sigma_est=obj.sigma;
                p_est = obj.w;
            else
                % initialize
                counter = 2;
                mu_est = 2*mean(values) * sort(rand(C,1));
                sigma_est = ones(C,1)*std(values);
                p_est = ones(C,1)/C;
            end
            
            %obj_init=gmdistr_imec(mu_est, sigma_est, p_est);
            obj_est=gmdistr_imec(mu_est, sigma_est, p_est);
            FIT(1) = inf;
            FIT(2) = DistributionFit(obj_est,values);
            % now iterate
            while ( abs(FIT(end-1)-FIT(end)) >= epsilon & counter < maxIter & ~sum(isnan(obj_est.w)))|counter<minIter
                % [mu_est, sigma_est, p_est]
                
                %1) Given the old parameter estimates, we can compute for
                %each value the membership probabilities(MP)
                % E step: soft classification of the values into one of the mixtures
                obj_start=obj_est;
                [MemProb]=classify(obj_start,values);
                % M step: ML estimate the parameters of each class (i.e., p, mu, sigma)
                [obj_est]=ML_estimate(obj_est,values,MemProb);
                if isnan(obj_est.w)
                    obj_est=obj_start;
                end
                %difference(counter+1)= sum(abs(obj_start.mu - obj_est.mu)) + ...
                %    sum(abs(obj_start.sigma - obj_est.sigma)) + ...
                %    sum(abs(obj_start.w - obj_est.w));
                FIT(counter+1) = DistributionFit(obj_est,values);
                counter = counter + 1;
            end
            FIT=FIT(end);
            
        end
        function diff = DistributionFit(GM,values)
            y=(0:numel(values)-1)/numel(values);
            x=sort(values);
            y2=cdf(GM,x);
            diff=rms(y-y2);
            %             figure(101);
            %             plot(x,y);hold on;plot(x,y2);
        end
        
    end
    
    methods(Hidden = true, Static = true)
        function p = norm_density(x, mu, sigma)
            p = exp( -(x-mu).^2 / (2*sigma^2) ) / (sigma * sqrt(2*pi) );
        end
        function P = norm_cdf(x, mu, sigma)
            z=(x-mu)/sigma;
            P = 0.5 * erfc(-z ./ sqrt(2));
        end
    end
    
end % classdef


