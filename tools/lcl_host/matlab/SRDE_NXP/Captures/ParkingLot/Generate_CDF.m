clear all;
a = dir;
cnt = 0;
for m = 1:length(a)
    if contains(a(m).name,'.mat')
        cnt = cnt+1;
    end
end
d_mciq = cell(cnt,1);
d_cde = cell(cnt,1);
d_tof = cell(cnt,1);
d_true = cell(cnt,1);
h_true = cell(cnt,1);

cnt = 0;
for m = 1:length(a)
    if contains(a(m).name,'.mat')
        cnt = cnt + 1;
        d_mciq{cnt} = [];
        d_cde{cnt} = [];
        d_tof{cnt} = [];
        idx1 = strfind(a(m).name,'_x_');
        idx2 = strfind(a(m).name,'_y_');
        string = strrep(a(m).name((idx1+3):(idx2-1)),'p','.');
        idx3 = strfind(a(m).name,'_z_');
        idx4 = strfind(a(m).name,'.mat');
        string2 = strrep(a(m).name((idx3+3):(idx4-1)),'p','.');
        d_true{cnt} = [];
        h_true{cnt} = [];
        load(a(m).name);
        for k = 1:length(Result)
            d_mciq{cnt} = [d_mciq{cnt} Result(k).Result.Distance];
            d_cde{cnt} = [d_cde{cnt} Result(k).Result.CDE_Distance];
            d_tof{cnt} = [d_tof{cnt} Result(k).Result.ToF_Distance];
            d_true{cnt} = [d_true{cnt} str2double(string)];
            h_true{cnt} = [h_true{cnt} str2double(string2)];
        end
    end
    if str2double(string) == 1
        d_cde_cal = mean(d_cde{cnt})-1+0.4;
        d_mciq_cal = mean(d_mciq{cnt})-1+0.4;
        d_tof_cal = mean(d_tof{cnt})-1+0.7;
    end
end

% figure; hold on;
d_cde_err = [];
d_mciq_err = [];
d_tof_err = [];
d_true_err = [];
h_true_err = [];
for m = 1:cnt
%     plot(d_cde{m}-d_true(m)-d_cde_cal); hold on;
    d_cde_err = [d_cde_err d_cde{m}-d_true{m}-d_cde_cal];
    d_mciq_err = [d_mciq_err d_mciq{m}-d_true{m}-d_mciq_cal];
    d_tof_err = [d_tof_err d_tof{m}-d_true{m}-d_tof_cal];
    d_true_err = [d_true_err d_true{m}];
    h_true_err = [h_true_err h_true{m}];
end

figure; plot(d_true_err,d_cde_err,'o'); hold on; plot(d_true_err,d_mciq_err,'x');
figure; plot(d_true_err,d_tof_err,'o');

x = -1:0.01:1;
x_tof = -3:0.01:3;
H_cde = hist(d_cde_err,x); C_cde = cumsum(H_cde/sum(H_cde));
H_mciq = hist(d_mciq_err,x); C_mciq = cumsum(H_mciq/sum(H_mciq));
H_tof = hist(d_tof_err(h_true_err==0.46),x_tof); C_tof = cumsum(H_tof/sum(H_tof));

figure;
plot(x,C_cde); hold on; plot(x,C_mciq); ylim([-0.1,1.1]); grid on;
title('Phase based - Outdoors parking lot'); legend('CDE','MCIQ');
xlabel('Distance error (meters)'); ylabel('CDF');

figure;
plot(x_tof,C_tof); ylim([-0.1,1.1]); grid on;
title('ToF based - Outdoors parking lot'); legend('ToF');
xlabel('Distance error (meters)'); ylabel('CDF');


