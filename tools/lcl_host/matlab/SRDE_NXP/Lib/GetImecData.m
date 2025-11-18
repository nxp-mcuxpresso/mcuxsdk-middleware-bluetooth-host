function Result = GetImecData(uc_idx,main_folder,channel_mask)
if nargin < 3
    channel_mask = ones(80,1);
end
if nargin < 2
    main_folder = 'Captures';
end
switch uc_idx
% LOS cases - alone (06 July 2020 - parking lot)
    case 1,   folder_name = 'SCA\outside_20200706-imec\alone\11';     true_distance = 1.60;    scenario = 'LOS (parking lot) - alone - July 06';  
    case 2,   folder_name = 'SCA\outside_20200706-imec\alone\12';     true_distance = 2.10;    scenario = 'LOS (parking lot) - alone - July 06';
    case 3,   folder_name = 'SCA\outside_20200706-imec\alone\13';     true_distance = 2.50;    scenario = 'LOS (parking lot) - alone - July 06';
    case 4,   folder_name = 'SCA\outside_20200706-imec\alone\14';     true_distance = 3.00;    scenario = 'LOS (parking lot) - alone - July 06';
    case 5,   folder_name = 'SCA\outside_20200706-imec\alone\15';     true_distance = 3.50;    scenario = 'LOS (parking lot) - alone - July 06';
    case 6,   folder_name = 'SCA\outside_20200706-imec\alone\16';     true_distance = 4.00;    scenario = 'LOS (parking lot) - alone - July 06';
    case 7,   folder_name = 'SCA\outside_20200706-imec\alone\17';     true_distance = 4.50;    scenario = 'LOS (parking lot) - alone - July 06';
    case 8,   folder_name = 'SCA\outside_20200706-imec\alone\18';     true_distance = 5.00;    scenario = 'LOS (parking lot) - alone - July 06';    
    case 9,   folder_name = 'SCA\outside_20200706-imec\alone\19';     true_distance = 5.50;    scenario = 'LOS (parking lot) - alone - July 06';
    case 10,  folder_name = 'SCA\outside_20200706-imec\alone\20';     true_distance = 6.00;    scenario = 'LOS (parking lot) - alone - July 06';
% NLOS (backpocket) cases  (06 July 2020 - parking lot)       
    case 11,  folder_name = 'SCA\outside_20200706-imec\back\11';      true_distance = 1.60;    scenario = 'NLOS (parking lot) - backpocket - July 06'; 
    case 12,  folder_name = 'SCA\outside_20200706-imec\back\12';      true_distance = 2.10;    scenario = 'NLOS (parking lot) - backpocket - July 06';
    case 13,  folder_name = 'SCA\outside_20200706-imec\back\13';      true_distance = 2.50;    scenario = 'NLOS (parking lot) - backpocket - July 06';
    case 14,  folder_name = 'SCA\outside_20200706-imec\back\14';      true_distance = 3.00;    scenario = 'NLOS (parking lot) - backpocket - July 06';  
    case 15,  folder_name = 'SCA\outside_20200706-imec\back\15';      true_distance = 3.50;    scenario = 'NLOS (parking lot) - backpocket - July 06';
    case 16,  folder_name = 'SCA\outside_20200706-imec\back\16';      true_distance = 4.00;    scenario = 'NLOS (parking lot) - backpocket - July 06';
    case 17,  folder_name = 'SCA\outside_20200706-imec\back\17';      true_distance = 4.50;    scenario = 'NLOS (parking lot) - backpocket - July 06';
    case 18,  folder_name = 'SCA\outside_20200706-imec\back\18';      true_distance = 5.00;    scenario = 'NLOS (parking lot) - backpocket - July 06';
    case 19,  folder_name = 'SCA\outside_20200706-imec\back\19';      true_distance = 5.50;    scenario = 'NLOS (parking lot) - backpocket - July 06';
    case 20,  folder_name = 'SCA\outside_20200706-imec\back\20';      true_distance = 6.00;    scenario = 'NLOS (parking lot) - backpocket - July 06';
% LOS cases - alone (19 June 2020)
    case 101, folder_name = 'SCA\Results_IMEC_20200619\Alone-1';      true_distance = 3.10;    scenario = 'LOS - alone - June 19'; 
    case 102, folder_name = 'SCA\Results_IMEC_20200619\Alone-2';      true_distance = 2.60;    scenario = 'LOS - alone - June 19';
    case 103, folder_name = 'SCA\Results_IMEC_20200619\Alone-3';      true_distance = 2.15;    scenario = 'LOS - alone - June 19';
    case 104, folder_name = 'SCA\Results_IMEC_20200619\Alone-4';      true_distance = 1.75;    scenario = 'LOS - alone - June 19';
    case 105, folder_name = 'SCA\Results_IMEC_20200619\Alone-5';      true_distance = 1.60;    scenario = 'LOS - alone - June 19';
    case 106, folder_name = 'SCA\Results_IMEC_20200619\Alone-6';      true_distance = 1.75;    scenario = 'LOS - alone - June 19';
    case 107, folder_name = 'SCA\Results_IMEC_20200619\Alone-7';      true_distance = 2.15;    scenario = 'LOS - alone - June 19';
    case 108, folder_name = 'SCA\Results_IMEC_20200619\Alone-8';      true_distance = 2.60;    scenario = 'LOS - alone - June 19';
    case 109, folder_name = 'SCA\Results_IMEC_20200619\Alone-9';      true_distance = 3.10;    scenario = 'LOS - alone - June 19';     
    case 110, folder_name = 'SCA\Results_IMEC_20200619\Alone-10';     true_distance = 3.55;    scenario = 'LOS - alone - June 19';
    case 111, folder_name = 'SCA\Results_IMEC_20200619\Alone-11';     true_distance = 1.60;    scenario = 'LOS - alone - June 19'; 
    case 112, folder_name = 'SCA\Results_IMEC_20200619\Alone-12';     true_distance = 2.10;    scenario = 'LOS - alone - June 19';   
    case 113, folder_name = 'SCA\Results_IMEC_20200619\Alone-13';     true_distance = 2.50;    scenario = 'LOS - alone - June 19'; 
    case 114, folder_name = 'SCA\Results_IMEC_20200619\Alone-14';     true_distance = 3.00;    scenario = 'LOS - alone - June 19'; 
    case 115, folder_name = 'SCA\Results_IMEC_20200619\Alone-15';     true_distance = 3.50;    scenario = 'LOS - alone - June 19';   
    case 116, folder_name = 'SCA\Results_IMEC_20200619\Alone-16';     true_distance = 4.00;    scenario = 'LOS - alone - June 19'; 
    case 117, folder_name = 'SCA\Results_IMEC_20200619\Alone-17';     true_distance = 4.50;    scenario = 'LOS - alone - June 19';
% LOS cases - front (19 June 2020)
    case 121, folder_name = 'SCA\Results_IMEC_20200619\Front-1';      true_distance = 3.10;    scenario = 'LOS - front - June 19';
    case 122, folder_name = 'SCA\Results_IMEC_20200619\Front-2';      true_distance = 2.60;    scenario = 'LOS - front - June 19';
    case 123, folder_name = 'SCA\Results_IMEC_20200619\Front-3';      true_distance = 2.15;    scenario = 'LOS - front - June 19';
    case 124, folder_name = 'SCA\Results_IMEC_20200619\Front-4';      true_distance = 1.75;    scenario = 'LOS - front - June 19';        
    case 125, folder_name = 'SCA\Results_IMEC_20200619\Front-5';      true_distance = 1.60;    scenario = 'LOS - front - June 19';
    case 126, folder_name = 'SCA\Results_IMEC_20200619\Front-6';      true_distance = 1.75;    scenario = 'LOS - front - June 19';
    case 127, folder_name = 'SCA\Results_IMEC_20200619\Front-7';      true_distance = 2.15;    scenario = 'LOS - front - June 19';    
    case 128, folder_name = 'SCA\Results_IMEC_20200619\Front-8';      true_distance = 2.60;    scenario = 'LOS - front - June 19';
    case 129, folder_name = 'SCA\Results_IMEC_20200619\Front-9';      true_distance = 3.10;    scenario = 'LOS - front - June 19';
    case 130, folder_name = 'SCA\Results_IMEC_20200619\Front-10';     true_distance = 3.55;    scenario = 'LOS - front - June 19';  
    case 131, folder_name = 'SCA\Results_IMEC_20200619\Front-11';     true_distance = 1.60;    scenario = 'LOS - front - June 19';
    case 132, folder_name = 'SCA\Results_IMEC_20200619\Front-12';     true_distance = 2.10;    scenario = 'LOS - front - June 19';
    case 133, folder_name = 'SCA\Results_IMEC_20200619\Front-13';     true_distance = 2.50;    scenario = 'LOS - front - June 19';
    case 134, folder_name = 'SCA\Results_IMEC_20200619\Front-14';     true_distance = 3.00;    scenario = 'LOS - front - June 19';        
    case 135, folder_name = 'SCA\Results_IMEC_20200619\Front-15';     true_distance = 3.50;    scenario = 'LOS - front - June 19';
    case 136, folder_name = 'SCA\Results_IMEC_20200619\Front-16';     true_distance = 4.00;    scenario = 'LOS - front - June 19';
    case 137, folder_name = 'SCA\Results_IMEC_20200619\Front-17';     true_distance = 4.50;    scenario = 'LOS - front - June 19';             
% NLOS (backpocket) cases (19 June 2020)
    case 141, folder_name = 'SCA\Results_IMEC_20200619\Back-1';       true_distance = 3.10;    scenario = 'NLOS - backpocket - June 19';  
    case 142, folder_name = 'SCA\Results_IMEC_20200619\Back-2';       true_distance = 2.60;    scenario = 'NLOS - backpocket - June 19';         
    case 143, folder_name = 'SCA\Results_IMEC_20200619\Back-3';       true_distance = 2.15;    scenario = 'NLOS - backpocket - June 19'; 
    case 144, folder_name = 'SCA\Results_IMEC_20200619\Back-4';       true_distance = 1.75;    scenario = 'NLOS - backpocket - June 19';         
    case 145, folder_name = 'SCA\Results_IMEC_20200619\Back-5';       true_distance = 1.60;    scenario = 'NLOS - backpocket - June 19'; 
    case 146, folder_name = 'SCA\Results_IMEC_20200619\Back-6';       true_distance = 1.75;    scenario = 'NLOS - backpocket - June 19';         
    case 147, folder_name = 'SCA\Results_IMEC_20200619\Back-7';       true_distance = 2.15;    scenario = 'NLOS - backpocket - June 19';         
    case 148, folder_name = 'SCA\Results_IMEC_20200619\Back-8';       true_distance = 2.60;    scenario = 'NLOS - backpocket - June 19'; 
    case 149, folder_name = 'SCA\Results_IMEC_20200619\Back-9';       true_distance = 3.10;    scenario = 'NLOS - backpocket - June 19'; 
    case 150, folder_name = 'SCA\Results_IMEC_20200619\Back-10';      true_distance = 3.55;    scenario = 'NLOS - backpocket - June 19'; 
    case 151, folder_name = 'SCA\Results_IMEC_20200619\Back-11';      true_distance = 1.60;    scenario = 'NLOS - backpocket - June 19';         
    case 152, folder_name = 'SCA\Results_IMEC_20200619\Back-12';      true_distance = 2.10;    scenario = 'NLOS - backpocket - June 19'; 
    case 153, folder_name = 'SCA\Results_IMEC_20200619\Back-13';      true_distance = 2.50;    scenario = 'NLOS - backpocket - June 19';         
    case 154, folder_name = 'SCA\Results_IMEC_20200619\Back-14';      true_distance = 3.00;    scenario = 'NLOS - backpocket - June 19'; 
    case 155, folder_name = 'SCA\Results_IMEC_20200619\Back-15';      true_distance = 3.50;    scenario = 'NLOS - backpocket - June 19';         
    case 156, folder_name = 'SCA\Results_IMEC_20200619\Back-16';      true_distance = 4.00;    scenario = 'NLOS - backpocket - June 19';         
    case 157, folder_name = 'SCA\Results_IMEC_20200619\Back-17';      true_distance = 4.50;    scenario = 'NLOS - backpocket - June 19'; 
% Inside car
    case 201, folder_name = 'SCA\Results_IMEC-in_20200619\in-11';     true_distance = 2.50;    scenario = 'LOS - inside car - June 19';  
    case 202, folder_name = 'SCA\Results_IMEC-in_20200619\in-11_2';   true_distance = 1.50;    scenario = 'LOS - inside car - June 19'; 
    case 203, folder_name = 'SCA\Results_IMEC-in_20200619\in-12';     true_distance = 2.50;    scenario = 'LOS - inside car - June 19'; 
    case 204, folder_name = 'SCA\Results_IMEC-in_20200619\in-12_2';   true_distance = 1.50;    scenario = 'LOS - inside car - June 19';   
    case 205, folder_name = 'SCA\Results_IMEC-in_20200619\in-13';     true_distance = 2.50;    scenario = 'LOS - inside car - June 19';  
    case 206, folder_name = 'SCA\Results_IMEC-in_20200619\in-13_2';   true_distance = 1.50;    scenario = 'LOS - inside car - June 19'; 
    case 207, folder_name = 'SCA\Results_IMEC-in_20200619\in-14';     true_distance = 2.50;    scenario = 'LOS - inside car - June 19'; 
    case 208, folder_name = 'SCA\Results_IMEC-in_20200619\in-14_2';   true_distance = 1.50;    scenario = 'LOS - inside car - June 19';   
        
% Not supported
    otherwise,  error('Use-case index not supported; 1-->10 ; 11-->20 ; 101-->117 ; 121-->137 ; 141-->157 ; 201-->208');
end

% Read data
content = dir(fullfile(main_folder,folder_name)); 
for m = 1:length(content)
    if ~isempty(strfind(content(m).name,'.mat'))
        file_name = content(m).name; 
    end
end
load(fullfile(main_folder,folder_name,file_name));

% 1W,2W channel
cases_idx = 1:length(Result);
for N = cases_idx
    iq_in = complex(Result(N).Initiator.I,Result(N).Initiator.Q).*channel_mask;
    iq_re = complex(Result(N).Reflector.I,Result(N).Reflector.Q).*channel_mask;
    ch_2w = iq_in.*iq_re;
    if 0
        ch_1w = Result(N).Result.Hest.';
    else
        ch_1w = imec_calc_h_est(ch_2w);
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