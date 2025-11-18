%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%   LOG4M This is a simple logger based on the idea of the popular log4j.
% Description end

classdef log4m < handle 
    properties (Constant)
        ALL   = 0;
        TRACE = 1;
        DEBUG = 2;
        INFO  = 3;
        WARN  = 4;
        ERROR = 5;
        FATAL = 6;
        OFF   = 7;
    end
        
    properties(Access = protected)
        logger;
        lFile;
    end

    properties
        LoggerName;
    end
    
    properties(SetAccess = protected)
        fullpath = 'log4m.log';  %Default file
        commandWindowLevel = log4m.INFO;
        logLevel = log4m.OFF;
    end
    
    methods (Static)
        function obj = getLogger( logPath, LoggerName)
            %GETLOGGER Returns instance unique logger object.
            %   PARAMS:
            %       logPath - Relative or absolute path to desired logfile.
            %   OUTPUT:
            %       obj - Reference to singular logger object.
            %
            
            if(nargin == 0)
                logPath = '';
                LoggerName = 'log4m';
            elseif nargin == 1
                LoggerName = 'log4m';
            elseif(nargin > 2)
                error('getLogger only accepts one parameter input');
            end
            
            persistent localObj;
            if isempty(localObj) || ~isvalid(localObj)
                localObj = log4m(logPath, LoggerName);
            end
            obj = localObj;
        end
        
        function testSpeed( logPath )
            %TESTSPEED Gives a brief idea of the time required to log.
            %
            %   Description: One major concern with logging is the
            %   performance hit an application takes when heavy logging is
            %   introduced. This function does a quick speed test to give
            %   the user an idea of how various types of logging will
            %   perform on their system.
            %
            
            L = log4m.getLogger(logPath);
            
            
            disp('1e5 logs when logging only to command window');
            
            L.setCommandWindowLevel(L.TRACE);
            L.setLogLevel(L.OFF);
            tic;
            for i=1:1e5
                L.trace('log4mTest','test');
            end
            
            disp('1e5 logs when logging only to command window');
            toc;
            
            disp('1e6 logs when logging is off');
            
            L.setCommandWindowLevel(L.OFF);
            L.setLogLevel(L.OFF);
            tic;
            for i=1:1e6
                L.trace('log4mTest','test');
            end
            toc;
            
            disp('1e4 logs when logging to file');
            
            L.setCommandWindowLevel(L.OFF);
            L.setLogLevel(L.TRACE);
            tic;
            for i=1:1e4
                L.trace('log4mTest','test');
            end
            toc;
            
        end
        
        function CallerFunction = get_caller_function()
            temp = dbstack();
            CallerFunction = temp(3).name;
            temp = strsplit(CallerFunction, '.');
            CallerFunction = temp{end};
        end
    end
    
    
%% Public Methods Section %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    methods       
        function setFilename(self, logPath)
            %SETFILENAME Change the location of the text log file.
            %
            %   PARAMETERS:
            %       logPath - Name or full path of desired logfile
            %
            if ~isempty(logPath)
                [logRootDir, ~, ~] = fileparts(logPath);
                if ~isempty(logRootDir) && ~exist(logRootDir,'dir')
                    mkdir(logRootDir);
                end
                
                [fid,message] = fopen(logPath, 'a');

                if(fid < 0)
                    error(['Problem with supplied logfile path: ' message]);
                end
                fclose(fid);

                self.fullpath = logPath;
            end
        end
        
        function setLoggerName(self, LoggerName)
            self.LoggerName = LoggerName;
        end         
     
        function setCommandWindowLevel(self,loggerIdentifier)
            self.commandWindowLevel = loggerIdentifier;
        end


        function setLogLevel(self,logLevel)
            self.logLevel = logLevel;
        end
        

%% The public Logging methods %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        function trace(self, varargin)
            %TRACE Log a message with the TRACE level
            %
            %   PARAMETERS:
            %       funcName - Name of the function or location from which
            %       message is coming.
            %       message - Text of message to log.
            % 
            funcName = self.get_caller_function();
            self.writeLog(self.TRACE,funcName,varargin{:});
        end
        
        function debug(self, varargin)
            %TRACE Log a message with the DEBUG level
            %
            %   PARAMETERS:
            %       funcName - Name of the function or location from which
            %       message is coming.
            %       message - Text of message to log.
            % 
            funcName = self.get_caller_function();
            self.writeLog(self.DEBUG,funcName,varargin{:});
        end
        
 
        function info(self, varargin)
            %TRACE Log a message with the INFO level
            %
            %   PARAMETERS:
            %       funcName - Name of the function or location from which
            %       message is coming.
            %       message - Text of message to log.
            % 
            funcName = self.get_caller_function();
            self.writeLog(self.INFO, funcName, varargin{:});
        end
        

        function warn(self, varargin)
            %TRACE Log a message with the WARN level
            %
            %   PARAMETERS:
            %       funcName - Name of the function or location from which
            %       message is coming.
            %       message - Text of message to log.
            % 
            funcName = self.get_caller_function();
            self.writeLog(self.WARN,funcName, varargin{:});
        end
        
        function warning(self, varargin)
            self.warn(varargin{:});
        end       

        function error(self, varargin)
            %TRACE Log a message with the ERROR level
            %
            %   PARAMETERS:
            %       funcName - Name of the function or location from which
            %       message is coming.
            %       message - Text of message to log.
            % 
            funcName = self.get_caller_function();
            self.writeLog(self.ERROR,funcName,varargin{:});
        end
        

        function fatal(self, varargin)
            %TRACE Log a message with the FATAL level
            %
            %   PARAMETERS:
            %       funcName - Name of the function or location from which
            %       message is coming.
            %       message - Text of message to log.
            % 
            funcName = self.get_caller_function();
            self.writeLog(self.FATAL,funcName,varargin{:});
        end
        
    end

%% Private Methods %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   Unless you're modifying this, these should be of little concern to you.
    methods (Access = private)
        
        function self = log4m(fullpath_passed, loggername)
            if (nargin > 0)
                path = fullpath_passed;
            end
            self.setFilename(path);
            self.setLoggerName(loggername);
        end
        
%% WriteToFile        
        function writeLog(self,level,scriptName,varargin)
            
            % If necessary write to command window
            if( self.commandWindowLevel <= level )
                if level<self.WARN
                    outID = 1;
                else
                    outID = 2;
                end
                MsgStr = sprintf(varargin{:});
                fprintf(outID, '%s - %s: %s\n', datestr(now, 31), scriptName, MsgStr);
            end
            
            %If currently set log level is too high, just skip this log
            if(self.logLevel > level)
                return;
            end 
            
            % set up our level string
            switch level
                case{self.TRACE}
                    levelStr = 'TRACE';
                case{self.DEBUG}
                    levelStr = 'DEBUG';
                case{self.INFO}
                    levelStr = 'INFO';
                case{self.WARN}
                    levelStr = 'WARN';
                case{self.ERROR}
                    levelStr = 'ERROR';
                case{self.FATAL}
                    levelStr = 'FATAL';
                otherwise
                    levelStr = 'UNKNOWN';
            end

            % Append new log to log file
            if ~isempty(self.fullpath)
                try
                    MsgStr = sprintf(varargin{:});
                    fid = fopen(self.fullpath,'a');
                    fprintf(fid,'%s %s %s - %s\r\n' ...
                        , datestr(now,'yyyy-mm-dd HH:MM:SS') ...
                        , levelStr ...
                        , scriptName ... % Have left this one with the '.' if it is passed
                        , MsgStr);
                    fclose(fid);
                catch ME_1
                    display(ME_1);
                end
            end
        end
    end
end

