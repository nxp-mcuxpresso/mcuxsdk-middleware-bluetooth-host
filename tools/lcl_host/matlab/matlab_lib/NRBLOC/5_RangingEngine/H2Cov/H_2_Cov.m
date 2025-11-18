%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
function [COV,NofMult]=H_2_Cov(H_est,L)
    % Description
    %  Implementation efficient method to compute the covariance matrix
    %  from a frequency domain single-snap shot measurement vector without
    %  the need to create a Hankel matrix in between. Both the number of 
    %  operation and memory-usage is reduced.
    %  More details can be found in TN-17-WATS-TP2-198:“Proof of concept 
    %  ranging platform”.
    % Description end
if nargin==0% To test the correct behaviour
    rng(1)
    home
    for cnt=1:100
        N=randi(50)+30;
        L=floor(N/2)+randi(5)-3;
        
        H_est=(randn(N,1)+1i*randn(N,1)).';
        Hankel=H_2_Hankel(H_est,L-1);
        COV_ref=Hankel'*Hankel;
        [COV,NofMult]=H_2_Cov(H_est,L);
        if norm(COV-COV_ref)>1e-10
            norm(COV-COV_ref)
        end
        norm(COV-COV_ref)
    end
else %Actual algorithm 
    NofMult=0;
    N=size(H_est,2);
    
    COV=zeros(L,L,class(H_est));
    for diagOffset=0:L-1
        for cnt=1:L-diagOffset
            if cnt==1
                for cnt2=1:N-(L)+1
                    COV(cnt,cnt+diagOffset)=COV(cnt,cnt+diagOffset)+conj(H_est(cnt2))*H_est(cnt2+diagOffset);
                    NofMult=NofMult+1;
                end
            else
                COV(cnt, cnt + diagOffset) = COV(cnt-1,cnt-1+diagOffset)-conj(H_est(cnt-1))*H_est(cnt-1+diagOffset)+conj(H_est(cnt+N-(L)))*H_est(cnt+N-(L)+diagOffset);
                NofMult=NofMult+2;
            end
            if diagOffset~=0
                COV(cnt+diagOffset,cnt)=conj(COV(cnt,cnt+diagOffset));
            end
        end
    end
    
end