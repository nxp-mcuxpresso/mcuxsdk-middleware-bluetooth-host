function Result = GetScaAntDivData(uc_idx,main_folder,channel_mask)
if nargin < 3
    channel_mask = ones(80,1);
end
if nargin < 2
    main_folder = 'Captures';
end
switch uc_idx
% LOS cases - alone (15 July 2021)
    case 1,  folder_name = 'SCA\2021_07_15\alone\'; file_name = 'Pos1';  true_distance = 3.10;    scenario = ''; 
    case 2,  folder_name = 'SCA\2021_07_15\alone\'; file_name = 'Pos2';  true_distance = 2.62;    scenario = '';
    case 3,  folder_name = 'SCA\2021_07_15\alone\'; file_name = 'Pos3';  true_distance = 2.14;    scenario = '';
    case 4,  folder_name = 'SCA\2021_07_15\alone\'; file_name = 'Pos4';  true_distance = 1.69;    scenario = '';
    case 5,  folder_name = 'SCA\2021_07_15\alone\'; file_name = 'Pos5';  true_distance = 1.27;    scenario = '';
    case 6,  folder_name = 'SCA\2021_07_15\alone\'; file_name = 'Pos6';  true_distance = 0.93;    scenario = '';
    case 7,  folder_name = 'SCA\2021_07_15\alone\'; file_name = 'Pos7';  true_distance = 0.78;    scenario = '';
    case 8,  folder_name = 'SCA\2021_07_15\alone\'; file_name = 'Pos8';  true_distance = 0.93;    scenario = '';
    case 9,  folder_name = 'SCA\2021_07_15\alone\'; file_name = 'Pos9';  true_distance = 1.27;    scenario = '';     
    case 10, folder_name = 'SCA\2021_07_15\alone\'; file_name = 'Pos10'; true_distance = 1.69;    scenario = '';
    case 11, folder_name = 'SCA\2021_07_15\alone\'; file_name = 'Pos11'; true_distance = 0.78;    scenario = ''; 
    case 12, folder_name = 'SCA\2021_07_15\alone\'; file_name = 'Pos11'; true_distance = 1.21;    scenario = '';   
    case 13, folder_name = 'SCA\2021_07_15\alone\'; file_name = 'Pos12'; true_distance = 1.67;    scenario = ''; 
    case 14, folder_name = 'SCA\2021_07_15\alone\'; file_name = 'Pos13'; true_distance = 2.16;    scenario = ''; 
    case 15, folder_name = 'SCA\2021_07_15\alone\'; file_name = 'Pos15'; true_distance = 2.64;    scenario = '';   
    case 16, folder_name = 'SCA\2021_07_15\alone\'; file_name = 'Pos16'; true_distance = 3.14;    scenario = ''; 
    case 17, folder_name = 'SCA\2021_07_15\alone\'; file_name = 'Pos17'; true_distance = 3.63;    scenario = '';
% LOS cases - front (15 July 2021)
    case 21, folder_name = 'SCA\2021_07_15\front\'; file_name = 'Pos1';  true_distance = 3.10;    scenario = ''; 
    case 22, folder_name = 'SCA\2021_07_15\front\'; file_name = 'Pos2';  true_distance = 2.62;    scenario = '';
    case 23, folder_name = 'SCA\2021_07_15\front\'; file_name = 'Pos3';  true_distance = 2.14;    scenario = '';
    case 24, folder_name = 'SCA\2021_07_15\front\'; file_name = 'Pos4';  true_distance = 1.69;    scenario = '';
    case 25, folder_name = 'SCA\2021_07_15\front\'; file_name = 'Pos5';  true_distance = 1.27;    scenario = '';
    case 26, folder_name = 'SCA\2021_07_15\front\'; file_name = 'Pos6';  true_distance = 0.93;    scenario = '';
    case 27, folder_name = 'SCA\2021_07_15\front\'; file_name = 'Pos7';  true_distance = 0.78;    scenario = '';
    case 28, folder_name = 'SCA\2021_07_15\front\'; file_name = 'Pos8';  true_distance = 0.93;    scenario = '';
    case 29, folder_name = 'SCA\2021_07_15\front\'; file_name = 'Pos9';  true_distance = 1.27;    scenario = '';     
    case 30, folder_name = 'SCA\2021_07_15\front\'; file_name = 'Pos10'; true_distance = 1.69;    scenario = '';
    case 31, folder_name = 'SCA\2021_07_15\front\'; file_name = 'Pos11'; true_distance = 0.78;    scenario = ''; 
    case 32, folder_name = 'SCA\2021_07_15\front\'; file_name = 'Pos11'; true_distance = 1.21;    scenario = '';   
    case 33, folder_name = 'SCA\2021_07_15\front\'; file_name = 'Pos12'; true_distance = 1.67;    scenario = ''; 
    case 34, folder_name = 'SCA\2021_07_15\front\'; file_name = 'Pos13'; true_distance = 2.16;    scenario = ''; 
    case 35, folder_name = 'SCA\2021_07_15\front\'; file_name = 'Pos15'; true_distance = 2.64;    scenario = '';   
    case 36, folder_name = 'SCA\2021_07_15\front\'; file_name = 'Pos16'; true_distance = 3.14;    scenario = ''; 
    case 37, folder_name = 'SCA\2021_07_15\front\'; file_name = 'Pos17'; true_distance = 3.63;    scenario = '';          
% NLOS cases - back (15 July 2021)
    case 41, folder_name = 'SCA\2021_07_15\back\';  file_name = 'Pos1';  true_distance = 3.10;    scenario = ''; 
    case 42, folder_name = 'SCA\2021_07_15\back\';  file_name = 'Pos2';  true_distance = 2.62;    scenario = '';
    case 43, folder_name = 'SCA\2021_07_15\back\';  file_name = 'Pos3';  true_distance = 2.14;    scenario = '';
    case 44, folder_name = 'SCA\2021_07_15\back\';  file_name = 'Pos4';  true_distance = 1.69;    scenario = '';
    case 45, folder_name = 'SCA\2021_07_15\back\';  file_name = 'Pos5';  true_distance = 1.27;    scenario = '';
    case 46, folder_name = 'SCA\2021_07_15\back\';  file_name = 'Pos6';  true_distance = 0.93;    scenario = '';
    case 47, folder_name = 'SCA\2021_07_15\back\';  file_name = 'Pos7';  true_distance = 0.78;    scenario = '';
    case 48, folder_name = 'SCA\2021_07_15\back\';  file_name = 'Pos8';  true_distance = 0.93;    scenario = '';
    case 49, folder_name = 'SCA\2021_07_15\back\';  file_name = 'Pos9';  true_distance = 1.27;    scenario = '';     
    case 50, folder_name = 'SCA\2021_07_15\back\';  file_name = 'Pos10'; true_distance = 1.69;    scenario = '';
    case 51, folder_name = 'SCA\2021_07_15\back\';  file_name = 'Pos11'; true_distance = 0.78;    scenario = ''; 
    case 52, folder_name = 'SCA\2021_07_15\back\';  file_name = 'Pos11'; true_distance = 1.21;    scenario = '';   
    case 53, folder_name = 'SCA\2021_07_15\back\';  file_name = 'Pos12'; true_distance = 1.67;    scenario = ''; 
    case 54, folder_name = 'SCA\2021_07_15\back\';  file_name = 'Pos13'; true_distance = 2.16;    scenario = ''; 
    case 55, folder_name = 'SCA\2021_07_15\back\';  file_name = 'Pos15'; true_distance = 2.64;    scenario = '';   
    case 56, folder_name = 'SCA\2021_07_15\back\';  file_name = 'Pos16'; true_distance = 3.14;    scenario = ''; 
    case 57, folder_name = 'SCA\2021_07_15\back\';  file_name = 'Pos17'; true_distance = 3.63;    scenario = '';    
% Not supported
    otherwise,  error('Use-case index not supported; 1-->17 ; 21-->37 ; 41-->57');
end

% Read data
load(fullfile(main_folder,folder_name,file_name));

% Map resultfile onto result
Result = resultfile;
for N = 1:length(Result)
    Result(N).Initiator.I = real(resultfile(N).mciq__initiator__iq).*channel_mask;
    Result(N).Initiator.Q = imag(resultfile(N).mciq__initiator__iq).*channel_mask;
    Result(N).Reflector.I = real(resultfile(N).mciq__reflector__iq).*channel_mask;
    Result(N).Reflector.Q = imag(resultfile(N).mciq__reflector__iq).*channel_mask;
    Result(N).D_SRDE = Result(N).mciq__result__distance + 0.5;
    Result(N).D_CDE = Result(N).mciq__result__CDE_distance;
    Result(N).Result.Distance = Result(N).mciq__result__distance + 0.5;
    Result(N).Result.CdeDistance = Result(N).mciq__result__CDE_distance;
end

% 1W,2W channel
cases_idx = 1:length(Result);
for N = cases_idx
    iq_in = complex(Result(N).Initiator.I,Result(N).Initiator.Q);
    iq_re = complex(Result(N).Reflector.I,Result(N).Reflector.Q);
    ch_2w = iq_in.*iq_re;
    ch_1w = imec_calc_h_2D_est(iq_in,iq_re);
    Result(N).ch_1w = ch_1w;
    Result(N).ch_2w = ch_2w;
    Result(N).true_distance = true_distance+1.5;
    Result(N).scenario = scenario;
    Result(N).folder_name = folder_name;
    Result(N).iq_in = iq_in;
    Result(N).iq_re = iq_re;
end
% eval(sprintf('save %s_mod Result;',erase(fullfile(main_folder,folder_name,file_name),'.mat')));