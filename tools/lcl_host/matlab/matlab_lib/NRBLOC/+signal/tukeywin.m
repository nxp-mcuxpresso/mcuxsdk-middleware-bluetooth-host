%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
function w = tukeywin(N,R)
% Description
%   TUKEYWIN Tukey window.
%   Source [wikipedia]: The Tukey window, also known as the tapered cosine 
%   window, can be regarded as a cosine lobe that is convolved 
%   with a rectangular window. Should be equivalent to Matlab's tuckeywin
% description end

if nargin < 2 || isempty(R)
    R = 0.5;
end
validateattributes(R,{'numeric'},{'scalar','real','finite'},'tukeywin','R',2);

if N == 0 
    w = zeros(0,1);       % Empty matrix: 0-by-1
elseif N == 1
    w = 1;
else
    if R <= 0
        w = ones(N,1);
    elseif R >= 1
        w = signal.hann(N);
    else
        t = linspace(0,1,N)';
        per = R/2;
        tl = floor(per*(N-1))+1;
        th = N-tl+1;
        %  By definition, the Window is defined in three sections: taper, constant, taper
        w = [ ((1+cos(pi/per*(t(1:tl) - per)))/2);  ones(th-tl-1,1); ((1+cos(pi/per*(t(th:end) - 1 + per)))/2)];
    end
end