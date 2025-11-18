%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   This fixture sets some default settings
% Description end

classdef setupEnv < matlab.unittest.fixtures.Fixture
    properties
        Logger
    end
    methods
        function setup(self)
            % Make directory if it does not exist
            if ~exist(fullfile('.','Logging'),'dir')
                mkdir('Logging')
            end            

            self.Logger = log4m.getLogger(fullfile('Logging', [mfilename, '.log']));
            if strcmpi(self.Logger.fullpath, fullfile('Logging', [mfilename, '.log']))      % if the logger is created by this function
                self.Logger.setCommandWindowLevel(self.Logger.INFO);
                self.Logger.setLogLevel(self.Logger.INFO);
            end
            
            import matlab.unittest.fixtures.CurrentFolderFixture
            import matlab.unittest.fixtures.SuppressedWarningsFixture
%             import matlab.unittest.fixtures.PathFixture
            
            W = which('set_paths_NRBLOC');
            [BaseDir, ~, ~] = fileparts(W);                                 % Must run from this dir, because many scripting is using this dir as base
            self.applyFixture( CurrentFolderFixture( fullfile(BaseDir, 'Verification', 'Regression' ) ) );
            self.applyFixture( SuppressedWarningsFixture({'MATLAB:hg:AutoSoftwareOpenGL'}) );
%             self.applyFixture( PathFixture(BaseDir, 'IncludingSubfolders', true) );
            set(0,'DefaultAxesYgrid','on')          % Enable grid
            set(0,'DefaultAxesXgrid','on')          % Enable grid
            set(0,'DefaultAxesColorOrder',[  0,  48, 255;                   % blue
                                           255,   0,   0;                   % red
                                             0, 234, 117;                   % green
                                           255, 128,   0;                   % orange
                                           170,   0, 255;                   % purple
                                           210,   0,   0;                   % dark red
                                             0, 207, 255;                   % light blue
                                             0, 159,   0;                   % dark green
                                             0,   0, 128;                   % dark blue
                                                        ]/255)
                                                    
            % Create results-directory if it does not exist yet
            ResultDir = fullfile('Results');
            if ~exist(ResultDir,'dir')
                mkdir(ResultDir);
            end
            
            close all
        end
    end
end

