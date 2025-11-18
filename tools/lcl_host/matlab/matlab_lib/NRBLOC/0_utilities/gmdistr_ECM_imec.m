%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   Gaussian mixture model, whihc is simpler than the one implemented in matlab and does not require any toolbox.
% Description end

classdef gmdistr_ECM_imec
    
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
        function obj = gmdistr_ECM_imec(mu, sigma, p)
            if nargin==0
                warning('gmdistr_imec: TooFewInputs');
%                 p       = [0.5325 0.3937 0.0738];
%                 mu      = [0.1504 0.6165 2.5103];
%                 %mu      = [0.9504 0.9165 2.5103];
%                 sigma   = [0.1673 0.2287 1.5524];
%                 %sigma   = [0.3673 0.8287 1.5524];
%                 
%                 %p       = [0.8 1-0.8];
%                 %mu      = [0.1504 2];
%                 %sigma   = [0.1673 1.5524];

% '8_shape_HVpolarized_FW153_no_interference'
%                 p       = [0.8599,0.1154,0.0247];
%                 mu      = [1.9342,3.3582,-0.3473];
%                 sigma   = [0.1141,1.4655,1.0791];

% '8_shape_HVpolarized_VNA'
                 p        =[0.0235,0.5423,0.4342];
                 mu       =[4.0152,1.1089,1.5025];
                 sigma    =[3.1282,0.0642,0.1435];
                 
            elseif nargin < 2
                error('gmdistr_imec: TooFewInputs');
            end
            if ~ismatrix(mu) || ~isnumeric(mu)
                error('gmdistr_imec:Bad mu');
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
            if any(p < 0)
%                error(message('p must contain positive values'));
%            elseif abs(sum(p)- 1)>1e-10
%                error(message('sum(p) must must be unit'));
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

        function y=logpdf_ECM(obj,x)
            y=zeros(size(x,1),obj.NComponents);
            for cnt=1:obj.NComponents
                y(:,cnt) = log(obj.w(cnt))-log((obj.sigma(cnt)*sqrt(2*pi))) -(x-obj.mu(cnt)).^2 / (2*obj.sigma(cnt)^2) ;
            end
            %y=exp(y);
            %y=max(y.').';
        end
        
        function y=LS_pdf_ECM(obj,x)
            y=zeros(size(x,1),obj.NComponents);
            for cnt=1:obj.NComponents
                y(:,cnt) = ((x-obj.mu(cnt)).^2)./(obj.sigma(cnt)^2+eps.^2);
                %y(:,cnt) = ((x-obj.mu(cnt)).^2)./(obj.sigma(cnt)^2+eps.^2);
            end
            %y=exp(y);
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
            if isscalar(N)
                uniform_RV=rand(N,1);
                edges=cumsum(obj.w(1:end-1));
                I=sum(uniform_RV*ones(1,obj.NComponents-1)>ones(N,1)*edges,2)+1;
                mu_d = obj.mu(I);
                sigma_d =  obj.sigma(I);
                values = mu_d + sigma_d .* randn(1,N);
            elseif numel(N)==2
                for cnt=1:N(1)
                    values(cnt,:)=rand(obj,N(2));
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
            
            if C==1
                MemProb(j, :)=1;
            end    
                
            if C>1
                
                if sum(sum(MemProb)==0)>0
                    index=find(sum(MemProb)==0);
                    for i=1:numel(index)
                        temp=obj.norm_density_arg(values(index(i)), obj.mu, obj.sigma).*(obj.w~=0);
                        MemProb(:, index(i))=((temp==max(temp)).*(obj.w~=0)).';
                    end
                end
                % normalize
                de_nom=repmat(sum(MemProb), C, 1);
                ind1=find(de_nom~=0);
                MemProb(ind1) = MemProb(ind1) ./ de_nom(ind1);
            end
            
        end
        function [obj,K_comp]=ML_estimate(obj,values,MemProb,wth,sigth,ind_red)
            % M step: ML estimate the parameters of each class (i.e., p, mu, sigma)
            K_comp=0;
            for j=1:size(MemProb,1)
                obj.mu(j)    = sum(MemProb(j,:).*values )/(sum(MemProb(j,:))+eps);
                obj.sigma(j) = sqrt(sum(MemProb(j,:).*(values - obj.mu(j)).^2) /  (sum(MemProb(j,:))+eps) );
                if obj.NComponents~=1
                    obj.w(j)     = mean(MemProb(j,:));
                else
                    obj.w(j)=1;
                end
            end
            
            if sum(obj.w<=wth)>0 & obj.NComponents>1
                if ind_red
                 I=find(obj.w>wth);
                  obj.mu=obj.mu(I);
                  obj.sigma=obj.sigma(I);
                  obj.w=obj.w(I)/sum(obj.w(I));
                else
                K_comp=1;
                end
             elseif sum(obj.sigma<=sigth)>0 & obj.NComponents>1
                 if ind_red
                  I=find(obj.sigma>sigth);
                  obj.mu=obj.mu(I);
                  obj.sigma=obj.sigma(I);
                  obj.w=obj.w(I)/sum(obj.w(I));
                 else
                  K_comp=1;
                 end
            end

        end
        function [obj,K_comp]=ML_estimate_UGM(obj,values,MemProb)
            K_comp=0;
            % M step: ML estimate the parameters of each class (i.e., p, mu, sigma)
            for j=1:size(MemProb,1)
                obj.mu(j)    = sum( MemProb(j,:).*values ) / (sum(MemProb(j,:))+eps);
                obj.sigma(j) = sqrt( sum(MemProb(j,:).*(values - obj.mu(j)).^2) /  (sum(MemProb(j,:))+eps) );
                if obj.NComponents~=1
                    obj.w(j)     = mean(MemProb(j,:));
                else
                    obj.w(j)=1;
                end
            end
            if sum(obj.w<=2e-6)>0 & obj.NComponents>1
                  I=find(obj.w>2e-6);
                  obj.mu=obj.mu(I);
                  obj.sigma=obj.sigma(I);
                  obj.w=obj.w(I)/sum(obj.w(I));
                %K_comp=1;
             elseif sum(obj.sigma<=1e-3)>0 & obj.NComponents>1
                  I=find(obj.sigma>1e-3);
                  obj.mu=obj.mu(I);
                  obj.sigma=obj.sigma(I);
                  obj.w=obj.w(I)/sum(obj.w(I));
                %K_comp=1;
            end
        end 
        
        function [obj]=ML_estimate_MGMP(obj,values,MemProb)
            % M step: ML estimate the parameters of each class (i.e., p, mu, sigma)
            for j=1:size(MemProb,1)
                
                obj.mu(j)    = sum( MemProb(j,:).*values ) / (sum(MemProb(j,:))+eps);
                obj.sigma(j) = sqrt( sum(MemProb(j,:).*(values - obj.mu(j)).^2) /  (sum(MemProb(j,:))+eps) );
%               obj.sigma(j)
%               sqrt( sum(MemProb(j,:).*(values).^2) /  sum(MemProb(j,:)) -(obj.mu(j)).^2 ) 

                if obj.NComponents~=1
                    obj.w(j)     = mean(MemProb(j,:));
                else
                    obj.w(j)=1;
                end
            end
            
            
        end        
 
        function [obj,K_comp]=ML_estimate_multi(obj,values,MemProb,wth,sigth,ind_red,ind_adap_com)
            K_comp=0;
            % M step: ML estimate the parameters of each class (i.e., p, mu, sigma)
            for j=1:size(MemProb,1)
                
                obj.mu(j)    = sum( MemProb(j,:).*values ) / (sum(MemProb(j,:))+eps);
                obj.sigma(j) = sqrt( sum(MemProb(j,:).*(values - obj.mu(j)).^2) /  (sum(MemProb(j,:))+eps) );
%               obj.sigma(j)
%               sqrt( sum(MemProb(j,:).*(values).^2) /  sum(MemProb(j,:)) -(obj.mu(j)).^2 ) 

                if obj.NComponents~=1
                    obj.w(j)     = mean(MemProb(j,:));
                else
                    obj.w(j)=1;
                end
            end
            
            
            if ~ind_adap_com
                
                
                if sum(obj.w<=wth)>0 & obj.NComponents>1
                    if ind_red
                        I=find(obj.w>wth);
                        obj.mu=obj.mu(I);
                        obj.sigma=obj.sigma(I);
                        obj.w=obj.w(I)/sum(obj.w(I));
                    else
                        K_comp=1;
                    end
                elseif sum(obj.sigma<=sigth)>0 & obj.NComponents>1
                    if ind_red
                        I=find(obj.sigma>sigth);
                        obj.mu=obj.mu(I);
                        obj.sigma=obj.sigma(I);
                        obj.w=obj.w(I)/sum(obj.w(I));
                    else
                        K_comp=1;
                    end
                end
                
            end
        end
        
        function [comp_ind]=GM_compare_new(obj1,obj2)
            
            if obj1.NComponents~=obj2.NComponents
                error(message('The number of components for GM is not the same!'))
            end
            
            if obj1.NComponents==1
                
                comp_ind=[1;1];
                
            else
                
                Qtemp=zeros(1,obj1.NComponents);
                
                if (obj1.w(1)>obj1.w(2))
                    %                if ~(obj1.w(1)==0)
                    obj1temp=gmdistr_ECM_imec(obj1.mu(1), obj1.sigma(1), obj1.w(1));
                    %obj1temp=gmdistr_ECM_imec(obj1.mu(1), obj1.sigma(1), 1);
                    ind_temp=1;
                else
                    obj1temp=gmdistr_ECM_imec(obj1.mu(2), obj1.sigma(2), obj1.w(2));
                    %obj1temp=gmdistr_ECM_imec(obj1.mu(2), obj1.sigma(2), 1);
                    ind_temp=0;
                end
                
                
                for i=1:obj2.NComponents
                    obj2temp=gmdistr_ECM_imec(obj2.mu(i), obj2.sigma(i), obj2.w(i));
                    %obj2temp=gmdistr_ECM_imec(obj2.mu(i), obj2.sigma(i), 1);
                    Qtemp(i)=KL_div(obj1temp,obj2temp);
                    %Qtemp(i)=Jensen_Shannon_div(obj1temp,obj2temp);
                end
                
                if abs(abs(Qtemp(1))-abs(Qtemp(2)))<0.05
                    indtemp=min(abs(obj1temp.mu-obj2.mu))==abs(obj1temp.mu-obj2.mu(1));
                    if ind_temp
                        if indtemp
                             comp_ind=[1 2;1 2];
                        else
                            comp_ind=[1 2;2 1];
                        end
                    else
                        if indtemp
                            comp_ind=[1 2;2 1];
                        else
                            comp_ind=[1 2;1 2];
                        end
                    end
                else
                    
                    if abs(Qtemp(1))<abs(Qtemp(2))
                        if ind_temp
                            comp_ind=[1 2;1 2];
                        else
                            comp_ind=[1 2;2 1];
                        end
                    else
                        if ind_temp
                            comp_ind=[1 2;2 1];
                        else
                            comp_ind=[1 2;1 2];
                        end
                    end
                    
                end
                
            end
            
        end
        
        
        function [comp_ind]=GM_compare_new1(obj1,obj2) % comparing the pairing with similar means
            
            if obj1.NComponents~=obj2.NComponents
                error(message('The number of components for GM is not the same!'))
            end
            
            if obj1.NComponents==1
                
                comp_ind=[1;1];
                
            else
                
                if (obj1.w(1)>obj1.w(2))
                    %if ~(obj1.w(1)==0)
                    obj1temp=gmdistr_ECM_imec(obj1.mu(1), obj1.sigma(1), obj1.w(1));
                    %obj1temp=gmdistr_ECM_imec(obj1.mu(1), obj1.sigma(1), 1);
                    ind_temp=1;
                else
                    obj1temp=gmdistr_ECM_imec(obj1.mu(2), obj1.sigma(2), obj1.w(2));
                    %obj1temp=gmdistr_ECM_imec(obj1.mu(2), obj1.sigma(2), 1);
                    ind_temp=0;
                end
                
                if obj2.w(1)==0
                    if ind_temp
                     comp_ind=[1 2;2 1];  
                    else
                     comp_ind=[1 2;1 2];     
                    end
                elseif obj2.w(2)==0
                    if ind_temp
                       comp_ind=[1 2;1 2];      
                    else
                       comp_ind=[1 2;2 1]; 
                    end
                else
                    
                    indtemp=min(abs(obj1temp.mu-obj2.mu))==abs(obj1temp.mu-obj2.mu(1));
                    if ind_temp
                        if indtemp
                            comp_ind=[1 2;1 2];
                        else
                            comp_ind=[1 2;2 1];
                        end
                    else
                        if indtemp
                            comp_ind=[1 2;2 1];
                        else
                            comp_ind=[1 2;1 2];
                        end
                    end
                    
                end
                
            end
            
        end
        
        function [comp_ind]=GM_compare(obj1,obj2)  % comparing based on KL div.
            
            if obj1.NComponents~=obj2.NComponents
               error(message('The number of components for GM is not the same!')) 
            end
            
            if obj1.NComponents==1
                
                comp_ind=[1;1];
                
            else
                
                Qtemp=zeros(1,obj1.NComponents);
                
                if (obj1.w(1)>obj1.w(2)) 
%                if ~(obj1.w(1)==0) 
                    obj1temp=gmdistr_ECM_imec(obj1.mu(1), obj1.sigma(1), obj1.w(1));
                    %obj1temp=gmdistr_ECM_imec(obj1.mu(1), obj1.sigma(1), 1);
                    ind_temp=1;
                else
                    obj1temp=gmdistr_ECM_imec(obj1.mu(2), obj1.sigma(2), obj1.w(2));
                    %obj1temp=gmdistr_ECM_imec(obj1.mu(2), obj1.sigma(2), 1);
                    ind_temp=0;
                end
                
                
                for i=1:obj2.NComponents
                    obj2temp=gmdistr_ECM_imec(obj2.mu(i), obj2.sigma(i), obj2.w(i));
                    % obj2temp=gmdistr_ECM_imec(obj2.mu(i), obj2.sigma(i), 1);
                    Qtemp(i)=KL_div(obj1temp,obj2temp);
                    % Qtemp(i)=KL_div(obj2temp,obj1temp);
                    % Qtemp(i)=Bhattacharyya_dist(obj1temp,obj2temp);
                end
                
                if abs(Qtemp(1))<abs(Qtemp(2))
                    if ind_temp
                        comp_ind=[1 2;1 2];
                    else
                        comp_ind=[1 2;2 1];
                    end
                else
                    if ind_temp
                        comp_ind=[1 2;2 1];
                    else
                        comp_ind=[1 2;1 2];
                    end
                end
                
                
            end
            
        end
        
        function [comp_ind]=GM_compare_KL_MGMP(obj1,obj2) % comparing the pairing with similar means
         
          
            if obj1.NComponents~=obj2.NComponents
                error(message('The number of components for GM is not the same!'))
            end
            
            Nmax=obj1.NComponents;
            comp_ind=[1:Nmax;1:Nmax];

            [sort_obj1, ind_obj1]=sort(obj1.w); %#ok<ASGLU>
            obj2_zerow=[];
            obj2_com=1:Nmax;
            
            jj=Nmax;
            while jj && ~isempty(obj2_com)
                obj1temp = gmdistr_ECM_imec(obj1.mu(ind_obj1(jj)), obj1.sigma(ind_obj1(jj)), obj1.w(ind_obj1(jj)));
                KL_vec = zeros(1, length(obj2_com));
                for ii=1:length(obj2_com)
                obj2temp = gmdistr_ECM_imec(obj2.mu(obj2_com(ii)), obj2.sigma(obj2_com(ii)), obj2.w(obj2_com(ii)));
                %KL_vec(ii)= Bhattacharyya_dist(obj1temp, obj2temp);
                %KL_vec(ii) = Jensen_Shannon_div(obj1temp, obj2temp);
                KL_vec(ii) = KL_div(obj1temp,obj2temp);
                end
                ind=find(KL_vec == min(KL_vec));
                if (obj2.w(ind(1))~=0)
                    comp_ind(2,comp_ind(1,:)==ind_obj1(jj))=obj2_com(ind(1));
                    %obj2.mu(ind)=[];
                    %obj2_com(obj2_com==ind)=[];
                    obj2_com(ind(1))=[];
                    jj=jj-1;
                else
                    obj2_zerow=[obj2_zerow,obj2_com(ind(1))];       %#ok<AGROW>
                    %obj2.mu(ind)=[];
                    %obj2_com(obj2_com==ind)=[];
                    obj2_com(obj2_com==ind(1))=[];
                    continue;
                end
            end
            
            if jj~=0
            comp_ind(2,comp_ind(1,:)==[1:jj])=obj2_zerow;    
            end
            
        end        
        
        function [comp_ind, Flag_beta]=GM_compare_KL_MGMP_Flag(obj1,obj2) % comparing the pairing with similar means
         
            if obj1.NComponents~=obj2.NComponents
                error(message('The number of components for GM is not the same!'))
            end
            
            Nmax=obj1.NComponents;
            comp_ind=[1:Nmax;1:Nmax];

            [sort_obj1, ind_obj1]=sort(obj1.w); %#ok<ASGLU>
            obj2_zerow=[];
            obj2_com=1:Nmax;
            
            jj=Nmax;
            while jj && ~isempty(obj2_com)
                obj1temp = gmdistr_ECM_imec(obj1.mu(ind_obj1(jj)), obj1.sigma(ind_obj1(jj)), obj1.w(ind_obj1(jj)));
                KL_vec = zeros(1, length(obj2_com));
                for ii=1:length(obj2_com)
                obj2temp = gmdistr_ECM_imec(obj2.mu(obj2_com(ii)), obj2.sigma(obj2_com(ii)), obj2.w(obj2_com(ii))); 
                %KL_vec(ii)= Bhattacharyya_dist(obj1temp, obj2temp);                
                %KL_vec(ii) = Jensen_Shannon_div(obj1temp, obj2temp);                
                KL_vec(ii) = KL_div(obj1temp,obj2temp);
                end
                ind=find(KL_vec == min(KL_vec));
                if (obj2.w(ind(1))~=0)
                    comp_ind(2,comp_ind(1,:)==ind_obj1(jj))=obj2_com(ind(1));
                    jj=jj-1;
                else
                    obj2_zerow=[obj2_zerow,obj2_com(ind(1))];       %#ok<AGROW>
                    continue;
                end
            end
            
            if jj~=0
            comp_ind(2,comp_ind(1,:)==[1:jj])=obj2_zerow;    
            end
            
            if ~prod(unique(comp_ind(2,:)==1:Nmax))
               Flag_beta=1;
            else
               Flag_beta=0;  
            end            
            
        end           


        function [comp_ind, Flag_beta]=GM_compare_MGMP_Flag(obj1,obj2) % comparing the pairing with similar means
         
          
            if obj1.NComponents~=obj2.NComponents
                error(message('The number of components for GM is not the same!'))
            end
            
            Nmax=obj1.NComponents;
            comp_ind=[1:Nmax;1:Nmax];

            [sort_obj1, ind_obj1]=sort(obj1.w); %#ok<ASGLU>
            obj2_zerow=[];
            obj2_com=1:Nmax;
            
            jj=Nmax;
            while jj && ~isempty(obj2_com)  
                ind=find(abs(obj1.mu(ind_obj1(jj))-obj2.mu)==min(abs(obj1.mu(ind_obj1(jj))-obj2.mu)));
                if (obj2.w(ind(1))~=0)
                    comp_ind(2,comp_ind(1,:)==ind_obj1(jj))=obj2_com(ind(1));
                    jj=jj-1;
                else
                    obj2_zerow=[obj2_zerow,obj2_com(ind(1))];       %#ok<AGROW>
                    continue;
                end
            end
            
            if jj~=0
            comp_ind(2,comp_ind(1,:)==[1:jj])=obj2_zerow;    
            end
            
            if ~prod(unique(comp_ind(2,:)==1:Nmax))
               Flag_beta=1;
            else
               Flag_beta=0;  
            end
            
        end        
        
        function [comp_ind]=GM_compare_MGMP(obj1,obj2) % comparing the pairing with similar means
         
          
            if obj1.NComponents~=obj2.NComponents
                error(message('The number of components for GM is not the same!'))
            end
            
            Nmax=obj1.NComponents;
            comp_ind=[1:Nmax;1:Nmax];
       
%             [sort_obj1, ind_obj1]=sort(obj1.w);
%             
%             for jj=Nmax:-1:1
%                 ind=find(abs(obj1.mu(ind_obj1(jj))-obj2.mu)==min(abs(obj1.mu(ind_obj1(jj))-obj2.mu)));
%                 if obj2.w(ind)~=0
%                 comp_ind(2,comp_ind(1,:)==ind_obj1(jj))=ind;
%                 else
%                 continue;
%                 end
%             end
%             
%         end

            [sort_obj1, ind_obj1]=sort(obj1.w); %#ok<ASGLU>
            obj2_zerow=[];
            obj2_com=1:Nmax;
            
            jj=Nmax;
            while jj && ~isempty(obj2_com)  
                ind=find(abs(obj1.mu(ind_obj1(jj))-obj2.mu)==min(abs(obj1.mu(ind_obj1(jj))-obj2.mu)));
                if (obj2.w(ind(1))~=0)
                    comp_ind(2,comp_ind(1,:)==ind_obj1(jj))=obj2_com(ind(1));
                    obj2.mu(ind(1))=[];
                    %obj2_com(obj2_com==ind)=[];
                    obj2_com(ind(1))=[];
                    jj=jj-1;
                else
                    obj2_zerow=[obj2_zerow,obj2_com(ind(1))];       %#ok<AGROW>
                    obj2.mu(ind(1))=[];
                    %obj2_com(obj2_com==ind)=[];
                    obj2_com(obj2_com==ind(1))=[];
                    continue;
                end
            end
            
            if jj~=0
            comp_ind(2,comp_ind(1,:)==[1:jj])=obj2_zerow;    
            end
            
        end
        
        
        function [obj_merge]=GM_merge(obj1,obj2,betac,comp_ind)
            
            if nargin<3
                betac=0.5;
            end
            
            if nargin<4
                comp_ind=[1 2;1 2];
            end            
            
            if betac>1 || betac<0
                error('Combining coefficient (betac) should be in [0 1]');
            end
            
            obj_merge=gmdistr_ECM_imec(obj1.mu, obj1.sigma, obj1.w);
            N_com=obj1.NComponents;
            
            if obj1.NComponents==obj2.NComponents
                
                for i=1:N_com
                    pi1=(1-betac)*obj1.w(comp_ind(1,i));
                    pi2=(betac)*obj2.w(comp_ind(2,i));
                    obj_merge.w(i)=pi1+pi2;
                    obj_merge.mu(i)=(pi1.*obj1.mu(comp_ind(1,i))+pi2.*obj2.mu(comp_ind(2,i)))./(obj_merge.w(i));
                    obj_merge.sigma(i)=sqrt((1/obj_merge.w(i)).*(pi1.*obj1.sigma(comp_ind(1,i)).^2+pi2.*obj2.sigma(comp_ind(2,i)).^2+pi1.*obj1.mu(comp_ind(1,i)).^2+pi2.*obj2.mu(comp_ind(2,i)).^2)-obj_merge.mu(i).^2);
                end
            else
                obj_merge=obj2;
            end
            
            
        end
        
        function [obj_est, FIT] = ...
                gmfit_Anch(obj,values, C, epsilon,ind_init,maxIter,minIter,wth,sigth,ind_red)
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
                maxIter = 50;
            end
            if (nargin < 7)
                minIter = 20;
            end
            if (nargin < 8)
                wth = 8e-3;
            end            
            if (nargin < 9)
                sigth = 5e-2;
            end  
            if (nargin < 10)
                ind_red = 0;
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
            
            obj_est=gmdistr_ECM_imec(mu_est, sigma_est, p_est);
            FIT(1) = inf;
            FIT(2) = DistributionFit(obj_est,values);
            % now iterate
            K_comp=0;
            while ( abs(FIT(end-1)-FIT(end)) >= epsilon & ~K_comp & counter < maxIter & ~sum(isnan(obj_est.w)))|counter<minIter
                % [mu_est, sigma_est, p_est]
                
                %1) Given the old parameter estimates, we can compute for
                %each value the membership probabilities(MP)
                % E step: soft classification of the values into one of the mixtures
                obj_start=obj_est;
                [MemProb]=classify(obj_start,values);
                % M step: ML estimate the parameters of each class (i.e., p, mu, sigma)
                [obj_est,K_comp]=ML_estimate(obj_est,values,MemProb,wth,sigth,ind_red);
                %obj_est
                if isnan(obj_est.w)
                    obj_est=obj_start;
                end
                if K_comp
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
        
        
        function [obj_est, FIT] = ...
                gmfit_Anchor(obj,values, C,ind_init,wth,sigth,ind_red)
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
            epsilon = 1.0e-4;
            maxIter = 50;            
            minIter = 20;
            
            if (nargin < 4)
                ind_init = 1;
            end  
            
            if (nargin < 5)
                wth = 5e-4;
            end            
            if (nargin < 6)
                sigth = 5e-4;
            end  
            if (nargin < 7)
                ind_red = 1;
            end                
            
            if ind_init
                % initialize
                counter=obj.NComponents;
                mu_est=obj.mu;
                sigma_est=obj.sigma;
                p_est = obj.w;
            else
                % initialize
                counter = C;
                mu_est = 2*mean(values) * sort(rand(C,1));
                sigma_est = ones(C,1)*std(values);
                p_est = ones(C,1)/C;
            end
            
            obj_est=gmdistr_ECM_imec(mu_est, sigma_est, p_est);
            FIT(1) = inf;
            FIT(2) = DistributionFit(obj_est,values);
            % now iterate
            K_comp=0;
            while ( abs(FIT(end-1)-FIT(end)) >= epsilon && ~K_comp && counter < maxIter && ~sum(isnan(obj_est.w)))|| counter<minIter
                % [mu_est, sigma_est, p_est]
                
                %1) Given the old parameter estimates, we can compute for
                %each value the membership probabilities(MP)
                % E step: soft classification of the values into one of the mixtures
                obj_start=obj_est;
                [MemProb]=classify(obj_start,values);
                % M step: ML estimate the parameters of each class (i.e., p, mu, sigma)
                [obj_est,K_comp]=ML_estimate(obj_est,values,MemProb,wth,sigth,ind_red);
                %obj_est
                if isnan(obj_est.w)
                    obj_est=obj_start;
                end
                if K_comp
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
        
        function [obj_est] = ...
                Update_GM(obj,values)
            
                % Update GM based on one observtion from the Anchors.

                values=values(:)';
                if sum(isnan(values))>0
                    disp('warning:removing NaNs')
                    values=values(~isnan(values));
                end
            
                mu_est=obj.mu;
                sigma_est=obj.sigma;
                p_est = obj.w;

                obj_est=gmdistr_ECM_imec(mu_est, sigma_est, p_est);
            
                %1) Given the old parameter estimates, we can compute for
                %each value the membership probabilities(MP)
                
                % E step: Soft classification of the values into one of the mixtures
                obj_start=obj_est;
                [MemProb]=classify(obj_start,values);
                % M step: ML estimate the parameters of each class (i.e., p, mu, sigma)
                [obj_est,K_comp]=ML_estimate_UGM(obj_est,values,MemProb);
                
                if isnan(obj_est.w)
                    obj_est=obj_start;
                end
                
                if K_comp
                    obj_est=obj_start;
                end
  
        end    
        
        function [obj_est] = ...
                Update_GM_multi(obj,values,wth,sigth,ind_red,ind_adap_com)
            
                % Update GM based on one observtion from the Anchors.
                
                if nargin<6  
                    ind_adap_com=0;  
                end

                values=values(:)';
                if sum(isnan(values))>0
                    disp('warning:removing NaNs')
                    values=values(~isnan(values));
                end
            
                mu_est=obj.mu;
                sigma_est=obj.sigma;
                p_est = obj.w;

                obj_est=gmdistr_ECM_imec(mu_est, sigma_est, p_est);
            
                %1) Given the old parameter estimates, we can compute for
                %each value the membership probabilities(MP)
                
                % E step: Soft classification of the values into one of the mixtures
                obj_start=obj_est;
                [MemProb]=classify(obj_start,values);
                % M step: ML estimate the parameters of each class (i.e., p, mu, sigma)
                [obj_est,K_comp]=ML_estimate_multi(obj_est,values,MemProb,wth,sigth,ind_red,ind_adap_com);
                
                if isnan(obj_est.w)
                    obj_est=obj_start;
                end
                
                if K_comp
                    obj_est=obj_start;
                end
  
        end 
        
        function [obj_est] = ...
                Update_GM_MGMP(obj,values)
            
                % Update GM based on one observtion from the Anchors.

                values=values(:)';
                if sum(isnan(values))>0
                    disp('warning:removing NaNs')
                    values=values(~isnan(values));
                end
            
                mu_est=obj.mu;
                sigma_est=obj.sigma;
                p_est = obj.w;

                obj_est=gmdistr_ECM_imec(mu_est, sigma_est, p_est);
            
                %1) Given the old parameter estimates, we can compute for
                %each value the membership probabilities(MP)
                
                % E step: Soft classification of the values into one of the mixtures
                obj_start=obj_est;
                [MemProb]=classify(obj_start,values);
                % M step: ML estimate the parameters of each class (i.e., p, mu, sigma)
                [obj_est]=ML_estimate_MGMP(obj_est,values,MemProb);
                
                if isnan(obj_est.w)
                    obj_est=obj_start;
                end
                
  
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
            p = exp( -(x-mu).^2 / (2*sigma^2) ) / ((sigma+eps) * sqrt(2*pi) );
        end
        
        function p = norm_density_arg(x, mu, sigma)
%            p =( -(x-mu).^2 ./ (2*sigma.^2) ) ./ ((sigma+eps) * sqrt(2*pi) );
             p =( -(x-mu).^2 ./ (2*sigma.^2+2*eps.^2) )-log(((sigma+eps)*sqrt(2*pi)));
        end
        
        function P = norm_cdf(x, mu, sigma)
            z=(x-mu)/sigma;
            P = 0.5 * erfc(-z ./ sqrt(2));
        end
    end
    
end % classdef


