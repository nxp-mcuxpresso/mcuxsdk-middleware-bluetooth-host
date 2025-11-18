clear;
addpath(genpath('Lib'));
dbstop if error;

Scenario = 'Garage antenna diversity'; % Options: 'Garage without antenna diversity' ; 'Garage antenna diversity' ; 'Office room antenna diversity' ; 'Conducted'
do_mvdr = 0;
t_avg = [];
%% Settings
freq_rast = (0:1e6:79e6).'; channel_mask = ones(size(freq_rast)); dist_grid = -10:0.05:100;
remove_invalid_channels = 1;

disp('Reading input files:'); 
switch Scenario
    case 'Conducted'
        dir_output = dir('Captures\Conducted');
        L = 1;
        Nmax = 20;
        for m = 1:length(dir_output)
            if contains(dir_output(m).name,'.mat')
                load(fullfile('Captures\Conducted',dir_output(m).name));
                disp(dir_output(m).name);
                dist = 3 + 1.7;
                if ~exist('Result','var')
                    Result = resultfile;
                end
                for N=1:Nmax
                    if ~isnan(sum(resultfile(N).mciq__initiator__iq)) && ~isnan(sum(resultfile(N).mciq__reflector__iq))
                        Result(L).iq_in = resultfile(N).mciq__initiator__iq.*channel_mask;
                        Result(L).iq_re = resultfile(N).mciq__reflector__iq.*channel_mask;
                        
                        if remove_invalid_channels
                            Result(L).iq_in(23:25) = Result(L).iq_in(22);
                            Result(L).iq_re(23:25) = Result(L).iq_re(22);
                            Result(L).iq_in = Result(L).iq_in(3:77);
                            Result(L).iq_re = Result(L).iq_re(3:77);
                        end
                        Result(L).ch_2w = Result(L).iq_in.*Result(L).iq_re;
                        Result(L).ch_1w = imec_calc_h_est(Result(L).ch_2w);
                        if remove_invalid_channels
                            test = Result(L).ch_1w;
                            test = [0;0;test(1:21);0;0;0;test(22:72);0;0];
                            Result(L).ch_1w = test;
                        end
                            
                        Result(L).true_distance = dist;
                        Result(L).CFO_init = resultfile(N).mciq__initiator__cfo;
                        Result(L).CFO_refl = resultfile(N).mciq__reflector__cfo;
                        Result(L).Result.Distance = resultfile(N).mciq__result__distance+0.5;
                        if 0 % enable this code to print IQ in int16 format
                            j=1;
                            for i=1:length(Result(L).iq_in)
                                Result(L).iq_in_16(j) = round(real(Result(L).iq_in(i) * 1024));
                                Result(L).iq_in_16(j+1) = round(imag(Result(L).iq_in(i) * 1024));
                                Result(L).iq_re_16(j) = round(real(Result(L).iq_re(i) * 1024));
                                Result(L).iq_re_16(j+1) = round(imag(Result(L).iq_re(i) * 1024));
                                j = j + 2;
                            end
                            disp('const int16_t _IQ_A[] = {');
                            for i=1:length(Result(L).iq_in_16)
                                fprintf(" %d,", Result(L).iq_in_16(i));
                            end
                            disp('};');
                            disp('const int16_t _IQ_B[] = {');
                            for i=1:length(Result(L).iq_re_16)
                                fprintf(" %d,", Result(L).iq_re_16(i));
                            end
                            disp('};');
                        end
                        L = L + 1;
                    end
                end
            end
        end
        
    case 'Garage without antenna diversity'
        uc_idx = [101:117 121:137 141:157]; % Garage
        for mmm = 1:length(uc_idx)
            Result_temp = GetImecData(uc_idx(mmm),'Captures/',channel_mask);
            if ~exist('Result','var')
                Result = Result_temp;
            else
                Result(length(Result)+(1:length(Result_temp))) = Result_temp;
            end
        end
        
    case 'Garage antenna diversity'
        uc_idx = [1:17 21:37 41:47]; % Garage
        for mmm = 1:length(uc_idx)
            Result_temp = GetScaAntDivData(uc_idx(mmm),'Captures/',channel_mask);
            if ~exist('Result','var')
                Result = Result_temp;
            else
                Result(length(Result)+(1:length(Result_temp))) = Result_temp;
            end
        end
        
    case 'Office room antenna diversity'
        Nmax = 20;
        L = 1;
        XYs_mat = [repmat(0.5,1,14) ,       1:0.5:4 , repmat(4,1,13) , 3.5:-0.5:1      ; ....
            0.5:0.5:7 , repmat(7,1,7) ,  6.5:-0.5:0.5  , repmat(0.5,1,6) ];
        dist_vec = sqrt(XYs_mat(1,:).^2 + XYs_mat(2,:).^2) + 1.8;
        for mmm = 1:length(dist_vec)
            dist = dist_vec(mmm);
            x = XYs_mat(1,mmm);
            y = XYs_mat(2,mmm);
            Result_temp = GetIndoorsValidationData_dual(x,y,'Captures\FR_small_meeting_room_B126_MR1_LCL_RC2_dual',channel_mask);
            if ~exist('Result','var')
                Result = Result_temp;
            end
            for N = 1:Nmax
                if ~isnan(Result_temp(N).ch_1w)
                    Result(L) = Result_temp(N);
                    Result(L).true_distance = dist;
                    L = L+1;
                end
            end
        end
    otherwise
        error('Scenario not supported');
end

fprintf('\nWorking on %d measurements\n', length(Result));

d_mvdr_vec = [];
d_mvdr_1w_vec = [];
d_cde_vec = [];
d_srde_vec = [];
likeliness_srde_vec = [];
d_srde_emb_vec = [];
d_true = [];
for N = 1:length(Result)   
    if exist('t_e','var')
        if isempty(t_avg), t_avg = t_e; else, t_avg = 0.9*t_avg + 0.1*t_e; end
        if rem(N,round(0.1*length(Result))) == 0
            total = round((length(Result)-N)*t_avg); fprintf('Remaining %d sec.\n',total);
        end
    end
    if ~isnan(sum(sum(Result(N).iq_in))) && ~isnan(sum(sum(Result(N).iq_re)))
        tic;
        % Call SRDE
        [d_srde, y, d_all,y_all,Param, Likeliness] = SRDE(Result(N).ch_1w,freq_rast);
        % Build result vector, with SRDE distance, SRDE embedded and true
        % distance
        d_srde_vec = [d_srde_vec d_srde];
        likeliness_srde_vec = [likeliness_srde_vec Likeliness];
        d_true = [d_true Result(N).true_distance];
        d_srde_emb_vec = [d_srde_emb_vec Result(N).Result.Distance];
        t_e = toc;
    end
end

%Print results
for N = 1:length(d_srde_vec)
    fprintf('Meas %d, srde:%.3f lik:%.3f srde_python:%.3f\n', N, d_srde_vec(N),likeliness_srde_vec(N), d_srde_emb_vec(N));
end

%Generate CDF plot
x = -5:0.1:15;
err_srde = d_srde_vec - d_true; H_srde = cumsum(hist(err_srde,x))/length(err_srde); 
err_srde_emb = d_srde_emb_vec - d_true; H_srde_emb = cumsum(hist(err_srde_emb,x))/length(err_srde_emb); 
y = 0:0.01:1;
H_lik = cumsum(hist(likeliness_srde_vec,y))/length(likeliness_srde_vec); 
figure; plot(x,H_srde,'linewidth',2); hold on; plot(x,H_srde_emb,'--','linewidth',2);
grid on;
ylabel('CDF'); xlabel('Distance error (m)');
legend('SRDE (Matlab)','SRDE (embedded)');
title(Scenario);
figure; plot(y,H_lik,'linewidth',2); hold on;
ylabel('CDF'); xlabel('Likeliness)');
title(Scenario);