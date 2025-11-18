%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
function w = cosine_based_window(windowtype, N, sflag)
% Description
%   This function implements shared functionality between the 
%   window-functions 'signal.hann', 'signal.hamming', 'signal.blackman' 
%   and 'signal.blackmanharris'.
%   It is not intented to be called directly, please use the before 
%   mentioned window functions.
% Description end
if nargin<2
    sflag = 'symmetric';
end

% Check inputs
validateattributes(windowtype, {'char'}, {}, mfilename, 'windowtype', 1);
validateattributes(N, {'numeric'}, {'real','finite', 'scalar'}, mfilename, 'N', 2);
validateattributes(sflag, {'char'}, {}, mfilename, 'sflag', 3);

sflag = lower(sflag);
assert(ismember(sflag, {'symmetric', 'periodic'}), sprintf('sflag is expected to be either "symmetric" or "periodic", but is %s instead', sflag));
windowtype = lower(windowtype);
assert(ismember(windowtype, {'hann', 'hamming', 'blackman', 'blackmanharris'}), sprintf('windtype is expected to be either, "hann", "hamming", "blackman", "blackharris", but it is %s instead.', windowtype));

% Check trivial cases
if N==0
    w = zeros(0,1); % identical to hann
    return
elseif N==1
    w = 1;
    return
end

issym = strcmp(sflag, 'symmetric');
if ~issym
    N = N+1;
end
isodd = mod(N,2);

if isodd && issym
    L = (N+1)/2;
    indx_offset_end = 1;
    indx_offset_start = 1;
elseif ~isodd && issym
    L = N/2;
    indx_offset_end = 0;
    indx_offset_start = 1;
elseif isodd && ~issym
    L = (N+1)/2;
    indx_offset_end = 1;
    indx_offset_start = 2;
else
    L = N/2;
    indx_offset_end = 0;
    indx_offset_start = 2;
end
w = calc_half_window(L, N, windowtype);
w = [w; w(end-indx_offset_end:-1:indx_offset_start)];
end

function w = calc_half_window(L, N, windowtype)
t = (0:L-1)'/(N-1);
switch windowtype
    case 'hann'
        w = 0.5  - 0.5 *cos(2*pi*t);
    case 'hamming'
        w = 0.54 - 0.46*cos(2*pi*t);
    case 'blackman'
        w = 0.42 - 0.5*cos(2*pi*t) + 0.08*cos(4*pi*t);
    case 'blackmanharris'
        a = [0.35875 0.48829 0.14128 0.01168];
        w = a(1) - a(2)*cos(2*pi*t) + a(3)*cos(4*pi*t) - a(4)*cos(6*pi*t);
end
end
