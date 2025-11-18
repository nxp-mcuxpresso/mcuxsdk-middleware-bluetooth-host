%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
function proj_dirs_ca = findAndFilterProjectDirs(proj_id_str)
% Description
%   Determine all project-related DBB model directories.
% Description end


proj_dirs_ca = [];

%input argument check
if(nargin~=1)
    fprintf('Usage: findAndFilterProjectDirs(proj_id_str)\n');
    return;
end

%settings
%dirs_to_exclude = {'.svn','@','private','archive','generated_doc'};
dirs_to_exclude = {'.svn','@','private','generated_doc', '+', '.git'};
max_dir_depth = 3; %exclude sub-directories below this directory depth

%set path to helper functions               
base_dir = pwd;
path(path,fullfile(base_dir,'helpers'));
                              
%load path config from file
proj_cfg_file = [proj_id_str '_paths.cfg'];
project_dirs_ca = parseTextCfgFile(proj_cfg_file);

%find all sub-directories and filter results
all_flow_dirs = {};

for project_dir_id=1:numel(project_dirs_ca)
    %determine all sub-dirs but remove those containing elements from the
    %exclude list
    flow_dirs = recurseDirs(project_dirs_ca{project_dir_id});
    for filter_id=1:numel(dirs_to_exclude)
        idx_remove = findStringInCA(flow_dirs,dirs_to_exclude{filter_id});
        flow_dirs(idx_remove) = [];
    end

    %filter by directory depth
    dir_depth = cellfun(@(x) numel(regexp(x,'[\\\/]')),flow_dirs);
    flow_dirs(dir_depth>max_dir_depth) = [];
    
    all_flow_dirs = [all_flow_dirs flow_dirs]; %#ok<AGROW>
end
            
%return directories as cell array
proj_dirs_ca = all_flow_dirs;