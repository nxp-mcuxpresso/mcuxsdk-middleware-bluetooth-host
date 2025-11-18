%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
function cfg_ca = parseTextCfgFile(f_name)
% Description
%   Load (configuration) text file and return lines as cell array.
% Description end

try
    fHandle = fopen(f_name,'r');
catch
    error('%s ERROR: Could not open input file.\n',f_name);
end

%if no exception is thrown but we still get a fid == -1 we have a problem
if (fHandle == -1)
    error('%s ERROR: Could not open input file.\n', f_name);  
end

%parse file
data_ca = textscan(fHandle,'%s','delimiter','\n');

if(isempty(data_ca))
    fclose(fHandle);
    error('%s ERROR: Could not read from input file.\n', f_name);  
end

cfg_ca = data_ca{1};  