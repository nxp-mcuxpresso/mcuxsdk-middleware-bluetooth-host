%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
% Description
%     This provides a known seed for the random number generator.
% Description end
classdef SeedRng < matlab.unittest.TestCase
    properties                                                              % These properties are initialized in the class-constructor
        InitRngSeed
        Logger
    end
    
    methods (TestClassSetup)                                                % These functions will be called once, at the start of the test-run
    end
    
    methods (TestClassTeardown)                                             % These functions will be called once, at the end of the test-run
    end
    
    methods (TestMethodSetup)                                               % These functions will be called before running any function in the Test-methods group
        function MethodSetup(self)
           % This function seeds the random number generator before
           % execution of any test-function. This is done to get repeatable
           % results, independent of which or how many functions are
           % executed.
           rng(self.InitRngSeed);
%            fprintf('     The random number generator is seeded with %d\n', self.InitRngSeed);
        end
    end
    
    methods (TestMethodTeardown)                                            % These functions will be called after running any function in the Test-methods group
    end
    
    methods                                                                 % These functions are regular function which can be called from any other function. It also contains the class-constructor
        function self = SeedRng(varargin)
            p = inputParser;
            p.KeepUnmatched = true;
            p.addOptional('MySeed', []);
            p.parse(varargin{:});
            if isempty(p.Results.MySeed)
                rng('shuffle');
                s = rng('shuffle');                                             % For one or another reason, this extra call is needed to get a random seed directly after startup of Matlab.
                s = s.Seed;
            else
                s = p.Results.MySeed;
            end
            self.InitRngSeed = s;
            % Make directory if it does not exist
            if ~exist(fullfile('.','Logging'),'dir')
                mkdir('Logging')
            end            

            self.Logger = log4m.getLogger(fullfile('Logging', [mfilename, '.log']));
            if strcmpi(self.Logger.fullpath, fullfile('Logging', [mfilename, '.log']))      % if the logger is created by this function
                self.Logger.setCommandWindowLevel(self.Logger.INFO);
                self.Logger.setLogLevel(self.Logger.INFO);
            end

            self.Logger.info('==================================================================');
            self.Logger.info(sprintf('     The seed for %s will be %d', mfilename('class'), s));
            self.Logger.info('==================================================================');
        end
        
        function self = SetSeed(self, MySeed)
            self.InitRngSeed = MySeed;
            rng(MySeed);
            self.Logger.info(sprintf('     The seed for %s will be %d', mfilename('class'), MySeed));
        end
    end

    
end