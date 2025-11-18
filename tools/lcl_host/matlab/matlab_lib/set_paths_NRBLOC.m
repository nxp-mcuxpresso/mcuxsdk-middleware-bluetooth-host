%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
function [pdp, pathset] = set_paths_NRBLOC(varargin)
% Description
% Sets all required paths for the project.

persistent IsPathSet

CurrentPath = pwd;
WorkPath = CurrentPath;
pathset = false;

for n=1:nargin
    if isscalar(varargin{n}) && varargin{n} == true
        IsPathSet = [];
    elseif ischar(varargin{n})
        WorkPath = fullfile(CurrentPath, varargin{n});
    end
end

if isempty(IsPathSet)
    IsPathSet = true;
    cd(WorkPath);
    %to be sure: unset paths  first
    unset_paths

    %determine project ID string from filename
    m_file_name_parts_ca = regexp(mfilename(),'_','split');
    proj_id_str = m_file_name_parts_ca{end};
    proj_id_str=lower(proj_id_str);%unix is case-sensitive, windows is more tolerant
    %settings
    print_header =  ['[set_paths_' proj_id_str '] '];

    %set path to helper functions
    base_dir = pwd;
    path(path,fullfile(base_dir,'helpers'));

    %use helper function to determine project directories
    all_flow_dirs = findAndFilterProjectDirs(proj_id_str);

    %apply new path settings
    cellfun(@(x) path(path,fullfile(base_dir,x)),all_flow_dirs,'UniformOutput',false);
    fprintf('%s%s project paths set (added %d directories).\n',...
        print_header, upper(proj_id_str), numel(all_flow_dirs));

    addpath(genpath(fullfile(base_dir,'ThirdParty')));
    addpath(genpath(fullfile(base_dir,'secure_ranging')));
    addpath(fullfile(base_dir,'Verification', 'Regression'));

    % % compile the mex functions
    % c_files = dir(fullfile('mex', '*.c'));
    % for idx=1:length(c_files)
    %     fname = fullfile('mex', c_files(idx).name);
    %     [m_path, m_name, m_ext] = fileparts(fname);
    %     if ~(exist (fullfile(m_path, [m_name '.mexa64']), 'file')) 
    %         mex(fname);
    %         movefile([m_name, '.mexa64'], 'mex');
    %     end
    % end
    pathset = true;
    cd(CurrentPath);
end

if ispc
    pdp = '\\unix\nrbrpoc\projectdata';
elseif isunix
    pdp =  '/imec/other/nrbrpoc/projectdata';
else
    error('Sorry, cannot work in your operating system');
end

