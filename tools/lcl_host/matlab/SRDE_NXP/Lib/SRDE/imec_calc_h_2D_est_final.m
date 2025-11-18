function [H_est,H_est_1D] = imec_calc_h_2D_est_final(IQ_A,IQ_B)

Hest = zeros(size(IQ_A));
for iCh=1:size(IQ_A,2)
    H_est_sq = IQ_A(:,iCh).*IQ_B(:,iCh);
    H_est = imec_calc_h_est_final(H_est_sq);
    Hest(:,iCh) = H_est;
end
H_est = spatial_rec(IQ_A, Hest);
H_est_1D = Hest;


function H_est = spatial_rec(IQ_ref, H_est)
Ka = size(IQ_ref,2);
Kf = size(IQ_ref,1);
IQ_ref = IQ_ref*exp(1j*(angle(H_est(1))-angle(IQ_ref(1))));
diff_IQ_ref = IQ_ref(:,1:end-1).*conj(IQ_ref(:,2:end))./abs(IQ_ref(:,2:end));

for itone = 1:Kf
    % Overall antenna sign-flip correction
    if itone == 1
        d_high = abs(H_est(itone,:)-IQ_ref(itone,:));
        d_low = abs(-H_est(itone,:)-IQ_ref(itone,:));
        for iant = 1:Ka
            if d_high(iant) > d_low(iant)
                H_est(itone:end,iant) = -H_est(itone:end,iant);
            end
        end
        continue
    end

    % antenna sign-flip correction tone-by-tone
    for iant = 1:Ka-1
        diff_H_est = H_est(itone,1:end-1).*conj(H_est(itone,2:end))./abs(H_est(itone,2:end));            
        d_high = abs(diff_H_est-diff_IQ_ref(itone,:));
        d_low = abs(-diff_H_est-diff_IQ_ref(itone,:));

        if d_high(iant) > d_low(iant)
            % something wrong with the current antenna pair
            % try to tell which antenna's fault
            if Ka == 2 % if only 2 antenna available, the one with higher magnitude is trusted
                [~,iant2blame] = min(abs(H_est(itone,:)));
            elseif iant == Ka-1
                % if we've come to the last antenna pair, it's definitely
                % the latter antenna's fault
                iant2blame = iant+1;
            else
                if d_high(iant+1) > d_low(iant+1)
                    % something also wrong with the next antenna pair
                    iant2blame = iant+1;
                else
                    iant2blame = iant;
                end
            end
            H_est(itone:end,iant2blame) = -H_est(itone:end,iant2blame);
        end
    end
end
