%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
function w = hamming(N, sflag)
% Description
%   HAMMING(N) returns the N-point symmetric Hamming window in a column vector.
% Description end
if nargin<2
    sflag = 'symmetric';
end
w = signal.cosine_based_window('hamming', N, sflag);