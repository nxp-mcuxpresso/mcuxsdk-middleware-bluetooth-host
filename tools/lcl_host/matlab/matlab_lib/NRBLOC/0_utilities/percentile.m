%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This function returns the percentile pct of the values in arr.
% Description end


function val = percentile(arr, pct)

% remove all nan's
arr(isnan(arr)) = [];
if isempty(arr)
    val = NaN;
else
    len = length(arr);
    ind = ceil(pct/100*len);        % According to the nearest rank method described in https://en.wikipedia.org/wiki/Percentile, this should be a 'ceil'
    newarr = sort(arr);
    val = newarr(ind);
end
end