%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
function h = sfigure(h)
% Description
%   downloaded from https://nl.mathworks.com/matlabcentral/fileexchange/
%   SFIGURE  Create figure window (minus annoying focus-theft).
%       Usage is identical to figure.
% Description end

if nargin>=1 
	if ishandle(h)
		set(0, 'CurrentFigure', h);
	else
		h = figure(h);
	end
else
	h = figure;
end