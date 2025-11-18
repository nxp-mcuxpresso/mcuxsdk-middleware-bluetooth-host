%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
function [found_idx] = findStringInCA(cell_array_to_search,search_string)
% Description
%   Find occurence of string in a cell array of strings.
% Description end

strf_res_ca = cellfun(@(x) strfind(x,search_string),cell_array_to_search,'UniformOutput',false);
strf_res_ne_ca =  cellfun(@(x) ~isempty(x),strf_res_ca);
found_idx = find(strf_res_ne_ca>0);
