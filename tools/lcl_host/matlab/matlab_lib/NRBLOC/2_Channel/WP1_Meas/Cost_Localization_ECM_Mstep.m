%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   COST function used for initialization of ECM
% Description end

function [COST,COST_comp]=Cost_Localization_ECM_Mstep(TagPos,BSPOS,DistMeas,gm_model)

TagPos=TagPos.';

if size(DistMeas,1)~=size(BSPOS,1)
    error('size(DistMeas,1) should be equal to size(BSPOS,1)')
end

Dist=sqrt(sum((ones(size(BSPOS,1),1)*TagPos-BSPOS).^2,2));
COST_comp=zeros(1,size(DistMeas,2));
for cnt=1:size(DistMeas,2)
    RE_givenTagPos=DistMeas(:,cnt)-Dist;
    [MemProb]=classify(gm_model,RE_givenTagPos.');
    COST_comp(cnt)=sum(sum(LS_pdf_ECM(gm_model,RE_givenTagPos(~isnan(RE_givenTagPos))).*(MemProb.'),2));
end
COST=sum(COST_comp);
end

