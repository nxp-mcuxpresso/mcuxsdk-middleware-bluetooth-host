%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
function dir_list = listDirs(path)
% Description
%   List all directories in a given path.
% Description end


%check input parameters
if (nargin<1)
    path = '.';
elseif (nargin>1)
    fprintf('Usage: listDirs(path)\n');
    return;
end

d = dir(path);
is_dir = [d.isdir];
dirs = {d(is_dir).name};
filter_out = cellfun(@(x) strcmp(x,'.'),dirs);
filter_out = filter_out | cellfun(@(x) strcmp(x,'..'),dirs);
dir_list = dirs(~filter_out);