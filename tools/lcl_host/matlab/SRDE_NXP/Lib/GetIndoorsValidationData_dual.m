function Result = GetIndoorsValidationData_dual(x,y,main_folder,channel_mask)
if nargin < 4
    channel_mask = ones(80,1);
end
if nargin < 3
    main_folder = '..\Captures\Validation\FR_small_meeting_room_B126\MR1_LCL_RC1_dual';
end

folder_name = '';
distance = abs(complex(x,y));
true_distance = distance + 1.5;
str = sprintf('x_%dp%d_y_%dp%d',floor(x),floor((x-floor(x))*10),floor(y),floor((y-floor(y))*10));

% Read data
content = dir(fullfile(main_folder,folder_name)); 
file_name = '';
for m = 1:length(content)
    if ~isempty(strfind(content(m).name,'.mat')) && ~isempty(strfind(content(m).name,str))
        file_name = content(m).name; 
    end
end
if isempty(file_name)
    Result = [];
    return;
end
load(fullfile(main_folder,folder_name,file_name));
scenario = file_name; 

% 1W,2W channel
cases_idx = 1:length(Result);
for N = cases_idx
    iq_in = complex(Result(N).Initiator.I,Result(N).Initiator.Q).*channel_mask;
    iq_re = complex(Result(N).Reflector.I,Result(N).Reflector.Q).*channel_mask;
    ch_2w = iq_in.*iq_re;
    if 0
        ch_1w = Result(N).Result.Hest.';
    else
        ch_1w = imec_calc_h_2D_est(iq_in,iq_re);
    end
    Result(N).ch_1w = ch_1w;
    Result(N).ch_2w = ch_2w;
    Result(N).true_distance = true_distance;
    Result(N).scenario = scenario;
    Result(N).folder_name = folder_name;
    Result(N).iq_in = iq_in;
    Result(N).iq_re = iq_re;
end
% eval(sprintf('save %s_mod Result;',erase(fullfile(main_folder,folder_name,file_name),'.mat')));