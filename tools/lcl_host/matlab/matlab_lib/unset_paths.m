%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
function unset_paths()
% Description
%   Remove project paths from MATLAB path settings.
% Description end

%settings
path_to_remove_str = 'NRBLOC';

%set path to helper functions               
base_dir = fileparts(which('unset_paths'));
path(path,fullfile(base_dir,'helpers'));

%tokenize path string
path_str = path();
if(isunix)
    path_ca = regexp(path_str,':','split');
else
    path_ca = regexp(path_str,';','split');
end

%find path items to remove
path_items_to_remove_idx = findStringInCA(path_ca,path_to_remove_str);

%remove those items from path
for p_id=1:numel(path_items_to_remove_idx)
    rmpath(path_ca{path_items_to_remove_idx(p_id)});
end
warning('off', 'MATLAB:rmpath:DirNotFound');
rmpath(fullfile(base_dir,'helpers'))
rmpath(genpath(fullfile(base_dir,'ThirdParty')));
rmpath(genpath(fullfile(base_dir,'secure_ranging')));
rmpath(fullfile(base_dir,'Verification', 'Regression'));
rmpath(base_dir);
warning('on', 'MATLAB:rmpath:DirNotFound');


