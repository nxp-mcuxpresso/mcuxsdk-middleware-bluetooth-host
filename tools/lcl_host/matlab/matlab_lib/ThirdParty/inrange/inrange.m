%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
function y = inrange(X,R,varargin)

%Version 2: Per online comment from "Jos x" (thanks!), fixed the following:
% 1) no need for find, use logical indexing;  DONE
% 2) return a logical array (false/true) instead of a zero/one array; DONE
% 3) for a 1x2 input R you do no need the repmat, as you split it into two
%    scalars; DONE
% 4) reduce overhead: first get the size of X, than use X = X(:), and
%    finally reshape Y using the stored size of X; DONE
% 5) why not make a default value for boundary DONE
% 6) it may return some unexpected results when LOW==HIGH;  I DONT SEE IT,
%    but will add a comment.
% 7) Take a look at submission #9428 by John D'Errico how to implement
%    "boundary" effectively.  SORRY, TOO LAZY.
%
% Version 3:
%  removed some MLINT warnings by Pepijn Boer

narginchk(2,3);
if nargin==2
    boundary = 'includeboth';
else
    boundary = varargin{1};
end


XoriginalDim = size(X);
X = X(:);

if numel(R) ~= 2
    if ~isequal(size(R,2),2)
        error('RANGE input has too many columns.')
    end
    if ~isequal(size(R,1),numel(X))
        error('If RANGE is a matrix, X must be a vector of same length.')
    end
end

leftBound = R(:,1);
rightBound = R(:,2);
if any(leftBound > rightBound)
    error('Rows of RANGE must have form [LOW HIGH].')
end

switch boundary
    case 'includeboth'
        inRangeIX = (X >= leftBound) & (X <= rightBound);
    case 'includeleft'
        inRangeIX = (X >= leftBound) & (X < rightBound);
    case 'includeright'
        inRangeIX = (X > leftBound) & (X <= rightBound);
    case 'excludeboth'
        inRangeIX = (X > leftBound) & (X < rightBound);
    otherwise
        error('Valid options for third input are ''includeboth'', ''includeleft'', ''includeright'', ''excludeboth''.')
end

y = reshape(inRangeIX,XoriginalDim);

