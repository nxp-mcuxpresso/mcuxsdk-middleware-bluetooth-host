% This code is an embedded explanation of Kalman filter used in phase based
% ranging platform. 
% Input >> KF_input.csv
% Output >> x_out

clc
clear

%% hardware parameters
LO_fc_A = single(2.4e9);           % [Hz] passband centre-frequency of Local Oscilator of Initiator
LO_fc_B = single(2.4e9);           % [Hz] passband centre-frequency of Local Oscilator of Reflector
Delta_F = single(1e6);             % [Hz] channel frequency step
T_f = single(500e-6);              % [sec] Duration of a single tone measurement in two directions.
alpha = (LO_fc_A+LO_fc_B)/2*T_f/Delta_F;

%% tracking parameters (These parameters are )
%%%%%%%%%%%% Modified in AKF
n_a = 5;                            % number of samples to estimate the acceleration variance
is_outlier(1) = false;              % a flag showing if a range measurement is an outlier, false: not outlier, true: is outlier

% var_a = single(1);                  % constant in KF but not in AKF
var_a_ref = single(1);                % constant 
var_d = single(1);                  % constant in KF but not in AKF
%%%%%%%%%%%%
%% read input
% data is a two column matrix, the first column is measured distance at
% every sample and the second column includes the sampling time
data = csvread('KF_input.csv');     
data = single(data);
n = length(data);              % @Martijn: This is the number of measurements
Tracking_flag = true;          % continue tracking while this flag is true 

%% KF initialization
xhat(1) = data(1,1);
xhat(2) = single(0);
P(1,1) = single(1);
P(1,2) = single(0);
P(2,1) = single(0);
P(2,2) = single(1);
x_out(1,1) = xhat(1);
x_out(2,1) = xhat(2);
x_out(3,1) = single(0);

%% KF
cnt = 1;
while 1
    cnt = cnt+1;
    d_ss = data(1,cnt);
    Ts = data(2,cnt);
    % prediction
    xhat_plus(1) = xhat(1)+Ts*xhat(2);
    xhat_plus(2) = xhat(2);
    %%%%%%%%%%%% Added in AKF
    if cnt<=n_a
      var_a = var_a_ref; 
    else
        for k = 1:n_a
            a_store(k) = x_out(3,cnt-n_a+k-1);
        end
        var1 = 0;
        for k = 1:n_a
            var1 = var1+a_store(k)^2;
        end
        var1 = var1/(n_a-1);
        if var1<(2*var_a_ref)
            var_a = var_a_ref;
        else
            var_a = var1;
        end
    end        
    %%%%%%%%%%%%
    Pplus(1,1) = P(1,1)+Ts*P(1,2)+Ts*P(2,1)+(Ts^2)*P(2,2)+Ts^4*var_a/4;
    Pplus(1,2) = P(1,2)+Ts*P(2,2)+Ts^3*var_a/2;
    Pplus(2,1) = P(2,1)+Ts*P(2,2)+Ts^3*var_a/2;
    Pplus(2,2) = P(2,2)+Ts^2*var_a;
    % Update
    if isnan(d_ss)              % @Martijn, sometimes a measurement is completed but no range can be estimated, i.e. the output of ranging algorithm would be NaN
        xhat = xhat_plus;
        P = Pplus;
    else
        Y = xhat_plus(1)+alpha*xhat_plus(2);
        Z = d_ss-Y;
        R = var_d;              % constant in KF but not in AKF
        S = R+Pplus(1,1)+alpha*Pplus(1,2)+alpha*Pplus(2,1)+alpha^2*Pplus(2,2);
        %%%%%%%%%%%% Added in AKF
        if abs(Z)>S
            R = (R*(Z/S)^2);
            S = R+Pplus(1,1)+alpha*Pplus(1,2)+alpha*Pplus(2,1)+alpha^2*Pplus(2,2);
        end
        if abs(Z)>5*S
            is_outlier(cnt) = true;
        else
            is_outlier(cnt) = false;
        end       
        %%%%%%%%%%%%
        K(1) = (Pplus(1,1)+alpha*Pplus(1,2))/S;
        K(2) = (Pplus(2,1)+alpha*Pplus(2,2))/S;
        xhat(1) = xhat_plus(1)+K(1)*Z;
        xhat(2) = xhat_plus(2)+K(2)*Z;
        P(1,1) = Pplus(1,1)-K(1)*Pplus(1,1)-alpha*K(1)*Pplus(2,1);
        P(1,2) = Pplus(1,2)-K(1)*Pplus(1,2)-alpha*K(1)*Pplus(2,2);
        P(2,1) = Pplus(2,1)-K(2)*Pplus(1,1)-alpha*K(2)*Pplus(2,1);
        P(2,2) = Pplus(2,2)-K(2)*Pplus(1,2)-alpha*K(2)*Pplus(2,2);
        P(1,2) = (P(1,2)+P(2,1))/2;
        P(2,1) = (P(1,2)+P(2,1))/2;
        x_out(1,cnt) = xhat(1);
        x_out(2,cnt) = xhat(2);
        x_out(3,cnt) = (x_out(2,cnt)-x_out(2,cnt-1))/Ts;
    end
    % check and break if measurements are finished
    if cnt==n
        break;
    end
end

%% plot
figure, plot(data(1,:)), hold on, plot(x_out(1,:));
legend('Raw measurement', 'Tracking');
xlabel('sample number'), ylabel('range estimation [m]');


