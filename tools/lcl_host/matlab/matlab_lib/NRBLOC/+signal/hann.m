%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
function w = hann(N, sflag)
% Description
%   hann(N) returns the N-point symmetric Hann window in a column vector.
% Description end
if nargin<2
    sflag = 'symmetric';
end
w = signal.cosine_based_window('hann', N, sflag);