function [H_est,H_est_1D] = imec_calc_h_2D_est(IQ_A,IQ_B)

Hest = zeros(size(IQ_A));
for iCh=1:size(IQ_A,2)
    H_est_sq = IQ_A(:,iCh).*IQ_B(:,iCh);
    H_est = imec_calc_h_est(H_est_sq);
    Hest(:,iCh) = H_est;
end
H_est = spatial_rec(IQ_A, Hest);
H_est_1D = Hest;


function H_est = spatial_rec(IQ_ref, H_est)
Ka = size(IQ_ref,2);
Kf = size(IQ_ref,1);
IQ_ref = IQ_ref*exp(-1j*angle(IQ_ref(1))+1j*angle(H_est(1)));
for itone = 1:Kf
    % Overall antenna sign-flip correction
    if itone == 1
        for iant = 1:Ka
            current_H = H_est(itone,iant);
            flipped_H = -current_H;
            
            d_current = abs(current_H-IQ_ref(itone,iant));
            d_flipped = abs(flipped_H-IQ_ref(itone,iant));
            if d_current>d_flipped
                H_est(itone:end,iant) = -H_est(itone:end,iant);
            else
                % do nothing
            end
        end
        continue
    end
    
    % antenna sign-flip correction tone-by-tone
    % can do nothing if only 2 antennas are there
    if Ka == 2
        return
    end
    
    for iantpair = 1:Ka-1
        diff_IQ_ref = IQ_ref(itone,1:end-1).*conj(IQ_ref(itone,2:end))./abs(IQ_ref(itone,2:end));
        current_diff_Hs = H_est(itone,1:end-1).*conj(H_est(itone,2:end))./abs(H_est(itone,2:end));
        flipped_diff_Hs = current_diff_Hs;
        flipped_diff_Hs(iantpair) = -flipped_diff_Hs(iantpair);
        
        d_current = abs(current_diff_Hs(iantpair)-diff_IQ_ref(iantpair));
        d_flipped = abs(flipped_diff_Hs(iantpair)-diff_IQ_ref(iantpair));
        if d_current>d_flipped
            % something wrong with the current antenna pair
            % try to tell which antenna's fault
            if iantpair == Ka-1
                % if we've come to the last antenna pair, it's definitely
                % the latter antenna's fault
                iant2blame = iantpair+1;
            else
                d_next = abs(current_diff_Hs(iantpair+1)-diff_IQ_ref(iantpair+1));
                next_flipped_diff_Hs = current_diff_Hs;
                next_flipped_diff_Hs(iantpair+1) = -next_flipped_diff_Hs(iantpair+1);
                d_next_flipped = abs(next_flipped_diff_Hs(iantpair+1)-diff_IQ_ref(iantpair+1));
                
                if d_next>d_next_flipped
                    % something also wrong with the next antenna pair
                    iant2blame = iantpair+1;
                else
                    iant2blame = iantpair;
                end
            end
            H_est(itone:end,iant2blame) = -H_est(itone:end,iant2blame);
        else
            % do nothing
        end
    end
end
