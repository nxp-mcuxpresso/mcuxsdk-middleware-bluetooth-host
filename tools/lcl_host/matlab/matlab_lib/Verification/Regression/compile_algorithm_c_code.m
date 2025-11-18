%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   In Linux: this script compiles the Music-algorithm C-code and Channel-reconstruction
%   C-code and copies it to a specific directory.
%   In Windows: this script copies the the compiled files to a specific
%   directory. The compilation is done in Jenkins on a machine with special 
%   build environment.
% Description end
CurDir = cd;
[RootDir,~,~] = fileparts(which(mfilename));

logger = log4m.getLogger(fullfile(RootDir, 'Logging', [mfilename, '.log']));
logger.setCommandWindowLevel(logger.TRACE);

logger.trace('Set current working directory to %s', RootDir);
cd(RootDir);

if ispc
    Src = '\\unix\nrbrpoc\projectdata\build_artifacts\default\*.mexw64';
    Dst = fullfile('..', '..', 'NRBLOC', '5_RangingEngine');
    
    logger.trace('Unload mex-files');
    clear mex
    
    [status, msg, msgID] = copyfile(Src, Dst);
    if ~status
        error(msgID, msg);
    end
elseif isunix
    logger.trace('Machine is of Unix-type');
    [~, hostname] = system('hostname');
    logger.trace('Hostname is %s', hostname);

    
    logger.trace('Unload mex-files');
    clear mex
    
    SWRootDir = fullfile(RootDir, '..', '..', '..', '..', 'software','algo','ranging');
    logger.trace('SW-directory is at %s', SWRootDir);

    cd(SWRootDir);
    system('./build_mex.sh');
    cd(CurDir);
else
    error('Only windows or Linux machines are supported');
end 

cd(CurDir);