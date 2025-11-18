function w = tukeywinV2(N,R)
%--------------------------------------------------------------------------
% Project   : NRB ranging
% Version   : GIT
% Author    : Jac Romme (jac.romme@imec-nl.nl)
% Copyright : Stichting Imec Nederland (http://www.imec-nl.nl)
% Disclaimer: Imec strictly confidential
%--------------------------------------------------------------------------
% Description
%   TUKEYWINV2 Tukey window.
%   Source [wikipedia]: The Tukey window, also known as the tapered cosine 
%   window, can be regarded as a cosine lobe that is convolved 
%   with a rectangular window. Should be equivalent to Matlab's tuckeywin
% description end
if R <= 0,
    w = ones(N,1);
elseif R >= 1,
    w = hann(N);
else
    t = linspace(0,1,N)';
    per = R/2;
    tl = floor(per*(N-1))+1;
    th = N-tl+1;
    %  By definition, the Window is defined in three sections: taper, constant, taper
    w = [ ((1+cos(pi/per*(t(1:tl) - per)))/2);  ones(th-tl-1,1); ((1+cos(pi/per*(t(th:end) - 1 + per)))/2)];
end
