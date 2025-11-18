function [COV,NofMult]=H_2_Cov(H_est,L)
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