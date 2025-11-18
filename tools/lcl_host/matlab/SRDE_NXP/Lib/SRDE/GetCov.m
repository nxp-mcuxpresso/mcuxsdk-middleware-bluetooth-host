function Cov=GetCov(param,H_est)
Cov=zeros(param.L,param.L,class(H_est));
for cnt=1:size(H_est,1)
    Cov=Cov+H_2_Cov(H_est(cnt,:),param.L);%Result is Hermitian
end