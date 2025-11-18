%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This function fits the distribution with a Gaussian Mixture Model.
% Description end

function [CRLB,numComponents,obj]=CRLBwFittedGM(X,PLOT, nComponents)
if nargin<3
    nComponents = 3;
end

if nargin==0
    close all
    NofElem=10000;
    
    CRLBwFittedGM('test')
    %Example 1:
    for Example=1:0%:2
        figure
        switch Example
            case 1
                eta=0.1;
                w=[1-eta eta];
                mu=[0 0];
                sigmaSq=[0.5 100];
            case 2
                eta=0.0;
                w=[1-eta eta];
                mu=[0 0];
                sigmaSq=[1 0];
        end
        P=rand(NofElem,1)>eta;
        X = (P==1).*(mu(1) + sqrt(sigmaSq(1)).*randn(NofElem,1))+(P==0).*(mu(2) + sqrt(sigmaSq(2)).*randn(NofElem,1));
        CRLBwFittedGM(X,1);
        
        k=sigmaSq(2)/sigmaSq(1);
        c = (1-eta)/pi;
        d = eta/(pi*k+eps);
        
        Fisher2=pi/sigmaSq(1)*(c+d);
        CRLB2=1/Fisher2;
        
        subplot(2,1,1)
        title(['\mu = [' num2str(mu) ']' ',\sigma^2=[' num2str(sigmaSq(1)) ' '  num2str(sigmaSq(2)) ']' ', w=[' num2str(w(1)) ' ' num2str(w(2)) '], CRLB_{th}=' num2str(round(100*CRLB2)/100)] )
        xlim([-5 5])
    end
    
    
    
    %     %Example 2:
    %     figure
    %
    %     X = (P==1).*(mu(1) + sqrt(sigmaSq(1)).*randn(NofElem,1))+(P==0).*(mu(2) + sqrt(sigmaSq(2)).*randn(NofElem,1));
    %     Fisher(X,1)
    %     subplot(2,1,1)
    %     title(['\mu = [' num2str(mu) ']' ',\sigma^2=[' num2str(sigmaSq(1)) ' '  num2str(sigmaSq(2)) ']' ', w=[' num2str(w(1)) ' ' num2str(w(2)) ']'] )
    %     xlim([-5 5])
elseif ischar(X)
    NofTest=100;NofElem=100;
    
    for Test=1:NofTest
        eta=0.1+0*0.2*rand(1,1);
        w=[1-eta eta];
        mu=100*[0 0]*rand(1,1);
        sigmaSq=[0.5 100]*rand(1,1);
        
        P=rand(NofElem,1)>eta;
        X = (P==1).*(mu(1) + sqrt(sigmaSq(1)).*randn(NofElem,1))+(P==0).*(mu(2) + sqrt(sigmaSq(2)).*randn(NofElem,1));
        CRLB_m(Test,1)=CRLBwFittedGM(X);
        CRLB_m(Test,2)=CRLBwFittedGM(X);
        CRLB_m(Test,3)=CRLBwFittedGM(X);
        
        k=sigmaSq(2)/sigmaSq(1);
        c = (1-eta)/pi;
        d = eta/(pi*k+eps);
        
        Fisher2=pi/sigmaSq(1)*(c+d);
        CRLB2(Test)=1/Fisher2;
        if min(CRLB_m(Test,:))>5*CRLB2(Test)
            disp(CRLBwFittedGM(X))
            disp('?')
        end
    end
    CRLB=min(CRLB_m.');
    plot(CRLB)
    hold on
    plot(CRLB_m,'.');
    plot(CRLB2,'g')
    
    
else
    if nargin<2
        PLOT=false;
    end

    CDFMATCH=1;
    while CDFMATCH>0
        AIC = inf*zeros(1,nComponents);
        obj = cell(1,length(AIC));
        k=1;STOP=0;
        while (STOP==0)&&(k <=length(AIC))
            try
                obj{k} = fitgmdist(X,k);
                AIC(k)= obj{k}.AIC;
            catch ME
                disp(ME.message)
                AIC(k)=inf;
            end
            
            
            if k>1
                if AIC(k)> AIC(k-1)
                    STOP=true;
                end
            end
            k=k+1;
        end
        [minAIC,numComponents] = min(AIC);
        obj = obj{numComponents};
        
        h=min(sqrt(obj.Sigma))/40;
        Minrange=min(obj.mu-20*squeeze(sqrt(obj.Sigma)));
        Maxrange=max(obj.mu+20*squeeze(sqrt(obj.Sigma)));
        bins=Minrange:h:Maxrange;
        CDF_gm=cdf(obj,bins.');
        
        pdf_est=hist(X,bins);
        pdf_est=pdf_est/sum(pdf_est)/h;
        cdf_est=cumsum(pdf_est).'/sum(pdf_est);
        
        if abs(trapz(bins,CDF_gm-cdf_est))<0.05
            CDFMATCH=0;
        elseif CDFMATCH>20
            disp('CRLBwFittedGM:STOP trying fit GM')
            CDFMATCH=0;
        else
            disp('CRLBwFittedGM:retry fit GM')
            CDFMATCH=CDFMATCH+1;
        end
        
    end
    
    
    
    LogPDF= log(pdf(obj,bins.'));
    Der2LogPDF=(LogPDF(1:end-2)+LogPDF(3:end)-2*LogPDF(2:end-1))/h^2;
    BinsDer=bins(2:end-1);
    
    F=-trapz(BinsDer,pdf(obj,BinsDer.').*Der2LogPDF);
    CRLB=1/F;
    
    if PLOT
        figure
        MSE=trapz(bins,bins.^2.*(pdf(obj,bins.').'));
        sp_hdl(1)=subplot(2,1,1);
        plot(bins,cdf_est,'r')
        hold on
        plot(bins,cdf(obj,bins.'),'b--')
        ylabel('CDF [P(Err<x)]')
        xlabel('Err[m]')
        legend('Observed','Fitted Gaussian Mix','Location','NorthWest')
        
        sp_hdl(2)=subplot(2,1,2);
        plot(bins,pdf_est,'r')
        hold on
        plot(bins,pdf(obj,bins.'),'b--')
        
        ylabel('PDF [P/m]')
        xlabel('Err[m]')
        legend('Observed','Fitted Gaussian Mix','Location','NorthWest')
        
        %         sp_hdl(3)=subplot(3,1,3)
        %         plot(bins,LogPDF,'r')
        %         hold on
        %         plot(BinsDer,Der2LogPDF,'g')
        title(['CRLB=' num2str(round(100*sqrt(CRLB))/100) '[m],MSE=' num2str(round(100*sqrt(MSE))/100) '[m]'])
        linkaxes(sp_hdl,'x')
        xlim([Minrange Maxrange])
    end
end