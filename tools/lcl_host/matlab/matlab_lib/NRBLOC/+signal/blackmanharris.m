%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
function w = blackmanharris(N, sflag)
% Description
%   BLACKMANHARRIS(N) returns an N-point minimum 4-term Blackman-Harris 
%   window in a column vector.
% Description end
if nargin<2
    sflag = 'symmetric';
end
w = signal.cosine_based_window('blackmanharris', N, sflag);