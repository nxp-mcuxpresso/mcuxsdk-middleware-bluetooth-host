%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
function dir_tree = recurseDirs(path,dir_tree)
% Description
%   Recurse through directory tree.
% Description end


%input argument checking
if(nargin==1)
    dir_tree  = {path};
elseif(nargin<1 || nargin>2)
    path='.';
    dir_tree = {path};
end

dirs = cellfun(@(x) fullfile(path,x),listDirs(path),'UniformOutput',false);

%recurse into subdirectories
for i=1:length(dirs)
    dir_tree = [dir_tree dirs(i)]; %#ok<AGROW>
    dir_tree = recurseDirs(char(dirs(i)),dir_tree);
end