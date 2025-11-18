%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
function printLog(lvl,varargin)
% Description
% Print a log message to the screen and/or to file.


 
PRINT_LOG_ERR_LEVEL = 2;


%input argument check
if(~isnumeric(lvl))
    error('Log message level must be numeric.');
end


if(lvl>=PRINT_LOG_ERR_LEVEL)
    error(varargin{:});
elseif lvl==1
    warning(varargin{:});
else
    fprintf(varargin{:});
end
