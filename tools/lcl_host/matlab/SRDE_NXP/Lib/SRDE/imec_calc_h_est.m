function [H_est, K_est, Uncertainty] = imec_calc_h_est(H_est_sq)
H_est    = sqrt(H_est_sq);
I = abs(H_est(2:end)-H_est(1:end-1))> abs(H_est(2:end)+H_est(1:end-1));
for cnt=1:length(I)
    if I(cnt)==1
        H_est(cnt+1:end)=-H_est(cnt+1:end);
    end
end
Kf = length(H_est);
Phase_est_coarse=mean(diff_unwrap(angle(H_est_sq)));
Demod       = exp(-1i*Phase_est_coarse/2*(0:Kf-1));
H_demod     = H_est.*Demod;
K_est       = abs(mean(H_demod)).^2/(mean((abs(H_demod)).^2)-abs(mean(H_demod)).^2)/2;
Uncertainty = 0;

H_lowside = zeros(size(H_demod));
H_highside = zeros(size(H_demod));
Err_low1 = zeros(size(H_demod));
Err_low2 = zeros(size(H_demod));
Err_high1 = zeros(size(H_demod));
Err_high2 = zeros(size(H_demod));
Xa = zeros(size(H_demod));
Xb2 = zeros(size(H_demod));
for cnt=3:length(H_demod)-2
    H_lowside(cnt)  =  H_demod(cnt-1) + H_demod(cnt-1) - H_demod(cnt-2);
    H_highside(cnt) =  H_demod(cnt+1) + H_demod(cnt+1) - H_demod(cnt+2);
    Err_low1(cnt)   =  H_demod(cnt) - H_lowside(cnt);
    Err_low2(cnt)   = -H_demod(cnt) - H_lowside(cnt);
    Err_high1(cnt)  =  H_demod(cnt) - H_highside(cnt);
    Err_high2(cnt)  = -H_demod(cnt) - H_highside(cnt);
    
    Xa(cnt)=abs(Err_low1(cnt))>abs(Err_low2(cnt));      %X=abs(Err)-Parameter1*mean(abs(Err(3:end)));
    Xb2(cnt)=abs(Err_high1(cnt))>abs(Err_high2(cnt));   %X=abs(Err)-Parameter1*mean(abs(Err(3:end)));
    if (Xa(cnt)==1)&&(Xa(cnt-1)==0)
        if (Xb2(cnt-1)==1)||(Xb2(cnt-2)==1)
            H_est(cnt:end)=-H_est(cnt:end);
            Uncertainty = Uncertainty+1;
        end
    end
end
end

function x = diff_unwrap(angles)
    x = diff(angles);
    x = x+(x<-pi)*2*pi;
    x = x-(x>pi)*2*pi;
end
