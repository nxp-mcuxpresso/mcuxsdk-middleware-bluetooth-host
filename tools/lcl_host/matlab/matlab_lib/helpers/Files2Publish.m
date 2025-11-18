%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
function []=Files2Publish(TOP, varargin)
% Description
%   Often the version control repository contains more data than needed for a deliverable.
%   This m-file determines automatically, which m-files are needed to run a Top-level m-file and
%   copies them to the system's temp-directory, while preserving the directory-structure
%   Additionally, it is checked whether all mandatory fields in the m-files
%   comments are there. Only if this is true, the code is 'published'.
% Description end

ROOT=which(mfilename);
cd(fileparts(ROOT))
ROOT=ROOT(1:strfind(ROOT,[filesep 'helpers' filesep 'Files2Publish.m'])-1);

ExtraFilesLegacy = {fullfile(ROOT, 'nrbloc_paths.cfg'),... 
                    fullfile(ROOT, 'helpers', 'printLog.m'),...
                    fullfile(ROOT, 'NRBLOC', '2_Channel','WP1_Meas','Meas_1','WP1_P051_ChannelData.mat')};

parser               = inputParser();
parser.KeepUnmatched = true;
parser.addParameter('HeaderCheck', true, @(x) islogical(x));
parser.addParameter('GITCheck', true, @(x) islogical(x));
parser.addParameter('ExtraFiles', ExtraFilesLegacy, @(x) iscell(x));
parser.parse(varargin{:})

HeaderCheck = parser.Results.HeaderCheck;
GITCheck = parser.Results.GITCheck;
ExtraFiles = parser.Results.ExtraFiles;

if exist('unset_paths.m','file')
    cd(fileparts(which('unset_paths.m')))
    unset_paths
end

% The compilation of C-code requires the software-tree. This will not be
% copied to the temp-dir. Therefore the compile_algorithm_c_code-file
% is removed from TOP, such that it will not be tested.
TOPorg = TOP;
TOP(contains(TOP, 'compile_algorithm_c_code', 'IgnoreCase', true)) = [];    %#ok<NASGU>

NEWROOT=fullfile(tempdir,'NRBLOC', [ROOT(find(ROOT==filesep,1,'last')+1:end) '_pub']); % NRBRPOC must be in this path, because unset_paths triggers on this
VarsToSave = {'ROOT', 'NEWROOT', 'TOP', 'HeaderCheck', 'ExtraFiles'};
save(fullfile(tempdir,'TempVars.mat'), VarsToSave{:});
TOP = TOPorg;
%It could be that the TOP m-file clears the workspace!

% Start is the Source project
cd(ROOT)
set_paths_NRBLOC(true);
if GITCheck
    [OK, Git_HASH] = SCM_check();
    if ~OK
        printLog(0, 'Files2Publish] Unable to publish, Local repository not commited or pushed ...........................\n');
        printLog(2, Git_HASH);
    end
end

profile on
for k=1:length(TOP)
    fprintf('[Files2Publish] Executing "%s"\n', TOP{k});
    eval(TOP{k});
end
profile off
S = profile('INFO');

load(fullfile(tempdir,'TempVars.mat')); %#ok<LOAD> % This load is needed, because the NRBRLOC_simulator is a script and clears the workspace

% Extract all m-files used by TOP
FileNames = {};
for cnt=1:numel(S.FunctionTable)
    if ~sum(strcmp(S.FunctionTable(cnt).Type,{'M-anonymous-function','Java-method'}))
        filename = S.FunctionTable(cnt).CompleteName;
        [D,~,~] = fileparts(filename);
        if startsWith(D, ROOT, 'IgnoreCase', true)
            I = strfind(filename,'>');
            if ~isempty(filename)
                filename = filename(1:I-1);
            end
            FileNames{end+1} = filename;                                    %#ok<AGROW>
        end
    end
end

% Additional files to be published can be added here!
FileNames = [FileNames, ExtraFiles];
FileNames = unique(FileNames);% Many files are multiple, as the profiler also keeps track of which methods are used!

if HeaderCheck
    % exclude files from ThirdParty-folder
    indx_TP = contains(FileNames, 'ThirdParty');
    OK = CheckHeaders(FileNames(~indx_TP), 'ShowWarning', false, 'Check_Description', false);
    if ~OK
        printLog(2, '[Files2Publish] Unable to publish, errors in headers')
    end
end

printLog(0,'[Files2Publish] Passed all checks \n')

if exist(NEWROOT,'dir')
    printLog(0,'[Files2Publish] erasing target directory .... ')
    [status, msg, msgid] = rmdir(NEWROOT, 's');
    if status
        printLog(0,' [done] \n')
    else
        printLog(2, msgid, msg);
    end
end

for cnt=1:length(FileNames)
    SOURCE=FileNames{cnt};
    DESTINATION=strrep(SOURCE,ROOT,NEWROOT);
    CreateDirTree(fileparts(DESTINATION));
    [~,~,~] = copyfile(SOURCE,DESTINATION);
end

unset_paths
close all
clear
clear classes                                                               %#ok<CLCLS>
load(fullfile(tempdir,'TempVars.mat'));                                     %#ok<LOAD>
cd(NEWROOT)
try
    for k=1:length(TOP)
        fprintf('[Files2Publish] Executing "%s"\n', TOP{k});
        eval(TOP{k});
    end
    TXT2APPEND='with no error';
catch ME
    if isempty(ME.identifier)
        error(ME.message);
    else
        error(ME.identifier, ME.message);       % if ME.identifier is empty, no error is fired
    end
end
load(fullfile(tempdir,'TempVars.mat'));  %#ok<LOAD>                         % This load is needed, because the NRBRLOC_simulator is a script and clears the workspace
fprintf('[Files2Publish] published in %s %s\n', NEWROOT, TXT2APPEND)

cd(ROOT);
set_paths_NRBLOC(true);
[~,Git_HASH] = SCM_check();
cd(NEWROOT);
fid = fopen([NEWROOT filesep 'readme.txt'],'a');
fwrite(fid,['GIT version:' Git_HASH ]);
fclose(fid);

end

%%%%

function STATUS = CreateDirTree(pathstr)
if isfolder(pathstr)
    STATUS=true;
else
    I=find(pathstr==filesep,1,'last');
    pathstrTMP=pathstr(1:I-1);
    
    STATUS=CreateDirTree(pathstrTMP);
    if STATUS
        mkdir(pathstr)
        STATUS=true;
    else
        error('???')
    end
    
end
end


