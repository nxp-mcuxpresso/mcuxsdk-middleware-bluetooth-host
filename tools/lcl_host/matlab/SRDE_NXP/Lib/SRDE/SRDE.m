function [d,y, d_all,y_all,Param,Likeliness] = SRDE(Iq,freq_rast,evd_used)

if nargin < 3
    evd_used = 0;
end

Param.delta_F  = min(diff(freq_rast));
Param.L = 40;
% Param.TOL = [1e-7, 0.999, -Inf];
if 1
    Param.PS_approx = 0.005;
else
    % Use this for ideal conditions
    Param.PS_approx = 0.000000001;
end
if 0 % default cpp unit test config
    Param.MethodSubspaceSep = 14;
    Param.TOL = [1e-7, 0.999, -Inf];
    Param.maxiter = 100;
else
    Param.MethodSubspaceSep = 8;
    Param.TOL = [1e-7, 0.99, -Inf];
    Param.maxiter = 200;
end
Param.MinLevel = 0.1;
Param.InitPos = 0;
Param.NFFT = 512;

% Compute the covariance matrix
if size(Iq,1) > size(Iq,2)
    Iq = Iq.';
end
Cov = GetCov(Param,Iq);

% Eigenvetors
if evd_used == 0
    [eigenvects,EVs] = evd(Cov, Param.TOL(1), Param.TOL(2), Param.MethodSubspaceSep);
elseif evd_used == 1
    [eigenvects,EVs] = evd_complex(Cov, Param.TOL(1), Param.TOL(2), Param.MethodSubspaceSep);
elseif evd_used == 2
    [eigenvects,EVs] = evd_complex(Cov, Param.TOL(1), Param.TOL(2), Param.MethodSubspaceSep,0);
end
Ns               = SubSpaceSep(Param,EVs);

% Find first spectra peak
[omega,y, x_all,y_all] = FirstPeak_PS(Param,eigenvects(:,1:Ns-1).');

% Estimate distance
d = omega/(2*pi*Param.delta_F)*3e8;
d_all = x_all/(2*pi*Param.delta_F)*3e8;

% Likeliness
TRACE = trace(Cov);
Likeliness = ComputeLikeliness(omega,eigenvects,EVs,TRACE,Ns);
