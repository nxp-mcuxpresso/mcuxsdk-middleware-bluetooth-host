%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   COST function used for lateration assuming the same error distribution for
%   all anchors.
% Description end

function [COST,COST_comp]=Cost_Localization_cin(TagPos,BSPOS,DistMeas,gm_model,optcomp)

TagPos=TagPos.';

if nargin==5
    
    if strcmp(optcomp.type,'LS')
        Dist=sqrt(sum((ones(size(BSPOS,1),1)*TagPos-BSPOS).^2,2));
        COST=norm(DistMeas-(Dist+optcomp.bias));
        %COST=norm(DistMeas-(Dist+1)); % test        
    elseif strcmp(optcomp.type,'L1')
        Dist=sqrt(sum((ones(size(BSPOS,1),1)*TagPos-BSPOS).^2,2));
        COST=sum(abs(DistMeas-(Dist+optcomp.bias)));
    elseif strcmp(optcomp.type,'Huber')
        if ~isfield(optcomp,'Hub_th')
            optcomp.Hub_th=max(max(BSPOS));
        else
            Dist=sqrt(sum((ones(size(BSPOS,1),1)*TagPos-BSPOS).^2,2));
            costtemp=inf*ones(size(BSPOS,1),1);
            tempind=find(abs(DistMeas-(Dist+optcomp.bias))<=optcomp.Hub_th);
            costtemp(tempind)=0.5*(DistMeas(tempind)-(Dist(tempind)+optcomp.bias)).^2;
            tempind=find(abs(DistMeas-(Dist+optcomp.bias))>optcomp.Hub_th);
            costtemp(tempind)=optcomp.Hub_th*abs(DistMeas(tempind)-(Dist(tempind)+optcomp.bias))-0.5*optcomp.Hub_th.^2;
            COST=sum(costtemp);
        end
    end
    
else
    
    if size(DistMeas,1)~=size(BSPOS,1)
        error('size(DistMeas,1) should be equal to size(BSPOS,1)')
    end
    
    Dist=sqrt(sum((ones(size(BSPOS,1),1)*TagPos-BSPOS).^2,2));
    COST_comp=zeros(1,size(DistMeas,2));
    for cnt=1:size(DistMeas,2)
        RE_givenTagPos=DistMeas(:,cnt)-Dist;
        if ismethod(gm_model,'logpdf')
            %COST_comp(cnt)=-sum(log(pdf(gm_model,RE_givenTagPos(~isnan(RE_givenTagPos)))));
            COST_comp(cnt)=-sum(logpdf(gm_model,RE_givenTagPos(~isnan(RE_givenTagPos))));
        else
            COST_comp(cnt)=-sum(log(pdf(gm_model,RE_givenTagPos(~isnan(RE_givenTagPos)))));
        end
    end
    COST=sum(COST_comp);
end

end
